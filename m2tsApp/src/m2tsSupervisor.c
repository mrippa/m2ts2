/* Example showing how to register a new command with iocsh */
#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts.h"

/* This is the command, which the EPICS shell will call directly */

void initM2TSDAQ()
{

        /* AP323 Card 0*/
        if (M2TSInitAP323(0))
        {
            errlogPrintf("Error initializing the AP323\n");
        }

        /* AP323 Card 1*/
        if (M2TSInitAP323(1))
        {
            errlogPrintf("Error initializing the AP323\n");
        }

        /* AP471
        if (M2TSInitAP471())
        {
            errlogPrintf("Error initializing the AP471");
        }
*/
        /* AP236
        if (M2TSInitAP236())
        {
            errlogPrintf("Error initializing the AP236");
        }
*/
        /* AP48x
        if (M2TSInitAP48x())
        {
            errlogPrintf("Error initializing the AP482");
        }
*/
}

int ConfigM2TSDAQ()
{

    int status = 0;

    /*Configure AP323 Card 0 */
    if (M2TSConfigAP323(0) )
    {
        errlogPrintf("Error configuring the AP323\n");
        status = 1;
    }

    /* Configure AP323 Card 1 */
    if (M2TSConfigAP323(1) )
    {
        errlogPrintf("Error configuring the AP323\n");
        status = 1;
    }

    //ConfigAP471();
    //ConfigAP236();
    //ConfigAP48x();

    return status;
}

int StartM2TSAppThreads()
{

    int status = 0;
 
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

    return status;
}

/* Main Startup 
 *
 * This is the main startup routine for the EPICS shell.
*/
void M2TSStartup()
{

    initM2TSDAQ();
    ConfigM2TSDAQ();
    StartM2TSAppThreads();
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

