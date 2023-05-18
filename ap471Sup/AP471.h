
/*
{+D}
    SYSTEM:         AP471 Software

    FILE NAME:      AP471.h

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FM

    CODED BY:       FM

    ABSTRACT:       This module contains the definitions and structures
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

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:

    This module contains the definitions and structures used by
    the library.
*/

/*
    DEFINITIONS:
*/

/*
	AP471 board type
*/

#define APBOARD (word)0x7015        /* AP471 device ID */
#define AP471_DEVICE_NAME "ap471_"        /* name of device */



/*
    Parameter mask bit positions
*/

#define MASK           1        /* write mask registers */
#define EV_ENABLE      2        /* event enable registers */
#define EV_TYPE        4        /* event type registers */
#define EV_POLARITY    8        /* event polarity registers */
#define DEBCLOCK       0x10     /* debounce clock register */
#define DEBCONTROL     0x20     /* debounce control registers */
#define DEBDURATION    0x40     /* debounce duration registers */

/*
   DATA STRUCTURES
*/

/*
    Defined below is the structure which is used to hold the
    board's status information.
*/

struct sblk471
{
  uint32_t MaskRegStat[3];             /* Bits 15:0 */
  uint32_t EventEnableStat[3];         /* Bits 15:0 */
  uint32_t EventTypeStat[3];           /* Bits 15:0 */
  uint32_t EventPolarityStat[3];       /* Bits 15:0 */
  uint32_t EventPendingClrStat[3];     /* Bits 15:0 */
  uint32_t DebounceControlStat[3];     /* Bits 15:0 */
  uint32_t DebounceDurationStat[3];    /* Bits 15:0 */
  uint32_t DebounceClockSelectStat;    /* Bit 0 */
  uint32_t BoardIntEnableStat;         /* board interrupt enable register */
};


/*
    Defined below is the structure which is used to hold the
    board's interrupt information.
*/

struct AP471_isr_info
{
  uint32_t Status;
  uint32_t EventPending[3];	/* board interrupt pending flags */
};


/*
    Defined below is the structure for the boards memory map.
*/

struct map471
{
  uint32_t InterruptRegister;		/* Bits 15:0 */
  uint32_t LocationRegister;		/* Bits 15:0 */
  uint32_t IORegister[3];		/* Bits 15:0 */
  uint32_t WriteMaskReg[3];             /* Bits 15:0 */
  uint32_t EventEnableReg[3];           /* Bits 15:0 */
  uint32_t EventTypeReg[3];             /* Bits 15:0 */
  uint32_t EventPolarityReg[3];         /* Bits 15:0 */
  uint32_t EventPendingClrReg[3];	/* Bits 15:0 */
  uint32_t DebounceControl[3];		/* Bits 15:0 */
  uint32_t DebounceDuration[3];		/* Bits 15:0 */
  uint32_t DebounceClockSelect;		/* Bit 0 */
  uint32_t SoftwareReset;		/* Bit 0 */
  uint32_t XDAC_StatusControl;
  uint32_t XDAC_AddressReg;
  unsigned char Reserved1[0x0188];
  uint32_t FirmwareRevision;		/* 7:0 */
};


/*
    Defined below is the structure which is used to hold the
    board's configuration information.
*/

struct cblk471
{
  struct map471 *brd_ptr;     /* base address of the I/O board */
  struct sblk471 *sblk_ptr;   /* Status block pointer */
  uint32_t FPGAAdrData[10];   /* FPGA address & data order:0,1,2,20 thru 26 */
  int nHandle;                /* handle to an open board */
  uint16_t location;          /* AP location */
  uint16_t param;             /* parameter mask for configuring board */
  BOOL bAP;                   /* flag indicating board is open */
  BOOL bInitialized;          /* flag indicating ready to talk to board */	
  uint32_t revision;          /* Firmware Revision */
  uint32_t WriteMask[3];      /* output port mask registers */
  uint32_t EventEnable[3];    /* interrupt enable register */
  uint32_t EventType[3];      /* event type registers */
  uint32_t EventPolarity[3];  /* event polarity registers */
  uint32_t deb_control[3];    /* debounce control registers */
  uint32_t deb_duration[3];   /* debounce duration registers */
  uint32_t deb_clock;         /* debounce clock select register */
  uint32_t bd_enable;         /* board interrupt enable register */
};



/* Declare functions called */

void scfg471(struct cblk471 *c_blk);			/* routine to set up the configuration param. block */
void psts471(struct cblk471 *c_blk);			/* routine which calls the Read Status Command */
void cnfg471(struct cblk471 *c_blk);			/* routine to configure the board */
void rsts471(struct cblk471 *c_blk);			/* routine to read status information */
uint16_t rpnt471(struct cblk471 *c_blk, uint16_t port, uint16_t point);/* routine to read a input point */
uint16_t rprt471(struct cblk471 *c_blk, uint16_t port);	/* routine to read the input port */
uint16_t wpnt471(struct cblk471 *c_blk, uint16_t port,
			uint16_t point, uint16_t value);/* routine to write to a output point */
uint16_t wprt471(struct cblk471 *c_blk, uint16_t port, uint16_t value);/* routine to write to the output port */
int AP471Blocking_io(int nHandle, struct AP471_isr_info *isr_info, long *addr, long value, long cmd);



