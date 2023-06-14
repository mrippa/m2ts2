#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts471.h"

AP471Card m2tsAP471Card[NUM_AP471_CARDS];
int m2tsAP471InitFirst = 1;

int M2TSInitAP471(int cardNumber)
{

    AP471Card *p471Card;
    APSTATUS status = 0;
    int i;
    char *MyName = "M2TSInitAP471";
    /*
        Initialize the Configuration Parameter Block to default values.
    */

    /*
     * Check if the cards have already been intialized before.
     */

    if (m2tsAP471InitFirst == 1)
    {
        for (i = 0; i < NUM_AP471_CARDS; i++)
        {
            m2tsAP471Card[i].initialized = FALSE;
            m2tsAP471Card[i].configured = FALSE;
            m2tsAP471Card[i].card = i;
            m2tsAP471Card[i].hflag = 0;       /* indicate interrupt handler not installed yet */
            
            m2tsAP471Card[i].c_block.nHandle = 0;
            printf("Set AP471 Card %d handle to %d \n ", i, m2tsAP471Card[i].c_block.nHandle );
        }
        m2tsAP471InitFirst = 0;

    }

    if ((cardNumber < 0) | (cardNumber >= NUM_AP471_CARDS))
    {
        errlogPrintf("%s: Card number %d invalid -- must be 0 to %d.\n",
                     MyName, cardNumber, NUM_AP471_CARDS - 1);
        return (ERROR);
    }

    p471Card = &m2tsAP471Card[cardNumber];


    memset(&(p471Card->c_block), 0, sizeof(p471Card->c_block));
    memset(&(p471Card->s_block), 0, sizeof(p471Card->s_block));
    p471Card->c_block.bAP = FALSE;                                          /* indicate no AP initialized and set up yet */
    p471Card->c_block.bInitialized = FALSE;                                 /* indicate not ready to talk to AP */
    p471Card->c_block.sblk_ptr = (struct sblk471 *) &(p471Card->s_block);    /* set address of status structure */

    /*
        Initialize the AP library
    */
    if (InitAPLib() != S_OK)
    {
        printf("\nUnable to initialize the AP library. Exiting program.\n");
        exit(0);
    }

    /*
        Open an instance of a device
        Other device instances can be obtained
        by changing parameter 1 of APOpen()
    */

    status = APOpen(p471Card->card, &(p471Card->c_block).nHandle, AP471_DEVICE_NAME);
    if (status != S_OK)
    {
        printf("\nUnable to Open instance of %s.\n", AP471_DEVICE_NAME);
    }
    else
    {
        status = APInitialize(p471Card->c_block.nHandle);
        if (status == S_OK) /* Initialize */
        {
            GetAPAddress(p471Card->c_block.nHandle, &(p471Card->addr)); /* Read back address */
            p471Card->c_block.brd_ptr = (struct map471 *) (p471Card->addr);
            p471Card->c_block.bInitialized = TRUE;
            p471Card->c_block.bAP = TRUE;

            /* Create the interrupt processing thread */
            // pthread_create(&IHandler, NULL, (void *)&InterruptHandlerThread, (struct cblk471 *) &c_block471);
        }
        else {
            printf("AP471: Initialize is false with status 0x%X\n", status);
            return (ERROR);
        }

    }

    printf("471: Board address is %p \n", &(p471Card->c_block.brd_ptr));
    printf("471: Board Open flag %d \n", p471Card->c_block.bInitialized);
    printf("471: Board ready flag %d \n", p471Card->c_block.bAP);
    printf("Init AP471 done! 0x%x\n", status);

    return status;
}
