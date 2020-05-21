# MK file

CORA_BR_VERSION = 0.0.1
CORA_BR_SITE = $(BR2_EXTERNAL_BREXT0_PATH)/package/cora_br/src
CORA_BR_SITE_METHOD = local

#define ARTY_PMOD_BUILD_CMDS
#	$(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' PWD='$(@D)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
#endef

$(eval $(kernel-module))
$(eval $(generic-package))
