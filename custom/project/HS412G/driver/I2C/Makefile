include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(TOP)/custom/common/include \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/fsi \
               -I$(OUT_SDK_INC_DIR)/driver 

SOURCES	+= \
      AnD_I2C_Srv.c \


TARGET = libi2c_self.a

TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))
