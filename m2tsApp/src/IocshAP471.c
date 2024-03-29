#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts471.h"
#include "m2ts471Utils.h"

extern AP471Card m2tsAP471Card[NUM_AP471_CARDS];

void M2TS_ShowAP471States(int cardNumber)
{

    uint16_t portvalue = 0;
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];
    uint16_t i = 0;

    if (! p471Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return;
    }

    printf("471Show...\n");    

    rsts471(&(p471Card->c_block) ); /* get board's status info into structure */
    printf("\n\nBoard Status Information\n");
    printf("\nLocation Register:   %X", p471Card->c_block.location);
    printf("\nFirmware Revision:    %c", (char)p471Card->c_block.revision);
    printf("\nDebounce Clock:    0x%02X", p471Card->s_block.DebounceClockSelectStat);
    printf("\nDebounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X", p471Card->s_block.DebounceDurationStat[0],
           p471Card->s_block.DebounceDurationStat[1], p471Card->s_block.DebounceDurationStat[2]);
    printf("\nDebounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.DebounceControlStat[0],
           p471Card->s_block.DebounceControlStat[1], p471Card->s_block.DebounceControlStat[2]);
    printf("\nWrite Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.MaskRegStat[0],
           p471Card->s_block.MaskRegStat[1], p471Card->s_block.MaskRegStat[2]);
    printf("\nEvent Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.EventTypeStat[0],
           p471Card->s_block.EventTypeStat[1], p471Card->s_block.EventTypeStat[2]);
    printf("\nEvent Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.EventPolarityStat[0],
           p471Card->s_block.EventPolarityStat[1], p471Card->s_block.EventPolarityStat[2]);
    printf("\nEvent Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.EventEnableStat[0],
           p471Card->s_block.EventEnableStat[1], p471Card->s_block.EventEnableStat[2]);
    printf("\nEvent Pending:     R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X", p471Card->s_block.EventPendingClrStat[0],
           p471Card->s_block.EventPendingClrStat[1], p471Card->s_block.EventPendingClrStat[2]);
    printf("\nBoard Int Enable:  0x%02X\n", p471Card->s_block.BoardIntEnableStat);

    /*Show TTL Levels */
    for (i=0; i<3; i++) {
       portvalue = (long)rprt471(&p471Card->c_block, (uint16_t)i);
       //printf("Value of port %d: %d\n", i, portvalue);
       m2ts471PrintBits(portvalue);
    }
}

void setAP471Word(int cardNumber, uint16_t port, uint16_t word)
{
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (p471Card->c_block.bInitialized != TRUE)
    {
       printf("Error cblk471 unintialized\n");
       return;
    }

    wprt471(&(p471Card->c_block), (uint16_t)port, (uint16_t)word);
}

void setAP471Point(int cardNumber, uint16_t port, uint16_t point, uint16_t val)
{
    AP471Card *p471Card = &m2tsAP471Card[cardNumber];

    if (p471Card->c_block.bInitialized != TRUE)
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

/*setAP471Point*/
/* Information needed by iocsh */
static const iocshArg     setAP471PointArg0 = {"cardNumber", iocshArgInt};
static const iocshArg     setAP471PointArg1 = {"port", iocshArgInt};
static const iocshArg     setAP471PointArg2 = {"point", iocshArgInt};
static const iocshArg     setAP471PointArg3 = {"value", iocshArgInt};
static const iocshArg    *setAP471PointArgs[] = {&setAP471PointArg0, &setAP471PointArg1, &setAP471PointArg2, &setAP471PointArg3};

static const iocshFuncDef setAP471PointFuncDef = {"setAP471Point", 4, setAP471PointArgs};

static void setAP471PointFunc(const iocshArgBuf *args) {
    setAP471Point(args[0].ival, args[1].ival, args[2].ival, args[3].ival);
}

static void AP471UtilityRegister(void) {
    iocshRegister(&setAP471PointFuncDef, setAP471PointFunc);
    iocshRegister(&setAP471WordFuncDef, setAP471WordFunc);
    iocshRegister(&showAP471StatesFuncDef, showAP471StatesFunc);
}

/* Exports */
epicsExportRegistrar(AP471UtilityRegister);