#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts323.h"

int ConfigAP323(void) {

    APSTATUS status = 0;

    printf("Config AP323 done! 0x%x\n", status);

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
    c_block323.sa_end = &s_array[0];    /* address of end of scan array */

    /*
        Initialize the AP library
    */
    if (InitAPLib() != S_OK)
    {
        printf("\nUnable to initialize the AP library. Exiting program.\n");
        exit(0);
    }

    return status;
}