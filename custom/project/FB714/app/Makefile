include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/iop_scanner \
				-I$(OUT_SDK_INC_DIR)/bt \
				-I$(OUT_SDK_INC_DIR)/display \
				-I$(OUT_SDK_INC_DIR)/common \
				-I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/ap \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(CUSTOM_INC_DIR) \
                        -I$(CUSTOM_INC_DIR)/HDMI\
				-I$(OUT_SDK_INC_DIR)/audioplayer \
				-I$(OUT_SDK_INC_DIR)/audDspService \
				-I$(OUT_SDK_INC_DIR)/settingmanager \
				-I$(OUT_SDK_INC_DIR)/cfg_SDK \
				-I$(OUT_SDK_INC_DIR)/pipe \
				-I$(OUT_SDK_INC_DIR)/dmr_service \
				-I$(OUT_SDK_INC_DIR)/systemservice\
				-I$(OUT_SDK_INC_DIR)/FileList \
				-I$(CUSTOM_INC_DIR)/bt \
				-I$(OUT_SDK_INC_DIR)/mcu_interface \
				-I$(OUT_SDK_INC_DIR)/fsi \

SOURCES	+= \
      testing_entry.c \
      amp_cfg.c \
      userapp.c \
	  cfg_sdk.c \
	  TvRemoteReady_cfg.c\
	  hint_data_proc.c \
	  complex_hint_proc.c \

SOURCES	+= mcu_if_sp.c
SOURCES	+= CertTest_if.c


TARGET = libap.a

TARGET_MODULE = user

# [optional][default ram]may one of <ram|rom>
TARGET_STORAGE = ram

# [optional]set makefile to make other target
OTHER_TARGET_MAKEFILE = Makefile.rom

#excute target build
$(eval $(BUILD_TARGET))

