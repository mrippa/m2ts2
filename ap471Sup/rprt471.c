
#include "../apcommon/apcommon.h"
#include "AP471.h"

/* 
{+D}
    SYSTEM:             Acromag Digital I/O Board

    FILENAME:           rprt471.c
	
    MODULE NAME:        rprt471() - read input port
			    
    VERSION:            A
    
    CREATION DATE:      12/01/15
    
    CODED BY:           FJM
    
    ABSTRACT:           The module reads an input value from a single I/O port.
			
    CALLING SEQUENCE:   status = rprt471(c_blk, port);
			  where:
			    status (uint16_t)
			      The returned value of the I/O port
			      or error flag.
			    c_blk (pointer to structure)
			      Pointer to the configuration data structure.
			    port (uint16_t)
			      The target I/O port number.
			    
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

	This module reads a value from an I/O port, and returns the 
	current value of all points in that port.
*/


uint16_t rprt471(struct cblk471 *c_blk, uint16_t port)

{

/*
    ENTRY POINT OF ROUTINE
*/
   
   return((uint16_t)input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->IORegister[(port & 3)]));
}

