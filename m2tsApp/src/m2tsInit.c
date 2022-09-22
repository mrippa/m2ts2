/* Example showing how to register a new command with iocsh */
#include "m2ts.h"

#include <epicsExport.h>
#include <iocsh.h>


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
            printf("Error initializing the AP323");
        }

        /* AP236*/
        if(InitAP236() ) {
            printf("Error initializing the AP323");
        }

        /* AP48x*/
        if(InitAP48x() ) {
            printf("Error initializing the AP323");
        }
    }
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

/*M2ReadStatAP323*/
static const iocshFuncDef M2ReadStatAP323FuncDef = {"M2ReadStatAP323", 0, NULL};
static void M2ReadStatAP323Func(const iocshArgBuf *args) {
    M2ReadStatAP323();
}

static void M2ReadStatAP323Register(void) {
    iocshRegister(&M2ReadStatAP323FuncDef, M2ReadStatAP323Func);
}

/*M2AcqAP323*/
static const iocshFuncDef M2AcqAP323FuncDef = {"M2AcqAP323", 0, NULL};
static void M2AcqAP323Func(const iocshArgBuf *args) {
    M2AcqAP323();
}

static void M2AcqAP323Register(void) {
    iocshRegister(&M2AcqAP323FuncDef, M2AcqAP323Func);
}

epicsExportRegistrar(initM2TSRegister);
epicsExportRegistrar(M2ReadStatAP323Register);
epicsExportRegistrar(M2AcqAP323Register);
