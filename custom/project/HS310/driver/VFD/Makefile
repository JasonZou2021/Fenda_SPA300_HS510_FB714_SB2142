include $(TOP)/build/scripts/config.mk

# Tell make to search include files from INCLUDE_DIR
# [optional]default include already:<common|module|hardware|driver|qp|posix|fsi> under <src/include> and  <./include>
INCLUDE_DIR	+= -I$(OUT_SDK_INC_DIR)/cfg_SDK \
				-I$(OUT_SDK_INC_DIR)/hardware \
				-I$(OUT_SDK_INC_DIR)/freertos \
				-I$(OUT_SDK_INC_DIR)/driver \
				-I$(OUT_SDK_INC_DIR)/hardware \
				-I$(CUSTOM_INC_DIR) \
				-I$(OUT_SDK_INC_DIR)/qp \
				-I$(OUT_SDK_INC_DIR)/posix \
				-I$(OUT_SDK_INC_DIR)/common \

# The source files to make.
SOURCES	+= \
		VFD_Pt6315_Drv.c \
		VFD_Nec16312_Drv.c \
		vfd_api1944.c \
		VFD.c

# The lib name which you want to archive
TARGET = libdisplayvfd.a

# Set Module the target belong, maybe one of <MODULELIST> set in Makefile.am
TARGET_MODULE = user

#excute target build
$(eval $(BUILD_TARGET))