#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts482.h"
#include "m2ts482Utils.h"

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
