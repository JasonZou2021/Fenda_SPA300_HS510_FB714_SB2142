include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/settingmanager \
				-I$(OUT_SDK_INC_DIR)/systemservice \
				-I$(OUT_SDK_INC_DIR)/cfg_SDK \
				-I$(OUT_SDK_INC_DIR)/audioplayer \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/iop_scanner \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(PROJECT_DIR)/include \
				-I$(CUSTOM_INC_DIR) \
				-I$(OUT_SDK_INC_DIR)/bt


SOURCES	+= \
      Usersetting.c\
	  Usersetting_Default.c\

TARGET = libUsersetting.a

# [optional][default ram]may one of <ram|rom>
TARGET_STORAGE = ram

TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))

