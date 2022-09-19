
#include "apCommon.h"
#include "AP323.h"

/*
{+D}
    SYSTEM:	    Library Software

    MODULE NAME:    build control - build control word for AP323

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This builds up the control word to be used in
		    converting the AP323 board.

    CALLING
	SEQUENCE:   build_control(ptr);
		    where:
			ptr (pointer to structure)
			    Pointer to the configuration block structure.

    MODULE TYPE:    int

    I/O RESOURCES:

    SYSTEM
	RESOURCES:

    MODULES
	CALLED:

    REVISIONS:

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:
*/



static uint32_t build_control(c_blk)

struct cblk323 *c_blk;

{

/*
    declare local storage
*/

    uint32_t control;		/* board control register */

/*
    ENTRY POINT OF ROUTINE:
    Build up control word
*/

    control = DISABLE;			/* initialize control register value */

    if(c_blk->data_format == SB_SELECT) /* select straight binary */
       control |= 1;

    if(c_blk->trigger == TI_SELECT)	/* select trigger input */
       control |= 2;

    if(c_blk->trigger == TO_SELECT)	/* select trigger output */
       control |= 4;

    switch(c_blk->acq_mode)		/* select acquisition mode */
    {
      case SE_SELECT:
	 control |= (1 << 3);		/* All Channels Single Ended Input */
      break;

      case AZ_SELECT:			/* select auto zero */
	 switch(c_blk->range)
	 {
	   case RANGE_0TO5:		/* range 0 to 5 */
	   case RANGE_0TO10:		/* range 0 to 10 */
	      control |= (6 << 3);	/* 1.235v Calibration Voltage */
	   break;

	   default:
		control |= (7 << 3);	/* Auto Zero Calibration Voltage */
	   break;
	 }
      break;

      case CAL_SELECT:			/* calibration mode */
	 switch(c_blk->range)
	 {
	   case RANGE_5TO5:		/* range -5 to 5 */
	   case RANGE_0TO5:		/* range 0 to 5 */
		   control |= (4 << 3);	/* 4.940v Calibration Voltage */
	   break;

	   case RANGE_10TO10:		/* range -10 to 10 */
	   case RANGE_0TO10:		/* range 0 to 10 */
		   control |= (3 << 3);	/* 9.880v Calibration Voltage */
	   break;
	 }
      break;

      default:				/* differential input is default */
      break;
    }

    control |= (c_blk->scan_mode << 8); /* scan mode bits */

    if(c_blk->timer_en == TIMER_ON)	/* select timer enable */
       control |= 0x0800;

    control |= (c_blk->int_mode << 12); /* interrupt mode bits */

    return(control);			/* return control register */
}



/*
{+D}
    SYSTEM:	    Library Software

    MODULE NAME:    cnfg323 - configure AP323

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This configures the AP323 board.

    CALLING
	SEQUENCE:   cnfg323(ptr);
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

  DATE	  BY	    PURPOSE
-------  ----	------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:
*/



void cnfgAP323(c_blk)

struct cblk323 *c_blk;

{

byte *wsa_start;
int elements;		/* number of elements in scan array */

/*
    ENTRY POINT OF ROUTINE:
*/

    /* stop interrupts, scanning, & auto zero the input */
    output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->cntl_reg, DISABLE | 0x38);

    /* install new control register value */
    output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->cntl_reg, build_control(c_blk));

    /* clear scan list, sample FIFO, overflow flag */
    output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->trigFIFOclear, CLR_FIFO_SCN_LIST | CLR_SAMPLE_FIFO | CLR_SAMPLE_OVERFLAG );

    wsa_start = c_blk->sa_start; /* get start of user scan list */

    /* compute the number of elements in the scan array by doing pointer arithmetic & limit length */
    elements = (c_blk->sa_end - c_blk->sa_start) + 1;
    if(elements > 1024)
       elements = 1024;
 
    /* write the scan list to the board */
    for(; elements; elements--, wsa_start++)
      output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->scn_lst_FIFO, *wsa_start);

    /* update the prescaler and conversion timer */
    output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->tpc_reg, c_blk->timer_ps);
    output_long(c_blk->nHandle, (long*)&c_blk->brd_ptr->ctc_reg, c_blk->conv_timer);
    usleep(5);	/* Let input settle after board configuration as specified in user manual */
}
