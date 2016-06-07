DEBAYER_VERSION = 1.0
DEBAYER_SITE = $(BR2_EXTERNAL)/package/asc/debayer
DEBAYER_SITE_METHOD = local

define DEBAYER_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define DEBAYER_INSTALL_STAGING_CMDS
	$(INSTALL) -D -m 0777 $(@D)/debayer $(STAGING_DIR)/usr/bin/debayer
endef

define DEBAYER_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0777 $(@D)/debayer $(TARGET_DIR)/usr/bin/debayer
endef

$(eval $(generic-package))
