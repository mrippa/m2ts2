#include <stdlib.h>
#include <epicsExport.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>
#include <alarm.h>
#include <dbScan.h>
#include <dbDefs.h>
#include <ellLib.h>
#include <cantProceed.h>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <initHooks.h>

#include <aiRecord.h>

static ELLLIST allprngs = ELLLIST_INIT;

struct prngState
{
    ELLNODE node;
    unsigned int seed;
    unsigned int lastnum;
    epicsMutexId lock;
    IOSCANPVT scan;
    epicsThreadId generator;
};

static void start_workers(initHookState state);
static void worker(void *raw);

static long init(int phase)
{
    if (phase == 0)
        initHookRegister(&start_workers);
    return 0;
}

static long init_record(aiRecord *prec)
{
    struct prngState *priv;
    unsigned long start;

    priv = callocMustSucceed(1, sizeof(*priv), "prngintr");

    recGblInitConstantLink(&prec->inp, DBF_ULONG, &start);

    priv->seed = start;
    scanIoInit(&priv->scan);
    priv->lock = epicsMutexMustCreate();
    priv->generator = NULL;
    ellAdd(&allprngs, &priv->node);
    prec->dpvt = priv;

    return 0;
}

static void start_workers(initHookState state)
{
    ELLNODE *cur;
    if (state != initHookAfterInterruptAccept)
        return;
    for (cur = ellFirst(&allprngs); cur; cur = ellNext(cur))
    {
        struct prngState *priv = CONTAINER(cur, struct prngState, node);
        priv->generator = epicsThreadMustCreate("prngworker",
                                                epicsThreadPriorityMedium,
                                                epicsThreadGetStackSize(epicsThreadStackSmall),
                                                &worker, priv);
    }
}

static void worker(void *raw)
{
    struct prngState *priv = raw;
    while (1)
    {
        epicsMutexMustLock(priv->lock);
        priv->lastnum = rand_r(&priv->seed);
        epicsMutexUnlock(priv->lock);

        scanIoImmediate(priv->scan, priorityHigh);
        scanIoImmediate(priv->scan, priorityMedium);
        scanIoImmediate(priv->scan, priorityLow);

         epicsThreadSleep(1.0);
        //scanIoSetComplete(priv->scan);
    }
}

static long get_ioint_info(int dir, dbCommon *prec, IOSCANPVT *io)
{
    struct prngState *priv = prec->dpvt;

    if (priv)
    {
        *io = priv->scan;
    }
    return 0;
}

static long read_ai(aiRecord *prec)
{
    struct prngState *priv = prec->dpvt;
    if (!priv)
    {
        (void)recGblSetSevr(prec, COMM_ALARM, INVALID_ALARM);
        return 0;
    }

    epicsMutexMustLock(priv->lock);
    prec->rval = priv->lastnum;
    epicsMutexUnlock(priv->lock);

    return 0;
}

struct
{
    long num;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
    DEVSUPFUN special_linconv;
} devAi323Intr = {
    6, /* space for 6 functions */
    NULL,
    init,
    init_record,
    get_ioint_info,
    read_ai,
    NULL};
epicsExportAddress(dset, devAi323Intr);