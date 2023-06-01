#include "epicsTime.h"

void m2timeDiffInUS (EpicsTimer *et) {

    epicsTimeGetCurrent(&current_timestamp);

    diff_us = epicsTimeDiffInUS(&current_timestamp, &(buffer->timestamp));
}