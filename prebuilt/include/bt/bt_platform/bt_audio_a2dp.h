#ifndef __BT_AUDIO_A2DP_H__
#define __BT_AUDIO_A2DP_H__

#include "bt_mw_core.h"

/**
 * @file bt_audio_a2dp.h
 *
 * audio interface file. used by btif mode.
 *
 * Copyright (c) 2013 Sunplus Technology Co., Ltd
 *
 * <b>
 @verbatim
   FILE NAME : bt_audio_a2dp.h
   PURPOSE   : Platform dependent audio will be implemented here.
   AUTHOR    :
   PHASE     :
   Document  :
   History   :
       Date             Author     Version   Description
       ----------  --------------  ------  ---------------------------------
      2015/12/04   swjiang        v0.00      first creation

 @endverbatim
 * </b>\n
 *
 * @note N/A
 * @bug N/A
 */
//#include "bt_data_types.h"//do not include it in 8107 system.


/************ ********* ********* ********* ********* *********** ****
**			Definitions
********* ********* ********* ********* ********* ********* **********/
#ifdef __BT_AUDIO_A2DP_C__
	#define EXTERN
#else
	#define EXTERN extern
#endif

#define UINT16 unsigned short
#define UINT32 unsigned int
#define UINT8 unsigned char

#define BT_A2DP_DATA_DUMP //only used for debug

/************ ********* ********* ********* ********* *********** ****
**			enum
********* ********* ********* ********* ********* ********* **********/
typedef enum {
	BTAUDIO_A2DP_CODEC_SBC =0,
	BTAUDIO_A2DP_CODEC_AAC,
	BTAUDIO_A2DP_CODEC_UNKNOWN = 0xFF
}BTAudio_A2DP_Codec_e;

typedef enum {
	BTAUDIO_A2DP_SAMPLE_RATE_44K = 0,
	BTAUDIO_A2DP_SAMPLE_RATE_48K,
	BTAUDIO_A2DP_SAMPLE_RATE_UNKNOWN = 0xFF
}BTAudio_A2DP_SampleRate_e;

typedef enum {
	BTAUDIO_A2DP_RESULT_SUCCESS = 0,
	BTAUDIO_A2DP_RESULT_NOT_INIT,
	BTAUDIO_A2DP_RESULT_CODEC_ERR,
	BTAUDIO_A2DP_RESULT_UNKNOWN_ERR = 0xFF
}BTAudio_A2DP_Result_e;

typedef enum {
	BTAUDIO_A2DP_ENCODE_MODE_NIGHT = 0,
	BTAUDIO_A2DP_ENCODE_MODE_NORMAL = 1,
}BTAudio_A2DP_Encode_Mode_e;

typedef enum {
	BTAUDIO_A2DP_ENCODE_DATA_READY = 0,
}BTAudio_A2DP_Callback_Msg_e;

typedef enum {
	BTAUDIO_A2DP_MAIN_DEVICE = 0,
    BTAUDIO_A2DP_SUB_DEVICE = 1,
}BTAudio_A2DP_Device_Type_e;

/************ ********* ********* ********* ********* *********** ****
**			struct
********* ********* ********* ********* ********* ********* **********/
typedef struct {
	UINT8* pbBuf;
	UINT32 dBufSize;
	BTAudio_A2DP_Codec_e eCodecType;
}BTAudio_A2DP_Data_t;

typedef struct {
    BTAudio_A2DP_Encode_Mode_e eMode;
    UINT8 bBandMode;  // 4 or 8 (default)
    UINT8 bBlockMode; // 0: 4 blocks, 1: 8 blocks, 2: 12 blocks, 3: 16 blocks(default)
    UINT8 bChannel;   // 0: Mono 1: Dual channel 2: Stereo (default) 3: Joint Stereo
    UINT8 bBitAllocationMode;// 0: Loudness (default) 1: SNR
    UINT8 bBitPool;
    UINT16 wFrameLen;
    UINT16 wWssPacketNum;
}BTAudio_A2DP_Encode_Info_t;

typedef struct {
    UINT8 bBandMode; // 4 or 8 (default)
    UINT8 bBlockMode; // 0: 4 blocks, 1: 8 blocks, 2: 12 blocks, 3: 16 blocks(default)
    UINT8 bChannel; // 0: Mono 1: Dual channel 2: Stereo (default) 3: Joint Stereo
    UINT8 bBitAllocationMode; // 0: Loudness (default) 1: SNR
    UINT8 bJointstereoEnable; // 0: Disable(default) 1: Enable
    UINT8 bLfeEnable; // 0: Encode LF/RF channel 1: Encode LFE channel
    UINT8 bSampleRate; // 0:16k 1:48k

    // Mono/ Dual channel max_bitpool = (sub_band << 4 - 1) min_bitpool = 2  defualt = 20
    // Joint Stereo /Stereo max_bitpool = (sub_band << 5 - 1) min_bitpool = 4  default = 32
    UINT8 bBitPool;
    UINT8 bIntrFrameNum; // DSP interrupt RISC after dumping N frame
    //the before variables must the same as t_enc_sbc_info,will use by audio service.
    UINT8 abReserved[2];
    //the below variables only used by bt_audio_a2dp.c
    UINT16 wFrameLen;
}BTAudio_A2DP_Dsp_Encode_Sbc_Info_t;

typedef void (*BTAudioA2DPCbk)(BTAudio_A2DP_Callback_Msg_e eMsg, int dParam);

typedef struct {
	BTAudioA2DPCbk  hCbk;/*!<audio callback function*/
} BTAudio_A2DP_Callback_t;


/************ ********* ********* ********* ********* *********** ****
**			Macros
********* ********* ********* ********* ********* ********* **********/


/*******************************************************************************
**	Functions
********************************************************************************/
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_open(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_close(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_setDeviceType(BTAudio_A2DP_Device_Type_e eType);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_codecInit(void);//only use after bt prepared
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_prepare(void);//only called by sub device
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_start(BTAudio_A2DP_Codec_e eCodec,BTAudio_A2DP_SampleRate_e eSampleRate);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_stop(void);
EXTERN void A2DP_AV_IN_A2DPInfoRecord(UINT16 codec_type,UINT32 sample_rate);
EXTERN void A2DP_AV_IN_A2DPInfoClear(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_write(BTAudio_A2DP_Data_t* pstData);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_setWaterLevel(UINT32 dWaterLevel);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_setDelayTime(UINT32 dDelay);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_Encode_prepare(BTAudio_A2DP_Encode_Info_t stInfo,
    BTAudio_A2DP_Callback_t* pstCbk);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_Encode_start(void);
EXTERN void BTAudio_A2DP_EncodeData_filter(UINT32 dBufSize);
EXTERN UINT32 BTAudio_A2DP_Encode_read(UINT8* pbBuf,UINT32 dBufSize);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_Encode_stop(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_Encode_setSync(UINT32 dIsSync);
EXTERN BTAudio_A2DP_Result_e BT_Barrot_A2DPConnectComplete_Action(t_bdaddr addr,BTAudio_A2DP_Codec_e eCodec,BTAudio_A2DP_SampleRate_e eSampleRate);
EXTERN BTAudio_A2DP_Result_e  BT_Barrot_A2DPDisconnectComplete_Action(t_bdaddr addr);
EXTERN BTAudio_A2DP_Result_e  BT_Barrot_AudioStreamStarted_Action(t_bdaddr addr,BTAudio_A2DP_Codec_e eCodec,BTAudio_A2DP_SampleRate_e eSampleRate);
EXTERN BTAudio_A2DP_Result_e  BT_Barrot_AudioStreamStopped_Action(t_bdaddr addr);
EXTERN BTAudio_A2DP_Result_e  BT_Barrot_LinkDisconnectComplete_Action(t_bdaddr addr);

#ifdef BT_A2DP_DATA_DUMP
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_dumpStart(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_dumpStop(void);
EXTERN UINT8 BTAudio_A2DP_isDumpData(void);
EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_dumpData(UINT8* pbBuf,UINT32 dBufLen);
#endif

void A2DP_AV_IN_setAllowAudioInFlag(UINT8 bTrueFalse);
BTAudio_A2DP_Result_e BTAudio_A2DP_codecInit(void);

EXTERN BTAudio_A2DP_Result_e BTAudio_A2DP_stop_DSPOVER(void);


#undef EXTERN

#endif /* ifndef __BT_AUDIO_A2DP_H__ */





