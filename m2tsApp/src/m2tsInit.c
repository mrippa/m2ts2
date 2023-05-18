/* Example showing how to register a new command with iocsh */
#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts.h"

/* This is the command, which the EPICS shell will call directly */

void initM2TS(const char *name)
{
    if (name)
    {
        errlogPrintf("initM2TS %s, from m2ts\n", name);
    }
    else
    {
        puts("initM2TS from m2ts");

        /* AP323*/
        if (M2TSInitAP323(1))
        {
            errlogPrintf("Error initializing the AP323");
        }

        /* AP471
        if (InitAP471())
        {
            errlogPrintf("Error initializing the AP471");
        }
*/
        /* AP236
        if (InitAP236())
        {
            errlogPrintf("Error initializing the AP236");
        }
*/
        /* AP48x
        if (InitAP48x())
        {
            errlogPrintf("Error initializing the AP482");
        }
*/
    }

    /* 1. Mirror Control Task*/

    /* 2. Vibration Control Task*/

    /* 3. Communication (SynchroBus) Control Task*/

    /* 3.1 AutoGuider Control Task*/

    /* 3.2 Command Control Task*/

    /* 4. Supervisor Control Task Group*/

    /* 4.1 Safety Shutdown Task */

    /* 4.2 Exception Management Task */

    /* 4.3 Configuration Manager Task */

    /* 4.3.1 Prime Filter Configurator Task */

    /* 4.3.2 Trajectory Configurator Task */

    /* 5. XY Positioner Control Task */

    /* 6. Deployable Baffle Control Task */

    /* 7. Periscope Baffle Control Task */

    /* 8. Status Management Task */
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

