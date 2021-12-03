/********************************************************
 * Copyright (c) 2020 by Sunplus Technology Co., Ltd.
 *    ____               __
 *   / __/_ _____  ___  / /_ _____
 *  _\ \/ // / _ \/ _ \/ / // (_-<
 * /___/\_,_/_//_/ .__/_/\_,_/___/
 *              /_/
 * Sunplus Technology Co. 19, Innovation First Road,
 * Science-Based Industrial Park, Hsin-Chu, Taiwan, R.O.C.
 *
 ********************************************************/

/**
 * This is for Zhong Shan City Richsound Electronic Industrial Ltd.
 * DSP Communicaton Interface Specification (CIS) Version 1.7
 */

#ifndef __MCU_IF_RSR_H__
#define __MCU_IF_RSR_H__

#include <qp_pub.h>

#define DSP_VER_M	0 /* DSP software main version code */
#define DSP_VER_S	1 /* DSP software sub version code */

#define BIT(n)		(1 << (n))
#define SYNC_BYTE	(0xff)

typedef struct
{
	QHsm super;
	QTimeEvt timeRSR200msEvt;
	QActive *superAO; /* the AO this orthogonal component resides in */
	int mcuif_id;
} stMCU_IF_RSR_t;

typedef struct
{
	QEvt super;
	uint8_t *data;
	uint8_t len;
} stMCU_IF_RSR_Evt;

/* Command IDs */
enum command_id {
	DSP_MAIN_SETUP = 0x02,
	SYSTEM_INFO = 0x03, /* Information from MCU */
	CHANNEL_DELAY_SETTING = 0x04,
	DSP_VOLUME = 0x05,
	DSP_GUI_SETTING = 0x06,
	DSP_BM_CTRL = 0x08, /* DSP Bass Management control */
	DSP_RECORD_MUSIC_PLAY = 0x09, /* MCU set DSP play record music at selected channel */
	DSP_RECORD_MUSIC_CONTROL = 0x0a, /* MCU set DSP record music control */
	DSP_DTS_SETTING = 0x0b, /* DTS related settings */
	SYS_OTA_CTRL = 0x0e, /* DSP receives OTA data from SPI interface and upgrades host MCU firmware by UART */

	DSP_RET_STATUS = 0x20, /* DSP informs MCU the system status */
	DSP_RET_AUD_INFO = 0x21, /* DSP returns audio information */
	DSP_RET_OTA_INFO = 0x23, /* DSP returns status and information */
};

enum dsp_input_src {
	INPUT_SRC_NONE = 0x00,
	INPUT_I2S_8CH_SLAVE = 0x01,
	INPUT_I2S_2CH_MASTER_48K = 0x02,
	INPUT_I2S_2CH_MASTER_44_1K = 0x03,
	INPUT_I2S_2CH_SLAVE = 0x04,
	INPUT_SPDIF = 0x06,
	INPUT_STDBY = 0x08,
	INPUT_DSP_RST = 0x0a,
	INPUT_UDISK_UPGRD = 0x0c,
};

enum ch_mix_mode {
	CH_MIX_2CH = 0X01,
	CH_MIX_MCH,
	CH_MIX_DOLBY_DAP,
	CH_MIX_DTS_NEOX,
	CH_MIX_DTS_VIRTX,
};

/* downmix/upmix channel */
#define MIX_CH_LR	BIT(0)
#define MIX_CH_C	BIT(1)
#define MIX_CH_LFE	BIT(2)
#define MIX_CH_LRS	BIT(3)
#define MIX_CH_LRRS	BIT(4)
#define MIX_CH_LRTF	BIT(5)
#define MIX_CH_LRTM	BIT(6)
#define MIX_CH_LRTR	BIT(7)

enum i2s_format {
	I2S_FORMAT_I2S = 0,
	I2S_FORMAT_LEFT_JUSTIFIED,
	I2S_FORMAT_RIGHT_JUSTIFIED,
};

/* I2S Slave Input type */
enum isi_type {
	ISI_TYPE_NONE = 0x01,
	ISI_TYPE_PCM_2CH,
	ISI_TYPE_PCM_MCH,
	ISI_TYPE_SD,
	ISI_TYPE_HBR,
};

/* I2S Slave Input samplerate */
enum isi_samplerate {
	ISI_SAMPLERATE_8K = 0x00,
	ISI_SAMPLERATE_11_025K,
	ISI_SAMPLERATE_12K,
	ISI_SAMPLERATE_16K,
	ISI_SAMPLERATE_22_05K,
	ISI_SAMPLERATE_24K,
	ISI_SAMPLERATE_32K,
	ISI_SAMPLERATE_44_1K,
	ISI_SAMPLERATE_48K,
	ISI_SAMPLERATE_64K,
	ISI_SAMPLERATE_88_2K,
	ISI_SAMPLERATE_96K,
	ISI_SAMPLERATE_128K,
	ISI_SAMPLERATE_176_4K,
	ISI_SAMPLERATE_192K,
};

#define GPIO_42	BIT(2)
#define GPIO_43	BIT(3)
#define GPIO_48	BIT(6)

/* Channel Delay Setting channel ID */
enum cds_ch {
	CDS_FL = 0,
	CDS_FR,
	CDS_C,
	CDS_LFE,
	CDS_LS,
	CDS_RS,
	CDS_LRS,
	CDS_RRS,
	CDS_LTF,
	CDS_RTF,
	CDS_LTR,
	CDS_RTR,
};

/* DSP Volume channel ID */
enum dv_ch {
	DV_FL = 0,
	DV_FR,
	DV_C,
	DV_LFE,
	DV_LS,
	DV_RS,
	DV_LRS,
	DV_RRS,
	DV_LTF,
	DV_RTF,
	DV_LTR,
	DV_RTR,
};

/* DSP Bass Management control */
enum bm_ctrl {
	BM_GAIN_LR = 0x00,
	BM_GAIN_C,
	BM_GAIN_LFE,
	BM_GAIN_LRS,
	BM_GAIN_LRRS,
	BM_GAIN_LRTF,
	BM_GAIN_LRTR,
	BM_GAIN_BM2LR,
	BM_GAIN_BM2SLR,
	BM_CUT_LR,
	BM_CUT_C,
	BM_CUT_LFE,
	BM_CUT_LRS,
	BM_CUT_LRRS,
	BM_CUT_LRTF,
	BM_CUT_LRTR,
};

enum dsp_sys_status {
	SYS_STATUS_BOOT = 0x01,
	SYS_STATUS_CHKEDFW,
	SYS_STATUS_UPGRDING,
	SYS_STATUS_UPGRDED,
	SYS_STATUS_WAITCMD,
	SYS_STATUS_RUNNING = 0x0a, /* debug only */
};

/* Command to be acknowleged */
#define ACK_DSP_BM_CTRL			BIT(0)
#define ACK_DSP_GUI_SETTING		BIT(1)
#define ACK_DSP_VOLUME			BIT(2)
#define ACK_CHANNEL_DELAY_SETTING	BIT(3)
#define ACK_SYSTEM_INFO			BIT(4)
#define ACK_DSP_MAIN_SETUP		BIT(5)

enum dsp_info_aud_format {
	DI_AUD_FMT_NONE = 0x00,
	DI_AUD_FMT_PCM,
	DI_AUD_FMT_DD, 	/* dolby Digital */
	DI_AUD_FMT_DTS,
	DI_AUD_FMT_HDCD,	//0x04
	DI_AUD_FMT_MP3,
	DI_AUD_FMT_MPEG2,
	DI_AUD_FMT_MPEG2_AAC_ADTS,	//0x07
	DI_AUD_FMT_DDE, /* Dolby Digital Ex*/
	DI_AUD_FMT_DTS_ES_MATRIX,	//0x09
	DI_AUD_FMT_DTS_ES_DISCRETE,	//0x0a
	DI_AUD_FMT_DTS_ES_8CH_DISCRETE,
	DI_AUD_FMT_PCM_MCH = 0x0c,
	DI_AUD_FMT_DDP,	/* Dolby Digital Plus */
	DI_AUD_FMT_DD_TRUEHD,	/* Dolby Digital TrueHD */
	DI_AUD_FMT_DTS_MASTER,
	DI_AUD_FMT_DTS_HIRES = 0x10,	/* DTS High Resolution */
	DI_AUD_FMT_DTS_LBR,	/* DTS Low BitRate */
	DI_AUD_FMT_DSD,
	DI_AUD_FMT_WMA_PRO,
	DI_AUD_FMT_MAT,
	/* DTSX(0x15) Reserved */
	DI_AUD_FMT_DTS_9624 = 0x16,		//0x16
	DI_AUD_FMT_DTS_LOSSLESS,
	DI_AUD_FMT_DTS_UHD,
	DI_AUD_FMT_DTS_UHD_MASTER,
	DI_AUD_FMT_DTS_UHD_LOSSLESS,	//0x1a
	DI_AUD_FMT_DTS_UHD_GAME,
};


enum dsp_info_samplerate {
	DI_SAMPLERATE_8K = 0x00,
	DI_SAMPLERATE_11_025K,
	DI_SAMPLERATE_12K,
	DI_SAMPLERATE_16K,
	DI_SAMPLERATE_22_05K,
	DI_SAMPLERATE_24K,
	DI_SAMPLERATE_32K,
	DI_SAMPLERATE_44_1K,
	DI_SAMPLERATE_48K,
	DI_SAMPLERATE_64K,
	DI_SAMPLERATE_88_2K,
	DI_SAMPLERATE_96K,
	DI_SAMPLERATE_128K,
	DI_SAMPLERATE_176_4K,
	DI_SAMPLERATE_192K,
};

enum record_music_ctrl {
	RECORD_CTRL_START = 0x01,
	RECORD_CTRL_STOP,
	RECORD_CTRL_EXPORT,
	RECORD_CTRL_IMPORT,
	RECORD_CTRL_STATUS = 0x20,
};

enum sys_ota_ctrl {
	OTA_CTRL_START = 0x01,
	OTA_CTRL_EXIT = 0x03,
	OTA_CTRL_UPG_S8137 = 0x05,
	OTA_CTRL_UPG_MCU = 0x06,
	OTA_CTRL_ACK_SPI_MSG = 0x23,
};

enum ota_info_id {
	OTA_INFO_RCV_LEN = 0x01,
	OTA_INFO_RCV_FAIL = 0x02,
	OTA_INFO_RCV_OK = 0x03,
	OTA_INFO_EXIT_UPG = 0x04,
	OTA_INFO_UPG_S8137 = 0x05,
	OTA_INFO_PASS_MSG = 0x23,
};

enum dsp_app_table_group{
	DATG_NO_SELECT = 0x00,
	DATG_MATRIX1 = 0x01,
	DATG_MATRIX2 = 0x02,
	DATG_FILTER1 = 0x03,
	DATG_DRC = 0x04,
	DATG_VIRTUALX = 0x05,
	DATG_FILTER2 = 0x06,
	DATG_FILTER3 = 0x07,
	DATG_MATRIX3 = 0x08,
	DATG_BM = 0x09,
};

extern void MCU_IF_RSR_ctor(stMCU_IF_RSR_t *const me, QActive *superAO);

#endif /* __MCU_IF_RSR_H__ */
