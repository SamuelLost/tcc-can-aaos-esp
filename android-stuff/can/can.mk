PRODUCT_PACKAGES += \
	candump \
	cansend \
	can-calc-bit-timing \
	canbusload \
	canfdtest \
	cangen \
	cangw \
	canlogserver \
	canplayer \
	cansniffer \
	isotpdump \
	isotprecv \
	isotpsend \
	isotpserver \
	isotpsniffer \
	isotptun \
	isotpperf \
	log2asc \
	log2long \
	slcan_attach \
	slcand \
	slcanpty

PRODUCT_COPY_FILES += \
	$(LOCAL_PATH)/modules/mcp251x.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/mcp251x.ko \
	$(LOCAL_PATH)/modules/can-dev.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/can-dev.ko \
	$(LOCAL_PATH)/modules/spi-bcm2835.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/spi-bcm2835.ko \
	$(LOCAL_PATH)/modules/can.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/can.ko \
	$(LOCAL_PATH)/modules/can-raw.ko:$(TARGET_COPY_OUT_VENDOR)/lib/modules/can-raw.ko
