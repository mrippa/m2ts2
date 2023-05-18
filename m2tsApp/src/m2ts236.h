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

    long addr; /* holds board address */
    int range;
    int temp;
    double Volts, ideal;
    double zero, span, slope, gcoef, ocoef; /* storage for coefficient calibration */
    struct cblk236 c_block236;              /* configuration block */
    int ap_instance236;
    int current_channel;
} AP236Card;

static AP236Card m2tsAP236Card[NUM_AP236_CARDS];

int write_AP236out(double myvolts);

#endif