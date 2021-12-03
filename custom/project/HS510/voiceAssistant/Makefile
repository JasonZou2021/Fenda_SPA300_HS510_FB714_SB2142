include $(TOP)/build/scripts/config.mk

INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/bt \
		-I$(OUT_SDK_INC_DIR)/common \
		-I$(OUT_SDK_INC_DIR)/posix \
		-I$(OUT_SDK_INC_DIR)/fsi \
		-I$(OUT_SDK_INC_DIR)/driver \
		-I$(OUT_SDK_INC_DIR)/iop_scanner \
		-I$(OUT_SDK_INC_DIR)/qp \
		-I$(CUSTOM_INC_DIR) \
		-I$(OUT_SDK_INC_DIR)/audDspService \
		-I$(OUT_SDK_INC_DIR)/settingmanager \
		-I$(OUT_SDK_INC_DIR)/systemservice \
		-I$(OUT_SDK_INC_DIR)



SOURCES	+= \
 	VoiceAssistService.c \
	VoiceAssist_API.c


TARGET = libapva.a

TARGET_MODULE = user

# [optional][default ram]may one of <ram|rom>
#TARGET_STORAGE = ram

#excute target build
$(eval $(BUILD_TARGET))

