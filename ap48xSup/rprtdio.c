
#include "../apcommon/apcommon.h"
#include "AP48X.h"
/*
{+D}
    SYSTEM:		Acromag AP48x I/O Board

    FILENAME:	rprtdio.c

    MODULE
		NAME:	rprtdio() - read input port

    VERSION:	A

    CREATION
		DATE:	12/01/15

    CODED BY:	FJM

    ABSTRACT:	The module reads an input value from a single I/O port.

    CALLING
	SEQUENCE:	status = rprtdio(c_blk, port);
			  where:
			    status (long)
			      The returned value of the I/O port
			      or error flag.
			    ptr (pointer to structure)
			      Pointer to the board memory map structure.
			    port (unsigned)
			      The target port input or output. 0=Input, 1=output


    MODULE TYPE:	long

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

      DATE	BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTION DETAILS

	This module reads a value from an Input or Output port, and returns the
	current value of all points in the port.
*/


long rprtdio(c_blk, port)

struct ap48x *c_blk;
uint32_t port;

{

/*
    ENTRY POINT OF THE ROUTINE
*/

	if( port )
	    return ((long)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalOut));
	else
		return ((long)input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalInput));
}
