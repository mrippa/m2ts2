#include "m2ts323.h"
#include "m2MirrorControl.h"

extern AP323Card m2tsAP323Card[NUM_AP323_CARDS];
static const char * timeFormatStr = "%Y-%m-%dT%H:%M:%S.%06f:%z";

void M2AP323Copy(int cardNumber) {

    AP323Card *p323Card;
    p323Card = &m2tsAP323Card[cardNumber];

    int *sourceArray = p323Card->c_block.s_cor_buf[0];
    
    // Copy data from the sourceArray to the destinationBuffer.
    memcpy(ap323Samples, sourceArray, 1024 * sizeof(double));

    epicsEventSignal(mcDataReadySem);
}

int M2ReadAP323(int cardNumber, int  channelNumber)
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
        for (size_t i = 0; i < 1024; i++)
        {
            /* code */
           ap323Samples[i] = (((((double)p323Card->c_block.s_cor_buf[channelNumber][i]) * p323Card->s) / (double)65536.0) + (p323Card->z));
        }
    }
    epicsEventSignal(mcDataReadySem);

    return 0;
}

/**
 * @brief
 *
 * @param channel
 */
void M2ShowDataAP323(int cardNumber, int channelNumber)
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
void myreadstatAP323(struct cblk323 *c_blk)
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

    M2ShowDataAP323(0, 5); /*card 0, channel 5*/
}

int M2AcqTestAndShow(int cardNumber, int channelNumber)
{

    int i = 0;

    for (i = 0; i < 50; i++)
    {
        M2AcqAP323_runOnce(cardNumber);
        M2AcqAP323_show(cardNumber, channelNumber); 
    }
    printf("M2AcqTest finished\n");

    return (0);
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

void M2AcqAP323_show(int cardNumber, int channel_number)
{

    AP323Card *p323Card;
    int i;
    epicsTimeStamp now;
    char nowText[28];

    p323Card = &m2tsAP323Card[cardNumber];

    /* Get the local time as a time stamp */
    epicsTimeGetCurrent(&now);
    epicsTimeToStrftime(nowText, sizeof(nowText), timeFormatStr, &now);

    if (!p323Card->c_block.bInitialized)
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
    else
    {
        for (i = 0; i <= channel_number; i++)
        {
            printf("[%s] ch %d: %12.6f volts\n", nowText, i, ((((double)p323Card->c_block.s_cor_buf[0][i]) * 20.0) / (double)65536.0) + (-10.0));
        }
    }
}

/***
 * 
 * 
 */
void M2PrintBuffer() {

        printBuffer(&m2TestAI_CB);
}
