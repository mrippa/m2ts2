#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts48x.h"

int ConfigAP482(void)
{

    APSTATUS status = 0;

    printf("Config AP482 done! 0x%x\n", status);

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

void AP482SetClock_0( int frequency) {
    struct ap48x *c_blk;
    c_blk = &c_block48x; /* Get access to the global c_block471 structure*/
    uint32_t value;
    int counter; /* menu item variable */

    if (c_blk->bInitialized != TRUE)
    {
        printf("Error cblk482 unintialized\n");
        return;
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
//static const iocshArg     setAP482ClockArg1 = {"value", iocshArgInt};
static const iocshArg    *setAP482ClockArgs[] = {&setAP482ClockArg0};

static const iocshFuncDef setAP482ClockFuncDef = {"setAP482Clock", 1, setAP482ClockArgs};

static void setAP482ClockFunc(const iocshArgBuf *args) {
    setAP482Clock(args[0].ival);
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
