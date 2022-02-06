include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(OUT_SDK_INC_DIR)/fsi \
				-I$(TOP)/custom/common/include \
				-I$(OUT_SDK_INC_DIR)/AudDspService \
				-I$(OUT_SDK_INC_DIR)/settingmanager \
				-I$(OUT_SDK_INC_DIR)/hardware \
				-I$(OUT_SDK_INC_DIR)/MCU \
				-I$(OUT_SDK_INC_DIR)/systemservice\
				-I$(OUT_SDK_INC_DIR)/iop_scanner \
				
SOURCES	+= \
      hdmi_app.c\
      IT6622.c\
IT6622_Fw_Update.c\

     
TARGET = libhdmiite.a

TARGET_MODULE = user


#excute target build
$(eval $(BUILD_TARGET))

