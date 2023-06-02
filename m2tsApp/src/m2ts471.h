#ifndef M2AP471_H
#define M2AP471_H

#include "apCommon.h"
#include "AP471.h"
#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>

#define NUM_AP471_CARDS 1
typedef struct /* AP471Card */

{

    int card;                  /* AP Card instance*/
    int initialized;           /* Is this card initialized*/
    int configured;            /* Is this card configured*/    
    char cmd_buff[40];         /* command line input buffer */
    int item;                  /* menu item selection variable */
    long status;               /* returned status of driver routines */
    APSTATUS hstatus;          /* interrupt handler returned status */
    long addr;                 /* long to hold board address */
    int hflag;                 /* interrupt handler installed flag */
    struct cblk471 c_block; /* configuration block */
    struct sblk471 s_block; /* allocate status param. blk */
    pthread_t IHandler;        /* thread variable */

} AP471Card;

static AP471Card m2tsAP471Card[NUM_AP471_CARDS];

#endif