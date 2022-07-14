
#include "../apcommon/apcommon.h"
#include "AP323.h"
#include "math.h"

/*
{+D}
    SYSTEM:	    Library Software

    FILENAME:	    mccd323.c

    MODULE NAME:    mccdAP323 - multi channel corrected data

    VERSION:	    A

    CREATION DATE:  12/01/15

    CODED BY:	    FM

    ABSTRACT:	    This module is used to correct input conversions for the board.

    CALLING
	SEQUENCE:   int mccdAP323(ptr);
		    where:
			return (int)
			    -1 = Divide by zero detected 0 otherwise.
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

    This module is used to correct the input data for the board.
    A pointer to the Configuration Block will be passed to this routine.
    The routine will use a pointer within the Configuration Block to
    reference the registers on the Board.
*/


int mccdAP323(struct cblk323 *c_blk)
{

/*
    declare local storage
*/

    int i, j;			/* loop control */
    int *cp;			/* pointer to corrected data */
    int UnipolarFlag;		/* data processing flag, 0 for two's compliment, 1 for straight binary */
    unsigned short *rp;		/* pointer to raw data */
    union			/* used to convert between data types */
    {
      short s;
      unsigned short u;
    }w;

    double temp, seed, seed1;	/* intermediate values */
    double i_span;              /* ideal span value */
    double i_zero;              /* ideal zero value */
    double calhi;               /* high calibration input voltage */
    double callo;               /* low calibration input voltage */
    double slope;               /* slope */
    double az, as;		/* auto zero & auto span */

/*
    ENTRY POINT OF ROUTINE:
    Initialize variables
*/
    if( c_blk->s_cal_buf - c_blk->s_az_buf == 0 )
        return(-1);             /* Divide by zero detected */

    /* Select calibration voltages/ideal zero/span values, and data processing flag */
    switch(c_blk->range)
    {
        /* reference cal values 0=9.88V, 1=4.94V, 2=2.47V, and 3=1.235V */
	case RANGE_0TO5:
            sscanf(&c_blk->RefCalValues[3][0], "%lf", &callo); /* callo = 1.235;	Low Calibration Voltage */
            sscanf(&c_blk->RefCalValues[1][0], "%lf", &calhi); /* calhi = 4.940;	High Calibration Voltage */
	    i_zero = 0.0000;	/* Ideal ADC Input Zero */
	    i_span = 5.0000;	/* Ideal ADC Voltage Span */
	    UnipolarFlag = 1;	/* data processing flag, 1 for straight binary */
	break;

	case RANGE_5TO5:
            sscanf(&c_blk->RefCalValues[1][0], "%lf", &calhi); /* calhi = 4.940;	High Calibration Voltage 4.940 */
	    callo = 0.0;	/* Low Calibration Voltage */
	    i_zero = -5.0000;	/* Ideal ADC Input Zero */
	    i_span = 10.0000;	/* Ideal ADC Voltage Span */
	    UnipolarFlag = 0;	/* data processing flag, 0 for two's compliment */
	break;

	case RANGE_0TO10:
            sscanf(&c_blk->RefCalValues[3][0], "%lf", &callo); /* callo = 1.235;	Low Calibration Voltage */
            sscanf(&c_blk->RefCalValues[0][0], "%lf", &calhi); /* calhi = 9.880;	High Calibration Voltage */
	    i_zero =  0.0;	/* Ideal ADC Input Zero */
	    i_span = 10.0000;	/* Ideal ADC Voltage Span */
	    UnipolarFlag = 1;	/* data processing flag, 1 for straight binary */
	break;

	default: /* RANGE_10TO10 */
            sscanf(&c_blk->RefCalValues[0][0], "%lf", &calhi); /* calhi = 9.880;	High Calibration Voltage */
	    callo = 0.0;	/* Low Calibration Voltage */
	    i_zero = -10.0000;	/* Ideal ADC Input Zero */
	    i_span = 20.0000;	/* Ideal ADC Voltage Span */
	    UnipolarFlag = 0;	/* data processing flag, 0 for two's compliment */
	break;
    }

/*
If the ADC data format is BTC and the data processing flag (UnipolarFlag) is 1, convert az & cal to Straight Binary
                                      ----  OR  ----
If the ADC data format is SB and the data processing flag (UnipolarFlag) is 0, convert az & cal to two's compliment
*/
    if(( c_blk->data_format == TC_SELECT && UnipolarFlag == 1) || ( c_blk->data_format == SB_SELECT && UnipolarFlag == 0 ))
    {
      az = (double) (c_blk->s_az_buf  ^ 0x8000);
      as = (double) (c_blk->s_cal_buf ^ 0x8000);
    }
    else /* process data with out conversion */
    {
      az = (double) c_blk->s_az_buf;
      as = (double) c_blk->s_cal_buf;
    }

    slope = ( calhi - callo ) / ( as - az );	/* compute intermediate values outside of loops */
    seed = (double)65536.0 * slope / i_span;
    seed1 = (callo - i_zero) / slope - az;

/*printf("Slope %f CH %f CL %f AZ %f AS %f\nIZ %f IS %f S0 %f S1 = %f\n",slope, calhi, callo, az, as, i_zero, i_span, seed, seed1);*/

    for( j = 0; j < SA_CHANS; j++) /* for every input channel */
    {
      rp = c_blk->s_raw_buf[j];	/* get pointers into both data buffers */
      cp = c_blk->s_cor_buf[j];

      for( i = 0; i < SA_SIZE; i++ ) /* correct all channel data elements */
      {
/*
If the ADC data format is BTC and the data processing flag (UnipolarFlag) is 1, convert raw input to Straight Binary
                                      ----  OR  ----
If the ADC data format is SB and the data processing flag (UnipolarFlag) is 0, convert raw input to two's compliment
*/
        if(( c_blk->data_format == TC_SELECT && UnipolarFlag == 1) || ( c_blk->data_format == SB_SELECT && UnipolarFlag == 0 ))
           w.u = (rp[i] ^ 0x8000);
        else	/* process data with out conversion */
           w.u = rp[i];

        /* If the data processing flag UnipolarFlag is 1, use unsigned data type */
        if( UnipolarFlag == 1)
            temp = seed * ((double)w.u + seed1);	/* SB */
        else	/* use signed data type */
            temp = seed * ((double)w.s + seed1);	/* BTC */

        temp += (temp < 0.0) ? -0.5 : 0.5;	/* round */
        temp = fmin(temp, 65535.0);		/* clip low */
        temp = fmax(temp, 0.0);			/* clip high */
        cp[i] = (int)temp;			/* save corrected result */

/*printf("RD %X T %f CV %f CDx %X\n", rp[i], temp, ((((double)cp[i] * i_span) / (double)65536.0) + i_zero), cp[i]);*/
      }
    }
    return(0);
}

