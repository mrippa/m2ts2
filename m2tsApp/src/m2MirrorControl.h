#ifndef M2MC_H
#define M2MC_H

#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>
#include <epicsTime.h>

#include "CircularBuffer.h"

epicsThreadId		MirrorControlT1Id;
epicsEventId        mcDataReadySem;
extern CircularBuffer MCT1_CB;
extern double ap323Samples[1024];

void startMCLoopT1();
void MCPrintBufferT1();



#endif