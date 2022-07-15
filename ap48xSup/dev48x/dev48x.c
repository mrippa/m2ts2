
/*
{+D}
   SYSTEM:          Acromag AP48x

   MODULE NAME:     dev482.c

   VERSION:         A

   CREATION DATE:   12/01/15

   CODED BY:        FJM

   ABSTRACT:        ap48x device.

   CALLING
	SEQUENCE:

   MODULE TYPE:

   I/O RESOURCES:

   SYSTEM
	RESOURCES:

   MODULES
	CALLED:

   REVISIONS:

 DATE      BY       PURPOSE
-------- ----  ------------------------------------------------

{-D}
*/

/* AP48X device */

#ifndef BUILDING_FOR_KERNEL
#define BUILDING_FOR_KERNEL	/* controls conditional inclusion in file apcommon.h */
#endif


#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#include "../../apcommon/apcommon.h"
#include "../AP48X.h"



#define MAJOR_NUM	46

/* ///////////////////////////////////////////////////////////////// */
/* Message Signaled Interrupt (MSI) is a VECTOR based scheme and     */
/* needs to be enabled by setting CONFIG_PCI_MSI during kernel       */
/* config.                                                           */
/* Since the target of the inbound message is the local APIC,        */
/* providing CONFIG_X86_LOCAL_APIC must be enabled as well as        */
/* CONFIG_PCI_MSI.                                                   */
/* Use the following define to support Message Signaled Interrupt.   */
/* ///////////////////////////////////////////////////////////////// */

#define AP_USE_MSI_INT



int open_dev[MAX_APS];
int msi_status[MAX_APS];
unsigned int board_irq[MAX_APS];
unsigned long ap_address[MAX_APS];
struct pci_dev *p48xBoard[MAX_APS];
int ret_val = 0;
char devnamebuf[MAX_APS][32];

static wait_queue_head_t apWaitQueue[MAX_APS][MAX_CNTR]; /* wait queues for counter */
static int abWaitQueueFlag[MAX_APS][MAX_CNTR];	/* wait queues flags for counters */
static uint32_t CounterIIRs[MAX_APS][MAX_CNTR];/* Counter Interrupt Information Registers, individual device status */


struct file;
struct inode;
struct pt_regs;


static int
open( struct inode *inode, struct file *fp )
{
   int minor;

   minor = inode->i_rdev & 0xf;

   if( minor > (MAX_APS - 1))
	   return( -ENODEV );
  
   if( open_dev[minor] )
	   return( -EBUSY );

   open_dev[minor] = 1;

   return( 0 );
} 


static int
release( struct inode *inode, struct file *fp )
{
   int minor;

   minor = inode->i_rdev & 0xf;

   if( minor > (MAX_APS - 1))
	   return( -ENODEV );

   if( open_dev[minor] )
   {
	   open_dev[minor] = 0;
	   return( 0 );
   }
   return( -ENODEV );
}

static ssize_t
read( struct file *fp, char *buf, size_t length, loff_t *offset )
{ 
	unsigned long adata, ldata, idata;
	unsigned short sdata;
	unsigned char cdata;

	get_user( adata, (unsigned long *)buf );		/* pickup address */
	switch( length )
	{
		case 1:	/* 8 bit */
		   cdata = readb( (void *) adata );
		   ldata = ( unsigned long )cdata;		/* convert to long */
		break;
		case 2:	/* 16 bit */
		   sdata = readw( (void *) adata );
		   ldata = ( unsigned long )sdata;		/* convert to long */
		break;
		case 4:	/* 32 bit */
		   ldata = readl( (void *) adata );
		break;
		case 8:	/* Read 48x CounterIIRs */
		   get_user( idata, (unsigned long *)( buf + (sizeof(unsigned long)) ) );/* pickup instance index */
		   ldata = CounterIIRs[idata][adata & 0xF];	/* adata holds the counter index 0 thru 9 */
		   CounterIIRs[idata][adata & 0xF] = 0;		/* clear after read */
		break;
		default:
		    cdata = sdata = adata = ldata = idata = 0;
		    return( -EINVAL );
		break;
	}
	put_user( ldata,(unsigned long *)( buf + (sizeof(unsigned long)) ) );	/* update user data */
	return( length );
}



static ssize_t
write( struct file *fp, const char *buf, size_t length, loff_t *offset )
{ 
	unsigned long adata, ldata, flag, instance, counter;

	get_user( adata, (unsigned long *)buf );				/* pickup address */
	get_user( ldata, (unsigned long *)( buf + (sizeof(unsigned long)) ) );	/* pickup data */
	switch( length )
	{
		case 1:	/* 8 bit */
		   writeb( (int)ldata, (void *)adata );
		break;
		case 2:	/* 16 bit */
		   writew( (int)ldata, (void *)adata );
		break;
		case 4:	/* 32 bit */
		   writel( (int)ldata, (void *)adata );
		break;
		case 8:	/* blocking start convert */
		get_user( flag, (unsigned long *)(buf +(2*(sizeof(unsigned long)))) );		/* transfer size flag */
		get_user( instance, (unsigned long *)(buf +(3*(sizeof(unsigned long)))) );	/* board instance */
		get_user( counter, (unsigned long *)(buf +(4*(sizeof(unsigned long)))) );	/* counter number */

		abWaitQueueFlag[instance][counter] = 0;	/* indicate waiting */
		switch(flag)
		{
		   case 0:	/* flag=0=byte write */
			iowrite8( (unsigned char)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance][counter], abWaitQueueFlag[instance][counter] );
		   break;
		   case 1:	/* flag=1=word write */
			iowrite16( (unsigned short)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance][counter], abWaitQueueFlag[instance][counter] );
		   break;
		   case 2:	/* flag=2=long write */
			iowrite32( (u32)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance][counter], abWaitQueueFlag[instance][counter] );
		   break;
		   default:
			return -ENXIO;
		   break;
		}

		abWaitQueueFlag[instance][counter] = 2;       /* indicate complete */
                ldata = (unsigned long)CounterIIRs[instance][counter];	/* get the counters IIR status */
                put_user( ldata,(unsigned long *)( buf + (sizeof(unsigned long)) ) );	/* update user data */
		break;
		default:
		    return( -EINVAL );
		break;
	}
    return( length );
}


static long
device_ioctl( struct file *fp, unsigned int cmd, unsigned long arg)
{
    unsigned long ldata;
    int i;
	switch( cmd )
	{
		case 0:	/* reserved for future use */
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:/* return address */
                   for(i = 0; i < MAX_APS; i++)                        /* get all boards */
                   {
                       ldata = ( unsigned long )ap_address[i];         /* convert to long */
                       put_user( ldata, (unsigned long *)(arg+(i*(sizeof(unsigned long)))) );	/* update user data */
                   }    
		break;
		case 6:/* return IRQ number */
                   for(i = 0; i < MAX_APS; i++)                        /* get all boards */
                   {
                       ldata = ( unsigned long )board_irq[i];           /* convert IRQ to long */
                       put_user( ldata, (unsigned long *)(arg+(i*(sizeof(unsigned long)))) );	/* update user data */
                   }
		break;
	}
	return((long)cmd );
}


static struct file_operations ap48x_ops = {
  .owner = THIS_MODULE, /* owner of the world */
  .read = read,         /* read */
  .write = write,       /* write */
  .unlocked_ioctl = device_ioctl, /* unlocked_ioctl */
  .open = open,         /* open */
  .release = release,   /* release */
};


static irqreturn_t
ap48x_handler( int irq, void *did, struct pt_regs *cpu_regs )
{ 
  volatile struct map48x *map_ptr;	/* pointer to board memory map */
  volatile uint32_t istat;
  int int_status;
  int i, j;	/* loop control */

  int_status = 0;			/* indicate interrupt not handled */
  for(i = 0; i < MAX_APS; i++)		/* check all boards */
  {
     if( open_dev[i] )          	/* if board is open */
     {
	map_ptr = (struct map48x *)ap_address[i];
	istat = readl( &map_ptr->GIEPendingStatus );	/* board interrupt status */

	if( istat == ( AP_INT_ENABLE | AP_INT_PENDING ))/* board interrupt enabled and pending? */
        {
	  istat = readl( &map_ptr->InterruptStatusReg );/* individual counter interrupt status */

	  /* check each counter for an interrupt pending state */
	  for( j = 0; j < MAX_CNTR; j++ )
	  {
	    if( istat & (1 << j ))    /* check for individual counter interrupt pending */
	    {
		/* Read and save pending Counter Interrupt Information Register */
		CounterIIRs[i][j] = readl( &map_ptr->Counter[j].CounterIIR);

printk("AP48xISR HwdAdr=%lX Cntr=%X IIR=%X\n",(unsigned long)map_ptr, j, CounterIIRs[i][j]);

		/* if we were waiting for a blocked start AND received an interrupt from the counter */
		if(abWaitQueueFlag[i][j] == 0)
		{
		   abWaitQueueFlag[i][j] = 1;  /* when received, change the condition to true... */
		   wake_up_interruptible( &apWaitQueue[i][j] ); /* ... and wake the blocked write */
		}

		/* The user may place a call to an individual counter interrupt routine here */
	    }
	  }
	  writew( istat, &map_ptr->InterruptStatusReg ); /* write interrupt pending to clear */
  	  int_status = 1;

	}
     }
  }

  if( int_status )
       return( IRQ_HANDLED);
  else
       return( IRQ_NONE);
}


int
init_module( void ) 
{ 
  extern struct pci_dev *p48xBoard[MAX_APS];
  extern char devnamebuf[MAX_APS][32];
  extern uint32_t CounterIIRs[MAX_APS][MAX_CNTR];
  extern int abWaitQueueFlag[MAX_APS][MAX_CNTR];	/* wait queues flags for counters */
  struct pci_dev *p48x;
  int i, j, k, l;
  char devnumbuf[8];

  memset( &p48xBoard[0], 0, sizeof(p48xBoard));
  memset( &open_dev[0], 0, sizeof(open_dev));
  memset( &msi_status[0], 0, sizeof(msi_status));
  memset( &board_irq[0], 0, sizeof(board_irq));
  memset( &ap_address[0], 0, sizeof(ap_address));
  memset( &abWaitQueueFlag[0][0], 2, sizeof(abWaitQueueFlag));
  memset( &CounterIIRs[0][0], 0, sizeof(CounterIIRs));

  p48x = NULL;
  for( i = 0, j = 0; i < MAX_APS; i++ )
  {
    p48x = ( struct pci_dev *)pci_get_device( VENDOR_ID, AP48X, p48x ); /* AP48x */
    if( p48x )
    {
      p48xBoard[i] = p48x;
      ap_address[i] = (unsigned long)p48xBoard[i]->resource[0].start;
      ap_address[i] = (unsigned long)ioremap_nocache( ap_address[i], 4096); /* no cache! */

      if( ap_address[i] )
      {
	memset( &devnamebuf[i][0], 0, sizeof(devnamebuf[i]));
	memset( &devnumbuf[0], 0, sizeof(devnumbuf));
	strcpy(devnamebuf[i], DEVICE_NAME);
	sprintf(&devnumbuf[0],"%d",i);
	strcat(devnamebuf[i], devnumbuf);
        ret_val = pci_enable_device(p48xBoard[i]);

#ifdef AP_USE_MSI_INT
	msi_status[i] = pci_enable_msi(p48xBoard[i]);	/* Use Message Signaled Interrupts (MSI) */
#endif
        board_irq[i] = p48xBoard[i]->irq;

	ret_val = request_irq ( board_irq[i], (irq_handler_t)ap48x_handler, IRQF_SHARED, devnamebuf[i], ( void *)ap_address[i] );

#ifdef AP_USE_MSI_INT
        printk("%s mapped   I/O=%08lX IRQ=%02X Rv=%X MSI=%X\n",devnamebuf[i],(unsigned long)ap_address[i], board_irq[i],ret_val, msi_status[i]);
#else
        printk("%s mapped   I/O=%08lX IRQ=%02X Rv=%X\n",devnamebuf[i],(unsigned long)ap_address[i], board_irq[i],ret_val);
#endif
        j++;
      }
    }
    else
	break;
  }
  if( j )	/* found at least one device */
  {
    ret_val = register_chrdev ( MAJOR_NUM, DEVICE_NAME, &ap48x_ops );

    if( ret_val < 0)
    {
	printk(DEVICE_NAME);
	printk(" Failed to register error = %d\n", ret_val);
    }
    else
    {
	/* initialize wait queues */
	for( k = 0; k < MAX_APS; k++ )
	{
	  for( l = 0; l < MAX_CNTR; l++ )
		init_waitqueue_head(&apWaitQueue[k][l]);
	}
	return( 0 );
    }
  }
  return( -ENODEV );
}


void
cleanup_module( void )
{
  extern char devnamebuf[MAX_APS][32];
  int i;

  if( ret_val >= 0 )
  {
    unregister_chrdev( MAJOR_NUM, DEVICE_NAME );
    for( i = 0; i < MAX_APS; i++ )
    {
      if( ap_address[i] )
      {
        free_irq( board_irq[i], (void *)ap_address[i] );
#ifdef AP_USE_MSI_INT
	if(msi_status[i] == 0)			/* if enabled, remove Message Signaled Interrupts (MSI) */
	   pci_disable_msi(p48xBoard[i]);
#endif
        iounmap( (void *)ap_address[i] );
        printk("%s unmapped I/O=%08lX IRQ=%02X\n",devnamebuf[i],(unsigned long)ap_address[i], board_irq[i]);
        open_dev[i] = 0;
      }
    }
  }
}

MODULE_LICENSE("GPL and additional rights");

