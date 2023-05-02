#ifndef M2AP236_H
#define M2AP236_H

#include "apCommon.h"
#include "AP236.h"

char cmd_buff[32];            /* command line input buffer */
unsigned finished, finished2; /* flag to exit program */
long addr;                    /* holds board address */
int item;                     /* menu item selection variable */
int i;                        /* loop index */
int range;
int temp;
double Volts, ideal;
double zero, span, slope, gcoef, ocoef; /* storage for coefficient calibration */
struct cblk236 c_block236;              /* configuration block */
int ap_instance236 = 0;
int current_channel = 5;


#endif