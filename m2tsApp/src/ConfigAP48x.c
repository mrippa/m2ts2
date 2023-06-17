#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts48x.h"

int ConfigAP482(void)
{

    APSTATUS status = 0;
    AP482Card *p482Card = &m2tsAP482Card[cardNumber];

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    printf("Config AP482 done! 0x%x\n", status);

    for (i = 0; i < MAX_CNTR; i++)
    {
        c_block48x.m_CounterConstantA1[i] = 0xFFFFFFFF; /* constant registers */
        c_block48x.m_CounterConstantA2[i] = 0xFFFFFFFF; /* constant registers */
        c_block48x.m_CounterConstantB1[i] = 0xFFFFFFFF; /* constant registers */
        c_block48x.m_CounterConstantB2[i] = 0xFFFFFFFF; /* constant registers */

        c_block48x.m_Mode[i] = None;                       /* the counter mode */
        c_block48x.m_OutputPolarity[i] = OutPolLow;        /* output polarity */
        c_block48x.m_InputAPolarity[i] = InABCPolDisabled; /* input A polarity */
        c_block48x.m_InputBPolarity[i] = InABCPolDisabled; /* input B polarity */
        c_block48x.m_InputCPolarity[i] = InABCPolDisabled; /* input C polarity */
        c_block48x.m_ClockSource[i] = InC1_1953125Mhz;     /* clock source */
        c_block48x.m_SpecialIC[i] = Nosis;                 /* No Output or Special Interrupt Selected */
        c_block48x.m_InterruptEnable[i] = IntDisable;      /* interrupt enable */
        c_block48x.m_Debounce[i] = DebounceOff;            /* Debounce disabled */
    }
    c_block48x.counter_num = 0;

    return status;
}

void showAP482Clocks()
{
    int counter; /* menu item variable */
    uint32_t value;

    struct ap48x *c_blk;
    c_blk = &c_block48x; /* Get access to the global c_block471 structure*/

    if (c_blk->bInitialized != TRUE)
    {
        printf("Error cblk482 unintialized\n");
        return;
    }

    c_blk->counter_num = (BYTE)(0);

    counter = c_blk->counter_num;
    printf("\n\nCurrent Counter:             %d\n", counter);
    GetCounterConstantAx(c_blk, counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantA1 Register: %X\n", value);
    GetCounterConstantBx(c_blk, counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantB1 Register: %X\n\n", value);

    GetCounterConstantAx(c_blk, counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantA2 Register: %X\n", value);
    GetCounterConstantBx(c_blk, counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantB2 Register: %X\n\n", value);
    
}

void AP482SetClock( int frequency, int counter) {
    struct ap48x *c_blk;
    c_blk = &c_block48x; /* Get access to the global c_block471 structure*/
    uint32_t value;

    if (c_blk->bInitialized != TRUE)
    {
        printf("Error cblk482 unintialized\n");
        return;
    }

    /* Limit check counter (Allow 0 through 8)*/
    if (counter >8 || counter < 0) {

        printf("Error: Counter val >>%d<< out of range", counter);
    }

    if (frequency != 40 || frequency != 0) return;

    value = 0xBEBC; /*40 Hz Clock*/
    SetCounterConstantAx(c_blk, counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantBx(c_blk, counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantAx(c_blk, counter, 0x0, 2);	/* int x; 1 or 2 */
    SetCounterConstantBx(c_blk, counter, 0x0, 2);	/* int x; 1 or 2 */

}

/*showAP482Clocks*/
static const iocshFuncDef showAP482ClocksFuncDef = {"showAP482Clocks", 0, NULL};

static void showAP482ClocksFunc(const iocshArgBuf *args) {
    showAP482Clocks();
}

static void showAP482ClocksRegister(void) {
    iocshRegister(&showAP482ClocksFuncDef, showAP482ClocksFunc);
}

/*setAP482Clock*/
/* Information needed by iocsh */
static const iocshArg     setAP482ClockArg0 = {"freqhz", iocshArgInt};
static const iocshArg     setAP482ClockArg1 = {"counter", iocshArgInt};
static const iocshArg    *setAP482ClockArgs[] = {&setAP482ClockArg0, &setAP482ClockArg1};

static const iocshFuncDef setAP482ClockFuncDef = {"setAP482Clock", 2, setAP482ClockArgs};

static void setAP482ClockFunc(const iocshArgBuf *args) {
    AP482SetClock(args[0].ival, args[1].ival);
}

static void setAP482ClockRegister(void) {
    iocshRegister(&setAP482ClockFuncDef, setAP482ClockFunc);
}


/*setAP482Point*/
/* Information needed by iocsh */
static const iocshArg     setAP482PointArg0 = {"port", iocshArgInt};
static const iocshArg     setAP482PointArg1 = {"point", iocshArgInt};
static const iocshArg     setAP482PointArg2 = {"value", iocshArgInt};
static const iocshArg    *setAP482PointArgs[] = {&setAP482PointArg0, &setAP482PointArg1, &setAP482PointArg2};

static const iocshFuncDef setAP482PointFuncDef = {"setAP482Point", 3, setAP482PointArgs};

static void setAP482PointFunc(const iocshArgBuf *args) {
    //setAP482Point(args[0].ival, args[1].ival, args[2].ival);
}

static void setAP482PointRegister(void) {
    iocshRegister(&setAP482PointFuncDef, setAP482PointFunc);
}


/* Exports */
epicsExportRegistrar(showAP482ClocksRegister);
epicsExportRegistrar(setAP482ClockRegister);
epicsExportRegistrar(setAP482PointRegister);
