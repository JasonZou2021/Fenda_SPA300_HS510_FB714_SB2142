include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(CUSTOM_INC_DIR) \
               -I$(OUT_SDK_INC_DIR)/common \
               -I$(OUT_SDK_INC_DIR)/fsi \
               -I$(OUT_SDK_INC_DIR)/driver

SOURCES	+= \
      AD82128.c \
	  amp.c


TARGET = libamp.a

# [optional][default ram]may one of <ram|rom>
TARGET_STORAGE = ram

TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))
