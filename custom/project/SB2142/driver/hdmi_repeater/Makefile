include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(CUSTOM_INC_DIR) \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/fsi \
               -I$(OUT_SDK_INC_DIR)/driver

SOURCES	+= EP_hdmi_repeater.c


TARGET = libhdmi_repeater.a

# [optional][default ram]may one of <ram|rom>
TARGET_STORAGE = ram

# Set Module the target belong, maybe one of <MODULELIST> set in Makefile.am
# may be one of <base iop iop_rst fs bt net audio cli other dsptool
TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))
