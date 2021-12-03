include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(CUSTOM_INC_DIR) \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/CLI \
			   -I$(OUT_SDK_INC_DIR)/qp \
			   -I$(OUT_SDK_INC_DIR)/posix \
			   -I$(OUT_SDK_INC_DIR)/iop_scanner \
			   -I$(OUT_SDK_INC_DIR)/systemservice \
			   -I$(OUT_SDK_INC_DIR)/audDspService \
			   -I$(OUT_SDK_INC_DIR)/driver \
			   -I$(PROJECT_DIR)/app \

SOURCES	:= usercmd.c

TARGET = libusercmd.a

# [optional][default ram]may one of <ram|rom>
TARGET_STORAGE = ram

TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))
