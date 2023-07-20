#ifndef M2MC_H
#define M2MC_H

#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>
#include <epicsTime.h>

#include "CircularBuffer.h"

epicsThreadId		MirrorControlT1Id;
extern CircularBuffer MCT1_CB;
void startMCLoopT1();
void MCPrintBufferT1();

#endif