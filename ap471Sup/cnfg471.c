
#include "apCommon.h"
#include "AP471.h"

/*
{+D}
    SYSTEM:         Software

    FILENAME:       cnfg471.c

    MODULE NAME:    cnfg471 - configure 471 board

    VERSION:        A

    CREATION DATE:  12/01/15

    CODED BY:       FJM

    ABSTRACT:       This module is used to perform the configure function
					for the board.

    CALLING
	SEQUENCE:   cnfg471(ptr);
		    where:
			ptr (pointer to structure)
			    Pointer to the configuration block structure.

    MODULE TYPE:    void

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

    This module is used to perform the "configure board" function for 
    the board.  A pointer to the Configuration Block will be passed 
    to this routine.  The routine will use a pointer within the 
    Configuration Block to reference the registers on the Board.  
    Based on flag bits in the Attribute and Parameter Flag words in the 
    Configuration Block, the board will be configured and various 
    registers will be updated with new information which will be 
    transfered from the Configuration Block to registers on the Board.
*/



void cnfg471(struct cblk471 *c_blk)
{

/*
    ENTRY POINT OF ROUTINE:
*/

/* If the Mask Registers are to be updated, then update them */
   if(c_blk->param & MASK)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[0], c_blk->WriteMask[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[1], c_blk->WriteMask[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[2], c_blk->WriteMask[2]);
   }

/* If the Debounce Clock Register is to be updated */
   if(c_blk->param & DEBCLOCK)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceClockSelect, c_blk->deb_clock);
   }

/* If the Debounce Control Registers are to be updated */
   if(c_blk->param & DEBCONTROL)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[0], c_blk->deb_control[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[1], c_blk->deb_control[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[2], c_blk->deb_control[2]);
   }

/* If the Debounce Duration Registers are to be updated */
   if(c_blk->param & DEBDURATION)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[0], c_blk->deb_duration[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[1], c_blk->deb_duration[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[2], c_blk->deb_duration[2]);
   }

/* If the Event Polarity Registers are to be updated */
   if(c_blk->param & EV_POLARITY)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[0], c_blk->EventPolarity[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[1], c_blk->EventPolarity[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[2], c_blk->EventPolarity[2]);
   }

/* If the Event Type Registers are to be updated */
   if(c_blk->param & EV_TYPE)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[0], c_blk->EventType[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[1], c_blk->EventType[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[2], c_blk->EventType[2]);
   }

/* If the Event Enable Registers are to be updated */
   if(c_blk->param & EV_ENABLE)
   {
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[0], c_blk->EventEnable[0]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[1], c_blk->EventEnable[1]);
      output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[2], c_blk->EventEnable[2]);
   }
}


/*
{+D}
    FILENAME:           cnfg471.c

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will block until the channel generates an interrupt.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Blocking I/O 471, block & wait for an event to wake up

 Returns -1 if the handle is invalid else returns the IIR values.
*/


int AP471Blocking_io(int nHandle, struct AP471_isr_info *isr_info, long *addr, long value, long cmd)
{
    APDATA_STRUCT* pAP;	/*  local */
    unsigned long data[4];

    pAP = GetAP(nHandle);
    if(pAP == NULL)
	return(-1);

    /* place address to write to in data[0] */
    data[0] = (unsigned long) addr;

    /* place data value to write at above address in data[1] */
    data[1] = (unsigned long) value;

    /* place transfer size flag in data[2]... flag=0=byte write, flag=1=word write... */
    /*... flag=2=long write, flag=10=no write just wait for a input event to wake up */
    data[2] = (unsigned long)cmd;

    /* place board instance index in data[3] */
    data[3] = (unsigned long) pAP->nDevInstance;	/* Device Instance */

    /* pram3 = function: 8=blocking_start_convert */
    write( pAP->nAPDeviceHandle, &data[0], 8 );

    /* Parse data and place in structure */
    isr_info->Status = (uint32_t)data[0];              /* exit status */
    isr_info->EventPending[0] = (uint32_t)data[1];     /* individual channel interrupt cause info */
    isr_info->EventPending[1] = (uint32_t)data[2];
    isr_info->EventPending[2] = (uint32_t)data[3];

    return((int)0);
}




