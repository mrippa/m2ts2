
#include "../apcommon/apcommon.h"
#include <string.h>
#include "AP48X.h"

/*
{+D}
    SYSTEM:	    Acromag AP48x I/O Board

    FILENAME:       drvr48x.c

    MODULE NAME:    main - main routine of example software.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    F.M.

    CODED BY:	    F.M.

    ABSTRACT:	    This module is the main routine for the example program
                    which demonstrates how the Library is used.

    CALLING
	SEQUENCE:

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

    This module is the main routine for the example program
    which demonstrates how the Library is used.
*/

int main(argc, argv)
int argc; char *argv[];
{


/*
    DECLARE LOCAL DATA AREAS:
*/

    char cmd_buff[32];		/* command line input buffer */
    unsigned finished;		/* flag to exit program */
    int item;			/* menu item selection variable */
    unsigned point;		/* I/O point number */
    unsigned val;		/* value of port or point */
    APSTATUS hstatus;		/* interrupt handler returned status */
    long addr;			/* integer to hold board address */
    int i;			/* loop index */
    int hflag;			/* interrupt handler installed flag */
    uint32_t ulong_value;
    struct ap48x c_block48x;	/* configuration block */
    long status;
    int ap_instance = 0;

/*
    ENTRY POINT OF ROUTINE:
    INITIALIZATION
*/

    if(argc == 2)	/* device instance specified ? */
      ap_instance = atoi(argv[1]);

    finished = 0;	/* indicate not finished with program */
    hflag = 0;          /* indicate interrupt handler not installed yet */

/*
    Initialize the Configuration Parameter Block to default values.
*/
    memset(&c_block48x, 0, sizeof(c_block48x));

    c_block48x.bAP = FALSE;	/* indicate not initialized and set up yet */
    c_block48x.bInitialized = FALSE; /* indicate not ready to talk to AP48x */
    c_block48x.nHandle = 0;	/* make handle to a closed AP48x board */

    for( i = 0; i < MAX_CNTR; i++ )
    {
      c_block48x.m_CounterConstantA1[i] = 0xFFFFFFFF; /* constant registers */
      c_block48x.m_CounterConstantA2[i] = 0xFFFFFFFF; /* constant registers */
      c_block48x.m_CounterConstantB1[i] = 0xFFFFFFFF; /* constant registers */
      c_block48x.m_CounterConstantB2[i] = 0xFFFFFFFF; /* constant registers */

      c_block48x.m_Mode[i] = None;                       /* the counter mode */
      c_block48x.m_OutputPolarity[i] = OutPolLow;        /* output polarity */
      c_block48x.m_InputAPolarity[i] = InABCPolDisabled; /* input A polarity */
      c_block48x.m_InputBPolarity[i] = InABCPolDisabled; /* input B polarity */
      c_block48x.m_InputCPolarity[i] = InABCPolDisabled; /* input C polarity */
      c_block48x.m_ClockSource[i] = InC1_1953125Mhz;     /* clock source */
      c_block48x.m_SpecialIC[i] = Nosis;		 /* No Output or Special Interrupt Selected */
      c_block48x.m_InterruptEnable[i] = IntDisable;      /* interrupt enable */
      c_block48x.m_Debounce[i] = DebounceOff;            /* Debounce disabled */
    }
    c_block48x.counter_num = 0;

    if(InitAPLib() != S_OK)
    {
	  printf("\nUnable to initialize the AP library. Exiting program.\n");
	  exit(0);
    }

/*
	Open an instance of a AP device
	Other device instances can be obtained
	by changing parameter 1 of APOpen()
*/

    if(APOpen(ap_instance, &c_block48x.nHandle, DEVICE_NAME ) != S_OK)
    {
	  printf("\nUnable to Open instance of AP48X.\n");
	  finished = 1;	 /* indicate finished with program */
    }
    else
    {
      if(APInitialize(c_block48x.nHandle) == S_OK)/* Initialize AP48x */
      {
	  GetAPAddress(c_block48x.nHandle, &addr);	/* Read back address */
	  c_block48x.brd_ptr = (struct map48x *)addr;
	  c_block48x.bInitialized = TRUE;
	  c_block48x.bAP = TRUE;
      }
    }

/*
    Enter main loop
*/

    while(!finished)
    {
      printf("\n\nAP48x Counter/Timer Library Demonstration Rev. A\n\n");
      printf(" 1. Exit this Program\n");
      printf(" 2. Examine/Change Current Counter\n");
      printf(" 3. Set Up Configuration Parameters\n");
      printf(" 4. Configure Control Register\n");
      printf(" 5. Attach Exception Handler\n");
      printf(" 6. Detach Exception Handler\n");
      printf(" 7. Trigger/Stop/Toggle Constant Control\n");
      printf(" 8. Display Counter Control Registers\n");
      printf(" 9. Display Read Back Registers\n");
      printf("10. Examine/Change Counter Constant\n");
      printf("11. Write Counter Constant Registers\n");
      printf("12. Disable Interrupt\n");
      printf("13. Read Input Point\n");
      printf("14. Read Input Port\n");
      printf("15. Write Output Point\n");
      printf("16. Write Output Port\n");
      printf("17. Read Status\n");
      printf("\nSelect: ");
      scanf("%d",&item);

/*
    perform the menu item selected.
*/

      switch(item)
      {
        case 1: /* exit program command */
           printf("Exit program(y/n)?: ");
           scanf("%s",cmd_buff);
           if( cmd_buff[0] == 'y' || cmd_buff[0] == 'Y' )
              finished++;
        break;

        case 2: /* counter number */
            selectcnt48x(&c_block48x);
        break;

        case 3: /* set up configuration block parameters */
	    setconf48x(&c_block48x);
        break;

        case 4: /* configure */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
	        ConfigureCounterTimer(&c_block48x, c_block48x.counter_num);
printf("\n\nControl Word: %04x\n",build_control48x(&c_block48x,c_block48x.counter_num));
	    }
        break;

        case 5:     /* attach exception handler */
	    if(hflag)
		printf("\n>>> ERROR: HANDLERS ARE ATTACHED <<<\n");
	    else
	    {
		hstatus = EnableAPInterrupts(c_block48x.nHandle);
		if(hstatus != S_OK)
		{
		   printf(">>> ERROR WHEN ENABLING INTERRUPTS <<<\n");
		   hflag = 0;
		}
		else
		{
		   hflag = 1;
		   printf("\nHandlers are now attached\n");
	   	}
	    }
        break;
        
        case 6: /* detach exception handlers */
		hflag = 0;
		DisableAPInterrupts(c_block48x.nHandle);
        break;

        case 7: /* Software trigger control */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	        trig48x(&c_block48x);
        break;

        case 8: /* Read Counter Control registers */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		for( i = 0; i < MAX_CNTR; i++ )
		{
		   ReadCounterControl(&c_block48x, i, &ulong_value );
		   printf("\nCounter Control %02d = %08X",i,ulong_value);

		   ReadCounterControl(&c_block48x, ++i, &ulong_value );
		   printf("    %02d = %08X", i, ulong_value );
		}
            }
        break;

        case 9: /* Display Read Back registers */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		for( i = 0; i < MAX_CNTR; i++ )
		{
		   ReadCounter(&c_block48x, i, &ulong_value );
		   printf("\nRead Back Register %02d = %08X",i,ulong_value);

		   ReadCounter(&c_block48x, ++i, &ulong_value );
		   printf("    %02d = %08X", i, ulong_value);
		}
            }
        break;
  
        case 10: /* select counter constant */
            selectcon48x(&c_block48x);
        break;

        case 11: /* Write counter constant */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD ADDRESS NOT SET <<<\n");
	    else
	        WriteCounterConstant(&c_block48x, c_block48x.counter_num);
        break;

        case 12: /* Disable the interrupt on the current channel */
            if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	        DisableInterrupt(&c_block48x, c_block48x.counter_num);
        break;

	case 13: /* Read Digital Input Point */
	    if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		printf("\nEnter Input point number (0 - 1): ");
		scanf("%d",&point);
		status = rpntdio(&c_block48x, 0 ,point); /* port input or output. 0=Input, 1=output */
		if(status == -1)
		    printf("\n>>> ERROR: PARAMETER OUT OF RANGE <<<\n");
		else
		    printf("\nValue of point %d: %X\n",(unsigned int)point,(unsigned int)status);
	    }
	break;

	case 14: /* Read Digital Input Port */
	    if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		status = rprtdio(&c_block48x, 0); /* port input or output. 0=Input, 1=output */
		if(status == -1)
		    printf("\n>>> ERROR: PARAMETER OUT OF RANGE <<<\n");
		else
		    printf("\nValue = %X\n",(unsigned int)status);
	    }
	break;

	case 15: /* Write Digital Point */
	    if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		printf("\nEnter I/O point number  (0 - 5): ");
		scanf("%d",&point);
		printf("\nEnter point value (0 - 1): ");
		scanf("%x",&val);
		status = wpntdio(&c_block48x,point,val);
		if(status == -1)
		    printf("\n>>> ERROR: PARAMETER OUT OF RANGE <<<\n");
	    }
	    break;

	case 16: /* Write Digital Port */
	    if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
	    {
		printf("\nEnter output value in hex (0 - 0x3F): ");
		scanf("%x",&val);
		status = wprtdio(&c_block48x,val);
		if(status == -1)
		    printf("\n>>> ERROR: PARAMETER OUT OF RANGE <<<\n");
	    }
	break;

	case 17:     /* read board status command */
	    if(!c_block48x.bInitialized)
		printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
	    else
		psts48x(&c_block48x);  /* read board */
	break;

      }   /* end of switch */
    }	/* end of while */

    StopSimultaneousCounters(&c_block48x, 0xffff );	/* stop all */

    DisableAPInterrupts(c_block48x.nHandle);
    if(c_block48x.bAP == TRUE)
	  APClose(c_block48x.nHandle);

    printf("\nEXIT PROGRAM\n");
    return(0);
}   /* end of main */



/*
{+D}
    SYSTEM:         AP48x Software

    MODULE NAME:    setconf48x - set configuration block contents.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:	    Routine which is used to enter parameters into
		    the Configuration Block.

    CALLING
	SEQUENCE:   setconf48x(c_block48x)
		    where:
			c_block48x (structure pointer)
			  The address of the configuration param. block

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

void setconf48x(c_blk)
struct ap48x *c_blk;
{

/*
    DEFINITIONS:
*/


/*
    DECLARE LOCAL DATA AREAS:
*/
    int item;		      /* menu item variable */
    int counter;              /* counter number */
    unsigned finished;        /* flag to exit loop */
    BYTE byte_value;          /* storage for retieved data */
    BOOL bool_value;          /* storage for retreived data */

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
        counter = c_blk->counter_num;
        printf("\n\n\n\n");
        printf("\n\nCurrent Counter %d Configuration Parameters\n\n",counter);
	printf(" 1. Return to Previous Menu\n");
        printf(" 2. Board Pointer:         %lX\n",(unsigned long)c_blk->brd_ptr);
        GetMode(c_blk, counter, &byte_value);
        printf(" 3. Counter Mode:          %X\n",byte_value);
        GetOutputPolarity(c_blk, counter, &bool_value);
        printf(" 4. Output Polarity:       %X\n",bool_value);
        GetInputAPolarity(c_blk, counter, &byte_value);
        printf(" 5. Input A Polarity:      %X\n",byte_value);
        GetInputBPolarity(c_blk, counter, &byte_value);
        printf(" 6. Input B Polarity:      %X\n",byte_value);
        GetInputCPolarity(c_blk, counter, &byte_value);
        printf(" 7. Input C Polarity:      %X\n",byte_value);
        GetClockSource(c_blk, counter, &byte_value);
        printf(" 8. Clock Source:          %X\n",byte_value);
        GetDebounce(c_blk, counter, &bool_value);
        printf(" 9. Debounce:              %X\n",bool_value);
        GetInterruptEnable(c_blk, counter, &bool_value);
        printf("10. Interrupt Enable:      %X\n",bool_value);
	GetMode(c_blk, counter, &byte_value);
	if( byte_value == QuadPosition )
	{
	   GetSpecialInterrupt(c_blk, counter, &byte_value);
	   printf("11. Special Condition:     %X\n",byte_value);
	}

        printf("\nselect: ");
        scanf("%d",&item);
	switch(item)
	{
	case 1: /* return to previous menu */
	    finished++;
	    break;

        case 3: /* mode */
            printf("0 - Disabled\n");
            printf("1 - Quadrature Position\n");
            printf("2 - Pulse Width Modulation\n");
            printf("3 - Watchdog Timer\n");
            printf("4 - Event Counter/Frequency Measurement\n");
            printf("5 - Input Pulse Width Measurement\n");
            printf("6 - Input Period Measurement\n");
            printf("7 - One-shot Output Pulse\n\n");

            byte_value = (BYTE)get_param();
            SetMode(c_blk, counter, byte_value);
	    break;

        case 4: /* output polarity */
            printf("0 - Output Polarity Low\n");
            printf("1 - Output Polarity High\n\n");
            bool_value = (BOOL)get_param();
            SetOutputPolarity(c_blk, counter, bool_value);
	    break;

        case 5: /* input A polarity */
            printf("0 - In A Polarity Disabled\n");
	    GetMode(c_blk, counter, &byte_value);
	    if( byte_value == QuadPosition )
	    {
	        printf("1 - X1 Encoding\n");
	        printf("2 - X2 Encoding\n");
	        printf("3 - X4 Encoding\n");
	    }
	    else
	    {
	        printf("1 - In A Polarity Active Low\n");
	        printf("2 - In A Polarity Active High\n\n");
	    }
	    byte_value = (BYTE)get_param();
	    SetInputAPolarity(c_blk, counter, byte_value);
	    break;

        case 6: /* input B polarity */
            printf("0 - In B Polarity Disabled\n");
	    GetMode(c_blk, counter, &byte_value);
	    if( byte_value == QuadPosition )
	       printf("1 - In B Polarity Enabled\n");
	    else
	    {
	      printf("1 - In B Polarity Active Low\n");
              printf("2 - In B Polarity Active High\n\n");
            }
            byte_value = (BYTE)get_param();
            SetInputBPolarity(c_blk, counter, byte_value);
            break;

        case 7: /* input C polarity */
            printf("0 - In C Polarity Disabled\n");
	    GetMode(c_blk, counter, &byte_value);
	    if( byte_value == QuadPosition )
	    {
	        printf("1 - A = 0, B = 1\n");
	        printf("2 - A = 1, B = 0\n");
	        printf("3 - A = 1, B = 1\n");
	        printf("4 - A = 0, B = 0\n");
	    }
	    else
	    {
	        printf("1 - In C Polarity Active Low\n");
	        printf("2 - In C Polarity Active High\n");
		if( byte_value == Watchdog )
	            printf("3 - In C Polarity Gate-Off Control\n");
		if( byte_value == EventCounting )
	            printf("3 - In C Polarity Up/Down Control\n");
	    }
	    printf("\n");
            byte_value = (BYTE)get_param();
            SetInputCPolarity(c_blk, counter, byte_value);
            break;

        case 8: /* clock source */
	    GetMode(c_blk, counter, &byte_value);
	    if( byte_value == EventCounting)
	    {
	      printf("0 - Event Counting\n");
	      printf("7 - Frequency Measurement\n\n");
	    }
	    else
	    {
	      printf("0 - Internal  1.953125MHz Clock\n");
	      printf("1 - Internal  3.90625MHz Clock\n");
	      printf("2 - Internal  7.8125MHz Clock\n");
	      printf("3 - Internal 15.625MHz Clock\n");
	      printf("4 - Internal 62.5MHz Clock\n");
	      printf("5 - Undefined\n");
	      printf("6 - CNTInB\n");
	      printf("7 - External Clock Enable\n\n");
	    }
            byte_value = (BYTE)get_param();
            SetClockSource(c_blk, counter, byte_value);
	    break;

        case 9: /* debounce */
            printf("0 - Debounce Disabled\n");
            printf("1 - Debounce Enabled\n\n");
            bool_value = (BOOL)get_param();
            SetDebounce(c_blk, counter, bool_value);
            break;

        case 10: /* interrupt enable */
            printf("0 - Interrupts Disabled\n");
            printf("1 - Interrupts Enabled\n\n");
            bool_value = (BOOL)get_param();
            SetInterruptEnable(c_blk, counter, bool_value);
            break;

        case 11: /* Special Interrupt Condition */
            printf("0 - No Special Interrupt Selected\n");
            printf("1 - Interrupt when counter equals Counter Constant A\n");
            printf("2 - Interrupt on Index and reload on Index\n");
            printf("3 - Interrupt on Index (no reload)\n\n");
            byte_value = (BYTE)get_param();
	    byte_value &= 3;
	    SetSpecialInterrupt(c_blk, counter, byte_value);
            break;
         }
    }
}



/*
{+D}
    SYSTEM:         AP48x Software

    MODULE NAME:    trig48x - Software trigger for AP48x.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       Routine triggers counter/timers on the AP48x.

    CALLING
        SEQUENCE:   trig48x(c_block48x)
		    where:
			c_block48x (structure pointer)
			  The address of the configuration param. block

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

void trig48x(c_blk)
struct ap48x *c_blk;
{

/*
    DECLARE LOCAL DATA AREAS:
*/
    int item, trig;             /* menu item variable */
    unsigned finished;          /* flag to exit loop */

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
        printf("\n\nTrigger\\Stop\\Toggle Constant Options\n\n");
	printf(" 1. Return to Previous Menu\n");
        printf(" 2. Trigger (Start) Single Counter/Timer\n");
        printf(" 3. Trigger (Start) Multiple Counter/Timers\n");
        printf(" 4. Stop Single Counter/Timer\n");
        printf(" 5. Stop Multiple Counter/Timers\n");
        printf(" 6. Toggle Single Counter Constant\n");
        printf(" 7. Toggle Multiple Counter Constants\n");
        printf(" 8. Trigger (Start) Single Counter/Timer & Block\n");

        printf("\nselect: ");
        scanf("%d",&item);
	switch(item)
	{
	case 1: /* return to previous menu */
	    finished++;
	    break;

        case 2: /* Single trigger */
            printf("Enter Counter Number to Trigger (0 - %d): ",MAX_CNTR - 1);
            scanf("%x",&trig);
            StartCounter(c_blk, trig);
	    break;

        case 3: /* Multiple trigger */
            printf("Enter Hex Mask for Multiple Trigger: ");
            scanf("%x",&trig);
            StartSimultaneousCounters(c_blk, (word)trig);
	    break;

        case 4: /* Single stop */
            printf("Enter Counter Number to Stop (0 - %d): ",MAX_CNTR - 1);
            scanf("%x",&trig);
            StopCounter(c_blk, trig);
	    break;

        case 5: /* Multiple stop */
            printf("Enter Hex Mask for Multiple Stop: ");
            scanf("%x",&trig);
            StopSimultaneousCounters(c_blk, (word)trig);
	    break;

        case 6: /* Single toggle */
            printf("Enter Counter Number to Toggle Constant (0 - %d): ",MAX_CNTR - 1);
            scanf("%x",&trig);
            ToggleConstant(c_blk, trig);
	    break;

        case 7: /* Multiple toggle */
            printf("Enter Hex Mask for Multiple Toggle Constant: ");
            scanf("%x",&trig);
            ToggleSimultaneousConstants(c_blk, (word)trig);
	    break;

        case 8: /* Blocking Single trigger */
            printf("Enter Counter Number to Trigger (0 - %d): ",MAX_CNTR - 1);
            scanf("%x",&trig);
            printf("Blocking Start Counter please wait...");
            fflush(stdout);
            trig = BlockingStartCounter48x(c_blk, trig);
            printf(" done! IIRValue = %X\n", trig);
            break;
         }
    }
}



/*
{+D}
    SYSTEM:         AP48x Software

    MODULE NAME:    selectcnt48x - Select counter for AP48x.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       Routine selects counter/timers on the AP48x.

    CALLING
        SEQUENCE:   selectcnt48x(c_block48x)
		    where:
			c_block48x (structure pointer)
			  The address of the configuration param. block

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

void selectcnt48x(c_blk)
struct ap48x *c_blk;
{

/*
    DECLARE LOCAL DATA AREAS:
*/
    int item, cntr;             /* menu item variable */
    unsigned finished;          /* flag to exit loop */

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
        printf("\n\nCurrent Counter: %d\n\n",c_blk->counter_num);
	printf(" 1. Return to Previous Menu\n");
        printf(" 2. Change Counter\n");
        printf("\nselect: ");
        scanf("%d",&item);
	switch(item)
	{
	case 1: /* return to previous menu */
	    finished++;
	    break;

        case 2: /* Select counter */
selcounter:
            printf("Enter New Counter Number (0 - %d): ",MAX_CNTR - 1);
            scanf("%d",&cntr);
	    if( cntr > (MAX_CNTR - 1) )
	    {
	      printf("\n");
	      goto selcounter;
	    }
            c_blk->counter_num = (BYTE)(cntr);
	    break;
         }
    }
}


/*
{+D}
    SYSTEM:         AP48x Software

    MODULE NAME:    selectcon48x - Select counter constant for AP48x.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       Routine selects counter constants for the AP48x.

    CALLING
        SEQUENCE:   selectcon48x(c_block48x)
		    where:
			c_block48x (structure pointer)
			  The address of the configuration param. block

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

void selectcon48x(c_blk)
struct ap48x *c_blk;
{

/*
    DECLARE LOCAL DATA AREAS:
*/
    int item;                   /* menu item variable */
    int counter;                /* counter number stroage */
    unsigned finished;          /* flag to exit loop */
    uint32_t value;

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
      counter = c_blk->counter_num;
      printf("\n\nCurrent Counter:             %d\n",counter);
      GetCounterConstantAx(c_blk, counter, &value, 1);	/* int x; 1 or 2 */
      printf("Counter ConstantA1 Register: %X\n", value);
      GetCounterConstantBx(c_blk, counter, &value, 1);	/* int x; 1 or 2 */
      printf("Counter ConstantB1 Register: %X\n\n", value);

      GetCounterConstantAx(c_blk, counter, &value, 2);	/* int x; 1 or 2 */
      printf("Counter ConstantA2 Register: %X\n", value);
      GetCounterConstantBx(c_blk, counter, &value, 2);	/* int x; 1 or 2 */
      printf("Counter ConstantB2 Register: %X\n\n", value);

      printf(" 1. Return to Previous Menu\n");
      printf(" 2. Change Counter ConstantA1\n");
      printf(" 3. Change Counter ConstantB1\n");
      printf(" 4. Change Counter ConstantA2\n");
      printf(" 5. Change Counter ConstantB2\n");
      printf("\nselect: ");

      scanf("%d",&item);
      switch(item)
      {
      case 1: /* return to previous menu */
          finished++;
      break;
      case 2: /* Select constant A1 */
	  value = (uint32_t)get_param();
	  SetCounterConstantAx(c_blk, counter, value, 1);	/* int x; 1 or 2 */
      break;
      case 3: /* Select constant B1 */
	  value = (uint32_t)get_param();
	  SetCounterConstantBx(c_blk, counter, value, 1);	/* int x; 1 or 2 */
      break;
      case 4: /* Select constant A2 */
	  value = (uint32_t)get_param();
	  SetCounterConstantAx(c_blk, counter, value, 2);	/* int x; 1 or 2 */
      break;
      case 5: /* Select constant B2 */
	  value = (uint32_t)get_param();
	  SetCounterConstantBx(c_blk, counter, value, 2);	/* int x; 1 or 2 */
      break;
      }
    }
}

/*
{+D}
    SYSTEM:         AP48x Software

    MODULE NAME:    psts48x - Read Board Status for AP48x.

    VERSION:	    A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:	    FJM

    ABSTRACT:       Routine which is used to cause the "Read Board Status"
		    command to be executed and to print out the results to the console.
    CALLING
	SEQUENCE:   psts48x(&c_block48x)
		    where:
			c_block48x (structure pointer)
			  The address of the configuration param. block
			 
    MODULE TYPE:    void
    
    I/O RESOURCES:  
    
    SYSTEM
	RESOURCES:  
	
    MODULES
	CALLED:

    REVISIONS:      
    
  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:
    
*/

void psts48x(c_blk)
struct ap48x *c_blk;
{

/*
    DECLARE LOCAL DATA AREAS:
*/
    int item;           /* menu item variable */
    unsigned finished;  /* flags to exit loops */
    int index;

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
	  rmid48x(c_blk); /* get board's status info into structure */
	  printf("\n\nBoard Status Information\n");

	  printf("\nLocation Register:      %X",c_blk->location);
	  printf("\nFirmware Revision:       %c",(char)c_blk->revision);
	  printf("\nCounter IIRs 0 thru %d): ",MAX_CNTR - 1);
	  for( index = 0; index < MAX_CNTR; index++)
	    printf("%02X ", (uint32_t)read48x_CounterIIR(c_blk->nHandle, (uint32_t)index));

	  printf("\n\n1. Return to Previous Menu");
	  printf("\n2. Read Status Again\n3. FPGA Temp/Vcc Values\n");

	  scanf("%d",&item);

	  switch(item){

	  case 1: /* return to previous menu */
	    finished++;
	    break;
      case 3: /* display temp & VCC info from FPGA */
		for( index = 0; index < 9; index++)
		{
		  printf("Adr: %02X  FPGAData: %04X  ",
			  ((c_blk->FPGAAdrData[index] >> 16) & 0x7F),
			  ((c_blk->FPGAAdrData[index] >> 6) & 0x0FFF));

		  if((c_blk->FPGAAdrData[index] >> 16) & 3 ) /* Vcc */
			  printf("%7.3f V\n", ((c_blk->FPGAAdrData[index] >> 6) & 0x03FF) / 1024.0 * 3.0);
		  else            /* T deg C */
		      printf("%7.3f Deg C\n", ((c_blk->FPGAAdrData[index] >> 6) & 0x0FFF) * 503.975 / 1024.0 - 273.15);
		}
	  break;
	  }
    }
}

