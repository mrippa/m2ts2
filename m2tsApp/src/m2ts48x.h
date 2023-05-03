#ifndef M2AP48X_H
#define M2AP48X_H

#include "apCommon.h"
#include "AP48X.h"

char cmd_buff[32]; /* command line input buffer */
unsigned finished; /* flag to exit program */
int item;          /* menu item selection variable */
unsigned point;    /* I/O point number */
unsigned val;      /* value of port or point */
APSTATUS hstatus;  /* interrupt handler returned status */
long addr;         /* integer to hold board address */
int i;             /* loop index */
int hflag;         /* interrupt handler installed flag */
uint32_t ulong_value;
struct ap48x c_block48x; /* configuration block */
long status;
int ap_instance48x = 0;

#endif