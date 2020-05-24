pwd
insmod fs/configfs/configfs.ko
insmod drivers/usb/gadget/libcomposite.ko
insmod drivers/usb/gadget/function/u_ether.ko
insmod drivers/usb/gadget/function/usb_f_ss_lb.ko
insmod drivers/usb/gadget/function/usb_f_ecm.ko 
insmod drivers/usb/gadget/function/usb_f_ecm_subset.ko 
insmod drivers/usb/gadget/function/usb_f_eem.ko 
insmod drivers/usb/gadget/function/usb_f_rndis.ko 
insmod drivers/usb/gadget/legacy/g_ether.ko

