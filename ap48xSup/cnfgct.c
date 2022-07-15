
#include "../apcommon/apcommon.h"
#include "AP48X.h"

/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        ReadCounter

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FM

    ABSTRACT:           This file retrieves the read back register value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
  MODULES FUNCTION DETAILS

  Read the value of a counter.
  The current value of the counter is placed at the address specified by val.
  Returns parameter out of range error if the counter value is invalid.
  Returns invalid pointer error if val is NULL, else returns success.
*/


APSTAT ReadCounter(struct ap48x *c_blk, int counter, uint32_t *val)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( val == NULL )			/* valid range ? */
	return(	InvalidPointer );

    /* write a 1 to the counter load read back register bit before reading */
    output_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->LoadReadBack), (long)(1 << counter));

    /* read counter */
    *val = (uint32_t)input_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->Counter[counter].CounterReadBack));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        ReadCounterControl

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FM

    ABSTRACT:           This file retrieves the counter control register value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Read the value of a counter control register.
  The current value is placed at the address specified by val.
  Returns parameter out of range error if the counter value is invalid.
  Returns invalid pointer error if val is NULL, else returns success.
*/


APSTAT ReadCounterControl(struct ap48x *c_blk, int counter, uint32_t *val)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( val == NULL )			/* valid range ? */
	return(	InvalidPointer );

    *val = (uint32_t)input_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->Counter[counter].CounterControl));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        WriteCounterControl

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FM

    ABSTRACT:           This file Write the counter control register value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Write counter control register.
  The current value of 'val' is placed into the counter control reg.
  Returns parameter out of range error if the counter value is invalid,
  else returns success.
*/


APSTAT WriteCounterControl(struct ap48x *c_blk, int counter, uint32_t val)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    output_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->Counter[counter].CounterControl), val);

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetCounterConstantAx

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This file retrieves the counter constant register value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Read the value of a counter constant.
  The current value of the register is placed at the address specified by val.
  Returns parameter out of range error if the counter value is invalid.
  Returns invalid pointer error if val is NULL, else returns success.
  x is 1 or 2
*/


APSTAT GetCounterConstantAx(struct ap48x *c_blk, int counter, uint32_t *val, int x)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( val == NULL )			/* valid range ? */
	return(	InvalidPointer );

    if( x == 2 )	/* A2 ? */
	*val = (uint32_t)c_blk->m_CounterConstantA2[counter]; /* get A2 */
    else
	*val = (uint32_t)c_blk->m_CounterConstantA1[counter]; /* get A1 */

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetCounterConstantAx

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This file stores the counter constant value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Set the counter constant.
  Returns parameter out of range error if the counter value is invalid
  else returns success.
  x is 1 or 2
*/


APSTAT SetCounterConstantAx(struct ap48x *c_blk, int counter, uint32_t val, int x)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( x == 2 )	/* A2 ? */
	c_blk->m_CounterConstantA2[counter] = val;  /* save in A2 */
    else
	c_blk->m_CounterConstantA1[counter] = val;  /* save in A1 */

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetCounterConstantBx

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This file retrieves the counter constant register value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Read the value of a counter constant.
  The current value of the counter constant register is placed at
  the address specified by val.
  Returns parameter out of range error if the counter value is invalid.
  Returns invalid pointer error if val is NULL, else returns success.
  x is 1 or 2
*/


APSTAT GetCounterConstantBx(struct ap48x *c_blk, int counter, uint32_t *val, int x)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( val == NULL )			/* valid range ? */
	return(	InvalidPointer );

    if( x == 2 )	/* B2 ? */
	*val = (uint32_t)c_blk->m_CounterConstantB2[counter]; /* get B2 */
    else
	*val = (uint32_t)c_blk->m_CounterConstantB1[counter]; /* get B1 */

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetCounterConstantBx

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This file stores the counter constant value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Set the counter constant.
  Returns parameter out of range error if the counter value is invalid
  else returns success.
  x is 1 or 2
*/


APSTAT SetCounterConstantBx(struct ap48x *c_blk, int counter, uint32_t val, int x)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( x == 2 )	/* B2 ? */
	c_blk->m_CounterConstantB2[counter] = val;  /* save B2 */
    else
	c_blk->m_CounterConstantB1[counter] = val;  /* save B1 */

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        WriteCounterConstant

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine write the stored counter constant value
                        to the board registers.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

  Write a value to the counter constant register.
  Returns parameter out of range error if the counter value is invalid
  else returns success.
*/


APSTAT WriteCounterConstant(struct ap48x *c_blk, int counter)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    /* 32bit A1 */
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->Counter[counter].CounterConstantA1,
			c_blk->m_CounterConstantA1[counter] );

    /* 32bit A2 */
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->Counter[counter].CounterConstantA2,
			c_blk->m_CounterConstantA2[counter] );

    /* 32bit B1 */
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->Counter[counter].CounterConstantB1,
			c_blk->m_CounterConstantB1[counter] );

    /* 32bit B2 */
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->Counter[counter].CounterConstantB2,
			c_blk->m_CounterConstantB2[counter] );

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetMode

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This file retrieves the current MODE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get mode value for counter
 The current value is placed at the address specified by mode.
 Returns parameter out of range error if the counter value is invalid.
 Returns Invalid pointer if 'mode' is NULL, else returns success.
*/

APSTAT GetMode(struct ap48x *c_blk, int counter, BYTE *mode)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( mode == NULL )			/* valid range ? */
	return(	InvalidPointer );

    *mode = c_blk->m_Mode[counter];

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetMode

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine stores the MODE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set mode
 Returns parameter out of range error if the 'counter' or 'mode' value
 is invalid, else returns success.
*/

APSTAT SetMode(struct ap48x *c_blk, int counter, BYTE mode)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if(  mode > OneShot )	  /* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_Mode[counter] = mode;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetInterruptEnable

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current INTERRUPT ENABLE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get interrupt enable
 The current value is placed at the address specified by enable.
 Returns parameter out of range error if the counter value is invalid.
 Returns Invalid pointer if 'enable' is NULL, else returns success.
*/

APSTAT GetInterruptEnable(struct ap48x *c_blk, int counter, BOOL *enable)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( enable == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *enable = c_blk->m_InterruptEnable[counter];

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetInterruptEnable

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the INTERRUPT ENABLE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set interrupt enable
 Bit 0 set in this register enables the corresponding counter interrupt.
 Returns parameter out of range error if the 'counter' or 'enable'
 values are invalid.
*/

APSTAT SetInterruptEnable( struct ap48x *c_blk, int counter, BOOL enable)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if(enable < IntDisable || enable > IntEnable )	/* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_InterruptEnable[counter] = enable;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetClockSource

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current CLOCK SOURCE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get clock source
 Returns parameter out of range error if the 'counter' value is invalid.
 The current value is placed at the address specified by 'source'.
 Returns Invalid pointer if 'source' is NULL, else returns success.
*/

APSTAT GetClockSource(struct ap48x *c_blk, int counter, BYTE *source)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( source == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *source = c_blk->m_ClockSource[counter];

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetClockSource

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the CLOCK SOURCE value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set clock source
 Returns parameter out of range error if the 'counter' value is invalid.
 Returns parameter out of range error if the source value is invalid
 else returns success.
*/

APSTAT SetClockSource(struct ap48x *c_blk, int counter, BYTE source)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_ClockSource[counter] = source;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetOutputPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current OUTPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get output polarity
 Bit 0 set in a register enables the active LOW output polarity.
 Returns parameter out of range error if the 'counter' value is invalid.
 The current value is placed at the address specified by polarity.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT GetOutputPolarity(struct ap48x *c_blk, int counter, BOOL *polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *polarity = c_blk->m_OutputPolarity[counter];

    return( Success );
}


/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetOutputPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the OUTPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set output polarity
 Bit 0 set in a register enables the active LOW output polarity.
 Returns parameter out of range error if the 'counter' or polarity values
 are invalid else returns success.
*/

APSTAT SetOutputPolarity(struct ap48x *c_blk, int counter, BOOL polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity < OutPolLow || polarity > OutPolHi )	/* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_OutputPolarity[counter] = polarity;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetInputAPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get input A polarity
 Returns parameter out of range error if the 'counter' value is invalid.
 The current value is placed at the address specified by polarity.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT GetInputAPolarity(struct ap48x *c_blk, int counter, BYTE *polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *polarity = c_blk->m_InputAPolarity[counter];

    return( Success );
}


/*
{+D}
    FILENAME:           cnfg48.c

    MODULE NAME:        SetInputAPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set input A polarity
 Set the values of the input polarity registers.
 Returns parameter out of range error if the polarity value is invalid
 else returns success.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT SetInputAPolarity(struct ap48x *c_blk, int counter, BYTE polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity > InAX4 )	/* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_InputAPolarity[counter] = polarity;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetInputBPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get input B polarity
 Returns parameter out of range error if the 'counter' value is invalid.
 The current value is placed at the address specified by polarity.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT GetInputBPolarity(struct ap48x *c_blk, int counter, BYTE *polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *polarity = c_blk->m_InputBPolarity[counter];

    return( Success );
}


/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetInputBPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set input B polarity
 Set the values of the input polarity registers.
 Returns parameter out of range error if the polarity value is invalid
 else returns success.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT SetInputBPolarity(struct ap48x *c_blk, int counter, BYTE polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity > InABCPolHi )	/* valid range ? */
    return( ParameterOutOfRange );

    c_blk->m_InputBPolarity[counter] = polarity;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetInputCPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get input C polarity
 Returns parameter out of range error if the 'counter' value is invalid.
 The current value is placed at the address specified by polarity.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT GetInputCPolarity(struct ap48x *c_blk, int counter, BYTE *polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity == NULL )			/* valid range ? */
	return(	InvalidPointer );

    *polarity = c_blk->m_InputCPolarity[counter];

    return( Success );
}


/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetInputCPolarity

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the INPUT POLARITY Value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set input C polarity
 Set the values of the input polarity registers.
 Returns parameter out of range error if the polarity value is invalid
 else returns success.
 Returns Invalid pointer if 'polarity' is NULL, else returns success.
*/

APSTAT SetInputCPolarity(struct ap48x *c_blk, int counter, BYTE polarity)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( polarity > InCQuadMax )	/* valid range ? */
    return( ParameterOutOfRange );

    c_blk->m_InputCPolarity[counter] = polarity;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        GetDebounce

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the current DEBOUNCE value.

    CALLING SEQUENCE:

    MODULE TYPE:

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

 Get Debounce for counters
 The current value is placed at the address specified by mode.
 Returns parameter out of range error if the counter value is invalid.
 Returns Invalid pointer if 'debounce' is NULL, else returns success.
*/

APSTAT GetDebounce(struct ap48x *c_blk, int counter, BOOL *debounce)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( debounce == NULL )		/* valid range ? */
	return(	InvalidPointer );

    *debounce = c_blk->m_Debounce[counter];

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetDebounce

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the DEBOUNCE value.

    CALLING SEQUENCE:

    MODULE TYPE:

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

 Set Debounce for counters
 Valid values for "debounce" are 0 and 1.
 Returns parameter out of range error if the 'counter' or 'debounce' value
 is invalid, else returns success.
*/

APSTAT SetDebounce(struct ap48x *c_blk, int counter, BOOL debounce)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if(debounce < DebounceOff || debounce > DebounceOn )/* valid range ? */
	return( ParameterOutOfRange);

    c_blk->m_Debounce[counter] = debounce;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        StopCounter

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine stops the counter/timer function.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Stop counter
 disables a counter timer.
 Returns parameter out of range error if the counter value is invalid
 else returns success.
*/

APSTAT StopCounter(struct ap48x *c_blk, int counter)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    output_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->CounterStop), (long)(1 << counter)); /* Stop counter */

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        StopSimultaneousCounters

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will issue a software trigger to stop
                        multiple channels.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Stop Simultaneous counter
 Stops one counter, any combination, or all counter timers.
 Bit mask of counters to stop bit11 ... bit0
 Returns success.
*/

APSTAT StopSimultaneousCounters(struct ap48x *c_blk, word mask)
{
    output_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->CounterStop ), (long)(mask & 0xFFF));
    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        DisableInterrupt

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine disables the interrupt the current channel.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Disable counter interrupts
 Returns parameter out of range error if the counter value is invalid
 else returns success.
*/

APSTAT DisableInterrupt(struct ap48x *c_blk, int counter)
{
    uint32_t bpos;	/* bit position */
    uint32_t nValue;	/* current value of register */

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    bpos = 1 << counter;	/* get counters bit position */

    /* read the value from hardware */
    nValue = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->CounterInterruptEnable);

    /* Disable counter interrupt */
    output_long( c_blk->nHandle, (long*)(&c_blk->brd_ptr->CounterInterruptEnable), ( nValue & ~bpos ));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        EnableInterrupt

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine enables the interrupt the current channel.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Enable counter interrupts
 Returns parameter out of range error if the counter value is invalid
 else returns success.
*/

APSTAT EnableInterrupt(struct ap48x *c_blk, int counter)
{
    uint32_t bpos;	/* bit position */
    uint32_t nValue;	/* current value of register */

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    bpos = 1 << counter;	/* get counters bit position */

    /* read the value from hardware */
    nValue = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->CounterInterruptEnable);

    /* Enable counter interrupt */
    output_long( c_blk->nHandle, (long*)(&c_blk->brd_ptr->CounterInterruptEnable), ( nValue | bpos ));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        StartCounter

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will issue a software trigger to one channel.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Start a counter
 Starts one counter timer.
 Returns parameter out of range error if the counter value is invalid
 else returns success.
*/

APSTAT StartCounter(struct ap48x *c_blk, int counter)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    /* Trigger counter */

    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->CounterTrigger, (long)(1 << counter));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        StartSimultaneousCounters

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will issue a software trigger to multiple channels.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Start Simultaneous counter
 Starts one counter, any combination, or all counter timers.
 Bit mask of counters to start bit11 ... bit0
 Returns success.
*/

APSTAT StartSimultaneousCounters(struct ap48x *c_blk, word mask)
{
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->CounterTrigger, (long)(mask & 0xFFF));
    return( Success );
}


/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        ToggleConstant

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine toggles the counter constant function.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}

*/

/*
    MODULES FUNCTION DETAILS

 Toggle counter constant
 Returns parameter out of range error if the counter value is invalid
 else returns success.
*/

APSTAT ToggleConstant(struct ap48x *c_blk, int counter)
{
    uint32_t bpos;	/* bit position */
    uint32_t nValue;	/* current value of register */

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    bpos = 1 << counter;	/* get counters bit position */

    /* read the value from hardware */
    nValue = input_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->ToggleConstant);

    if( nValue & bpos ) /* if the bit is set... clear it */
      output_long( c_blk->nHandle, (long*)(&c_blk->brd_ptr->ToggleConstant), ( nValue & ~bpos ));
    else		/* else set the bit */
      output_long( c_blk->nHandle, (long*)(&c_blk->brd_ptr->ToggleConstant), ( nValue | bpos ));

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        ToggleSimultaneousConstants

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will toggle counter constants for multiple channels.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:


    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Toggle Simultaneous counter constants
 Toggles one counter, any combination, or all counter constants for the timers.
 Bit mask of counters to stop bit11 ... bit0
 Returns success.
*/

APSTAT ToggleSimultaneousConstants(struct ap48x *c_blk, word mask)
{
    output_long( c_blk->nHandle, (long *)(&c_blk->brd_ptr->ToggleConstant ), (long)(mask & 0xFFF));
    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        ConfigureCounterTimer

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will poll all of the control options,
                        create the control word and write it out to the board.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Configure counter timer
*/

APSTAT ConfigureCounterTimer(struct ap48x *c_blk, int counter)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    /* Disable interrupts */
    DisableInterrupt(c_blk, counter);

    /* disable/stop the counter */
    StopCounter(c_blk, counter);

    /* clear any pending interrupts from counter */
    output_long( c_blk->nHandle, (long *)&c_blk->brd_ptr->InterruptStatusReg, (long)(1 << counter));

    /* write counter constants */
    WriteCounterConstant(c_blk, counter );

    /* Build up and install the new control register value */
    WriteCounterControl(c_blk, counter, build_control48x(c_blk, counter) );

    /* enable interrupts ? */
    if(c_blk->m_InterruptEnable[counter] == IntEnable )
	EnableInterrupt(c_blk, counter);

    return( Success );
}



/*
{+D}

    FILENAME:           cnfg48x.c

    MODULE NAME:        GetSpecialInterrupt

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine gets the special interrupt condition value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Get special interrupt condition
 Bit 0 - 2 in this register are special interrupt condition.
 Returns parameter out of range error if the 'counter' or 'special' values are invalid.
 Returns Invalid pointer if 'enable' is NULL, else returns success.
*/

APSTAT GetSpecialInterrupt(struct ap48x *c_blk, int counter, BYTE *special)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    if( special == NULL )
	return(	InvalidPointer );

    *special = c_blk->m_SpecialIC[counter];

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    MODULE NAME:        SetSpecialInterrupt

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine sets the special interrupt condition value.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Set special interrupt condition
 Bit 0 - 2 set in this register enables the special interrupt condition.
 Returns parameter out of range error if the 'counter' or 'special'
 values are invalid.
*/

APSTAT SetSpecialInterrupt(struct ap48x *c_blk, int counter, BYTE special)
{
    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( ParameterOutOfRange );

    c_blk->m_SpecialIC[counter] = special;

    return( Success );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will issue a software trigger to one channel
			then block until the channel generates an interrupt.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*

    MODULES FUNCTION DETAILS

 Function to read the counter IIR

 When an interrupt for a counter is serviced the interrupt information register (IIR)
 for that counter is read and saved.    The latest interrupt information register value
 for the specified counter is returned by this function.   The read of a counter IIR
 causes its last saved value to be cleared.

 Returns 0 if the counter value or handle is invalid
 else returns the counter IIR value.
*/


uint32_t read48x_CounterIIR(int nHandle, uint32_t counter)
{
    APDATA_STRUCT* pAP;	/*  local */
    unsigned long data[2];

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( (uint32_t)0 );

    pAP = GetAP(nHandle);
    if(pAP == NULL)
	return((uint32_t)0);

    /* place counter index to read from in data[0] */
    data[0] = (unsigned long) counter;

    /* pram3 = function: 1=read8bits,2=read16bits,4=read32bits,8=read48x_CounterIIR */
    data[1] = (unsigned long) pAP->nDevInstance;	/* Device Instance */
    read( pAP->nAPDeviceHandle, &data[0], 8 );
    return( (uint32_t)SwapLong( (long)data[1] ) );
}



/*
{+D}
    FILENAME:           cnfg48x.c

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will issue a software trigger to one channel
			then block until the channel generates an interrupt.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------

{-D}
*/

/*
    MODULES FUNCTION DETAILS

 Blocking Start Counter48x
 Starts one counter timer then blocks.

 Returns 0 if the counter value or handle is invalid
 else returns the counter IIR value.
*/


int BlockingStartCounter48x(struct ap48x *c_blk, int counter)
{
    APDATA_STRUCT* pAP;	/*  local */
    unsigned long data[5];

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( 0 );

    pAP = GetAP(c_blk->nHandle);
    if(pAP == NULL)
	return(0);

    /* place counter trigger address to write to in data[0] */
    data[0] = (unsigned long) &c_blk->brd_ptr->CounterTrigger;

    /* place data value to write at above address in data[1] */
    data[1] = (unsigned long) SwapLong(1 << counter);

    /* place transfer size flag in data[2]... flag=0=byte write, flag=1=word write, flag=2=long write */
    data[2] = (unsigned long)2;

    /* place board instance index in data[3] */
    data[3] = (unsigned long) pAP->nDevInstance;	/* Device Instance */

    /* place counter number in data[4] */
    data[4] = (unsigned long) counter;

    /* pram3 = function: 8=blocking_start_convert */
    write( pAP->nAPDeviceHandle, &data[0], 8 );

    return( (int)SwapLong( (long)data[1] ) );	/* returns the counter IIR value */
}



/*
{+D}
    FILENAME:           cnfg48x.c

    VERSION:		A

    CREATION DATE:      12/01/15

    CODED BY:           FJM

    ABSTRACT:           This routine will generate the control word for a counter.

    CALLING SEQUENCE:

    MODULE TYPE:

    I/O RESOURCES:

    SYSTEM RESOURCES:

    MODULES CALLED:

    REVISIONS:

    DATE       BY	PURPOSE
    --------   -----	---------------------------------------------------


{-D}
*/

/*

    MODULES FUNCTION DETAILS

 Function to generate control word for counters

 Returns 0 if the counter value is invalid
 else returns the counter control word.
*/

uint32_t build_control48x(struct ap48x *c_blk, int counter)
{
    uint32_t control;

    if( counter < 0 || counter >= MAX_CNTR )	/* valid range ? */
	return( (uint32_t)0 );

    control = (uint32_t)(c_blk->m_Mode[counter] & 7);

    if( control == QuadPosition )
    {
      control |= ((c_blk->m_InputBPolarity[counter] & 1) << 6);/* select input B polarity */
      control |= ((c_blk->m_InputCPolarity[counter] & 7) << 7);/* select input C index */
      control |= ((c_blk->m_SpecialIC[counter] & 3) << 13);   /* select special interrupt condition */
    }
    else
    {
      control |= ((c_blk->m_InputBPolarity[counter] & 3) << 6);/* select input B polarity */
      control |= ((c_blk->m_InputCPolarity[counter] & 3) << 8);/* select input C polarity */
    }

    control |= ((c_blk->m_InputAPolarity[counter] & 3) << 4);/* select input a polarity */

    if(c_blk->m_OutputPolarity[counter] == OutPolHi)	/* select output polarity */
       control |= 8;

    control |= ((c_blk->m_ClockSource[counter] & 7) << 10);

    if(c_blk->m_Debounce[counter] == DebounceOn)	/* select debounce */
       control |= 0x8000;

    return(control);
}

