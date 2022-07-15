
/*
{+D}
    SYSTEM:         Software for AP48X
    
    FILE NAME:      AP48X.h

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    F.M.

    CODED BY:	    F.M.

    ABSTRACT:	    This module contains the definitions and structures
                    used by the library.

    CALLING
	SEQUENCE:

    MODULE TYPE:    header file

    I/O RESOURCES:

    SYSTEM
	RESOURCES:

    MODULES
	CALLED:

    REVISIONS:

DATE	 BY	PURPOSE
-------  ----	------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:

    This module contains the definitions and structures used by the library.
*/

/*
    DEFINITIONS:
*/

/* //////////////////////////////////////////////////// */
/* Select board type, and device name by selecting      */
/* the defines below that correspond to your hardware.  */
/* Default is AP482.                                    */
/* A change here requires the complete rebuild of the   */
/* demo application as well as the kernel code.         */
/* May also require changes to the kernel code makefile.*/
/* //////////////////////////////////////////////////// */


/*
	Board device names

	Select one of the supported board types below by uncommenting
	its corresponding define to enable access to that board type.
*/

#define AP482		1
//#define AP483		2
//#define AP484		3


/*
	AP482 board type
*/

#ifdef AP482
#define AP48X (word)0x7042          /* AP482 device ID */
#define DEVICE_NAME "ap482_"        /* name of device */
#define MAX_CNTR	10          /* 10 counters */
#endif /* AP482 */


/*
	AP483 board type
*/

#ifdef AP483
#define AP48X (word)0x7043         /* AP483 device ID */
#define DEVICE_NAME "ap483_"       /* name of device */
#define MAX_CNTR	8          /* 8 counters */
#endif /* AP483 */


/*
	AP484 board type
*/

#ifdef AP484
#define AP48X (word)0x7044         /* AP484 device ID */
#define DEVICE_NAME "ap484_"       /* name of device */
#define MAX_CNTR	6          /* 6 counters */
#endif /* AP484 */




typedef enum
{
	Success = 0,
	ParameterOutOfRange = 1,	/* Parameter in error */
	InvalidPointer = 2,		/* Flag NULL pointers */
	DataDirection = 3,		/* data direction error */
	TimeOut = 4			/* time-out error */
} APSTAT;



struct map48x		   /* Memory map of the counter I/O board */
{
    uint32_t GIEPendingStatus;		/* 7:0 Global Interrupt Enable and Pending Status */
    uint32_t LocationRegister;		/* Bits 15:0 Location Register */
    uint32_t InterruptStatusReg;	/* 15:0 Counters Interrupt Status/Clear Register */
    uint32_t CounterInterruptEnable;	/* 15:0 Counter Interrupt Enable/Disable Register */
    uint32_t CounterTrigger;		/* Bits 15:0 Counter Trigger Register */
    uint32_t CounterStop;		/* Bits 15:0 Counter Stop Register */
    uint32_t LoadReadBack;		/* Bits 15:0 Load Read Back Register */
    uint32_t ToggleConstant;		/* Bits 15:0 Toggle Constant Register */

    struct
    {
      uint32_t CounterControl;		/* Bits 15:0 Counter Control Register */
      uint32_t CounterIIR;		/* Bits 15:0 Counter Interrupt Information Register */
      uint32_t CounterReadBack;		/* Bits 31:0 Counter Read Back Register */
      uint32_t CounterConstantA1;	/* Bits 31:0 Counter Constant A Register1 */
      uint32_t CounterConstantA2;	/* Bits 31:0 Counter Constant A Register2 */
      uint32_t CounterConstantB1;	/* Bits 31:0 Counter Constant B Register1 */
      uint32_t CounterConstantB2;	/* Bits 31:0 Counter Constant B Register2 */
      uint32_t Counternotused;
    } Counter[10];			/* must stay at 10 for structure/register alignment */

    uint32_t DigitalInput;		/* Bits 15:0 Digital Input Register */	
    uint32_t DigitalOut;		/* Bits 15:0 Digital Output Register */	
    uint32_t XDAC_StatusControl;	/* Bits 15:0 XADC Status/Control Register */	
    uint32_t XDAC_AddressReg;		/* Bits 15:0 XADC Address Register */
    unsigned char Reserved[0x90];
    uint32_t FirmwareRevision;		/* 31:0 */
};



/*
	Mode and option selection enumerations
*/

enum
{
	None	  = 0,          /* disable counter */
	QuadPosition  = 1,      /* quadrature position measurement */
	PulseWidthMod = 2,      /* pulse width modulation */
	Watchdog  = 3,          /* watchdog function */
	EventCounting   = 4,    /* event counting/frequency measurement */
	InPulseWidthMeasure = 5,/* input pulse width measurement */
	InPeriodMeasure  = 6,   /* input period measurement */
	OneShot   = 7,          /* One-Shot output pluse */

	Nosis = 0,		/* No Output or Special Interrupt Selected */

	OutPolLow = 0,          /* output polarity active low */
	OutPolHi  = 1,          /* output polarity active high */

	InABCPolDisabled = 0,	/* disabled */
	InABCPolLow  = 1,       /* input polarity active low */
	InABCPolHi   = 2,       /* input polarity active high */
	InABCUpDown  = 3,       /* up/down */
	InABCGate_off  = 3,     /* gate off */
	InAX4  = 3,
	InCQuadMax  = 4,

	InC1_1953125Mhz = 0,   /* internal 1.953125MHZ clock */

	DebounceOn = 1,		/* Debounce enabled */
	DebounceOff = 0,	/* Debounce disabled */

	IntDisable= 0,          /* disable interrupt */
	IntEnable = 1           /* interrupt enabled */
};




struct ap48x
{
    struct map48x *brd_ptr;			/* base address of the I/O board */
    int nHandle;				/* handle to an open board */
    int counter_num;				/* current counter being serviced */
    ushort location;				/* AP location */
    uint32_t revision;				/* Firmware Revision */
    uint32_t FPGAAdrData[10];			/* FPGA address & data order:0,1,2,20 thru 26 */
    BYTE m_Mode[MAX_CNTR];			/* the counter mode */
    BYTE m_InputAPolarity[MAX_CNTR];		/* input polarity A */
    BYTE m_InputBPolarity[MAX_CNTR];		/* input polarity B */
    BYTE m_InputCPolarity[MAX_CNTR];		/* input polarity C */
    BYTE m_ClockSource[MAX_CNTR];		/* clock source */
    BYTE m_SpecialIC[MAX_CNTR];			/* special interrupt condition */
    BOOL m_OutputPolarity[MAX_CNTR];		/* output polarity */
    BOOL m_Debounce[MAX_CNTR];			/* debounce */
    BOOL m_InterruptEnable[MAX_CNTR];		/* interrupt enable */
    BOOL bAP;					/* flag indicating a board is open */
    BOOL bInitialized;				/* flag indicating ready */	
    uint32_t m_CounterConstantA1[MAX_CNTR];	/* constant registers copies are here */
    uint32_t m_CounterConstantA2[MAX_CNTR];	/* constant registers copies are here */
    uint32_t m_CounterConstantB1[MAX_CNTR];	/* constant registers copies are here */
    uint32_t m_CounterConstantB2[MAX_CNTR];	/* constant registers copies are here */
};



/*
    DECLARE MODULES CALLED:
*/

/* Declare digital I/O functions called */
long rprtdio(struct ap48x *c_blk, uint32_t port); 	/* routine to read the input port */
long rpntdio(struct ap48x *c_blk, uint32_t port , uint32_t point);	/* routine to read a input point */
long wpntdio(struct ap48x *c_blk, unsigned point, unsigned value);     /* routine to write to a output point */
long wprtdio(struct ap48x *c_blk, unsigned value);	/* routine to write to the output port */

/* ************************** End of Digital I/O section defines */



/* ************************** Start of Counter/Timer I/O section defines */

void trig48x(struct ap48x *c_blk);		/* routine to software trigger */
void setconf48x(struct ap48x *c_blk);		/* routine to set up the configuration param. block */
void selectcnt48x(struct ap48x *c_blk);		/* routine to examine/change the current counter */
void selectcon48x(struct ap48x *c_blk);		/* routine to examine/change the counter constant */
void psts48x(struct ap48x *c_blk);
void rmid48x(struct ap48x *c_blk);
uint32_t build_control48x(struct ap48x *c_blk, int counter);	/* builds counter control words */
uint32_t read48x_CounterIIR(int nHandle, uint32_t counter);
int BlockingStartCounter48x(struct ap48x *c_blk, int counter);

APSTAT ConfigureCounterTimer(struct ap48x *c_blk, int counter);	/* Configure counter timer */
APSTAT ReadCounter(struct ap48x *c_blk, int counter, uint32_t *val);/* Read counter */
APSTAT WriteCounterConstant( struct ap48x *c_blk, int counter);	/* Write counter constant to board register */
APSTAT ToggleConstant(struct ap48x *c_blk, int counter );
APSTAT ToggleSimultaneousConstants(struct ap48x *c_blk,  word mask );
APSTAT StopCounter(struct ap48x *c_blk, int counter);		/* Stop a counter */
APSTAT DisableInterrupt(struct ap48x *c_blk, int counter);	/* Disable counter interrupts */
APSTAT EnableInterrupt(struct ap48x *c_blk, int counter);
APSTAT GetDebounce(struct ap48x*c_blk, int counter, BOOL *debounce);/* get debounce */
APSTAT SetDebounce(struct ap48x *c_blk, int counter, BOOL debounce);/* get debounce */
APSTAT GetMode(struct ap48x *c_blk, int counter, BYTE *mode);	/* get mode */
APSTAT SetMode(struct ap48x *c_blk, int counter, BYTE mode);	/* set mode */
APSTAT GetInterruptEnable(struct ap48x *c_blk, int counter, BOOL *enable);	/* get interrupt enable */
APSTAT SetInterruptEnable(struct ap48x *c_blk, int counter, BOOL enable);	/* set interrupt enable */
APSTAT GetSpecialInterrupt(struct ap48x *c_blk, int counter, BYTE *special);	/* get special interrupt */
APSTAT SetSpecialInterrupt(struct ap48x *c_blk, int counter, BYTE special);	/* set special interrupt */
APSTAT GetClockSource(struct ap48x *c_blk, int counter, BYTE *source);	/* get clock source */
APSTAT SetClockSource(struct ap48x *c_blk, int counter, BYTE source);	/* set clock source */
APSTAT GetOutputPolarity(struct ap48x *c_blk, int counter, BOOL *polarity);/* get output polarity */
APSTAT SetOutputPolarity(struct ap48x *c_blk, int counter, BOOL polarity);/* set output polarity */
APSTAT GetInputAPolarity(struct ap48x *c_blk, int counter, BYTE *polarity);/* get input A polarity */
APSTAT SetInputAPolarity(struct ap48x *c_blk, int counter, BYTE polarity);/* set input A polarity */
APSTAT GetInputBPolarity(struct ap48x *c_blk, int counter, BYTE *polarity);/* get input B polarity */
APSTAT SetInputBPolarity(struct ap48x *c_blk, int counter, BYTE polarity);/* set input B polarity */
APSTAT GetInputCPolarity(struct ap48x *c_blk, int counter, BYTE *polarity);/* get input C polarity */
APSTAT SetInputCPolarity(struct ap48x *c_blk, int counter, BYTE polarity);/* set input C polarity */
APSTAT StartCounter(struct ap48x *c_blk, int counter);				/* start counter */
APSTAT StartSimultaneousCounters(struct ap48x *c_blk, ushort mask);		/* start simultaneous counters */
APSTAT StopSimultaneousCounters(struct ap48x *c_blk,  ushort mask);		/* stop simultaneous counters */
APSTAT GetCounterConstantAx(struct ap48x *c_blk, int counter, uint32_t *val, int x);	/* get constant */
APSTAT SetCounterConstantAx(struct ap48x *c_blk, int counter, uint32_t val, int x);	/* set constant */
APSTAT GetCounterConstantBx(struct ap48x *c_blk, int counter, uint32_t *val, int x);	/* get constant */
APSTAT SetCounterConstantBx(struct ap48x *c_blk, int counter, uint32_t val, int x);	/* set constant */
APSTAT ReadCounterControl(struct ap48x *c_blk, 	int counter, uint32_t *val);
APSTAT WriteCounterControl(struct ap48x *c_blk, int counter, uint32_t val);

