
#include "../apCommon/apCommon.h"
#include "AP471.h"

/*
{+D}
    SYSTEM:         Library Software - Board

    FILENAME:       rsts471.c

    MODULE NAME:    rsts471 - read status

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM
    
    ABSTRACT:       This module is used to read status of the board.

    CALLING
	SEQUENCE:   rsts471(c_blk);
		    where:
			c_blk (pointer to structure)
			    Pointer to the configuration structure.

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

    This module is used to perform the read status function for the
    board.  A pointer to the configuration structure will be passed to this 
    routine.  The routine will use a pointer together with offsets to 
    reference the registers on the Board and will transfer the status 
    information from the configuration structure.
*/



void rsts471(struct cblk471 *c_blk)

{

   long addr, index;

/*
    ENTRY POINT OF ROUTINE
*/
printf("rsts471 enter1\n");
    c_blk->location = (word)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->LocationRegister);/* AP location */
    c_blk->revision = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->FirmwareRevision);	 /* AP Revision */

    c_blk->sblk_ptr->DebounceClockSelectStat = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceClockSelect);
    c_blk->sblk_ptr->BoardIntEnableStat = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceClockSelect);

    c_blk->sblk_ptr->DebounceDurationStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[0]);
    c_blk->sblk_ptr->DebounceDurationStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[1]);
    c_blk->sblk_ptr->DebounceDurationStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceDuration[2]);

    c_blk->sblk_ptr->DebounceControlStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[0]);
    c_blk->sblk_ptr->DebounceControlStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[1]);
    c_blk->sblk_ptr->DebounceControlStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DebounceControl[2]);

    c_blk->sblk_ptr->MaskRegStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[0]);
    c_blk->sblk_ptr->MaskRegStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[1]);
    c_blk->sblk_ptr->MaskRegStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->WriteMaskReg[2]);

    c_blk->sblk_ptr->EventTypeStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[0]);
    c_blk->sblk_ptr->EventTypeStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[1]);
    c_blk->sblk_ptr->EventTypeStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventTypeReg[2]);

    c_blk->sblk_ptr->EventPolarityStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[0]);
    c_blk->sblk_ptr->EventPolarityStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[1]);
    c_blk->sblk_ptr->EventPolarityStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPolarityReg[2]);

    c_blk->sblk_ptr->EventEnableStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[0]);
    c_blk->sblk_ptr->EventEnableStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[1]);
    c_blk->sblk_ptr->EventEnableStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventEnableReg[2]);

    c_blk->sblk_ptr->EventPendingClrStat[0] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[0]);
    c_blk->sblk_ptr->EventPendingClrStat[1] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[1]);
    c_blk->sblk_ptr->EventPendingClrStat[2] = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[2]);

printf("rsts471 enter2\n");
   /* read temp & VCC info from FPGA */
   for( addr = 0, index = 0; index < 3; index++, addr++)
   {
     output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_AddressReg, (long)addr); /* FPGA[addr] */
     c_blk->FPGAAdrData[index] = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_StatusControl); /* addr & data [index] */
   }

   for( addr = 0x20, index = 3; index < 6; index++, addr++)
   {
     output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_AddressReg, (long)addr); /* FPGA[addr] */
     c_blk->FPGAAdrData[index] = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_StatusControl); /* addr & data [index] */
   }

   for( addr = 0x24, index = 6; index < 9; index++, addr++)
   {
     output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_AddressReg, (long)addr); /* FPGA[addr] */
     c_blk->FPGAAdrData[index] = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->XDAC_StatusControl); /* addr & data [index] */
   }
}
