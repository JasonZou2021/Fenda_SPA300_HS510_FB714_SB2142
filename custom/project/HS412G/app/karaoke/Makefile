include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(OUT_SDK_INC_DIR)/audDspService \
				-I$(OUT_SDK_INC_DIR)/settingmanager\
				-I$(OUT_SDK_INC_DIR)/bt \
				-I$(OUT_SDK_INC_DIR)/iop_scanner \
				-I$(OUT_SDK_INC_DIR)/systemservice \

SOURCES	+= \
      karaoke.c\


TARGET = libkaraoke.a

TARGET_MODULE = user


#excute target build
$(eval $(BUILD_TARGET))

