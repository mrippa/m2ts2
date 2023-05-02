/* Example showing how to register a new command with iocsh */
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts.h"

EPICSTHREADFUNC RunLoop()
{

    double volts_input = 0.0;

    for (;;)
    {
        M2AcqAP323_runOnce();
        M2ReadAP323(&volts_input);
        write_AP236out(volts_input);
 
        //epicsThreadSleep(0.0);
    }
}

/* This is the command, which the vxWorks shell will call directly */
void initM2TS(const char *name) {
    if (name) {
        printf("initM2TS %s, from m2ts\n", name);
    } else {
        puts("initM2TS from m2ts");
        
        /* AP323*/
        if(InitAP323() ) {
            printf("Error initializing the AP323");
        }

        /* AP471*/
        if(InitAP471() ) {
            printf("Error initializing the AP471");
        }

        /* AP236*/
        if(InitAP236() ) {
            printf("Error initializing the AP236");
        }

        /* AP48x*/
        if(InitAP48x() ) {
            printf("Error initializing the AP482");
        }
    }

    RunLoopTaskId = epicsThreadCreate("RunLoop", 
                                      90, epicsThreadGetStackSize(epicsThreadStackMedium),
                                      (EPICSTHREADFUNC)RunLoop, NULL);

    //taskwdInsert(RunLoopTaskId, NULL, NULL);
}

/* Information needed by iocsh */
static const iocshArg     initM2TSArg0 = {"name", iocshArgString};
static const iocshArg    *initM2TSArgs[] = {&initM2TSArg0};
static const iocshFuncDef initM2TSFuncDef = {"initM2TS", 1, initM2TSArgs};

/* Wrapper called by iocsh, selects the argument types that initM2TS needs */
static void initM2TSCallFunc(const iocshArgBuf *args) {
    initM2TS(args[0].sval);
}

/* Registration routine, runs at startup */
static void initM2TSRegister(void) {
    iocshRegister(&initM2TSFuncDef, initM2TSCallFunc);
}

epicsExportRegistrar(initM2TSRegister);

