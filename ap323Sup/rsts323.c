
#include "apCommon.h"
#include "AP323.h"

/*
{+D}
    SYSTEM:         Library Software

    MODULE NAME:    rstsAP323 - read status of the board

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FM

    CODED BY:       FM
    
    ABSTRACT:       This module is used to read status of the AP323 board.

    CALLING
        SEQUENCE:   rstsAP323(ptr);
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

    This module is used to perform the read status function
    for the board.  A pointer to the Configuration Block will
    be passed to this routine.  The routine will use a pointer
    within the Configuration Block together with offsets
    to reference the registers on the Board and will transfer the 
    status information from the Board to the Configuration Block.
*/



void rstsAP323(c_blk)
struct cblk323 *c_blk;
{

   long addr, index;

/*
    ENTRY POINT OF ROUTINE
*/

   c_blk->control = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->cntl_reg); /* board control register */
   c_blk->location = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->LocationRegister);/* AP location */
   c_blk->revision = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->FirmwareRevision);/* AP Revision */
   c_blk->scan_status = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->scn_status);   /* scan status */
   c_blk->FIFO_count = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->sampleFIFOcount);/* sample FIFO count */
   c_blk->scan_count = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->scn_lst_cnt);    /* scan count */

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
