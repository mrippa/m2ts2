#include "m2ts323.h"
extern AP323Card m2tsAP323Card[NUM_AP323_CARDS];

int M2TSConfigAP323(int cardNumber)
{

    int i;

    APSTATUS status = 0;
    AP323Card *p323Card = &m2tsAP323Card[cardNumber];

    if (!p323Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    memset(p323Card->s_array, 0, sizeof(p323Card->s_array));   /* clear s_array */
    memset(p323Card->cor_data, 0, sizeof(p323Card->cor_data)); /* clear corrected sample buffer */
    memset(p323Card->raw_data, 0, sizeof(p323Card->raw_data)); /* clear raw sample buffer */

    for (i = 0; i < SA_CHANS; i++)
    {
        p323Card->c_block.s_cor_buf[i] = &(p323Card->cor_data[i][0]); /* corrected buffer start for each channel */
        p323Card->c_block.s_raw_buf[i] = &(p323Card->raw_data[i][0]); /* raw buffer start for each channel */
    }

    switch (cardNumber)
    {

    case 0:
        p323Card->c_block.range = RANGE_10TO10;               /* default +-10 V */
        p323Card->c_block.acq_mode = SE_SELECT;               /* mode */
        p323Card->c_block.scan_mode = UN_CONT;                /* scan mode */
        p323Card->c_block.data_format = SB_SELECT;            /* A/D data format */
        p323Card->c_block.timer_ps = 0x40;                    /* prescaler */
        p323Card->c_block.conv_timer = 0x6;                   /* counter */
        p323Card->c_block.timer_en = TIMER_ON;                /* timer on */
        p323Card->c_block.trigger = TO_SELECT;                /* trigger I/O is output */
        p323Card->c_block.int_mode = INT_DIS;                 /* disable interrupt mode */
        p323Card->c_block.control = 0;                        /* control register used by read only*/
        p323Card->c_block.sa_start = &(p323Card->s_array[0]); /* address of start of scan array */
        p323Card->c_block.sa_end = &(p323Card->s_array[0]);   /* address of end of scan array */
        break;

    case 1:
        p323Card->c_block.range = RANGE_10TO10;               /* default +-10 V */
        p323Card->c_block.acq_mode = SE_SELECT;               /* mode */
        p323Card->c_block.scan_mode = UN_CONT;                /* scan mode ....>Burst single 14us per conv.time not used*/
        p323Card->c_block.data_format = SB_SELECT;            /* A/D data format */
        p323Card->c_block.timer_ps = 0x40;                    /* prescaler */
        p323Card->c_block.conv_timer = 0x2;                   /* conversion time set to 0x2 which is min val. ... */
        p323Card->c_block.timer_en = TIMER_ON;                /* timer on */
        p323Card->c_block.trigger = TO_SELECT;                /* trigger I/O is output */
        p323Card->c_block.int_mode = INT_AGC;                 /* disable interrupt mode */
        p323Card->c_block.control = 0;                        /* control register used by read only*/
        p323Card->c_block.sa_start = &(p323Card->s_array[0]); /* address of start of scan array */
        p323Card->c_block.sa_end = &(p323Card->s_array[0]);   /* address of end of scan array */

        /*TODO: Set Spand and Z offset for ..................CARDS */
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
        break;

    default:
        printf("Config AP323 for cardNumber %d success 0x%x\n", cardNumber, status);
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
                printf("\nHandlers are now attached for AP323 Card %d\n", cardNumber );
            }
        }
    }

    printf("Config AP323 done! 0x%x\n", status);

    return status;
}