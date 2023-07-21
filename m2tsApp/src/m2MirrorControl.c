#include "m2MirrorControl.h"

CircularBuffer MCT1_CB;
double ap323Samples[1024];
int ap323RawSamples[1024];

EPICSTHREADFUNC M2MirrorControlT1()
{

    initializeBuffer(&MCT1_CB, 1024, "MCTest1");

    for (;;)
    {

        epicsEventMustWait(mcDataReadySem);

        for (int i=0; i<1024; i++) {
            writeValue(&MCT1_CB, (int)ap323RawSamples[i]);
        }
        //epicsThreadSleep(0.00025);
    }

    destroyBuffer(&MCT1_CB);
}


void startMCLoopT1()
{
    mcDataReadySem = epicsEventMustCreate(epicsEventEmpty);
    MirrorControlT1Id = epicsThreadCreate("M2MirrorControlT1",
                                          96, epicsThreadGetStackSize(epicsThreadStackMedium),
                                          (EPICSTHREADFUNC)M2MirrorControlT1, NULL);
}

void MCPrintBufferT1()
{

    printBuffer(&MCT1_CB);
}