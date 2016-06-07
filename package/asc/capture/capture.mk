# capture

CAPTURE_VERSION = 1.0
CAPTURE_SITE = $(BR2_EXTERNAL)/package/asc/capture
CAPTURE_SITE_METHOD = local

define CAPTURE_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define CAPTURE_ISNTALL_STAGING_CMDS
	$(INSTALL) -D -m 0777 $(@D)/capture $(STAGING_DIR)/usr/bin/capture
endef

define CAPTURE_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0777 $(@D)/capture $(TARGET_DIR)/usr/bin/capture
endef

$(eval $(generic-package))
