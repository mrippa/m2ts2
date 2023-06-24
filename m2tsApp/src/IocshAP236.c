#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"
extern AP236Card m2tsAP236Card[NUM_AP236_CARDS];

static int AP236ShowChannel(int cardNumber, int mychannel)
{
    AP236Card *p236Card = &m2tsAP236Card[cardNumber];

    if (!p236Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    printf("\n\nConfiguration Parameters for Channel %X\n\n", mychannel);
    printf("Board Pointer:	%lX\n", (unsigned long) (p236Card->c_block.brd_ptr));
    printf("Parameter Mask:     %X\n", p236Card->c_block.opts.chan[mychannel].ParameterMask);
    printf("Output Update Mode: %X\n", p236Card->c_block.opts.chan[mychannel].UpdateMode);
    printf("Output Range:       %X\n", p236Card->c_block.opts.chan[mychannel].Range);
    printf("Power-up Voltage:   %X\n", p236Card->c_block.opts.chan[mychannel].PowerUpVoltage);
    printf("Thermal Shutdown:   %X\n", p236Card->c_block.opts.chan[mychannel].ThermalShutdown);
    printf("5%% Overrange:       %X\n", p236Card->c_block.opts.chan[mychannel].OverRange);
    printf("Clear Voltage:      %X\n", p236Card->c_block.opts.chan[mychannel].ClearVoltage);
    printf("Data Reset:         %X\n", p236Card->c_block.opts.chan[mychannel].DataReset);
    printf("Full Device Reset:  %X\n", p236Card->c_block.opts.chan[mychannel].FullReset);

    return 0;
}

int AP236WriteOutput(int cardNumber, int channelNumber, double outvolts)
{

    AP236Card *p236Card = &m2tsAP236Card[cardNumber];
    int range;

    if (!p236Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    /* Write Corrected Data To Output */
    if (!p236Card->c_block.bInitialized)
        printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
    else
    {
        /* get current channels range setting */
        range = (int)(p236Card->c_block.opts.chan[channelNumber].Range & 0x7);

        if (outvolts >= (*p236Card->c_block.pIdealCode)[range][ENDPOINTLO] &&
            outvolts <= (*p236Card->c_block.pIdealCode)[range][ENDPOINTHI])
        {
            cd236(&p236Card->c_block, channelNumber, outvolts); /* correct data for channel */
            wro236(&p236Card->c_block, channelNumber, (word)(p236Card->c_block.cor_buf[channelNumber]));
        }
        else
            printf("\n >>> Voltage Out of Range! <<<\n");
    }

    // printf("Wrote %f volts to channel 5\n", myvolts);

    return 0;
}

/* AP236WriteOutput */
static const iocshArg AP236WriteOutputArg0 = {"cardNumber", iocshArgInt};
static const iocshArg AP236WriteOutputArg1 = {"channelNumber", iocshArgInt};
static const iocshArg AP236WriteOutputArg2 = {"volts out", iocshArgDouble};
static const iocshArg *AP236WriteOutputArgs[] = {&AP236WriteOutputArg0, &AP236WriteOutputArg1, &AP236WriteOutputArg2};
static const iocshFuncDef AP236WriteOutputFuncDef = {"AP236WriteOutput", 3, AP236WriteOutputArgs};

/* Wrapper called by iocsh, selects the argument types that initM2TS needs */
static void AP236WriteOutputCallFunc(const iocshArgBuf *args)
{
    AP236WriteOutput(args[0].ival, args[1].ival, args[2].dval);
}

/*AP236ShowChannel*/
static const iocshArg     AP236ShowChannelArg0 = {"card", iocshArgInt};
static const iocshArg     AP236ShowChannelArg1 = {"channel", iocshArgInt};
static const iocshArg    *AP236ShowChannelArgs[] = {&AP236ShowChannelArg0, &AP236ShowChannelArg1};
static const iocshFuncDef AP236ShowChannelFuncDef = {"AP236ShowChannel", 2, AP236ShowChannelArgs};

static void AP236ShowChannelFunc(const iocshArgBuf *args) {
    AP236ShowChannel(args[0].ival, args[1].ival);
}

/* AP236 Utility Registration*/
static void AP236UtilityRegister(void)
{
    iocshRegister(&AP236WriteOutputFuncDef, AP236WriteOutputCallFunc);
    iocshRegister(&AP236ShowChannelFuncDef, AP236ShowChannelFunc);
}

epicsExportRegistrar(AP236UtilityRegister);
