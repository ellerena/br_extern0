// Sample module 0
//

#include <linux/module.h>
#include <linux/init.h>

#define MODNAME	 "module_0"

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Eddie Llerena");     ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Arty Lnx Pmod module");  ///< The description -- see modinfo
MODULE_VERSION("0.1"); 

static int __init module_0_init(void)
{
	printk(KERN_INFO MODNAME " - init\n");
	return 0;
}

static void __exit module_0_exit(void)
{
	printk(KERN_INFO MODNAME " - exit\n");
}

module_init(module_0_init);
module_exit(module_0_exit);
