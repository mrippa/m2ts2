//================================================================================

#ifndef APZCOMMON__H
#define APZCOMMON__H

//================================================================================
/*
{+D}
    SYSTEM:		    Pentium

    FILENAME:	    apzcommon.h

    MODULE NAME:    Functions common to the Pentium example software.

    VERSION:	    A

    CREATION DATE:  4/21/20

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       This file contains the definitions, structures and prototypes.

    CALLING
	SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM
	RESOURCES:

    MODULES
	CALLED:

    REVISIONS:

  DATE	   BY	    PURPOSE
--------  ----	------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:

	This file contains the definitions, structures and prototypes.
*/

//================================================================================

#define VENDOR_ID (word)0x16D5    /* Acromag's vendor ID for all PCI bus products */
#define MAX_APZS 4                /* maximum number of boards */


#ifndef BUILDING_FOR_KERNEL
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* Required for FC4 */
#include <stdlib.h>     /* malloc */
#include <string.h>     /* memset */
#endif /* BUILDING_FOR_KERNEL */

//================================================================================

typedef unsigned char BYTE;
typedef int BOOL;
typedef unsigned char byte;		/* custom data type */
typedef unsigned short word;    /* custom data type */
typedef short WORD;

typedef int APZSTATUS;          /* APZSTATUS return value from the common functions */
#define TRUE	1               /* Boolean value for true */
#define FALSE	0               /* Boolean value for false */

#define APZ_RESET	0x8000      /* ORed with interrupt register to reset board */
#define APZ_INT_ENABLE	0x0001  /* interrupt enable */
#define APZ_INT_PENDING	0x0002  /* interrupt pending bit */

#define MASTER_INT_ENABLE 0x1 //Writing this value to BAR1->InterruptRegister (BAR1+0x0) Master-Enables Interrupts on the APZU
#define ALL_CHANNELS_ALL_CLEAR 0xFFFFFFFF //Write this value to the Channel Int Pending Register to clear all pending interrupts
#define DIO_INTERRUPT_BIT 0x8 //This bit goes high in the PCIe Int Status Reg on DIO interrupt
#define DMA_COMPLETE_INTERRUPT_BIT 0x4 //This bit enables DMA complete ints in the PCIe Int Enable Reg, and
													//it goes high in the PCIe Int Status Reg on DMA complete interrupt
#define DMA_ERR_INTERRUPT_BIT 0x2 //This bit enables DMA error interrupts in the PCIe Int Enable Reg, and
												//it goes high in the PCIe Int Status Reg on DMA error interrupt
#define PCIE_INT_ENABLE 0x1  //This bit enables DIO and DMA ints in the PCIe Int Enable Reg, and 
										//it is unused (reserved) in the PCIe Int Status Reg

//================================================================================

/*
	APZSTATUS return values
	Errors will have most significant bit set and are preceded with an E_.
	Success values will be succeeded with an S_.
*/

#define ERROR			0x8000	/* general */
#define E_OUT_OF_MEMORY 	0x8001	/* Out of memory status value */
#define E_OUT_OF_APZS	   	0x8002	/* All APZ spots have been taken */
#define E_INVALID_HANDLE	0x8003	/* no APZ exists for this handle */
#define E_NOT_INITIALIZED	0x8006	/* not initialized */
#define E_NOT_IMPLEMENTED	0x8007	/* Function is not implemented */
#define E_NO_INTERRUPTS 	0x8008	/* unable to handle interrupts */
#define S_OK			0x0000	/* Everything worked successfully */

//================================================================================

/*
	APZ data structure
*/
typedef struct
{
	int nHandle;			/* handle from addpmc() */
	int nAPZDeviceHandle;	/* handle from kernel open() */
	long lBaseAddress;		//Pointer to the BAR0 base address of the board
   long lBaseAddress_phys; //Pointer to the BAR0 base address of the board (physical)
	long lBaseAddress1;		//Pointer to the BAR1 base address of the board
   long lBaseAddress1_phys;//Pointer to the BAR1 base address of the board (physical)
	long lBaseAddress2;		//Pointer to the BAR2 base address of the board
   long lBaseAddress2_phys;//Pointer to the BAR2 base address of the board (physical)
	int nDevInstance;		/* Device Instance */
	int nInteruptID;		/* ID of interrupt handler */
	int nIntLevel;			/* Interrupt level */
	char devname[64];		/* device name */
	BOOL bInitialized;		/* intialized flag */
	BOOL bIntEnabled;		/* interrupts enabled flag */
}APZDATA_STRUCT;

struct APZ_BAR0_MEMORY_MAP
{
	unsigned char Reserved0[0x60];     //0x0000 Reserved
	uint32_t PCIe_Interrupt_EnableReg; //0x0060 PCIe Int Enable Register
	uint32_t PCIe_Interrupt_StatusReg; //0x0064 PCIe Int Status Register
   uint32_t AXI_Interrupt_EnableReg;  //0x0068 AXI Int Enable Register
   uint32_t AXI_Interrupt_StatusReg;  //0x006C AXI Int Status Register
   unsigned char Reserved1[0x10];     //0x0070 Reserved
};

typedef struct
{
	uint32_t InterruptRegister;	   //0x0 Interrupt Pending/control Register */
	unsigned char Reserved0[0xC];     //0x4 Reserved
	
	uint32_t IntEnableRegister;      //0x10 - Bits (model dependant) 27:0 Interrupt Enable Register
	unsigned char Reserved1[0x8];    //0x14 Reserved
	uint32_t IntStatusRegister;      //0x1C Channel By Channel Interrupt Pending Register
	unsigned char Reserved2[0x3FE0]; //0x20 Reserved
	
	unsigned char blockRAM[0x4000];  //0x4000 Block RAM
}APZ_BOARD_MEMORY_MAP;

//================================================================================

/*
	Function Prototypes
*/
APZSTATUS GetAPZAddress(int nHandle, long* pAddressVirt, long *pAddressPhys);
APZSTATUS GetAPZAddress1(int nHandle, long* pAddressVirt, long *pAddressPhys);
APZSTATUS GetAPZAddress2(int nHandle, long* pAddressVirt, long *pAddressPhys);
APZSTATUS EnableAPZInterrupts(int nHandle);
APZSTATUS DisableAPZInterrupts(int nHandle);
APZSTATUS InitAPZLib(void);
APZSTATUS APZOpen(int nDevInstance, int* pHandle, char* devname);
APZSTATUS APZClose(int nHandle);
APZSTATUS APZInitialize(int nHandle);


/*  Functions used by above functions */
void AddAPZ(APZDATA_STRUCT* pAPZ);
void DeleteAPZ(int nHandle);
APZDATA_STRUCT* GetAPZ(int nHandle);
int get_nAPZDeviceHandle(int nHandle);
uint32_t APZBlockingStartConvert(int nHandle, long *p, long v, long parameter);
void APZTerminateBlockedStart(int nHandle);

word SwapBytes( word v );
long SwapLong( long v );
byte input_byte(int nHandle, byte*);		/* function to read an input byte */
word input_word(int nHandle, word*);		/* function to read an input word */
void output_byte(int nHandle, byte*, byte);	/* function to output a byte */
void output_word(int nHandle, word*, word);	/* function to output a word */
long input_long(int nHandle, long*);		/* function to read an input long */
void output_long(int nHandle, long*, long);	/* function to output a long */
int waitForByteValue(int nHandle, byte *reg, byte valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec);
int waitForWordValue(int nHandle, word *reg, word valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec);
int waitForLongValue(int nHandle, long *reg, long valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec);
long get_param(void);		/* input a parameter */

//================================================================================

#endif

//================================================================================
