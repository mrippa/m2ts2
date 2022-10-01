#ifndef M2AP323_H
#define M2AP323_H

#include "apCommon.h"
#include "AP323.h"

unsigned finished;         /* flag to exit program */
int hstatus;               /* returned status */
long addr;                 /* holds board address */
int i, j;                  /* loop index */
double s;                  /* span value */
double z;                  /* zero value */
int hflag;                 /* interrupt handler installed flag */
struct cblk323 c_block323; /* configuration block */
int ap_instance = 1;
int adc_running;            /* AP 323 is running in continuous acquisition mode*/


#endif