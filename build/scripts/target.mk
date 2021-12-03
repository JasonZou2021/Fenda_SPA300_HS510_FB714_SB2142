#----------------------------------------------------------/
#Usage: include by other Makefile in subfolders.
#Description: define Makefile options for c/cpp/assembly.
#----------------------------------------------------------/

.SUFFIXES:
.PHONY: all clean $(DIRS)
.DEFAULT_GOAL := all

ifeq ($(CONFIG_MK_TAG),)
$(error "Makefile must set include $(TOP)/build/scripts/config.mk at first line")
endif

CFLAGS_MACHINE    = -m728 -membedded-data

#ifeq ($(TARGET)$(DIRS),)
#$(error Error:none <TARGET|DIRS> set, must set one for build target or sub-dirs.)
#endif

$(foreach dir, $(DIRS), $(eval $(call gen_targetdir,$(dir))))

#make sub-dirs
ifeq ($(TARGET),)

all: $(DIRS)

clean:
	$(MAKE_SUBDIRS)

else
#build target and sub-dirs

ifneq ($(TARGET_MODULE), )
ifneq ($(filter audio,$(TARGET_MODULE)),)
TARGET_MODULE:=aud
$(warning module change: audio replace with aud now!)
endif
MODULE = $(TARGET_MODULE)
else
$(error must set TARGET_MODULE in Makefile)
endif
ifeq ($(TARGET_STORAGE), )
TARGET_STORAGE = ram
endif
ifeq ($(filter $(MODULE), $(MODULE_LIST)), )
$(error MODULE $(MODULE) must be one of <$(MODULE_LIST)>)
endif

ifeq ($(TARGET),)
$(error TARGET not set in Makefile)
endif

ifeq ($(SOURCES)_$(TESTSOURCES),_)
$(error SOURCES or TESTSOURCES not set in Makefile)
endif

TARGET_TYPE_LIST = sdk test custom
ifneq ($(TARGET_TYPE),)
ifeq ($(findstring $(TARGET_TYPE),$(TARGET_TYPE_LIST)),)
$(error $(TARGET) error: TARGET_TYPE must be one of <$(TARGET_TYPE_LIST)> )
endif
else
#set TARGET_TYPE use path name when not defined
TARGET_TYPE = $(firstword $(subst /, ,$(patsubst $(TOP)/%,%,$(CURRENT_DIR))))
ifeq ($(TARGET_TYPE),src)
TARGET_TYPE := sdk
ifeq ($(word 2,$(subst /, ,$(patsubst $(TOP)/%,%,$(CURRENT_DIR)))),test)
TARGET_TYPE := test
endif
endif
endif
ifeq ($(TARGET_TYPE),sdk)
INCLUDE_DIR += $(SDK_INCLUDE_DIR)
MODULE_LIBS = $(SDK_MODULE_LIBS)
ifeq ($(notdir $(TARGET)),$(TARGET))
TARGET := $(OUT_SDK_LIB_DIR)/$(TARGET)
endif
else
ifeq ($(TARGET_TYPE),test)
INCLUDE_DIR += $(SDK_INCLUDE_DIR)
MODULE_LIBS = $(CUSTOM_MODULE_LIBS)
TARGET := $(OUT_CUSTOM_LIB_DIR)/$(notdir $(TARGET))
else
INCLUDE_DIR += $(CUSTOM_INCLUDE_DIR)
INCLUDE_DIR := $(filter-out -I$(INCLUDEROOT)/%, $(INCLUDE_DIR))
#$(info INCLUDE_DIR:$(INCLUDE_DIR))
MODULE_LIBS = $(CUSTOM_MODULE_LIBS)
TARGET := $(OUT_CUSTOM_LIB_DIR)/$(notdir $(TARGET))
endif
endif
#$(info TOP:$(TOP))
#$(info TARGET_TYPE:$(TARGET_TYPE) MODULE:$(MODULE) TARGET:$(notdir $(TARGET)))

OBJ_PATH := $(OUT_OBJ_DIR)/$(basename $(notdir $(TARGET)))
OBJ_DEBUG_PATH := $(OUT_DIR)/obj_debug/$(basename $(notdir $(TARGET)))

CFLAGS_WAR          = -W -Wall
#Disable log system and not use Werror to be avoid some variables on use in LOG
ifeq ($(LOG_SYSTEM_DISABLED),1)
CFLAGS_COMPILE     += -DLOG_SYSTEM_DISABLED
else
ifneq ($(BUILT4ISP2),1)
CFLAGS_WAR         += -Werror
else
CFLAGS_WAR         += -G0
endif
endif

CFLAGS_SUPPORT     += -D__EMU__ -DFINALSYS -D__FREERTOS__
CFLAGS_COMPILE     += $(CFLAGS_SUPPORT) $(CFLAGS_WAR)

CFLAGS_COMPILE     += -DFLOAT_PRINT

ifneq ($(ISPBIN), )
$(info ISP DEFINED:$(ISPBIN))
CFLAGS_COMPILE     += -DISPBIN
endif

ifeq ($(BUILT4ISP2),1)
$(info BUILT4ISP2=1)
CFLAGS_COMPILE     += -DBUILT4ISP2
endif

#BJ 2019.12.12 Merge new as below
ifeq ($(SPI_NOR_FLASH), )
CFLAGS_COMPILE     += -DSPI_NOR_FLASH
endif
#BJ 2019.12.12 Merge end

# 2020.09.02 for romter not write
ifneq ($(ROMTER), )
CFLAGS_COMPILE     += -DROMTER
endif
# 2020.09.02 for romter not write

ifeq ($(SYS_ZEBU_ENV), 1)
CFLAGS_COMPILE     += -DSYS_ZEBU_ENV=1
endif

ifeq ($(SYS_ZEBU_ZMEM), 1)
CFLAGS_COMPILE     += -DSYS_ZEBU_ZMEM=1
endif
CFLAGS_COMPILE     += -DSYS_NOR_BIN_COMPRESS=$(SYS_NOR_BIN_COMPRESS)

ifeq ($(CFG_DISABLE_BOOT_STANDBY_MODE), 1)
CFLAGS_COMPILE     += -DCFG_DISABLE_BOOT_STANDBY_MODE
endif

ifeq ($(SDK_RELEASE),1)
CFLAGS_COMPILE     += -DSDK_RELEASE
endif

CFLAGS_ENDIAN       = -DLITTLE_ENDIAN
CFLAGS_COPT         = -g -Os -fdata-sections -ffunction-sections -funsigned-char
CFLAGS_C            = $(CFLAGS_COPT) $(CFLAGS_MACHINE) $(CFLAGS_ENDIAN) $(SUB_MACRO) #--verbose
CFLAGS_S            = -g1 -O2 $(CFLAGS_MACHINE) $(CFLAGS_ENDIAN)

CFLAGS_MIPS_S       = $(CFLAGS_S) $(CFLAGS_COMPILE) $(INCLUDE_DIR)
CFLAGS_MIPS_C       = $(CFLAGS_C) $(CFLAGS_COMPILE) $(INCLUDE_DIR) -fno-builtin-printf -I$(TOP)/src/include/libc -I$(TOP)/src/include/libm
CFLAGS_MIPS_CPP     = $(CFLAGS_C) $(CFLAGS_COMPILE) $(INCLUDE_DIR)

ifeq ($(SYS_QSPY), 1)
CFLAGS_MIPS_C       += -DQ_SPY
endif

ifeq ($(USING_TESTTING), 1)
CFLAGS_MIPS_C       += -DUSING_TESTTING
endif

ifeq ($(UART0_BUFFER), 1)
ifneq ($(BUILT4ISP2),1)		# Force to disable UART0_BUFFER if BUILT4ISP2=1
CFLAGS_MIPS_C       += -DUART0_BUFFER
endif
endif


ifeq ($(SUPPORT_FILELIST), 1)
CFLAGS_MIPS_C       += -DSUPPORT_FILELIST
#(ECHO) "@CFLAGS_SUPPORT $$CFLAGS_SUPPORT"
endif

ifeq ($(SUPPORT_ID3_GET), 1)
CFLAGS_MIPS_C       += -DSUPPORT_ID3_GET
#(ECHO) "@CFLAGS_SUPPORT $$CFLAGS_SUPPORT"
endif

ifeq ($(SUPPORT_FLODER), 1)
CFLAGS_MIPS_C       += -DSUPPORT_FLODER
#(ECHO) "@CFLAGS_SUPPORT $$CFLAGS_SUPPORT"
endif

ifeq ($(SUPPORT_RESUME), 1)
CFLAGS_MIPS_C       += -DSUPPORT_RESUME
#(ECHO) "@CFLAGS_SUPPORT $$CFLAGS_SUPPORT"
endif

ifeq ($(WM_ON_BOARD), 1)
CFLAGS_MIPS_C       += -DWM_ON_BOARD
endif

SOURCE_FOR_S        = $(foreach s_source_file, $(SOURCES),$(filter %.S, $(s_source_file)))
SOURCE_FOR_C        = $(foreach c_source_file, $(SOURCES),$(filter %.c, $(c_source_file)))
SOURCE_FOR_CPP      = $(foreach cpp_source_file, $(SOURCES),$(filter %.cpp, $(cpp_source_file)))

OBJECT_S            = $(addprefix $(OBJ_PATH)/,$(SOURCE_FOR_S:.S=.o))
OBJECT_C            = $(addprefix $(OBJ_PATH)/,$(SOURCE_FOR_C:.c=.o))
OBJECT_CPP          = $(addprefix $(OBJ_PATH)/,$(SOURCE_FOR_CPP:.cpp=.o))
ALL_OBJECT			= $(OBJECT_S) $(OBJECT_C) $(OBJECT_CPP)
#ALL_OBJECT_NOTDIR	= $(patsubst $(OBJ_PATH)/%, %, $(OBJECT_S) $(OBJECT_C) $(OBJECT_CPP))

LIBNAME = $(subst lib,,$(basename $(notdir $(TARGET))))
CPDIRS = $(patsubst %/, %, $(dir $(RELEASE_API_H)))

#RELEASE_DIRS = $(shell find $(RELEASE_PATHS) -maxdepth 3 -type d)
RELEASE_PATHS = $(foreach dir,$(RELEASE_API_H),$(INCLUDEROOT)/$(dir))
RELEASE_DIRS = $(filter-out %.i %.inc %.h, $(RELEASE_PATHS))
RELEASE_FILES = $(filter %.i %.inc %.h, $(RELEASE_PATHS))
RELEASE_DIRS_FILES = $(foreach dir,$(RELEASE_DIRS),$(wildcard $(dir)/*.h $(dir)/*.inc $(dir)/*.i $(dir)/*/*.h $(dir)/*/*.inc $(dir)/*/*.i $(dir)/*/*/*.h $(dir)/*/*/*.inc $(dir)/*/*/*.i))
RELEASE_FILE_NAMES = $(patsubst $(INCLUDEROOT)/%, %, $(RELEASE_FILES) $(RELEASE_DIRS_FILES))
RELEASE_FILE_PATHS = $(sort $(patsubst %/, %, $(patsubst $(INCLUDEROOT)/%, %, $(dir $(RELEASE_FILES)) $(dir $(RELEASE_DIRS_FILES)))))

#$(warning @@@@@RELEASE_FILE_NAMES :$(RELEASE_FILE_NAMES))
#$(warning @@@@@RELEASE_FILE_PATHS :$(RELEASE_FILE_PATHS))

# -- Common Message, please don't remove. --
MSG_SPLIT_LINE =
MSG_COMPILING  = @$(ECHO) "  CC <$<>"
MSG_ARCHIVE    = @$(ECHO) "  AR <$(subst $(TOP)/,,$@)>"
MSG_LINKING    = @$(ECHO) "  LD <$@>"
MSG_CLEAN      = @$(ECHO) "  CLEAN: target and it's immediate files cleaned: $(TARGET)."
MSG_GOAL_OK    = @$(ECHO) "***" $(TARGET) "is built successfully! ***"

DEPENDCY_OBJ = $(OBJECT_S) $(OBJECT_C) $(OBJECT_CPP)
AR_GENERATE = @$(XAR) -cr $@ $(DEPENDCY_OBJ)
GENERATE_MODULE_LIBS = YES

#collect module/libs info
define update_module_libs
$(shell echo $(MODULE):$(TARGET_STORAGE):$(notdir $(TARGET)) >> $(MODULE_LIBS))
endef
ifeq ($(wildcard $(OBJ_PATH)),)
$(shell $(MKDIR) -p $(OBJ_PATH))
endif
ifeq ($(OBJ_DEBUG),1)
ifeq ($(wildcard $(OBJ_DEBUG_PATH)),)
$(shell $(MKDIR) -p $(OBJ_DEBUG_PATH))
endif
endif

all: $(TARGET) $(DIRS) collect
	$(MAKE) cp_all_header
ifneq ($(OTHER_TARGET_MAKEFILE),)
	$(MAKE_OTHER_TARGET)
endif

$(TARGET): $(PRJ_CFG_H) $(SDK_CFG_H) $(DEPENDCY_OBJ) $(HOOK_TARGET)
	$(MSG_SPLIT_LINE)
ifeq ($(BUILD_VERBOSE),0)
	$(MSG_ARCHIVE)
else
	@$(ECHO) $(AR_GENERATE)
endif
	$(AR_GENERATE)

$(SDK_CFG_H): $(SDK_CFG) $(OUT_DIR)/config
	@$(AWK) '$$2 == "="{print $$0}' $(SDK_CFG) | $(AWK) 'NF == 3 {print "#define", $$1, $$3}' > $(SDK_CFG_H)
	#@$(ECHO) "=======================================";
	#@$(ECHO) generate $(subst $(TOP)/,,$(SDK_CFG_H)) from $(subst $(TOP)/,,$(SDK_CFG)),
	#@$(ECHO) "****config:"
	#@$(CAT) $(SDK_CFG_H)
	#@$(ECHO) "=======================================";

$(TARGET): $(PRJ_CFG_H)

$(OBJ_PATH)/%.o: %.S
	$(MSG_SPLIT_LINE)
ifeq ($(BUILD_VERBOSE),0)
	$(MSG_COMPILING)
else
	@$(ECHO) $(XCC) $(CFLAGS_MIPS_S) -c -o $@ $<
endif
	@$(XCC) $(CFLAGS_MIPS_S) -MM -MT $@ -o $(@:.o=.d) $<
	@$(XCC) $(CFLAGS_MIPS_S) -MM -MT $@ -o $(@:.o=.d) $<
	@$(XCC) $(CFLAGS_MIPS_S) -c -o $@ $<
ifeq ($(OBJ_DEBUG),1)
	@$(XCC) $(filter-out -fdata-sections -ffunction-sections,$(CFLAGS_MIPS_S)) -c -o $(subst $(OBJ_PATH),$(OBJ_DEBUG_PATH),$@) $<
endif

$(OBJ_PATH)/%.o: %.c
	$(MSG_SPLIT_LINE)
ifeq ($(BUILD_VERBOSE),0)
	$(MSG_COMPILING)
else
	@$(ECHO) $(XCC) $(CFLAGS_MIPS_C) -c -o $@ $<
endif
	@$(XCC) $(CFLAGS_MIPS_C) -MM -MT $@ -o $(@:.o=.d) $<
	@$(XCC) $(CFLAGS_MIPS_C) -MM -MT $@ -o $(@:.o=.d) $<
	@$(XCC) $(CFLAGS_MIPS_C) -c -o $@ $<
ifeq ($(OBJ_DEBUG),1)
	@$(XCC) $(filter-out -fdata-sections -ffunction-sections,$(CFLAGS_MIPS_C)) -c -o $(subst $(OBJ_PATH),$(OBJ_DEBUG_PATH),$@) $<
endif

$(OBJ_PATH)/%.o: %.cpp
	$(MSG_SPLIT_LINE)
ifeq ($(BUILD_VERBOSE),0)
	$(MSG_COMPILING)
else
	@$(ECHO) $(XCC) $(CFLAGS_MIPS_CPP) -c -o $@ $<
endif
	@$(XCC) $(CFLAGS_MIPS_CPP) -MM -MT $@ -o $(@:.o=.d) $<
	@$(XCC) $(CFLAGS_MIPS_CPP) -c -o $@ $<
ifeq ($(OBJ_DEBUG),1)
	@$(XCC) $(filter-out -fdata-sections -ffunction-sections,$(CFLAGS_MIPS_CPP)) -c -o $(subst $(OBJ_PATH),$(OBJ_DEBUG_PATH),$@) $<
endif


-include $(ALL_OBJECT:.o=.d)

clean:
ifeq ($(BUILD_VERBOSE),0)
	$(MSG_CLEAN)
else
	@$(ECHO) $(RM) -rf $(TARGET) $(OBJ_PATH) $(OBJ_DEBUG_PATH)
endif
	@$(RM) -rf $(TARGET) $(OBJ_PATH) $(OBJ_DEBUG_PATH)
ifneq ($(OTHER_TARGET_MAKEFILE),)
	$(MAKE_OTHER_TARGET)
endif
ifneq ($(DIRS),)
	$(MAKE_SUBDIRS)
endif

define cp_release_h =
$(1):$(2) $(if $(wildcard $(subst /$(notdir $(1)),,$(1))),,$(subst /$(notdir $(1)),,$(1)))
	@$(CP) -f $(2) $(1)
endef
$(foreach doth, $(RELEASE_FILE_NAMES),$(eval $(call cp_release_h,$(OUT_SDK_INC_DIR)/$(doth),$(INCLUDEROOT)/$(doth))))

cp_all_header: $(foreach doth, $(RELEASE_FILE_NAMES), $(addprefix $(OUT_SDK_INC_DIR)/,$(doth)))

define mkdir_release_h =
$(1):
	$(MKDIR) -p $(1);
endef
$(foreach hdir,$(RELEASE_FILE_PATHS),$(eval $(call mkdir_release_h,$(addprefix $(OUT_SDK_INC_DIR)/,$(hdir)))))


collect:
	$(call update_module_libs)

dis:
	@$(ECHO) "disassemble $(LINK_NAME).text"
	@$(XOBJDUMP) -d $(BIN_DIR)/$(LINK_TARGET) > $(BIN_DIR)/$(LINK_NAME).text


#reach make target end
endif
