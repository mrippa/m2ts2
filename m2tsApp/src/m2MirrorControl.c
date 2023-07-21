#include "m2MirrorControl.h"

CircularBuffer MCT1_CB;
CircularBuffer MCT1_ScanTimerCB;

double ap323Samples[1024];
int ap323RawSamples[1024];

EPICSTHREADFUNC M2MirrorControlT1()
{

    initializeBuffer(&MCT1_CB, 1024, "MCTest1");
    initializeBuffer(&MCT1_ScanTimerCB, 100, "MCTest1");
    
    static int j = 0;
    for (;;)
    {

        epicsEventMustWait(mcDataReadySem);
        if (j==100) j=0;

        for (int i=0; i<1024; i++) {
            writeValue(&MCT1_CB, (int)ap323Samples[i]);
        }
        writeValue(&MCT1_ScanTimerCB, j++);
        //epicsThreadSleep(0.00025);
    }

    destroyBuffer(&MCT1_CB);
    destroyBuffer(&MCT1_ScanTimerCB);
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
    printBuffer(&MCT1_ScanTimerCB);
}