include $(TOP)/build/scripts/config.mk

# Tell make to search include files from INCLUDE_DIR
# [optional]default include already:<common|module|hardware|driver|qp|posix|fsi> under <src/include> and  <./include>
INCLUDE_DIR += -I$(OUT_SDK_INC_DIR)/cfg_SDK \
							-I$(OUT_SDK_INC_DIR)/common \
							-I$(OUT_SDK_INC_DIR)/bt \
							-I$(OUT_SDK_INC_DIR)/bt/bt_platform \
							-I$(OUT_SDK_INC_DIR)/posix \
							-I$(OUT_SDK_INC_DIR)/audDspService \
							-I$(OUT_SDK_INC_DIR)/qp \
							-I$(OUT_SDK_INC_DIR)/driver \
							-I$(OUT_SDK_INC_DIR)/systemservice \
							-I$(CUSTOM_INC_DIR)/bt \
							-I$(OUT_SDK_INC_DIR)/settingmanager \
							-I$(OUT_SDK_INC_DIR)/iop_scanner \


SOURCES += \
	bt_ao_sub_action.c \
	bt_ao.c \
	bt_transmit.c \
	bt_ao_auto.c \
	bt_ao_audio.c \
	bt_ao_comment_action.c \
	bt_ao_service_api.c \


TARGET = libbt_sub_ao.a
#RELEASE_API_H += bt/bt_ao_service_api.h  bt/bt_common.h

# Set Module the target belong, maybe one of <MODULELIST> set in Makefile.am
# may be one of <base iop iop_rst fs bt net audio cli other dsptool
TARGET_MODULE = bt


# [optional]set makefile to make other target
OTHER_TARGET_MAKEFILE = Makefile_main

#excute target build
$(eval $(BUILD_TARGET))


