#ifndef __IT6622__
#define __IT6622__


typedef unsigned char iTE_u1 ;
typedef char iTE_s8, *iTE_ps8;
typedef unsigned char iTE_u8, *iTE_pu8;
typedef short iTE_s16, *iTE_ps16;
typedef unsigned short iTE_u16, *iTE_pu16;
typedef unsigned long iTE_u32, *iTE_pu32;

#define SB_CEC_AUTO				(0)		// follow TV
#define SB_CEC_OFF				(1)

#define SB_POWER_OFF			(0)
#define SB_POWER_ON				(1)
#define SB_STANDBY				(2)
#define SB_PASS_THROUGH			(3)

iTE_u8 const CEC_VENDOR_ID[3] = { 0xA8, 0xF7, 0x66 };		// ITE
iTE_u8 const CEC_OSD_NAME[14] = "IT6622SB";
iTE_u8 const CEC_OSD_STRING[13] = "IT6622_SB";

//****************************************************************************

/***********  Version 0x00 ~ 0x08 ***********/
/***********  Chip Information 0x00 ~ 0x02 ***********/
/***********  Firmware Major Version 0x03 ~ 0x04 ***********/
/***********  Reserved 0x05 ***********/
/***********  Firmware Minor Version 0x06 ~ 0x08 ***********/
/***********  Reserved 0x09 ~ 0x0F ***********/
/***********  Audio Active 0x10 ***********/
#define I2C_ADO_ACTIVE_ARC_SHIFT	(0)
#define I2C_ADO_ACTIVE_ARC_MASK		(1 << I2C_ADO_ACTIVE_ARC_SHIFT)
#define I2C_ADO_ACTIVE_ARC_CLR		(0 << I2C_ADO_ACTIVE_ARC_SHIFT)
#define I2C_ADO_ACTIVE_ARC_SET		(1 << I2C_ADO_ACTIVE_ARC_SHIFT)

#define I2C_ADO_ACTIVE_EARC_SHIFT	(1)
#define I2C_ADO_ACTIVE_EARC_MASK	(1 << I2C_ADO_ACTIVE_EARC_SHIFT)
#define I2C_ADO_ACTIVE_EARC_CLR		(0 << I2C_ADO_ACTIVE_EARC_SHIFT)
#define I2C_ADO_ACTIVE_EARC_SET		(1 << I2C_ADO_ACTIVE_EARC_SHIFT)

#define I2C_ADO_ACTIVE_HDMI_SHIFT	(2)
#define I2C_ADO_ACTIVE_HDMI_MASK	(1 << I2C_ADO_ACTIVE_HDMI_SHIFT)
#define I2C_ADO_ACTIVE_HDMI_CLR		(0 << I2C_ADO_ACTIVE_HDMI_SHIFT)
#define I2C_ADO_ACTIVE_HDMI_SET		(1 << I2C_ADO_ACTIVE_HDMI_SHIFT)
// (3) ~ (7) Reserved
/***********  Audio Volume 0x11 ***********/
#define I2C_ADO_VOL_VALUE_SHIFT		(0)
#define I2C_ADO_VOL_VALUE_MASK		(0x7F << I2C_ADO_VOL_VALUE_SHIFT)

#define I2C_ADO_VOL_MUTE_SHIFT		(7)
#define I2C_ADO_VOL_MUTE_MASK		(1 << I2C_ADO_VOL_MUTE_SHIFT)
#define I2C_ADO_VOL_MUTE_CLR		(0 << I2C_ADO_VOL_MUTE_SHIFT)
#define I2C_ADO_VOL_MUTE_SET		(1 << I2C_ADO_VOL_MUTE_SHIFT)
/***********  Reserved 0x12 ***********/
/***********  Video Latency 0x13 ***********/
/***********  Latency Flags 0x14 ***********/
/***********  Audio Output Delay 0x15 ***********/
/***********  Audio Information 0x16 ~ 0x19 ***********/
/***********  Audio Channel Allocation 0x1A ***********/
/***********  SB Volume Max. 0x1B ***********/
/***********  Audio Mux. Select 0x1C ***********/
#define I2C_ADO_MUX_SEL_SHIFT		(0)
#define I2C_ADO_MUX_SEL_MASK		(7 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_HDMI		(0 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EARC		(1 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S1	(2 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S2	(3 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_I2S3	(4 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF1	(5 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF2	(6 << I2C_ADO_MUX_SEL_SHIFT)
#define I2C_ADO_MUX_SEL_EXT_SPDIF3	(7 << I2C_ADO_MUX_SEL_SHIFT)
// (3) ~ (7) Reserved
/***********  MCLK Frequency Select 0x1D ***********/
#define I2C_ADO_MCLK_FREQ_SHIFT		(0)
#define I2C_ADO_MCLK_FREQ_MASK		(7 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_128FS		(0 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_256FS		(1 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_384FS		(2 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_512FS		(3 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_640FS		(4 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_768FS		(5 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_894FS		(6 << I2C_ADO_MCLK_FREQ_SHIFT)
#define I2C_ADO_MCLK_FREQ_1024FS	(7 << I2C_ADO_MCLK_FREQ_SHIFT)
/***********  Audio InforFrame Packet 0x1E ~ 0x23 ***********/
/***********  Audio Channel Status 0x24 ~ 0x28 ***********/
/***********  Reserved 0x29 ~ 0x2F ***********/
/***********  SB EDID 0x40 ~ 0xBF ***********/
#define I2C_EDID_UPD_PID_SHIFT	(0)
#define I2C_EDID_UPD_PID_MASK	(1 << I2C_EDID_UPD_PID_SHIFT)
#define I2C_EDID_UPD_PID_CLR	(0 << I2C_EDID_UPD_PID_SHIFT)
#define I2C_EDID_UPD_PID_SET	(1 << I2C_EDID_UPD_PID_SHIFT)

#define I2C_EDID_UPD_PNAME_SHIFT	(1)
#define I2C_EDID_UPD_PNAME_MASK		(1 << I2C_EDID_UPD_PNAME_SHIFT)
#define I2C_EDID_UPD_PNAME_CLR		(0 << I2C_EDID_UPD_PNAME_SHIFT)
#define I2C_EDID_UPD_PNAME_SET		(1 << I2C_EDID_UPD_PNAME_SHIFT)

#define I2C_EDID_UPD_ADB0_SHIFT		(3)
#define I2C_EDID_UPD_ADB0_MASK		(1 << I2C_EDID_UPD_ADB0_SHIFT)
#define I2C_EDID_UPD_ADB0_CLR		(0 << I2C_EDID_UPD_ADB0_SHIFT)
#define I2C_EDID_UPD_ADB0_SET		(1 << I2C_EDID_UPD_ADB0_SHIFT)

#define I2C_EDID_UPD_ADB1_SHIFT		(4)
#define I2C_EDID_UPD_ADB1_MASK		(1 << I2C_EDID_UPD_ADB1_SHIFT)
#define I2C_EDID_UPD_ADB1_CLR		(0 << I2C_EDID_UPD_ADB1_SHIFT)
#define I2C_EDID_UPD_ADB1_SET		(1 << I2C_EDID_UPD_ADB1_SHIFT)

#define I2C_EDID_UPD_LATENCY_SHIFT	(5)
#define I2C_EDID_UPD_LATENCY_MASK	(1 << I2C_EDID_UPD_LATENCY_SHIFT)
#define I2C_EDID_UPD_LATENCY_CLR	(0 << I2C_EDID_UPD_LATENCY_SHIFT)
#define I2C_EDID_UPD_LATENCY_SET	(1 << I2C_EDID_UPD_LATENCY_SHIFT)
/***********  Reserved 0xC0 ~ 0xCF ***********/
/***********  CEC Fire Command Data 0xD0 ~ 0xDF ***********/
/***********  CEC Latch Command Data 0xE0 ~ 0xEF ***********/
/***********  HDMI Rx Select 0xF0 ***********/
#define I2C_HDMI_SELECT_SHIFT	(0)
#define I2C_HDMI_SELECT_MASK	(3 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_R0		(0 << I2C_HDMI_SELECT_SHIFT)
#define I2C_HDMI_SELECT_R1		(1 << I2C_HDMI_SELECT_SHIFT)
// (2) ~ (3) reserved
#define I2C_HDMI_ADO_SEL_SHIFT	(4)
#define I2C_HDMI_ADO_SEL_MASK	(3 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_R0		(0 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_R1		(1 << I2C_HDMI_ADO_SEL_SHIFT)
#define I2C_HDMI_ADO_SEL_NONE	(0xF << I2C_HDMI_ADO_SEL_SHIFT)
// (5) ~ (6) reserved
/***********  Audio Mode 0xF1 ***********/
#define I2C_MODE_POWER_SHFIT	(0)
#define I2C_MODE_POWER_MASK		(3 << I2C_MODE_POWER_SHFIT)
#define I2C_MODE_POWER_DOWN		(0 << I2C_MODE_POWER_SHFIT)
#define I2C_MODE_POWER_ON		(1 << I2C_MODE_POWER_SHFIT)
#define I2C_MODE_POWER_STANDBY	(2 << I2C_MODE_POWER_SHFIT)
// (2) reserved
// (2) reserved
#define I2C_MODE_EASTECH_FACTORY_SHIFT	(2)
#define I2C_MODE_EASTECH_FACTORY_MASK	(1 << I2C_MODE_EASTECH_FACTORY_SHIFT)
#define I2C_MODE_EASTECH_FACTORY_CLR	(0 << I2C_MODE_EASTECH_FACTORY_SHIFT)
#define I2C_MODE_EASTECH_FACTORY_SET	(1 << I2C_MODE_EASTECH_FACTORY_SHIFT)

#define I2C_MODE_ADO_SYS_SHIFT	(3)
#define I2C_MODE_ADO_SYS_MASK	(1 << I2C_MODE_ADO_SYS_SHIFT)
#define I2C_MODE_ADO_SYS_DIS	(0 << I2C_MODE_ADO_SYS_SHIFT)
#define I2C_MODE_ADO_SYS_EN		(1 << I2C_MODE_ADO_SYS_SHIFT)

#define I2C_MODE_EARC_SHIFT		(4)
#define I2C_MODE_EARC_MASK		(1 << I2C_MODE_EARC_SHIFT)
#define I2C_MODE_EARC_DIS		(0 << I2C_MODE_EARC_SHIFT)
#define I2C_MODE_EARC_EN		(1 << I2C_MODE_EARC_SHIFT)

#define I2C_MODE_CEC_SHIFT		(5)
#define I2C_MODE_CEC_MASK		(1 << I2C_MODE_CEC_SHIFT)
#define I2C_MODE_CEC_DIS		(0 << I2C_MODE_CEC_SHIFT)
#define I2C_MODE_CEC_EN			(1 << I2C_MODE_CEC_SHIFT)

#define I2C_MODE_TX_ADO_SHIFT	(6)
#define I2C_MODE_TX_ADO_MASK	(1 << I2C_MODE_TX_ADO_SHIFT)
#define I2C_MODE_TX_ADO_EN		(0 << I2C_MODE_TX_ADO_SHIFT)
#define I2C_MODE_TX_ADO_DIS		(1 << I2C_MODE_TX_ADO_SHIFT)

#define I2C_MODE_TX_OUT_SHIFT	(7)
#define I2C_MODE_TX_OUT_MASK	(1 << I2C_MODE_TX_OUT_SHIFT)
#define I2C_MODE_TX_OUT_EN		(0 << I2C_MODE_TX_OUT_SHIFT)
#define I2C_MODE_TX_OUT_DIS		(1 << I2C_MODE_TX_OUT_SHIFT)
/***********  TV Status 0xF2 ***********/
#define I2C_TV_STA_HPD_SHIFT		(0)
#define I2C_TV_STA_HPD_MASK			(1 << I2C_TV_STA_HPD_SHIFT)
#define I2C_TV_STA_HPD_CLR			(0 << I2C_TV_STA_HPD_SHIFT)
#define I2C_TV_STA_HPD_SET			(1 << I2C_TV_STA_HPD_SHIFT)

#define I2C_TV_STA_CEC_SHIFT		(1)
#define I2C_TV_STA_CEC_MASK			(1 << I2C_TV_STA_CEC_SHIFT)
#define I2C_TV_STA_CEC_OFF			(0 << I2C_TV_STA_CEC_SHIFT)
#define I2C_TV_STA_CEC_ON			(1 << I2C_TV_STA_CEC_SHIFT)

#define I2C_TV_STA_REQ_SWITCH_SHIFT		(2)
#define I2C_TV_STA_REQ_SWITCH_MASK		(3 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_TV  		(3 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_R0		(0 << I2C_TV_STA_REQ_SWITCH_SHIFT)
#define I2C_TV_STA_REQ_SWITCH_R1		(1 << I2C_TV_STA_REQ_SWITCH_SHIFT)

#define I2C_TV_STA_REQ_ADO_SHIFT	(4)
#define I2C_TV_STA_REQ_ADO_MASK		(1 << I2C_TV_STA_REQ_ADO_SHIFT)
#define I2C_TV_STA_REQ_ADO_CLR		(0 << I2C_TV_STA_REQ_ADO_SHIFT)
#define I2C_TV_STA_REQ_ADO_SET		(1 << I2C_TV_STA_REQ_ADO_SHIFT)
//(5)~(7) reserved
/***********  CEC Command Received 0xF3 ***********/
#define I2C_CEC_CMD_STANDBY_SHIFT	(0)
#define I2C_CEC_CMD_STANDBY_MASK	(1 << I2C_CEC_CMD_STANDBY_SHIFT)
#define I2C_CEC_CMD_STANDBY_CLR		(0 << I2C_CEC_CMD_STANDBY_SHIFT)
#define I2C_CEC_CMD_STANDBY_SET		(1 << I2C_CEC_CMD_STANDBY_SHIFT)

#define I2C_CEC_CMD_SYS_ADO_OFF_SHIFT	(1)
#define I2C_CEC_CMD_SYS_ADO_OFF_MASK	(1 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_OFF_CLR		(0 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_OFF_SET		(1 << I2C_CEC_CMD_SYS_ADO_OFF_SHIFT)
//(2) ~ (3) reserved
#define I2C_CEC_CMD_ACTIVE_SRC_SHIFT	(4)
#define I2C_CEC_CMD_ACTIVE_SRC_MASK		(1 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)
#define I2C_CEC_CMD_ACTIVE_SRC_CLR		(0 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)
#define I2C_CEC_CMD_ACTIVE_SRC_SET		(1 << I2C_CEC_CMD_ACTIVE_SRC_SHIFT)

#define I2C_CEC_CMD_SYS_ADO_ON_SHIFT	(5)
#define I2C_CEC_CMD_SYS_ADO_ON_MASK		(1 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_ON_CLR		(0 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)
#define I2C_CEC_CMD_SYS_ADO_ON_SET		(1 << I2C_CEC_CMD_SYS_ADO_ON_SHIFT)

#define I2C_CEC_CMD_LATCH_SHIFT			(6)
#define I2C_CEC_CMD_LATCH_MASK			(1 << I2C_CEC_CMD_LATCH_SHIFT)
#define I2C_CEC_CMD_LATCH_CLR			(0 << I2C_CEC_CMD_LATCH_SHIFT)
#define I2C_CEC_CMD_LATCH_SET			(1 << I2C_CEC_CMD_LATCH_SHIFT)

#define I2C_CEC_CMD_ROUT_CHG_SHIFT		(7)
#define I2C_CEC_CMD_ROUT_CHG_MASK		(1 << I2C_CEC_CMD_ROUT_CHG_SHIFT)
#define I2C_CEC_CMD_ROUT_CHG_CLR		(0 << I2C_CEC_CMD_ROUT_CHG_SHIFT)
#define I2C_CEC_CMD_ROUT_CHG_SET		(1 << I2C_CEC_CMD_ROUT_CHG_SHIFT)
/***********  CEC Command Received 2 0xF4 ***********/
#define I2C_CEC_CMD_POWER_SHIFT			(0)
#define I2C_CEC_CMD_POWER_MASK			(1 << I2C_CEC_CMD_POWER_SHIFT)
#define I2C_CEC_CMD_POWER_CLR			(0 << I2C_CEC_CMD_POWER_SHIFT)
#define I2C_CEC_CMD_POWER_SET			(1 << I2C_CEC_CMD_POWER_SHIFT)

#define I2C_CEC_CMD_POWER_ON_SHIFT		(1)
#define I2C_CEC_CMD_POWER_ON_MASK		(1 << I2C_CEC_CMD_POWER_ON_SHIFT)
#define I2C_CEC_CMD_POWER_ON_CLR		(0 << I2C_CEC_CMD_POWER_ON_SHIFT)
#define I2C_CEC_CMD_POWER_ON_SET		(1 << I2C_CEC_CMD_POWER_ON_SHIFT)

#define I2C_CEC_CMD_POWER_OFF_SHIFT		(2)
#define I2C_CEC_CMD_POWER_OFF_MASK		(1 << I2C_CEC_CMD_POWER_OFF_SHIFT)
#define I2C_CEC_CMD_POWER_OFF_CLR		(0 << I2C_CEC_CMD_POWER_OFF_SHIFT)
#define I2C_CEC_CMD_POWER_OFF_SET		(1 << I2C_CEC_CMD_POWER_OFF_SHIFT)
// (3) ~ (7) reserved
/***********  HDMI Rx Status 0xF5 ***********/
#define I2C_R0_STA_SHIFT		(0)
#define I2C_R0_STA_MASK			(3 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_NO			(0 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_5V			(1 << I2C_R0_STA_SHIFT)
#define I2C_R0_STA_IN			(2 << I2C_R0_STA_SHIFT)

#define I2C_R1_STA_SHIFT		(2)
#define I2C_R1_STA_MASK			(3 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_NO			(0 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_5V			(1 << I2C_R1_STA_SHIFT)
#define I2C_R1_STA_IN			(2 << I2C_R1_STA_SHIFT)
// (4) ~ (7) reserved
/***********  HDMI Mode Rx0 0xF6 ***********/
#define I2C_RX0_EDID_SHIFT		(0)
#define I2C_RX0_EDID_MASK		(1 << I2C_RX0_EDID_SHIFT)
#define I2C_RX0_EDID_TV			(0 << I2C_RX0_EDID_SHIFT)
#define I2C_RX0_EDID_SB			(1 << I2C_RX0_EDID_SHIFT)

#define I2C_RX0_HDCP_VER_SHIFT	(1)
#define I2C_RX0_HDCP_VER_MASK	(1 << I2C_RX0_HDCP_VER_SHIFT)
#define I2C_RX0_HDCP_VER_AUTO	(0 << I2C_RX0_HDCP_VER_SHIFT)
#define I2C_RX0_HDCP_VER_23		(1 << I2C_RX0_HDCP_VER_SHIFT)

#define I2C_RX0_HDCP_RPT_SHIFT	(2)
#define I2C_RX0_HDCP_RPT_MASK	(1 << I2C_RX0_HDCP_RPT_SHIFT)
#define I2C_RX0_HDCP_RPT_CLR	(0 << I2C_RX0_HDCP_RPT_SHIFT)
#define I2C_RX0_HDCP_RPT_SET	(1 << I2C_RX0_HDCP_RPT_SHIFT)
// (3) ~ (6)reserved
#define I2C_RX0_ACTIVE_SHIFT	(7)
#define I2C_RX0_ACTIVE_MASK		(1 << I2C_RX0_ACTIVE_SHIFT)
#define I2C_RX0_ACTIVE_BY_SEL	(0 << I2C_RX0_ACTIVE_SHIFT)
#define I2C_RX0_ACTIVE_FORCE	(1 << I2C_RX0_ACTIVE_SHIFT)

/***********  HDMI Mode Rx1 0xF7 ***********/
#define I2C_RX1_EDID_SHIFT		(0)
#define I2C_RX1_EDID_MASK		(1 << I2C_RX1_EDID_SHIFT)
#define I2C_RX1_EDID_TV			(0 << I2C_RX1_EDID_SHIFT)
#define I2C_RX1_EDID_SB			(1 << I2C_RX1_EDID_SHIFT)

#define I2C_RX1_HDCP_VER_SHIFT	(1)
#define I2C_RX1_HDCP_VER_MASK	(1 << I2C_RX1_HDCP_VER_SHIFT)
#define I2C_RX1_HDCP_VER_AUTO	(0 << I2C_RX1_HDCP_VER_SHIFT)
#define I2C_RX1_HDCP_VER_23		(1 << I2C_RX1_HDCP_VER_SHIFT)

#define I2C_RX1_HDCP_RPT_SHIFT	(2)
#define I2C_RX1_HDCP_RPT_MASK	(1 << I2C_RX1_HDCP_RPT_SHIFT)
#define I2C_RX1_HDCP_RPT_CLR	(0 << I2C_RX1_HDCP_RPT_SHIFT)
#define I2C_RX1_HDCP_RPT_SET	(1 << I2C_RX1_HDCP_RPT_SHIFT)
// (3) ~ (6)reserved
#define I2C_RX1_ACTIVE_SHIFT	(7)
#define I2C_RX1_ACTIVE_MASK		(1 << I2C_RX1_ACTIVE_SHIFT)
#define I2C_RX1_ACTIVE_BY_SEL	(0 << I2C_RX1_ACTIVE_SHIFT)
#define I2C_RX1_ACTIVE_FORCE	(1 << I2C_RX1_ACTIVE_SHIFT)
/***********  Reserved 0xF8~ 0xFB ***********/
/***********  CEC Transmit Count 0xFC ***********/
/***********  CEC Received Count 0xFD ***********/
/***********  CEC Command Latch OP Code 0xFD ***********/
/***********  Host Update 0xFE ***********/
// (0) reserved
#define I2C_UPD_ADO_SHIFT		(1)
#define I2C_UPD_ADO_MASK		(1 << I2C_UPD_ADO_SHIFT)
#define I2C_UPD_ADO_SET			(1 << I2C_UPD_ADO_SHIFT)
#define I2C_UPD_ADO_CLR			(0)
// (2) reserved
#define I2C_UPD_BLK_SHIFT		(3)
#define I2C_UPD_BLK_MASK		(3 << I2C_UPD_BLK_SHIFT)
#define I2C_UPD_BLK_DONE		(0 << I2C_UPD_BLK_SHIFT)
#define I2C_UPD_BLK_SB_EDID		(1 << I2C_UPD_BLK_SHIFT)
#define I2C_UPD_BLK_EARC_0		(2 << I2C_UPD_BLK_SHIFT)
#define I2C_UPD_BLK_EARC_1		(3 << I2C_UPD_BLK_SHIFT)
// (5) reserved
#define I2C_UPD_CEC_FIRE_SHIFT	(6)
#define I2C_UPD_CEC_FIRE_MASK	(1 << I2C_UPD_CEC_FIRE_SHIFT)
#define I2C_UPD_CEC_FIRE_CLR	(0 << I2C_UPD_CEC_FIRE_SHIFT)
#define I2C_UPD_CEC_FIRE_SET	(1 << I2C_UPD_CEC_FIRE_SHIFT)

#define I2C_UPD_SYS_SHIFT		(7)
#define I2C_UPD_SYS_MASK		(1 << I2C_UPD_SYS_SHIFT)
#define I2C_UPD_SYS_CLR			(0 << I2C_UPD_SYS_SHIFT)
#define I2C_UPD_SYS_SET			(1 << I2C_UPD_SYS_SHIFT)
/***********  ITE Interrupt 0xFF ***********/
#define I2C_INT_READY_SHIFT		(0)
#define I2C_INT_READY_MASK		(1 << I2C_INT_READY_SHIFT)
#define I2C_INT_READY_CLR		(0 << I2C_INT_READY_SHIFT)
#define I2C_INT_READY_SET		(1 << I2C_INT_READY_SHIFT)

#define I2C_INT_AUDIO_SHIFT		(1)
#define I2C_INT_AUDIO_MASK		(1 << I2C_INT_AUDIO_SHIFT)
#define I2C_INT_AUDIO_CLR		(0 << I2C_INT_AUDIO_SHIFT)
#define I2C_INT_AUDIO_SET		(1 << I2C_INT_AUDIO_SHIFT)
// (2) reserved
#define I2C_INT_TV_LATENCY_SHIFT	(3)
#define I2C_INT_TV_LATENCY_MASK		(1 << I2C_INT_TV_LATENCY_SHIFT)
#define I2C_INT_TV_LATENCY_CLR		(0 << I2C_INT_TV_LATENCY_SHIFT)
#define I2C_INT_TV_LATENCY_SET		(1 << I2C_INT_TV_LATENCY_SHIFT)

// (4) ~ (5) reserved
#define I2C_INT_XIAOMI_CEC_GOT_SHIFT	(5)
#define I2C_INT_XIAOMI_CEC_GOT_MASK	    (1 << I2C_INT_XIAOMI_CEC_GOT_SHIFT)
#define I2C_INT_XIAOMI_CEC_GOT_CLR		(0 << I2C_INT_XIAOMI_CEC_GOT_SHIFT)
#define I2C_INT_XIAOMI_CEC_GOT_SET		(1 << I2C_INT_XIAOMI_CEC_GOT_SHIFT)

#define I2C_INT_CEC_GOT_SHIFT	(6)
#define I2C_INT_CEC_GOT_MASK	(1 << I2C_INT_CEC_GOT_SHIFT)
#define I2C_INT_CEC_GOT_CLR		(0 << I2C_INT_CEC_GOT_SHIFT)
#define I2C_INT_CEC_GOT_SET		(1 << I2C_INT_CEC_GOT_SHIFT)

#define I2C_INT_SYS_SHIFT		(7)
#define I2C_INT_SYS_MASK		(1 << I2C_INT_SYS_SHIFT)
#define I2C_INT_SYS_CLR			(0 << I2C_INT_SYS_SHIFT)
#define I2C_INT_SYS_SET			(1 << I2C_INT_SYS_SHIFT)

#define MODE_CHG_SB_EDID	(1 << 0)		//	[0], 0xFE[3], EDID update
#define MODE_CHG_ADO		(1 << 1)		//	[1], 0xF1, Audio mode update
#define MODE_CHG_RX_SEL		(1 << 2)		//	[2], 0xF0, Rx Port Select
#define MODE_CHG_RX_EDID	(1 << 3)		//	[3], 0xF5, Rx Edid mode change
#define MODE_CHG_R0			(1 << 4)		//	[4], 0xF6, Rx0 Mode change
#define MODE_CHG_R1			(1 << 5)		//	[5], 0xF7, Rx1 Mode change
//	[6], 0xF8
//	[7], 0xF9

#define EDID_UPDATE_VPID		(1 << 0)
#define EDID_UPDATE_PNAME		(1 << 1)
#define EDID_UPDATE_SPK_ADB0	(1 << 3)
#define EDID_UPDATE_ADB1		(1 << 4)
#define EDID_UPDATE_LATENCY		(1 << 5)



#define ADO_CONF_ACTIVE_SHIFT					(26)
#define ADO_CONF_ACTIVE_MASK					(0x01 << ADO_CONF_ACTIVE_SHIFT)
#define ADO_CONF_ACTIVE_SET					(0x01 << ADO_CONF_ACTIVE_SHIFT)
#define ADO_CONF_ACTIVE_CLR					(0x00)

// AudSel
#define ADO_CONF_SEL_SHIFT						(10)
#define ADO_CONF_SEL_MASK						(0x01 << ADO_CONF_SEL_SHIFT)
#define ADO_CONF_SEL_I2S						(0x00 << ADO_CONF_SEL_SHIFT)
#define ADO_CONF_SEL_SPDIF						(0x01 << ADO_CONF_SEL_SHIFT)

// AudCh
#define ADO_CONF_CH_SHIFT						(0)
#define ADO_CONF_CH_MASK						(0x0F << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_0							(0x00 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_2							(0x02 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_3							(0x03 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_4							(0x04 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_5							(0x05 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_6							(0x06 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_7							(0x07 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_8							(0x08 << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_10							(0x0A << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_12							(0x0C << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_16							(0x0D << ADO_CONF_CH_SHIFT)
#define ADO_CONF_CH_32							(0x0E << ADO_CONF_CH_SHIFT)

// Audio input without Information
#define ADO_ENC_NO_INFO_SHIFT				(1)
#define ADO_ENC_NO_INFO_MASK				(0x01 << ADO_ENC_NO_INFO_SHIFT)
#define ADO_ENC_NO_INFO_SET					(0x01 << ADO_ENC_NO_INFO_SHIFT)
#define ADO_ENC_NO_INFO_CLR					(0x00)

// EnMCLKSample, TRUE for HBR via SPDIF
// FOR SPDIF IN SUPPORT HBR and ADO_ENC_NO_INFO_SET
#define ADO_ENC_SPDIF_MCLK_IN_SHIFT			(2)
#define ADO_ENC_SPDIF_MCLK_IN_MASK			(0x01 << ADO_ENC_SPDIF_MCLK_IN_SHIFT)
#define ADO_ENC_SPDIF_MCLK_IN_SET			(0x01 << ADO_ENC_SPDIF_MCLK_IN_SHIFT)
#define ADO_ENC_SPDIF_MCLK_IN_CLR			(0x00)

#define ADO_ENC_SPDIF_CHSTA_CHK_SHIFT		(3)
#define ADO_ENC_SPDIF_CHSTA_CHK_MASK		(0x01 << ADO_ENC_SPDIF_CHSTA_CHK_SHIFT)
#define ADO_ENC_SPDIF_CHSTA_CHK_SET			(0x01 << ADO_ENC_SPDIF_CHSTA_CHK_SHIFT)
#define ADO_ENC_SPDIF_CHSTA_CHK_CLR			(0x00)


// AudType
#define ADO_CONF_TYPE_SHIFT					(11)
#define ADO_CONF_TYPE_MASK					(0x03 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_LPCM					(0x00 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_NLPCM					(0x01 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_HBR						(0x02 << ADO_CONF_TYPE_SHIFT)
#define ADO_CONF_TYPE_DSD						(0x03 << ADO_CONF_TYPE_SHIFT)


// EnDV3D
#define ADO_CONF_LAYOUT_SHIFT				(24)
#define ADO_CONF_LAYOUT_MASK				(0x01 << ADO_CONF_LAYOUT_SHIFT)
#define ADO_CONF_LAYOUT_0					(0x00)
#define ADO_CONF_LAYOUT_1					(0x01 << ADO_CONF_LAYOUT_SHIFT)

//****************************************************************************
#define I2C_STA_CHANGE(a, b, c)		do{		(a) = ((a) & (~(b))) | ((c) & (b));	}while(0)
#define I2C_ITE_CHIP			(0x00)
#define I2C_FW_MAJOR_VERSION	(0x03)
#define I2C_FW_MINOR_VERSION	(0x06)
#define I2C_MCU_POWER_FIRST_STA	(0x0F)
#define I2C_ADO_SRC_STA			(0x10)
#define I2C_ADO_VOL				(0x11)
//#define I2C_ARC_VOL_W			(0x12)
#define I2C_VDO_LATENCY			(0x13)
#define I2C_LATENCY_FLAGS		(0x14)
#define I2C_ADO_OUTPUT_DELAY	(0x15)
#define I2C_ADO_INFO			(0x16)
#define I2C_ADO_CA				(0x1A)
#define I2C_VOLUME_MAX			(0x1B)
#define I2C_ADO_SEL				(0x1C)
#define I2C_MCLK_FREQ_SEL		(0x1D)
#define I2C_ADO_INFOPKT			(0x1E)
#define I2C_ADO_CHSTA			(0x24)
#define I2C_EDID_UPDATE			(0x40)
#define I2C_EDID_VPID			(0x41)
#define I2C_EDID_PNAME			(0x4B)
#define I2C_EDID_PDESC			(0x5D)
#define I2C_EDID_SPK_ALLOC		(0x6F)
#define I2C_EDID_ADB0			(0x73)
#define I2C_EDID_ADB1			(0x92)
#define I2C_EDID_V_LAT			(0xB1)
#define I2C_EDID_A_LAT			(0xB2)
#define I2C_EDID_IV_LAT			(0xB3)
#define I2C_EDID_IA_LAT			(0xB4)
#define I2C_TV_V_LAT			(0xBC)
#define I2C_TV_A_LAT			(0xBD)
#define I2C_TV_IV_LAT			(0xBE)
#define I2C_TV_IA_LAT			(0xBF)
#define I2C_SOUND_MODE   		(0xC0)
#define I2C_SOUND_SOURCE  		(0xC1)
#define I2C_CEC_TRANS_DATA		(0xD0)
#define I2C_CEC_LATCH_DATA		(0xE0)
#define I2C_SYS_RX_SEL			(0xF0)
#define I2C_SYS_ADO_MODE		(0xF1)
#define I2C_SYS_TX_STA			(0xF2)
#define I2C_SYS_CEC_RECEIVE		(0xF3)
#define I2C_SYS_CEC_RECEIVE2	(0xF4)
#define I2C_SYS_RX_STATUS		(0xF5)
#define I2C_SYS_RX_MODE_0		(0xF6)
#define I2C_SYS_RX_MODE_1		(0xF7)
#define I2C_SYS_RX_MODE_2		(0xF8)
#define I2C_SYS_RX_MODE_3		(0xF9)
#define I2C_SYS_CEC_TRANS_CNT	(0xFC)
#define I2C_SYS_CEC_LATCH_CNT	(0xFD)
#define I2C_SYS_CHANGE			(0xFE)
#define I2C_SYS_INT				(0xFF)
//****************************************************************************
//****************************************************************************
#if 0
typedef enum {

    CMD_POWER_STANDBY = 0,
	CMD_PASSTHROUGH,
    CMD_POWER_ON,
    CMD_POWER_OFF,
    CMD_DUMP_I2C,
    CMD_VOLUME_MUTE,
    CMD_VOLUME_UP,
    CMD_VOLUME_DOWN,
 	CMD_MODE_ARC_EARC,
  	CMD_MODE_BT,
 	CMD_MODE_AUX_IN,				
  	CMD_MODE_USB,							
 	CMD_MODE_COAX,						
	CMD_MODE_OPT,

} hdmi_host_cmd_t;
#endif
//****************************************************************************
//****************************************************************************
//****************************************************************************
#define I2C_STA_CHANGE(a, b, c)		do{		(a) = ((a) & (~(b))) | ((c) & (b));	}while(0)
	


#endif


