
#include "apCommon.h"
#include "AP48X.h"
/*
{+D}
    SYSTEM:			Acromag AP48X I/O Board

    FILENAME:       rmid48x.c

    MODULE NAME:    rmid48x - read ap48x board

    VERSION:		A

    CREATION DATE:	12/01/15

    DESIGNED BY:    F.J.M

    ABSTRACT:       This module is used to read the status of the ap48x board.

    CALLING
        SEQUENCE:   rmid48x(ptr);
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

  DATE	     BY	    PURPOSE
  --------  ----    ------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:

    This module is used to perform the read status function
    for the board.  A pointer to the Configuration Block will
    be passed to this routine.  The routine will use a pointer
    within the Configuration Block together with offsets
    to reference the registers on the Board and will transfer the 
    status information from the Board to the Configuration Block.
*/



void rmid48x(c_blk)
struct ap48x *c_blk;
{
	long addr, index;

/*
    ENTRY POINT OF ROUTINE
*/
   c_blk->location = (word)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->LocationRegister);/* AP location */

   c_blk->revision = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->FirmwareRevision);	/* AP Revision */

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
