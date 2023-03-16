#include <epicsExport.h>
#include <iocsh.h>

#include "m2ts236.h"
#include "m2ts.h"

/* Ideal Zero SB, BTC, Slope, endpoint, and clip constants
   ranges[8], ideal straight binary is [0], ideal 2'Comp is [1], slope is [2], 
   endpoint low is [3], endpoint high is [4], clip low is [5] clip high is [6]
*/

double IdealCode[8][7] =
{
  /* IdealZeroSB, IdealZeroBTC, IdealSlope, -10 to 10V, cliplo, cliphi */
  { 32768.0, 0.0, 3276.8, -10.0, 10.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope,   0 to 10V, cliplo, cliphi */
  { 0.0, -32768.0, 6553.6, 0.0, 10.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope,  -5 to  5V, cliplo, cliphi */
  { 32768.0, 0.0, 6553.6, -5.0, 5.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope,   0 to  5V, cliplo, cliphi */
  { 0.0, -32768.0, 13107.2, 0.0, 5.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope, -10 to 10V, cliplo, cliphi */
  { 32768.0, 0.0, 3276.8, -10.0, 10.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope,  -3 to  3V, cliplo, cliphi */
  { 32768.0 ,0.0, 10922.67, -3.0, 3.0, -32768.0, 32767.0},

  /* IdealZeroSB, IdealZeroBTC, IdealSlope, 0V to +16V, cliplo, cliphi */
  { 0.0, -32768.0, 4095.9, 0.0, 16.0, -32768.0, 32767.0 },

  /* IdealZeroSB, IdealZeroBTC, IdealSlope, 0V to +20V, cliplo, cliphi */
  { 0.0, -32768.0, 3276.8, 0.0, 20.0, -32768.0, 32767.0 },
};

static int show_AP236Channel(struct cblk236 *c_blk, int mychannel)
{

    printf("\n\nConfiguration Parameters for Channel %X\n\n", mychannel);
    printf(" 1. Return to Previous Menu\n");
    printf(" 2. Board Pointer:	%lX\n", (unsigned long)c_blk->brd_ptr);
    printf(" 3. Parameter Mask:     %X\n", c_blk->opts.chan[mychannel].ParameterMask);
    printf(" 4. Output Update Mode: %X\n", c_blk->opts.chan[mychannel].UpdateMode);
    printf(" 5. Output Range:       %X\n", c_blk->opts.chan[mychannel].Range);
    printf(" 6. Power-up Voltage:   %X\n", c_blk->opts.chan[mychannel].PowerUpVoltage);
    printf(" 7. Thermal Shutdown:   %X\n", c_blk->opts.chan[mychannel].ThermalShutdown);
    printf(" 8. 5%% Overrange:       %X\n", c_blk->opts.chan[mychannel].OverRange);
    printf(" 9. Clear Voltage:      %X\n", c_blk->opts.chan[mychannel].ClearVoltage);
    printf("10. Data Reset:         %X\n", c_blk->opts.chan[mychannel].DataReset);
    printf("11. Full Device Reset:  %X\n", c_blk->opts.chan[mychannel].FullReset);
}

static int write_AP236out (double myvolts)
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
          cd236(&c_block236, 5, Volts); /* correct data for channel */
          wro236(&c_block236, 5, (word)(c_block236.cor_buf[5]));
      }
      else
        printf("\n >>> Voltage Out of Range! <<<\n");
    }

    printf("Wrote %f volts to channel 5\n", myvolts);

    return 0;
}

int InitAP236(void ) {
    
    
    APSTATUS status = 0;

    finished2 = finished = 0;	/* indicate not finished */
    range = 0;
    memset(&c_block236, 0, sizeof(c_block236));

    /* Put the address of the initialized constant array into the configuration block structure */
    c_block236.pIdealCode = &IdealCode;	/* pointer to Ideal Zero straight binary, and Slope constants */

/*
    Initialize the Configuration Parameter Block to default values.
*/

    c_block236.bAP = FALSE;		/* indicate not initialized and set up yet */
    c_block236.bInitialized = FALSE;	/* indicate not ready */
    c_block236.nHandle = 0;		/* make handle to a closed board */

/*
	Initialize the AP library
*/
    if(InitAPLib() != S_OK)
    {
	printf("\nUnable to initialize the AP library. Exiting program.\n");
	exit(0);
    }

/*
	Open an instance of a AP device
	Other device instances can be obtained
	by changing parameter 1 of APOpen()
*/
    status = APOpen(ap_instance236, &c_block236.nHandle, DEVICE_NAME );

    if( status != S_OK)
    {
	  printf("\nUnable to Open instance of AP236.\n");
	  finished = 1;	 /* indicate finished with program */
    }
    else
    {
      if (APInitialize(c_block236.nHandle) == S_OK) /* Initialize */
      {
          GetAPAddress(c_block236.nHandle, &addr); /* Read back address */
          c_block236.brd_ptr = (struct map236 *)addr;
          c_block236.bInitialized = TRUE;
          c_block236.bAP = TRUE;
      }
    }

    /*Setup 236 on channel 5*/
	c_block236.opts.chan[5].ParameterMask      = 0xff;
    c_block236.opts.chan[5].UpdateMode         = 0;   /*Transparent mode*/
    c_block236.opts.chan[5].Range              = 0;  /* +/-10Volts*/
    c_block236.opts.chan[5].PowerUpVoltage     = 1;   /*Mid Scale*/
    c_block236.opts.chan[5].ThermalShutdown    = 0;   /*Disabled*/
    c_block236.opts.chan[5].OverRange          = 0;   /*Disabled*/
    c_block236.opts.chan[5].ClearVoltage       = 1;   /*Mid Scale*/
    c_block236.opts.chan[5].DataReset          = 1;   /*Enabled*/
    c_block236.opts.chan[5].FullReset          = 1;   /*Enabled*/
 
    printf("Init AP236 SETUP new changes and status is: 0x%x\n", status);
    
    /*Write the configuration to the registers*/
    if(!c_block236.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	else
		cnfg236(&c_block236, 5); /* configure channel */


    show_AP236Channel(&c_block236, 5);

    return status;
}



/* Information needed by iocsh */
static const iocshArg     writeAP236Arg0 = {"volts out", iocshArgDouble};
static const iocshArg    *writeAP236Args[] = {&writeAP236Arg0};
static const iocshFuncDef writeAP236FuncDef = {"write_AP236out", 1, writeAP236Args};

/* Wrapper called by iocsh, selects the argument types that initM2TS needs */
static void writeAP236CallFunc(const iocshArgBuf *args) {
    write_AP236out(args[0].dval);
}

/* Registration routine, runs at startup */
static void writeAP236Register(void) {
    iocshRegister(&writeAP236FuncDef, writeAP236CallFunc);
}

epicsExportRegistrar(writeAP236Register);