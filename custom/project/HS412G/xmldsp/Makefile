include $(TOP)/build/scripts/config.mk

XML_DSP_DIR=.
XML_BIN=$(BIN_DIR)/xml_pack.bin

XML_FILES=$(sort $(subst $(TOP)/,,$(wildcard $(XML_DSP_DIR)/*.xml)))
XML_TARGET=$(patsubst %,$(BIN_DIR)/%.lzoy,$(sort $(notdir $(wildcard $(XML_DSP_DIR)/*.xml))))

.DEFAULT_GOAL := all

ifeq ($(DSP_H_LIB),1)
define gen_lzoy_target =
$(BIN_DIR)/$(notdir $(1)).lzoy: $(1)
	$(LZOYPACK) -1 $(1) $(BIN_DIR)/$(notdir $(1)).lzoy
endef
$(foreach module, $(XML_FILES), $(eval $(call gen_lzoy_target,$(module))))
endif

.PHONY: lzoybin
lzoybin:
	if [[ $(DSP_H_LIB) == 1 ]]; then \
		$(MAKE) $(XML_TARGET) TOP=$(TOP);\
	fi

all: lzoybin
	if [[ $(DSP_H_LIB) == 1 ]]; then \
		$(XMLPACK) -o $(XML_BIN) -b $(CFG_ROMBIN_MIN_BLOCKSIZE) -n $(CFG_XMLPACK_HEAD_SIZE) -x $(CFG_SINGLE_XML_SIZE) -p $(BIN_DIR)/ -c .lzoy $(XML_FILES) ;\
		$(ECHO) "  XMLPACK $(XML_FILES) to $(subst $(TOP)/,,$(XML_BIN))" ;\
	fi

