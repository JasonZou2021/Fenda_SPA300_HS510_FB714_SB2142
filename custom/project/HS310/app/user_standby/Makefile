include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(OUT_SDK_INC_DIR)/systemservice\
				-I$(OUT_SDK_INC_DIR)/iop_scanner \


SOURCES	+= \
      user_standby.c\


TARGET = libuser_standby.a

TARGET_MODULE = user


#excute target build
$(eval $(BUILD_TARGET))

