#include <epicsPrint.h>
#include <epicsExport.h>
#include <iocsh.h>

#include "apCommon.h"
#include "m2ts323.h"

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

/* Forward Declarations for Private Access to InitAP323.c */
static void showData(int cardNumber, int channelNumbr);
static void myreadstatAP323(struct cblk323 *c_blk);
static void start323MainLoop(int cardNumber);

int cor_data[SA_CHANS][SA_SIZE];            /* allocate  corrected data storage area */
unsigned short raw_data[SA_CHANS][SA_SIZE]; /* allocate raw data storage area */
byte s_array[1024];                         /* input channel scan array */

int m2tsAP323CardsConfigured = 0;
int m2tsAP323ConfigFirst     = 1;




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

    if (m2tsAP323ConfigFirst == 1)
    {
        for (i = 0; i < NUM_AP323_CARDS; i++)
        {
            m2tsAP323Card[i].initialized = FALSE;
            m2tsAP323Card[i].configured = FALSE;
            m2tsAP323Card[i].card = i;
            m2tsAP323Card[i].hflag = 0;       /* indicate interrupt handler not installed yet */
            m2tsAP323Card[i].adc_running = 0; /* indicate the adc is not running*/
            
            m2tsAP323Card[i].c_block.nHandle = 0;
            printf("Set AP323 Card %d handle to %d \n ", i, m2tsAP323Card[i].c_block.nHandle );
        }
        m2tsAP323ConfigFirst = 0;

    }

    if ((cardNumber < 0) | (cardNumber >= NUM_AP323_CARDS))
    {
        errlogPrintf("%s: Card number %d invalid -- must be 0 to %d.\n",
                     MyName, cardNumber, NUM_AP323_CARDS - 1);
        return (ERROR);
    }

    p323Card = &m2tsAP323Card[cardNumber];
    memset(&(p323Card->c_block), 0, sizeof(p323Card->c_block)); /*  Initialize the Configuration Parameter Block */
    printf("Size of the p323Card->c_block member: %zu bytes\n", sizeof(p323Card->c_block));
    
    memset(s_array, 0, sizeof(s_array));        /* clear s_array */
    memset(cor_data, 0, sizeof(cor_data)); /* clear corrected sample buffer */
    memset(raw_data, 0, sizeof(raw_data)); /* clear raw sample buffer */

    for (i = 0; i < SA_CHANS; i++)
    {
        p323Card->c_block.s_cor_buf[i] = &cor_data[i][0]; /* corrected buffer start for each channel */
        p323Card->c_block.s_raw_buf[i] = &raw_data[i][0]; /* raw buffer start for each channel */
    }


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
    printf("AP323 card %d has handle %d\n", p323Card->card, p323Card->c_block.nHandle);
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
            printf("APInitialize is false with status 0x%X\n", status);
            return (ERROR);
        }
        else
        {
            printf("APInitialize is false\n");
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

    /* Basic Test Loop for AP323*/
    //start323MainLoop(cardNumber);

    printf("Board address is %p ", &(p323Card->c_block.brd_ptr));
    printf("Board address is %d ", p323Card->c_block.bInitialized);
    printf("Board address is %d ", p323Card->c_block.bAP);
    printf("Init AP323 done! 0x%x\n", status);

    return status;
}

int M2ReadStatusAP323(int cardNumber)
{

    AP323Card *p323Card;
    p323Card = &m2tsAP323Card[cardNumber];

    if (!(p323Card->c_block.bInitialized))
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
    else
        myreadstatAP323( &(p323Card->c_block) ); /* read board status */

    return 0;
}

/*
    SYSTEM:	    AP323 Software

    MODULE NAME:    readstatAP323 - read board status.

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FJM

    ABSTRACT:	    Routine which is used to "Read Board Status" and to
            print the results to the console.

    CALLING
        SEQUENCE:   readstatAP323(&p323Card->c_block)
            where:
                        p323Card->c_block (structure pointer)
                          The address of the configuration param. block

    MODULE TYPE:    void

*/
static void myreadstatAP323(struct cblk323 *c_blk)
{

    /*
        DECLARE LOCAL DATA AREAS:
    */
    int index;

    /*
        ENTRY POINT OF ROUTINE:
    */
    rstsAP323(c_blk); /* Read Status Command */
    printf("\nBoard Status Information\n");
    printf("Location Register:            %X\n", c_blk->location);
    printf("Firmware Revision:             %c\n", (char)c_blk->revision);
    printf("Board Control Register:     %04X\n", c_blk->control);
    printf("Scan Status Register:       %04X\n", c_blk->scan_status);
    printf("Scan List Count Register:   %04X\n", c_blk->scan_count);
    printf("FIFO Sample Count Register: %04X\n", c_blk->FIFO_count);

    for (index = 0; index < 9; index++)
    {
        printf("Adr: %02X  FPGAData: %04X  ",
               ((c_blk->FPGAAdrData[index] >> 16) & 0x7F),
               ((c_blk->FPGAAdrData[index] >> 6) & 0x0FFF));

        if ((c_blk->FPGAAdrData[index] >> 16) & 3) /* Vcc */
            printf("%7.3f V\n", ((c_blk->FPGAAdrData[index] >> 6) & 0x03FF) / 1024.0 * 3.0);
        else /* T deg C */
            printf("%7.3f Deg C\n", ((c_blk->FPGAAdrData[index] >> 6) & 0x0FFF) * 503.975 / 1024.0 - 273.15);
    }

    /*This section dumps the board config*/
    printf("\n\nConfiguration Parameters\n\n");
    printf(" 1. Return to Previous Menu\n");
    printf(" 2. Board Pointer:	%lX\n", (unsigned long)c_blk->brd_ptr);
    printf(" 3. Input Range:        %X\n", c_blk->range);
    printf(" 4. Acquisition Mode:   %X\n", c_blk->acq_mode);
    printf(" 5. Scan Mode:          %X\n", c_blk->scan_mode);
    printf(" 6. Data Format:        %X\n", c_blk->data_format);
    printf(" 7. Timer Prescaler:    %X\n", c_blk->timer_ps);
    printf(" 8. Conversion Timer:   %04X\n", c_blk->conv_timer);
    printf(" 9. Timer Enable:       %X\n", c_blk->timer_en);
    printf("10. Trigger Direction:  %X\n", c_blk->trigger);
    printf("11. Interrupt Mode:     %X\n", c_blk->int_mode);
    printf("12. Set Up Scan Array\n");
    printf("    Scan Array Start:   %lX\n", (unsigned long)c_blk->sa_start);
    printf("    Scan Array End:     %lX\n", (unsigned long)c_blk->sa_end);

    showData(0, 5); /*card 0, channel 5*/
}

/**
 * @brief
 *
 * @param channel
 */
static void showData(int cardNumber, int channelNumber)
{

    AP323Card *p323Card;
    int i,j;
    
    p323Card = &m2tsAP323Card[cardNumber];

    switch (p323Card->c_block.range)
    {
    case RANGE_0TO5:
        p323Card->z = 0.0000;
        p323Card->s = 5.0000;
        break;

    case RANGE_5TO5:
        p323Card->z = -5.0000;
        p323Card->s = 10.0000;
        break;

    case RANGE_0TO10:
        p323Card->z = 0.0;
        p323Card->s = 10.0000;
        break;

    default:
        p323Card->z = -10.0000; /* RANGE_10TO10 */
        p323Card->s = 20.0000;
        break;
    }

    for (i = 0; i < 2; i++)
    {
        /*
            check for modulo 8 to see if we need to print title info.
        */
        //              if((i & 0x3) == 0)
        //              {
        //                printf("\nCh %X  Volts[",current_channel);
        //                printf("%X",(i & 0xF00) >> 8);
        //                printf("%X",(i & 0xF0) >> 4);
        //                printf("%X] ",i & 0xf);
        //              }
        //
        printf("%12.6f\n", ((((double)p323Card->c_block.s_cor_buf[channelNumber][i]) * p323Card->s) / (double)65536.0) + p323Card->z);

        if (i == 91 || i == 183 || i == 275 || i == 367 || i == 459 || i == 551 || i == 643 || i == 735 || i == 827 || i == 919 || i == 1023)
        {
            // printf("\n\nEnter 0 to Exit or Data Block to View 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B Select: ");
            // scanf("%x", &j);
            j = 0;
            switch (j)
            {
            case 1:
                i = -1;
                break;
            case 2:
                i = 91;
                break;
            case 3:
                i = 183;
                break;
            case 4:
                i = 275;
                break;
            case 5:
                i = 367;
                break;
            case 6:
                i = 459;
                break;
            case 7:
                i = 551;
                break;
            case 8:
                i = 643;
                break;
            case 9:
                i = 735;
                break;
            case 0xA:
                i = 827;
                break;
            case 0xB:
                i = 919;
                break;
            default:
                goto quit_volt;
                break;
            }
        }
    }
quit_volt:
    printf("\n");
}

void M2AcqAP323_show(int cardNumber, int channel_number)
{

    AP323Card *p323Card;
    int i;

    p323Card = &m2tsAP323Card[cardNumber];

    if (! p323Card->c_block.bInitialized)
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
    else
    {
        for (i = 0; i <= channel_number; i++)
        {
            printf("ch %d: %12.6f volts\n", i, ((((double)p323Card->c_block.s_cor_buf[0][i]) * 20.0) / (double)65536.0) + (-10.0));
        }
    }
}

/* M2Acquire
 *
 *
 */
void M2AcqAP323_runOnce(int cardNumber)
{
    AP323Card *p323Card;

    p323Card = &m2tsAP323Card[cardNumber];

    if (p323Card->adc_running)
    {
        // printf("\n>>>ERROR: thread called with ADC Running\n");
        handle_error("ADC Running");
    }
    p323Card->adc_running = 1;

    if (!p323Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        handle_error("ADC BOARD");
    }

    calibrateAP323(&(p323Card->c_block), AZ_SELECT);  /* get auto-zero values */
    calibrateAP323(&(p323Card->c_block), CAL_SELECT); /* get calibration values */

    if (p323Card->hflag == 0 && p323Card->c_block.int_mode != 0)
    {
        handle_error("ADC NO_INT");
    }

    // printf("Start M2AcqAP323_run\n");

    convertAP323(&(p323Card->c_block)); /* convert the board */
    mccdAP323(&(p323Card->c_block));    /* correct input data */

    p323Card->adc_running = 0;
    // printf("End M2AcqAP323_run\n");

    return;
}

int M2ReadAP323(int cardNumber, int  channelNumber, double *val)
{

    AP323Card *p323Card;

    p323Card = &m2tsAP323Card[cardNumber];

    if (!p323Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return -1;
    }
    else
    {
        *val = (((((double)p323Card->c_block.s_cor_buf[0][channelNumber]) * 20.0) / (double)65536.0) + (-10.0));
    }

    return 0;
}

int M2AcqStartAndShow()
{

    int i = 0;

    for (i = 0; i < 50; i++)
    {
        M2AcqAP323_runOnce(0);
        M2AcqAP323_show(0, 5); /* Card 0, Channel 5*/
    }
    printf("M2AcqStart finished\n");

    return (0);
}

EPICSTHREADFUNC AP323RunLoop( )
{

    double volts_input = 0.0;

    for (;;)
    {
        M2AcqAP323_runOnce(0);
        M2ReadAP323( 0, 5, &volts_input); /* Card 0, channel 5 */
        //write_AP236out(volts_input);

        // epicsThreadSleep(0.0);
    }
}

static void start323MainLoop(int cardNumber)
{

    AP323Card *p323Card;
    p323Card = &m2tsAP323Card[cardNumber]; /* Card 0 */

    p323Card->AP323RunLoopTaskId = epicsThreadCreate("AP323RunLoop",
                                           90, epicsThreadGetStackSize(epicsThreadStackMedium),
                                           (EPICSTHREADFUNC)AP323RunLoop, NULL);

    // taskwdInsert(RunLoopTaskId, NULL, NULL);
}

/*M2ReadStatAP323*/
static const iocshFuncDef M2ReadStatAP323FuncDef = {"M2ReadStatAP323", 0, NULL};

static void M2ReadStatAP323Func(const iocshArgBuf *args)
{
    M2ReadStatusAP323(1); /* Card 1*/
} 

static void M2ReadStatAP323Register(void)
{
    iocshRegister(&M2ReadStatAP323FuncDef, M2ReadStatAP323Func);
}

/*M2AcqStart*/
static const iocshFuncDef M2AcqStartFuncDef = {"M2AcqStart", 0, NULL};

static void M2AcqStartFunc(const iocshArgBuf *args)
{
    M2AcqStartAndShow();
}

static void M2AcqStartRegister(void)
{
    iocshRegister(&M2AcqStartFuncDef, M2AcqStartFunc);
}

epicsExportRegistrar(M2ReadStatAP323Register);
epicsExportRegistrar(M2AcqStartRegister);
