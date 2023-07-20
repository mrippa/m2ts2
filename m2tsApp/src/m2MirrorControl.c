#include "m2MirrorControl.h"

CircularBuffer MCT1_CB;

EPICSTHREADFUNC M2MirrorControlT1() {

    volatile int reg1 = 0;
    for (;;) {

        static int direction = 1;

        reg1 += direction;
        if ( reg1 == 1e6 || reg1 == 0)
            direction = -direction;

        //writeValue(&MCT1_CB, (double) reg1);

        epicsThreadSleep(0.00025);
    }


}

void startMCLoopT1()
{
    initializeBuffer( &MCT1_CB, 100, "MCTest1");
    MirrorControlT1Id = epicsThreadCreate("M2MirrorControlT1",
                                          96, epicsThreadGetStackSize(epicsThreadStackMedium),
                                          (EPICSTHREADFUNC)M2MirrorControlT1, NULL);
    destroyBuffer(&MCT1_CB);
}


void MCPrintBufferT1() {

    printBuffer(&MCT1_CB);
}