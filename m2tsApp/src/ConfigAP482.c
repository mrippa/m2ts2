#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts482.h"
extern AP482Card m2tsAP482Card[NUM_AP482_CARDS];

int M2TSConfigAP482(int cardNumber)
{

    APSTATUS status = 0;
    AP482Card *p482Card = &m2tsAP482Card[cardNumber];
    int i;

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    p482Card->c_block.counter_num = 0;

    for (i = 0; i < MAX_CNTR; i++)
    {
        p482Card->c_block.m_CounterConstantA1[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantA2[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantB1[i] = 0xFFFFFFFF; /* constant registers */
        p482Card->c_block.m_CounterConstantB2[i] = 0xFFFFFFFF; /* constant registers */

        p482Card->c_block.m_Mode[i] = None;                       /* the counter mode */
        p482Card->c_block.m_OutputPolarity[i] = OutPolLow;        /* output polarity */
        p482Card->c_block.m_InputAPolarity[i] = InABCPolDisabled; /* input A polarity */
        p482Card->c_block.m_InputBPolarity[i] = InABCPolDisabled; /* input B polarity */
        p482Card->c_block.m_InputCPolarity[i] = InABCPolDisabled; /* input C polarity */
        p482Card->c_block.m_ClockSource[i] = InC1_1953125Mhz;     /* clock source */
        p482Card->c_block.m_SpecialIC[i] = Nosis;                 /* No Output or Special Interrupt Selected */
        p482Card->c_block.m_InterruptEnable[i] = IntDisable;      /* interrupt enable */
        p482Card->c_block.m_Debounce[i] = DebounceOff;            /* Debounce disabled */
    }

    printf("Config AP482 done! 0x%x\n", status);

    return status;
}

int AP482ShowClocks(int cardNumber)
{
    int counter; /* menu item variable */
    uint32_t value;
    AP482Card *p482Card = &m2tsAP482Card[cardNumber];

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return -1;
    }

    /*TODO: select counter from input*/
    p482Card->c_block.counter_num = (BYTE)(0);

    counter = p482Card->c_block.counter_num;
    printf("\n\nCurrent Counter:             %d\n", counter);
    GetCounterConstantAx(&(p482Card->c_block), counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantA1 Register: %X\n", value);
    GetCounterConstantBx(&(p482Card->c_block), counter, &value, 1); /* int x; 1 or 2 */
    printf("Counter ConstantB1 Register: %X\n\n", value);

    GetCounterConstantAx(&(p482Card->c_block), counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantA2 Register: %X\n", value);
    GetCounterConstantBx(&(p482Card->c_block), counter, &value, 2); /* int x; 1 or 2 */
    printf("Counter ConstantB2 Register: %X\n\n", value);
    
    return 0;
}

int AP482SetClock( int cardNumber, int frequency, int counter) {

    AP482Card *p482Card = &m2tsAP482Card[cardNumber];
    uint32_t value;

    if (!p482Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    p482Card->c_block.counter_num = counter;

    /* Limit check counter (Allow 0 through 8)*/
    if (counter >8 || counter < 0) {

        printf("Error: Counter val >>%d<< out of range", counter);
    }

    if (frequency != 40 || frequency != 0) {
           printf("Error: AP482 Set Clock: freq: %d\n", frequency);
        return (ERROR);
    }
    value = 0xBEBC; /*40 Hz Clock*/
    SetCounterConstantAx(&(p482Card->c_block), counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantBx(&(p482Card->c_block), counter, value, 1);	/* int x; 1 or 2 */
    SetCounterConstantAx(&(p482Card->c_block), counter, 0x0, 2);	/* int x; 1 or 2 */
    SetCounterConstantBx(&(p482Card->c_block), counter, 0x0, 2);	/* int x; 1 or 2 */

    printf("Set AP482 Card %d, counter %d to clock freq value of: 0x%x ", cardNumber, counter, value );
}

/*AP482ShowClocks*/
static const iocshArg     AP482ShowClocksArg0 = {"cardNumber", iocshArgInt};
static const iocshArg    *AP482ShowClocksArgs[] = {&AP482ShowClocksArg0};
static const iocshFuncDef AP482ShowClocksFuncDef = {"AP482ShowClocks", 1, AP482ShowClocksArgs};

static void AP482ShowClocksFunc(const iocshArgBuf *args) {
    AP482ShowClocks(args[0].ival);
}


/*AP482SetClocks*/
/* Information needed by iocsh */
static const iocshArg     AP482SetClocksArg0 = {"cardNumber", iocshArgInt};
static const iocshArg     AP482SetClocksArg1 = {"freqhz", iocshArgInt};
static const iocshArg     AP482SetClocksArg2 = {"counter", iocshArgInt};
static const iocshArg    *AP482SetClocksArgs[] = {&AP482SetClocksArg0, &AP482SetClocksArg1, &AP482SetClocksArg2};

static const iocshFuncDef AP482SetClocksFuncDef = {"AP482SetClocks", 3, AP482SetClocksArgs};

static void AP482SetClocksFunc(const iocshArgBuf *args) {
    AP482SetClock(args[0].ival, args[1].ival, args[2].ival);
}

static void AP482UtilityRegister(void) {
    iocshRegister(&AP482SetClocksFuncDef, AP482SetClocksFunc);
    iocshRegister(&AP482ShowClocksFuncDef, AP482ShowClocksFunc);
}

/* Exports */
epicsExportRegistrar(AP482UtilityRegister);
