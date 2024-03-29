
#include "../apCommon/apCommon.h"
#include "AP471.h"

/* 
{+D}
    SYSTEM:             Acromag Digital I/O  Board

    FILENAME:           wpnt471.c
	
    MODULE NAME:        wpnt471() - write output point
			    
    VERSION:            B
    
    CREATION DATE:      12/01/15
    
    CODED BY:           FJM
	
    ABSTRACT:           Module writes an output value to a single I/O point.
    
    CALLING SEQUENCE:   status = wpnt471(c_blk, port, point, value);
			  where:
			    status (uint16_t)
			      The returned error status
			    cblk (pointer to structure)
			      Pointer pointer to configuration structure
			    port (uint16_t)
			      The target I/O port number
			    point (uint16_t)
			      The target I/O point number
			    value (uint16_t)
			      The output value
			    
    MODULE TYPE:        uint16_t
    
    I/O RESOURCES:
    
    SYSTEM RESOURCES:   

    MODULES CALLED:
    
    REVISIONS:
    
      DATE      BY      PURPOSE
    --------   -----    ---------------------------------------------------
    08/05/19    FJM		Data manipulation was ( temp & ~bpos ) | (value & 1)
	--------    ---		should have been ( temp & ~bpos ) | value

{-D}
*/


/*  
    MODULES FUNCTION DETAILS

	This module writes a value (0 or 1) to a point of an output port.
*/
    
uint16_t wpnt471(struct cblk471 *c_blk, uint16_t port, uint16_t point, uint16_t value)

{

/*
    DECLARE LOCAL DATA AREAS:
*/

    uint16_t bpos;      /* bit position */
    uint16_t temp;

/*
    ENTRY POINT OF ROUTINE
*/

    port &= 3;          /* limit port 0 - 3 */
    point &= 0xF;       /* limit point 15 - 0 */
	value &= 1;         /* limit value 0 or 1 */
    bpos = 1 << point;
    value <<= point;

    temp = (uint16_t)input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->IORegister[port]);
    temp = ( temp & ~bpos ) | value;
    output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->IORegister[port], (long)temp);
    return(0);
}
