#TOP = ../../../..
sinclude $(TOP)/build/scripts/config.mk

.PHONY: gen_module
gen_module:
	echo gen_module_ldp...

ifneq ($(wildcard $(SDK_MODULE_LIBS)),)
sdk_module_libs_src := $(shell cat $(SDK_MODULE_LIBS))
all_module_libs += $(sdk_module_libs_src)
endif
ifneq ($(wildcard $(CUSTOM_MODULE_LIBS)),)
custom_module_libs := $(shell cat $(CUSTOM_MODULE_LIBS))
all_module_libs += $(custom_module_libs)
endif
ifeq ($(ISPBIN),)
ifneq ($(wildcard $(PREBUILT_MODLIBS)),)
prebuilt_module_libs := $(shell sed -e '\:^\#:d' $(PREBUILT_MODLIBS)) $(shell sed -e '\:^\#:d' $(PREBUILT_DSP_MODLIBS))
all_module_libs += $(filter-out $(sdk_module_libs_src),$(prebuilt_module_libs))
endif
#sdk libs must include prebuilt libs
sdk_module_libs := $(filter-out $(shell cat $(CUSTOM_MODULE_LIBS) $(PREBUILT_DSP_MODLIBS)),$(all_module_libs))
all_module_libs := $(filter $(addprefix %:,$(PRODUCT_LIBS)),$(all_module_libs))
endif
#let some lib be the first
all_module_libs := $(FIRST_LINK_LIB) $(sort $(filter-out $(FIRST_LINK_LIB),$(all_module_libs)))

$(shell rm -f $(OUT_SDK_LIB_DIR)/sdk_module_libs)
$(foreach lib,$(sdk_module_libs),$(shell echo $(lib) >>$(OUT_SDK_LIB_DIR)/sdk_module_libs))

$(shell rm -f $(MODULE_LDP))
$(shell rm -f $(ALL_MODULE_LIBS))
$(shell rm -f $(ALL_BIN_OBJ))
$(foreach module, $(MODULE_LIST),\
	$(if $(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs))),\
	$(eval $(module)_ram_libs:=$(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs)))) \
	$(file >>$(ALL_MODULE_LIBS),$(module):ram: $(subst $(module):ram:,,$(filter $(module):ram:lib%.a,$(all_module_libs))))) \
	$(if $(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs))),\
	$(eval $(module)_rom_libs:=$(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs)))) \
	$(file >>$(ALL_MODULE_LIBS),$(module):rom: $(subst $(module):rom:,,$(filter $(module):rom:lib%.a,$(all_module_libs))))))
ifeq ($(ISPBIN),)
all_libs := $(filter-out $(basename $(GCC_LIB)) $(basename $(GCC_KERNEL_LIB)), $(filter lib%.a, $(shell cat $(ALL_MODULE_LIBS))))
all_libs_path := $(wildcard $(GCC_LIB)) $(wildcard $(GCC_KERNEL_LIB)) $(foreach lib,$(all_libs),$(firstword $(wildcard $(OUT_SDK_LIB_DIR)/$(lib)) $(wildcard $(OUT_CUSTOM_LIB_DIR)/$(lib)) \
	$(wildcard $(PREBUILT_LIB_DIR)/$(lib)) $(wildcard $(PREBUILT_LIBDSP_DIR)/$(lib))))
else
all_libs := $(basename $(GCC_KERNEL_LIB)), $(filter lib%.a, $(shell cat $(ALL_MODULE_LIBS)))
all_libs_path := $(wildcard $(GCC_KERNEL_LIB)) $(foreach lib,$(all_libs),$(firstword $(wildcard $(OUT_SDK_LIB_DIR)/$(lib)) $(wildcard $(OUT_CUSTOM_LIB_DIR)/$(lib)) \
	$(wildcard $(PREBUILT_LIB_DIR)/$(lib)) $(wildcard $(PREBUILT_LIBDSP_DIR)/$(lib))))
endif
ALL_MODULE_LIBS_PATH :=$(ALL_MODULE_LIBS).path
$(shell rm -f $(ALL_MODULE_LIBS_PATH))
$(file >>$(ALL_MODULE_LIBS_PATH),$(all_libs_path))
#generate lib objlist
$(foreach lib,$(all_libs_path),$(eval $(basename $(notdir $(lib)))_objs:=$(shell $(XAR) -t $(lib))))
$(file >>$(ALL_BIN_OBJ),$(foreach lib,$(all_libs_path),$(addprefix $(OUT_OBJ_DIR)/$(basename $(notdir $(lib)))/,$($(basename $(notdir $(lib)))_objs))))
$(shell sed -n 's#\([0-9a-zA-Z.:_/-]*.o\)#\nINPUT\(\1\)#gp' $(ALL_BIN_OBJ) >>$(MODULE_LDP))

ifeq ($(ISPBIN),)
ifneq ($(BUILT4ISP2),1)
#generate module.ldp
define gen_apinit_sec
	$(foreach init, driver ao ao_sync aoarg device cmd test idlehook edhook poweron fstable priomalloc, \
	$(shell echo "" >> $(MODULE_LDP)) \
	$(shell echo "		. = ALIGN(4);" >> $(MODULE_LDP)) \
	$(shell echo "		__init_$(init)_start = .;">> $(MODULE_LDP)) \
	$(shell echo "		KEEP(*(.__init.$(init)))">> $(MODULE_LDP)) \
	$(shell echo "		__init_$(init)_end = .;">> $(MODULE_LDP)) \
	)
endef

#$(info ---- gen mod objlist:$(1):$(2):$($(1)_$(2)_libs))
define gen_mod_objlist
$(foreach lib, $($(1)_$(2)_libs),$($(basename $(lib))_objs))
endef

$(foreach storage, ram rom,\
$(foreach module, $(MODULE_LIST),\
	$(if $($(module)_$(storage)_libs),\
	$(eval $(module)_$(storage)_objs:=$(call gen_mod_objlist,$(module),$(storage))))))

define gen_all_other
$(if $(filter other, $(1)),\
	$(if $(filter other:rom,$(1):$(2)),, \
		$(file >> $(MODULE_LDP),$(foreach __objsec,$(3), *($(__objsec)*)))\
	))
endef
#$(info ---- gen mod sec:$(1):$(2):$($(1)_$(2)_objs))
define gen_mod_sec
$(if $(filter .data,$(3)),$(if $(filter $(1):$(2),$(firstword $(REALMODULE_LIST)):ram),\
	$(shell echo "	asdData = .;" >> $(MODULE_LDP))))\
$(if $(filter .sbss,$(3)),$(if $(filter $(1):$(2),$(firstword $(REALMODULE_LIST)):ram),\
	$(shell echo "	asdBstart = .;" >> $(MODULE_LDP))))\
$(if $($(1)_$(2)_objs),$(file >> $(MODULE_LDP),\
	$(foreach __obj, $($(1)_$(2)_objs), \
	$(if $(filter $(__obj),$(KEEP_OBJS)),$(foreach __objsec,$(3), KEEP(*$(__obj)($(__objsec)*)) ), \
	$(foreach __objsec,$(3), *$(__obj)($(__objsec)*) ))))) \
$(call gen_all_other,$(1),$(2),$(3)) \
$(if $(filter .data,$(3)),$(if $(filter $(1):$(2),$(lastword $(REALMODULE_LIST)):rom),\
	$(shell echo "	_gp = .;" >> $(MODULE_LDP))))\
$(if $(filter .sdata,$(3)),$(if $(filter $(1):$(2),$(lastword $(REALMODULE_LIST)):rom),\
	$(shell echo "	asdEdata = .;" >> $(MODULE_LDP))))\
$(if $(filter .bss,$(3)),$(if $(filter $(1):$(2),$(lastword $(REALMODULE_LIST)):rom),\
	$(shell echo "	asdBend = .;" >> $(MODULE_LDP))))\
$(shell echo "	. = ALIGN(16);" >> $(MODULE_LDP))
endef

#arg1:module name
#arg2:storage,ram or rom
#arg3:memory section name,like rom or sdram_base,etc.
define gen_mod_sectext
	$(if $(filter ram,$(2)),$(shell echo "	.drv_$(1) :" >> $(MODULE_LDP)), \
	$(shell echo "	.rom_drv_$(1) :" >> $(MODULE_LDP))) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(if $(filter ram,$(2)),$(shell echo "	_text_drv_$(1) = .;" >> $(MODULE_LDP)),\
	$(shell echo "	_text_rom_$(1) = .;" >> $(MODULE_LDP)) ) \
	$(if $(shell awk '/$(1):$(2):/' $(ALL_MODULE_LIBS))$(filter $(1):$(2),other:ram), \
	$(call gen_mod_sec,$(1),$(2),.text .rodata .rdata) \
	$(if $(filter base:ram, $(1):$(2)), $(gen_apinit_sec) \
	$(shell echo "	. = ALIGN(16);" >> $(MODULE_LDP)))) \
	$(if $(filter ram,$(2)),$(shell echo "	_text_drv_$(1)_end = .;" >> $(MODULE_LDP)),\
	$(shell echo "	_text_rom_$(1)_end = .;" >> $(MODULE_LDP)) ) \
	$(shell echo "	} > $(3)" >> $(MODULE_LDP))
endef
define gen_mod_secdata
$(foreach module, $(REALMODULE_LIST), \
	$(shell echo "	._dat_$(module) :" >> $(MODULE_LDP)) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(call gen_mod_sec,$(module),ram,.data) \
	$(call gen_mod_sec,$(module),rom,.data) \
	$(shell echo "	} > sdram_data" >> $(MODULE_LDP)))
endef
define gen_mod_secsdata
$(foreach module, $(REALMODULE_LIST), \
	$(shell echo "	._sda_$(module) :" >> $(MODULE_LDP)) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(call gen_mod_sec,$(module),ram,.sdata .lit8 .lit4 .lita) \
	$(call gen_mod_sec,$(module),rom,.sdata .lit8 .lit4 .lita) \
	$(shell echo "	} > sdram_data" >> $(MODULE_LDP))) \
	$(shell echo "_data_size = $(foreach module, $(REALMODULE_LIST), \
	SIZEOF(._dat_$(module))+SIZEOF(._sda_$(module))+) 0;" >> $(MODULE_LDP))
endef

define gen_mod_romdata
	$(shell echo "	.rom_data :" >> $(MODULE_LDP)) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(shell echo "	asdDataps = .;" >> $(MODULE_LDP)) \
	$(shell echo "	. += _data_size;" >> $(MODULE_LDP)) \
	$(shell echo "	asdDatape = .;" >> $(MODULE_LDP)) \
	$(shell echo "		. = ALIGN(16);" >> $(MODULE_LDP)) \
	$(shell echo "	} > rom" >> $(MODULE_LDP))
endef

define gen_mod_secsbss
$(foreach module, $(REALMODULE_LIST), \
	$(shell echo "	._sbs_$(module) :" >> $(MODULE_LDP)) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(call gen_mod_sec,$(module),ram,.sbss .scommon) \
	$(call gen_mod_sec,$(module),rom,.sbss .scommon) \
	$(shell echo "	} > sdram_data" >> $(MODULE_LDP))) \
	$(shell echo "_sbs_size = $(foreach module, $(REALMODULE_LIST), \
	SIZEOF(._sbs_$(module))+) 0;" >> $(MODULE_LDP))
endef

define gen_mod_secom
$(foreach module, $(REALMODULE_LIST), \
	$(shell echo "	._com_$(module) :" >> $(MODULE_LDP)) \
	$(shell echo "	{" >> $(MODULE_LDP)) \
	$(call gen_mod_sec,$(module),ram,.bss COMMON) \
	$(call gen_mod_sec,$(module),rom,.bss COMMON) \
	$(shell echo "	} > sdram_data" >> $(MODULE_LDP))) \
	$(shell echo "_com_size = $(foreach module, $(REALMODULE_LIST), \
	SIZEOF(._com_$(module))+) 0;" >> $(MODULE_LDP))
endef

define gen_module_ldp
	$(shell echo "SECTIONS " >> $(MODULE_LDP)) \
	$(shell echo "{" >> $(MODULE_LDP)) \
	$(info generate rom text sections ...) \
$(foreach module, $(REALMODULE_LIST), \
	$(call gen_mod_sectext,$(module),rom,rom)) \
	$(info generate ram text sections ...) \
$(foreach module, $(REALMODULE_LIST), \
	$(call gen_mod_sectext,$(module),ram,sdram_$(module))) \
	$(info generate data sections ...) \
	$(call gen_mod_secdata) \
	$(info generate sdata sections ...) \
	$(call gen_mod_secsdata) \
	$(info generate rom data sections ...) \
	$(call gen_mod_romdata) \
	$(info generate sbss sections ...) \
	$(call gen_mod_secsbss) \
	$(info generate common sections ...) \
	$(call gen_mod_secom) \
	$(file >> $(MODULE_LDP), _etext = ADDR(.rom_drv_$(lastword $(REALMODULE_LIST))) + SIZEOF(.rom_drv_$(lastword $(REALMODULE_LIST)));) \
	$(shell echo "}" >> $(MODULE_LDP)) \
	$(shell echo "" >> $(MODULE_LDP)) \
	$(info done.)
endef

$(gen_module_ldp)

endif	# BUILT4ISP2
endif	# ISPBIN
