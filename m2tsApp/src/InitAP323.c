#include <epicsPrint.h>
#include "m2ts323.h"

AP323Card m2tsAP323Card[NUM_AP323_CARDS];
CircularBuffer m2TestAI_CB;

#define handle_error_en(en, msg) \
    do                           \
    {                            \
        errno = en;              \
        perror(msg);             \
        exit(EXIT_FAILURE);      \
    } while (0)

#define handle_error(msg)   \
    do                      \
    {                       \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

static void start323MainLoop(int cardNumber);

int m2tsAP323InitFirst       = 1;


/*
 *
 * TODO:
 */
int M2TSInitAP323( int cardNumber)
{

    AP323Card *p323Card;
    APSTATUS status = 0;
    int i;
    char *MyName = "M2TSInitAP323";

    /*
     * Check if the cards have already been intialized before.
     */

    if (m2tsAP323InitFirst == 1)
    {
        printf("\nNew Card---------------------------------\n");
        for (i = 0; i < NUM_AP323_CARDS; i++)
        {
            m2tsAP323Card[i].initialized = FALSE;
            m2tsAP323Card[i].configured = FALSE;
            m2tsAP323Card[i].card = i;
            m2tsAP323Card[i].hflag = 0;       /* indicate interrupt handler not installed yet */
            m2tsAP323Card[i].adc_running = 0; /* indicate the adc is not running*/
            
            m2tsAP323Card[i].c_block.nHandle = 0;
            printf("Clear AP323 Card %d handle to %d \n", i, m2tsAP323Card[i].c_block.nHandle );
        }
        m2tsAP323InitFirst = 0;
    }

    if ((cardNumber < 0) | (cardNumber >= NUM_AP323_CARDS))
    {
        errlogPrintf("%s: Card number %d invalid -- must be 0 to %d.\n",
                     MyName, cardNumber, NUM_AP323_CARDS - 1);
        return (ERROR);
    }

    /* Assign the pointer to Card and set the Configuration Block */
    p323Card = &m2tsAP323Card[cardNumber];
    memset(&(p323Card->c_block), 0, sizeof(p323Card->c_block)); 
    
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

    status = APOpen(p323Card->card, &(p323Card->c_block).nHandle, AP323_DEVICE_NAME);
    if (status != S_OK)
    {
        printf("\nUnable to Open instance of AP323 with status %d \n", status);
    }
    else
    {
        status = APInitialize(p323Card->c_block.nHandle);
        if (status == S_OK) /* Initialize */
        {
            GetAPAddress(p323Card->c_block.nHandle, &(p323Card->addr)); /* Read back address */
            p323Card->c_block.brd_ptr = (struct map323 *) (p323Card->addr);
            p323Card->c_block.bInitialized = TRUE;
            p323Card->c_block.bAP = TRUE;
        }
        else {
            printf("AP323: Initialize is false with status 0x%X\n", status);
            return (ERROR);
        }
        memset(&(p323Card->c_block).IDbuf[0], 0, sizeof(p323Card->c_block.IDbuf)); /* empty the buffer */
        ReadFlashID323(&(p323Card->c_block), &(p323Card->c_block).IDbuf[0]);

        /* Install ideal values for each reference value */
        /* Reference Values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
        strcpy( &(p323Card->c_block).RefCalValues[0][0], "9.88");
        strcpy( &(p323Card->c_block).RefCalValues[1][0], "4.94");
        strcpy( &(p323Card->c_block).RefCalValues[2][0], "2.47");
        strcpy( &(p323Card->c_block).RefCalValues[3][0], "1.235");

        /* If the AP323 flash ID is found and confirmed */
        if ((strstr((const char *)&(p323Card->c_block).IDbuf[0], (const char *)AP323_FlashIDString) == NULL)) /* AP323 ID */
            printf("\nUnable to read APBoard FLASH ID.\n");
        else
        {
            /* Read the factory reference values from flash and overwrite the previously installed ideal values */
            if (ReadCalCoeffs323( &(p323Card->c_block) ) != 0) /* read from flash into structure */
                printf("\nUnable to read calibration values from Flash memory.\n");
        }
    }

    /* Setup interrupts if needed*/
    if (p323Card->c_block.int_mode != INT_DIS)
    {
        if (p323Card->hflag == 1)
        {
            printf("Interrup handlers already installed.\n");
        }
        else
        {

            p323Card->hstatus = 0;
            p323Card->hstatus = EnableAPInterrupts(p323Card->c_block.nHandle);
            if (p323Card->hstatus != S_OK)
            {
                printf(">>> ERROR WHEN ENABLING INTERRUPTS <<<\n");
                p323Card->hflag = 0;
            }
            else
            {
                p323Card->hflag = 1;
                printf("\nHandlers are now attached\n");
            }
        }
    }

    /*Init Calibration Status for each board*/
    p323Card->cal_autozero_complete = 0;
    p323Card->cal_select_complete = 0;

    p323Card->acqSem = epicsEventMustCreate(epicsEventEmpty);

    /* Basic Test Loop for AP323*/
    start323MainLoop(cardNumber);

    printf("Init Success: AP323 card has handle %d!\n", p323Card->c_block.nHandle );

    return status;
}

/* M2Acquire
 *
 *
 */
void M2AcqAP323_runOnce(int cardNumber)
{
    AP323Card *p323Card;

    p323Card = &m2tsAP323Card[cardNumber];

    if (!p323Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        handle_error("ADC BOARD");
    }

    if (p323Card->adc_running)
    {
        handle_error("ADC Running");
    }
    p323Card->adc_running = 1;

    if (p323Card->cal_autozero_complete != 1) {
        calibrateAP323(&(p323Card->c_block), AZ_SELECT);  /* get auto-zero values */
        p323Card->cal_autozero_complete = 1;
    }

    if (p323Card->cal_select_complete != 1) {
        calibrateAP323(&(p323Card->c_block), CAL_SELECT); /* get calibration values */
        //p323Card->cal_select_complete = 1; //TODO: temporary we close this after correct below
    }

    if (p323Card->hflag == 0 && p323Card->c_block.int_mode != 0)
    {
        handle_error("ADC NO_INT");
    }

    convertAP323(&(p323Card->c_block)); /* convert the board */


    /* Test optimizing performance by calling the correction once only */
    if (p323Card->cal_select_complete != 1) {
        mccdAP323(&(p323Card->c_block));    /* correct input data */
        p323Card->cal_select_complete = 1;  /* TODO: Borrowed Flag from Cal_Select*/ 
    }

    p323Card->adc_running = 0;
    epicsEventSignal(p323Card->acqSem);

    return;
}

EPICSTHREADFUNC AP323RunLoop( AP323Card *p323Card)
{

    double volts_input = 0.0;

    initializeBuffer(&m2TestAI_CB, 1000, "Test AI Signal");

    if (p323Card->card == 0)
        return (0);

    /*Only run for card 1*/
    for (;;)
    {
        //static int loop_count = 0;
        static int loop_first = 1;
        
        //volts_input += 0.000005; /*add 5 uV each sample*/    
        M2AcqAP323_runOnce(p323Card->card);
        epicsEventMustWait(p323Card->acqSem);

        M2ReadAP323( p323Card->card, 0, &volts_input); /* Card X, channel 0*/
        writeValue(&m2TestAI_CB, volts_input);
        //write_AP236out(volts_input);

        //epicsThreadSleep(0.0005); /* sleep 500 us */
        //loop_count++;
        if (loop_first == 1) {
            p323Card->cal_autozero_complete = 0;
            p323Card->cal_select_complete = 0;
            loop_first = 0; 
        }
    }

    /*clean up*/
    destroyBuffer(&m2TestAI_CB);

    return (0);
}


static void start323MainLoop(int cardNumber)
{

    AP323Card *p323Card;
    p323Card = &m2tsAP323Card[cardNumber]; /* Card 0 */

    p323Card->AP323RunLoopTaskId = epicsThreadCreate("AP323RunLoop",
                                           90, epicsThreadGetStackSize(epicsThreadStackMedium),
                                           (EPICSTHREADFUNC)AP323RunLoop, p323Card);

}
