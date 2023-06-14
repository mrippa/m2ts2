#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts471.h"

int M2TSConfigAP471(int cardNumber)
{

    APSTATUS status = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (! p471Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }    

    printf("Config AP471 done! 0x%x\n", status);

    return status;
}

void M2TS_ShowAP471States(int cardNumber)
{

    APSTATUS status = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];
    uint16_t i = 0;

    if (! p471Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return;
    }    


    rsts471(p471Card->cblock ); /* get board's status info into structure */
    printf("\n\nBoard Status Information\n");
    printf("\nLocation Register:   %X", p471Card->location);
    printf("\nFirmware Revision:    %c", (char)p471Card->revision);
    printf("\nDebounce Clock:    0x%02X", p471Card->sblk_ptr->DebounceClockSelectStat);
    printf("\nDebounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X", p471Card->sblk_ptr->DebounceDurationStat[0],
           p471Card->sblk_ptr->DebounceDurationStat[1], p471Card->sblk_ptr->DebounceDurationStat[2]);
    printf("\nDebounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->DebounceControlStat[0],
           p471Card->sblk_ptr->DebounceControlStat[1], p471Card->sblk_ptr->DebounceControlStat[2]);
    printf("\nWrite Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->MaskRegStat[0],
           p471Card->sblk_ptr->MaskRegStat[1], p471Card->sblk_ptr->MaskRegStat[2]);
    printf("\nEvent Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->EventTypeStat[0],
           p471Card->sblk_ptr->EventTypeStat[1], p471Card->sblk_ptr->EventTypeStat[2]);
    printf("\nEvent Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->EventPolarityStat[0],
           p471Card->sblk_ptr->EventPolarityStat[1], p471Card->sblk_ptr->EventPolarityStat[2]);
    printf("\nEvent Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->EventEnableStat[0],
           p471Card->sblk_ptr->EventEnableStat[1], p471Card->sblk_ptr->EventEnableStat[2]);
    printf("\nEvent Pending:     R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->sblk_ptr->EventPendingClrStat[0],
           p471Card->sblk_ptr->EventPendingClrStat[1], p471Card->sblk_ptr->EventPendingClrStat[2]);
    printf("\nBoard Int Enable:  0x%02X\n", p471Card->sblk_ptr->BoardIntEnableStat);

    /*Show TTL Levels */
    for (i=0; i<3; i++) {
       status = (long)rprt471(p471Card->cblock, (uint16_t)i);
       printf("Value of port %d: %lX\n", i, status);
    }
}

void setAP471Word(int cardNumber, uint16_t port, uint16_t word)
{
    APSTATUS status = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (p471Card->bInitialized != TRUE)
    {
       printf("Error cblk471 unintialized\n");
       return;
    }

    wprt471(p471Card, (uint16_t)port, (uint16_t)word);
}

void setAP471Point(int cardNumber, uint16_t port, uint16_t point, uint16_t val)
{

    APSTATUS status = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (p471Card->bInitialized != TRUE)
    {
       printf("Error cblk471 unintialized\n");
       return;
    }

    wpnt471(&(p471Card->c_block), (uint16_t)port, (uint16_t)point,(uint16_t)val);
    printf("hello world\n");
}

/*showAP471States*/
static const iocshArg     showAP471StatesArg0 = {"cardNumber", iocshArgInt};
static const iocshArg    *showAP471StatesArgs[] = {&showAP471StatesArg0};
static const iocshFuncDef showAP471StatesFuncDef = {"showAP471States", 1, showAP471StatesArgs};

static void showAP471StatesFunc(const iocshArgBuf *args) {
    M2TS_ShowAP471States(args[0].ival);
}

/*setAP471Word*/
/* Information needed by iocsh */
static const iocshArg     setAP471WordArg0 = {"cardNumber", iocshArgInt};
static const iocshArg     setAP471WordArg1 = {"port", iocshArgInt};
static const iocshArg     setAP471WordArg2 = {"value", iocshArgInt};
static const iocshArg    *setAP471WordArgs[] = {&setAP471WordArg0, &setAP471WordArg1, &setAP471WordArg2};

static const iocshFuncDef setAP471WordFuncDef = {"setAP471Word", 3, setAP471WordArgs};

static void setAP471WordFunc(const iocshArgBuf *args) {
    setAP471Word(args[0].ival, args[1].ival, args[2].ival );
}

static void setAP471WordFunc(const iocshArgBuf *args) {
    iocshRegister(&setAP471WordFuncDef, setAP471WordFunc);
}

/*setAP471Point*/
/* Information needed by iocsh */
static const iocshArg     setAP471WordArg0 = {"cardNumber", iocshArgInt};
static const iocshArg     setAP471PointArg0 = {"port", iocshArgInt};
static const iocshArg     setAP471PointArg1 = {"point", iocshArgInt};
static const iocshArg     setAP471PointArg2 = {"value", iocshArgInt};
static const iocshArg    *setAP471PointArgs[] = {&setAP471PointArg0, &setAP471PointArg1, &setAP471PointArg2, &setAP471PointArg3};

static const iocshFuncDef setAP471PointFuncDef = {"setAP471Point", 4, setAP471PointArgs};

static void setAP471PointFunc(const iocshArgBuf *args) {
    setAP471Point(args[0].ival, args[1].ival, args[2].ival, args[4].ival);
}

static void AP471UtilityRegister(void) {
    iocshRegister(&setAP471PointFuncDef, setAP471PointFunc);
    iocshRegister(&setAP471WordFuncDef, setAP471WordFunc);
    iocshRegister(&showAP471StatesFuncDef, showAP471StatesFunc);
}

/* Exports */
epicsExportRegistrar(AP471UtilityRegister);
