#include <epicsExport.h>
#include <iocsh.h>

#include "m2MirrorControl.h"

/*Print Circular Buffer*/
static const iocshFuncDef PrintBufferFuncDef = {"PrintBuffer", 0, NULL};

static void PrintBufferFunc(const iocshArgBuf *args)
{
    MCPrintBufferT1();
}

static void MCUtilsRegister(void)
{
    iocshRegister(&PrintBufferFuncDef, PrintBufferFunc);
}

epicsExportRegistrar(MCUtilsRegister);
