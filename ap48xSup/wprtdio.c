
#include "apCommon.h"
#include "AP48X.h"
/*
{+D}
    SYSTEM:		Acromag AP48x I/O Board

    FILENAME:	wprtdio.c

    MODULE
		NAME:	wprtdio() - write output port

    VERSION:	A

    CREATION
		DATE:	12/01/15

    CODED BY:	FJM

    ABSTRACT:	This module writes output values to a single I/O port.

    CALLING
	SEQUENCE:	status = wprtdio(c_blk, value);
			  where:
			    status (long)
			      The returned error status.
				c_blk (pointer to structure)
				  Pointer to the ap482 structure.
			    value (unsigned)
			      The output value.

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
*/


long wprtdio(c_blk, value)

struct ap48x *c_blk;
uint32_t value; 	    /* the output value */

{

/*
    ENTRY POINT OF ROUTINE
*/

	output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalOut, value);
	return(0);
}
