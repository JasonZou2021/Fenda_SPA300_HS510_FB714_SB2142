include $(TOP)/build/scripts/config.mk

target_file=dsptool_config.xml

$(BIN_DIR)/$(target_file): $(target_file)
	@$(ECHO) "  Compress $(target_file)"
	@$(LZOYPACK) -1 $(target_file) $(BIN_DIR)/$(target_file)

