include $(TOP)/build/scripts/config.mk

EP_DIR=firmware_bin
ep_target_files=$(sort $(wildcard $(EP_DIR)/*.bin))

ep_target_file=$(word $(words $(ep_target_files)),$(ep_target_files))
# for custom setting
#ep_target_file=$(EP_DIR)/EP92A7E_Sunplus_V65.bin

out_target_file=$(BIN_DIR)/ep_firmware.bin

all:
	@$(ECHO) "  Copy $(ep_target_file) to $(subst $(TOP)/,,$(out_target_file))"
	@$(CP) -f $(ep_target_file) $(out_target_file)

