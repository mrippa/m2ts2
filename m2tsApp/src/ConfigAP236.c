#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"

int ConfigAP236(void) {

    APSTATUS status = 0;

    printf("Config AP236 Start! 0x%x\n", status);

    /*Setup 236 on channel 5*/
    c_block236.opts.chan[5].ParameterMask = 0xff;
    c_block236.opts.chan[5].UpdateMode = 0;      /*Transparent mode*/
    c_block236.opts.chan[5].Range = 0;           /* +/-10Volts*/
    c_block236.opts.chan[5].PowerUpVoltage = 1;  /*Mid Scale*/
    c_block236.opts.chan[5].ThermalShutdown = 0; /*Disabled*/
    c_block236.opts.chan[5].OverRange = 0;       /*Disabled*/
    c_block236.opts.chan[5].ClearVoltage = 1;    /*Mid Scale*/
    c_block236.opts.chan[5].DataReset = 1;       /*Enabled*/
    c_block236.opts.chan[5].FullReset = 1;       /*Enabled*/

    printf("Init AP236 SETUP new changes and status is: 0x%x\n", status);

    /*Write the configuration to the registers*/
    if (!c_block236.bInitialized)
        printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
    else
        cnfg236(&c_block236, 5); /* configure channel */

    printf("AP236 SETUP written");

    return status;
}

static int showAP236Channel(int mychannel)
{

    struct cblk236 *c_blk;
    c_blk = &c_block236; /* Get access to the global c_block471 structure*/


    printf("\n\nConfiguration Parameters for Channel %X\n\n", mychannel);
    printf("Board Pointer:	%lX\n", (unsigned long)c_blk->brd_ptr);
    printf("Parameter Mask:     %X\n", c_blk->opts.chan[mychannel].ParameterMask);
    printf("Output Update Mode: %X\n", c_blk->opts.chan[mychannel].UpdateMode);
    printf("Output Range:       %X\n", c_blk->opts.chan[mychannel].Range);
    printf("Power-up Voltage:   %X\n", c_blk->opts.chan[mychannel].PowerUpVoltage);
    printf("Thermal Shutdown:   %X\n", c_blk->opts.chan[mychannel].ThermalShutdown);
    printf("5%% Overrange:       %X\n", c_blk->opts.chan[mychannel].OverRange);
    printf("Clear Voltage:      %X\n", c_blk->opts.chan[mychannel].ClearVoltage);
    printf("Data Reset:         %X\n", c_blk->opts.chan[mychannel].DataReset);
    printf("Full Device Reset:  %X\n", c_blk->opts.chan[mychannel].FullReset);

    return 0;
}

int write_AP236out(double myvolts)
{
    /* Write Corrected Data To Output */
    if (!c_block236.bInitialized)
        printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
    else
    {
        /* get current channels range setting */
        range = (int)(c_block236.opts.chan[5].Range & 0x7);

        if (myvolts >= (*c_block236.pIdealCode)[range][ENDPOINTLO] &&
            myvolts <= (*c_block236.pIdealCode)[range][ENDPOINTHI])
        {
            cd236(&c_block236, 5, myvolts); /* correct data for channel */
            wro236(&c_block236, 5, (word)(c_block236.cor_buf[5]));
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
