
#include "../apCommon/apCommon.h"
#include "AP471.h"

/* 
{+D}
    SYSTEM:             Acromag Digital I/O Board

    FILENAME:           rpnt471.c
	
    MODULE NAME:        rpnt471() - read input point
			    
    VERSION:            A
    
    CREATION DATE:      12/01/15
    
    CODED BY:           FJM
    
    ABSTRACT:           The module reads a input value from a single I/O point.

    CALLING SEQUENCE:   status=rpnt471(c_blk,port,point);
			  where:
			    status (uint16_t)
			      The returned value of the I/O point
			      or error flag
			    c_blk (pointer to structure)
			      Pointer to the configuration data structure
			    port (uint16_t)
			      The target I/O port number
			    point (uint16_t)
			      The target I/O point
			    
    MODULE TYPE:        long
			
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

	This module reads a value from an I/O point of an I/O port, and 
	returns the current value of the point.
*/

uint16_t rpnt471(struct cblk471 *c_blk, uint16_t port, uint16_t point)

{

/*
    DECLARE LOCAL DATA AREAS:
*/

    uint16_t temp;

/*
    ENTRY POINT OF ROUTINE
*/

   temp = (uint16_t)input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->IORegister[(port & 3)]);

   if(temp & (1 << (point & 15)))
      return((uint16_t)1);
   else 
      return((uint16_t)0);
}

