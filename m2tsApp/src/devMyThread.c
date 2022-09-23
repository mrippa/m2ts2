#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>

#include "mythread.h"
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


static void M2AcqAP323_show(int channel_number)
{

    if (!c_block323.bInitialized)
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
    else
    {
        for (i = 0; i <= channel_number; i++)
        {
            printf("ch %d: %12.6f volts\n", i, ((((double)c_block323.s_cor_buf[0][i]) * 20.0) / (double)65536.0) + (-10.0));
        }
    }
}

/* M2Acquire
 *
 *
 */
static void* M2AcqAP323_runOnce()
{

    if(adc_running)
    {
        //printf("\n>>>ERROR: thread called with ADC Running\n");
        handle_error("ADC Running");
    }
    adc_running = 1;

    if (!c_block323.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        handle_error("ADC BOARD"); 
    }

    calibrateAP323(&c_block323, AZ_SELECT);  /* get auto-zero values */
    calibrateAP323(&c_block323, CAL_SELECT); /* get calibration values */

    if (hflag == 0 && c_block323.int_mode != 0)
    {
        handle_error("ADC NO_INT");
    }

    //printf("Start M2AcqAP323_run\n");
    
    convertAP323(&c_block323); /* convert the board */
    mccdAP323(&c_block323); /* correct input data */

    adc_running = 0;
    //printf("End M2AcqAP323_run\n");

    return NULL;
}


int M2AcqStart() {

    int i = 0;

    for (i=0; i<50; i++){
        M2AcqAP323_runOnce();
        M2AcqAP323_show(0);
    }
    printf("M2AcqStart finished\n");

    return(0);
}
