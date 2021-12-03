sinclude $(TOP)/build/scripts/config.mk

PACK_MK := $(TOP)/build/scripts/pack.mk
MODSZ_MK := $(TOP)/build/scripts/modsize.mk
MODULE_MK := $(TOP)/build/scripts/module.mk
EN_CODESIZE := 0
AB_BIN ?= 0

ISP_BIN_SIZE := $(shell echo "$$(( (($(CFG_SP_ROM_SIZE_ISP) + $(CFG_ROMBIN_MIN_BLOCKSIZE) - 1) & ~($(CFG_ROMBIN_MIN_BLOCKSIZE) - 1)) << 10 ))")

# partition map: get A/B bin offset
GEN_PARTMAP_VAR = " \
export $(shell $(SED) -e '\@^[[:blank:]]*\#@d;\@^$$@d; s@\#.*@@' $(PROJECT_CFG) | $(AWK) -F= '/^CFG_/{gsub(/ /,"",$$0); printf "%s=%s\n",$$1,$$2}') \
 $(shell $(SED) -e '\@^[[:blank:]]*\#@d;\@^$$@d;s@\#.*@@' $(BOARD_CFG) | $(AWK) -F= '/^CFG_/{gsub(/ /,"",$$0); printf "%s=%s\n",$$1,$$2}') \
 BIN_DIR=$(BIN_DIR) PACK_MAP_BIN=$(PACK_MAP_BIN) PREBUILD_BIN_DIR=$(PREBUILD_BIN_DIR) ISP_BIN_NAME=$(ISP_BIN_NAME) GEN_PARTMAP=$(GEN_PARTMAP) AB_BIN=$(AB_BIN) PACK_DIR=$(PACK_DIR) XB2=$(XB2) MODIFYUPGRADEFLAG=$(MODIFYUPGRADEFLAG)"
ifeq ("$(wildcard $(PACK_TBL))","")
$(error Pack Script file $(PACK_TBL) Not Found !)
endif
APP_BIN_OFFSET = $$(($(shell eval $(GEN_PARTMAP_VAR); "$(PACK_PARTMAP)" -i "$(PACK_TBL)" -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s $(ISP_BIN_SIZE) -g app.bin)))
APPB_BIN_OFFSET = $$(($(shell eval $(GEN_PARTMAP_VAR); "$(PACK_PARTMAP)" -i "$(PACK_TBL)" -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s $(ISP_BIN_SIZE) -g app_b.bin)))
MAX_BIN_SIZE = $$(($(shell eval $(GEN_PARTMAP_VAR); "$(PACK_PARTMAP)" -i "$(PACK_TBL)" -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s $(ISP_BIN_SIZE) -g sunplus_last.bin)))

PACK_MAP_BIN = $(BIN_DIR)/part_map.bin

CFLAGS_WAR          = -W -Wall
ifneq ($(BUILT4ISP2),1)
CFLAGS_WAR         += -Werror
else
#CFLAGS_WAR         += -G0
endif

CFLAGS_SUPPORT     += -D__EMU__ -DFINALSYS
CFLAGS_COMPILE      = $(CFLAGS_SUPPORT) $(CFLAGS_WAR)
CFLAGS_GEN_LD       = $(CFLAGS_COMPILE) $(INCLUDE_DIR) $(LD_INCLUDE_DIR) -x c++
ifeq ($(BUILT4ISP2),1)
CFLAGS_GEN_LD      += -DBUILT4ISP2
endif

# isp flag
ISP_LDSCRIPT        = $(ISP_BIN_DIR)/ispl.ld
ISP_LDFLAGS         = -nostartfiles -T $(ISP_LDSCRIPT) $(LIBRARY_DIRS)  \
                      --cref -Wl,--no-check-sections,--gc-sections,--sort-common,-Map,$(ISP_BIN_DIR)/$(LINK_NAME).map
ISP_RAM_MODELS      = $(filter-out base,$(MODULE_LIST))
ifeq ($(SYS_NOR_BIN_COMPRESS), 1)
ISP_MODELS          =  $(foreach module,$(ISP_RAM_MODELS), $(ISP_BIN_DIR)/$(module).bin.gz)
else ifeq ($(SYS_NOR_BIN_COMPRESS), 2)
ISP_MODELS          =  $(foreach module,$(ISP_RAM_MODELS), $(ISP_BIN_DIR)/$(module).bin.lzoy)
else
ISP_MODELS          =  $(foreach module,$(ISP_RAM_MODELS), $(ISP_BIN_DIR)/$(module).bin)
endif

ISP_BOOT_OBJECT     =
#end

#ap flag
LDSCRIPT            = $(BIN_DIR)/romL.ld
LDBASELIB           = -lgcc
LDWRAPPER           = -Wl,--wrap=toupper -Wl,--wrap=tolower -Wl,--wrap=isupper -Wl,--wrap=islower -Wl,--wrap=isdigit -Wl,--wrap=isspace -Wl,--wrap=isalpha \
					  -Wl,--wrap=malloc -Wl,--wrap=free -Wl,--wrap=printf  -Wl,--wrap=strlen -Wl,--wrap=sprintf -Wl,--wrap=snprintf -Wl,--wrap=vsprintf -Wl,--wrap=vsnprintf \
					  -Wl,--wrap=strcasecmp -Wl,--wrap=strncasecmp -Wl,--wrap=strdup -Wl,--wrap=strerror -Wl,--wrap=memccpy -Wl,--wrap=strlcpy  -Wl,--wrap=sleep \
					  -Wl,--wrap=usleep -Wl,--wrap=calloc -Wl,--wrap=realloc -Wl,--wrap=sscanf -Wl,--wrap=strtod -Wl,--wrap=strtof -Wl,--wrap=strtoul -Wl,--wrap=strlwr \
					  -Wl,--wrap=strupr -Wl,--wrap=strnlen -Wl,--wrap=qsort -Wl,--wrap=memset -Wl,--wrap=memcpy -Wl,--wrap=memcmp -Wl,--wrap=strcpy -Wl,--wrap=strchr \
					  -Wl,--wrap=strrchr -Wl,--wrap=strcmp -Wl,--wrap=strncmp -Wl,--wrap=strcat -Wl,--wrap=strncpy -Wl,--wrap=strspn -Wl,--wrap=strcspn -Wl,--wrap=strstr \
					  -Wl,--wrap=memmove -Wl,--wrap=memchr -Wl,--wrap=bcmp -Wl,--wrap=bcopy -Wl,--wrap=bzero -Wl,--wrap=index -Wl,--wrap=rindex -Wl,--wrap=strcoll \
					  -Wl,--wrap=strncat -Wl,--wrap=strpbrk -Wl,--wrap=strxfrm -Wl,--wrap=gettimeofday -Wl,--wrap=strtok -Wl,--wrap=strtol -Wl,--wrap=strtoll -Wl,--wrap=rand \
					  -Wl,--wrap=srand -Wl,--wrap=swab -Wl,--wrap=gmtime -Wl,--wrap=asctime -Wl,--wrap=atoi -Wl,--wrap=bsearch -Wl,--wrap=mempcpy -Wl,--wrap=time \
					  -Wl,--wrap=pow -Wl,--wrap=powf -Wl,--wrap=log10 -Wl,--wrap=log10f -Wl,--wrap=sqrtf -Wl,--wrap=sqrt -Wl,--wrap=atan2f -Wl,--wrap=atan2
LDFLAGS             = -nostartfiles -T $(LDSCRIPT) $(LIBRARY_DIRS) $(LDBASELIB) \
                      --cref $(LDWRAPPER) -Wl,--no-check-sections,--gc-sections,--sort-common,-Map,$(BIN_DIR)/$(LINK_NAME).map
ifeq ($(SYS_NOR_BIN_COMPRESS), 1)
MODELS             =  $(foreach module,$(REALMODULE_LIST), $(BIN_DIR)/$(module).bin.gz)
else ifeq ($(SYS_NOR_BIN_COMPRESS), 2)
MODELS             =  $(foreach module,$(REALMODULE_LIST), $(BIN_DIR)/$(module).bin.lzoy)
else
MODELS             =  $(foreach module,$(REALMODULE_LIST), $(BIN_DIR)/$(module).bin)
endif
BOOT_OBJECT =
#end

ifeq ($(SYS_NOR_BIN_COMPRESS), 2)
define gen_lzoy_target =
.PHONY: $(1)
$(1):
	$(LZOYPACK) $(BIN_DIR)/$(1) $(BIN_DIR)/$(1).lzoy
endef
$(foreach module, $(MODULE_LIST), $(eval $(call gen_lzoy_target,$(module).bin)))
LZOY_MODELS = $(foreach module,$(MODULE_LIST), $(module).bin)
.PHONY: lzoybin
lzoybin:
	@$(MAKE) -s -f $(PACK_MK) $(LZOY_MODELS) TOP=$(TOP)
endif

dump2bin:
	@$(eval DATA_START_ADDR:=0xffffffff$(shell $(AWK) '{if (/_text_rom_base *= *\./) {printf "%lx", strtonum($$1)}}' $(BIN_DIR)/$(LINK_NAME).map)) \
	$(eval DATA_END_ADDR:=0xffffffff$(shell $(AWK) '{if (/_text_rom_other_end *= *\./) {printf "%lx", strtonum($$1)}}' $(BIN_DIR)/$(LINK_NAME).map)) \
	$(shell $(XOBJDUMP) --start-address=$(DATA_START_ADDR) --stop-address=$(DATA_END_ADDR) -s $(LINK_TARGET) > $(BIN_DIR)/$(LINK_NAME).dump) \
	$(ECHO) "\t\trom DATA_START_ADDR: $(DATA_START_ADDR) - $(DATA_END_ADDR)"
	@$(eval DATA_START_ADDR:=0xffffffff$(shell $(AWK) '{if (/^sdram_data +/) {printf "%lx", strtonum($$2)}}' $(BIN_DIR)/$(LINK_NAME).map))\
	$(eval DATA_END_ADDR:=0xffffffff$(shell $(AWK) '{if (/^sdram_data +/) {printf "%lx", strtonum($$2) + strtonum($$3)}}' $(BIN_DIR)/$(LINK_NAME).map))\
	$(shell $(XOBJDUMP) --start-address=$(DATA_START_ADDR) --stop-address=$(DATA_END_ADDR) -s $(LINK_TARGET) >> $(BIN_DIR)/$(LINK_NAME).dump) \
	$(ECHO) "\t\tsdram_data DATA_START_ADDR: $(DATA_START_ADDR) - $(DATA_END_ADDR)" \
	$(shell $(AWK) -b '{if (/^ (\w{8} ){5}/){for(i=2;i<6;++i){printf "%c%c%c%c",strtonum("0x"substr($$(i),1,2)),strtonum("0x"substr($$(i),3,2)),strtonum("0x"substr($$(i),5,2)),strtonum("0x"substr($$(i),7,2)) > "$(BIN_DIR)/$(LINK_NAME).bin"}}}' $(BIN_DIR)/$(LINK_NAME).dump)
	$(foreach module, $(MODULE_LIST), $(shell $(XOBJCOPY) -j .drv_$(module) -O binary $(LINK_TARGET) $(BIN_DIR)/$(module).bin 2>/dev/null))
	$(foreach module, $(MODULE_LIST), $(shell $(CAT) /dev/null >> $(BIN_DIR)/$(module).bin))
ifeq ($(SYS_NOR_BIN_COMPRESS), 1)
	$(foreach module, $(MODULE_LIST), $(shell $(FGZIP2) -f $(BIN_DIR)/$(module).bin))
else ifeq ($(SYS_NOR_BIN_COMPRESS), 2)
	@$(MAKE) -s -f $(PACK_MK) lzoybin TOP=$(TOP)
endif

isp_dump2bin:
	$(foreach section, rom1 ram1 rom2, $(shell $(XOBJCOPY) -j .$(section) -O binary $(ISP_LINK_TARGET) $(ISP_BIN_DIR)/$(LINK_NAME)_$(section).bin 2>/dev/null))
	$(foreach section, rom1 ram1 rom2, $(shell $(CAT) /dev/null >> $(ISP_BIN_DIR)/$(LINK_NAME)_$(section).bin))
	$(CAT) `$(ECHO) "rom1 ram1 rom2" | $(AWK) -vprefix=$(ISP_BIN_DIR)/$(LINK_NAME)_ '{for (i=1;i<=NF;++i) printf prefix $$i".bin "}'` > $(BIN_DIR)/$(LINK_NAME).bin
	$(foreach module, $(ISP_RAM_MODELS), $(shell $(XOBJCOPY) -j .drv_$(module) -O binary $(LINK_TARGET) $(ISP_BIN_DIR)/$(module).bin 2>/dev/null))
	$(foreach module, $(ISP_RAM_MODELS), $(shell $(CAT) /dev/null >> $(ISP_BIN_DIR)/$(module).bin))
ifeq ($(SYS_NOR_BIN_COMPRESS), 1)
	$(foreach module, $(ISP_RAM_MODELS), $(shell $(FGZIP2) -f $(BIN_DIR)/$(module).bin))
else ifeq ($(SYS_NOR_BIN_COMPRESS), 2)
	$(foreach module, $(ISP_RAM_MODELS), $(shell $(LZOYPACK) $(BIN_DIR)/$(module).bin $(BIN_DIR)/$(module).bin.lzoy))
endif


linktarget:
	@$(ECHO) "linking $(subst $(TOP)/,,$(LINK_TARGET)) ..."
	@$(ECHO) "\t1: generate linker script."
ifneq ($(BUILD_VERBOSE), 0)
	@$(ECHO) $(CPP) $(CFLAGS_GEN_LD) $(TOP)/build/scripts/romL.ldp
endif
ifeq ($(BUILT4ISP2),1)
	@$(CPP) $(CFLAGS_GEN_LD) $(TOP)/build/scripts/romL_isp2.ldp > $(LDSCRIPT)
else
ifeq ($(BUILD_B), 1)
	@$(CPP) -DAPPBIN_OFFSET=$(APPB_BIN_OFFSET) $(CFLAGS_GEN_LD) $(TOP)/build/scripts/romL.ldp > $(LDSCRIPT)
else
	@$(CPP) -DAPPBIN_OFFSET=$(APP_BIN_OFFSET) $(CFLAGS_GEN_LD) $(TOP)/build/scripts/romL.ldp > $(LDSCRIPT)
endif
endif
	@$(ECHO) "\t2: linking..."
ifneq ($(BUILD_VERBOSE), 0)
	@$(ECHO) $(XLD) $(CFLAGS_MIPS_C) -o $(LINK_TARGET) $(BOOT_OBJECT) $(LDFLAGS)
endif
	@-$(XLD) $(CFLAGS_MIPS_C) -o $(LINK_TARGET) $(BOOT_OBJECT) $(LDFLAGS) > $(BIN_DIR)/link.log 2>&1; $(ECHO) $$? > $(BIN_DIR)/link.ret
	@if [[ `$(CAT) $(BIN_DIR)/link.ret` != 0 ]]; then \
		$(ECHO) "\t\tlink failed" ;\
		$(SED) -n '\#is full#!p' $(BIN_DIR)/link.log ;\
		if [[ `$(SED) -n '0, \#is full#p' $(BIN_DIR)/link.log` ]]; then \
			$(ECHO) section full, need update mcode.i; $(ECHO) 1 > $(BIN_DIR)/mcode_i_update;\
			$(MCODE) -s $(OUT_BIN_DIR)/romL.map -o $(OUT_SDK_INC_DIR)/module/mcode.i -i $(OUT_SDK_INC_DIR)/module/mcode.inc; \
		fi; \
		exit 1; \
	else \
		$(ECHO) "\t\tlink success";\
	fi
	@$(ECHO) "\t3: dumping..."
ifeq ($(BUILT4ISP2),1)
	$(XOBJCOPY) -j .text -O binary $(LINK_TARGET) $(BIN_DIR)/$(LINK_NAME)_text.bin
	$(XOBJCOPY) -j .data -O binary $(LINK_TARGET) $(BIN_DIR)/$(LINK_NAME)_data.bin
	$(CAT) $(BIN_DIR)/$(LINK_NAME)_text.bin $(BIN_DIR)/$(LINK_NAME)_data.bin > $(BIN_DIR)/$(LINK_NAME).bin
	$(ECHO) "\nContents in DRAM:"
	ls -lF $(subst $(TOP)/,,$(BIN_DIR)/$(LINK_NAME).bin)
#	$(XOBJDUMP) -dh $(LINK_TARGET) > $(BIN_DIR)/$(LINK_NAME).dis
	@$(ECHO) "\n\n BUILT4ISP2=1, target $@ stop here.\n\n"
else
	@$(MAKE) -s -f $(PACK_MK) dump2bin TOP=$(TOP)
	@OLD_BIN_SIZE=`$(AWK) '{if (/APPROM_BIN_SIZE */) {gsub(/\(|\)/,"",$$3); print $$3}}' $(OUT_SDK_INC_DIR)/module/module.i` ;\
	pinmux_size=`stat -c %s $(PINMUX_BIN)`;\
	app_hdr_size=`stat -c %s $(PREBUILD_BIN_DIR)/empty_128B.bin`;\
	$(MODUAL) -a $$(($$app_hdr_size + $$pinmux_size)) -f $(BIN_DIR)/$(LINK_NAME).bin -n $(BIN_DIR)/module.i.new -i $(OUT_SDK_INC_DIR)/module/module.inc $(MODELS);\
	NEW_BIN_SIZE=`$(AWK) '{if (/APPROM_BIN_SIZE */) {gsub(/\(|\)/,"",$$3); print $$3}}' $(BIN_DIR)/module.i.new 2>/dev/null` ;\
	if [[ $$OLD_BIN_SIZE != $$NEW_BIN_SIZE ]] && [[ $$OLD_BIN_SIZE != 0 ]];then \
		$(CP) -f $(BIN_DIR)/module.i.new $(OUT_SDK_INC_DIR)/module/module.i; \
		$(ECHO) "APPROM_BIN_SIZE changed, old:$$OLD_BIN_SIZE, new:$$NEW_BIN_SIZE";\
		$(ECHO) 1 > $(BIN_DIR)/module_i_update;\
		exit 1;\
	else \
		$(CP) -f $(BIN_DIR)/module.i.new $(OUT_SDK_INC_DIR)/module/module.i; \
		$(ECHO) "APPROM_BIN_SIZE:$$NEW_BIN_SIZE check ok.";\
	fi
endif

pack:
	@$(ECHO) 0 > $(BIN_DIR)/mcode_i_update ;\
	$(ECHO) 0 > $(BIN_DIR)/module_i_update ;\
	$(ECHO) "" > $(BIN_DIR)/link.log ;\
	$(ECHO) 0 > $(BIN_DIR)/link.ret ;\
	$(ECHO) 0 > $(BIN_DIR)/pack.ret
	@$(MAKE) $(MAKE_JOBS) -f $(MODULE_MK) $(BIN_DIR)/rom.bin TOP=$(TOP)
#check rom.bin size <= XX MB
	@if { [ "$(AB_BIN)" == "1" ] && [ "$(BUILD_B)" != "1" ]; } || [ "$(AB_BIN)" == "0" ]; then \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "    **generate rom.bin successful**" ;\
		if [ $(MAX_BIN_SIZE) -le $$(($(CFG_ROM_MAXSIZE) << 10)) ]; then \
			$(ECHO) "\E[0;1;32;40m    Full rom.bin ($$(($(MAX_BIN_SIZE) >> 10)) KB) <= $$(($(CFG_ROM_MAXSIZE) >> 10))MB ($(CFG_ROM_MAXSIZE) KB) !!!\E[0m" ;\
		else \
			$(ECHO) "\E[0;1;33;40m    Warning : Full rom.bin ($$(($(MAX_BIN_SIZE) >> 10)) KB) > ($(CFG_ROM_MAXSIZE) KB) !!!\E[0m" ;\
			$(ECHO) "2" > $(BIN_DIR)/pack.ret ;\
		fi ;\
		$(ECHO) "----------------------------------------" ;\
	fi

real_pack:
	@$(ECHO) "Collect module information ..."
	@$(MAKE) -s -f $(MODULE_MK) module TOP=$(TOP)
	@$(ECHO) "Collect module information (done)."

	@$(MAKE) -s -f $(PACK_MK) linktarget TOP=$(TOP)

	@$(ECHO) "\t4: packaging..."
ifeq ($(BUILT4ISP2),1)
	@$(ECHO) "\n\n BUILT4ISP2=1, target $@ stop here.\n\n"
else

	@if [ -z $(PACK_TBL) ] || [ ! -f $(PACK_TBL) ]; then \
		$(ECHO) "\033[31mError\033[0m: Pack Script file \"$(PACK_TBL)\" Not Found !"; \
		exit 1; \
	fi
ifeq ($(AB_BIN), 0)
	@$(RM) -f $(BIN_DIR)/app_b.bin
endif
ifeq ($(BUILD_B), 1)
	@$(CAT) $(PREBUILD_BIN_DIR)/empty_128B.bin $(PINMUX_BIN) $(BIN_DIR)/$(LINK_NAME).bin > $(BIN_DIR)/app_b.bin
	@$(ADDHEADERINFO) -s $(APPB_BIN_OFFSET) -c $(subst $(TOP)/,,$(PREBUILD_BIN_DIR)/config) -f $(BIN_DIR)/app_b.bin
else
	@$(CAT) $(PREBUILD_BIN_DIR)/empty_128B.bin $(PINMUX_BIN) $(BIN_DIR)/$(LINK_NAME).bin > $(BIN_DIR)/app.bin
	@$(ADDHEADERINFO) -s $(APP_BIN_OFFSET) -c $(subst $(TOP)/,,$(PREBUILD_BIN_DIR)/config) -f $(BIN_DIR)/app.bin
endif
	@if { [ "$(AB_BIN)" == "1" ] && [ "$(BUILD_B)" != "1" ]; } || [ "$(AB_BIN)" == "0" ]; then \
		eval $(GEN_PARTMAP_VAR) ;\
		"$(SHELL)" -c "$(PACK_PARTMAP) -l -x 1 -p $(PREBUILD_BIN_DIR)/$(ISP_BIN_NAME) -i $(PACK_TBL) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s $(ISP_BIN_SIZE) -o $(BIN_DIR)/rom_pack.bin" ;\
		if [ "$$?" != "0" ]; then \
			$(ECHO) "1" > $(BIN_DIR)/pack.ret ;\
			exit 1 ;\
		fi ;\
		"$(SHELL)" -c "$(PACK_PARTMAP) -p $(PREBUILD_BIN_DIR)/$(ISP_BIN_NAME) -i $(PACK_TBL) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s $(ISP_BIN_SIZE) -o $(BIN_DIR)/romL_pack.bin" ;\
		$(ECHO) "all made" ;\
		$(CP) -f $(BIN_DIR)/rom_pack.bin $(TOP)/rom.bin ;\
		$(CP) -f $(BIN_DIR)/rom_pack.bin $(BIN_DIR)/all.bin ;\
		$(CP) -f $(BIN_DIR)/romL_pack.bin $(TOP)/romL.bin ;\
		$(CP) -f $(BIN_DIR)/romL.obj $(TOP)/romL.obj ;\
		$(CP) -f $(BIN_DIR)/romL.map $(TOP)/romL.map ;\
	elif [ "$(AB_BIN)" == "1" ] && [ "$(BUILD_B)" == "1" ]; then \
		$(CP) -f $(BIN_DIR)/romL.obj $(TOP)/romL_b.obj ;\
		$(CP) -f $(BIN_DIR)/romL.map $(TOP)/romL_b.map ;\
	fi

ifeq ($(EN_CODESIZE),1)
	@$(MAKE) -s -f $(MODSZ_MK) codesize TOP=$(TOP)
endif

endif

part:
	@eval $(GEN_PARTMAP_VAR) ;\
	eval "export AB_BIN=1" ;\
	"$(SHELL)" -c "$(PACK_PARTMAP) -l -x 2 -c 3 -i $(PACK_TBL) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s 0 -o $(TOP)/spupdate.bin -a \"$(PART_ARGS) \""

parta:
	@eval $(GEN_PARTMAP_VAR) ;\
	eval "export AB_BIN=1" ;\
	"$(SHELL)" -c "$(PACK_PARTMAP) -d -l -x 2 -c 3 -i $(PACK_TBL) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s 0 -o $(TOP)/spupdate.bin -a \"$(PART_ARGS) \""

part_all:
	@eval $(GEN_PARTMAP_VAR) ;\
	"$(SHELL)" -c "$(PACK_PARTMAP) -l -c 2 -i $(PACK_TBL) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -s 0 -o $(TOP)/spupdate_all.bin"

part_mj12all:
	@$(CAT) $(PREBUILD_BIN_DIR)/empty_128B.bin $(BIN_DIR)/rom_pack.bin > $(TOP)/MJ12_all.bin
	@$(ADDHEADERINFO) -s 0 -c $(subst $(TOP)/,,$(PREBUILD_BIN_DIR)/config) -f $(TOP)/MJ12_all.bin > /dev/null
	@$(MODIFYUPGRADEFLAG) $(TOP)/MJ12_all.bin 0 > /dev/null
	@$(DD) if=$(TOP)/MJ12_all.bin of=$(TOP)/MJ12_all.bin bs=1 skip=124 seek=16 count=4 conv=notrunc status=none
	@$(ECHO) "\xaa\xaa" | $(DD) of=$(TOP)/MJ12_all.bin bs=1 seek=40 count=2 conv=notrunc status=none
	@$(ECHO) "Generator MJ12_all.bin for MJ12 update OK!"

isp_pack:
	@$(ECHO) "Collect module information ..."
	@$(MAKE) -s -f $(MODULE_MK) TOP=$(TOP)
	@$(ECHO) "Collect module information (done)."
	@$(MKDIR) -p $(ISP_BIN_DIR)
	@$(ECHO) "linking $(subst $(TOP)/,,$(ISP_LINK_TARGET)) ..."
	@$(ECHO) "\t1: generate linker script. without draminit data"
	@$(CPP) $(CFLAGS_GEN_LD) -DWO_DRAM_DATA $(TOP)/build/scripts/ispl.ldp > $(ISP_BIN_DIR)/ispl.ld
	@$(ECHO) "\t2: linking...without draminit data"
	@$(XLD) $(CFLAGS_MIPS_C) -o $(ISP_LINK_TARGET) $(ISP_BOOT_OBJECT) $(ISP_LDFLAGS)
	@$(ECHO) "\t3: build standalone dram_init"
	@DRAM_NOR_OFFSET=`$(AWK) '{if (/asdDraminits *= *\./) {printf "0x%lx", strtonum($$1)}}' $(ISP_BIN_DIR)/$(LINK_NAME).map`; \
	$(MAKE) $(MAKE_JOBS) -C $(PATH_DRAM_INIT) scan DRAM_NOR_OFFSET=$$DRAM_NOR_OFFSET TOP=
	@$(HEXDUMP) -v -e '"LONG(0x" 1/4 "%08X" ")\n"' $(INPUT_DRAM_INIT_SCAN) > $(ISP_BIN_DIR)/dramscan.raw
	@$(ECHO) "\t4: generate linker script."
	@$(CPP) $(CFLAGS_GEN_LD) $(TOP)/build/scripts/ispl.ldp > $(ISP_BIN_DIR)/ispl.ld
	@$(ECHO) "\t5: linking..."
	@$(XLD) $(CFLAGS_MIPS_C) -o $(ISP_LINK_TARGET) $(ISP_BOOT_OBJECT) $(ISP_LDFLAGS)
	@$(ECHO) "\t6: dumping..."
	@$(MAKE) -s -f $(PACK_MK) isp_dump2bin TOP=$(TOP)
	@$(MODUAL) -f $(ISP_BIN_DIR)/$(LINK_NAME).bin $(ISP_MODELS)
	@$(ECHO) "\t7: packaging..."

	@$(ECHO) "\033[32m[ISP_BIN_SIZE]\e[0m : $(ISP_BIN_SIZE)"
	@ispboot_size=`stat -c %s $(ISP_BIN_DIR)/$(LINK_NAME).bin`; \
	if [ $$ispboot_size -gt $(ISP_BIN_SIZE) ];then \
		$(ECHO) "\033[31m[Error\e[0m: ispboot.bin size $$ispboot_size is bigger than ISP_BIN_SIZE=$(ISP_BIN_SIZE) !"; \
		exit 1; \
	fi
	@$(DD) if=/dev/zero of=$(ISP_BIN_DIR)/ispboot.bin bs=1 count=$(ISP_BIN_SIZE) status=none
	@$(DD) if=$(ISP_BIN_DIR)/$(LINK_NAME).bin of=$(ISP_BIN_DIR)/ispboot.bin conv=notrunc status=none

