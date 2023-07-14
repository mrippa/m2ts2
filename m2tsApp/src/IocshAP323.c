#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts323.h"

extern AP323Card m2tsAP323Card[NUM_AP323_CARDS];
extern CircularBuffer m2TestAI_CB;

/*M2ReadStatAP323*/
static const iocshArg     M2ReadStatAP323Arg0 = {"cardNumber", iocshArgInt};
static const iocshArg    *M2ReadStatAP323Args[] = {&M2ReadStatAP323Arg0};
static const iocshFuncDef M2ReadStatAP323FuncDef = {"M2ReadStatAP323", 1, M2ReadStatAP323Args};

static void M2ReadStatAP323Func(const iocshArgBuf *args)
{
    M2ReadStatusAP323(args[0].ival);
} 

static void M2ReadStatAP323Register(void)
{
    iocshRegister(&M2ReadStatAP323FuncDef, M2ReadStatAP323Func);
}

/*Print Circular Buffer*/
static const iocshFuncDef PrintBufferFuncDef = {"PrintBuffer", 0, NULL};

static void PrintBufferFunc(const iocshArgBuf *args)
{
    M2PrintBuffer();
}

static void PrintBufferRegister(void)
{
    iocshRegister(&PrintBufferFuncDef, PrintBufferFunc);
}

/*M2AcqTest             ......(TEST).........*/
static const iocshArg     M2AcqTestAP323Arg0 = {"cardNumber", iocshArgInt};
static const iocshArg     M2AcqTestAP323Arg1 = {"channel", iocshArgInt};
static const iocshArg    *M2AcqTestAP323Args[] = {&M2AcqTestAP323Arg0, &M2AcqTestAP323Arg1};
static const iocshFuncDef M2AcqTestFuncDef = {"M2AcqTest", 2, M2AcqTestAP323Args};

static void M2AcqTestFunc(const iocshArgBuf *args)
{
    M2AcqTestAndShow(args[0].ival, args[1].ival);
}

static void M2AcqTestRegister(void)
{
    iocshRegister(&M2AcqTestFuncDef, M2AcqTestFunc);
}

epicsExportRegistrar(M2ReadStatAP323Register);
epicsExportRegistrar(M2AcqTestRegister);
epicsExportRegistrar(PrintBufferRegister);
