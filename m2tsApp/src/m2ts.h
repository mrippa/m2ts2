#ifndef M2TS_H
#define M2TS_H


#include <stdio.h>
#include <string.h>

#include <epicsEvent.h>
#include <epicsRingPointer.h>
#include <epicsThread.h>
#include <callback.h>

int InitAP323(void );
int InitAP236(void );
int InitAP471(void );
int InitAP48x(void );


#endif