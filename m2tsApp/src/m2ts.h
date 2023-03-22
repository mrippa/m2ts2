#ifndef M2TS_H
#define M2TS_H

#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>

#include <stdio.h>
#include <string.h>

int InitAP323(void );
int InitAP236(void );
int InitAP471(void );
int InitAP48x(void );

int M2ReadStatAP323(void);
int M2AcqStartAndShow();
int M2ReadAP323(double *val);
void M2AcqAP323_runOnce();
void M2AcqAP323_show(int channel_number);

int write_AP236out (double myvolts);
epicsThreadId		RunLoopTaskId;
#endif