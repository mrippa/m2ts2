
#include "apCommon.h"
#include "AP323.h"

/*
{+D}
    SYSTEM:	    AP323 Software

    FILENAME:	    drvr323.c

    MODULE NAME:    main - main routine of example software.

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This module is the main routine for the example program
            which demonstrates how the Library is used.

    CALLING
    SEQUENCE:

    MODULE TYPE:    void

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------


{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:

    This module is the main routine for the example program
    which demonstrates how the Library is used.
*/

int cor_data[SA_CHANS][SA_SIZE];            /* allocate  corrected data storage area */
unsigned short raw_data[SA_CHANS][SA_SIZE]; /* allocate raw data storage area */
byte s_array[1024];                         /* input channel scan array */

int main(argc, argv)
int argc;
char *argv[];
{

    /*
        DECLARE LOCAL DATA AREAS:
    */

    char cmd_buff[32]; /* command line input buffer */
    unsigned finished; /* flag to exit program */
    int item;          /* menu item selection variable */
    int current_channel;
    int hstatus;               /* returned status */
    long addr;                 /* holds board address */
    int i, j;                  /* loop index */
    double s;                  /* span value */
    double z;                  /* zero value */
    int hflag;                 /* interrupt handler installed flag */
    struct cblk323 c_block323; /* configuration block */
    int ap_instance = 0;

    /*
        ENTRY POINT OF ROUTINE:
        INITIALIZATION
    */

    if (argc == 2)
        ap_instance = atoi(argv[1]);

    finished = 0; /* indicate not finished with program */
    hflag = 0;    /* indicate interrupt handler not installed yet */

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
    if (APOpen(ap_instance, &c_block323.nHandle, DEVICE_NAME) != S_OK)
    {
        printf("\nUnable to Open instance of AP323.\n");
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

    /*
        Enter main loop
    */

    while (!finished)
    {
        printf("\n\nAP323 Library Demonstration  Rev. A\n\n");
        printf(" 1. Exit this Program                    9. Correct Data For Scan Array\n");
        printf(" 2. Set Up Configuration Parameters     10. Display Auto Zero Data\n");
        printf(" 3. Read Status Command                 11. Display Calibration Data\n");
        printf(" 4. Attach Exception Handler            12. Display Raw Input Data\n");
        printf(" 5. Detach Exception Handler            13. Display Corrected Data\n");
        printf(" 6. Acquire Auto Zero Data              14. Display Corrected Data In Volts\n");
        printf(" 7. Acquire Calibration Data            15. Clear All Data Buffers\n");
        printf(" 8. Acquire Raw Input Data              16. View Reference Values\n");
        printf("\nSelect: ");
        scanf("%d", &item);

        /*
            perform the menu item selected.
        */

        switch (item)
        {
        case 1: /* exit program command */
            printf("Exit program(y/n)?: ");
            scanf("%s", cmd_buff);
            if (cmd_buff[0] == 'y' || cmd_buff[0] == 'Y')
                finished++;
            break;

        case 2: /* set up configuration block parameters */
            setconfAP323(&c_block323);
            break;

        case 3: /* read board status command */
            if (!c_block323.bInitialized)
                printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
            else
                readstatAP323(&c_block323); /* read board status */
            break;

        case 4: /* attach exception handler */
            if (hflag)
                printf("\n>>> ERROR: HANDLERS ARE ATTACHED <<<\n");
            else
            {
                hstatus = 0;
                /* If there were any errors, then print error messages.
                   If no errors, then set "handler attached" flag.*/

                if (hstatus)
                {
                    printf(">>> ERROR WHEN ATTACHING HANDLER <<<\n");
                    hflag = 0;
                }
                else
                {
                    hstatus = 0;
                    hstatus = EnableAPInterrupts(c_block323.nHandle);
                    if (hstatus != S_OK)
                    {
                        printf(">>> ERROR WHEN ENABLING INTERRUPTS <<<\n");
                        hflag = 0;
                    }
                    else
                    {
                        hflag = 1;
                        printf("\nHandlers are now attached\n");
                    }
                }
            } /* end of if/else */
            break;

        case 5: /* detach exception handlers */

            if (!hflag)
                printf(">>> ERROR: HANDLERS ALREADY DETACHED <<<\n");
            else
            {
                c_block323.scan_mode = 0; /* stop scan */
                c_block323.int_mode = 0;  /* disable interrupts */
                cnfgAP323(&c_block323);   /* configure the board */
                hflag = 0;
                DisableAPInterrupts(c_block323.nHandle);
            }
            break;

        case 6: /* read auto zero values */
            if (!c_block323.bInitialized)
            {
                printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
                break;
            }

            calibrateAP323(&c_block323, AZ_SELECT); /* get auto-zero values */
            break;

        case 7: /* read calibration data values */
            if (!c_block323.bInitialized)
            {
                printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
                break;
            }

            calibrateAP323(&c_block323, CAL_SELECT); /* get calibration values */
            break;

        case 8: /* read raw input data values */
            if (!c_block323.bInitialized)
            {
                printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
                break;
            }
            if (hflag == 0 && c_block323.int_mode != 0)
            {
                printf("\n>>> ERROR: NO INTERRUPT HANDLERS ATTACHED <<<\n");
                break;
            }
            convertAP323(&c_block323); /* convert the board */
            break;

        case 9:                     /* calculate corrected data value for multi channel*/
            mccdAP323(&c_block323); /* correct input data */
            break;

        case 10: /* display autozero data */
            printf("\naz  data %04X\n", c_block323.s_az_buf);
            break;

        case 11: /* display calibration data */
            printf("\ncal data %04X\n", c_block323.s_cal_buf);
            break;

        case 12: /* display raw input data */
            printf("\n\nEnter channel number in hex: ");
            scanf("%x", &current_channel);
            current_channel &= 0x2f; /* limit range */

            for (i = 0; i < SA_SIZE; i++)
            {
                /*
                    check for modulo 8 to see if we need to print title info.
                */
                if ((i & 0x7) == 0)
                {
                    printf("\nCh %X  raw data[", current_channel);
                    printf("%X", (i & 0xF00) >> 8);
                    printf("%X", (i & 0xF0) >> 4);
                    printf("%X] ", i & 0xf);
                }

                /*
                    print the data with leading zeros.
                */
                printf(" %X", (c_block323.s_raw_buf[current_channel][i] & 0xf000) >> 12);
                printf("%X", (c_block323.s_raw_buf[current_channel][i] & 0x0f00) >> 8);
                printf("%X", (c_block323.s_raw_buf[current_channel][i] & 0x00f0) >> 4);
                printf("%X", c_block323.s_raw_buf[current_channel][i] & 0x000f);

                if (i == 167 || i == 335 || i == 503 || i == 671 || i == 839 || i == 1023)
                {
                    printf("\n\nEnter 0 to Exit or Data Block to View  1, 2, 3, 4, 5, 6.   Select: ");
                    scanf("%d", &j);
                    switch (j)
                    {
                    case 1:
                        i = -1;
                        break;
                    case 2:
                        i = 167;
                        break;
                    case 3:
                        i = 335;
                        break;
                    case 4:
                        i = 503;
                        break;
                    case 5:
                        i = 671;
                        break;
                    case 6:
                        i = 839;
                        break;
                    default:
                        goto quit_raw;
                        break;
                    }
                }
            }
        quit_raw:
            printf("\n");
            break;

        case 13: /* display corrected data */
            printf("\n\nEnter channel number in hex: ");
            scanf("%x", &current_channel);
            current_channel &= 0x2f; /* limit range */

            for (i = 0; i < SA_SIZE; i++)
            {
                /*
                    check for modulo 8 to see if we need to print title info.
                */
                if ((i & 0x7) == 0)
                {
                    printf("\nCh %X  cor data[", current_channel);
                    printf("%X", (i & 0xF00) >> 8);
                    printf("%X", (i & 0xF0) >> 4);
                    printf("%X] ", i & 0xf);
                }

                /*
                    print the data with leading zeros.
                */

                printf(" %X", (c_block323.s_cor_buf[current_channel][i] & 0xf000) >> 12);
                printf("%X", (c_block323.s_cor_buf[current_channel][i] & 0x0f00) >> 8);
                printf("%X", (c_block323.s_cor_buf[current_channel][i] & 0x00f0) >> 4);
                printf("%X", c_block323.s_cor_buf[current_channel][i] & 0x000f);

                if (i == 167 || i == 335 || i == 503 || i == 671 || i == 839 || i == 1023)
                {
                    printf("\n\nEnter 0 to Exit or Data Block to View  1, 2, 3, 4, 5, 6.   Select: ");
                    scanf("%d", &j);
                    switch (j)
                    {
                    case 1:
                        i = -1;
                        break;
                    case 2:
                        i = 167;
                        break;
                    case 3:
                        i = 335;
                        break;
                    case 4:
                        i = 503;
                        break;
                    case 5:
                        i = 671;
                        break;
                    case 6:
                        i = 839;
                        break;
                    default:
                        goto quit_raw;
                        break;
                    }
                }
            }
            printf("\n");
            break;

        case 14: /* display corrected data in volts */
            printf("\n\nEnter channel number in hex: ");
            scanf("%x", &current_channel);
            current_channel &= 0x2f; /* limit range */

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
                    printf("\n\nEnter 0 to Exit or Data Block to View 1, 2, 3, 4, 5, 6, 7, 8, 9, A, B Select: ");
                    scanf("%x", &j);
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
            break;

        case 15:                                   /* clear all data buffers */
            memset(cor_data, 0, sizeof(cor_data)); /* clear corrected sample buffer */
            memset(raw_data, 0, sizeof(raw_data)); /* clear raw sample buffer */
            break;

        case 16: /* View Reference Values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
            printf("Voltage Ref[0] = %s\n", &c_block323.RefCalValues[0][0]);
            printf("Voltage Ref[1] = %s\n", &c_block323.RefCalValues[1][0]);
            printf("Voltage Ref[2] = %s\n", &c_block323.RefCalValues[2][0]);
            printf("Voltage Ref[3] = %s\n", &c_block323.RefCalValues[3][0]);
            break;
        } /* end of switch */
    }     /* end of while */

    /*
        disable interrupts from module
    */
    if (c_block323.bInitialized)
    {
        c_block323.scan_mode = 0; /* stop scan */
        c_block323.int_mode = 0;  /* disable interrupts */
        cnfgAP323(&c_block323);   /* configure the board */
    }

    DisableAPInterrupts(c_block323.nHandle);
    if (c_block323.bAP)
        APClose(c_block323.nHandle);

    printf("\nEXIT PROGRAM\n");
    return (0);
} /* end of main */

/*
{+D}
    SYSTEM:	    AP323 Software

    FILENAME:	    drvr323.c

    MODULE NAME:    setconfAP323 - set configuration block contents.

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FJM

    ABSTRACT:	    Routine which is used to enter parameters into
            the Configuration Block.

    CALLING
    SEQUENCE:   setconfAP323(c_block323)
            where:
            c_block323 (structure pointer)
              The address of the configuration param. block

    MODULE TYPE:    void

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

    REVISIONS:

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:
*/

void setconfAP323(c_blk) struct cblk323 *c_blk;
{

    /*
        DECLARE LOCAL DATA AREAS:
    */
    int item;                     /* menu item variable */
    unsigned finished, finished2; /* flags to exit loops */
    int i;                        /* loop index */
    int elements;                 /* number of elements in scan array */

    /*
        ENTRY POINT OF ROUTINE:
    */
    finished = 0;
    while (!finished)
    {
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
        printf("\nselect: ");
        scanf("%d", &item);
        switch (item)
        {
        case 1: /* return to previous menu */
            finished++;
            break;

        case 2: /* board address */
            printf("ADDRESS CAN NOT BE CHANGED\n");
            break;

        case 3: /* input range */
            printf("\nInput Range:\n  0 to  +5 = 0\n -5 to  +5 = 1\n-10 to +10 = 2\n  0 to +10 = 3\n");
            c_blk->range = (byte)get_param();
            break;

        case 4: /* acquisition mode */
            printf("\nAcquisition Modes:\nDifferential = 0\nSingle Ended = 1\nAuto Zero    = 2\nCalibration  = 3\n");
            c_blk->acq_mode = (byte)get_param();
            break;

        case 5: /* scan mode */
            printf("\nScan Modes:\nDisable Scan       = 0\nUniform Continuous = 1\nUniform Single     = 2");
            printf("\nBurst Continuous   = 3\nBurst Single       = 4\nExternal Trigger   = 5\n");
            c_blk->scan_mode = (byte)get_param();
            break;

        case 6: /* data format */
            printf("\nData Format:\nTwo's Compliment = 0\nStraight Binary  = 1\n");
            c_blk->data_format = (byte)get_param();
            break;

        case 7: /* timer prescaler */
            printf("\nTimer Prescaler:\nThis 8-bit value has a minimum allowed value restriction of 0x40 or 64 decimal\n");
            c_blk->timer_ps = (byte)get_param();
            break;

        case 8: /* conversion timer */
            printf("\nConversion Timer:\nA 16-bit conversion timer value and has a minimum allowed value restriction of 1\n");
            c_blk->conv_timer = (word)get_param();
            break;

        case 9: /* timer enable */
            printf("\nTimer Enable:\nDisable Timer = 0\nEnable Timer  = 1\n");
            c_blk->timer_en = (byte)get_param();
            break;

        case 10: /* trigger direction */
            printf("\nTrigger Direction:\nTrigger Disabled = 0\nTrigger Input    = 1\nTrigger Output   = 2\n");
            c_blk->trigger = (byte)get_param();
            break;

        case 11: /* interrupt mode */
            printf("\nInterrupt Mode:\nInterrupts Disabled          = 0\nInterrupt After Each Channel = 1\nInterrupt After Each Scan    = 2\n");
            c_blk->int_mode = (byte)get_param();
            break;

        case 12: /* set up scan array */
            finished2 = 0;
            /*
              compute the number of elements in the scan array by doing pointer arithmetic
            */
            elements = (c_blk->sa_end - c_blk->sa_start) + 1;

            while (!finished2)
            {
                printf("\nSet Up Scan Array\n\n");
                printf(" 1. Return to Previous Menu\n");
                printf(" 2. Enter the number of elements (%d decimal max): %d\n", SA_SIZE, elements);
                for (i = 0; i < elements; i++)
                {
                    printf("%2d. Channel for Array Index %2X:%3X\n", (i + 3), i, *(c_blk->sa_start + i));
                }
                printf("\nselect: ");
                scanf("%d", &item);
                switch (item)
                {
                case 1: /* return to previous menu */
                    finished2++;
                    break;

                case 2: /* get no. of elements */

                    elements = (word)get_param();
                    if (elements < 1)
                        elements = 1;
                    else
                    {
                        if (elements > 1024)
                            elements = 1024;
                    }
                    c_blk->sa_end = c_blk->sa_start + (elements - 1);
                    break;

                default:
                    if (item > 2 && item <= 2 + elements)
                    {
                        printf("channel number - ");
                        *(c_blk->sa_start + item - 3) = (word)get_param();
                    }
                    break;
                }
            }
            break;
        }
    }
}

/*
{+D}
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

    I/O RESOURCES:

    SYSTEM
    RESOURCES:

    MODULES
    CALLED:

   REVISIONS:

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTIONAL DETAILS:
*/

void readstatAP323(c_blk) struct cblk323 *c_blk;
{

    /*
        DECLARE LOCAL DATA AREAS:
    */
    int item;          /* menu item variable */
    unsigned finished; /* flags to exit loops */
    int index;

    /*
        ENTRY POINT OF ROUTINE:
    */

    finished = 0;
    while (!finished)
    {
        rstsAP323(c_blk); /* Read Status Command */
        printf("\nBoard Status Information\n");
        printf("Location Register:            %X\n", c_blk->location);
        printf("Firmware Revision:             %c\n", (char)c_blk->revision);
        printf("Board Control Register:     %04X\n", c_blk->control);
        printf("Scan Status Register:       %04X\n", c_blk->scan_status);
        printf("Scan List Count Register:   %04X\n", c_blk->scan_count);
        printf("FIFO Sample Count Register: %04X\n", c_blk->FIFO_count);

        printf("\nRead Board Status\n");
        printf("1. Return to Previous Menu\n");
        printf("2. Read Status Again\n3. FPGA Temp/Vcc Values\n");
        printf("\nselect: ");
        scanf("%d", &item);

        switch (item)
        {
        case 1: /* return to previous menu */
            finished++;
            break;

        case 3: /* display temp & VCC info from FPGA */
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
            break;
        }
    }
}
