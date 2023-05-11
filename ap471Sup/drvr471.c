
#include <pthread.h>
#include "../apCommon/apCommon.h"
#include "AP471.h"

/*
{+D}
    SYSTEM:         Software

    FILENAME:       drvr471.c

    MODULE NAME:    main - main routine of example software.

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       This module is the main routine for the example program
                    which demonstrates how the AP471 Library is used.

    CALLING
	SEQUENCE:   

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

	This module is the main routine for the example program
	which demonstrates how the AP471 Library is used.
*/



static void InterruptHandlerThread(struct cblk471 *c_blk)
{
  struct AP471_isr_info isr_info;	/* structure used to hold the board's interrupt information */
  int Status;

/*
   Interrupt handler thread, calls AP471Blocking_io() in a loop processing interrupts.
   AP471_isr_info.Status returns with a value of zero, when software triggered by calling function APTerminateBlockedStart()
   to wake up the interrupt handler thread and cause it to exit, otherwise, interrupts are processed and displayed.

   The interrupt thread examines the return value in AP471_isr_info.Status AND if it is zero, executes pthread_exit to terminate.
   The main thread waits for the interrupt thread to terminate by executing pthread_join prior to main thread termination.
*/

  while(1)
  {
    memset(&isr_info, 0, sizeof(isr_info));

    /* 2 = a long write using the specified value at the specified address then block & wait for an event to wake up... */
    /* writing a one to this address enables board interrupts, the function status is also returned */
    Status = AP471Blocking_io(c_blk->nHandle, &isr_info, (long*)&c_blk->brd_ptr->InterruptRegister, (long)1, (long) 2);

    /* AP471Blocking_io function status AND AP471_isr_info.Status == 0, zero, executes pthread_exit to terminate */
    if(Status == 0 && isr_info.Status == 0)
       pthread_exit(0);

    /* Print channel event pending flags */
    printf("\nISR EVPend[0]=0x%02X EVPend[1]=0x%02X EVPend[2]=0x%02X\n",
              isr_info.EventPending[0], isr_info.EventPending[1], isr_info.EventPending[2]);


/* TODO: Add your own test(s) for channels with interrupts pending and the code needed to process them */



/*  Write the Event Pending Clear Registers to acknowledge/clear the pending interrupts */
    output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[0], (long) isr_info.EventPending[0]);
    output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[1], (long) isr_info.EventPending[1]);
    output_long( c_blk->nHandle, (long*)&c_blk->brd_ptr->EventPendingClrReg[2], (long) isr_info.EventPending[2]);
  }
}




int main(argc, argv)
int argc; char *argv[];
{


/*
    DECLARE LOCAL DATA AREAS:
*/

    char cmd_buff[40];  /* command line input buffer */
    int item;           /* menu item selection variable */
    long status;        /* returned status of driver routines */
    APSTATUS hstatus;   /* interrupt handler returned status */
    unsigned finished;  /* flag to exit program */
    long addr;          /* long to hold board address */
    int point;          /* I/O point number */
    int port;           /* I/O port number */
    int val;            /* value of port or point */
    int hflag;          /* interrupt handler installed flag */
    struct cblk471 c_block471; /* configuration block */
    struct sblk471 s_block471; /* allocate status param. blk */
    pthread_t IHandler; /* thread variable */
    int ap_instance = 0;    
/*
    ENTRY POINT OF ROUTINE:
    INITIALIZATION
*/

    if(argc == 2)
      ap_instance = atoi(argv[1]);

    finished = 0;     /* indicate not finished with program */
    hflag = 0;        /* indicate interrupt handler not installed yet */

/*
    Initialize the Configuration Parameter Block to default values.
*/

    memset(&c_block471, 0, sizeof(c_block471));
    memset(&s_block471, 0, sizeof(s_block471));
    c_block471.bAP = FALSE;          /* indicate no AP initialized and set up yet */
    c_block471.bInitialized = FALSE; /* indicate not ready to talk to AP */
    c_block471.sblk_ptr = (struct sblk471*)&s_block471; /* set address of status structure */
    
/*
	Initialize the AP library
*/
    if(InitAPLib() != S_OK)
    {
      printf("\nUnable to initialize the AP library. Exiting program.\n");
      exit(0);
    }

/*
	Open an instance of a device
	Other device instances can be obtained
	by changing parameter 1 of APOpen()
*/


    if(APOpen(ap_instance, &c_block471.nHandle, DEVICE_NAME ) != S_OK)
    {
      printf("\nUnable to Open instance of %s.\n",DEVICE_NAME );
      finished = 1;	 /* indicate finished with program */
    }
    else
    {
      if(APInitialize(c_block471.nHandle) == S_OK)/* Initialize */
      {
        GetAPAddress(c_block471.nHandle, &addr);	/* Read back address */
        c_block471.brd_ptr = (struct map471 *)addr;
        c_block471.bInitialized = TRUE;
        c_block471.bAP = TRUE;

        /* Create the interrupt processing thread */
        pthread_create(&IHandler, NULL, (void *)&InterruptHandlerThread, (struct cblk471 *) &c_block471);
      }
    }

/*
    Enter main loop
*/      

    while(!finished) {
	printf("\n\nAP471 Library Demonstration Version A\n\n");
	printf(" 1. Exit this Program\n");
	printf(" 2. Set Up Configuration Parameters\n");
	printf(" 3. Configure Board Command\n");
	printf(" 4. Read Status Command\n");
	printf(" 5. Attach Interrupt Handler\n");
	printf(" 6. Detach Interrupt Handler\n");
	printf(" 7. Read Input Point\n");
	printf(" 8. Read Input Port\n");
	printf(" 9. Write Output Point\n");
	printf("10. Write Output Port\n");
	printf("\nSelect: ");
	scanf("%d",&item);

/*
    perform the menu item selected.
*/  
	switch(item) {

	case 1: /* exit program command */

          printf("Exit program(y/n)?: ");
          scanf("%s",cmd_buff);
          if( cmd_buff[0] == 'y' || cmd_buff[0] == 'Y' )
             finished++;
	break;
	
	case 2: /* set up configuration parameters */

          scfg471(&c_block471);
	break;

	case 3: /* configure board command */
	
          if(!c_block471.bInitialized)
            printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
            cnfg471(&c_block471); /* configure the board */

	break;

	case 4:     /* read board status command */
	
          if(!c_block471.bInitialized)
            printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
            psts471(&c_block471); /* read board status */
	break;

	case 5:     /* attach exception handler */
          if(hflag)
            printf("\n>>> ERROR: HANDLERS ARE ATTACHED <<<\n");
          else
          {
            hstatus = EnableAPInterrupts(c_block471.nHandle);
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
	  } /* end of if/else */
	break;

	case 6: /* detach exception handlers */
          hflag = 0;
          DisableAPInterrupts(c_block471.nHandle);
	break;

	case 7: /* Read Digital Input Point */

          if(!c_block471.bInitialized)
             printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
          {
            printf("\nEnter Input port number   (0 - 2): ");
            scanf("%x",&port);
            printf("\nEnter Input point number  (15 - 0): ");
            scanf("%d",&point);
            status = (long)rpnt471(&c_block471,(uint16_t)port,(uint16_t)point);
            printf("\nValue of port %d point %d: %lx\n",port,point,status);
          }
	break;

	case 8: /* Read Digital Input Port */

          if(!c_block471.bInitialized)
             printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
          {
            printf("\nEnter Input port number  (0 - 2): ");
            scanf("%x",&port);
            status = (long)rprt471(&c_block471,(uint16_t)port);
            printf("\nValue of port %d: %lX\n",port,status);
          }
	break;

	case 9: /* Write Digital Point */

          if(!c_block471.bInitialized)
             printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
          {
            printf("\nEnter Output port number (0 - 2): ");
            scanf("%x",&port);
            printf("\nEnter I/O point number   (15 - 0): ");
            scanf("%d",&point);
            printf("\nEnter point value (0 - 1): ");
            scanf("%x",&val);
            wpnt471(&c_block471,(uint16_t)port,(uint16_t)point,(uint16_t)val);
          }
	break;

	case 10: /* Write Digital Port */

          if(!c_block471.bInitialized)
             printf("\n>>> ERROR: BOARD NOT SET UP <<<\n");
          else
          {
            printf("\nEnter Output port number  (0 - 2):  ");
            scanf("%x",&port);
            printf("\nEnter output value in hex (0 - 0xFFFF): ");
            scanf("%x",&val);
            wprt471(&c_block471,(uint16_t)port,(uint16_t)val);
          }
	break;
	}   /* end of switch */
    }   /* end of while */

    if(c_block471.bAP)
    {
       DisableAPInterrupts(c_block471.nHandle); /* disable module interrupts */
       APTerminateBlockedStart(c_block471.nHandle); /* signal the interrupt handler thread that it is time to terminate */
       pthread_join(IHandler, NULL);	/* wait for interrupt handler thread termination */

       APClose(c_block471.nHandle);
    }
    printf("\nEXIT PROGRAM\n");
    return(0);
}   /* end of main */


/*
{+D}
    SYSTEM:         Software

    FILENAME:       drvr471.c

    MODULE NAME:    scfg471 - set configuration block contents.

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       Routine which is used to enter parameters into
		    the Configuration Block.
 
    CALLING
	SEQUENCE:   scfg471(c_block471)
		    where:
			c_block471 (structure pointer)
			  The address of the configuration param. block

    MODULE TYPE:    void

    I/O RESOURCES:  

    SYSTEM
	RESOURCES:  

    MODULES
	CALLED:     get_param()     input a parameter from console

    REVISIONS:      

  DATE    BY        PURPOSE
-------  ----   ------------------------------------------------

{-D}
*/


/*
    MODULES FUNCTIONAL DETAILS:
*/

static unsigned get_reg_num()
{
   unsigned i;

   printf("\nEnter Register Number (0, 1, 2) ");
   scanf("%x",&i);
   if( i > 2)
      i = 2;

   return( i );
}




void scfg471(c_blk)

struct cblk471 *c_blk;
{

/*
    DECLARE LOCAL DATA AREAS:
*/
    int item;                       /* menu item variable */
    unsigned finished;              /* flags to exit loops */
    unsigned index;

/*
    ENTRY POINT OF ROUTINE:
*/
    finished = 0;
    while(!finished)
    {
	printf("\n\nConfiguration Parameters\n\n");
	printf(" 1. Return to Previous Menu\n");
	printf(" 2. Parameter Mask:    0x%02X\n",c_blk->param);
        printf(" 3. Debounce Clock:    0x%02X\n",c_blk->deb_clock);
        printf(" 4. Debounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X\n",
                 c_blk->deb_duration[0], c_blk->deb_duration[1], c_blk->deb_duration[2]);
        printf(" 5. Debounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
                 c_blk->deb_control[0], c_blk->deb_control[1], c_blk->deb_control[2]);
	printf(" 6. Write Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
                 c_blk->WriteMask[0], c_blk->WriteMask[1], c_blk->WriteMask[2]);
	printf(" 7. Event Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
                 c_blk->EventType[0], c_blk->EventType[1], c_blk->EventType[2]);
	printf(" 8. Event Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
                 c_blk->EventPolarity[0], c_blk->EventPolarity[1], c_blk->EventPolarity[2]);
	printf(" 9. Event Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X\n",
                 c_blk->EventEnable[0], c_blk->EventEnable[1], c_blk->EventEnable[2]);
        printf("\nSelect: ");
        scanf("%x",&item);

	switch(item)
	{
          case 1: /* return to previous menu */
	    finished++;
          break;

          case 2: /* parameter mask */
            printf("\nBoard Register Update Bits (0x7F for all)\n0x01 = Mask Registers\n");
            printf("0x02 = Event Enable Registers\n0x04 = Event Type Registers\n0x08 = Event Polarity Registers\n");
            printf("0x10 = Debounce Clock Register\n0x20 = Debounce Control Registers\n");
            printf("0x40 = Debounce Duration Registers\n");
	    c_blk->param = (word)get_param();
          break;

          case 3: /* Debounce Clock */
            printf("0 = Internal Clock, 1 = External Clock\n");
            c_blk->deb_clock = (uint32_t)(get_param() & 1);
          break;

          case 4: /* Debounce Duration */  
            index = get_reg_num();
            printf("Duration Bit Pairs 31 30 ... 1 0 Correspond to I/O Channels CH 15 ... CH 00\n");
            printf("Bits 31 - 0, Each Channel Pair 00=3-4uS, 01=48-64uS, 10=0.75-1mS, 11=6-8mS\n");
            c_blk->deb_duration[index] = (uint32_t)get_param();
          break;

          case 5: /* Debounce Control */
            index = get_reg_num();
            printf("Bits 15 - 0, Set Bits Enable Channel Debounce, Clear Bits Disable\n");
            c_blk->deb_control[index] = (uint32_t)(get_param() & 0xFFFF);
          break;

          case 6: /* Write Mask */
            index = get_reg_num();
            printf("Bits 15 - 0, Set Bits Prevent Channel Writes, Clear Bits Allow Writes\n");
            c_blk->WriteMask[index] = (uint32_t)(get_param() & 0xFFFF);
          break;

          case 7: /* Event Type */
            index = get_reg_num();
            printf("Bits 15 - 0, Set Bits Enable COS Channel Events, Clear Bits Select Edge\n");
            c_blk->EventType[index] = (uint32_t)(get_param() & 0xFFFF);
          break;

          case 8: /* Event Polarity */
            index = get_reg_num();
            printf("Bits 15 - 0, Set Bits Select High Edge Events, Clear Bits Select Low Edge\n");
            c_blk->EventPolarity[index] = (uint32_t)(get_param() & 0xFFFF);
          break;

          case 9: /* Event Enable */
            index = get_reg_num();
            printf("Bits 15 - 0, Set Bits Enable Event Detection, Clear Bits Disable Detection\n");
            c_blk->EventEnable[index] = (uint32_t)(get_param() & 0xFFFF);
          break;
	}
    }
}



/*
{+D}
    SYSTEM:         Software

    FILENAME:       drvr471.c

    MODULE NAME:    psts471 - print board status information

    VERSION:        A

    CREATION DATE:  12/01/15

    DESIGNED BY:    FJM

    CODED BY:       FJM

    ABSTRACT:       Routine which is used to cause the "Read Board Status" command
                    to be executed and to print out the results to the console.

    CALLING
	SEQUENCE:   psts471(&c_block471)
		    where:
			c_block471 (structure pointer)
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

void psts471(c_blk)
struct cblk471 *c_blk;
{

/*
    DEFINITIONS:
*/


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
	  rsts471(c_blk); /* get board's status info into structure */
	  printf("\n\nBoard Status Information\n");
	  printf("\nLocation Register:   %X",c_blk->location);
	  printf("\nFirmware Revision:    %c",(char)c_blk->revision);
          printf("\nDebounce Clock:    0x%02X",c_blk->sblk_ptr->DebounceClockSelectStat);
          printf("\nDebounce Duration: R0 = 0x%08X R1 = 0x%08X R2 = 0x%08X",c_blk->sblk_ptr->DebounceDurationStat[0],
                                    c_blk->sblk_ptr->DebounceDurationStat[1], c_blk->sblk_ptr->DebounceDurationStat[2]);
          printf("\nDebounce Control:  R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->DebounceControlStat[0],
                                    c_blk->sblk_ptr->DebounceControlStat[1], c_blk->sblk_ptr->DebounceControlStat[2]);
	  printf("\nWrite Mask:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->MaskRegStat[0],
                                    c_blk->sblk_ptr->MaskRegStat[1], c_blk->sblk_ptr->MaskRegStat[2]);
	  printf("\nEvent Type:        R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->EventTypeStat[0],
                                    c_blk->sblk_ptr->EventTypeStat[1], c_blk->sblk_ptr->EventTypeStat[2]);
	  printf("\nEvent Polarity:    R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->EventPolarityStat[0],
                                    c_blk->sblk_ptr->EventPolarityStat[1], c_blk->sblk_ptr->EventPolarityStat[2]);
	  printf("\nEvent Enable:      R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->EventEnableStat[0],
                                    c_blk->sblk_ptr->EventEnableStat[1], c_blk->sblk_ptr->EventEnableStat[2]);
	  printf("\nEvent Pending:     R0 =     0x%04X R1 =     0x%04X R2 =     0x%04X",c_blk->sblk_ptr->EventPendingClrStat[0],
                                    c_blk->sblk_ptr->EventPendingClrStat[1], c_blk->sblk_ptr->EventPendingClrStat[2]);
          printf("\nBoard Int Enable:  0x%02X\n",c_blk->sblk_ptr->BoardIntEnableStat);

	  printf("\n\n1. Return to Previous Menu");
	  printf("\n2. Read Status Again\n3. FPGA Temp/Vcc Values\n");
	  printf("\nselect: ");
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

