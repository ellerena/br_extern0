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

#include <linux/mod_devicetable.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/io.h>
#include <linux/interrupt.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#define DRIVER_NAME     "Arty Lnx Pmod Driver"
#define DEVICE_NAME     "dArtypmod"
#define CLASS_NAME      "cArtypmod"

static int    maj, min;                     ///< Stores the device number -- determined automatically
static char   mtxt[256] = {0};              ///< Memory for the string that is passed from userspace
static short  mlen = 0;                     ///< Used to remember the size of the string stored
static struct class*  cdClass  = NULL;      ///< The device-driver class struct pointer
static struct device* cdDevice = NULL;      ///< The device-driver device struct pointer
static struct cdev* cdev = NULL;

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

struct kSpace {
    int irq;
    unsigned long mem_start;
    unsigned long mem_end;
    void __iomem *base_addr;
};

static int cd_probe(struct platform_device *pdev)
{
    struct resource *r_mem;             /* IO mem resources */
    struct device *dev = &pdev->dev;
    struct kSpace *lp = NULL;
    dev_t devno = 0;
    int rc = 0;

    dev_info(dev, DEVICE_NAME ".probe\n");

    if (!(r_mem = platform_get_resource(pdev, IORESOURCE_MEM, 0)))
    {
        dev_err(dev, "invalid address\n");
        return -ENODEV;
    }

    if (!(lp = (struct kSpace *) kmalloc(sizeof(struct kSpace), GFP_KERNEL)))
    {
        dev_err(dev, "Cound not allocate mydrv device\n");
        return -ENOMEM;
    }

    dev_set_drvdata(dev, lp);

    lp->mem_start = r_mem->start;
    lp->mem_end = r_mem->end;

    if (!request_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1, DRIVER_NAME))
    {
        dev_err(dev, "request_mem_region failed @%p\n", (void *)lp->mem_start);
        rc = -EBUSY;
        goto error1;
    }

    if (!(lp->base_addr = ioremap(lp->mem_start, lp->mem_end - lp->mem_start + 1)))
    {
        dev_err(dev, "ioremap failes.\n");
        rc = -EIO;
        goto error2;
    }

    devno = MKDEV(maj, min);
    rc = maj ? register_chrdev_region(devno, 1, DRIVER_NAME) : alloc_chrdev_region(&devno, min, devs, DRIVER_NAME);
    maj = MAJOR(devno);
    dev_info(dev, "allocating cd region %d:%d, result:%d\n", maj, min, rc);
    dev_info(dev, "reg0: x%08x, reg1: x%08x\n", *(unsigned*)(lp->base_addr),*(unsigned*)(lp->base_addr + 4));
    if (rc <0) return rc;

    dev_info(dev, "register device class\n");
    if (IS_ERR(cdClass = class_create(THIS_MODULE, CLASS_NAME)))
    {
        goto error4;
    }

    dev_info(dev, "register device create\n");
    if(IS_ERR(cdDevice = device_create(cdClass, NULL, MKDEV(maj, 0), NULL, DEVICE_NAME)))
    {
        goto error5;
    }

    cdev_init(cdev, &fo);
    cdev->owner = THIS_MODULE;
    rc = cdev_add(cdev, devno, 1);
    dev_info(dev, ".probe completion code %d [%08x]\n", rc, (unsigned)lp->base_addr);

    return rc;

    error5:
    class_destroy(cdClass);           // Repeated code but the alternative is goto statements
    error4:
    unregister_chrdev(maj, DEVICE_NAME);
    //error3:
    //free_irq(lp->irq, lp);
    error2:
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    error1:
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    return rc;
}

static int cd_remove(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct kSpace *lp = dev_get_drvdata(dev);
    dev_t devno = MKDEV(maj, 0);

    dev_info(dev, DEVICE_NAME ".remove\n");
    cdev_del(cdev);
    device_destroy(cdClass, devno);
    //class_unregister(cdClass);
    class_destroy(cdClass);
    unregister_chrdev_region(devno, 1);
    //dev_info(dev, "free irq.");
    //free_irq(lp->irq, lp);
    release_mem_region(lp->mem_start, lp->mem_end - lp->mem_start + 1);
    kfree(lp);
    dev_set_drvdata(dev, NULL);
    maj = 0;
    return 0;
}

static int cd_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO DEVICE_NAME ".Open\n");
    return 0;
}

static ssize_t cd_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{

    mlen = copy_from_user(mtxt, buffer, len);
    if(!mlen)  /* data transfer ok */
    {
        mlen = len;
        mtxt[mlen] = 0;
    }
    else
    {
        mlen = 0;
    }
    printk(KERN_INFO DEVICE_NAME ".Write [%d]\n", mlen);
    return mlen;
}

static ssize_t cd_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    len = copy_to_user(buffer, mtxt, mlen);
    printk(KERN_INFO DEVICE_NAME ".Read [%d] %s\n", mlen, mtxt);
    return len;
}

static int cd_release(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO DEVICE_NAME ".Release\n");
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

