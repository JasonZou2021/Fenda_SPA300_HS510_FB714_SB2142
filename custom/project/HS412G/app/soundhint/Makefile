include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(TOP)/custom/common/include \
				-I$(OUT_SDK_INC_DIR)/audDspService \

SOURCES	+= \
      HintData_BtReady.c \

TARGET = libsoundhint.a

TARGET_MODULE = user

TARGET_STORAGE = ram

# [optional]set makefile to make other target
OTHER_TARGET_MAKEFILE = Makefile.rom

#excute target build
$(eval $(BUILD_TARGET))

