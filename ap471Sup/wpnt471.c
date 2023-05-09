
#include "apCommon.h"
#include "AP471.h"

/* 
{+D}
    SYSTEM:             Acromag Digital I/O  Board

    FILENAME:           wpnt471.c
	
    MODULE NAME:        wpnt471() - write output point
			    
    VERSION:            A
    
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

    uint16_t bpos;              /* bit position */
    uint16_t temp;

/*
    ENTRY POINT OF ROUTINE
*/

    port &= 3;		/* limit port 0 - 3 */
    point &= 0xF;		/* limit point 15 - 0 */
    bpos = 1 << point;
    value <<= point;

    printf("wpnt471 port: %d\n", port);
    printf("wpnt471 point: %d\n", point);
    printf("wpnt471 value: %d\n", value);
    printf("wpnt471 bpos: %d\n", bpos);

    temp = (uint16_t) input_long( c_blk->nHandle, (long *) &c_blk->brd_ptr->IORegister[port]);
    printf("wpnt471 temp (before): %d\n", temp);

    temp = ( temp & ~bpos ) | (value & 1);
    printf("wpnt471 temp (after): %d\n", temp);

    output_long( c_blk->nHandle, (long *) &c_blk->brd_ptr->IORegister[port], (long)temp);
    printf("IORegister[%d]: %d\n", port, c_blk->brd_ptr->IORegister[port]);

    return(0);
}
