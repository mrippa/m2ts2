
/*
{+D}
   SYSTEM:          Acromag AP236

   MODULE NAME:     dev236.c

   VERSION:         B

   CREATION DATE:   12/01/15

   CODED BY:        FJM

   ABSTRACT:        ap236 device.

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
6/22/20  FJM   Changed ioremap_nocache() to ioremap()

{-D}
*/

/* AP236 device */

#ifndef BUILDING_FOR_KERNEL
#define BUILDING_FOR_KERNEL /* controls conditional inclusion in file apcommon.h */
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
#include "../AP236.h"

#define MAJOR_NUM 46

int open_dev[MAX_APS];
unsigned long ap_address[MAX_APS];
struct pci_dev *p236Board[MAX_APS];
int ret_val = 0;
char devnamebuf[MAX_APS][32];

struct file;
struct inode;
struct pt_regs;

static int
open(struct inode *inode, struct file *fp)
{
   int minor;

   minor = inode->i_rdev & 0xf;

   if (minor > (MAX_APS - 1))
      return (-ENODEV);

   if (open_dev[minor])
      return (-EBUSY);

   open_dev[minor] = 1;

   return (0);
}

static int
release(struct inode *inode, struct file *fp)
{
   int minor;

   minor = inode->i_rdev & 0xf;

   if (minor > (MAX_APS - 1))
      return (-ENODEV);

   if (open_dev[minor])
   {
      open_dev[minor] = 0;
      return (0);
   }
   return (-ENODEV);
}

static ssize_t
read(struct file *fp, char *buf, size_t length, loff_t *offset)
{
   unsigned long adata, ldata;
   unsigned short sdata;
   unsigned char cdata;

   get_user(adata, (unsigned long *)buf); /* pickup address */
   switch (length)
   {
   case 1: /* 8 bit */
      cdata = readb((void *)adata);
      ldata = (unsigned long)cdata; /* convert to long */
      break;
   case 2: /* 16 bit */
      sdata = readw((void *)adata);
      ldata = (unsigned long)sdata; /* convert to long */
      break;
   case 4: /* 32 bit */
      ldata = readl((void *)adata);
      break;
   default:
      cdata = sdata = adata = ldata = 0;
      return (-EINVAL);
      break;
   }
   put_user(ldata, (unsigned long *)(buf + (sizeof(unsigned long)))); /* update user data */
   return (length);
}

static ssize_t
write(struct file *fp, const char *buf, size_t length, loff_t *offset)
{
   unsigned long adata, ldata;

   get_user(adata, (unsigned long *)buf);                             /* pickup address */
   get_user(ldata, (unsigned long *)(buf + (sizeof(unsigned long)))); /* pickup data */
   switch (length)
   {
   case 1: /* 8 bit */
      writeb((int)ldata, (void *)adata);
      break;
   case 2: /* 16 bit */
      writew((int)ldata, (void *)adata);
      break;
   case 4: /* 32 bit */
      writel((int)ldata, (void *)adata);
      break;
   default:
      return (-EINVAL);
      break;
   }
   return (length);
}

static long
device_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
   unsigned long ldata;
   int i;
   switch (cmd)
   {
   case 0: /* reserved for future use */
   case 1:
   case 2:
   case 3:
   case 4:
   case 5:                          /* return address */
      for (i = 0; i < MAX_APS; i++) /* get all boards */
      {
         ldata = (unsigned long)ap_address[i];                                    /* convert to long */
         put_user(ldata, (unsigned long *)(arg + (i * (sizeof(unsigned long))))); /* update user data */
      }
      break;
   }
   return ((long)cmd);
}

static struct file_operations ap236_ops = {
    .owner = THIS_MODULE,           /* owner of the world */
    .read = read,                   /* read */
    .write = write,                 /* write */
    .unlocked_ioctl = device_ioctl, /* unlocked_ioctl */
    .open = open,                   /* open */
    .release = release,             /* release */
};

int init_module(void)
{
   extern struct pci_dev *p236Board[MAX_APS];
   extern char devnamebuf[MAX_APS][32];
   struct pci_dev *p236;
   int i, j;
   char devnumbuf[8];
   unsigned long region_size;

   memset(&p236Board[0], 0, sizeof(p236Board));
   memset(&open_dev[0], 0, sizeof(open_dev));
   memset(&ap_address[0], 0, sizeof(ap_address));

   p236 = NULL;
   for (i = 0, j = 0; i < MAX_APS; i++)
   {
      p236 = (struct pci_dev *)pci_get_device(VENDOR_ID, AP236, p236); /* AP236 */
      if (p236)
      {
         p236Board[i] = p236;
         ap_address[i] = (unsigned long)p236Board[i]->resource[0].start;
         region_size = p236Board[i]->resource[0].end - p236Board[i]->resource[0].start;
         region_size++;

         ap_address[i] = (unsigned long)ioremap(ap_address[i], region_size);
         if (ap_address[i])
         {
            memset(&devnamebuf[i][0], 0, sizeof(devnamebuf[i]));
            memset(&devnumbuf[0], 0, sizeof(devnumbuf));
            strcpy(devnamebuf[i], DEVICE_NAME);
            sprintf(&devnumbuf[0], "%d", i);
            strcat(devnamebuf[i], devnumbuf);
            ret_val = pci_enable_device(p236Board[i]);

            printk("%s mapped   I/O=%08lX IRQ=NONE Rv=%X\n", devnamebuf[i], (unsigned long)ap_address[i], ret_val);
            j++;
         }
      }
      else
         break;
   }
   if (j) /* found at least one device */
   {
      ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &ap236_ops);

      if (ret_val < 0)
      {
         printk(DEVICE_NAME);
         printk(" Failed to register error = %d\n", ret_val);
      }
      else
         return (0);
   }
   return (-ENODEV);
}

void cleanup_module(void)
{
   extern char devnamebuf[MAX_APS][32];
   int i;

   if (ret_val >= 0)
   {
      unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
      for (i = 0; i < MAX_APS; i++)
      {
         if (ap_address[i])
         {
            iounmap((void *)ap_address[i]);
            printk("%s unmapped I/O=%08lX IRQ=NONE\n", devnamebuf[i], (unsigned long)ap_address[i]);
            open_dev[i] = 0;
         }
      }
   }
}

MODULE_LICENSE("GPL and additional rights");
