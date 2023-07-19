#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>
#include <epicsTime.h>


epicsThreadId		MirrorControlT1Id;

void startMCLoopT1();
