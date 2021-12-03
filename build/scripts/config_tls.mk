#----------------------------------------------------------/
#Usage: include by Makefile.am
#Description: define Makefile tool variables
#----------------------------------------------------------/

#----------------------------------------------------------/
# GUI Definition
#----------------------------------------------------------/
OUT_ROOT                = $(TOP)/out
OUT_DIR                 = $(OUT_ROOT)/$(BOARD_NAME)/$(SYS_PROJECT)
OUT_ISP_DIR             = $(OUT_ROOT)/$(BOARD_NAME)/ISP/bin
OUT_SDK_DIR             = $(OUT_DIR)/sdk
OUT_SDK_LIB_DIR         = $(OUT_SDK_DIR)/lib
OUT_SDK_INC_DIR         = $(OUT_SDK_DIR)/include
OUT_CUSTOM_LIB_DIR      = $(OUT_DIR)/custom/lib
OUT_BIN_DIR             = $(OUT_DIR)/bin
OUT_BIN_PACK_DIR        = $(OUT_BIN_DIR)/pack
OUT_OBJ_DIR             = $(OUT_DIR)/obj
MODULE_LDP              = $(OUT_BIN_DIR)/module.ldp
SDK_MODULE_LIBS         = $(OUT_SDK_LIB_DIR)/module_libs
CUSTOM_MODULE_LIBS      = $(OUT_CUSTOM_LIB_DIR)/module_libs
ALL_MODULE_LIBS         = $(OUT_BIN_DIR)/module_libs
OUT_CODESIZE            = $(OUT_BIN_DIR)/codesize
PREBUILT_DIR            = $(TOP)/prebuilt
PREBUILT_LIB_DIR        = $(PREBUILT_DIR)/lib
PREBUILT_LIBDSP_DIR     = $(PREBUILT_DIR)/libdsp
PREBUILT_INCLUDE_DIR    = $(PREBUILT_DIR)/include
PREBUILT_MODLIBS        = $(PREBUILT_LIB_DIR)/module_libs
PREBUILT_DSP_MODLIBS    = $(PREBUILT_LIBDSP_DIR)/module_libs
ALL_BIN_OBJ             = $(OUT_CUSTOM_LIB_DIR)/all_bin_objs
INCLUDEROOT             = $(TOP)/src/include
SRC_INC_DIR             = $(TOP)/src/include
COMMON_DIR              = $(TOP)/custom/common
CUSTOM_INC_DIR          = $(PROJECT_DIR)/include
SDK_COMMAND_PREFIX      = mipsel-elf-
PATH_PREFIX             = $(TOP)/build
PINMUX_BIN              = $(OUT_BIN_DIR)/pinmux.bin
PINMUX_BIN_H            = $(OUT_BIN_DIR)/pinmux_bin.h
SDK_CFG                 = $(TOP)/build/config/sdk.cfg
SDK_CFG_H               = $(OUT_DIR)/config/sdk_cfg.h
PRJ_CFG_H               = $(OUT_DIR)/config/project_cfg.h
ifeq ($(OS),Windows_NT)
TOOL_PATH               = $(PATH_PREFIX)/tools/app-win
else
TOOL_PATH               = $(PATH_PREFIX)/tools/app-linux
endif

ECHO                    = echo -e
CC                      = $(SDK_COMMAND_PREFIX)gcc
CCX                     = $(SDK_COMMAND_PREFIX)g++
AR                      = $(SDK_COMMAND_PREFIX)ar
OBJCOPY                 = $(SDK_COMMAND_PREFIX)objcopy
OBJDUMP                 = $(SDK_COMMAND_PREFIX)objdump
CPP                     = $(SDK_COMMAND_PREFIX)cpp -P -traditional
LDEXE                   = $(SDK_COMMAND_PREFIX)ld
SIZE                    = $(SDK_COMMAND_PREFIX)size

XCC                     = $(CC)
XAR                     = $(AR)
XLD                     = $(CC)
XCPP                    = $(CCX)
XOBJCOPY                = $(OBJCOPY)
XOBJDUMP                = $(OBJDUMP)

DOXYGEN                 = $(TOOL_PATH)/doxygen.exe
FGZIP2                  = $(TOOL_PATH)/fgzip2.exe
LZOYPACK                = $(TOOL_PATH)/lzoypack.exe
MODUAL                  = $(TOOL_PATH)/module.exe
XB2                     = $(TOOL_PATH)/xb2.exe
MCODE                   = $(TOOL_PATH)/mcode.exe
PINMUXCFG               = $(TOOL_PATH)/pinmuxcfg.exe
ADDHEADERINFO           = $(TOOL_PATH)/addhdr2bin.exe
MODIFYUPGRADEFLAG       = $(TOOL_PATH)/ModifyUpgradeFlag.exe
GEN_PARTMAP             = $(TOOL_PATH)/gen_partmap.exe
XMLPACK                 = $(TOOL_PATH)/xml_pack.exe
ISP2                    = $(TOOL_PATH)/isp2.exe
PACK_PARTMAP            = $(TOOL_PATH)/pack_partmap.sh
SHOW_SECTION            = $(TOOL_PATH)/showsection.sh
APP_INFO                = $(TOOL_PATH)/app_info.sh


DD                      = dd
CP                      = cp
MV                      = mv
RM                      = rm
CAT                     = cat
MKDIR                   = mkdir
AWK                     = awk
SED                     = sed
HEXDUMP                 = hexdump
XXD                     = xxd
RSYNC                   = rsync

