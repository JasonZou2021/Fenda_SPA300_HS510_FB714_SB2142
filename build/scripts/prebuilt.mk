sinclude $(TOP)/build/scripts/config.mk

define ar_x_objs =
$(1):$(2)
	@$(ECHO) extract objs of $(notdir $(2)) to $(1)
	@$(MKDIR) -p $(1)
	@$(RM) -f $(1)/*.o
	@cd $(1); $(XAR) -x $(2); if [ -e _popcount_tab ]; then mv _popcount_tab _popcount_tab.o; fi
endef
$(foreach lib, $(wildcard $(GCC_LIB)) $(wildcard $(GCC_KERNEL_LIB)) $(wildcard $(PREBUILT_LIB_DIR)/lib*.a) $(wildcard $(PREBUILT_LIBDSP_DIR)/lib*.a),$(eval $(call ar_x_objs,$(subst $(TOP)/,,$(OUT_OBJ_DIR)/$(basename $(notdir $(lib)))),$(lib))))

extract_prebuilt: $(foreach lib, $(wildcard $(GCC_LIB)) $(wildcard $(GCC_KERNEL_LIB)) $(wildcard $(PREBUILT_LIB_DIR)/lib*.a) $(wildcard $(PREBUILT_LIBDSP_DIR)/lib*.a),$(subst $(TOP)/,,$(OUT_OBJ_DIR)/$(basename $(notdir $(lib)))))

define cp_prebuilt_h =
$(1):$(2) $(if $(wildcard $(subst /$(notdir $(1)),,$(1))),,$(subst /$(notdir $(1)),,$(1)))
	@$(ECHO) cp $(notdir $(2)) from $(subst $(TOP),,$(dir $(2))) to $(subst $(TOP),,$(dir $(1)))
	@$(CP) -f $(2) $(1)
endef
PREBUILT_H_FILES := $(wildcard $(PREBUILT_INCLUDE_DIR)/*/*.h) $(wildcard $(PREBUILT_INCLUDE_DIR)/*/*/*.h) $(wildcard $(PREBUILT_INCLUDE_DIR)/module/*.i) $(wildcard $(PREBUILT_INCLUDE_DIR)/module/*.inc)
PREBUILT_H_DIRS := $(sort $(patsubst %/, %, $(patsubst $(PREBUILT_INCLUDE_DIR)/%, %, $(dir $(PREBUILT_H_FILES)))))
$(foreach doth, $(PREBUILT_H_FILES)\
	,$(eval $(call cp_prebuilt_h,$(subst $(PREBUILT_INCLUDE_DIR),$(OUT_SDK_INC_DIR),$(doth)),$(doth))))

extract_prebuilt: $(foreach doth, $(wildcard $(PREBUILT_INCLUDE_DIR)/*/*.h) $(wildcard $(PREBUILT_INCLUDE_DIR)/*/*/*.h) $(wildcard $(PREBUILT_INCLUDE_DIR)/module/*.i) $(wildcard $(PREBUILT_INCLUDE_DIR)/module/*.inc)\
	,$(subst $(PREBUILT_INCLUDE_DIR),$(OUT_SDK_INC_DIR),$(doth)))

define mkdir_prebuilt_h =
$(1):
	$(MKDIR) -p $(1);
endef
$(foreach hdir,$(PREBUILT_H_DIRS),$(eval $(call mkdir_prebuilt_h,$(addprefix $(OUT_SDK_INC_DIR)/,$(hdir)))))
