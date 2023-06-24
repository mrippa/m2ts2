#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"
extern AP236Card m2tsAP236Card[NUM_AP236_CARDS];

int M2TSConfigAP236(int cardNumber) {

    APSTATUS status = 0;
    AP236Card *p236Card = &m2tsAP236Card[cardNumber];

    if (!p236Card->c_block.bInitialized)
    {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    printf("Config AP236 Start! 0x%x\n", status);

    /*Setup 236 on channel 5*/
    p236Card->c_block.opts.chan[5].ParameterMask = 0xff;
    p236Card->c_block.opts.chan[5].UpdateMode = 0;      /*Transparent mode*/
    p236Card->c_block.opts.chan[5].Range = 0;           /* +/-10Volts*/
    p236Card->c_block.opts.chan[5].PowerUpVoltage = 1;  /*Mid Scale*/
    p236Card->c_block.opts.chan[5].ThermalShutdown = 0; /*Disabled*/
    p236Card->c_block.opts.chan[5].OverRange = 0;       /*Disabled*/
    p236Card->c_block.opts.chan[5].ClearVoltage = 1;    /*Mid Scale*/
    p236Card->c_block.opts.chan[5].DataReset = 1;       /*Enabled*/
    p236Card->c_block.opts.chan[5].FullReset = 1;       /*Enabled*/

    printf("Init AP236 SETUP new changes and status is: 0x%x\n", status);

    /*Write the configuration to the registers*/
    cnfg236(&(p236Card->c_block), 5); /* configure channel */

    printf("AP236 SETUP written");

    return status;
}

static int showAP236Channel(int cardNumber, int mychannel)
{
    AP236Card *p236Card = &m2tsAP236Card[cardNumber];
    uint32_t value;

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

int write_AP236out(int cardNumber, double myvolts)
{

    AP236Card *p236Card = &m2tsAP236Card[cardNumber];
    uint32_t value;

    if (!p236Card->c_block.bInitialized) {
        printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
        return (ERROR);
    }

    /* Write Corrected Data To Output */
    if (!p236Card->c_block.bInitialized)
        printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
    else
    {
        /* get current channels range setting */
        p236Card->range = (int)(p236Card->c_block..opts.chan[5].Range & 0x7);

        if (myvolts >= (*p236Card->c_block..pIdealCode)[range][ENDPOINTLO] &&
            myvolts <= (*p236Card->c_block..pIdealCode)[range][ENDPOINTHI])
        {
            cd236(&p236Card->c_block., 5, myvolts); /* correct data for channel */
            wro236(&p236Card->c_block., 5, (word)(p236Card->c_block..cor_buf[5]));
        }
        else
            printf("\n >>> Voltage Out of Range! <<<\n");
    }

    // printf("Wrote %f volts to channel 5\n", myvolts);

    return 0;
}

/*showAP236Channel*/
static const iocshArg     showAP236ChannelArg0 = {"channel", iocshArgInt};
static const iocshArg    *showAP236ChannelArgs[] = {&showAP236ChannelArg0};
static const iocshFuncDef showAP236ChannelFuncDef = {"showAP236Channel", 1, showAP236ChannelArgs};

static void showAP236ChannelFunc(const iocshArgBuf *args) {
    showAP236Channel(args[0].ival);
}

static void showAP236ChannelRegister(void) {
    iocshRegister(&showAP236ChannelFuncDef, showAP236ChannelFunc);
}


/* writeAP236out */
static const iocshArg writeAP236Arg0 = {"volts out", iocshArgDouble};
static const iocshArg *writeAP236Args[] = {&writeAP236Arg0};
static const iocshFuncDef writeAP236FuncDef = {"write_AP236out", 1, writeAP236Args};

/* Wrapper called by iocsh, selects the argument types that initM2TS needs */
static void writeAP236CallFunc(const iocshArgBuf *args)
{
    write_AP236out(args[0].dval);
}

/* Registration routine, runs at startup */
static void writeAP236Register(void)
{
    iocshRegister(&writeAP236FuncDef, writeAP236CallFunc);
}

epicsExportRegistrar(writeAP236Register);
epicsExportRegistrar(showAP236ChannelRegister);
