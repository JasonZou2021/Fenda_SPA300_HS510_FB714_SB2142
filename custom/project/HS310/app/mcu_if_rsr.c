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
 * DSP Communicaton Interface Specification (CIS) Version 1.9
 */

#include <string.h>
#include <types.h>
#include <math.h>
#include "userapp.h"
#define LOG_EXTRA_STR "[MCU_IF_RSR]"
#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"
#include "mcu_interface.h"
#include "AudDspService.h"
#include "mcu_if_rsr.h"
#include "pinmux.h"

#define rsr_log(f, a...) LOGD(f, ##a)

extern void SpHdsp_setAtmos(QActive *pSender, int Idx, int value1, int value2);
extern void AudDspService_Set_OutSampleRate(QActive *pSender, UINT32 value);

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

static stMCU_IF_RSR_t *self; /* global data for internal use */
static uint8_t *packet_buf;
static uint8_t cmd_to_ack;
static enum dsp_input_src current_src = 0;

typedef void (*cmd_handler)(uint8_t *, uint8_t);

static uint8_t calc_checksum(uint8_t *data, uint8_t len)
{
	uint8_t sum = 0;
	uint8_t i = 0;

	while (i < len)
		sum += data[i++];
	return ((((sum >> 4) + (sum & 0x0f)) & 0x0f) + 0x30);
}

static void packet_reader(void *arg)
{
	stMCU_IF_RSR_t *me = arg;
	stMCU_IF_RSR_Evt *evt;
	struct timeval start_time, cur_time;
	uint64_t time_elapsed = 0; /* ms */
	int len, received;
	int ret = 0;

	do {
		ret = MCUIF_Read(me->mcuif_id, (char *)packet_buf, 1);
		if (ret != 1) {
			return;
		}
	} while (packet_buf[0] != SYNC_BYTE);

	ret = MCUIF_Read(me->mcuif_id, (char *)packet_buf + 1, 1);
	if (ret != 1) {
		return;
	}
	len = packet_buf[1] + 1;
	received = 2; /* SYNC_BYTE and LEN_BYTE */
	/* wait for the rest data of the packet if needed */
	gettimeofday(&start_time, NULL);
	while (received < len && time_elapsed < 100) {
		received += MCUIF_Read(me->mcuif_id, packet_buf + received, len - received);
		gettimeofday(&cur_time, NULL);
		time_elapsed = (cur_time.tv_sec - start_time.tv_sec) * 1000 + (cur_time.tv_usec - start_time.tv_usec) / 1000;
	}
	if (received < len) {
		rsr_log("waiting for rest data timeout, drop the received data,  packet_buf %x-%x\n", packet_buf[0], packet_buf[1]);
		return;
	}
	if (calc_checksum(&packet_buf[1], len - 2) != packet_buf[len - 1]) {
		rsr_log("Wrong checksum, data may be corrupted, drop it\n");
		return;
	}

	rsr_log("CIS data get success! len=%d.\n", len);
	evt = Q_NEW(stMCU_IF_RSR_Evt, MCUIF_RCV_PKT_SIG);
	evt->data = packet_buf;
	evt->len = len;
	QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
	return;
}

static void send_packet(uint8_t cmdid, uint8_t *data, uint8_t len)
{
	uint8_t packet[256];

	packet[0] = 0xFF;
	packet[1] = len + 3;
	packet[2] = cmdid;
	memcpy(&packet[3], data, len);
	packet[len + 3] = calc_checksum(&packet[1], len + 2);
	MCUIF_Write(self->mcuif_id, packet, len + 4);


	rsr_log("send_packet %x-%x-%x\n", packet[0], packet[1], packet[2]);
}

static void send_dsp_sys_status(uint8_t status)
{
	uint8_t data[12] = {0};

	/* version code: byte 0, 1 */
	data[0] = DSP_VER_M;
	data[1] = DSP_VER_S;
	data[2] = status;
	send_packet(DSP_RET_STATUS, data, 12);
}

static uint8_t AudFmt_2_RSRFmt(int fmt)
{
	uint8_t ret_fmt = 0;
	uint8_t AudFmt = (uint8_t)fmt;
	switch(AudFmt)
	{
		case 0x01:
			ret_fmt = DI_AUD_FMT_PCM;	//0x01
			break;
		case 0x03:
			ret_fmt = DI_AUD_FMT_DD;	/* Dolby Digital, 0x02 */
			break;
		case 0x08:
			ret_fmt = DI_AUD_FMT_DTS;	//0x03
			break;
		case 0x15:
			ret_fmt = DI_AUD_FMT_HDCD;	//0x04
			break;
		case 0x16:
			ret_fmt = DI_AUD_FMT_MP3;	//0x05
			break;
		case 0x17:
			ret_fmt = DI_AUD_FMT_MPEG2;	//0x06
			break;
		case 0x18:
			ret_fmt = DI_AUD_FMT_MPEG2_AAC_ADTS;	//0x07
			break;
		case 0x04:
			ret_fmt = DI_AUD_FMT_DDE;	/* Dolby Digital Ex, 0x08*/
			break;
		case 0x0a:
			ret_fmt = DI_AUD_FMT_DTS_ES_MATRIX;	//0x09
			break;
		case 0x0b:
			ret_fmt = DI_AUD_FMT_DTS_ES_DISCRETE;	//0x0a
			break;
		case 0x0c:
			ret_fmt = DI_AUD_FMT_DTS_ES_8CH_DISCRETE;	//0x0b
			break;
		case 0x02:
			ret_fmt = DI_AUD_FMT_PCM_MCH;	//0x0c
			break;
		case 0x05:
			ret_fmt = DI_AUD_FMT_DDP; /* Dolby Digital Plus, 0x0d */
			break;
		case 0x06:
			ret_fmt = DI_AUD_FMT_DD_TRUEHD; /* Dolby Digital TrueHD, 0x0e */
			break;
		case 0x0e:
			ret_fmt = DI_AUD_FMT_DTS_MASTER;	//0x0f
			break;
		case 0x0d:
			ret_fmt = DI_AUD_FMT_DTS_HIRES;	/* DTS High Resolution, 0x10 */
			break;
		case 0x0f:
			ret_fmt = DI_AUD_FMT_DTS_LBR;	/* DTS Low BitRate, 0x11 */
			break;
		case 0x19:
			ret_fmt = DI_AUD_FMT_DSD;	//0x12
			break;
		case 0x1a:
			ret_fmt = DI_AUD_FMT_WMA_PRO;	//0x13
			break;
		case 0x07:
			ret_fmt = DI_AUD_FMT_MAT;	//0x14
			break;
		case 0x09:
			ret_fmt = DI_AUD_FMT_DTS_9624;	//0x16
			break;
		case 0x10:
			ret_fmt = DI_AUD_FMT_DTS_LOSSLESS;	//0x17
			break;
		case 0x11:
			ret_fmt = DI_AUD_FMT_DTS_UHD;	//0x18
			break;
		case 0x12:
			ret_fmt = DI_AUD_FMT_DTS_UHD_MASTER;	//0x19
			break;
		case 0x13:
			ret_fmt = DI_AUD_FMT_DTS_UHD_LOSSLESS;	//0x1a
			break;
		case 0x14:
			ret_fmt = DI_AUD_FMT_DTS_UHD_GAME;	//0x1b
			break;
		default:
			ret_fmt = DI_AUD_FMT_NONE;
			break;
	}

	return ret_fmt;
}

static uint8_t AudSmp_2_RSRSmp(int smp)
{
	uint8_t ret_Smp = 0xff;
	switch(smp)
	{
		case 8000:
			ret_Smp = DI_SAMPLERATE_8K;
			break;
		case 11025:
			ret_Smp = DI_SAMPLERATE_11_025K;
			break;
		case 12000:
			ret_Smp = DI_SAMPLERATE_12K;
			break;
		case 16000:
			ret_Smp = DI_SAMPLERATE_16K;
			break;
		case 22050:
			ret_Smp = DI_SAMPLERATE_22_05K;
			break;
		case 24000:
			ret_Smp = DI_SAMPLERATE_24K;
			break;
		case 32000:
			ret_Smp = DI_SAMPLERATE_32K;
			break;
		case 44100:
			ret_Smp = DI_SAMPLERATE_44_1K;
			break;
		case 48000:
			ret_Smp = DI_SAMPLERATE_48K;
			break;
		case 64000:
			ret_Smp = DI_SAMPLERATE_64K;
			break;
		case 88200:
			ret_Smp = DI_SAMPLERATE_88_2K;
			break;
		case 96000:
			ret_Smp = DI_SAMPLERATE_96K;
			break;
		case 128000:
			ret_Smp = DI_SAMPLERATE_128K;
			break;
		case 176400:
			ret_Smp = DI_SAMPLERATE_176_4K;
			break;
		case 192000:
			ret_Smp = DI_SAMPLERATE_192K;
			break;
		default:
			ret_Smp = 0xff;
			break;
	}

	return ret_Smp;
}

static void AudGui_2_RSRGui(unsigned int *piAudgui, uint8_t *pogui1, uint8_t *pogui2)
{
	if( (*piAudgui) & BIT(0) )
	{
		(*pogui1) |= BIT(4);	//byte10-bit4, DAP
	}
	if( (*piAudgui) & BIT(1) )
	{
		(*pogui2) |= BIT(1);	//byte11-bit1, Tunning Mode
	}
	if( (*piAudgui) & BIT(2) )
	{
		//DIRECT, ignore
	}
	if( (*piAudgui) & BIT(3) )
	{
		(*pogui2) |= BIT(6);	//byte11-bit6, Height Filter
	}
	if( (*piAudgui) & BIT(4) )
	{
		(*pogui2) |= BIT(5);	//byte12-bit5, LM
	}
	if( (*piAudgui) & BIT(5) )
	{
		(*pogui1) |= BIT(2);	//byte10-bit2, VLAMP
	}
	if( (*piAudgui) & BIT(6) )
	{
		(*pogui1) |= BIT(0);	//byte10-bit0, Upmix
	}
	if( (*piAudgui) & BIT(7) )
	{
		(*pogui1) |= BIT(1);	//byte10-bit1, CSPREAD
	}
	if( (*piAudgui) & BIT(8) )
	{
		(*pogui2) |= BIT(2);	//byte11-bit2, loads XML, CONFIG
		(*pogui2) |= BIT(3);	//byte11-bit3, Select endpoint in .xml, CONFIG
		(*pogui2) |= BIT(4);	//byte11-bit4, Loads virt in .xml, CONFIG
	}
	if( (*piAudgui) & BIT(9) )
	{
		(*pogui1) |= BIT(3);	//byte10-bit3, VIRT
	}
	if( (*piAudgui) & BIT(10) )
	{
		(*pogui1) |= BIT(5);	//byte10-bit5, DRC
	}
	if( (*piAudgui) & BIT(11) )
	{
		(*pogui2) |= BIT(6);	//byte10-bit6, POST
	}
	if( (*piAudgui) & BIT(12) )
	{
		//PROFILE, ignore
	}

}

static void get_aud_info(uint8_t *data, int8_t size)
{
	int ret_eng_vec[16] = {0};
	uint8_t temp_Engy[12] = {0};
	int dCh_Num=0, dSampleRate=0, dFormat=0;
	uint8_t RsrAudFmt=0, RsrSmp=0, RsrChNum=0, temp=0;
	uint8_t i, GuiStatus1=0, GuiStatus2=0;
	unsigned int gui_ret=0;

	if (size < 12) {
		rsr_log("buffer too small: the payload length of 'return DSP info' is at least 12 byte\n");
		return;
	}

	// Get Audio Information
	SpHdsp_getAudioFormat(&dCh_Num, &dSampleRate, &dFormat);
	// Format
	if(dFormat & 0x8000)
	{
		temp = BIT(7);	// Dolby Atmos Audio
	}
	RsrAudFmt |= temp;
	temp = 0;
	if(dFormat & 0x4000)
	{
		temp = BIT(6);	// Dolby Surround Decode
	}
	RsrAudFmt |= temp;
	RsrAudFmt |= AudFmt_2_RSRFmt(dFormat);
	rsr_log("===== Now Format: 0x%x\n", RsrAudFmt);

	// Input Fs
	RsrSmp = AudSmp_2_RSRSmp(dSampleRate);
	rsr_log("===== Now Samplate: 0x%x.\n", RsrSmp);

	// Channels
	RsrChNum = (uint8_t)dCh_Num;
	rsr_log("===== Now Channel Number: %d.\n", RsrChNum);

	// Get 16-ch Audio Output Energy
	AudDspService_Get_OutputEnergy(ret_eng_vec);
	for(i=0; i<12; i++)
	{
		ret_eng_vec[i] = (int)(log10(ret_eng_vec[i]) / log10(4.64));	//channel level convert
		temp_Engy[i] = (uint8_t)(ret_eng_vec[i]);	//clear more high bits
	}

	// Get GUI Setting Status
	SpHdsp_getAtmosRetValue(NULL, &gui_ret);
	rsr_log("===== AudGuiStatus: 0x%x\n", gui_ret);
	AudGui_2_RSRGui(&gui_ret, &GuiStatus1, &GuiStatus2);
	rsr_log("===== GuiSt1: 0x%x, GuiSt2: 0x%x.\n", GuiStatus1, GuiStatus2);

	data[0] = RsrAudFmt;	/* audio format */
	data[1] = RsrSmp; 		/* input fs */

	data[3] = RsrChNum;		/* Mch-PCM Audio channels */
	data[4] = (((temp_Engy[0]) << 4) | (temp_Engy[1]));		/* L,R energy */
	data[5] = (((temp_Engy[4]) << 4) | (temp_Engy[5]));		/* C,Lfe energy */
	data[6] = (((temp_Engy[2]) << 4) | (temp_Engy[3]));		/* Ls,Rs energy */
	data[7] = (((temp_Engy[6]) << 4) | (temp_Engy[7]));		/* Lrs,Rrs energy */
	data[8] = (((temp_Engy[8]) << 4) | (temp_Engy[9]));		/* Ltf,Rtf energy */
	data[9] = (((temp_Engy[10]) << 4) | (temp_Engy[11])); 		/* Ltr,Rtr energy */
	data[10] = GuiStatus1; 	/* gui-setting status 1 */
	data[11] = GuiStatus2; 	/* gui-setting status 2 */
	return;
}

static void send_dsp_aud_info(uint8_t *data, int8_t size)
{
	if (size < 12) {
		rsr_log("buffer too small: the payload length of 'return DSP info' is at least 12 byte\n");
		return;
	}

	data[2] = cmd_to_ack;
	cmd_to_ack = 0;
	send_packet(DSP_RET_AUD_INFO, data, 12);
	return;
}

static void set_dsp_input_src(enum dsp_input_src src)
{
	static int8_t called_flag = 0;

	if (called_flag && (current_src == src))
		return;

	switch (src) {
	case INPUT_I2S_8CH_SLAVE:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, IIS8CH_HDMIHBR);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case INPUT_I2S_2CH_MASTER_48K:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, IIS8CH_HDMISTD);
		AudDspService_Set_OutSampleRate(NULL, 48000);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case INPUT_I2S_2CH_MASTER_44_1K:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, IIS8CH_HDMISTD);
		AudDspService_Set_OutSampleRate(NULL, 44100);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case INPUT_I2S_2CH_SLAVE:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, IIS8CH_HDMISTD);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case INPUT_SPDIF:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_SPDIF(NULL, SPDIF_IEC0);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case INPUT_STDBY:
		break;
	case INPUT_DSP_RST:
		break;
	case INPUT_SRC_NONE:
	case INPUT_UDISK_UPGRD:
		/* TODO: */
		break;
	default:
		rsr_log("%s unknown source id %d\n", __FUNCTION__, src);
		return;
	}
	current_src = src;
	called_flag = 1;
	return;
}

static void set_mix_mode(enum ch_mix_mode mode)
{
	static int8_t called_flag = 0;
	static enum ch_mix_mode current_mode = 0;
	int8_t mix_mode;

	if (called_flag && (current_mode == mode))
		return;

	switch (mode) {
	case CH_MIX_2CH:
		mix_mode = 0;
		break;
	case CH_MIX_MCH:
		mix_mode = 1;
		break;
	case CH_MIX_DOLBY_DAP:
		mix_mode = 2;
		break;
	case CH_MIX_DTS_NEOX:
		mix_mode = 3;
		break;
	case CH_MIX_DTS_VIRTX:
		mix_mode = 4;
		break;
	default:
		rsr_log("%s unknown channel mix mode %d\n", __FUNCTION__, mode);
		return;
	}

	SpdHdsp_set_Audio_Flow(NULL, HDSP_AUDIO_CIS);
	SpHdsp_Set_UpmixMode(NULL, mix_mode);

	current_mode = mode;
	called_flag = 1;
	return;
}

static void set_mix_ch(uint8_t downmix_ch, uint8_t upmix_ch)
{
	static int8_t called_flag = 0;
	static uint8_t current_downmix_ch = 0;
	static uint8_t current_upmix_ch = 0;
	uint8_t ch = 0;

	if (!called_flag || ((current_downmix_ch != downmix_ch))) {
		ch |= downmix_ch & MIX_CH_LR ?  BIT(0) : 0;
		ch |= downmix_ch & MIX_CH_C ?  BIT(3) : 0;
		ch |= downmix_ch & MIX_CH_LFE ?  BIT(2) : 0;
		ch |= downmix_ch & MIX_CH_LRS ?  BIT(1) : 0;
		ch |= downmix_ch & MIX_CH_LRRS ?  BIT(4) : 0;
		ch |= downmix_ch & MIX_CH_LRTF ?  BIT(5) : 0;
		ch |= downmix_ch & MIX_CH_LRTR ?  BIT(6) : 0;
		/* set down mix */
		rsr_log("set down mix channel: 0x%x\n", ch);
		SpHdsp_Set_DownmixConfig(NULL, ch);
		current_downmix_ch = downmix_ch;
	}

	if (!called_flag || ((current_upmix_ch != upmix_ch))) {
		ch = 0;
		ch |= upmix_ch & MIX_CH_LR ?  BIT(0) : 0;
		ch |= upmix_ch & MIX_CH_C ?  BIT(3) : 0;
		ch |= upmix_ch & MIX_CH_LFE ?  BIT(2) : 0;
		ch |= upmix_ch & MIX_CH_LRS ?  BIT(1) : 0;
		ch |= upmix_ch & MIX_CH_LRRS ?  BIT(4) : 0;
		ch |= upmix_ch & MIX_CH_LRTF ?  BIT(5) : 0;
		ch |= upmix_ch & MIX_CH_LRTR ?  BIT(6) : 0;
		/* set up mix */
		rsr_log("set up mix channel: 0x%x\n", ch);
		SpHdsp_Set_UpmixConfig(NULL, ch);
		current_upmix_ch = upmix_ch;
	}
	called_flag = 1;
	return;
}

static void set_i2s_format(uint8_t settings)
{
	static int8_t called_flag = 0;
	static int8_t send_info_on = 0;
	static uint8_t current_settings = 0;
	int samplerate;

	if (called_flag && (current_settings == settings))
		return;

	if (!called_flag || ((current_settings & 0xc0) != (settings & 0xc0))) {
		switch (settings >> 6) {
		case 0:
			samplerate = 48000;
			AudDspService_Set_BypassMode(NULL, 0);
			break;
		case 1:
			samplerate = 44100;
			AudDspService_Set_BypassMode(NULL, 0);
			break;
		case 2:
			samplerate = 96000;
			AudDspService_Set_BypassMode(NULL, 1);
			break;
		default:
			rsr_log("Wrong I2S out sample rate\n");
			samplerate = 0;
			break;
		}
		AudDspService_Set_OutSampleRate(NULL, samplerate);
	}

	/* I2S output format */
	if (!called_flag || ((current_settings & 0x30) != (settings & 0x30))) {
		switch ((settings & 0x30) >> 4) {
		case I2S_FORMAT_I2S:
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_DELAY, 1);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_JUSTIFY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_PARITY, 0);
			break;
		case I2S_FORMAT_LEFT_JUSTIFIED:
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_DELAY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_JUSTIFY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_PARITY, 1);
			break;
		case I2S_FORMAT_RIGHT_JUSTIFIED:
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_DELAY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_JUSTIFY, 1);
			AudDspService_Set_IISFormat(NULL, U_I2S_PCM, U_I2S_CFG_PARITY, 1);
		default:
			break;
		}
	}

	/* I2S input format */
	if (!called_flag || ((current_settings & 0x03) != (settings & 0x03))) {
		switch (settings & 0x03) {
		case I2S_FORMAT_I2S:
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_DELAY, 1);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_JUSTIFY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_PARITY, 0);
			break;
		case I2S_FORMAT_LEFT_JUSTIFIED:
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_DELAY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_JUSTIFY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_PARITY, 1);
			break;
		case I2S_FORMAT_RIGHT_JUSTIFIED:
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_DELAY, 0);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_JUSTIFY, 1);
			AudDspService_Set_IISFormat(NULL, U_I2S_HDMI, U_I2S_CFG_PARITY, 1);
		default:
			break;
		}
	}

	if (settings & BIT(3)) {
		/* disable MCLK */
		GPIO_Funtion_Set(61, IO_GPIO);
		GPIO_OE_Set(61, IO_OUTPUT);
		GPIO_Output_Write(61, IO_LOW);
	}
	else
	{
		/* ensable MCLK */
		GPIO_Funtion_Set(61, IO_MODULEMODE);
		Pinmux_Module_Alloc(I2S_OUT_8CH);
	}

	if (settings & 0x04)
	{
		if (!send_info_on)
		{
			/* Notify DSP information to MCU periodically */
			QTimeEvt_armX(&(self->timeRSR200msEvt), TICKS_PER_200MS, 0);
			send_info_on = 1;
		}
	} else {
		if (send_info_on) {
			QTimeEvt_disarmX(&(self->timeRSR200msEvt));
			send_info_on = 0;
		}
	}
	current_settings = settings;
	called_flag = 1;
	return;
}

static void handler_dsp_main_setup(uint8_t *data, uint8_t len)
{
	enum dsp_input_src dsp_input_src;
	enum ch_mix_mode mix_mode;
	uint8_t downmix_ch;
	uint8_t upmix_ch;
	uint8_t i2s_format_setting;

	(void)len;
	dsp_input_src = data[0] & 0x0f;
	mix_mode = data[0] >> 4;
	downmix_ch = data[1];
	upmix_ch = data[2];
	i2s_format_setting = data[3];

	set_dsp_input_src(dsp_input_src);
	set_mix_mode(mix_mode);
	set_mix_ch(downmix_ch, upmix_ch);
	set_i2s_format(i2s_format_setting);

	cmd_to_ack |= ACK_DSP_MAIN_SETUP;
}

static void set_i2s_slave_input(enum isi_type type, enum isi_samplerate samplerate)
{
	static int8_t called_flag = 0;
	static enum isi_type current_type = 0;
	(void)samplerate; /* sample rate is auto detected by DSP, so just ignore it */

	if (current_src == INPUT_SPDIF)
		return;
	if (called_flag && (current_type == type))
		return;
	called_flag = 1;

	switch (type) {
	case ISI_TYPE_SD:
	case ISI_TYPE_PCM_2CH:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, Source_HDMISTD);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case ISI_TYPE_HBR:
	case ISI_TYPE_PCM_MCH:
		AudDspService_SPDIF_STOP(NULL);
		AudDspService_Flow_Stop(NULL);
		AudDspService_Prepare_8CH_IISIN(NULL, Source_HDMIHBR);
		AudDspService_Set_SourceGain(NULL, 0);
		AudDspService_Flow_Play(NULL);
		AudDspService_SPDIF_PLAY(NULL);
		break;
	case ISI_TYPE_NONE:
	default:
		break;
	}

	current_type = type;
	return;
}

static void select_DSP_app_mode(int8_t mode)
{
	static int8_t called_flag = 0;
	static uint8_t current_mode[10] = {0};

	int8_t group = (mode & 0xf0) >> 4;
	int8_t index = mode & 0x0f;

	if (group == 0)
		return;
	if (called_flag && (current_mode[group] == index))
		return;

	switch (group)
	{
		case DATG_MATRIX1:
			rsr_log(" select Matrix 1, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_MMIX, 0, index+1);
			break;
		case DATG_MATRIX2:
			rsr_log(" select Matrix 2, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_MMIX, 1, index+1);
			break;
		case DATG_FILTER1:
			rsr_log(" select Filter 1, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_EQ, 0, index+1);
			break;
		case DATG_DRC:
			rsr_log(" select DRC, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_DRC, 0, index+1);
			break;
		case DATG_VIRTUALX:
			rsr_log(" select Virtual X, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_VX, 0, index+1);
			break;
		case DATG_FILTER2:
			rsr_log(" select Filter 2, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_EQ, 1, index+1);
			break;
		case DATG_FILTER3:
			rsr_log(" select Filter 3, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_EQ, 2, index+1);
			break;
		case DATG_MATRIX3:
			rsr_log(" select Matrix 3, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_MMIX, 2, index+1);
			break;
		case DATG_BM:
			rsr_log(" select BM, mode %d.\n", index+1);
			AudDspService_Set_AppMode(NULL, SET_BASS, 0, index+1);
			break;

		case DATG_NO_SELECT:
		default:
			rsr_log(" NO select.\n");
			break;
	}

	called_flag = 1;
	current_mode[group] = index;
	return;
}

static void IO_mute_switch(int8_t enable)
{
	static int8_t called_flag = 0;
	static int8_t current_state = 0;

	if (called_flag && (current_state == enable))
		return;

	if(enable)
	{
		//rsr_log("Set DSP DeMUTE!\n");
		//AudDspService_Set_DeMute(NULL);
	}
	else
	{
		//rsr_log("Set DSP MUTE!\n");
		//AudDspService_Set_Mute(NULL);
	}

	called_flag = 1;
	current_state = enable;
	return;
}

static void set_vol_ramp_step(int8_t step)
{
	static int8_t called_flag = 0;
	static int8_t current_step = 0;

	if (called_flag && (current_step == step))
		return;

	if(step <= 0)
		step = 0;
	if(step >= 0x3F)
		step = 0x3F;

	AudDspService_CIS_Ramp_Step(NULL, step*30); 			// unit 30ms

	called_flag = 1;
	current_step = step;
}

static void handler_system_info(uint8_t *data, uint8_t len)
{
	enum isi_type input_type;
	enum isi_samplerate input_samplerate;
	uint8_t gpio_state;
	int8_t app_mode;
	int8_t IO_mute;
	int8_t vol_ramp_step;

	(void)len;
	input_type = data[0] >> 5;
	input_samplerate = data[0] & 0x1f;
	gpio_state = data[1];
	app_mode = data[2];
	IO_mute = data[3] & 0x1;
	vol_ramp_step = (data[3] >> 1) & 0x3F;

	set_i2s_slave_input(input_type, input_samplerate);
	select_DSP_app_mode(app_mode);
	IO_mute_switch(IO_mute);
	set_vol_ramp_step(vol_ramp_step);

	cmd_to_ack |= ACK_SYSTEM_INFO;
}

static void handler_channel_delay_setting(uint8_t *data, uint8_t len)
{
	static int8_t called_flag = 0;
	static uint8_t current_delay_setting[12] = {0};
	uint8_t delay;
	int8_t ch_map[12] = {
		[CDS_FL] = 0,
		[CDS_FR] = 1,
		[CDS_C] = 4,
		[CDS_LFE] = 5,
		[CDS_LS] = 2,
		[CDS_RS] = 3,
		[CDS_LRS] = 6,
		[CDS_RRS] = 7,
		[CDS_LTF] = 8,
		[CDS_RTF] = 9,
		[CDS_LTR] = 10,
		[CDS_RTR] = 11,
	};
	int8_t i;

	(void)len;
	for (i = CDS_FL; i <= CDS_RTR; i++) {
		delay = data[i];
		if (!called_flag || (current_delay_setting[i] != delay)) {
			AudDspService_Set_CIS_PARAM(NULL, SET_DELAY, ch_map[i], delay, 0);
			current_delay_setting[i] = delay;
		}
	}
	cmd_to_ack |= ACK_CHANNEL_DELAY_SETTING;
	called_flag = 1;
}

static void handler_dsp_volume(uint8_t *data, uint8_t len)
{
	static int8_t called_flag = 0;
	static uint8_t current_volume[12] = {0};
	uint8_t value;
	int8_t volume_ch_map[12] = {
		[DV_FL] = 0,
		[DV_FR] = 1,
		[DV_C] = 4,
		[DV_LFE] = 5,
		[DV_LS] = 2,
		[DV_RS] = 3,
		[DV_LRS] = 6,
		[DV_RRS] = 7,
		[DV_LTF] = 8,
		[DV_RTF] = 9,
		[DV_LTR] = 10,
		[DV_RTR] = 11,
	};
	int8_t i;

	(void)len;
	for (i = DV_FL; i <= DV_RTR; i++) {
		value = data[i];
		if (!called_flag || (value != current_volume[i])) {
			AudDspService_Set_CIS_PARAM(NULL, SET_VOLUME, volume_ch_map[i], value, 0);
			current_volume[i] = value;
		}
	}
	called_flag = 1;
	cmd_to_ack |= ACK_DSP_VOLUME;
}

static void handler_dsp_gui_setting(uint8_t *data, uint8_t len)
{
	static int8_t called_flag = 0;
	static uint8_t current_dsp_gui_settings[12] = {0};
	(void)len;
	uint8_t new_value, old_value;
	uint8_t aud_info[12] = {0};

	new_value = data[0];
	old_value = current_dsp_gui_settings[0];
	if (!called_flag || (new_value != old_value)) {
		if (!called_flag || ((new_value & 0x0f) != (old_value & 0x0f)))
			SpHdsp_setAtmos(NULL, 12, new_value & 0x0f, 0); /* DAP profile */
		if (!called_flag || ((new_value & 0x30) != (old_value & 0x30)))
			SpHdsp_setAtmos(NULL, 10, 0, (new_value & 0x30) >> 4); /* DRC setting */
		if (!called_flag || ((new_value & 0x40) != (old_value & 0x40))) {
			if (!(new_value & 0x40))
				SpHdsp_setAtmos(NULL, 11, 0, 0); /* Post setting */
			else
				SpHdsp_setAtmos(NULL, 11, 1, 0); /* Post setting */
		}
		current_dsp_gui_settings[0] = new_value;
	}
	new_value = data[1];
	old_value = current_dsp_gui_settings[1];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 10, 1, new_value); /* DRC cut scale factor X */
		current_dsp_gui_settings[1] = new_value;
	}
	new_value = data[2];
	old_value = current_dsp_gui_settings[2];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 10, 2, new_value); /* DRC cut scale factor Y */
		current_dsp_gui_settings[2] = new_value;
	}
	new_value = data[3];
	old_value = current_dsp_gui_settings[3];
	if (!called_flag || (new_value != old_value)) {
		if (!called_flag || ((new_value & 0x10) != (old_value & 0x10)))
			SpHdsp_setAtmos(NULL, 7, (new_value & 0x10) >> 4, 0); /* center spreading of surround decoder */
		if (!called_flag || ((new_value & 0x20) != (old_value & 0x20)))
			SpHdsp_setAtmos(NULL, 5, !((new_value & 0x20) >> 5), 0); /* volume leveler output amplification */
		if (!called_flag || ((new_value & 0xc0) != (old_value & 0xc0))) {
			switch ((new_value & 0xc0) >> 6) {
			case 0:
				SpHdsp_setAtmos(NULL, 9, 0, 1); /* virtualization setting: speaker */
				break;
			case 1:
				SpHdsp_setAtmos(NULL, 9, 0, 2); /* virtualization setting: headphone */
				break;
			case 2:
				SpHdsp_setAtmos(NULL, 9, 0, 0); /* virtualization setting: off */
				break;
			}
		}
		current_dsp_gui_settings[3] = new_value;
	}
	new_value = data[4];
	old_value = current_dsp_gui_settings[4];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 9, 1, new_value); /* virtualization angle setting: front angle */
		current_dsp_gui_settings[4] = new_value;
	}
	new_value = data[5];
	old_value = current_dsp_gui_settings[5];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 9, 2, new_value); /* virtualization angle setting: surround angle  */
		current_dsp_gui_settings[5] = new_value;
	}
	new_value = data[6];
	old_value = current_dsp_gui_settings[6];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 9, 3, new_value); /* virtualization angle setting: height angle  */
		current_dsp_gui_settings[6] = new_value;
	}
	new_value = data[7];
	old_value = current_dsp_gui_settings[7];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 9, 4, new_value); /* virtualization angle setting: rear surround angle  */
		current_dsp_gui_settings[7] = new_value;
	}
	new_value = data[8];
	old_value = current_dsp_gui_settings[8];
	if (!called_flag || (new_value != old_value)) {
		if (!called_flag || ((new_value & 0x08) != (old_value & 0x08)))
			SpHdsp_setAtmos(NULL, 2, (new_value & 0x08) >> 3, 0); /* legacy setting */
		if (!called_flag || ((new_value & 0x10) != (old_value & 0x10)))
			SpHdsp_setAtmos(NULL, 4, (new_value & 0x10) >> 4, 0); /* loudness */
		if (!called_flag || ((new_value & 0x20) != (old_value & 0x20)))
			SpHdsp_setAtmos(NULL, 3, (new_value & 0x20) >> 5, 0); /* hfilt */
		if (!called_flag || ((new_value & 0x40) != (old_value & 0x40)))
			SpHdsp_setAtmos(NULL, 6, (new_value & 0x40) >> 6, 0); /* upmix */
		current_dsp_gui_settings[8] = new_value;
	}
	new_value = data[9];
	old_value = current_dsp_gui_settings[9];
	if (!called_flag || (new_value != old_value)) {
		if (!called_flag || ((new_value & 0x02) != (old_value & 0x02)))
			SpHdsp_setAtmos(NULL, 1, (new_value & 0x02) >> 1, 0); /* tuning mode */
		if (!called_flag || ((new_value & 0x04) != (old_value & 0x04)))
			/* loads the configuration with virtualization enabled provided within the .xml file */
			SpHdsp_setAtmos(NULL, 8, 2, (new_value & 0x04) >> 2);
		if (!called_flag || ((new_value & 0x08) != (old_value & 0x08)))
			/* Selects an endpoint configuration based on the name provided within the .xml configuration file */
			SpHdsp_setAtmos(NULL, 8, 1, (new_value & 0x08) >> 3);
		if (!called_flag || ((new_value & 0xf0) != (old_value & 0xf0)))
			/* Loads the configuration (.xml) file generated by the Dolby tuning tool */
			SpHdsp_setAtmos(NULL, 8, 0, (new_value & 0xf0) >> 4);
		current_dsp_gui_settings[9] = new_value;
	}
	new_value = data[10];
	old_value = current_dsp_gui_settings[10];
	if (!called_flag || (new_value != old_value)) {
		SpHdsp_setAtmos(NULL, 13, new_value, 0); /* Surround Boost level */
		current_dsp_gui_settings[10] = new_value;
	}
	called_flag = 1;
	cmd_to_ack |= ACK_DSP_GUI_SETTING;
	get_aud_info(aud_info, 12);
	send_dsp_aud_info(aud_info, 12);

	//SpHdsp_getAtmos(NULL);

}

static void handler_dsp_bm_ctrl(uint8_t *data, uint8_t len)
{
	static int8_t called_flag = 0;
	static uint8_t current_bm_settings[16] = {0};
	uint8_t new_value, old_value;
	int value1, value2;
	int8_t i;
	/* map BM_GAIN_ and BM_CUT_ to DSP channel id */
	int8_t bm_ctrl_ch_map[][2] = {
		[BM_GAIN_LR] = {0, 1},
		[BM_GAIN_C] = {4, -1},
		[BM_GAIN_LFE] = {5, -1},
		[BM_GAIN_LRS] = {2, 3},
		[BM_GAIN_LRRS] = {6, 7},
		[BM_GAIN_LRTF] = {8, 9},
		[BM_GAIN_LRTR] = {10, 11},
		[BM_GAIN_BM2LR] = {0, 1},
		[BM_GAIN_BM2SLR] = {2, 3},
		[BM_CUT_LR] = {0, 1},
		[BM_CUT_C] = {4, -1},
		[BM_CUT_LFE] = {5, -1},
		[BM_CUT_LRS] = {2, 3},
		[BM_CUT_LRRS] = {6, 7},
		[BM_CUT_LRTF] = {8, 9},
		[BM_CUT_LRTR] = {10, 11},
	};

	(void)len;
	for (i = BM_GAIN_LR; i <= BM_CUT_LRTR; i++) {
		new_value = data[i];
		old_value = current_bm_settings[i];
		if (!called_flag || (new_value != old_value)) {
			if (i <= BM_GAIN_LRTR) {
				//value1 = 0x3c - new_value * 2;
				value1 = new_value;
				value2 = 0;
			} else if (i <= BM_GAIN_BM2SLR) {
				//value1 = 0x3c - new_value * 2;
				value1 = new_value;
				value2 = 1;
			} else {
				value1 = new_value;

				if(value1 < 10)
				{
					value1 = 10;
				}
				else if(value1 > 200)
				{
					value1 = 200;
				}

				value2 = 2;
			}
			if (bm_ctrl_ch_map[i][0] >= 0)
				AudDspService_Set_CIS_PARAM(NULL, SET_BASS, bm_ctrl_ch_map[i][0], value1, value2);
			if (bm_ctrl_ch_map[i][1] >= 0)
				AudDspService_Set_CIS_PARAM(NULL, SET_BASS, bm_ctrl_ch_map[i][1], value1, value2);
			current_bm_settings[i] = new_value;
		}
	}
	called_flag = 1;
	cmd_to_ack |= ACK_DSP_BM_CTRL;
}

/* handler table. it's a sparse table */
static cmd_handler cmd_handlers[] = {
	[DSP_MAIN_SETUP] = handler_dsp_main_setup,
	[SYSTEM_INFO] = handler_system_info,
	[CHANNEL_DELAY_SETTING] = handler_channel_delay_setting,
	[DSP_VOLUME] = handler_dsp_volume,
	[DSP_GUI_SETTING] = handler_dsp_gui_setting,
	[DSP_BM_CTRL] = handler_dsp_bm_ctrl,
};

/* data length table */
static UINT16 data_length[] = {
	[DSP_MAIN_SETUP] = 4,
	[SYSTEM_INFO] = 4,
	[CHANNEL_DELAY_SETTING] = 12,
	[DSP_VOLUME] = 12,
	[DSP_GUI_SETTING] = 12,
	[DSP_BM_CTRL] = 16,
};

static void dispatch_cmd(uint8_t cmdid, uint8_t *data, uint8_t len)
{
	cmd_handler h;

	if (cmdid >= ARRAY_SIZE(cmd_handlers)) {
		rsr_log("ERROR: INVALID command %u\n", cmdid);
	}

	if(len != data_length[cmdid])
	{
		(void)data;
		rsr_log("ERROR: INVALID data length!\n");
		return;
	}

	h = cmd_handlers[cmdid];
	if (h) {
		h(data, len);
	} else {
		rsr_log("No handler for command %u\n", cmdid);
	}
}

static QState MCU_IF_RSR_Active(stMCU_IF_RSR_t *const me, QEvt const *const e)
{
	QState status;
	const stMCU_IF_RSR_Evt *evt = Q_EVT_CAST(stMCU_IF_RSR_Evt);
	switch (e->sig) {
	case Q_ENTRY_SIG:
		rsr_log("MCU_IF_RSR_Active ENTRY\n");
		send_dsp_sys_status(SYS_STATUS_WAITCMD);
		status = Q_HANDLED();
		break;
	case Q_EXIT_SIG:
		rsr_log("MCU_IF_RSR_Active EXIT\n");
		status = Q_HANDLED();
		break;
	case Q_INIT_SIG:
		rsr_log("MCU_IF_RSR_Active INIT\n");
		status = Q_HANDLED();
		break;
	case MCUIF_RCV_PKT_SIG:
		dispatch_cmd(evt->data[2], evt->data + 3, evt->len - 4);
		status = Q_HANDLED();
		break;
	case RSR_TIMER_200ms_SIG:
		{
			uint8_t aud_info[12] = {0};
			rsr_log("========= MCU_IF_RSR_Active, Get Audio Info.\n");
			get_aud_info(aud_info, 12);
			cmd_to_ack = ACK_DSP_MAIN_SETUP;
			send_dsp_aud_info(aud_info, 12);

			QTimeEvt_rearm(&me->timeRSR200msEvt, TICKS_PER_200MS);

			status = Q_HANDLED();
			break;
		}
	case RSR_AudFmt_CHANGED_SIG:
		{
			uint8_t aud_info[12] = {0};
			rsr_log("========= MCU_IF_RSR_Active, AudFmt Changed.\n");
			get_aud_info(aud_info, 12);
			cmd_to_ack = 0;
			send_dsp_aud_info(aud_info, 12);

			status = Q_HANDLED();
			break;
		}
	default:
		status = Q_SUPER(&QHsm_top);
		break;
	}
	return status;
}

static QState MCU_IF_RSR_initial(stMCU_IF_RSR_t *const me, const QEvt *const e)
{
	(void)e;
	packet_buf = malloc(256);
	assert(packet_buf != NULL);
	me->mcuif_id = MCUIF_Init("uart1", packet_reader, me);
	if (me->mcuif_id < 0) {
		return (QState)Q_RET_NULL;
	}
	MCUIF_Config(me->mcuif_id, eMCUIF_ConfigType_BaudRate, eUART_BR_115200);
	MCUIF_Start(me->mcuif_id);
	
	//QTimeEvt_armX(&(self->timeRSR200msEvt), TICKS_PER_200MS, 0);
	
	return Q_TRAN(&MCU_IF_RSR_Active);
}

void MCU_IF_RSR_ctor(stMCU_IF_RSR_t *const me, QActive *superAO)
{
	rsr_log("MCU_IF_RSR_ctor ENTRY; \n");
	QHsm_ctor(&me->super, Q_STATE_CAST(&MCU_IF_RSR_initial));
	me->superAO = superAO;
	self = me;
	QTimeEvt_ctorX(&me->timeRSR200msEvt, me->superAO, RSR_TIMER_200ms_SIG, 0U);
}

