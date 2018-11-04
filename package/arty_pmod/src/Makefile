#Makefile
#obj-m += $(addsuffix .o, $(notdir $(basename $(filter-out %.c, $(wildcard $(BR2_EXTERNAL_ARTY_PMOD_PATH)/package/*.c)))))
obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_BREXT0_PATH)/package/arty_pmod/src/*.c))))

GIT_FOLDER = "/home/eddie/git"
EXTRA_CFLAGS := -I"$(GIT_FOLDER)/vivado/sdk/arty_br_app/src"
EXTRA_CFLAGS += -I"$(GIT_FOLDER)/vivado/sdk/arty_lnx_pmod_standalone_bsp_nosvc/microblaze_0/include"
EXTRA_CFLAGS += -DUBLAZE_FW

ccflags-y := -std=gnu99 -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all clean

all:
	$(info ******************* Running All $(OSTYPE) *******************)
	$(MAKE) -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	$(info ******************* Clean All $(OSTYPE) *******************)
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean


