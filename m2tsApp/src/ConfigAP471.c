#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts471.h"

int ConfigAP471(void)
{

    APSTATUS status = 0;

    printf("Config AP471 done! 0x%x\n", status);

    return status;
}

void showAP471States()
{

    uint16_t i = 0;
    struct cblk471 *c_blk;
    c_blk = &c_block471;  /* Get access to the global c_block471 structure*/

    if (c_blk->bInitialized != TRUE) {
        printf("Error cblk471 unintialized\n");
        return;   
    }

    rsts471(c_blk); /* get board's status info into structure */
    printf("\n\nBoard Status Information\n");
    printf("\nLocation Register:   %X", c_blk->location);
    printf("\nFirmware Revision:    %c", (char)c_blk->revision);
    printf("\nDebounce Clock:    0x%02X", c_blk->sblk_ptr->DebounceClockSelectStat);
    printf("\nDebounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X", c_blk->sblk_ptr->DebounceDurationStat[0],
           c_blk->sblk_ptr->DebounceDurationStat[1], c_blk->sblk_ptr->DebounceDurationStat[2]);
    printf("\nDebounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->DebounceControlStat[0],
           c_blk->sblk_ptr->DebounceControlStat[1], c_blk->sblk_ptr->DebounceControlStat[2]);
    printf("\nWrite Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->MaskRegStat[0],
           c_blk->sblk_ptr->MaskRegStat[1], c_blk->sblk_ptr->MaskRegStat[2]);
    printf("\nEvent Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->EventTypeStat[0],
           c_blk->sblk_ptr->EventTypeStat[1], c_blk->sblk_ptr->EventTypeStat[2]);
    printf("\nEvent Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->EventPolarityStat[0],
           c_blk->sblk_ptr->EventPolarityStat[1], c_blk->sblk_ptr->EventPolarityStat[2]);
    printf("\nEvent Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->EventEnableStat[0],
           c_blk->sblk_ptr->EventEnableStat[1], c_blk->sblk_ptr->EventEnableStat[2]);
    printf("\nEvent Pending:     R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", c_blk->sblk_ptr->EventPendingClrStat[0],
           c_blk->sblk_ptr->EventPendingClrStat[1], c_blk->sblk_ptr->EventPendingClrStat[2]);
    printf("\nBoard Int Enable:  0x%02X\n", c_blk->sblk_ptr->BoardIntEnableStat);

    /*Show TTL Levels */
    for (i=0; i<3; i++) {
       status = (long)rprt471(c_blk, (uint16_t)i);
       printf("Value of port %d: %lX\n", i, status);
    }
}

void setAP471Word(uint16_t port, uint16_t word)
{
    struct cblk471 *c_blk;
    c_blk = &c_block471; /* Get access to the global c_block471 structure*/

    if (c_blk->bInitialized != TRUE)
    {
       printf("Error cblk471 unintialized\n");
       return;
    }

    wprt471(c_blk, (uint16_t)port, (uint16_t)word);
}

void setAP471Point(uint16_t port, uint16_t point, uint16_t val)
{
    struct cblk471 *c_blk;
    c_blk = &c_block471; /* Get access to the global c_block471 structure*/

    if (c_blk->bInitialized != TRUE)
    {
       printf("Error cblk471 unintialized\n");
       return;
    }

    wpnt471(&c_block471,(uint16_t)port,(uint16_t)point,(uint16_t)val);
}

/*showAP471States*/
static const iocshFuncDef showAP471StatesFuncDef = {"showAP471States", 0, NULL};

static void showAP471StatesFunc(const iocshArgBuf *args) {
    showAP471States();
}

static void showAP471StatesRegister(void) {
    iocshRegister(&showAP471StatesFuncDef, showAP471StatesFunc);
}

/*setAP471Word*/
/* Information needed by iocsh */
static const iocshArg     setAP471WordArg0 = {"port", iocshArgInt};
static const iocshArg     setAP471WordArg1 = {"value", iocshArgInt};
static const iocshArg    *setAP471WordArgs[] = {&setAP471WordArg0, &setAP471WordArg1};

static const iocshFuncDef setAP471WordFuncDef = {"setAP471Word", 2, setAP471WordArgs};

static void setAP471WordFunc(const iocshArgBuf *args) {
    setAP471Word(args[0].ival, args[1].ival);
}

static void setAP471WordRegister(void) {
    iocshRegister(&setAP471WordFuncDef, setAP471WordFunc);
}


/*setAP471Point*/
/* Information needed by iocsh */
static const iocshArg     setAP471PointArg0 = {"port", iocshArgInt};
static const iocshArg     setAP471PointArg1 = {"point", iocshArgInt};
static const iocshArg     setAP471PointArg2 = {"value", iocshArgInt};
static const iocshArg    *setAP471PointArgs[] = {&setAP471PointArg0, &setAP471PointArg1, &setAP471PointArg2};

static const iocshFuncDef setAP471PointFuncDef = {"setAP471Point", 3, setAP471PointArgs};

static void setAP471PointFunc(const iocshArgBuf *args) {
    setAP471Point(args[0].ival, args[1].ival, args[2].ival);
}

static void setAP471PointRegister(void) {
    iocshRegister(&setAP471PointFuncDef, setAP471PointFunc);
}


/* Exports */
epicsExportRegistrar(showAP471StatesRegister);
epicsExportRegistrar(setAP471WordRegister);
epicsExportRegistrar(setAP471PointRegister);
