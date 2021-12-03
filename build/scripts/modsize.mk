sinclude $(TOP)/build/scripts/config.mk

MODSZ_MK := $(TOP)/build/scripts/modsize.mk
ifneq ($(wildcard $(BIN_DIR)/$(LINK_NAME).sz),)
define get_mod_size =
$(1)_size:
	let "compute_size=$(strip $(addsuffix +,$(shell awk '{if (/^\.[_A-Za-z]*_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_rom_size=$(strip $(addsuffix +,$(shell awk '{if (/^\.rom_drv_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_ram_size=$$$$compute_size-$$$$compute_rom_size";\
	let "compute_text_size=$(strip $(addsuffix +,$(shell awk '{if (/^\.drv_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_data_size=$(strip $(addsuffix +,$(shell awk '{if (/^\._dat_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_sdata_size=$(strip $(addsuffix +,$(shell awk '{if (/^\._sda_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_sbss_size=$(strip $(addsuffix +,$(shell awk '{if (/^\._sbs_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_bss_size=$(strip $(addsuffix +,$(shell awk '{if (/^\._com_$(1)/) {print $$2}}' $(BIN_DIR)/$(LINK_NAME).sz))0)";\
	let "compute_alldata_size=$$$$compute_data_size+$$$$compute_sdata_size+$$$$compute_sbss_size";\
	$(ECHO) "$(1): $$$$compute_size ( ram: $$$$compute_ram_size rom: $$$$compute_rom_size ) ** ram detail: text/rodata:$$$$compute_text_size data:$$$$compute_alldata_size bss/COMMON:$$$$compute_bss_size **" >>$(OUT_CODESIZE);
endef
$(foreach mod,$(MODULE_LIST),$(eval $(call get_mod_size,$(mod))))
endif

codesize:
	@$(RM) -f $(OUT_CODESIZE)
	@$(SIZE) -A --radix=10 $(LINK_TARGET) | sed 's/.drv_aud_cfg/.rom_drv_aud_cfg/' > $(BIN_DIR)/$(LINK_NAME).sz
	@$(MAKE) -s -f $(MODSZ_MK) compute_module_size TOP=$(TOP)

compute_module_size:$(foreach mod,$(MODULE_LIST),$(mod)_size)
	@$(ECHO) "----------------------------------------"
	@$(ECHO) "----------all module libraries----------"
	@$(CAT) $(ALL_MODULE_LIBS)
	@$(ECHO) "----------------------------------------"
	@$(ECHO) "------module section size(bytes)--------"
	@$(AWK) '!($$2 == 0){totalram+=$$5;totalrom+=$$7;print $$0}END{totalsize=totalram+totalrom;print "------------total code size:------------";\
	print "   --total="totalsize,"(ram="totalram ,"rom="totalrom")--"}' $(OUT_CODESIZE)
	@$(ECHO) "----------------------------------------"
