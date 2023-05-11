
/*
{+D}
   SYSTEM:          Acromag AP471

   MODULE NAME:     ap471.c

   VERSION:         C

   CREATION DATE:   12/01/15

   CODED BY:        FJM

   ABSTRACT:        ap471 device.

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
06/22/20  FJM  Changed ioremap_nocache() to ioremap()
07/19/22  FJM  Changed irqreturn_t

{-D}
*/

/* AP471 device */


#ifndef BUILDING_FOR_KERNEL
#define BUILDING_FOR_KERNEL	/* controls conditional inclusion in file apCommon.h */
#endif


//#include <linux/init.h>
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

#include "../../apCommon/apCommon.h"
#include "../AP471.h"


#define MAJOR_NUM	48


/* ///////////////////////////////////////////////////////////////// */
/* Message Signaled Interrupt (MSI) is a VECTOR based scheme and     */
/* needs to be enabled by setting CONFIG_PCI_MSI during kernel       */
/* config.                                                           */
/* Since the target of the inbound message is the local APIC,        */
/* providing CONFIG_X86_LOCAL_APIC must be enabled as well as        */
/* CONFIG_PCI_MSI.                                                   */
/* Use the following define to support Message Signaled Interrupt.   */
/* ///////////////////////////////////////////////////////////////// */

/*#define AP_USE_MSI_INT*/


int open_dev[MAX_APS];
int msi_status[MAX_APS];
unsigned int board_irq[MAX_APS];
unsigned long ap_address[MAX_APS];
struct pci_dev *p471Board[MAX_APS];
int ret_val = 0;
char devnamebuf[MAX_APS][32];


static wait_queue_head_t apWaitQueue[MAX_APS];	/* wait queues for inputs */
static int abWaitQueueFlag[MAX_APS];		/* wait queues flags */
static struct AP471_isr_info ISR[MAX_APS];	/* Interrupt Status Registers */



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
		   case 10:	/* flag=10=no write just wait for an event to wake up */
			wait_event_interruptible( apWaitQueue[instance], abWaitQueueFlag[instance] );
		   break;
		   default:
			return -ENXIO;
		   break;
		}

		abWaitQueueFlag[instance] = 2;           /* indicate complete */

                ldata = (unsigned long)ISR[instance].Status; /* get exit status */
                put_user( ldata,(unsigned long *) buf ); /* update user data[0] */

                ldata = (unsigned long)ISR[instance].EventPending[0]; /* get the interrupt status[0] */
                put_user( ldata,(unsigned long *)( buf + (1*(sizeof(unsigned long)))) ); /* update user data[1] */

                ldata = (unsigned long)ISR[instance].EventPending[1];/* get the interrupt status[1] */
                put_user( ldata,(unsigned long *)( buf + (2*(sizeof(unsigned long)))) ); /* update user data[2] */

                ldata = (unsigned long)ISR[instance].EventPending[2];/* get the interrupt status[2] */
                put_user( ldata,(unsigned long *)( buf + (3*(sizeof(unsigned long)))) ); /* update user data[3] */
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
                   for(i = 0; i < MAX_APS; i++)              /* get all boards */
                   {
                     ldata = ( unsigned long )ap_address[i]; /* convert to long */
                     put_user( ldata, (unsigned long *)(arg+(i*(sizeof(unsigned long)))) );	/* update user data */
                   }    
		break;
		case 6:/* return IRQ number */
                   for(i = 0; i < MAX_APS; i++)              /* get all boards */
                   {
                     ldata = ( unsigned long )board_irq[i];          /* convert IRQ to long */
                     put_user( ldata, (unsigned long *)(arg+(i*(sizeof(unsigned long)))) );	/* update user data */
                   }
		break;
		case 21: /* wake up blocked thread so it can terminate */
	           get_user( ldata, (unsigned long *)arg  ); /* pickup instance number */
	           abWaitQueueFlag[ldata] = 2;               /* indicate complete */

                   /* Event Pending all zero = no interrupt = terminate wake-up */
                   ISR[ldata].Status = ISR[ldata].EventPending[0] = ISR[ldata].EventPending[1] = ISR[ldata].EventPending[2] = 0;
	           wake_up_interruptible( &apWaitQueue[ldata] ); /* wake the blocked write */
		break;
	}
	return((long)cmd );
}




static struct file_operations ap471_ops = {
  .owner = THIS_MODULE, /* owner of the world */
  .read = read,         /* read */
  .write = write,       /* write */
  .unlocked_ioctl = device_ioctl, /* unlocked_ioctl */
  .open = open,         /* open */
  .release = release,   /* release */
};



static irqreturn_t
ap471_handler( int irq, void *did )
/* ap471_handler( int irq, void *did, struct pt_regs *cpu_regs ) used in older Linux versions */
{
  volatile uint32_t uValue;
  volatile struct map471* pAPCard;
  int int_status, i;

   int_status = 0;              /* indicate interrupt not handled */
   for(i = 0; i < MAX_APS; i++) /* check all boards */
   {
     if( open_dev[i] )          /* if board is open */
     {
       pAPCard = (struct map471*)ap_address[i];
       uValue = readl((u32*)&pAPCard->InterruptRegister);
       if( uValue == ( AP_INT_ENABLE | AP_INT_PENDING ))/* board interrupt enabled and pending? */
       {
         writel( (long)0, (u32*)&pAPCard->InterruptRegister); /* disable interrupts from this board */

         int_status = 1; /* set interrupt serviced flag */

         /* read & save the three Event Pending Clear Registers */
         ISR[i].EventPending[0] = readl((u32*)&pAPCard->EventPendingClrReg[0]);
         ISR[i].EventPending[1] = readl((u32*)&pAPCard->EventPendingClrReg[1]);
         ISR[i].EventPending[2] = readl((u32*)&pAPCard->EventPendingClrReg[2]);
/*
printk("AP471_%XISR A=%lX P0=%X P1=%X P2=%X\n",i, (unsigned long)pAPCard, ISR[i].EventPending[0], ISR[i].EventPending[1], ISR[i].EventPending[2]);
*/
         /* if we were waiting for a blocked start AND received an interrupt */
         if(abWaitQueueFlag[i] == 0)
         {
           ISR[i].Status = abWaitQueueFlag[i] = 1;  /* when received, change the condition to true... */
           wake_up_interruptible( &apWaitQueue[i]); /* ... and wake the blocked write */
         }
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
  extern struct pci_dev *p471Board[MAX_APS];
  extern char devnamebuf[MAX_APS][32];
  extern struct AP471_isr_info ISR[MAX_APS];
  extern int abWaitQueueFlag[MAX_APS];	/* wait queues flags */
  struct pci_dev *pAP;
  int i,j,k;
  char devnumbuf[8];
  unsigned long region_size;

  memset( &p471Board[0], 0, sizeof(p471Board));
  memset( &open_dev[0], 0, sizeof(open_dev));
  memset( &msi_status[0], 0, sizeof(msi_status));
  memset( &board_irq[0], 0, sizeof(board_irq));
  memset( &ap_address[0], 0, sizeof(ap_address));
  memset( &ISR[0], 0, sizeof(ISR[MAX_APS]));
  memset( &abWaitQueueFlag[0], 2, sizeof(abWaitQueueFlag));

  pAP = NULL;
  for( i = 0, j = 0; i < MAX_APS; i++ )
  {
    pAP = ( struct pci_dev *)pci_get_device( VENDOR_ID, APBOARD, pAP ); /* AP471 */
    if( pAP )
    {
      p471Board[i] = pAP;
      ap_address[i] = (unsigned long)p471Board[i]->resource[0].start;
      region_size = p471Board[i]->resource[0].end - p471Board[i]->resource[0].start;
      region_size++;

      ap_address[i] = (unsigned long)ioremap( ap_address[i], region_size);

      if( ap_address[i] )
      {
        memset( &devnamebuf[i][0], 0, sizeof(devnamebuf[i]));
        memset( &devnumbuf[0], 0, sizeof(devnumbuf));
        strcpy(devnamebuf[i], DEVICE_NAME);
        sprintf(&devnumbuf[0],"%d",i);
        strcat(devnamebuf[i], devnumbuf);
        ret_val = pci_enable_device(p471Board[i]);

#ifdef AP_USE_MSI_INT
        msi_status[i] = pci_enable_msi(p471Board[i]);	/* Use Message Signaled Interrupts (MSI) */
#endif
        board_irq[i] = p471Board[i]->irq;

        ret_val = request_irq ( board_irq[i], (irq_handler_t)ap471_handler, IRQF_SHARED, devnamebuf[i], ( void *)ap_address[i] );

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
	ret_val = register_chrdev ( MAJOR_NUM, DEVICE_NAME, &ap471_ops );

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
           pci_disable_msi(p471Board[i]);
#endif
        iounmap( (void *)ap_address[i] );
        printk("%s unmapped I/O=%08lX IRQ=%02X\n",devnamebuf[i],(unsigned long)ap_address[i], board_irq[i]);
        open_dev[i] = 0;
      }
    }
  }
}

MODULE_LICENSE("GPL and additional rights");

