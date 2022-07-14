
/*
{+D}
   SYSTEM:          Acromag AP323

   MODULE NAME:     ap323.c

   VERSION:         B

   CREATION DATE:   12/01/15

   CODED BY:        FJM

   ABSTRACT:        AP323 device.

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
01/13/17  FJM  Add wait for input interrupt and wake up blocked thread

{-D}
*/

/* AP323 device */

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
#include "../AP323.h"


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
struct pci_dev *p323Board[MAX_APS];
int ret_val = 0;
char devnamebuf[MAX_APS][32];


static wait_queue_head_t apWaitQueue[MAX_APS];	/* wait queues for inputs */
static int abWaitQueueFlag[MAX_APS];		/* wait queues flags */


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
	unsigned long adata, ldata ;
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
		default:
		    cdata = sdata = adata = ldata = 0;
		    return( -EINVAL );
		break;
	}
	put_user( ldata,(unsigned long *)( buf + (sizeof(unsigned long)) ) );	/* update user data */
	return( length );
}



static ssize_t
write( struct file *fp, const char *buf, size_t length, loff_t *offset )
{ 
	unsigned long adata, ldata, flag, instance;

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
		get_user( flag, (unsigned long *)(buf +(2*(sizeof(unsigned long)))) ); /* transfer size flag */
		get_user( instance, (unsigned long *)(buf +(3*(sizeof(unsigned long)))) ); /* board instance */
		abWaitQueueFlag[instance] = 0;	/* indicate waiting */
		switch(flag)
		{
		   case 0:	/* flag=0=byte write */
			iowrite8( (unsigned char)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance], abWaitQueueFlag[instance] );
		   break;
		   case 1:	/* flag=1=word write */
			iowrite16( (unsigned short)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance], abWaitQueueFlag[instance] );
		   break;
		   case 2:	/* flag=2=long write */
			iowrite32( (u32)ldata, (void *)adata );
			wait_event_interruptible( apWaitQueue[instance], abWaitQueueFlag[instance] );
		   break;
		   case 10:	/* flag=10=no write just wait for a input event to wake up */
			wait_event_interruptible( apWaitQueue[instance], abWaitQueueFlag[instance] );
		   break;
		   default:
			return -ENXIO;
		   break;
		}

		abWaitQueueFlag[instance] = 2;       /* indicate complete */

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
		case 5:/* return AP address */
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
		case 21: /* wake up blocked thread so it can terminate */
	           get_user( ldata, (unsigned long *)arg  );	/* pickup instance number */
	           abWaitQueueFlag[ldata] = 2;			/* indicate complete */
	           wake_up_interruptible( &apWaitQueue[ldata] );
		break;
	}
	return((long)cmd );
}


static struct file_operations ap323_ops = {
  .owner = THIS_MODULE, /* owner of the world */
  .read = read,         /* read */
  .write = write,       /* write */
  .unlocked_ioctl = device_ioctl, /* unlocked_ioctl */
  .open = open,         /* open */
  .release = release,   /* release */
};


static irqreturn_t
ap323_handler( int irq, void *did, struct pt_regs *cpu_regs )
{ 
  volatile struct map323 *map_ptr;    /* pointer to board memory map */
  volatile word interrupt_register;   /* interrupt register */
  int int_status;
  int i;

   int_status = 0;                    /* indicate interrupt not handled */

   for(i = 0; i < MAX_APS; i++)       /* check all boards */
   {
     if( open_dev[i] )                /* if board is open */
     {
	map_ptr = (struct map323 *)ap_address[i];
	interrupt_register = readw(&map_ptr->InterruptRegister);

	/* board interrupt enabled and pending? */
	if( interrupt_register == ( AP_INT_ENABLE | AP_INT_PENDING ))
	{
          interrupt_register |= INT_RELEASE;     /* set interrupt release bit */
          writew( interrupt_register, &map_ptr->InterruptRegister );/* release interrupt */
  	  int_status = 1;             /* indicate interrupt handled */

  	  /* if we were waiting for a blocked start AND received an interrupt */
  	  if(abWaitQueueFlag[i] == 0)
  	  {
	   abWaitQueueFlag[i] = 1;  /* when received, change the condition to true... */
	   wake_up_interruptible( &apWaitQueue[i]); /* ... and wake the blocked write */
  	  }
printk("AP323 ISR Adr=%lX\n", (unsigned long)map_ptr);
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
  extern struct pci_dev *p323Board[MAX_APS];
  extern char devnamebuf[MAX_APS][32];
  extern int abWaitQueueFlag[MAX_APS];	/* wait queues flags */
  struct pci_dev *p323;
  int i,j,k;
  char devnumbuf[8];

  memset( &p323Board[0], 0, sizeof(p323Board));
  memset( &open_dev[0], 0, sizeof(open_dev));
  memset( &msi_status[0], 0, sizeof(msi_status));
  memset( &board_irq[0], 0, sizeof(board_irq));
  memset( &ap_address[0], 0, sizeof(ap_address));
  memset( &abWaitQueueFlag[0], 2, sizeof(abWaitQueueFlag));

  p323 = NULL;
  for( i = 0, j = 0; i < MAX_APS; i++ )
  {
    p323 = ( struct pci_dev *)pci_get_device( VENDOR_ID, AP323, p323 ); /* AP323 */
    if( p323 )
    {
      p323Board[i] = p323;
      ap_address[i] = (unsigned long)p323Board[i]->resource[0].start;
      ap_address[i] = (unsigned long)ioremap_nocache( ap_address[i], 4096); /* no cache! */

      if( ap_address[i] )
      {
	memset( &devnamebuf[i][0], 0, sizeof(devnamebuf[i]));
	memset( &devnumbuf[0], 0, sizeof(devnumbuf));
	strcpy(devnamebuf[i], DEVICE_NAME);
	sprintf(&devnumbuf[0],"%d",i);
	strcat(devnamebuf[i], devnumbuf);
        ret_val = pci_enable_device(p323Board[i]);

#ifdef AP_USE_MSI_INT
        msi_status[i] = pci_enable_msi(p323Board[i]);	/* Use Message Signaled Interrupts (MSI) */
#endif
        board_irq[i] = p323Board[i]->irq;

        ret_val = request_irq ( board_irq[i], (irq_handler_t)ap323_handler, IRQF_SHARED, devnamebuf[i], ( void *)ap_address[i] );

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
    ret_val = register_chrdev ( MAJOR_NUM, DEVICE_NAME, &ap323_ops );

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
	   init_waitqueue_head(&apWaitQueue[k]);
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
			pci_disable_msi(p323Board[i]);
#endif
        iounmap( (void *)ap_address[i] );
        printk("%s unmapped I/O=%08lX IRQ=%02X\n",devnamebuf[i],(unsigned long)ap_address[i], board_irq[i]);
        open_dev[i] = 0;
      }
    }
  }
}

MODULE_LICENSE("GPL and additional rights");
