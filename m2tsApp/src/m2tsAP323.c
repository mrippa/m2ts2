#include "m2ts.h"
#include "apCommon.h"
#include "AP323.h"

static void showData(int current_channel);
static void myreadstatAP323(struct cblk323 *c_blk);

int cor_data[SA_CHANS][SA_SIZE];            /* allocate  corrected data storage area */
unsigned short raw_data[SA_CHANS][SA_SIZE]; /* allocate raw data storage area */
byte s_array[1024];                         /* input channel scan array */

unsigned finished;         /* flag to exit program */
int hstatus;               /* returned status */
long addr;                 /* holds board address */
int i, j;                  /* loop index */
double s;                  /* span value */
double z;                  /* zero value */
int hflag;                 /* interrupt handler installed flag */
struct cblk323 c_block323; /* configuration block */
int ap_instance = 1;

/*
 *
 * TODO:
 */
int InitAP323(void)
{

    APSTATUS status = 0;

    /*
        ENTRY POINT OF ROUTINE:
        INITIALIZATION
    */

    // if (argc == 2)
    //     ap_instance = atoi(argv[1]);

    hflag = 0; /* indicate interrupt handler not installed yet */

    memset(&c_block323, 0, sizeof(c_block323)); /*  Initialize the Configuration Parameter Block */
    memset(s_array, 0, sizeof(s_array));        /* clear s_array */

    memset(cor_data, 0, sizeof(cor_data)); /* clear corrected sample buffer */
    memset(raw_data, 0, sizeof(raw_data)); /* clear raw sample buffer */

    for (j = 0; j < SA_CHANS; j++)
    {
        c_block323.s_cor_buf[j] = &cor_data[j][0]; /* corrected buffer start for each channel */
        c_block323.s_raw_buf[j] = &raw_data[j][0]; /* raw buffer start for each channel */
    }

    c_block323.range = RANGE_10TO10;    /* default +-10 V */
    c_block323.acq_mode = SE_SELECT;    /* mode */
    c_block323.scan_mode = UN_CONT;     /* scan mode */
    c_block323.data_format = SB_SELECT; /* A/D data format */
    c_block323.timer_ps = 0x40;         /* prescaler */
    c_block323.conv_timer = 0x6;        /* counter */
    c_block323.timer_en = TIMER_ON;     /* timer on */
    c_block323.trigger = TO_SELECT;     /* trigger I/O is output */
    c_block323.int_mode = INT_DIS;      /* disable interrupt mode */
    c_block323.control = 0;             /* control register used by read only*/
    c_block323.sa_start = &s_array[0];  /* address of start of scan array */
    c_block323.sa_end = &s_array[100];  /* address of end of scan array */
    c_block323.bAP = FALSE;             /* indicate not initialized and set up yet */
    c_block323.bInitialized = FALSE;    /* indicate not ready */
    c_block323.nHandle = 0;             /* make handle to a closed board */

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

    status = APOpen(ap_instance, &c_block323.nHandle, DEVICE_NAME);

    if (status != S_OK)
    {
        printf("\nUnable to Open instance of AP323 with status %d \n", status);
        finished = 1; /* indicate finished with program */
    }
    else
    {
        if (APInitialize(c_block323.nHandle) == S_OK) /* Initialize */
        {
            GetAPAddress(c_block323.nHandle, &addr); /* Read back address */
            c_block323.brd_ptr = (struct map323 *)addr;
            c_block323.bInitialized = TRUE;
            c_block323.bAP = TRUE;
        }
        memset(&c_block323.IDbuf[0], 0, sizeof(c_block323.IDbuf)); /* empty the buffer */
        ReadFlashID323(&c_block323, &c_block323.IDbuf[0]);

        /* Install ideal values for each reference value */
        /* Reference Values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
        strcpy(&c_block323.RefCalValues[0][0], "9.88");
        strcpy(&c_block323.RefCalValues[1][0], "4.94");
        strcpy(&c_block323.RefCalValues[2][0], "2.47");
        strcpy(&c_block323.RefCalValues[3][0], "1.235");

        /* If the AP323 flash ID is found and confirmed */
        if ((strstr((const char *)&c_block323.IDbuf[0], (const char *)FlashIDString) == NULL)) /* AP323 ID */
            printf("\nUnable to read APBoard FLASH ID.\n");
        else
        {
            /* Read the factory reference values from flash and overwrite the previously installed ideal values */
            if (ReadCalCoeffs323(&c_block323) != 0) /* read from flash into structure */
                printf("\nUnable to read calibration values from Flash memory.\n");
        }
    }

    printf("Init AP323 done Go to bed! 0x%x\n", status);

    return status;
}

int M2ReadStatAP323(void)
{
    if (!c_block323.bInitialized)
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
    else
        myreadstatAP323(&c_block323); /* read board status */

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
        SEQUENCE:   readstatAP323(&c_block323)
            where:
                        c_block323 (structure pointer)
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

    showData(0);
}

/* M2Acquire
 *
 *
 */
int M2AcqAP323()
{

    if (!c_block323.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return ERROR;
    }

    calibrateAP323(&c_block323, AZ_SELECT);  /* get auto-zero values */
    calibrateAP323(&c_block323, CAL_SELECT); /* get calibration values */

    if (hflag == 0 && c_block323.int_mode != 0)
    {
        printf("\n>>> ERROR: NO INTERRUPT HANDLERS ATTACHED <<<\n");
        return ERROR;
    }

    convertAP323(&c_block323); /* convert the board */
    mccdAP323(&c_block323);    /* correct input data */

    printf("M2AcqAP323\n");
    return 0;
}

/**
 * @brief
 *
 * @param channel
 */
static void showData(int current_channel)
{

    switch (c_block323.range)
    {
    case RANGE_0TO5:
        z = 0.0000;
        s = 5.0000;
        break;

    case RANGE_5TO5:
        z = -5.0000;
        s = 10.0000;
        break;

    case RANGE_0TO10:
        z = 0.0;
        s = 10.0000;
        break;

    default:
        z = -10.0000; /* RANGE_10TO10 */
        s = 20.0000;
        break;
    }

    for (i = 0; i < SA_SIZE; i++)
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
        printf("%12.6f\n", ((((double)c_block323.s_cor_buf[current_channel][i]) * s) / (double)65536.0) + z);

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
