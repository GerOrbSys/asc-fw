# spi

SPI_VERSION = 0.1
SPI_SITE = $(BR2_EXTERNAL)/package/asc/spi
SPI_SITE_METHOD = local

define SPI_BUILD_CMDS
	$(MAKE) CC="$(TARGET_CC)" LD="$(TARGET_LD)" -C $(@D) all
endef

define SPI_ISNTALL_STAGING_CMDS
	$(INSTALL) -D -m 0777 $(@D)/spi $(STAGING_DIR)/usr/bin/spi
endef

define SPI_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0777 $(@D)/spi $(TARGET_DIR)/usr/bin/spi
endef

$(eval $(generic-package))
