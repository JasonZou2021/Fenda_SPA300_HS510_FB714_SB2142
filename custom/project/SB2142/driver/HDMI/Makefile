include $(TOP)/build/scripts/config.mk

# Tell make to search include files from INCLUDE_DIR
# [optional]default include
already:<common|module|hardware|driver|qp|posix|fsi> under <src/include> and  <./include/HDMI>
INCLUDE_DIR	+= -I$(CUSTOM_INC_DIR) \
               -I$(CUSTOM_INC_DIR)/HDMI\
               -I$(OUT_SDK_INC_DIR)/posix \
               -I$(OUT_SDK_INC_DIR)/freertos \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/fsi \
               -I$(OUT_SDK_INC_DIR)/driver\
               -I$(OUT_SDK_INC_DIR)/iop_scanner \
               -I$(OUT_SDK_INC_DIR)/qp\
               -I$(OUT_SDK_INC_DIR)/systemservice\
               -I$(OUT_SDK_INC_DIR)/cfg_SDK


# The source files to make.
SOURCES += HDMI_Srv.c\
	cec_main.c \
	cec_cmd.c \
	cec_opfun.c

# SOURCES += edid.c

# The lib name which you want to archive
TARGET = libhdmi.a
#RELEASE_API_H += driver/HDMI/cec_user_ctl.h

# Set Module the target belong, maybe one of <MODULELIST> set in Makefile.am
# may be one of <base iop iop_rst fs bt net audio cli other dsptool
TARGET_MODULE = base

#excute target build
$(eval $(BUILD_TARGET))