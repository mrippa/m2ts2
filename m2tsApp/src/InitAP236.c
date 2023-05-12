#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"

/* Ideal Zero SB, BTC, Slope, endpoint, and clip constants
   ranges[8], ideal straight binary is [0], ideal 2'Comp is [1], slope is [2],
   endpoint low is [3], endpoint high is [4], clip low is [5] clip high is [6]
*/

double IdealCode[8][7] =
    {
        /* IdealZeroSB, IdealZeroBTC, IdealSlope, -10 to 10V, cliplo, cliphi */
        {32768.0, 0.0, 3276.8, -10.0, 10.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope,   0 to 10V, cliplo, cliphi */
        {0.0, -32768.0, 6553.6, 0.0, 10.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope,  -5 to  5V, cliplo, cliphi */
        {32768.0, 0.0, 6553.6, -5.0, 5.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope,   0 to  5V, cliplo, cliphi */
        {0.0, -32768.0, 13107.2, 0.0, 5.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope, -10 to 10V, cliplo, cliphi */
        {32768.0, 0.0, 3276.8, -10.0, 10.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope,  -3 to  3V, cliplo, cliphi */
        {32768.0, 0.0, 10922.67, -3.0, 3.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope, 0V to +16V, cliplo, cliphi */
        {0.0, -32768.0, 4095.9, 0.0, 16.0, -32768.0, 32767.0},

        /* IdealZeroSB, IdealZeroBTC, IdealSlope, 0V to +20V, cliplo, cliphi */
        {0.0, -32768.0, 3276.8, 0.0, 20.0, -32768.0, 32767.0},
};


int InitAP236(void)
{

    APSTATUS status = 0;
    range = 0;
    ap_instance236 = 0;

    memset(&c_block236, 0, sizeof(c_block236));

    /* Put the address of the initialized constant array into the configuration block structure */
    c_block236.pIdealCode = &IdealCode; /* pointer to Ideal Zero straight binary, and Slope constants */

    /*
        Initialize the Configuration Parameter Block to default values.
    */

    c_block236.bAP = FALSE;          /* indicate not initialized and set up yet */
    c_block236.bInitialized = FALSE; /* indicate not ready */
    c_block236.nHandle = 0;          /* make handle to a closed board */

    /*
        Initialize the AP library
    */
    if (InitAPLib() != S_OK)
    {
        printf("\nUnable to initialize the AP library. Exiting program.\n");
        exit(0);
    }

    /*
        Open an instance of a AP device
        Other device instances can be obtained
        by changing parameter 1 of APOpen()
    */
    status = APOpen(ap_instance236, &c_block236.nHandle, DEVICE_NAME);

    if (status != S_OK)
    {
        printf("\nUnable to Open instance of AP236.\n");
    }
    else
    {
        if (APInitialize(c_block236.nHandle) == S_OK) /* Initialize */
        {
            GetAPAddress(c_block236.nHandle, &addr); /* Read back address */
            c_block236.brd_ptr = (struct map236 *)addr;
            c_block236.bInitialized = TRUE;
            c_block236.bAP = TRUE;
        }
    }

    return status;
}

