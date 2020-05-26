#Makefile
#For building kernel modules using Buildroot toolchain
#from either eclipse or buildroot

GIT_FOLDER = /home/eddie/git
ifndef BR2_EXTERNAL_BREXT0_PATH
BR_CROSS := /home/eddie/git/buildroot/output/host/bin/
BR2_EXTERNAL_BREXT0_PATH := $(GIT_FOLDER)/br_extern0
KDIR := /home/eddie/git/buildroot/output/build/linux-custom
endif
EXTRA_CFLAGS := -I"$(GIT_FOLDER)/vivado/sdk/cora_br_app/inc"
EXTRA_CFLAGS += -I"$(GIT_FOLDER)/vivado/sdk/cora_br_standalone_bsp_nosvc/ps7_cortexa9_0/include"
EXTRA_CFLAGS += -DNOXIL -Wno-date-time -Wno-error=date-time -Wno-unused-result
##KDIR := "/home/eddie/git/petalnx/tools/linux-i386/gcc-arm-linux-gnueabi/arm-linux-gnueabihf/libc/usr/include"
##"/home/eddie/git/petalnx/corabr/build/tmp/work-shared/plnx-zynq7/kernel-source/tools/include"
##"/home/eddie/git/petalnx/tools/linux-i386/gcc-arm-linux-gnueabi/arm-linux-gnueabihf/libc/usr/include"
##"/home/eddie/git/petalnx/corabr/build/tmp/work-shared/plnx-zynq7/kernel-source/include"
KDIR ?= '/lib/modules/$(shell uname -r)/build'

export ARCH:=arm
export CROSS_COMPILE:=$(BR_CROSS)arm-buildroot-linux-gnueabihf-
obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_BREXT0_PATH)/package/cora_br/src/*.c))))
ccflags-y := -std=gnu99 -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all clean

all: | $(KDIR)
	$(info ******************* Running All $(OSTYPE) *******************)
	$(MAKE) -C $(KDIR) M='$(PWD)' modules
	
$(KDIR):
	$(error $(KDIR) folder missing)

clean:
	$(info ******************* Clean All $(OSTYPE) *******************)
	$(MAKE) -C '$(KDIR)' M='$(PWD)' clean


