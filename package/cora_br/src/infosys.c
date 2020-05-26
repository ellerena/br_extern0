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
#include <linux/kobject.h>
#include <linux/of_platform.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/uaccess.h>        // Required for the copy to user function

#define XIL_TYPES_H
typedef char char8;
#include "regs.h"
#include "InfoModule.h"

#define PRND(x, ...)       //dev_info(x, ...)
#define PRNK(x, ...)       printk(x, ...)

#define DRIVER_NAME     "CoraLnxInfoDevice"
#define DEVICE_NAME     "dinfo"

#define COM_RST         (0)
#define COM_K           (1)
#define COM_CAP         (2)
#define COM_DMP         (3)

static int cd_probe(struct platform_device *pdev);
static int cd_remove(struct platform_device *pdev);

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

static struct kSpace_ {
    int irq;
    unsigned long mem_sz;
    void __iomem *base_addr;
} kSpace;

static int    mcom = 0;
static struct kobject *info_kobj = NULL;

static ssize_t dat_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
   u32 count, *ptemp, *p;

   ptemp = (u32*)kSpace.base_addr;
   count = 16;

    switch(mcom)
    {
       case COM_K:	/*1*/
       case COM_CAP:	/*2*/
          p = (u32*)buf;
          p[0] = RD_OFF32(INFO_DATE_OFFSET);
          p[1] = RD_OFF32(INFO_TIME_OFFSET);
          p[2] = RD_OFF32(INFO_VERSION_OFFSET);
          p[3] = RD_OFF32(INFO_PRODUCT_OFFSET);
          break;

       case COM_RST:	/*0*/
          sprintf(buf, __DATE__ " " __TIME__);
          count = strlen(buf);
          break;

       case COM_DMP:	/*3*/
          sprintf(buf, "Paris et Libere");
          break;
    }

    return count;
}

static ssize_t dat_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    volatile u32 *ptemp;

    ptemp = (u32*)kSpace.base_addr;
    mcom = (*buf) & 0x3;                  /* [0..3] */
    printk("\t%d [%x](%zu)\n", mcom, *buf, count);

    return count;
}
static struct kobj_attribute dat_file = __ATTR_RW(dat);

static int cd_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct resource *r_mem;             /* IO mem resources */
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

    if(!(info_kobj = kobject_create_and_add(DEVICE_NAME, kernel_kobj->parent)))
    {
        PRND(dev, "%d\n", __LINE__);
        return -ENOMEM;
    }
    if(sysfs_create_file(info_kobj, &dat_file.attr))
    {
        PRND(dev, "%d\n", __LINE__);
        kobject_put(info_kobj);
    }

    return rc;
}

static int cd_remove(struct platform_device *pdev)
{
    kobject_put(info_kobj);
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

