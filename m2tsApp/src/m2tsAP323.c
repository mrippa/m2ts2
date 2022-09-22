#include "m2ts.h"
#include "apCommon.h"
#include "AP323.h"

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
int ap_instance = 0;


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
        readstatAP323(&c_block323); /* read board status */

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
void readstatAP323(struct cblk323 *c_blk)
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
}

/* M2Acquire
*
*
*/
int M2AcqAP323() 
{

    printf("M2AcqAP323");
    return 0;
}


