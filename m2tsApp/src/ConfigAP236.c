#include "m2ts236.h"

extern AP236Card m2tsAP236Card[NUM_AP236_CARDS];

static int M2TSConfigAP236(int cardNumber) {

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

