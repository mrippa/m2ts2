#ifndef M2AP323_H
#define M2AP323_H

#include "apCommon.h"
#include "AP323.h"
#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>
#include <epicsTime.h>

#define NUM_AP323_CARDS 2

typedef struct /* AP323Card */
{
    int card;                  /* AP Card instance*/
    int initialized;           /* Is this card initialized*/
    int configured;            /* Is this card configured*/    
    int hstatus;               /* returned status */
    long addr;                 /* holds board address */
    double s;                  /* span value */
    double z;                  /* zero value */
    int hflag;                 /* interrupt handler installed flag */
    struct cblk323 c_block; /* configuration block */
    int adc_running;           /* AP 323 is running in continuous acquisition mode*/
    int cal_autozero_complete;    /* Calibration autozero complete flag */
    int cal_select_complete;      /* Calibration valus complete flag */


    epicsEventId	    acqSem;
    epicsThreadId		AP323RunLoopTaskId;
    int cor_data[SA_CHANS][SA_SIZE];            /* allocate  corrected data storage area */
    unsigned short raw_data[SA_CHANS][SA_SIZE]; /* allocate raw data storage area */
    byte s_array[1024];                         /* input channel scan array */

} AP323Card;

static AP323Card m2tsAP323Card[NUM_AP323_CARDS];

/* ***
*  EPICS IOCSH Definitions for AP323
*
*/
int M2ReadStatusAP323(int cardNumber);
int M2AcqStartAndShow();
int M2ReadAP323(int cardNumber, int channel_number, double *val);
void M2AcqAP323_runOnce(int cardNumber);
void M2AcqAP323_show(int cardNumber, int channel_number);
void PrintBuffer(void);

#endif