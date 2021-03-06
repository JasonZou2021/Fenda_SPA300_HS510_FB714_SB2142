# everything (.text, .rodata, ..) is in DRAM if 1
BUILT4ISP2 ?= 0

TOP = $(CURDIR)
sinclude $(TOP)/build/scripts/config.mk
sinclude $(TOP)/build/scripts/prjcfg.mk
sinclude $(TOP)/build/scripts/zebu.mk
#default mark gen_mcodei target
#sinclude $(TOP)/build/scripts/mcode.mk
#$(info TOP:$(TOP))
ifeq (,$(findstring $(patsubst %/rls,%,$(TOP)),$(realpath $(dir $(GCC_LIB)))))
$(error Your PATH variable not in current directory, Please set environment First.)
endif

#ap must be the last one for link.
DIRS += src
DIRS += $(PROJECT_DIR)

ifneq ($(SDK_RELEASE),1)
SDK_RELEASE := 0
endif

ifeq ("$(origin V)", "command line")
  BUILD_VERBOSE = $(V)
endif
ifndef BUILD_VERBOSE
  BUILD_VERBOSE = 0
endif
export BUILD_VERBOSE

ifeq ($(BUILD_VERBOSE),0)
	MAKE_VERBOSE := --no-print-directory
else
	MAKE_VERBOSE :=
endif

.PHONY: all list help clean doxygen pack prerequisite sdk_release release release2
.NOTPARALLEL: prerequisite
.DEFAULT_GOAL=all

$(PRJ_CFG_H): $(PROJECT_CFG) $(BOARD_CFG) $(COMMON_DIR)/common.cfg
	@$(MKDIR) -p $(OUT_DIR)/config
	@$(ECHO) "#ifndef _PROJECT_CFG_H_" > $(PRJ_CFG_H)
	@$(ECHO) "#define _PROJECT_CFG_H_" >> $(PRJ_CFG_H)
	@$(ECHO)  >> $(PRJ_CFG_H)
	@$(ECHO) "// This header file is automatically generated by custom config, such as project.cfg, common.cfg..." >> $(PRJ_CFG_H)
	@$(ECHO) "// Do not include it in SDK source code or try to modify it" >> $(PRJ_CFG_H)
	@$(ECHO)  >> $(PRJ_CFG_H)
	@$(AWK) '$$1 ~ /^CFG_/ && $$2 == "=" && NF == 3 {print "#define", $$1, "("$$3")"}' $(PROJECT_CFG) >> $(PRJ_CFG_H)
	@$(AWK) '$$1 ~ /^USER_PROJECT_TAG/ && $$2 == "="  && NF == 3 {print "#define", $$1, "("$$3")"}' $(PROJECT_CFG) >> $(PRJ_CFG_H)
	@$(AWK) '$$1 ~ /^CFG_/ && $$2 == "=" && NF == 3 {print "#define", $$1, "("$$3")"}' $(BOARD_CFG) >> $(PRJ_CFG_H)
	@$(ECHO) "#define IOP_CODE_SIZE_CFG ("$(IOP_CODE_SIZE_CFG)")" >> $(PRJ_CFG_H)
	@$(ECHO) "#define DSP_H0_SIZE_CFG ("$(DSP_H0_SIZE_CFG)")" >> $(PRJ_CFG_H)
	@$(ECHO) "#define DSP_H1_SIZE_CFG ("$(DSP_H1_SIZE_CFG)")" >> $(PRJ_CFG_H)
	@$(ECHO) "#define DSP_C_SIZE_CFG ("$(DSP_C_SIZE_CFG)")" >> $(PRJ_CFG_H)
	@$(ECHO)  >> $(PRJ_CFG_H)
	@$(ECHO) "#endif // _PROJECT_CFG_H_" >> $(PRJ_CFG_H)
	@$(ECHO)  >> $(PRJ_CFG_H)
	@$(ECHO) generate $(subst $(TOP)/,,$(PRJ_CFG_H))

.PHONY: $(DIRS) target_dirs
$(foreach dir, $(DIRS), $(eval $(call gen_targetdir,$(dir))))

target_dirs: $(DIRS)
ABBIN: prerequisite $(PINMUX_BIN) $(PRJ_CFG_H)
	@$(MAKE) -s -f $(TOP)/build/scripts/codebase_version.mk TOP=$(TOP)
	@$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs
	@$(ECHO) "----------------------------------------"
	@$(ECHO) "build app_b.bin : 1"
	@-$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 BUILD_B=1
	@if [[ `$(CAT) $(BIN_DIR)/mcode_i_update` == 0 ]];then \
		if [[ `$(CAT) $(BIN_DIR)/link.ret` != 0 ]]; then \
			exit 1 ;\
		fi \
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed mcode.i, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(ECHO) "build app_b.bin : 2" ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 BUILD_B=1 || true ;\
	fi
	@if [[ `$(CAT) $(BIN_DIR)/module_i_update` == 0 ]];then \
		if [[ `$(CAT) $(BIN_DIR)/link.ret` != 0 ]]; then \
			exit 1 ;\
		fi \
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed module.i, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(ECHO) "build app_b.bin : 3" ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 BUILD_B=1 || true ;\
	fi
	@if [[ `$(CAT) $(BIN_DIR)/module_i_update` == 0 ]];then \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "build app.bin : 1" ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 ;\
		if [[ `$(CAT) $(BIN_DIR)/pack.ret` == 0 ]];then \
			exit 0 ;\
		else \
			exit 1 ;\
		fi ;\
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed module.i 2nd time, APPROM_BIN_SIZE may influence itself gzip size, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(ECHO) "build app_b.bin : 4" ;\
		$(MAKE) -s -f $(TOP)/abuild/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 BUILD_B=1 ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(ECHO) "build app_a.bin": 1 ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) AB_BIN=1 ;\
	fi
ifneq (,$(PROJECT_EXTRA_CMD))
	@$(PROJECT_EXTRA_CMD)
endif

all: prerequisite $(PINMUX_BIN) $(PRJ_CFG_H)
	@$(MAKE) -s -f $(TOP)/build/scripts/codebase_version.mk TOP=$(TOP)
	@$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs
	@-$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP)
	@if [[ `$(CAT) $(BIN_DIR)/mcode_i_update` == 0 ]];then \
		if [[ `$(CAT) $(BIN_DIR)/link.ret` != 0 ]]; then \
			exit 1 ;\
		fi \
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed mcode.i, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) || true ;\
	fi
	@if [[ `$(CAT) $(BIN_DIR)/module_i_update` == 0 ]];then \
		if [[ `$(CAT) $(BIN_DIR)/link.ret` != 0 ]]; then \
			exit 1 ;\
		fi \
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed module.i, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) || true ;\
	fi
	@if [[ `$(CAT) $(BIN_DIR)/module_i_update` == 0 ]];then \
		if [[ `$(CAT) $(BIN_DIR)/pack.ret` == 0 ]];then \
			exit 0 ;\
		else \
			exit 1 ;\
		fi ;\
	else \
		$(ECHO) "----------------------------------------" ;\
		$(ECHO) "fixed module.i 2nd time, APPROM_BIN_SIZE may influence itself gzip size, re-build now ..." ;\
		$(RM) -f $(SDK_MODULE_LIBS) ;\
		$(RM) -f $(CUSTOM_MODULE_LIBS) ;\
		$(MAKE) $(MAKE_JOBS) $(MAKE_VERBOSE) target_dirs ;\
		$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP) ;\
	fi
ifneq (,$(PROJECT_EXTRA_CMD))
	@$(PROJECT_EXTRA_CMD)
endif

pack:
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk pack TOP=$(TOP)

ifeq ($(firstword $(MAKECMDGOALS)), $(filter $(firstword $(MAKECMDGOALS)), part parta))
part_ARGS := $(wordlist 2, $(words $(MAKECMDGOALS)), $(MAKECMDGOALS))
$(eval $(part_ARGS):;@:)
endif

.PHONY: part parta part_all part_mj12all
part:
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk part TOP=$(TOP) PART_ARGS="$(part_ARGS)"

parta:
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk parta TOP=$(TOP) PART_ARGS="$(part_ARGS)"

part_all:
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk part_all TOP=$(TOP)

part_mj12all:
	@$(MAKE) -s -f $(TOP)/build/scripts/pack.mk part_mj12all TOP=$(TOP)

help:
	@$(ECHO) "======================================="
	@$(ECHO) "          Source Code Manual           "
	@$(ECHO) "======================================="
	@$(ECHO) ""
	@$(ECHO) "[build flow]"
	@$(ECHO) "  $$ make config (configure the project to build)"
	@$(ECHO) "  $$ make all "
	@$(ECHO) "[build option]"
	@$(ECHO) "  - list      : show project config list"
	@$(ECHO) "  - help      : show this Manual"
	@$(ECHO) "  - all       : build all"
	@$(ECHO) "  - clean     : clean obj file"
	@$(ECHO) "  - doxygen   : create doxygen file obj file [example :make doxygen DXGIN=.\src DXGOUT=.\out]"

isp_dvlp:
	@$(MAKE) BUILT4ISP2=1 all
	@$(MAKE) xboot BUILT4ISP2=1
	@$(RM) -rf $(OUT_BIN_DIR)/built4isp2
	@$(MKDIR) -p $(OUT_BIN_DIR)/built4isp2
	@$(MV) $(OUT_DIR)/xboot/bin/xboot.img $(OUT_BIN_DIR)/built4isp2
	@$(MV) -T $(OUT_BIN_DIR)/romL.bin $(OUT_BIN_DIR)/built4isp2/isp2app.bin
	# @$(CP) -f ~/somewhere/working_all.bin $(OUT_BIN_DIR)/built4isp2/all.bin # all.bin = (128-byte header + rom.bin)
	# @$(CP) -f ~/somewhere/working_romL.bin $(OUT_BIN_DIR)/built4isp2/romL.bin # romL.bin is required for partial update only
	@touch $(OUT_BIN_DIR)/built4isp2/all.bin	# make sure there is all.bin
	@$(ECHO) "Pack materials into ISPBOOOT.BIN"
	$(ISP2) $(OUT_BIN_DIR)/built4isp2/ISPBOOOT.BIN \
		$(OUT_BIN_DIR)/built4isp2/xboot.img \
		$(OUT_BIN_DIR)/built4isp2/isp2app.bin \
		$(OUT_BIN_DIR)/built4isp2/all.bin
	@$(ECHO) "----------------------------------------"
	@$(ECHO) "$(OUT_BIN_DIR)/built4isp2:"
	@ls -lF $(OUT_BIN_DIR)/built4isp2
	@$(ECHO) "----------------------------------------"

xboot:
ifeq ($(SYS_ZEBU_ENV),1)
	$(MAKE) -C $(PATH_XBOOT) spa100_zebu_defconfig TOP=$(TOP)
else
	$(MAKE) -C $(PATH_XBOOT) spa100_defconfig TOP=$(TOP)
endif
	$(MAKE) -C $(PATH_XBOOT) TOP=$(TOP) BUILT4ISP2=$(BUILT4ISP2)

prerequisite:
	@if test -f $(PROJECT_MK); then \
		$(ECHO) "current project: $(SYS_PROJECT)"; \
	else \
		$(ECHO) "======================================="; \
		$(ECHO) "*no project configured"; \
		$(ECHO) "*please use <make config> to configure first." ;\
		$(ECHO) "======================================="; \
		exit 1; \
	fi
	@$(RM) -f $(SDK_MODULE_LIBS)
	@$(RM) -f $(CUSTOM_MODULE_LIBS)
ifneq ($(ISPBIN),1)
	@$(RM) -f $(TOP)/*.bin
	@$(RM) -f $(TOP)/*.map
	@$(RM) -f $(TOP)/*.obj
	@$(MAKE) $(MAKE_JOBS) -s -f $(TOP)/build/scripts/prebuilt.mk extract_prebuilt TOP=$(TOP)
else
	@$(MAKE) $(MAKE_JOBS) -s -f $(TOP)/build/scripts/prebuilt.mk extract_prebuilt ISPBIN=1 TOP=$(TOP)
endif

clean:
ifneq ($(SYS_PROJECT),)
	$(foreach lib,$(shell $(SED) -n -e 's#:# #g' -e 's#\.a##gp' $(CUSTOM_MODULE_LIBS) | $(AWK) '{print $$3}'),$(shell rm -f $(OUT_OBJ_DIR)/$(lib)/*))
	@$(RM) -f $(OUT_CUSTOM_LIB_DIR)/*.a
	@$(ECHO) "Clean custom project $(SYS_PROJECT) immediate files success."
else
	@$(ECHO) "No custom project immediate files need clean."
endif

prj_clean:
	@$(RM) -rf $(OUT_DIR)/*
	@$(RM) -f $(TOP)/*.bin
	@$(RM) -f $(TOP)/*.map
	@$(RM) -f $(TOP)/*.obj
	@$(MKDIR) -p $(OUT_DIR)
	@$(MKDIR) -p $(OUT_BIN_DIR)
	@$(MKDIR) -p $(OUT_BIN_PACK_DIR)
	@$(MKDIR) -p $(OUT_SDK_DIR)
	@$(MKDIR) -p $(OUT_SDK_LIB_DIR)
	@$(MKDIR) -p $(OUT_SDK_INC_DIR)
	@$(MKDIR) -p $(OUT_CUSTOM_LIB_DIR)
	@$(ECHO) "Clean current project rom.bin and immediate files success."
	@$(ECHO) "======================================="

all_clean:
	@$(RM) -rf $(OUT_ROOT)
	@$(RM) -f $(TOP)/*.bin
	@$(RM) -f $(TOP)/*.map
	@$(RM) -f $(TOP)/*.obj
	@$(ECHO) "Clean all project config & build result success."
	@$(ECHO) "======================================="

#doxygen
DXGIN = $(input)
DXGOUT = $(output)
ifeq ($(DXGIN), )
DXGIN = .\src
endif
ifeq ($(DXGOUT), )
DXGOUT = .\out\doxygen
endif
DOXYFILE = $(TOP)/build/config/Doxyfile
OUTDOXYFILE = $(OUT_ROOT)/doxygen/Doxyfile
doxygen: prerequisite
	@$(ECHO) "@@doxygen $(DXGIN)"
	@$(ECHO) "@@doxygen $(DXGOUT)"
	@$(RM) -rf $(dir $(OUTDOXYFILE));\
	$(MKDIR) -p $(dir $(OUTDOXYFILE));\
	$(CP) -f $(DOXYFILE) $(OUTDOXYFILE)
	@$(ECHO) "INPUT        = $(DXGIN)" >> $(OUTDOXYFILE)
	@$(ECHO) "OUTPUT_DIRECTORY       = $(DXGOUT)" >> $(OUTDOXYFILE)
	$(DOXYGEN) $(OUTDOXYFILE)

isp:
ifeq ($(BUILT4ISP2),1)
	@$(ECHO) "BUILT4ISP2=1, do nothing for target $@"
else
	@$(MAKE) $(MAKE_VERBOSE) -B isp_project ISPBIN=1
	@$(CP) $(OUT_ISP_DIR)/ispboot.bin $(PREBUILD_BIN_DIR)/ispboot_auto.bin
endif


isp_clean:
	@$(RM) -rf $(dir $(OUT_ISP_DIR))

isp_setup_prjout:
	@$(MKDIR) -p $(OUT_DIR)
	@$(MKDIR) -p $(OUT_BIN_DIR)
	@$(MKDIR) -p $(OUT_SDK_DIR)
	@$(MKDIR) -p $(OUT_SDK_LIB_DIR)
	@$(MKDIR) -p $(OUT_SDK_INC_DIR)
	@$(MKDIR) -p $(OUT_CUSTOM_LIB_DIR)

isp_project: isp_setup_prjout prerequisite $(PRJ_CFG_H)
	@$(MAKE) -s -f $(TOP)/build/scripts/codebase_version.mk TOP=$(TOP)
	@$(MAKE) $(MAKE_JOBS) -C $(PATH_NORMAL_BOOT) -s ISPBIN=1 BUILT4ISP2=$(BUILT4ISP2) TOP=$(TOP)
	@$(MAKE) $(MAKE_JOBS) -C $(PATH_ISP_BOOT) -s ISPBIN=1 TOP=$(TOP)
	@$(MAKE) isp_pack ISPBIN=1

isp_pack:
	@$(MAKE) -f $(TOP)/build/scripts/pack.mk isp_pack TOP=$(TOP)

$(PINMUX_BIN):$(PINMUX_CFG)
	@$(PINMUXCFG) -i $(TOP)/$(PINMUX_CFG) -o $(dir $(PINMUX_BIN))
ifeq ($(BUILT4ISP2),1)
	@$(ECHO) "// Autogen, do not edit\n\nconst unsigned char pinmux_bin[] __attribute__ ((aligned(16))) = {" > $(PINMUX_BIN_H)
	@$(CAT) $(PINMUX_BIN) | xxd -c 16 -i >> $(PINMUX_BIN_H)
	@$(ECHO) "};" >> $(PINMUX_BIN_H)
	@$(MV) $(PINMUX_BIN_H) $(CUSTOM_INC_DIR)
endif

sdk_release:
	@$(MAKE) -f $(TOP)/build/scripts/sdkrls.mk TOP=$(TOP) RLSNAME=$(RLSNAME) sdk_release

sdk_release_clean:
	@$(MAKE) -f $(TOP)/build/scripts/sdkrls.mk TOP=$(TOP) RLSNAME=$(RLSNAME) sdk_release_clean

sdk_release_sync:
	@$(MAKE) -f $(TOP)/build/scripts/sdkrls.mk TOP=$(TOP) RLSNAME=$(RLSNAME) sdk_release_sync

ifeq ($(RLSBOARD),)
RLSBOARD:=SPA300_6
endif

ifeq ($(RLSPRJ),)
RLSPRJ:=sunplus_main_SPA300
endif
ifeq ($(RLSNAME),)
RLSNAME:=base
endif

release:
	@$(RM) -rf rls
	@$(MAKE) $(MAKE_VERBOSE) all_clean
	@$(MAKE) $(MAKE_VERBOSE) $(RLSPRJ)
	@$(MAKE) $(MAKE_VERBOSE) $(RLSBOARD)
	@$(MAKE) $(MAKE_VERBOSE) sdk_release_clean
	@$(MAKE) -s -f $(TOP)/build/scripts/codebase_version.mk TOP=$(TOP)
	@$(MAKE) $(MAKE_VERBOSE) sdk_release_sync
	@$(MAKE) $(MAKE_VERBOSE) -C rls release2

release2:
	@$(MAKE) all SDK_RELEASE=1
	@$(MAKE) sdk_release RLSNAME=$(RLSNAME)
	@$(MAKE) all_clean
	@$(RM) -rf src out build/scripts/sdkrls.mk

MODSZ_MK := $(TOP)/build/scripts/modsize.mk
codesize:
	@$(MAKE) -s -f $(MODSZ_MK) codesize TOP=$(TOP)

test: prerequisite
	@$(ECHO) build test libs...
	@$(MAKE) -s -C src/test TOP=$(TOP)
	@$(ECHO) "----------------------------------------"
	@$(ECHO) build test libs finished.

appinfo:
	@"$(SHELL)" -c "$(APP_INFO) -c $(PREBUILD_BIN_DIR)/config"

showispsectionsize2:
	@if [ !  -z "$(CFG_ROMBIN_MIN_BLOCKSIZE)" ]; then \
		if [ -f "$(SHOW_SECTION)" ]; then \
			$(SHOW_SECTION) -i $(OUT_ISP_DIR)/romL.map -p $(OUT_ISP_DIR) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -r $(PREBUILD_BIN_DIR) $(REALMODULE_LIST) ;\
		fi \
	else \
		$(ECHO) "Please make config first!" ;\
	fi

showispsectionsize:
	@$(MAKE) showispsectionsize2 ISPBIN=1

showsectionsize:
	@if [ !  -z "$(CFG_ROMBIN_MIN_BLOCKSIZE)" ]; then \
		if [ -f "$(SHOW_SECTION)" ]; then \
			$(SHOW_SECTION) -i $(TOP)/romL.map -p $(BIN_DIR) -b $$(($(CFG_ROMBIN_MIN_BLOCKSIZE) << 10)) -r $(PREBUILD_BIN_DIR) $(REALMODULE_LIST) ;\
		fi \
	else \
		$(ECHO) "Please make config first!" ;\
	fi

