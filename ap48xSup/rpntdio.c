
#include "apCommon.h"
#include "AP48X.h"
/*
{+D}
    SYSTEM:		Acromag AP48x I/O Board

    MODULE NAME:	rpntdio() - read point

    VERSION:		A

    CREATION DATE:	12/01/15

    CODED BY:		FJM

    ABSTRACT:		The module reads a input value from a single I/O point.

    CALLING SEQUENCE:	status=rpntdio(ptr, port, point);
			  where:
			    status (long)
			      The returned value of the I/O point
			      or error flag.
			    ptr (pointer to structure)
			      Pointer memory map structure.
			    port (unsigned)
			      The target port input or output. 0=Input, 1=output
			    point (unsigned)
			      The target input point.

    MODULE TYPE:	long

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

  DATE	     BY	    PURPOSE
  --------  ----    ------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTION DETAILS

	This module reads a value from an I/O point.
*/

long rpntdio(c_blk, port, point)

struct ap48x *c_blk;
uint32_t port; 	    /* the port */
uint32_t point;	    /* the I/O point of a port */

{

/*
    ENTRY POINT OF ROUTINE
*/

	if( port )
	{
	  if ( (unsigned)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalOut) & (1 << point) )
	    return(1);
	  else
	    return(0);
	}
	else
	{
 	  if ( (unsigned)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalInput) & (1 << point) )
	    return(1);
	  else
	    return(0);
	}
}
