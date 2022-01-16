/*************************************************************************************************
 * @file mcu_if_sp.h
 * @brief To communicate with MCU by Sunplus packet protocol
 * @note Copyright (c) 2020 Sunplus Technology Co., Ltd. All rights reserved.
 *
 * @author zy.bai@sunmedia.com.cn
 * @version v0.01
 * @date 2020/12/25
 *************************************************************************************************/
#ifndef _MCU_IF_SP_H_
#define _MCU_IF_SP_H_

#include "qp_pub.h"

// #define MCU_MASTER

typedef struct {
    uint8_t sflag;
    uint8_t devId;
    uint8_t cmd;
    uint8_t subCmd;
    uint16_t payloadLen;
} PacketHeader_t;

typedef struct
{
	QHsm super;
	QActive *superAO;
	QTimeEvt waitAck;
	int mcuif_id;
	int baud;
	struct {
		PacketHeader_t header;
		UINT8 *payload;
		UINT8 retries;
	} CmdSent;
#ifdef	MCU_MASTER
	INT32 sdfd;
	UINT8 *asbpucUSBBuffer;
	char *pbUpgFilePath;
	int read_offset;
	UINT32 read_len;
#endif
} stMCUIF_SP_t;

typedef struct
{
	QEvt super;
    PacketHeader_t *header;
	uint8_t *payload;
    void (*cb)(void *arg);
} MCUIF_SP_Evt_t;

typedef struct
{
	int src;
} stMCU_IF_Src_t;

typedef struct
{
	QEvt super;
	stMCU_IF_Src_t src_data;
} MCUIF_Src_Evt_t;

typedef struct
{
	uint8_t channel_ID;
	uint8_t volume;
} stMCU_IF_Vol_t;

typedef struct
{
	QEvt super;
	stMCU_IF_Vol_t vol_data;
} MCUIF_Vol_Evt_t;

typedef struct
{
	UINT32 baud;
} stMCU_IF_Baud_t;

typedef struct
{
	UINT32 offset;
	UINT32 len;
} stUpgrade_Ack_t;

typedef struct
{
	int error_code;
	stUpgrade_Ack_t ack_payload;
} stResponse_UpgrdSrv_t;


extern void MCUIF_SP_ctor(stMCUIF_SP_t *const me, QActive *superAO);

#if(0)
#define CMD_FWUPG_START (0x40)
#define CMD_FWUPG_SEND_DATA (0x41)
#endif

#define CMD_SYS_UPGRD (0x40)
#define CMD_ISP_UPGRD (0x41)
#define CMD_SEND_DATA_START (0x01)
#define CMD_SEND_DATA (0x02)

#define CMD_ACK (0x5b)
#define CMD_NACK (0x5c)

#define CMD_SP_READY (0x50)
#define CMD_TICK (0x5e)
#define CMD_VOL (0x62)
#define CMD_SRC (0x63)
#define CMD_AUD_CTL (0x64)
#define CMD_I2S_CFG (0x65)
#define CMD_DSP_CTL (0x66)
#define CMD_ATMOS_CFG (0x67)
#define CMD_EQ_FILTER (0x68)
#define CMD_DEC_CFG (0x69)
#define CMD_MATRIX_CFG (0x6a)
#define CMD_BAUD (0x6b)

#define CMD_SEND_AUDINFO (0x70)

void MCUIF_SP_ctor(stMCUIF_SP_t *const me, QActive *superAO);

// api
void MCUIF_SP_set_baud(stMCUIF_SP_t *me, UINT8 USER_BAUD);

// api for master only
#ifdef MCU_MASTER
void MCUIF_SP_set_vol(stMCUIF_SP_t *me, UINT8 vol_int);
void MCUIF_SP_set_src(stMCUIF_SP_t *me, UINT8 USER_SRC);
int MCUIF_SP_upgrade(stMCUIF_SP_t *me, char *pbUpgFilePath);
int MCUIF_SP_upgrade_isp(stMCUIF_SP_t *me, char *pbUpgFilePath);
#endif

enum samplerate {
	SAMPLERATE_8K = 0,
	SAMPLERATE_11_025K,
	SAMPLERATE_12K,
	SAMPLERATE_16K,
	SAMPLERATE_22_05K,
	SAMPLERATE_24K,
	SAMPLERATE_32K,
	SAMPLERATE_44_1K,
	SAMPLERATE_48K,
	SAMPLERATE_64K,
	SAMPLERATE_88_2K,
	SAMPLERATE_96K,
	SAMPLERATE_128K,
	SAMPLERATE_176_4K,
	SAMPLERATE_192K,
	SAMPLERATE_MAX,
};

#if 0
enum audio_format {
	FORMAT_NONE = 0,
	FORMAT_PCM,
	FORMAT_DOLBY_DIGITAL,
	FORMAT_DTS,
	FORMAT_HDCD,
	FORMAT_MP3,
	FORMAT_MPEG2,
	FORMAT_AAC_ADTS,
	FORMAT_DOLBY_DIGITAL_EX,
	FORMAT_DTS_ES_D61,
	FORMAT_DTS_ES_M61,
	FORMAT_PCM_MCH,
	FORMAT_DD_PLUS,
	FORMAT_DD_TRUEHD,
	FORMAT_DTS_HIRES,
	FORMAT_DTS_LBR,
	FORMAT_DSD,
	FORMAT_WMA_PRO,
	FORMAT_MAT,
	FORMAT_DTS_9624,
	FORMAT_DTS_MA,
	FORMAT_DTS_ES_D8,
	FORMAT_DTS_LOSSLESS,
	FORMAT_DTS_UHD,
	FORMAT_DTS_UHD_MA,
	FORMAT_DTS_UHD_GAME,
	FORMAT_MAX
};
#endif

enum dolby_audio_format {
	DOLBY_SURROUND = 0x4000,
	DOLBY_ATMOS = 0x8000,
};

enum i2s_cfg_type {
	I2S_CFG_TYPE_DELAY = 0,
	I2S_CFG_TYPE_JUSTIFY,
	I2S_CFG_TYPE_DATAWIDTH,
	I2S_CFG_TYPE_PARITY,
	I2S_CFG_TYPE_LRCYCLE,
	I2S_CFG_TYPE_EDGE,
	I2S_CFG_TYPE_LRSEL,
	I2S_CFG_TYPE_SAMPLERATE,
};

enum dsp_ctl_type {
	DSP_CTL_SAMPLERATE = 1,
	DSP_CTL_DELAY,
	DSP_CTL_TYPE_MAX,
};

enum dsp_samplerate {
	DSP_SAMPLERATE_32K = 1,
	DSP_SAMPLERATE_44_1K,
	DSP_SAMPLERATE_48K,
	DSP_SAMPLERATE_64K,
	DSP_SAMPLERATE_88_2K,
	DSP_SAMPLERATE_96K,
	DSP_SAMPLERATE_128K,
	DSP_SAMPLERATE_176_4K,
	DSP_SAMPLERATE_192K,
	DSP_SAMPLERATE_MAX,
};

enum atmos_cfg_cmd {
	ATMOS_ENABLE_DAP = 0, /* Enable DAP */
	ATMOS_DIRECT_DEC, /* Direct Dec */
	ATMOS_DRC, /* DRC */
	ATMOS_LM, /* Loudness Management */
	ATMOS_CFG_FILE, /* configuration file */
	ATMOS_VIRT, /* Virtualization setting */
	ATMOS_TUNING_MODE, /* Tuning Mode */
	ATMOS_PROFILE, /* Profile */
	ATMOS_BE, /* Bass Enhancer */
	ATMOS_CB, /* Calibration Boost */
	ATMOS_DE, /* Dialog Enhancer */
	ATMOS_MISC, /* Media Intelligence Steer Control */
	ATMOS_GAIN, /* Gain */
	ATMOS_HCF, /* Height Cue Filter */
	ATMOS_SD, /* Surround Decoder */
	ATMOS_VL, /* Volume Leveler */
	ATMOS_GEQ, /* Graphic EQ */
	ATMOS_IEQ, /* Intelligent EQ */
};

enum eq_filter_cmd {
	EQ_FILTER_ENABLE = 1,
	EQ_FILTER_MODE,
	EQ_FILTER_SET,
	EQ_FILTER_SAVE
};

enum eq_filter_pram {
	EQ_FILTER_PARAM_TYPE = 0,
	EQ_FILTER_PARAM_FREQ,
	EQ_FILTER_PARAM_GAIN,
	EQ_FILTER_PARAM_Q,
	EQ_FILTER_PARAM_PHASE,
	EQ_FILTER_PARAM_SLOPE,
	EQ_FILTER_PARAM_METHOD,
	EQ_FILTER_PARAM_STAGE,
};

enum dec_cfg_cmd {
	DEC_OUT_CH = 0x01,
};

enum matrix_cfg_cmd {
	MATRIX_ENABLE = 0x01,
	MATRIX_MODE,
};

#endif // _MCU_IF_SP_H_

