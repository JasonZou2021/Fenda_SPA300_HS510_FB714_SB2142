include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= 	-I$(CUSTOM_INC_DIR) \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/qp \
               -I$(OUT_SDK_INC_DIR)/posix \
               -I$(OUT_SDK_INC_DIR)/fsi \
               -I$(OUT_SDK_INC_DIR)/hardware \
               -I$(PROJECT_DIR)/display \
               -I$(OUT_SDK_INC_DIR)/driver

SOURCES	+= \
	    LED_Drv.c \

TARGET = libled.a

TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))
