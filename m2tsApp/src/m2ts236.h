#ifndef M2AP236_H
#define M2AP236_H

#include "apCommon.h"
#include "AP236.h"
#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>

#define NUM_AP236_CARDS 1

typedef struct /* AP236Card */
{
    int card;               /* AP Card instance*/
    int initialized;        /* Is this card initialized*/
    int configured;         /* Is this card configured*/    
    int hstatus;            /* returned status */
    long addr;              /* holds board address */
    int range;
    int temp;
    double Volts, ideal;
    double zero, span, slope, gcoef, ocoef; /* storage for coefficient calibration */
    struct cblk236 c_block;              /* configuration block */
    int current_channel;
} AP236Card;

extern AP236Card m2tsAP236Card[NUM_AP236_CARDS];


#endif