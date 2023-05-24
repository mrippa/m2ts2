
/*
{+D}
    SYSTEM:	    Software for AP323

    FILE NAME:	    AP323.h

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FM

    CODED BY:	    FM

    ABSTRACT:	    This module contains the definitions and structures
		    used by the AP323 library.

    CALLING
	SEQUENCE:

    MODULE TYPE:    header file

    I/O RESOURCES:

    SYSTEM
	RESOURCES:

    MODULES
	CALLED:

    REVISIONS:

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:

    This module contains the definitions and structures used by the library.
*/



/* definitions */


#define AP323 (word)0x7017	/* AP323 device ID */
#define AP323_DEVICE_NAME "ap323_"	/* the name of the device */
#define AP323_FlashIDString "AP323"   /* Flash ID String value */


#define SA_CHANS	40	/* number of channels */
#define SA_SIZE         1024	/* possible # of elements in a channel's scan array */
#define AVE_SAMPLES	200	/* number of samples to average for calibration values */


#define RANGE_0TO5	0	/* input range */
#define RANGE_5TO5	1	/* input range */
#define RANGE_10TO10	2	/* input range */
#define RANGE_0TO10	3	/* special auto zero value for this range */

#define TC_SELECT	0	/* two's compliment */
#define SB_SELECT	1	/* straight binary */

#define T_DISABLED	0	/* trigger disabled */
#define TI_SELECT	1	/* trigger direction (input) */
#define TO_SELECT	2	/* trigger direction (output) */

#define DI_SELECT	0	/* differential select */
#define SE_SELECT	1	/* single ended select */
#define AZ_SELECT	2	/* auto-zero select */
#define CAL_SELECT	3	/* calibration select */

#define DISABLE		0	/* disable scan */
#define UN_CONT		1	/* uniform continuous */
#define UN_SNGL		2	/* uniform single */
#define BU_CONT		3	/* burst continuous */
#define BU_SNGL		4	/* burst single */
#define EX_TRIG		5	/* external trigger */

#define TIMER_OFF	0	/* disable timer */
#define TIMER_ON	1	/* enable timer */

#define INT_DIS		0	/* disable interrupt */
#define INT_AEC		1	/* interrupt after each channel */
#define INT_AGC		2	/* interrupt after group of channels */
#define INT_RELEASE	0x8000	/* interrupt release */

#define START_CONV		1	/* start conversion */
#define CLR_FIFO_SCN_LIST	2	/* Clear Scan List FIFO */
#define CLR_SAMPLE_FIFO		4	/* Clear Sample FIFO */
#define CLR_SAMPLE_OVERFLAG	8	/* Clear Sample FIFO overflow flag */
#define SL_FIFO_EMPTY		1	/* Scan List FIFO empty */
#define SL_FIFO_FULL		2	/* Scan List FIFO full */
#define SAMPLE_FIFO_EMPTY	4	/* Sample FIFO empty */
#define SAMPLE_FIFO_FULL	8	/* Sample FIFO full */
#define SAMPLE_FIFO_OVRFLOW	16	/* Sample FIFO overflow */


/*
    STRUCTURES:

    Defined below is the memory map template for the AP323 Board.
    This data structure provides access to the various registers on the board.
*/


struct map323
{
    uint32_t InterruptRegister;	/* interrupt register */
    uint32_t LocationRegister;	/* Bits 15:0 Location Register */
    uint32_t cntl_reg;		/* board control register */
    uint32_t tpc_reg;		/* timer prescaler register */
    uint32_t ctc_reg;		/* conversion timer count register */
    uint32_t scn_lst_FIFO;	/* scan list FIFO */
    uint32_t scn_lst_cnt;	/* scan list count */
    uint32_t scn_status;	/* scan status */
    uint32_t sampleFIFO;	/* sample FIFO */
    int sampleFIFOcount;	/* sample FIFO count */
    uint32_t trigFIFOclear;	/* Trigger / FIFO Clear Register */
    unsigned char unused[0x34];
    uint32_t XDAC_StatusControl;/* Bits 15:0 XADC Status/Control Register */	
    uint32_t XDAC_AddressReg;	/* Bits 15:0 XADC Address Register */
    unsigned char Reserved2[0x198];
    uint32_t FirmwareRevision;	/* 31:0 */
    uint32_t FLASHData;		/* 7:0 */
    uint32_t FlashChipSelect;	/* bit 0 */
};


/*
    Defined below is the structure which is used for the read analog command
*/

struct cblk323
{
    struct map323 *brd_ptr;	/* pointer to base address of board */
    uint32_t FPGAAdrData[10];	/* FPGA address & data order:0,1,2,20 thru 26 */
    int nHandle;		/* handle to an open board */
    BOOL bAP;			/* flag indicating open */
    BOOL bInitialized;		/* flag indicating ready */
    int *s_cor_buf[SA_CHANS];	/* corrected buffer */
    unsigned short *s_raw_buf[SA_CHANS];/* raw buffer */
    unsigned short s_count[SA_CHANS];	/* number of samples in each array */
    unsigned short s_az_buf;	/* auto zero data storage area */
    unsigned short s_cal_buf;	/* calibration buffers */
    byte *sa_start;		/* ptr to array of chans */
    byte *sa_end;		/* ptr to end of chans in array */
    char RefCalValues[4][8];	/* reference cal values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
    unsigned char IDbuf[32];	/* storage for APxxx ID string */
    uint32_t conv_timer;	/* conversion timer */
    uint32_t data_format;	/* SB or BTC */
    uint32_t trigger;		/* triggering option software/external */
    uint32_t acq_mode;		/* the acquisition mode */
    uint32_t scan_mode;		/* the scan mode */
    uint32_t timer_ps;		/* timer prescaler */
    uint32_t timer_en;		/* timer enable */
    uint32_t int_mode;		/* the interrupt mode */
    uint32_t range;		/* input range setting of the board */
    uint32_t control;		/* board control register used by read status */
    uint32_t scan_status;	/* scan status register used by read status */
    uint32_t scan_count;	/* scan count register used by read status */
    uint32_t FIFO_count;	/* FIFO count register used by read status */
    uint32_t location;		/* AP location */
    uint32_t revision;		/* Firmware Revision */
};


/* Declare functions called */

void rstsAP323(struct cblk323 *c_blk);		/* read board status */
void cnfgAP323(struct cblk323 *c_blk);		/* configure board */
void setconfAP323(struct cblk323 *c_blk);	/* configuration param. block set up */
void readstatAP323(struct cblk323 *c_blk);	/* routine which calls the Read Status Command */
int mccdAP323(struct cblk323 *c_blk);		/* routine to correct input data */
void calibrateAP323(struct cblk323 *c_blk, int mode);/* get calibration data */
void convertAP323(struct cblk323 *c_blk);	/* get input data */
void move_dataAP323(struct cblk323 *c_blk);
int ReadCalCoeffs323(struct cblk323 *c_blk);
int ReadFlashID323(struct cblk323 *c_blk, unsigned char *p ); /*read flash ID */

/*
    declare interrupt handlers
*/

void isr_AP323(void* pAddr);	/* interrupt handler for AP323 */

