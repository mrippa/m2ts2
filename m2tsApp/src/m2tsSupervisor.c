/* Example showing how to register a new command with iocsh */
#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts.h"
#include "m2ts323.h"
#include "m2MirrorControl.h"

/* This is the command, which the EPICS shell will call directly */

int initM2TSDAQ()
{

    int status = 0;

        /* AP323 Card 0*/
        if (M2TSInitAP323(0))
        {
            errlogPrintf("Error initializing the AP323\n");
            return -1;
        }

        /* AP323 Card 1*/
        if (M2TSInitAP323(1))
        {
            errlogPrintf("Error initializing the AP323\n");
            return -1;
        }

        /* AP471 */
        if (M2TSInitAP471(0))
        {
            errlogPrintf("Error initializing the AP471");
            return -1;
        }

        /* AP236 */
        if (M2TSInitAP236(0))
        {
            errlogPrintf("Error initializing the AP236");
            return-1;
        }

        /* AP48x */
        if (M2TSInitAP482(0))
        {
            errlogPrintf("Error initializing the AP482");
            return -1;
        }

        return status;
}

int ConfigM2TSDAQ()
{

    int status = 0;

    /*Configure AP323 Card 0 */
    if (M2TSConfigAP323(0) )
    {
        errlogPrintf("Error configuring the AP323\n");
        status = -1;
    }

    /* Configure AP323 Card 1 */
    if (M2TSConfigAP323(1) )
    {
        errlogPrintf("Error configuring the AP323\n");
        status = -1;
    }

    if (M2TSConfigAP471(0)) {
        errlogPrintf("Error configuring the AP471\n");
        status = -1;
    }

    if (M2TSConfigAP482(0)) {
        errlogPrintf("Error configuring the AP482\n");
        status = -1;
    }

    if (M2TSConfigAP236(0)) {
        errlogPrintf("Error configuring the AP236\n");
        status = -1;
    }

    return status;
}

int StartM2TSAppThreads()
{

    int status = 0;
 
    /* 1. Mirror Control Task*/
    start323MainLoop(1);
    startMCLoopT1();

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

    return status;
}

/* Main Startup 
 *
 * This is the main startup routine for the EPICS shell.
*/
void M2TSStartup()
{

    if ( initM2TSDAQ() ) return; 
    if ( ConfigM2TSDAQ() ) return;
    if ( StartM2TSAppThreads() ) return;
}


/* Information needed by iocsh */
static const iocshFuncDef initM2TSFuncDef = {"M2TSStartup", 0, NULL};

/* Wrapper called by iocsh, selects the argument types that initM2TS needs */
static void initM2TSCallFunc(const iocshArgBuf *args) {
    M2TSStartup();
}

/* Registration routine, runs at startup */
static void initM2TSRegister(void) {
    iocshRegister(&initM2TSFuncDef, initM2TSCallFunc);
}

epicsExportRegistrar(initM2TSRegister);

