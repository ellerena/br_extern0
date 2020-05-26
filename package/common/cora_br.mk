# MK file

COMMON_VERSION = 0.0.1
COMMON_SITE = $(BR2_EXTERNAL_BREXT0_PATH)/package/common/src
COMMON_SITE_METHOD = local

define COMMON_BUILD_CMDS
	$(info ******************* Compiling *******************)
	$(MAKE) -C /home/eddie/git/elle_luna/Intel/intel_gcc_lnx_cgi/builds/
endef

#$(eval $(kernel-module))
$(eval $(generic-package))
#$(eval $(host-generic-package))
