
/*
{+D}
    SYSTEM:         Linux

    FILENAME:		apzcommon.c

    MODULE NAME:	Functions common to the example software.

    VERSION:		A

    CREATION DATE:	4/21/20

    DESIGNED BY:	FJM

    CODED BY:		FJM

    ABSTRACT:		This file contains the implementation of the functions for modules.

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
	This file contains the implementation of the functions for Acromag modules.
*/

#include "apzcommon.h"

/*	Global variables */
int	gNumberAPZs = -1;		/* Number of boards that have been opened and/or flag=-1, if library is uninitialized see function InitAPZLib() */


APZDATA_STRUCT *gpAPZ[MAX_APZS];	/* pointer to the boards */



/*
        Some systems can resolve BIG_ENDIAN/LITTLE_ENDIAN data transfers in hardware.
        If the system is resolving BIG_ENDIAN/LITTLE_ENDIAN data transfers in hardware
        the SWAP_ENDIAN define should be commented out.

        When resolving the BIG_ENDIAN/LITTLE_ENDIAN data transfers in hardware is not
        possible or desired the SWAP_ENDIAN define is provided.

        Define SWAP_ENDIAN to enable software byte swapping for word and long transfers
*/

/* #define SWAP_ENDIAN		/ * SWAP_ENDIAN enables software byte swapping for word and long transfers */



/*	Use this define to exclude I/O functions when two or more common files are combined in the same project */

/*#define NO_APZ_IOFUNCTIONS */

#ifndef NO_APZ_IOFUNCTIONS
word SwapBytes( word v )
{
#ifdef SWAP_ENDIAN		/* endian correction if needed */
  word  Swapped;

  Swapped = v << 8;
  Swapped |= ( v >> 8 );
  return( Swapped );
#else				/* no endian correction needed */
  return( v );
#endif /* SWAP_ENDIAN */

}


long SwapLong( long v )
{
#ifdef SWAP_ENDIAN		/* endian correction if needed */
 word Swap1, Swap2;
 long Swapped;

  Swap1 = (word)(v >> 16);
  Swap1 = SwapBytes( Swap1 );

  Swap2 = (word)v & 0xffff;
  Swap2 = SwapBytes( Swap2 );

  Swapped = (long)(Swap2 << 16);
  Swapped |= (long)(Swap1 & 0xffff);
  return( Swapped );
#else				/* no endian correction needed */
  return( v );
#endif /* SWAP_ENDIAN */

}


byte input_byte(int nHandle, byte *p)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return((byte)0);

	if( p )
	{
           /* place address to read byte from in data [0]; */
           data[0] = (unsigned long) p;
           data[1] = (unsigned long) 0;
           /* pram3 = function: 1=read8bits,2=read16bits,4=read32bits */
           read( pAPZ->nAPZDeviceHandle, &data[0], 1 );
           return( (byte)data[1] );
	}
	return((byte)0);
}

word input_word(int nHandle, word *p)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return((word)0);

	if( p )
	{
           /* place address to read word from in data [0]; */
           data[0] = (unsigned long) p;
           /* pram3 = function: 1=read8bits,2=read16bits,4=read32bits */
           read( pAPZ->nAPZDeviceHandle, &data[0], 2 );
           return( SwapBytes( (word)data[1] ) );
	}
	return((word)0);
}


long input_long(int nHandle, long *p)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return((long)0);

	if( p )
	{
           /* place address to read word from in data [0]; */
           data[0] = (unsigned long) p;
           /* pram3 = function: 1=read8bits,2=read16bits,4=read32bits */
           read( pAPZ->nAPZDeviceHandle, &data[0], 4 );
           return( SwapLong( (long)data[1] ) );
	}
	return((long)0);
}


void output_byte(int nHandle, byte *p, byte v)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return;

	if( p )
	{
		/* place address to write byte in data [0]; */
		data[0] = (unsigned long) p;
		/* place value to write @ address data [1]; */
		data[1] = (unsigned long) v;
	        /* pram3 = function: 1=write8bits,2=write16bits,4=write32bits */
		write( pAPZ->nAPZDeviceHandle, &data[0], 1 );
	}
}

void output_word(int nHandle, word *p, word v)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return;

	if( p )
	{
           /* place address to write word in data [0]; */
           data[0] = (unsigned long) p;
           /* place value to write @ address data [1]; */
           data[1] = (unsigned long) SwapBytes( v );
           /* pram3 = function: 1=write8bits,2=write16bits,4=write32bits */
           write( pAPZ->nAPZDeviceHandle, &data[0], 2 );
	}
}


void output_long(int nHandle, long *p, long v)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[2];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return;

	if( p )
	{
           /* place address to write word in data [0]; */
           data[0] = (unsigned long) p;
           /* place value to write @ address data [1]; */
           data[1] = (unsigned long) SwapLong( v );
           /* pram3 = function: 1=write8bits,2=write16bits,4=write32bits */
           write( pAPZ->nAPZDeviceHandle, &data[0], 4 );
	}
}

int waitForByteValue(int nHandle, byte *reg, byte valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec)
{
	//If we have more than 1000 usec, convert them to msec
	int actualUSec = failAfterX_Microsec % 1000;
	failAfterX_Millisec = failAfterX_Millisec + ((failAfterX_Microsec-actualUSec)/1000);
	failAfterX_Microsec = actualUSec;
	
	//Read the register and see if it has the value we want
	byte valueWeGot = input_byte(nHandle, reg);

	//While we have yet to see the value we want..
	int msecWaited = 0;
	int usecWaited = 0;
	while(valueWeGot != valueToWaitFor)
	{
		//Sleep for one microsec
		//usleep(1);
		
		//Increment the number of microsecs we've waited
		usecWaited++;
		
		//If we've waited 1000 usec..
		if(usecWaited >= 1000)
		{
			//Then we've waited another full millisec
			msecWaited++;
			usecWaited = 0;
		}
		
		//If we waited our set time and still haven't seen the value we want..
		if(msecWaited >= failAfterX_Millisec && usecWaited >= failAfterX_Microsec)
		{
			//Then return an error
			return -1;
		}
		
		//Read the register again and see if it has the value we want
		valueWeGot = input_byte(nHandle, reg);
	}
	
	//At this point, we've seen the value we want to see
	return 1;
}

int waitForWordValue(int nHandle, word *reg, word valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec)
{
	//If we have more than 1000 usec, convert them to msec
	int actualUSec = failAfterX_Microsec % 1000;
	failAfterX_Millisec = failAfterX_Millisec + ((failAfterX_Microsec-actualUSec)/1000);
	failAfterX_Microsec = actualUSec;
	
	//Read the register and see if it has the value we want
	word valueWeGot = input_word(nHandle, reg);

	//While we have yet to see the value we want..
	int msecWaited = 0;
	int usecWaited = 0;
	while(valueWeGot != valueToWaitFor)
	{
		//Sleep for one microsec
		//usleep(1);
		
		//Increment the number of microsecs we've waited
		usecWaited++;
		
		//If we've waited 1000 usec..
		if(usecWaited >= 1000)
		{
			//Then we've waited another full millisec
			msecWaited++;
			usecWaited = 0;
		}
		
		//If we waited our set time and still haven't seen the value we want..
		if(msecWaited >= failAfterX_Millisec && usecWaited >= failAfterX_Microsec)
		{
			//Then return an error
			return -1;
		}
		
		//Read the register again and see if it has the value we want
		valueWeGot = input_word(nHandle, reg);
	}
	
	//At this point, we've seen the value we want to see
	return 1;
}

int waitForLongValue(int nHandle, long *reg, long valueToWaitFor, int failAfterX_Millisec, int failAfterX_Microsec)
{
	//If we have more than 1000 usec, convert them to msec
	int actualUSec = failAfterX_Microsec % 1000;
	failAfterX_Millisec = failAfterX_Millisec + ((failAfterX_Microsec-actualUSec)/1000);
	failAfterX_Microsec = actualUSec;
	
	//Read the register and see if it has the value we want
	long valueWeGot = input_long(nHandle, reg);

	//While we have yet to see the value we want..
	int msecWaited = 0;
	int usecWaited = 0;
	while(valueWeGot != valueToWaitFor)
	{
		//Sleep for one microsec
		//usleep(1);
		
		//Increment the number of microsecs we've waited
		usecWaited++;
		
		//If we've waited 1000 usec..
		if(usecWaited >= 1000)
		{
			//Then we've waited another full millisec
			msecWaited++;
			usecWaited = 0;
		}
		
		//If we waited our set time and still haven't seen the value we want..
		if(msecWaited >= failAfterX_Millisec && usecWaited >= failAfterX_Microsec)
		{
			//Then return an error
			return -1;
		}
		
		//Read the register again and see if it has the value we want
		valueWeGot = input_long(nHandle, reg);
	}
	
	//At this point, we've seen the value we want to see, or we failed after X microsec
	//return either way	
	return 1;
}


long get_param()
{

    int temp;

    printf("enter hex parameter: ");
    scanf("%x",&temp);
    printf("\n");
    return((long)temp);
}

#endif /* NO_APZ_IOFUNCTIONS */


/*
 Blocking options

 parameter =  0 = byte write & block
 parameter =  1 = word write & block
 parameter =  2 = 32 bit write & block
 parameter = 10 = no write, just block & wait for an input event to wake up

 Returns the interrupt pending status value.
*/

uint32_t APZBlockingStartConvert(int nHandle, long *p, long v, long parameter)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data[4];

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
	   return(0);

	data[0] = (unsigned long) p;		/* place address of write in data[0] */

	switch(parameter)
	{
		case 0:	/* flag=0=byte write */
		    data[1] = (unsigned long) v;		/* place byte value to write @ address data[1]; */
		break;
		case 1:	/* flag=1=word write */
		    data[1] = (unsigned long) SwapBytes( v );	/* place word value to write @ address data[1]; */
		break;
		case 2:	/* flag=2=long write */
		    data[1] = (unsigned long) SwapLong( v );	/* place long value to write @ address data[1]; */
		break;
	}

 	data[2] = (unsigned long)parameter;	/* save Blocking option parameter */

 	/* place board instance index in data[3] */
 	data[3] = (unsigned long) pAPZ->nDevInstance;	/* Device Instance */

 	write( pAPZ->nAPZDeviceHandle, &data[0], 8 );		/* function: 8=blocking_start_convert */

	return( (uint32_t)SwapLong( (long)data[1] ) );	/* return Interrupt Pending value */
}


void APZTerminateBlockedStart(int nHandle)
{
	APZDATA_STRUCT* pAPZ;	/*  local */
	unsigned long data;


	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return;

	data = (unsigned long) pAPZ->nDevInstance;	/* Device Instance */
	ioctl( pAPZ->nAPZDeviceHandle, 21, &data );	/* get wake up/terminate cmd */
}


APZSTATUS GetAPZAddress(int nHandle, long *pAddressVirt, long *pAddressPhys)
{
	APZDATA_STRUCT* pAPZ;	/*  local */

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return E_INVALID_HANDLE;

	*pAddressVirt = (long)pAPZ->lBaseAddress; //BAR0 (virtual)
   *pAddressPhys = (long)pAPZ->lBaseAddress_phys; //BAR0 (physical)
	return (APZSTATUS)S_OK;
}


APZSTATUS GetAPZAddress1(int nHandle, long* pAddressVirt, long *pAddressPhys)
{
	APZDATA_STRUCT* pAPZ;	/*  local */

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return E_INVALID_HANDLE;

	*pAddressVirt = (long)pAPZ->lBaseAddress1; //BAR1 (virtual)
   *pAddressPhys = (long)pAPZ->lBaseAddress1_phys; //BAR1 (physical)
	return (APZSTATUS)S_OK;
}


APZSTATUS GetAPZAddress2(int nHandle, long* pAddressVirt, long *pAddressPhys)
{
	APZDATA_STRUCT* pAPZ;	/*  local */

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
		return E_INVALID_HANDLE;

	*pAddressVirt = (long)pAPZ->lBaseAddress2; //BAR2 (virtual)
   *pAddressPhys = (long)pAPZ->lBaseAddress2_phys; //BAR2 (physical)
	return (APZSTATUS)S_OK;
}



APZSTATUS EnableAPZInterrupts(int nHandle)
{
	APZDATA_STRUCT* pAPZ;
	APZ_BOARD_MEMORY_MAP* pAPZCard;
	uint32_t nValue32;
	struct APZ_BAR0_MEMORY_MAP *bar0_ptr;
	uint32_t mask;

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == 0)
		return E_INVALID_HANDLE;

	if(pAPZ->bInitialized == FALSE)
		return E_NOT_INITIALIZED;

	//Enable Interrupts
		//Get a shorthand to the BAR0 and BAR1 pointers
		bar0_ptr = (struct APZ_BAR0_MEMORY_MAP *)pAPZ->lBaseAddress;
		pAPZCard = (APZ_BOARD_MEMORY_MAP*)pAPZ->lBaseAddress1;
		
      //Enable all of the desired channels
      //This gets done in cnfgAPZU30x()
		
      //Clear all pending interrupts, channel by channel (Write 0xFFFFFFFF to BAR1+0x1C) 
      output_long(nHandle, (long *)&pAPZCard->IntStatusRegister, ALL_CHANNELS_ALL_CLEAR);
      waitForLongValue(nHandle, (long *)&pAPZCard->IntStatusRegister, 0, 0, 1000);
		
		//Clear The PCIe Int Status Register, so we don't interrupt on interrupt enable (Write an 8|4|2 to clear bit3 thru 1 of BAR0+0x64)
		mask = DIO_INTERRUPT_BIT | DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT;
		output_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_StatusReg, mask);
		waitForLongValue(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_StatusReg, 0, 0, 1000);
		
		//Enable interrupts via the PCIe Interrupt Enable Register (Set BAR0+0x60 bit0)
		nValue32 = (uint32_t)input_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_EnableReg);
		output_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_EnableReg, nValue32 | PCIE_INT_ENABLE);
		waitForLongValue(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_EnableReg, nValue32 | PCIE_INT_ENABLE, 0, 1000);
		
		//Clear any pending AXI interrupts, if there are any
		output_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_StatusReg, 
				DIO_INTERRUPT_BIT | DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT);
		
		//Disable AXI interrupts, if they're on
		nValue32 = (uint32_t)input_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_EnableReg);
		mask = ~(DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT | PCIE_INT_ENABLE);
		output_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_EnableReg, nValue32 & mask);

      //Enable interrupts via the Master Interrupt Enable Register (Write a 0x1 to BAR1+0x0)
      output_long(nHandle, (long *)&pAPZCard->InterruptRegister, MASTER_INT_ENABLE);
	
	//Mark interrupts enabled
	pAPZ->bIntEnabled = TRUE;
	
	//Return our success
	return (APZSTATUS)S_OK;
}


APZSTATUS DisableAPZInterrupts(int nHandle)
{
	APZDATA_STRUCT* pAPZ;
	APZ_BOARD_MEMORY_MAP* pAPZCard;
	uint32_t nValue32;
	struct APZ_BAR0_MEMORY_MAP *bar0_ptr;
	uint32_t mask;

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == 0)
		return E_INVALID_HANDLE;

	if(pAPZ->bInitialized == FALSE)
		return E_NOT_INITIALIZED;
		
	//Get a shorthand to the BAR0 and BAR1 pointers
	bar0_ptr = (struct APZ_BAR0_MEMORY_MAP *)pAPZ->lBaseAddress;
	pAPZCard = (APZ_BOARD_MEMORY_MAP*)pAPZ->lBaseAddress1;
	
	//Disable interrupts
      //Disable Interrupts on all channels
      output_long(nHandle, (long *)&pAPZCard->IntEnableRegister, 0);
	
		//Clear all pending interrupts, channel by channel (Write 0xFFFFFFFF to BAR1+0x1C) 
		output_long(nHandle, (long *)&pAPZCard->IntStatusRegister, 0xFFFFFFFF);
		waitForLongValue(nHandle, (long *)&pAPZCard->IntStatusRegister, 0, 0, 1000); //Wait a little while for this change to register
	
		//Clear The PCIe Int Status Register, so we don't interrupt again on interrupt re-enable (Clear BAR0+0x64 interrupt stat bits)
		output_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_StatusReg, 
				DIO_INTERRUPT_BIT | DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT);
		waitForLongValue(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_StatusReg, 0, 0, 1000);
		
		//Disable interrupts via the PCIe Interrupt Enable Register (Clear BAR0+0x60 bit0, leave the other bits the same)
		nValue32 = (uint32_t)input_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_EnableReg);
		mask = ~(DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT | PCIE_INT_ENABLE);
		output_long(nHandle, (long *)&bar0_ptr->PCIe_Interrupt_EnableReg, nValue32 & mask);
		
		//Clear any pending AXI interrupts, if there are any
		output_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_StatusReg, 
				DIO_INTERRUPT_BIT | DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT);
		
		//Disable AXI interrupts, if they're on
		nValue32 = (uint32_t)input_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_EnableReg);
		mask = ~(DMA_COMPLETE_INTERRUPT_BIT | DMA_ERR_INTERRUPT_BIT | PCIE_INT_ENABLE);
		output_long(nHandle, (long *)&bar0_ptr->AXI_Interrupt_EnableReg, nValue32 & mask);
		
		//Disable interrupts via the Master Interrupt Enable Register, as well (Write a 0x0 to BAR1+0x0)
		output_long(nHandle, (long*)&pAPZCard->InterruptRegister, 0);
	
	//Mark interrupts disabled
	pAPZ->bIntEnabled = FALSE;

	//Return success
	return (APZSTATUS)S_OK;
}


APZSTATUS InitAPZLib(void)
{
	int i;				/* General purpose index */

    if( gNumberAPZs == -1)		/* first time used - initialize pointers to 0 */
    {
	  gNumberAPZs = 0;		/* Initialize number of APZs to 0 */

	  /* initialize the pointers to the APZ data structure */
	  for(i = 0; i < MAX_APZS; i++)
		gpAPZ[i] = 0;		/* set to a NULL pointer */
    }
	return (APZSTATUS)S_OK;
}


APZSTATUS APZOpen(int nDevInstance, int* pHandle, char* devname)
{
	APZDATA_STRUCT* pAPZ;		/* local pointer */
	unsigned long data[MAX_APZS];
	char devnamebuf[64];
	char devnumbuf[8];

	*pHandle = -1;		/* set callers handle to an invalid value */

	if(gNumberAPZs == MAX_APZS)
		return E_OUT_OF_APZS;

	/* Allocate memory for a new APZ structure */
	pAPZ = (APZDATA_STRUCT*)malloc(sizeof(APZDATA_STRUCT));

	if(pAPZ == 0)
		return (APZSTATUS)E_OUT_OF_MEMORY;

	pAPZ->nHandle = -1;
	pAPZ->bInitialized = FALSE;
	pAPZ->bIntEnabled = FALSE;
	pAPZ->nAPZDeviceHandle = 0;
	pAPZ->lBaseAddress = pAPZ->lBaseAddress1 = pAPZ->lBaseAddress2 = 0;
	pAPZ->nInteruptID = 0;
	pAPZ->nIntLevel = 0;
	pAPZ->nDevInstance = -1;	/* Device Instance */

	memset( &pAPZ->devname[0], 0, sizeof(pAPZ->devname));
	memset( &devnamebuf[0], 0, sizeof(devnamebuf));
	memset( &devnumbuf[0], 0, sizeof(devnumbuf));

	strcpy(devnamebuf, "/dev/");
	strcat(devnamebuf, devname);
	sprintf(&devnumbuf[0],"%d",nDevInstance);
	strcat(devnamebuf, devnumbuf);

	pAPZ->nAPZDeviceHandle = open( devnamebuf, O_RDWR );

	if( pAPZ->nAPZDeviceHandle < 0 )
	{
        	free((void*)pAPZ);		/* delete the memory for this AP */
			return (APZSTATUS)ERROR;
	}
	strcpy(&pAPZ->devname[0], &devnamebuf[0]);	/* save device name */
	pAPZ->nDevInstance = nDevInstance;	/* Device Instance */

	//Get the BAR0 addresses
	memset( &data[0], 0, sizeof(data)); /* no mem if data[x] returns 0 from ioctl() */
   ioctl( pAPZ->nAPZDeviceHandle, 7, &data[0] ); //Get the BAR0 physical address
   pAPZ->lBaseAddress_phys = data[nDevInstance];
	ioctl( pAPZ->nAPZDeviceHandle, 3, &data[0] ); //Get the BAR0 virtual address
	pAPZ->lBaseAddress = data[nDevInstance];

	//Get the BAR1 addresses
	memset( &data[0], 0, sizeof(data)); /* no mem if data[x] returns 0 from ioctl() */
   ioctl( pAPZ->nAPZDeviceHandle, 8, &data[0] ); //Get the BAR1 physical address
   pAPZ->lBaseAddress1_phys = data[nDevInstance];
	ioctl( pAPZ->nAPZDeviceHandle, 4, &data[0] ); //Get the BAR1 virtual address
	pAPZ->lBaseAddress1 = data[nDevInstance];

	//Get the BAR2 addresses
	memset( &data[0], 0, sizeof(data)); /* no mem if data[x] returns 0 from ioctl() */
   ioctl( pAPZ->nAPZDeviceHandle, 9, &data[0] ); //Get the BAR2 physical address
   pAPZ->lBaseAddress2_phys = data[nDevInstance];
	ioctl( pAPZ->nAPZDeviceHandle, 5, &data[0] ); //Get the BAR2 virtual address
	pAPZ->lBaseAddress2 = data[nDevInstance];

	/* Get IRQ Number */
	ioctl( pAPZ->nAPZDeviceHandle, 6, &data[0] );		/* get IRQ cmd */
	pAPZ->nIntLevel = ( int )( data[nDevInstance] & 0xFF );

	AddAPZ(pAPZ);                  /* call function to add APZ to array and set handle */
	*pHandle = pAPZ->nHandle;      /* return our handle */

	return (APZSTATUS)S_OK;
}


APZSTATUS APZClose(int nHandle)
{
	/*  Delete the AP with the provided handle */
	APZDATA_STRUCT* pAPZ;	/* local pointer */

	pAPZ = GetAPZ(nHandle);

	if(pAPZ == 0)
		return E_INVALID_HANDLE;

	if(pAPZ->bInitialized == FALSE)
		return E_NOT_INITIALIZED;

  	close( pAPZ->nAPZDeviceHandle );

  	pAPZ->nAPZDeviceHandle = -1;
	DeleteAPZ(nHandle);		/*  Delete the APZ with the provided handle */

	return (APZSTATUS)S_OK;
}


APZSTATUS APZInitialize(int nHandle)
{
	APZDATA_STRUCT* pAPZ;

	pAPZ = GetAPZ(nHandle);
	if(pAPZ == 0)
		return E_INVALID_HANDLE;

	pAPZ->bInitialized = TRUE;	/* APZ is now initialized */

	return (APZSTATUS)S_OK;
}


void AddAPZ(APZDATA_STRUCT* pAPZ)
{
	int i, j;			/* general purpose index */
	BOOL bFound;			/* general purpose BOOL */

	for(i = 0; i < MAX_APZS; i++)	/* Determine a handle for this APZ */
	{
		bFound = TRUE;
		for(j = 0; j < gNumberAPZs; j++)
		{
			if(i == gpAPZ[j]->nHandle)
			{
				bFound = FALSE;
				break;
			}
		}

		if(bFound)
			break;
	}

	pAPZ->nHandle = i;          	/* set new handle */
	gpAPZ[gNumberAPZs] = pAPZ;		/* add APZ to array */
	gNumberAPZs++;			/* increment number of APZs */
}


void DeleteAPZ(int nHandle)
{
	APZDATA_STRUCT* pAPZ;
	int i;

	if(gNumberAPZs == 0)
		return;

	pAPZ = 0;			/* initialize pointer to null */
	for(i = 0; i < gNumberAPZs; i++)/* Find APZ that has this handle */
	{
		if(nHandle == gpAPZ[i]->nHandle)
		{
			pAPZ = gpAPZ[i];
			break;
		}
	}
	if(pAPZ == 0)			/* return if no APZ has been found */
		return;

	free((void*)pAPZ);		/* delete the memory for this APZ */

	/* Rearrange APZ array */
	gpAPZ[i] = gpAPZ[gNumberAPZs - 1];
	gpAPZ[gNumberAPZs - 1] = 0;
	gNumberAPZs--;			/* decrement APZ count */
}


APZDATA_STRUCT* GetAPZ(int nHandle)
{
	APZDATA_STRUCT* pAPZ;
	int i;				/* General purpose index */

	for(i = 0; i < gNumberAPZs; i++)/* Find APZ that has this handle */
	{
		if(nHandle == gpAPZ[i]->nHandle)
		{
			pAPZ = gpAPZ[i];
			return pAPZ;
		}
	}
	return (APZDATA_STRUCT*)0;	/* return null */
}


int get_nAPZDeviceHandle(int nHandle)
{
	APZDATA_STRUCT* pAPZ;
	
	pAPZ = GetAPZ(nHandle);
	if(pAPZ == NULL)
	{
		//Then return -1 instead of our handle
		return -1;
	}
	
	//Return the handle we retrieved
	return pAPZ->nAPZDeviceHandle;
}


