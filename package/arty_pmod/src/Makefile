#Makefile
#obj-m += $(addsuffix .o, $(notdir $(basename $(filter-out %.c, $(wildcard $(BR2_EXTERNAL_ARTY_PMOD_PATH)/package/*.c)))))
obj-m += $(addsuffix .o, $(notdir $(basename $(wildcard $(BR2_EXTERNAL_BREXT0_PATH)/package/arty_pmod/src/*.c))))
#obj-m += pmod.o

ccflags-y := -DDEBUG -g -std=gnu99 -Wno-declaration-after-statement -Wframe-larger-than=1000000000

.PHONY: all clean

all:
	$(info ******************* Running All $(OSTYPE) *******************)
#	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' modules
	$(MAKE) -C '/lib/modules/$(shell uname -r)/build' M='$(PWD)' modules

clean:
	$(info ******************* Clean All $(OSTYPE) *******************)
	$(MAKE) -C '$(LINUX_DIR)' M='$(PWD)' clean


