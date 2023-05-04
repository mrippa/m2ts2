#ifndef M2AP471_H
#define M2AP471_H

#include "apCommon.h"
#include "AP471.h"

char cmd_buff[40];         /* command line input buffer */
int item;                  /* menu item selection variable */
long status;               /* returned status of driver routines */
APSTATUS hstatus;          /* interrupt handler returned status */
long addr;                 /* long to hold board address */
int point;                 /* I/O point number */
int port;                  /* I/O port number */
int val;                   /* value of port or point */
int hflag;                 /* interrupt handler installed flag */
struct cblk471 c_block471; /* configuration block */
struct sblk471 s_block471; /* allocate status param. blk */
pthread_t IHandler;        /* thread variable */
int ap_instance471 = 0;

#endif