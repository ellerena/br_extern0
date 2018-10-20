# MK file

ARTY_PMOD_VERSION = 0.0.1
ARTY_PMOD_SITE = $(BR2_EXTERNAL_BREXT0_PATH)/package/arty_pmod/src
ARTY_PMOD_SITE_METHOD = local

#define ARTY_PMOD_BUILD_CMDS
#	$(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' PWD='$(@D)' CC='$(TARGET_CC)' LD='$(TARGET_LD)'
#endef

$(eval $(kernel-module))
$(eval $(generic-package))
