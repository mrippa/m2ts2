#include "m2MirrorControl.h"

EPICSTHREADFUNC M2MirrorControlT1() {

    volatile int reg1 = 0;
    for (;;) {

        reg1++;
        if (reg1 == 1e10) reg1 = 0;

        epicsThreadSleep(0.005);
    }


}

void startMCLoopT1()
{

    MirrorControlT1Id = epicsThreadCreate("M2MirrorControlT1",
                                          96, epicsThreadGetStackSize(epicsThreadStackMedium),
                                          (EPICSTHREADFUNC)M2MirrorControlT1, NULL);
}
