include $(TOP)/build/scripts/config.mk

# Tell make to search include files from INCLUDE_DIR
# [optional]default include already:<common|module|hardware|driver|qp|posix|fsi> under <src/include> and  <./include>
INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/posix \
               -I$(OUT_SDK_INC_DIR)/driver \
               -I$(CUSTOM_INC_DIR) \
               -I$(OUT_SDK_INC_DIR)/freertos \
               -I$(OUT_SDK_INC_DIR)/cfg_SDK \
               -I$(OUT_SDK_INC_DIR)/systemservice\
               -I$(OUT_SDK_INC_DIR)/MCU \
               -I$(OUT_SDK_INC_DIR)/qp \
               -I$(OUT_SDK_INC_DIR)/iop_scanner \
			   -I$(OUT_SDK_INC_DIR)/fsi \
			   -I$(OUT_SDK_INC_DIR)/audDspService \

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/gpio_intr

SOURCES	+= \
	Display_Srv.c \
	VFD_Srv.c	\
	KEY_Srv.c	\
	Display_cfg.c	\
	VFD_Srv_API.c \
	KEY_gpio.c \
	LED_Srv.c	\
	LED_Srv_API.c \
	vfd_app.c \
	vfd_seg_map.c 

ifeq ($(CFG_REPEATER_TYPE),1)
SOURCES	+= Repeater_ITE.c
else ifeq ($(CFG_REPEATER_TYPE),0)
SOURCES	+= Repeater_EP.c
endif

TARGET = libdisplay.a


# Set Module the target belong, maybe one of <MODULELIST> set in Makefile.am
# may be one of <base iop iop_rst fs bt net audio cli other dsptool
TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))


