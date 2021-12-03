#sinclude $(TOP)/build/scripts/config.mk

PROJECT_CFGS:=$(sort $(wildcard $(CUST_PRO_DIR)/*/project*.cfg))
BOARD_CFGS:=$(sort $(wildcard $(CUST_BRD_DIR)/*/board*.cfg))

NULL :=

EN_PRJNAME_CFG := 1
ifeq ($(EN_PRJNAME_CFG),1)
define get_boardname_bycfg
$(eval BOARD_NUM:=) \
$(eval $(shell $(SED) -n '/^BOARD_NUM =/p' $(1))) \
$(eval BOARD_NAMES+=$(BOARD_NUM)) \
$(eval BOARD_CFG_$(BOARD_NUM):=$(1))
endef
define get_prjname_bycfg
$(eval PROJECT_NAME:=) \
$(eval $(shell $(SED) -n '/^PROJECT_NAME =/p' $(1))) \
$(if $(PROJECT_NAME),,$(eval PROJECT_NAME:=$(word 2,$(subst /, ,$(1))))) \
$(eval PROJECT_NAMES+=$(PROJECT_NAME)) \
$(eval PROJECT_CFG_$(PROJECT_NAME):=$(1))
endef
#collect project names
$(foreach cfg, $(BOARD_CFGS), $(eval $(call get_boardname_bycfg,$(cfg))))
$(foreach cfg, $(PROJECT_CFGS), $(eval $(call get_prjname_bycfg,$(cfg))))
#$(info PROJECT_NAMES:$(PROJECT_NAMES):$(PROJECT_CFGS))
define gen_boardcfg_target =
.PHONY: $(1)
$(1):
	@$(MAKE) -s board_config SET_BOARD=$(1)
endef
define gen_config_target =
.PHONY: $(1)
$(1):
	@$(MAKE) -s pro_config SET_PROJECT=$(1)
endef
$(foreach prj,$(BOARD_NAMES),$(eval $(call gen_boardcfg_target,$(prj))))
$(foreach prj,$(PROJECT_NAMES),$(eval $(call gen_config_target,$(prj))))
endif

pro_config:
	SET_PROJECT=$(SET_PROJECT);\
	SET_CFG=$(TOP)/$(PROJECT_CFG_$(SET_PROJECT));\
	echo "---------------------------------------";\
	echo "project = $$SET_PROJECT";\
	echo "project configure = $$SET_CFG";\
	echo "---------------------------------------";\
	if test -z $$SET_PROJECT || test -z $$SET_CFG; then \
		echo "****configure failed.";\
		exit 1;\
	fi;\
	$(MKDIR) -p $(OUT_ROOT); \
	echo "SYS_PROJECT = $$SET_PROJECT" > $(PROJECT_MK); \
	echo "PROJECT_CFG = $$SET_CFG" >> $(PROJECT_MK);

config:
	$(info ---------------------------------------)
	$(info $(NULL)      configure project to build       )
	@if test -z $(SET_PROJECT); then \
		echo "---------------------------------------";\
		echo "<<select project>>:";\
		let "cfg_num=0";\
		for prj_cfg in $(PROJECT_CFGS); do \
			let "cfg_num=$$cfg_num+1";\
			echo $$cfg_num. "`printf "%-60s" $$prj_cfg | $(SED) 's#^$(CUST_PRO_DIR)/##g'`" ["`$(SED) -n 's#^PROJECT_PROMPT =##p' $$prj_cfg | $(AWK) -F'\n' '{printf "%-25s",$$1}'`" ]' TAG:'[`$(SED) -n 's#^USER_PROJECT_TAG =##p' $$prj_cfg` ];\
		done ;\
		read -p "Select [1 .. $$cfg_num, default 1] :" select_number;\
		if test -z $$select_number; then \
			select_number=1;\
		fi;\
		let "cfg_num=0";\
		for prj_cfg in $(PROJECT_CFGS); do \
			let "cfg_num=$$cfg_num+1";\
			if [[ $$cfg_num -eq $$select_number ]]; then \
				SET_PROJECT=`$(SED) -n 's#^PROJECT_NAME =##p' $$prj_cfg`;\
				if test -z $$SET_PROJECT; then \
					SET_PROJECT=`echo $$prj_cfg | $(AWK) -F/ '{print $$3}'`;\
				fi;\
				SET_CFG=$(TOP)/$$prj_cfg;\
				break; \
			fi;\
		done;\
	fi;\
	echo "---------------------------------------";\
	echo "project = $$SET_PROJECT";\
	echo "project configure = $$SET_CFG";\
	echo "---------------------------------------";\
	if test -z $$SET_PROJECT || test -z $$SET_CFG; then \
		echo "****configure failed.";\
		exit 1;\
	fi;\
	$(MKDIR) -p $(OUT_ROOT); \
	echo "SYS_PROJECT = $$SET_PROJECT" > $(PROJECT_MK); \
	echo "PROJECT_CFG = $$SET_CFG" >> $(PROJECT_MK);
	@$(MAKE) -s board_config

board_config:
	$(info ---------------------------------------)
	$(info $(NULL)       configure board to build)
	@if test -z $(SET_BOARD); then \
		echo "---------------------------------------";\
		echo "<<select board>>:";\
		let "cfg_num=0";\
		for borad_cfg in $(BOARD_CFGS); do \
		let "cfg_num=$$cfg_num+1";\
		echo $$cfg_num. "`printf "%-60s" $$borad_cfg | $(SED) 's#^$(CUST_BRD_DIR)/##g'`" [ "`$(SED) -n 's#^BOARD_NAME =##p' $$borad_cfg | $(AWK) '{printf "%-20s",$$1}'`" ]' TAG:'[`$(SED) -n 's#^BOARD_NUM =##p' $$borad_cfg` ];\
		done ;\
		read -p "Select [1 .. $$cfg_num, default 1] :" select_board_number;\
		if test -z $$select_board_number; then \
			select_board_number=1;\
		fi;\
		let "cfg_num=0";\
		for borad_cfg in $(BOARD_CFGS); do \
			let "cfg_num=$$cfg_num+1";\
			if [[ $$cfg_num -eq $$select_board_number ]]; then \
				SET_BOARD=`$(SED) -n 's#^BOARD_NAME =##p' $$borad_cfg`;\
				if test -z $$SET_BOARD; then \
					SET_BOARD=`echo $$borad_cfg | $(AWK) -F/ '{print $$3}'`;\
				fi;\
				SET_BOARD_CFG=$(TOP)/$$borad_cfg;\
				break; \
			fi;\
		done ;\
	else \
		SET_BOARD_CFG=$(TOP)/$(BOARD_CFG_$(SET_BOARD));\
		SET_BOARD=`$(SED) -n 's#^BOARD_NAME =##p' $$SET_BOARD_CFG`;\
	fi;\
	echo "---------------------------------------";\
	echo "board = $$SET_BOARD";\
	echo "board configure = $$SET_BOARD_CFG";\
	echo "---------------------------------------";\
	if test -z $$SET_BOARD || test -z $$SET_BOARD_CFG; then \
		echo "****configure failed.";\
		exit 1;\
	fi;\
	$(MKDIR) -p $(OUT_ROOT); \
	echo "BOARD_NAME = $$SET_BOARD" >> $(PROJECT_MK); \
	echo "BOARD_CFG = $$SET_BOARD_CFG" >> $(PROJECT_MK);\
	$(MAKE) -s setup_prjout TOP=$(TOP); \
	echo "**configure ok.";

setup_prjout:
	$(MKDIR) -p $(OUT_DIR);
	$(MKDIR) -p $(OUT_BIN_DIR);
	$(MKDIR) -p $(OUT_BIN_PACK_DIR);
	$(MKDIR) -p $(OUT_SDK_DIR);
	$(MKDIR) -p $(OUT_SDK_LIB_DIR);
	$(MKDIR) -p $(OUT_SDK_INC_DIR);
	$(MKDIR) -p $(OUT_CUSTOM_LIB_DIR);

EN_PRJLIST := 0
ifeq ($(EN_PRJLIST),1)
define gen_list_info =
	$(info $(NULL)           Project List                )
	$(foreach prj,$(PROJECT_NAMES),$(info [$(prj)] $(PROJECT_CFG_$(prj))))
	$(info ---------------------------------------)
	$(info *   please use <make config>)
	$(info *   or <make [$(addsuffix |,$(PROJECT_NAMES))]> configure project to build)
endef

list:
	$(eval $(gen_list_info))
	@$(ECHO) " "
endif

