#Makefile

export ARCH:=arm
export CROSS_COMPILE:=arm-xilinx-linux-gnueabi-

obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_BREXT0_PATH)/package/cora_br/src/*.c))))

GIT_FOLDER = "/home/eddie/git"
EXTRA_CFLAGS := -I"$(GIT_FOLDER)/vivado/sdk/cora_br_app/inc"
EXTRA_CFLAGS += -I"$(GIT_FOLDER)/vivado/sdk/cora_br_standalone_bsp_nosvc/ps7_cortexa9_0/include"
EXTRA_CFLAGS += -DNOXIL -Wno-error=date-time 
##KDIR := "/home/eddie/git/petalnx/tools/linux-i386/gcc-arm-linux-gnueabi/arm-linux-gnueabihf/libc/usr/include"
##"/home/eddie/git/petalnx/corabr/build/tmp/work-shared/plnx-zynq7/kernel-source/tools/include"
##"/home/eddie/git/petalnx/tools/linux-i386/gcc-arm-linux-gnueabi/arm-linux-gnueabihf/libc/usr/include"
##"/home/eddie/git/petalnx/corabr/build/tmp/work-shared/plnx-zynq7/kernel-source/include"
KDIR := '/lib/modules/$(shell uname -r)/build'

ccflags-y := -std=gnu99 -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all clean

all:
	$(info ******************* Running All $(OSTYPE) *******************)
	$(MAKE) -C $(KDIR) M='$(PWD)' modules

clean:
	$(info ******************* Clean All $(OSTYPE) *******************)
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean


