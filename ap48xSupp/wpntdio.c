
#include "../apcommon/apcommon.h"
#include "AP48X.h"
/*
{+D}
    SYSTEM:		Acromag AP48x I/O Board

    FILENAME:	wpntdio.c

    MODULE
		NAME:	wpntdio() - write output point

    VERSION:	A

    CREATION
		DATE:	12/01/15

    CODED BY:	FJM

    ABSTRACT:	Module writes an output value to a single I/O point.

    CALLING
	SEQUENCE:	status = wpntdio(c_blk, point, value);
			  where:
			    status (long)
			      The returned error status.
				c_blk (pointer to structure)
			      Pointer to the ap482 structure.
			    point (unsigned)
			      The target I/O point number
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

	This module writes a value (0 or 1) to a point of an output port.
*/

long wpntdio(c_blk, point, value)

struct ap48x *c_blk;
uint32_t point; 	    /* the I/O point of a port */
uint32_t value; 	    /* the output value */

{

/*
    DECLARE LOCAL DATA AREAS:
*/

    uint32_t bpos;		/* bit position */
    uint32_t nValue;	/* current value of port */
/*
    ENTRY POINT OF ROUTINE
*/

    if (point > 31 || value > 1)	/* error checking */
		return(-1);
    else
    {
	bpos = 1 << point;
	value <<= point;

	nValue = input_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalOut);
	output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->DigitalOut, ( nValue & ~bpos ) | value);

	return(0);
    }
}
