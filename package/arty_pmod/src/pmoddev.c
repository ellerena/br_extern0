/*
 * main.c
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
#include <linux/fs.h>             // Header for the Linux file system support
#include <linux/uaccess.h>        // Required for the copy to user function
#include <linux/types.h>
#include <linux/mod_devicetable.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>
#include <linux/kobject.h>

#define _GCC_WRAP_STDINT_H      /* don't load stdint.h */
#include "BR_regs.h"

#define PRND(x, ...)       //dev_info(x, ...)
#define PRNK(x, ...)       //printk(x, ...)

#define DRIVER_NAME     "ArtyLnxPmodDevice"
#define DEVICE_NAME     "dpmod"
#define CLASS_NAME      "cpmod"

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
    { .compatible = "xlnx,pmodCapturer-1.0", },
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

struct kSpace_ {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

static int    maj, min;                     // Stores the device number -- determined automatically
static size_t mlen = 0;                     // Used to remember the size of the string stored
static struct class*  cdClass  = NULL;      // The device-driver class struct pointer
static struct device* cdDevice = NULL;      // The device-driver device struct pointer
static struct cdev* cdev = NULL;
static struct kSpace_ *kSpace = NULL;
static int    mcom = 0, mdat = 0;

static int cd_probe(struct platform_device *pdev)
{
    struct resource *r_mem;             /* IO mem resources */
    dev_t devno = 0;
    int rc = 0;

    PRND(dev, DEVICE_NAME ".probe\n");

    if (!(r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0)))
    {
        PRND(dev, "%d\n", __LINE__);
        return -ENODEV;
    }

    if (!(kSpace = (struct kSpace_ *) kmalloc(sizeof(struct kSpace_), GFP_KERNEL)))
    {
        PRND(dev, "%d\n", __LINE__);
        return -ENOMEM;
    }

    kSpace->mem_start = r_mem->start;
    kSpace->mem_end = r_mem->end;

    if (!request_mem_region(kSpace->mem_start, kSpace->mem_end - kSpace->mem_start + 1, DRIVER_NAME))
    {
        PRND(dev, "%d\n", __LINE__);
        rc = -EBUSY;
        goto error1;
    }

    if (!(kSpace->base_addr = ioremap(kSpace->mem_start, kSpace->mem_end - kSpace->mem_start + 1)))
    {
        PRND(dev, "%d\n", __LINE__);
        rc = -EIO;
        goto error2;
    }
    PRND(dev, "K Base Address: x%08x\n", *(unsigned*)(kSpace->base_addr));

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
    PRND(dev, ".probe completion code %d [%08x]\n", rc, (unsigned)kSpace->base_addr);

    return rc;

error5:
    class_destroy(cdClass);           // Repeated code but the alternative is goto statements
error4:
    unregister_chrdev(maj, DEVICE_NAME);
//error3:
    //free_irq(kSpace->irq, kSpace);
error2:
    release_mem_region(kSpace->mem_start, kSpace->mem_end - kSpace->mem_start + 1);
error1:
    kfree(kSpace);
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
    //dev_info(dev, "free irq.");
    //free_irq(kSpace->irq, kSpace);
    release_mem_region(kSpace->mem_start, kSpace->mem_end - kSpace->mem_start + 1);
    kfree(kSpace);
    kSpace = NULL;
    maj = 0;
    return 0;
}

static int cd_open(struct inode *inodep, struct file *filep)
{
    PRNK(DEVICE_NAME ".Open\n");
    return 0;
}

static ssize_t cd_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    volatile u32 *ptemp;

    ptemp = (u32*)kSpace->base_addr;

    mlen = copy_from_user((void*)&mcom, buffer, 1);     /* just need 1 byte */
    if(!mlen)                                           /* read Ok */
    {
        switch (mcom)
        {
            case COM_RST:
                WR_OFF32(PCAP_CTRL_OFFSET, 1);     // reset counter
                WR_OFF32(PCAP_CTRL_OFFSET, 0);
                break;

            case COM_CAP:
                WR_OFF32(PCAP_CTRL_OFFSET, 2);     // trigger capture 0
                WR_OFF32(PCAP_CTRL_OFFSET, 0);     // disable trigger 0
                break;
        }
        mlen = len;
    }
    else
    {
        mlen = 0;
    }
    PRNK(DEVICE_NAME ".Write mcom:%d pmod:x%08x\n", mcom, RD_OFF32(PCAP_CTRL_OFFSET));
    return mlen;
}

static ssize_t cd_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    unsigned i, temp;
    volatile u32 *ptemp;

    ptemp = (u32*)kSpace->base_addr;

    switch(mcom)
    {
        case COM_CAP:
            temp = RD_OFF32(PCAP_CTRL_OFFSET) - (1 << 16);
            WR_OFF32(PCAP_CTRL_OFFSET, temp);
            mdat = ~RD_OFF32(PCAP_VAL0_OFFSET);
            copy_to_user(buffer, (void*)&mdat, 4);
            mdat = ~RD_OFF32(PCAP_VAL1_OFFSET);
            copy_to_user(buffer + 4, (void*)&mdat, 4);
            break;
        case COM_DMP:
            temp = 0xff& (RD_OFF32(PCAP_CTRL_OFFSET) >> 16);
            for (i = 0; i < temp; i++)
            {
                WR_OFF32(PCAP_CTRL_OFFSET, (i << 16));
                mdat = ~RD_OFF32(PCAP_VAL0_OFFSET);
                copy_to_user(buffer, (void*)&mdat, 4);
                buffer += 4;
                mdat = ~RD_OFF32(PCAP_VAL1_OFFSET);
                copy_to_user(buffer, (void*)&mdat, 4);
                buffer += 4;
            }
            break;
        case COM_K:
        case COM_RST:
            mdat = RD_OFF32(PCAP_CTRL_OFFSET);
            copy_to_user(buffer, (void*)&mdat, 4);
            break;
    }

    PRNK(DEVICE_NAME ".Read mcom:%d pmod:x%08x len:%d\n", mcom, RD_OFF32(PCAP_CTRL_OFFSET), len);
    return len;
}

static int cd_release(struct inode *inodep, struct file *filep)
{
    PRNK(DEVICE_NAME ".Release\n");
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

