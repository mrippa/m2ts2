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
epicsExportRegistrar(initM2TSRegister);