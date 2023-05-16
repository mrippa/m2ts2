#ifndef M2AP323_H
#define M2AP323_H

#include "apCommon.h"
#include "AP323.h"

#if 0
#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>
#endif

#define NUM_AP323_CARDS 2

typedef struct /* AP323Card */
{
    int hstatus;               /* returned status */
    long addr;                 /* holds board address */
    int i, j;                  /* loop index */
    double s;                  /* span value */
    double z;                  /* zero value */
    int hflag;                 /* interrupt handler installed flag */
    struct cblk323 c_block323; /* configuration block */
    int ap_instance323;
    int adc_running; /* AP 323 is running in continuous acquisition mode*/

    epicsThreadId		AP323RunLoopTaskId;

} AP323Card;

static AP323Card drvAP323Card[NUM_AP323_CARDS];



/* ***
*  EPICS IOCSH Definitions for AP323
*
*/
int M2ReadStatAP323(void);
int M2AcqStartAndShow();
int M2ReadAP323(double *val);
void M2AcqAP323_runOnce();
void M2AcqAP323_show(int channel_number);


#endif