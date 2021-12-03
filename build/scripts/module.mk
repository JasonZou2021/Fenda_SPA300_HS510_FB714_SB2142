sinclude $(TOP)/build/scripts/config.mk

MODLDP_MK := $(TOP)/build/scripts/module_ldp.mk

ifneq ($(wildcard $(SDK_MODULE_LIBS)),)
sdk_module_libs := $(shell cat $(SDK_MODULE_LIBS))
all_module_libs += $(sdk_module_libs)
endif
ifneq ($(wildcard $(CUSTOM_MODULE_LIBS)),)
custom_module_libs := $(shell cat $(CUSTOM_MODULE_LIBS))
all_module_libs += $(custom_module_libs)
endif
ifeq ($(ISPBIN),)
ifneq ($(wildcard $(PREBUILT_MODLIBS)),)
prebuilt_module_libs := $(shell sed -e '\:^\#:d' $(PREBUILT_MODLIBS)) $(shell sed -e '\:^\#:d' $(PREBUILT_DSP_MODLIBS))
all_module_libs += $(filter-out $(sdk_module_libs),$(prebuilt_module_libs))
endif
all_module_libs := $(filter $(addprefix %:,$(PRODUCT_LIBS)),$(all_module_libs))
endif
#let some lib be the first
all_module_libs := $(FIRST_LINK_LIB) $(sort $(filter-out $(FIRST_LINK_LIB),$(all_module_libs)))


ALL_MODULE_LIBS_NEW :=$(ALL_MODULE_LIBS).new
ALL_MODULE_LIBS_PATH :=$(ALL_MODULE_LIBS).path
ALL_BIN_OBJ_NEW :=$(ALL_BIN_OBJ).new
$(shell rm -f $(ALL_MODULE_LIBS_NEW))
$(shell rm -f $(ALL_BIN_OBJ_NEW))
$(foreach module, $(MODULE_LIST),\
	$(if $(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs))),\
	$(eval $(module)_ram_libs:=$(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs)))) \
	$(file >>$(ALL_MODULE_LIBS_NEW),$(module):ram: $(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs))))) \
	$(if $(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs))),\
	$(eval $(module)_rom_libs:=$(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs)))) \
	$(file >>$(ALL_MODULE_LIBS_NEW),$(module):rom: $(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs))))))
all_libs := $(filter lib%.a, $(shell cat $(ALL_MODULE_LIBS_NEW)))
all_libs_path := $(foreach lib,$(all_libs),$(firstword $(wildcard $(OUT_SDK_LIB_DIR)/$(lib)) $(wildcard $(OUT_CUSTOM_LIB_DIR)/$(lib)) \
	$(wildcard $(PREBUILT_LIB_DIR)/$(lib)) $(wildcard $(PREBUILT_LIBDSP_DIR)/$(lib))))

#generate lib objlist
$(foreach lib,$(all_libs_path),$(eval $(basename $(notdir $(lib)))_objs:=$(shell $(XAR) -t $(lib))))
$(file >>$(ALL_BIN_OBJ_NEW),$(foreach lib,$(all_libs_path),$(addprefix $(OUT_OBJ_DIR)/$(basename $(notdir $(lib)))/,$($(basename $(notdir $(lib)))_objs))))

UPDATE_MODULE_LDP := 1
ifeq ($(shell cat $(ALL_MODULE_LIBS_NEW)),$(shell cat $(ALL_MODULE_LIBS) 2>/dev/null))
ifeq ($(shell cat $(ALL_BIN_OBJ_NEW)),$(shell cat $(ALL_BIN_OBJ)))
#$(info $(MODULE_LDP) not need update)
UPDATE_MODULE_LDP := 0
else
$(info $(subst $(TOP)/,,$(MODULE_LDP)) need update: add or delete objs)
endif
else
$(info $(subst $(TOP)/,,$(MODULE_LDP)) need update: add or delete libs)
endif
.PHONY: module
module:
ifeq ($(UPDATE_MODULE_LDP),0)
	@$(ECHO) gen module.ldp not need update.
else
	@$(ECHO) gen module.ldp due to add or delete libs/objs...
	@$(MAKE) -s -f $(MODLDP_MK) TOP=$(TOP)
endif

ifeq ($(all_libs_path),$(shell cat $(ALL_MODULE_LIBS_PATH) 2>/dev/null))
#$(warning all_libs_path equal to old one)
$(BIN_DIR)/rom.bin: $(all_libs_path)
	@$(ECHO) pack rom.bin due to libs update...
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk real_pack TOP=$(TOP)
else
#$(warning all_libs_path not equal to old one)
.PHONY: $(BIN_DIR)/rom.bin
$(BIN_DIR)/rom.bin:
	@$(ECHO) pack rom.bin due to add or delete libs...
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk real_pack TOP=$(TOP)
endif
