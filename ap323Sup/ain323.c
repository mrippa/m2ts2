
#include "apCommon.h"
#include "AP323.h"

/*
{+D}
    SYSTEM:	    Library Software

    MODULE NAME:    calibrate AP323

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:       This acquires the auto-zero or the calibration input
                for the board.

    CALLING
    SEQUENCE:   calibrate(ptr,mode);
            where:
            ptr (pointer to structure)
                Pointer to the configuration block structure.
            mode (int)
                mode auto-zero or calibration.
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

void calibrateAP323(struct cblk323 *c_blk, int mode)
{

    /*
        declare local storage
    */

    struct cblk323 temp_blk; /* copy of users configuration */
    byte temp_scn_data;      /* input channel scan data */
    double f_cor;
    int i;
    unsigned short *data;

    /*
        ENTRY POINT OF ROUTINE:
    */

    memcpy(&temp_blk, c_blk, sizeof(temp_blk)); /* copy users configuration structure */

    if (mode == AZ_SELECT)
        data = (unsigned short *)&c_blk->s_az_buf; /* auto-zero data */
    else
        data = (unsigned short *)&c_blk->s_cal_buf; /* calibration data */

    temp_blk.int_mode = INT_DIS;                          /* disable interrupts */
    temp_blk.acq_mode = mode;                             /* input mode */
    temp_blk.scan_mode = BU_SNGL;                         /* new scan mode */
    temp_blk.timer_en = TIMER_OFF;                        /* timer off */
    temp_scn_data = 0;                                    /* select channel zero in temp_scn_data */
    temp_blk.sa_start = temp_blk.sa_end = &temp_scn_data; /* point to the single channel element */

    cnfgAP323(&temp_blk); /* configure the board */

    for (f_cor = 0.0, i = 0; i < AVE_SAMPLES; i++)
    {
        output_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->trigFIFOclear, CLR_SAMPLE_FIFO); /* clear sample FIFO  */
        output_long(temp_blk.nHandle, (long *)&temp_blk.brd_ptr->trigFIFOclear, START_CONV);  /* start scan */

        /* wait until new data is present for channels */
        while (input_long(temp_blk.nHandle, (long *)&temp_blk.brd_ptr->sampleFIFOcount) == 0)
            ; /* wait for nonzero sample count */

        usleep(10); /* Linux */

        f_cor += (short)input_long(temp_blk.nHandle, (long *)&temp_blk.brd_ptr->sampleFIFO);
    }

    f_cor /= (double)i;

    f_cor += (f_cor < 0.0) ? -0.5 : 0.5; /* round */

    *data = (unsigned short)f_cor;
}

/*
{+D}
    SYSTEM:	    Library Software

    MODULE NAME:    convert AP323

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This acquires the input conversions for the board.

    CALLING
    SEQUENCE:   convert(ptr);
            where:
            ptr (pointer to structure)
                Pointer to the configuration block structure.

    MODULE TYPE:    int

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

void convertAP323(struct cblk323 *c_blk)
{

    /*
        declare local storage
    */

    uint32_t target_count;

    /*
        ENTRY POINT OF ROUTINE:
    */

    cnfgAP323(c_blk); /* configure the board */

    if (c_blk->scan_mode == DISABLE) /* stop scan request */
        return;

    if (c_blk->int_mode == INT_DIS) /* interrupts disabled */
    {
        /* Get the number of samples configured from the FIFO scan list counter into target_count */
        target_count = input_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->scn_lst_cnt);

        /* If the trigger direction is set to output or the scan mode is external */
        /* trigger only, the start conversion register is used to start the scan */
        if (c_blk->trigger == TO_SELECT || c_blk->scan_mode == EX_TRIG)
            output_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->trigFIFOclear, START_CONV); /* start scan */

        /* Non-interrupt driven scan modes just poll for the end of scan, then the data is moved from the */
        /* board to the raw data array; wait until new channel data is present, wait for target count samples */
        while ((uint32_t)input_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->sampleFIFOcount) < target_count)
            ;
    }
    else
    {
        //printf("blocking_start_convert wait...");
        fflush(stdout);
        /* Interrupt driven scan mode will block until the end of scan */
        /* then the data is moved from the board to the raw data array */
        if (c_blk->trigger == TO_SELECT || c_blk->scan_mode == EX_TRIG) /* start scan using a 32 bit write then block */
            APBlockingStartConvert(c_blk->nHandle, (long *)&c_blk->brd_ptr->trigFIFOclear, (long)START_CONV, (long)2);

        //printf(" done!\n");
    }
    /* move data */
    move_dataAP323(c_blk);
}

/*
{+D}
    SYSTEM:         Library Software - AP323 Board

    MODULE NAME:    move_data AP323

    VERSION:        A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This acquires the input conversions for the AP341 board.

    CALLING
    SEQUENCE:   move_dataAP323(ptr);
            where:
            ptr (pointer to structure)
                Pointer to the configuration block structure.

    MODULE TYPE:    int

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

void move_dataAP323(struct cblk323 *c_blk)

{

    /*
        declare local storage
    */

    int i, j, sample_count;
    uint32_t temp_data;

    /*
        ENTRY POINT OF ROUTINE:
        Move data to array
    */

    memset(&c_blk->s_count[0], 0, sizeof(c_blk->s_count)); /* clear channel data sample counters */

    sample_count = input_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->sampleFIFOcount);

    if (sample_count > SA_SIZE)
        sample_count = SA_SIZE;

    for (j = 0; j < sample_count; j++) /* while < sample_count get data */
    {
        temp_data = input_long(c_blk->nHandle, (long *)&c_blk->brd_ptr->sampleFIFO); /* read from board */

        i = (int)((temp_data & 0x002F0000) >> 16); /* get channel number from data */

        if (c_blk->s_count[i] >= SA_SIZE)
            c_blk->s_count[i] = 0;

        c_blk->s_raw_buf[i][c_blk->s_count[i]++] = (unsigned short)temp_data;
    }
}
