
#include "../apCommon/apCommon.h"
#include "AP471.h"

/* 
{+D}
    SYSTEM:             Acromag Digital I/O Board

    FILENAME:           wprt471.c
	
    MODULE NAME:        wprt471() - write output port
			    
    VERSION:            A
    
    CREATION DATE:      12/01/15
    
    CODED BY:           FJM
    
    ABSTRACT:           This module writes output values to a single I/O port.
    
    CALLING SEQUENCE:   status = wprt471(c_blk, port, value);
			  where:
			    status (uint16_t)
			      The returned error status.
			    c_blk (pointer to structure)
			      Pointer to configuration structure.
			    port (uint16_t)
			      The target I/O port number.
			    value (uint16_t)
			      The output value.
			    
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

    This module writes a 16-bit hex output value to an output port.
*/


uint16_t wprt471(struct cblk471 *c_blk, uint16_t port, uint16_t value)

{

/*
    ENTRY POINT OF ROUTINE
*/

   output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->IORegister[(port & 3)], (long)value);
   return(0);
}
