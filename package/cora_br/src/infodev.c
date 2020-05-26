/*
 * infodev.c
 *
 *  Created on: Oct 5, 2018
 *      Author: eddie
 */

#ifndef CONFIG_OF
#define CONFIG_OF
#endif

#include <linux/init.h>           // Macros used to mark up functions e.g. __init __exit
#include <linux/module.h>         // Core header for loading LKMs into the kernel
#include <linux/device.h>         // Header to support the kernel Driver Model
#include <linux/kernel.h>         // Contains types, macros, functions for the kernel
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/cdev.h>
#include <linux/uaccess.h>        // Required for the copy to user function
#include <linux/io.h>
#include <linux/types.h>

#define XIL_TYPES_H
typedef char char8;
#include "regs.h"
#include "InfoModule.h"

#define PRND(x, ...)    //dev_info(x, ...)
#define PRNK(x, ...)    //printk(x, ...)

#define DRIVER_NAME     "CoraLnxInfoDevice"
#define DEVICE_NAME     "dinfo"
#define CLASS_NAME      "cinfo"
#define BUFSZ           (256u)

#define COM_RST         (0)
#define COM_K           (1)
#define COM_CAP         (2)
#define COM_DMP         (3)

static int     cd_probe(struct platform_device *pdev);
static int     cd_remove(struct platform_device *pdev);
static int     cd_open(struct inode *, struct file *);
static int     cd_release(struct inode *, struct file *);
static ssize_t cd_read(struct file *, char *, size_t, loff_t *);
static ssize_t cd_write(struct file *, const char *, size_t, loff_t *);

#ifdef CONFIG_OF
static const struct of_device_id drv_of_match[] = {
    { .compatible = "xlnx,InfoModule-1.0", },
    { /* end of list */ },
};

MODULE_DEVICE_TABLE(of, drv_of_match);
#else
#define drv_of_match
#endif

static struct platform_driver plat_driver = {
   .probe  = cd_probe,
   .remove = cd_remove,
   .driver = { .name = DRIVER_NAME,
               .of_match_table = drv_of_match,
               .owner = THIS_MODULE,},
};

static struct file_operations fo = {
   .open = cd_open,
   .write = cd_write,
   .read = cd_read,
   .release = cd_release,
};

static struct kSpace_ {
   int irq;
   unsigned long mem_sz;
   void __iomem *base_addr;
} kSpace;

static int    maj, min;                     // Stores the device number -- determined automatically
static struct class*  cdClass  = NULL;      // The device-driver class struct pointer
static struct device* cdDevice = NULL;      // The device-driver device struct pointer
static struct cdev* cdev = NULL;
static int    mcom = 0;
static char   mdat[BUFSZ] = "Paris et libere";

static int cd_probe(struct platform_device *pdev)
{
   struct device *dev = &pdev->dev;
   struct resource *r_mem;             /* IO mem resources */
   dev_t devno = 0;
   int rc = 0;

   dev_info(dev, DEVICE_NAME ".probe. " __DATE__ " " __TIME__ "\n");

   if (!(r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0)))
   {
      PRND(dev, "%d\n", __LINE__);
      return -ENODEV;
   }

   if (!(kSpace.base_addr = devm_ioremap_resource(dev, r_mem)))
   {
      PRND(dev, "%d\n", __LINE__);
      return -EIO;
   }
   kSpace.mem_sz = r_mem->end - r_mem->start + 1;

   if(maj)                 /* allocate or register character device */
   {
      devno = MKDEV(maj, min);
      register_chrdev_region(devno, 1, DRIVER_NAME);
   }
   else
   {
      alloc_chrdev_region(&devno, min, 1, DRIVER_NAME);
      maj = MAJOR(devno);
   }

   PRND(dev, "register device class\n");
   if (IS_ERR(cdClass = class_create(THIS_MODULE, CLASS_NAME))) goto error4;

   PRND(dev, "register device create\n");
   if(IS_ERR(cdDevice = device_create(cdClass, NULL, MKDEV(maj, 0), NULL, DEVICE_NAME))) goto error5;

   cdev = cdev_alloc();
   cdev->ops = &fo;
   cdev->owner = THIS_MODULE;
   rc = cdev_add(cdev, devno, 1);
   PRND(dev, ".probe completion code %d [%08x]\n", rc, (unsigned)kSpace.base_addr);

   return rc;

error5:
   class_destroy(cdClass);           // Repeated code but the alternative is goto statements
error4:
   unregister_chrdev(maj, DEVICE_NAME);
//error3:
   //free_irq(kSpace.irq, kSpace);
   return rc;
}

static int cd_remove(struct platform_device *pdev)
{
   struct device *dev = &pdev->dev;
   dev_t devno = MKDEV(maj, 0);

   dev_info(dev, DEVICE_NAME ".remove\n");
   cdev_del(cdev);
   device_destroy(cdClass, devno);
   //class_unregister(cdClass);
   class_destroy(cdClass);
   unregister_chrdev_region(devno, 1);
   //free_irq(kSpace.irq, kSpace);
   maj = 0;
   return 0;
}

static int cd_open(struct inode *inodep, struct file *filep)
{
   PRNK(DEVICE_NAME ".open\n");
   return 0;
}

static ssize_t cd_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
   u32 *ptemp, *pmdat;

   ptemp = (u32*)kSpace.base_addr;
   pmdat = (u32*)mdat;
   len = 16;

   switch(mcom)
   {
      case COM_K:   /*1*/
      case COM_CAP: /*2*/
         pmdat[0] = RD_OFF32(INFO_DATE_OFFSET);
         pmdat[1] = RD_OFF32(INFO_TIME_OFFSET);
         pmdat[2] = RD_OFF32(INFO_VERSION_OFFSET);
         pmdat[3] = RD_OFF32(INFO_PRODUCT_OFFSET);
         break;

      case COM_RST: /*0*/
         sprintf(mdat, __DATE__ " " __TIME__);
         len = strlen(mdat);
         break;

      case COM_DMP: /*3*/
         sprintf((void*)pmdat, "Paris et Libere");
         break;
   }

   copy_to_user(buffer, (void*)pmdat, len);

   return len;
}

static ssize_t cd_write(struct file *filep, const char *buf, size_t len, loff_t *offset)
{
   volatile u32 *ptemp;

   ptemp = (u32*)kSpace.base_addr;
   copy_from_user((void*)&mcom, buf, 1); /* just need 1 byte */
   mcom &= 0x3;                          /* [0..3] */
   printk("\t%d (%zu)\n", mcom, len);

   return len;
}

static int cd_release(struct inode *inodep, struct file *filep)
{
   PRNK(DEVICE_NAME ".release\n");
   return 0;
}

static int init_drv(void)
{
   return platform_driver_register(&plat_driver);
}

static void exit_drv(void)
{
   platform_driver_unregister(&plat_driver);
}

module_init(init_drv);
module_exit(exit_drv);

MODULE_AUTHOR("Eddie Llerena");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(DEVICE_NAME);
MODULE_VERSION("0.1");

