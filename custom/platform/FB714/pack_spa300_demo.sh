
partition_tbl="\
$PREBUILD_BIN_DIR/vendor_data.bin	0x1000		\
$BIN_DIR/xml_pack.bin				0x28000		\
$BIN_DIR/dsptool_config.xml         0x6000		\
$BIN_DIR/ep_firmware.bin            0x28000		\
$BIN_DIR/app.bin					0x3c0000	\

"
# AB_BIN info
if [ ! -z "$AB_BIN" ] && [ "$AB_BIN" -eq "1" ]; then
partition_tbl="$partition_tbl \
$BIN_DIR/app_b.bin					0x3c0000	\
"
fi

