#----------------------------------------------------------/
#Usage: sinclude by Makefile.include
#Description: define Makefile variables
#----------------------------------------------------------/
ifneq ($(OS),Windows_NT)
SHELL=/bin/bash
endif

CONFIG_MK_TAG = 1
#disable find TOP,not need
ifeq ($(FIND_TOP),true)
#fix TOP when define from Makefile
ifeq ($(origin TOP),file)
TOP =
PARENT_DIR := $(CURDIR)
define get_parentdir =
PARENT_DIR := $(patsubst %/,%,$(dir $(PARENT_DIR)))
ifneq ($(wildcard $(PARENT_DIR)/build/scripts/config.mk),)
TOP = $(PARENT_DIR)
endif
endef
$(foreach dir_level,1 2 3 4 5 6 7,$(if $(TOP),,$(eval $(call get_parentdir,$(dir_level)))))
endif
endif

################################################################
# Control of parallel jobs                                     #
################################################################
ifeq ($(NO_PARALLEL_BUILD), 1)
MAKE_JOBS   =
else
CPU_NUM     = $(shell grep -c processor /proc/cpuinfo)
JOB_LIMIT   = $(shell echo $$(($(CPU_NUM) * 5 / 4)))
LOAD_LIMIT  = $(CPU_NUM)
MAKE_JOBS   = -j $(JOB_LIMIT) -l $(LOAD_LIMIT)
endif


#convert dos seperate char
TOP := $(patsubst %/,%,$(subst \,/,$(TOP)))
#use TOP disk char replace CURDIR disk char
CURRENT_DIR = $(subst $(firstword $(subst /, ,$(CURDIR))),$(firstword $(subst /, ,$(TOP))),$(CURDIR))

CUST_PRO_DIR := custom/project
CUST_BRD_DIR := custom/platform

PROJECT_MK := $(TOP)/out/PROJECT.mk
sinclude $(PROJECT_MK)

ifeq ($(ISPBIN),1)
SYS_PROJECT = ISP
endif

include $(TOP)/build/scripts/config_tls.mk

GCC_CMD := $(shell which $(CC) &> /dev/null; echo $$?)
ifneq ("$(GCC_CMD)","0")
$(error "Please set environment variable")
endif
GCC_LIB := $(shell $(CC) -print-libgcc-file-name -m728)
GCC_KERNEL_LIB := $(shell dirname `$(CC) -print-libgcc-file-name -m728`)/libkernel_.a

ifeq ($(wildcard $(SDK_CFG)),)
$(error $(SDK_CFG) not found.)
else
include $(SDK_CFG)
endif

ifneq ($(BOARD_CFG),)
ifeq ($(wildcard $(BOARD_CFG)),)
$(error $(BOARD_CFG) not found.)
else
include $(BOARD_CFG)
endif
endif

ifneq ($(PROJECT_CFG),)
PROJECT_DIR := $(dir $(PROJECT_CFG))
ifeq ($(wildcard $(PROJECT_CFG)),)
$(error $(PROJECT_CFG) not found.)
else
include $(PROJECT_CFG)
endif
endif

ISP_BIN_NAME	?= ispboot_auto.bin

#$(warning @@@@@@@@OUT_SDK_DIR:$(OUT_SDK_DIR))
LIB_DIR                 = $(PATH_PREFIX)/lib
LIBRARY_DIRS            = -L$(LIB_DIR) -L$(OUT_SDK_LIB_DIR)
BIN_DIR                 = $(OUT_BIN_DIR)
PACK_DIR                = $(OUT_BIN_PACK_DIR)
LINK_NAME               = romL
LINK_TARGET             = $(BIN_DIR)/$(LINK_NAME).obj

SDK_INCLUDE_DIR        += -I$(dir $(PRJ_CFG_H)) -I$(TOP) -I./include -I$(SRC_INC_DIR) -I$(SRC_INC_DIR)/common -I$(SRC_INC_DIR)/module -I$(SRC_INC_DIR)/hardware \
	-I$(TOP)/src/include/driver -I$(TOP)/src/include/freertos -I$(TOP)/src/include/fsi -I$(TOP)/src/include/posix -I$(TOP)/src/include/qp -I$(TOP)/src/include/libc
ifeq ($(BUILT4ISP2),1)
SDK_INCLUDE_DIR        += -I$(CUSTOM_INC_DIR)
endif

CUSTOM_INCLUDE_DIR     += -I$(dir $(PRJ_CFG_H)) -I$(TOP) -I./include -I$(PROJECT_DIR)/include -I$(CUSTOM_INC_DIR) -I$(OUT_SDK_INC_DIR) -I$(OUT_SDK_INC_DIR)/common \
	-I$(OUT_SDK_INC_DIR)/module -I$(OUT_SDK_INC_DIR)/hardware -I$(OUT_SDK_INC_DIR)/libc -I$(OUT_SDK_INC_DIR)/cfg_SDK

LD_INCLUDE_DIR         += -I$(dir $(PRJ_CFG_H)) -I$(OUT_SDK_INC_DIR)/common -I$(OUT_SDK_INC_DIR)/module -I$(OUT_SDK_INC_DIR)/hardware -I$(TOP)/build/scripts -I$(TOP)/out/bin -I$(OUT_BIN_DIR) -I$(OUT_SDK_INC_DIR)/cfg_SDK

ISP_BIN_DIR				= $(BIN_DIR)
ISP_LINK_TARGET			= $(ISP_BIN_DIR)/$(LINK_NAME).obj
PREBUILD_BIN_DIR		= $(TOP)/prebuilt/bin
PREBUILT_LIB_DIR		= $(TOP)/prebuilt/lib

PATH_NORMAL_BOOT		:= $(TOP)/src/boot/normal_boot
PATH_ISP_BOOT			:= $(TOP)/src/boot/isp_boot
PATH_XBOOT				:= $(TOP)/src/boot/xboot
PATH_DRAM_INIT			:= $(TOP)/src/boot/dram_init
INPUT_DRAM_INIT_SCAN	:= $(PATH_DRAM_INIT)/bin/dramscan.bin

ifneq ($(ISPBIN),1)
MODULE_LIST = base \
			iop0 iop1 iop2 fs \
			aud ap ap_demux fl fl_id3 \
			bt net airplay dlna \
			cli user other
else
MODULE_LIST = base isp ispupgrade
endif
REALMODULE_LIST := $(filter-out ,$(strip $(MODULE_LIST)))
FIRST_LINK_LIB := base:rom:libboot.a base:ram:libkernel.a

BUILD_TARGET := include $(TOP)/build/scripts/target.mk

MAKE_OTHER_TARGET = @+for other_target in $(OTHER_TARGET_MAKEFILE); do \
	if test -f $$other_target; then \
		$(MAKE) -s -f $$other_target $@ TOP=$(TOP); \
	fi \
	done

define gen_targetdir =
$(1):
	@+if test -d $(1); then \
		if ( \
			if test -f $(1)/Makefile; then \
				$(MAKE) -C $(1) -s TOP=$(TOP) ISPBIN=$(ISPBIN) BUILT4ISP2=$(BUILT4ISP2) OBJ_DEBUG=$(OBJ_DEBUG) SDK_RELEASE=$(SDK_RELEASE) LOG_SYSTEM_DISABLED=$(LOG_SYSTEM_DISABLED); \
			else \
				$(ECHO) "$(1) Have no Makefile"; false;\
			fi ); then \
			true; \
		else \
			exit 1; \
		fi; \
	fi
endef

MAKE_SUBDIRS = for dir in $(DIRS); do \
	if test -d $$dir; then \
		if ( \
			if test -f $$dir/Makefile; then \
				$(ECHO) "cd $$dir: make $@";\
				$(MAKE) -C $$dir -s $@ TOP=$(TOP) ISPBIN=$(ISPBIN) BUILT4ISP2=$(BUILT4ISP2) OBJ_DEBUG=$(OBJ_DEBUG) SDK_RELEASE=$(SDK_RELEASE) LOG_SYSTEM_DISABLED=$(LOG_SYSTEM_DISABLED); \
			else \
				$(ECHO) "$$dir Have no Makefile"; false;\
			fi ); then \
			true; \
		else \
			exit 1; \
		fi; \
	fi \
done

