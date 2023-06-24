#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"

int m2tsAP236InitFirst       = 1;

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


int M2TSInitAP236(int cardNumber)
{

    AP236Card *p236Card;
    APSTATUS status = 0;
    int i;
    char *MyName = "M2TSInitAP236";


    /*
     * Check if the cards have already been intialized before.
     */

    if (m2tsAP236InitFirst == 1)
    {
        printf("\nNew Card---------------------------------\n");
        for (i = 0; i < NUM_AP236_CARDS; i++)
        {
            m2tsAP236Card[i].initialized = FALSE;
            m2tsAP236Card[i].configured = FALSE;
            m2tsAP236Card[i].card = i;
            m2tsAP236Card[i].range = 0; /* Set the range to zero*/
            m2tsAP236Card[i].temp = 0; /* Set the temp to zero*/

            m2tsAP236Card[i].c_block.nHandle = 0;
            printf("Set AP236 Card %d handle to %d \n ", i, m2tsAP236Card[i].c_block.nHandle );
        }
        m2tsAP236InitFirst = 0;
    }

    if ((cardNumber < 0) | (cardNumber >= NUM_AP236_CARDS))
    {
        errlogPrintf("%s: Card number %d invalid -- must be 0 to %d.\n",
                     MyName, cardNumber, NUM_AP236_CARDS - 1);
        return (ERROR);
    }

    p236Card = &m2tsAP236Card[cardNumber];
    memset(&(p236Card->c_block), 0, sizeof(p236Card->c_block)); /*  Initialize the Configuration Parameter Block */
    printf("Size of the p236Card->c_block member: %zu bytes\n", sizeof(p236Card->c_block));

    /* Put the address of the initialized constant array into the configuration block structure */
    p236Card->c_block.pIdealCode = &IdealCode; /* pointer to Ideal Zero straight binary, and Slope constants */

    /*
        Initialize the Configuration Parameter Block to default values.
    */

    p236Card->c_block.bAP = FALSE;          /* indicate not initialized and set up yet */
    p236Card->c_block.bInitialized = FALSE; /* indicate not ready */
    p236Card->c_block.nHandle = 0;          /* make handle to a closed board */

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
    status = APOpen(p236Card->card, &(p236Card->c_block).nHandle, AP236_DEVICE_NAME);
    printf("AP236 card %d has handle %d\n", p236Card->card, p236Card->c_block.nHandle);

    if (status != S_OK)
    {
        printf("\nUnable to Open instance of AP236.\n");
    }
    else
    {
        status = APInitialize(p236Card->c_block.nHandle);
        if (status == S_OK) /* Initialize */
        {
            GetAPAddress(p236Card->c_block.nHandle, &(p236Card->addr)); /* Read back address */
            p236Card->c_block.brd_ptr = (struct map236 *) (p236Card->addr);
            p236Card->c_block.bInitialized = TRUE;
            p236Card->c_block.bAP = TRUE;
        }
        else
        {
            printf("AP323: Initialize is false with status 0x%X\n", status);
            return (ERROR);
        }
    }

    printf("-----------------------------------------------\n");
    printf("AP236 card %d has handle %d\n", p236Card->card, p236Card->c_block.nHandle);
    printf("Size of the p236Card->c_block member: %zu bytes\n", sizeof(p236Card->c_block));
    printf("236: Board address is %p \n", &(p236Card->c_block.brd_ptr));
    printf("236: Board Open flag %d \n", p236Card->c_block.bInitialized);
    printf("236: Board ready flag %d \n", p236Card->c_block.bAP);
    printf("Init AP236 done!\n");

    return status;
}

