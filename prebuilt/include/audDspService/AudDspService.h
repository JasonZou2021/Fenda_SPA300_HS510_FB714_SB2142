/**************************************************************************/
/**
*
* \file AudDspService.h
*
* \brief audio dsp serivce provide APIs which moniters DSP status and
*        manipulates requests to DSP from other active objects
* \note Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*        All rights reserved.
* \note This code has been modified according to Sunplus Coding Regulation
*       Check Sunplus Coding Regulation v0.0.3 for the naming rules.
*
* \author
* \version v0.01
* \date 2016/05/24
***************************************************************************/
#ifndef AUD_DSP_SERVICE_h
#define AUD_DSP_SERVICE_h

/*---------------------------------------------------------------------------*
* INCLUDE DECLARATIONS
*---------------------------------------------------------------------------*/
//#include <stdio.h>
#include "types.h"
#include "qpc.h"
#include "qp_pub.h"
#include "wave_fmt.h"  //current 8107
#include "AudUserControl.h"

/*---------------------------------------------------------------------------*
* MACRO DECLARATIONS
*---------------------------------------------------------------------------*/
#define ODSP_ENCODER_WATCHDOG  0
#define ODSP_FORCE_MESSAGE     1
#define ODSP_FORCE_DSR         0
#define ODSP_FORCE_SYNC        1
#define ODSP_FORCE_ENDDING     1

#define ODSP_DEBUG_HINT_PCM    0

#define ODSP_DEBUG_PRINT_DSR   0
#define ODSP_DEBUG_PRINT_MSG   0
#define ODSP_DEBUG_PRINT_TIMER 0
#define ODSP_DEBUG_TIME_INIT   0

#define ODSP_FORCE_ALAC        0
#define ODSP_FORCE_SBC         0

#if  ODSP_DEBUG_PRINT_TIMER
	#define ODSP_DEBUG_PRINT_TIMER_SUB 1
#else
	#define ODSP_DEBUG_PRINT_TIMER_SUB 0
#endif

#define ODSP_TIMER_PLAYER 1
#define ODSP_TIMER_ADC    1
#define ODSP_TIMER_SPDIF  1 //50
#define ODSP_TIMER_I2S    1 //50
#define ODSP_TIMER_SINE   1 //50
#define ODSP_TIMER_HINT   2

#define CODING_MODE_WAV 255
#define CODING_MODE_FOR_RESET 0xFFFF   //update_160726

#define AUDIO_CHVOL_MUTE_DB	-140

typedef int32_t (*AudDdpServiceChk_CB_t)();

#define L_CH 					(1 << L)
#define R_CH 					(1 << R)
#define LS_CH 					(1 << Ls)
#define RS_CH 					(1 << Rs)
#define C_CH 					(1 << C)
#define SUB_CH 					(1 << Sub)
#define LTF_CH 					(1 << Ltf)
#define RTF_CH 					(1 << Rtf)
#define LRS_CH 					(1 << Lrs)
#define RRS_CH 					(1 << Rrs)
#define LTR_CH 					(1 << Ltr)
#define RTR_CH 					(1 << Rtr)
#define LW_CH 					(1 << Lw)
#define RW_CH 					(1 << Rw)
#define LTM_CH 					(1 << Ltm)
#define RTM_CH 					(1 << Rtm)

typedef enum
{
	ODSP_SPDIF_NONE = 0,
	ODSP_SPDIF_CHANNEL,
	ODSP_SPDIF_SAMPLERATE,
}eODSP_SPDIF_PARAM;




typedef enum
{
	ODSP_HINT_MT_GAIN_LOW = 0x0FFFFF, //0x7FFFFF unity
	ODSP_HINT_MT_GAIN_MED = 0x2FFFFF, //0x7FFFFF unity

	ODSP_HINT_HT_RISE_SHARP =  9,  //ramp_step
	ODSP_HINT_HT_RISE_FAST  = 10,  //ramp_step
	ODSP_HINT_HT_RISE_SLOW  = 11,  //ramp_step

	ODSP_HINT_HT_FALL_40MS  = 10,  //hs_ramp_up_block
	ODSP_HINT_HT_FALL_80MS  = 20,  //hs_ramp_up_block
	ODSP_HINT_HT_FALL_160MS = 40,  //hs_ramp_up_block

} eODSP_HINT_PARAMETER ;

typedef enum
{
	ODSP_ERR_WATCHDOG,
	ODSP_ERR_UNSUPPORT,  //carry coding_mode
	ODSP_ERR_STATE,     //update_160602  //carry state and signal
	ODSP_ERR_NO_CLIENT,
	ODSP_ERR_ADDRESS_UNDER,
	ODSP_ERR_ADDRESS_OVER,
} eODSP_ERR;

typedef enum
{
	ODSP_STATUS_INIT,
	ODSP_STATUS_IDLE,  //player report no data
	ODSP_STATUS_PREPARE,
	ODSP_STATUS_PREPARE0,
	ODSP_STATUS_LOADCODEC, //pdate_161221
	ODSP_STATUS_SILENCE,   //pdate_161221
	ODSP_STATUS_PLAY,
	ODSP_STATUS_PAUSE,
	ODSP_STATUS_SEEK,
	ODSP_STATUS_FINAL,
	ODSP_ENC_STATUS_INIT,   //zenatii
	ODSP_ENC_STATUS_IDLE,  //player report no data
	ODSP_ENC_STATUS_PREPARE,
	ODSP_ENC_STATUS_ENCODE,
	ODSP_ENC_STATUS_PAUSE,

	//Add spdif

	ODSP_SPDIF_STATUS_INIT,   //zenatii
    ODSP_SPDIF_STATUS_IDLE,  //player report no data
    ODSP_SPDIF_STATUS_PREPARE,
    ODSP_SPDIF_STATUS_ACTIVE,
	ODSP_SPDIF_STATUS_CHANGE,
} eODSP_STATUS;

typedef enum
{
	ODSP_STATUS_HINT_OFF = 0,
	ODSP_STATUS_HINT_ON
} eODSP_STATUS_HINT; //update_161201

//status for signal ODSP_FINISH_FILL_SIG
typedef enum
{
	//	ODSP_CONTAINER_TONE,
	ODSP_CONTAINER_WAV,
	ODSP_CONTAINER_AAC,
	ODSP_CONTAINER_M4A,
	ODSP_CONTAINER_MKV,
	ODSP_CONTAINER_BT,
	ODSP_CONTAINER_OTHERS,
	ODSP_CONTAINER_NON_WAV,  //player report no data
} eODSP_CONTAINER;

//status for signal ODSP_FINISH_FILL_SIG
typedef enum
{
	ODSP_ENUM_FILL_OK,
	ODSP_ENUM_FILL_OVER,  //player report no data
} eODSP_ENUM_FILL;

typedef enum
{
	ODSP_ENC_ENUM_RETRIEVE_OK,
} eODSP_ENC_ENUM_RETRIEVE; //zenatii

//type for signal ODSP_GET_PARAM_SIG
typedef enum
{
	ODSP_GET_ODSP_STATUS,       //return eODSP_STATUS
	ODSP_GET_CODING_MODE,       //return UINT64 (8107 Audcodec.h)
	ODSP_GET_BITRATE,           //return UINT32
	ODSP_GET_AUDIO_ENERGY,      //return UINT32
	ODSP_GET_SMP_RATE,          //return UINT16 (Auddef.h)
	ODSP_GET_SPDIF_MODE,        //return UINT8
} eODSP_GET_PARAM;

typedef enum
{
	ODSP_MSG_PREPARE_END = 0,
	ODSP_MSG_OVER,
	ODSP_MSG_ERR,
	ODSP_MSG_STATE_CHANGE,

	ODSP_ENC_MSG_PREPARE_END, //zenatii
	ODSP_ENC_MSG_OVER,
	ODSP_ENC_MSG_ERR,
	ODSP_ENC_MSG_STATE_CHANGE,

	ODSP_MSG_HINT //update_161201
} eODSP_MSG;  //update_160808

//type for signal ODSP_HINTSOUND_SIG
typedef enum
{
	ODSP_HINT_PLAY_PCM,
	ODSP_HINT_STOP,
} eODSP_HINTSOUND;

typedef enum
{
	ODSP_APP_EN = 100,
	ODSP_APP_MODE = 101,
} eODSP_APP_CMD;


typedef enum
{
	ODSP_CUSTOMTDM_DISABLE = 0,
	ODSP_CUSTOMTDM_ENABLE,
}eODSP_CUSTOMTDM_CTRL;

typedef enum
{
	ODSP_CUSTOMTDM_LR = 0,
	ODSP_CUSTOMTDM_LsRs,
	ODSP_CUSTOMTDM_LrsRrs,
}eODSP_CUSTOMTDM_Channel;

typedef enum _DSP_AppType {
	AppType_Reserved = 0,
	AppType_MATRIX,
	AppType_BIQUAD,
	AppType_Reserved1,
	AppType_VIRTUSALX,
	AppType_Reserved2,
	AppType_CHVOLUME,
	AppType_DRC,
	AppType_DELAY,
	AppType_BASS ,
	AppType_MASTERVOL,
	AppType_SS_III,
	AppType_SS_IV,
	AppType_MAX,
} eODSP_APP_TYPE;


//event for signal ODSP_GET_PARAM_FEEDBACK_SIG
typedef struct
{
	QEvt stEvt;
	eODSP_GET_PARAM eGetType;
	union
	{
		eODSP_STATUS eStatus; //ODSP_GET_ODSP_STATUS
		UINT32 dCodingMode;   //(8107 Audcodec.h)  //ODSP_GET_CODING_MODE
		UINT32 dBitRate;      //ODSP_GET_BITRATE,
		UINT32 dEnergy;       //ODSP_GET_AUDIO_ENERGY
		UINT16 dSmpRtae ;     //(Auddef.h) ODSP_GET_SMP_RATE
		//UINT8               //ODSP_GET_SPDIF_MODE
	};
} OdspGetResultEvt;

//event for signal ODSP_REPORT_CNT_SIG     //update_160811
typedef struct
{
	QEvt stEvt;
	//t_dec_sample_count;
	UINT32 dec_count_h;
	UINT32 dec_count_l;
	UINT32 fifo_count_h;
	UINT32 fifo_count_l;
} OdspReportCntEvt;

typedef struct
{
	QEvt stEvt;
	UINT8 bMsmartmode;
} OdspMsmartMEvt;

//event for signal ODSP_ERR_SIG
typedef struct
{
	QEvt stEvt;
	eODSP_ERR eStatus;
	//update_160602
} OdspErrEvt;
//event for signal ODSP_FILL_SIG
typedef struct
{
	QEvt stEvt;
	UINT32 dSize1;    //the size of first empty buffer
	UINT32 dSize2;    //the size of second empty buffer
	UINT8 *pbBuf1;   //the address of first empty buffer
	UINT8 *pbBuf2;   //the address of second empty buffer
} OdspFillEvt;

//event for signal ODSP_ENC_RETRIEVE_SIG
typedef struct
{
	QEvt stEvt;
	UINT32 dSize1;    //the size of first empty buffer
	UINT32 dSize2;    //the size of second empty buffer
	UINT8 *pbBuf1;   //the address of first empty buffer
	UINT8 *pbBuf2;   //the address of second empty buffer
} OdspEncRetrieveEvt;

typedef struct
{
	QEvt stEvt;
	eODSP_MSG eMsg;
	INT32 dParam;
} OdspMessageEvt;


typedef enum
{
	eODSP_SPDIF_Repeater = 0,
	eODSP_SPDIF_Onboard,

}eODSP_SPDIF_Input;


typedef enum
{
	SPDIF_IEC0 = 0x0,           // SPDIF_IN0
	SPDIF_IEC1 = 0x1,           // SPDIF_IN1
	SPDIF_IEC2 = 0x2,           // SPDIF_IN2
	SPDIF_HDMI_ARC = 0x3,       // SPA100 ARC pin
} eSPDIF_RX;

/*! audio input channel index */
typedef enum _AUD_IntAdcIdx_e
{
	AIN0 	= 0,		/*!< ADC Input AIN L0.R0 */
	AIN1	= 1,		/*!< ADC Input AIN L1.R1 */
	AIN2 	= 2,		/*!< ADC Input AIN L2.R2 */
} AUD_IntAdcIdx_e;

typedef enum
{
	IIS8CH_D0 = 0x0,            // IIS normal mode
	IIS8CH_D1 = 0x1,            // IIS normal mode
	IIS8CH_D2 = 0x2,            // IIS normal mode
	IIS8CH_D3 = 0x3,            // IIS normal mode
	IIS8CH_HDMISTD = 0x4,       // HDMI standard mode,use D0(1-lane)
	IIS8CH_HDMIHBR = 0x5,       // HDMI High BitRate mode,use D0~D3(4-lane)
} eIIS_8CH;

typedef union
{
	UINT32 Databyte;
	struct
	{
		unsigned TAD:4;
		unsigned reserved4_7:4;
		unsigned inselL:1;
		unsigned reserved9_11:3;
		unsigned inselR:1;
		unsigned reserved13_15:3;
		unsigned reserved16_23:8;
		unsigned reserved24_31:8;
	};

}IntADC_Config_t;

//VoiceAssist ctrl
typedef enum
{
	VA_Set_BGMGain,
	VA_Set_VAGain,
	VA_Get_BGM_Enr,
	VA_Get_VA_Enr,
	VA_Set_MIXCh,
} VA_CtrlID;

typedef struct _Servicer_VA_param
{
	UINT16 Ch_L_bitmark_MIXER0;
	UINT16 Ch_R_bitmark_MIXER0;
	UINT16 Ch_L_bitmark_MIXER1;
	UINT16 Ch_R_bitmark_MIXER1;
	UINT32 VASource_Gain;
	UINT32 VASource_Ramp_step;
	UINT32 VASource_Duck_Gain;
}Servicer_VA_Param_t;

typedef struct Dsp_Format_Info
{
    UINT16 Audio_Format;
    UINT16 Audio_type;
}Dsp_Format_Info_t;


typedef enum _CIS_UpMix_Type
{
	Cis_UpMix_OFF,
	Cis_UpMix_2_1,
}CIS_UpMix_Type;

typedef enum _CIS_UpMix_Mode
{
	CIS_UpMix_2CH = 0,
	CIS_UpMix_MCH,
	CIS_UpMix_DAP,
	CIS_UpMix_NERUALX,
	CIS_UpMix_VIRTUALX,
	CIS_UpMix_MAX,
}CIS_UpMix_Mode;

typedef enum
{
	SPKCONFIG_LR_BIT 		= 1<<0,
	SPKCONFIG_LSRS_BIT 		= 1<<1,
	SPKCONFIG_SUB_BIT 		= 1<<2,
	SPKCONFIG_C_BIT	 		= 1<<3,
	SPKCONFIG_LRSRRS_BIT 	= 1<<4,
	SPKCONFIG_LTFRTF_BIT 	= 1<<5,
	SPKCONFIG_LTRRTR_BIT 	= 1<<6,
	SPKCONFIG_LWRW_BIT 		= 1<<7,
	SPKCONFIG_LTMRTM_BIT 	= 1<<8,
	SPKCONFIG_MASK_BIT 		= ((1<<9)-1),
}CIS_SpkConfig_bit_e;

typedef enum
{
	HDSP_AUDIO_NORMAL,
	HDSP_AUDIO_CIS,

	HDSP_AUDIO_MAX,
}Hdsp_Audio_Flow;

typedef enum _SmartModeParamType
{
	TYPE_SMART_PARAM_CTRL = 0,
	TYPE_SMART_PARAM_FC,
}MSmodeParamTp_e;

typedef enum
{
	DTSX_SETTING_RAWDEC	= 0,
	DTSX_SETTING_VX,
	DTSX_SETTING_MAX,
}Hdsp_DTSX_setting;

typedef enum
{
	DTSX_SETTING_RAWDEC_BIT	= (1<<DTSX_SETTING_RAWDEC),
	DTSX_SETTING_VX_BIT		= (1<<DTSX_SETTING_VX),
}Hdsp_DTSX_setting_bit;


typedef enum {
	DRC_CHANNEL_ACTIVE = 0,
	DRC_MODE,						///< DRC full band ro two band

	DRC_RATIO_L = 3,			        ///< DRC ratio
	DRC_LIMIT_L,						///< DRC limiter
	DRC_COMPRESSOR_L,					///< DRC Compressor
	DRC_OFFSET_L,					    ///< DRC offset
	DRC_ATTACK_L,		//< DRC  attack time
	DRC_RELEASE_L,	//< DRC  release time

	DRC_RATIO_H = 10,			        ///< DRC ratio High band
	DRC_LIMIT_H,						///< DRC limiter High band
	DRC_COMPRESSOR_H,					///< DRC Compressor  High band
	DRC_OFFSET_H,					    ///< DRC offset High band
	DRC_ATTACK_H,		//< DRC  attack time High band
	DRC_RELEASE_H,	//< DRC  release time High band

} DRC_TYPE;

typedef enum
{
	AUDDSP_DEMUTE 	= 0,
	AUDDSP_MUTE 	= 1,
}AudDsp_Mute_e;

typedef enum
{
	ODSP_SPDIF_IEC0 = 0x0,
	ODSP_SPDIF_IEC1 = 0x1,
	ODSP_SPDIF_IEC2 = 0x2,
	ODSP_SPDIF_HDMI_ARC = 0x3,
    ODSP_SPDIF_8CHIIS_D0   = 0x4,
    ODSP_SPDIF_8CHIIS_ALL  = 0x5,
} eODSP_SPDIF_RX;



/*---------------------------------------------------------------------------*
 * GLOBAL DATA
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * EXTERNAL DECLARATIONS
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 * FUNCTION DECLARATIONS
 *---------------------------------------------------------------------------*/
void AudDspService_Enc_Prepare_Bt(void (*pfMsg)( eODSP_MSG msg, INT32 param), void (*pfRetrieve)(UINT32, UINT32, UINT8 *, UINT8 *), UINT8 bEncodeMode,
								  void *pstEncCommuParameter);  //OdspEncCommuParameter
void AudDspService_Enc_Flow_Encode(QActive *pSender);
void AudDspService_Enc_Flow_Pause(QActive *pSender);
void AudDspService_Enc_Flow_Stop(QActive *pSender);
void AudDspService_Enc_Data_FinishRetrieve(QActive *pSender, UINT8 *pbAddrCurr, UINT32 dSizeFill, UINT32 dSizeNotFill, eODSP_ENC_ENUM_RETRIEVE  eStatus) ;
void AudDspService_Enc_Set_BufMin(QActive *pSender, UINT32 dMin)              ;
void AudDspService_Enc_Set_Sync(QActive *pSender, UINT32 dSync)              ;

void AudDspService_Prepare_Tone(QActive *pSender);
void AudDspService_Prepare_2CH_IISIN(QActive *pSender);
void AudDspService_Prepare_8CH_IISIN(QActive *pSender, eIIS_8CH bIISFormat);
void AudDspService_Prepare_SINE(QActive *pSender);
void AudDspService_Prepare_ADC(QActive *pSender, AUD_IntAdcIdx_e ADC_Input);
void AudDspService_Prepare_Player0(QActive *pSender, eODSP_CONTAINER eContainerType, UINT32 dCodingMode, WAVFORMATMACRO *stWmf);
void AudDspService_Prepare_PlayData(void (*pfMsg)( eODSP_MSG msg, INT32 param), void (*pfFill)(UINT32, UINT32, UINT8 *, UINT8 *), eODSP_CONTAINER eContainerType, UINT32 dCodingMode, WAVFORMATMACRO *stWmf); //update_160808
#ifdef NEW_PREPARE_BT_FUNC
void AudDspService_Prepare_Bt(void (*pfMsg)( eODSP_MSG msg, INT32 param), void (*pfFill)(UINT32, UINT32, UINT8 *, UINT8 *),   UINT32 dCodingMode, WAVFORMATMACRO *stWmf);
#else
void AudDspService_Prepare_Bt(void (*pfMsg)( eODSP_MSG msg, INT32 param), void (*pfFill)(UINT32, UINT32, UINT8 *, UINT8 *),   UINT32 dCodingMode,   UINT32 dPcmFreq ); //update_160810
#endif
void AudDspService_Prepare_SPDIF(QActive *pSender, eSPDIF_RX bSpdifRx);
void AudDspService_Prepare_PDMIN0(QActive *pSender);
void AudDspService_Prepare_PDMIN1(QActive *pSender);
void AudDspService_Set_SourceGain(QActive *pSender, float fdB);
void AudDspService_Flow_Play(QActive *pSender);
void AudDspService_Flow_Pause(QActive *pSender);
void AudDspService_Flow_Stop(QActive *pSender);
void AudDspService_Flow_Seek(QActive *pSender);
void AudDspService_Flow_Continue(QActive *pSender);
void AudDspService_Flow_FastForward(QActive *pSender, UINT8 bSpeed);
void AudDspService_Flow_Reset(QActive *pSender);
void AudDspService_Flow_Terminate(QActive *const pSender, void *pParam);

void AudDspService_Data_FinishFill(QActive *pSender, UINT8 *pbAddrCurr, UINT32 dSizeFill, UINT32 dSizeNotFill, eODSP_ENUM_FILL  eStatus, INT16 Cnt);
void AudDspService_Get_Status(QActive *pSender);
UINT32 AudDspService_VoiceAssist_Contrl(QActive *pSender, VA_CtrlID ctrid,UINT32 p0, UINT32 p1);

/** @brief Reload Dsptool Xml from pData(Note this data is compressed)
 * @param Datasize: compressed data size
  */
//void AudDspService_Set_DspToolXml(void *pSender, UINT8* pData,UINT32 Datasize);

void AudDspService_Set_Mute(QActive *pSender);
void AudDspService_Set_DeMute(QActive *pSender);

void AudDspService_Set_Matrix(QActive *pSender, UINT16 wIdx, UINT16 wOutCh, UINT16 wInCh, float fdB);
void AudDspService_Set_Bass(QActive *pSender, UINT16 BassGRP_number, UINT16 Target_Channel, UINT16 Target_Band, float Target_dB);
/** @brief: Set Drc param_value(Currently only used for disable/enable DRC or set channel active)
 * 	@param QActive *pSender: Caller AO pointer
 * 	@param  wIdx: which num of DRC index
 * 	@param  wCmd:sets 100 or the following enum \n
 *		typedef enum {
 *			DRC_CHANNEL_ACTIVE ,
 *			DRC_MODE,		//< DRC full band ro two band
 *			DRC_RATIO_L ,			//< DRC ratio
 *			DRC_LIMIT_L,				//< DRC limiter
 *			DRC_COMPRESSOR_L,		//< DRC Compressor
 *			DRC_OFFSET_L,			 //< DRC offset
 *			DRC_ATTACK_L,		//< DRC  attack time
 *			DRC_RELEASE_L,	//< DRC  release time
 *		} DRC_TYPE; \n
 *	@param float Value:
 *	wCmd:enable or disable this DRC:Value range 0 or 1(disable/enable)
 *	wCmd = DRC_CHANNEL_ACTIVE:Value range is 0x1~0xffff,bit0/bit1…bit14/bit15=>L/R…Ltm/Rtm.if this bit is 1,this channel active
 *	wCmd = DRC_RATIO_L: Value range is 0~100,step is 1
 *	wCmd = DRC_LIMIT_L:Value range is -40 ~ 0 ,step is 0.1
 *	wCmd = DRC_ COMPRESSOR _L:Value range is -40 ~ 0 ,step is 0.1
 *	wCmd = DRC_LIMIT_L:Value range is 0 ~20 ,step is 0.1
 *	wCmd = DRC_ ATTACK _L:Value range is 0.5 ~500 ,step is 0.1
 *	wCmd = DRC_ RELEASE _L:Value range is 0.5 ~500 ,step is 0.1
 *
 * */
void AudDspService_Set_DRC(QActive *pSender, UINT16 wIdx, DRC_TYPE wCmd, float dValue);
void AudDspService_Set_Delay(QActive *pSender, UINT16 wIdx, UINT16 wChannel, UINT16 wMs);

/** @brief: Set Filter EQ_GaindB_value (For the time being,the APIs is only used to set Filter EQ_gain)
 * 	@param wIdx :[in]
 * 	@param cmd :[in]
 * 	@param wChannel :[in] range :0 ~ 15,L/R.../Ltm/Rtm
 * 	@param wBand :[in] range :0 ~ 19,band1/band2.../band19/band20
 * 	@param wdB :[in] :
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  cmd - 	cmd describtion - 			 		wdB range
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  0   - 	filter type		- 	0:All_pass,1:Bass,2:Treble,3:EQ,4:Low_pass,5:High_pass
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  1   - 	FREQ 			- 	20~20K HZ or 0. if Freq = 0 means disable this band
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  2   - 	Gain			- 	-18 ~ +18dB,step = 0.1
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  3   - 	Q				- 	0.5 ~ 10 ,step = 0.1
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  4   - 	Phase			- 	0:0 degree,1:180 degree
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  5   - 	Slope			- 	0:slope is 6,1:slope is 12
 * 		----------------------------------------------------------------------------------------- \n
 * 		-  6   - 	Method			- 	0:Buterworth,1:Linkwitz-Riley
 * 		----------------------------------------------------------------------------------------- \n
 *
 * 	@note if want to enable/disable this app ,cmd must set to 100 and wChannel set to 0 or 1(disable or enable)
 * 	@return none:
 */
void AudDspService_Set_Filter(QActive *pSender, UINT16 wIdx, INT16 cmd, UINT16 wChannel, UINT16 wBand, float wdB);
void AudDspService_Set_Filter_SyncMode(QActive *pSender, UINT16 wIdx, INT16 cmd, UINT16 wChannel, UINT16 wBand, float wdB);
void AudDspService_Set_AppMode(QActive *pSender, eAUD_POST_PROCESSING type,UINT16 wTargetApp, eAUD_POST_PROCESSING_MODE mode);


/** @brief set App channel volume group target gain
 *
 *  @param pSender :[in]
 *  @param ChVolGRP_numbem :[in] Which number in the ChannelVolume group you want to set (starting from 1)
 *  @param Channel :[in] Target channel Idx [L/R...Ltm/Rtm]->[0/1...14/15]
 *  @param Target_dB :[in] -103.5 ~ 0 dB(step = 0.1)
 *  @return none
 */
void AudDspService_Set_ChannelVol(QActive *pSender, UINT16 ChVolGRP_number, UINT16 Channel, float Target_dB);
/** @brief set App Master volume group target gain
 *
 *  @param pSender :[in]
 *  @param ChVolGRP_numbem :[in] Which number in the MasterlVolume group you want to set
 *  @param Target_dB :[in] -103.5 ~ 0 dB(step = 0.1)
 *  @return none
 */
void AudDspService_Set_MasterVol(QActive *pSender, UINT16 MasterVolGRP_number, float Target_dB);

/** @brief CIS Set Mute ramp step (Set master volume to -140dB to MUTE),During the time of dStep,Gain is set place
 * 	@param dStep targetStep(unit:MS)
 */
void AudDspService_CIS_Ramp_Step(QActive *pSender, UINT32 dStep);

/** @brief CIS set upmix (input 2.0 ,output 2.1,Use MartixMix support)
 *  @param pSender :[in]
 * 	@param type:[in] upmix type.0:dsiable upmix, 1:CIS_UpMix_2_1
 * 	@param Lratio_dB:[in] Mix how many dB of L-CH into Sub-CH range[-140~0dB,step=0.1]
 * 	@param Rratio_dB:[in] Mix how many dB of R-CH into Sub-CH range[-140~0dB,step=0.1]
 * 	@return none
 */
void AudDspService_CIS_Up_Mix(QActive *pSender, CIS_UpMix_Type type, float Lratio_dB,float Rratio_dB);

void AudDspService_Set_BypassMode(QActive *pSender, UINT8 bEnable); //AudDspService_Set_BypassMode(UINT8 bEnable)
void AudDspService_Set_SurroundMode(QActive *pSender, UINT8 bParam, UINT32 dVal);
void AudDspService_Set_SpeakerOutDelay(QActive *pSender, UINT8 bX); //AudDspService_Set_Speaker_Out_Delay(UINT8 bX)
void AudDspService_Set_Output(QActive *pSender, UINT16 wMode);             //_CLI_
void AudDspService_Set_BtLevel(QActive *pSender, UINT16 wLevel) ;
void AudDspService_Set_Test(QActive *pSender, UINT16 wParam0, UINT16 wParam1) ;
void AudDspService_Set_WatchDog_Disable(QActive *pSender, UINT16 wParam1);
//void AudDspService_Set_HdpState_Listen(QActive *const pSender, void *pParam)       ;
void AudDspService_Set_BufMin(QActive *pSender, UINT32 dMin)                 ;
void AudDspService_Set_PostProcessing(QActive *pSender, eAUD_POST_PROCESSING ePPType,
									  eAUD_POST_PROCESSING_MODE ePPMode);
void AudDspService_Set_CIS_PARAM(QActive *pSender, eAUD_POST_PROCESSING ePPType,
									 UINT16 nval1, UINT16 nval2, UINT16 nval3);

void AudDspService_Set_FixedSampleRate(QActive *pSender, UINT8 enable);

void AudDspService_Set_IISFormat(QActive *pSender, UINT8 id, UINT8 cfg, UINT16 value);


//DiableC
void AudDspService_Dsr( QActive *pSender, UINT32 dParam0)                    ; //somersby
void AudDspService_Cli_Sync( QActive *pSender, UINT8 bItem)                  ; //ODSP_CLI_SYNC, //OdspCliSyncEvt;
void AudDspService_Cli_Lock( QActive *pSender, UINT8 bEnable)                ; //ODSP_CLI_LOCK, //OdspCliLockEvt;
void AudDspService_Cli_Pc( QActive *pSender)                                 ; //ODSP_CLI_PC,
void AudDspService_Cli_State( QActive *pSender)                              ; //ODSP_CLI_STATE,
void AudDspService_Cli_Codec( QActive *pSender)                              ; //ODSP_CLI_CODEC,
void AudDspService_Cli_Free( QActive *pSender)                               ; //ODSP_CLI_FREE,
void AudDspService_Cli_Port( QActive *pSender, UINT8 bPort)                  ; //ODSP_CLI_PORT, //OdspCliPortEvt;
void AudDspService_Cli_Rdm( QActive *pSender, UINT32 dOffset, INT32 sdLength); //ODSP_CLI_RDM,  //OdspCliRdmEvt;
void AudDspService_Cli_Wdm( QActive *pSender, UINT32 dXoff, UINT32 dValue)   ; //ODSP_CLI_WDM,  //OdspCliWdmEvt;
void AudDspService_Cli_Cnt( QActive *pSender)                                ; //ODSP_CLI_CNT,
void AudDspService_Cli_Ff( QActive *pSender, UINT8 bSpeed)                   ; //ODSP_CLI_FF,   //OdspCliFfEvt;
void AudDspService_Cli_Play( QActive *pSender)                               ; //ODSP_CLI_PLAY,
void AudDspService_Cli_Pause( QActive *pSender)                              ; //ODSP_CLI_PAUSE,
void AudDspService_Cli_Stop( QActive *pSender)                               ; //ODSP_CLI_STOP,
void AudDspService_Cli_Continue( QActive *pSender)                           ; //ODSP_CLI_CON,
void AudDspService_Cli_Ep( QActive *pSender)                                 ; //ODSP_CLI_EP,
void AudDspService_Cli_Re( QActive *pSender)                                 ; //ODSP_CLI_RE,
void AudDspService_Cli_Io( QActive *pSender, UINT32 dPort0, UINT32 dPort1)   ; //ODSP_CLI_IO,
void AudDspService_Cli_Info( QActive *pSender, UINT16 bType);
void AudDspService_Cli_Msg( QActive *pSender )                               ; //ODSP_CLI_MSG
void AudDspService_Cli_Pause_RE( QActive *pSender, void * func,UINT32 param)                              ; //ODSP_CLI_PAUSE_RE,

INT64 AudDspService_Get_DecPTS(void);
void AudDspService_Set_FF_FB_EndingCtrl(QActive *pSender);
INT64 AudDspService_Get_FifoPTS(void);
UINT32 AudDspService_Get_BufferResidue(void);
UINT32 AudDspService_Get_BufferEmpty(void);
void AudDspService_Get_OutputEnergy(int *eng_vec);

/** @brief Get the input energy(only used by USB/CARD/ADC-IN source)
 * @param eng_vec_L [out]: return input L energy
 * @param eng_vec_R [out]: return input R energy
*/
void AudDspService_Get_InputEnergy(uint32_t *eng_vec_L, uint32_t *eng_vec_R);
UINT8 AudDspService_Spdif_Detect();


/** @brief Get the spdif input energy(only used by OPT/COAX/HDMI/ARC source)
 * @param eng_arrv [out]: return input L energy
 * @note : This API is to get SPDIF/HDMI each channel energy,only if the input is PCM \n
 * 		if OPT source:
		uint32_t arrv[2]; //arrv[0] is L-CH,arrv[1] is R-CH
		AudDspService_Get_ SPDIF _IntputEnergy (arrv); \n
		if HDMI source:
		uint32_t arrv[8];//arrv[0]/arrv[1]/arrv[2]…=>L/R/Ls…
		AudDspService_Get_ SPDIF _IntputEnergy (arrv); \n
*/
void AudDspService_Get_SPDIF_InputEnergy(uint32_t *eng_arrv);


UINT32 AudDspService_Enc_Get_BufferResidue(void);
UINT32 AudDspService_Enc_Get_BufferEmpty(void);
void AudDspService_CLI(UINT8 *pbFunctionName);
void processCLI(UINT8 *pbFunctionName);
void AudDspService_Sine_Set(QActive *pSender, UINT8 enable, UINT32 samplerate);
void AudDspService_Set_Sub_Mute(QActive *pSender);
void AudDspService_Set_Sub_Demute(QActive *pSender);


void AudDspService_Cli_BIo( QActive *pSender, UINT32 dPort0, UINT32 dPort1); //ODSP_CLI_BIO
void AudDspService_Set_OutputMode(QActive *pSender, UINT8 outmode, UINT16 outparam);             //_CLI_

QActive *SpOdspGet(void);

/* broadcast audio format change  */
int AudDspService_RegAudioInfoBroadcastListener(QActive* const me, ARFunc_t pfFunc);
int AudDspService_UnRegAudioInfoBroadcastListener(QActive* const me);
void AudDspService_AudioInfoBroadcastFree(void *pParam);
int AudDspService_AudioInfoBroadcastRequest(void const *pSender,Dsp_Format_Info_t* pstAudioCodecInfo);



//Add spdif
void AudDspService_SPDIF_SetParam(QActive *pSender, UINT32 type, UINT32 channel, UINT32 ch_mapping);
void AudDspService_SPDIF_PLAY(QActive *pSender);
void AudDspService_SPDIF_STOP(QActive *pSender);
void AudDspService_SPDIF_Debug(QActive *pSender);


//#define ENABLE_LISTENER
#ifdef ENABLE_LISTENER
/**
 *\brief media player notify listener struct declaration
 */
typedef struct spODSPListener_s
{
	void *user;
	void (*notify)(void *user, eODSP_MSG msg, INT32 param);
} spODSPListener_t;

void SpOdspRegListener(QActive *super, spODSPListener_t *listener);
void SpOdspUnregListener(QActive *super);

#endif

// for SPA100 CEVA decoder cert
typedef struct
{
	unsigned char output_mode;
	unsigned char stereo_dnmix_mode;
}sp_dts_option_s;

typedef struct
{
	unsigned char drc_mode;
	unsigned char lfe_mode;
	unsigned char stereo_dnmix_mode;
	unsigned char output_mode;
}sp_ddp_option_s;

// for Hdsp

typedef struct
{
	// static parameters
	unsigned char dap_enable;

    unsigned char direct;
    unsigned char tuning_mode;
    unsigned char config_file;
    unsigned char config_virt_enable;
    unsigned char virt_mode;
	unsigned char virt_frontangle;
    unsigned char virt_surrangle;
    unsigned char virt_heightangle;
    unsigned char virt_rearsurrangle;

	// runtime parameters
	unsigned char b_postproc;
	unsigned char profile_id;								  /** Dolby Audio Processing profile (default value: 0) 					 */
	unsigned char b_vlamp;										  /** If set, Volume Leveler output target level will be set to -20.0 dBFS (default value: 1)  */
	unsigned char b_enable_upmixer; 				  /** If set, channel-based content is upmixed. (default value: 1)			  */
	unsigned char b_center_spread;						  /** If set, enable center spreading in DAP. (default value: 0)			  */
	unsigned char b_height_filter;						  /** If set, enable height filter in DAP. (default value: 0)				  */
	unsigned char b_enable_loudness_mgmt;		  /** If set, enable loudness management in the Dolby TrueHD decoder.  (default value: 1) */
	unsigned char drc_mode;									  /** Dynamic Range Control setting for decoders. (default value: 1) */
	unsigned char drc_cut; 								  /** Requested DRC cut amount, on [0.0, 1.0]. (default value: 1.0) */
	unsigned char drc_boost;						  /** Requested DRC boost amount, on [0.0, 1.0]. (default value: 1.0) */
}sp_atmos_option_s;

typedef struct
{
	// static parameters
	unsigned int speaker_config;
	unsigned char Nerualx;
	unsigned char analog_compensation;
	unsigned char Decode_96_from_192;

	// runtime parameters
	unsigned short DRC;
	unsigned char direct_mode;
	unsigned char dialog_control;
	unsigned char disableT1relabel;
}sp_dtsx_option_s;

void SpHdsp_Set_DownmixConfig(QActive * pSender,unsigned int config);
void SpHdsp_Set_UpmixConfig(QActive * pSender,unsigned int config);
void SpHdsp_Set_UpmixMode(QActive *pSender, unsigned int mode);
void SpHdsp_Change_Format(QActive *pSender, int audio_format);
void SpHdsp_Reload(QActive *pSender, int dspid);
void SpHdsp_loaddspcode(QActive *pSender, int codec);
void SpHdsp_PowerOn(QActive *pSender, int dspid);
void SpHdsp_PowerOff(QActive *pSender, int dspid);
void SpHdsp_Play(QActive *pSender, int dspid);
void SpHdsp_Stop(QActive *pSender, int dspid);
void SpHdsp_Reset(QActive *pSender, int dspid);
void SpHdsp_param(QActive *pSender);
void SpHdsp_setDecOutput(QActive *pSender, int value1);
void SpHdsp_setAtmos(QActive *pSender, int Idx, int value1, int value2);
void SpHdsp_setDtsX(QActive *pSender, int Idx, int value1);
void SpHdsp_setPCM(QActive *pSender, int Idx, int value1);
void SpHdsp_getAudioFormat(int *ch_num, int *samplerate, int *format);
void SpHdsp_getAtmos(QActive *pSender);
void SpHdsp_getDtsX(QActive *pSender);
void SpHdsp_getAtmosRetValue(QActive *pSender, unsigned int *ret_value);
void SpdHdsp_set_Audio_Flow(QActive *pSender, unsigned int index);
void SpdHdsp_DTSX_VX_setting(QActive *pSender, unsigned int index, unsigned int value);
unsigned int SpHdsp_getDSPHStatus(void);

//ddp
void spAudapi_ddpout(UINT16 dP0);
void spAudapi_ddpdrc(UINT16 dP0);
void spAudapi_ddplfe(UINT16 dP0);
void spAudapi_ddpstereo(UINT16 dP0);
void spAudapi_dspmode_change(UINT16 dP0);
void spAudapi_ramp(UINT32 dP0, UINT32 dP1);

void AudDspService_Set_CustomTDM(QActive *pSender,UINT8 enable, UINT8 channel);

/*****************************************************************************
 *
 * \fn 			void AudDspService_Set_Dsp_MuteSyncmode(QActive *pSender, UINT8 bMute)
 *
 * \brief 		Send Mute or Demute signal to DSP-C and wait for dsp ack
 *
 * \param [in] 	pSender:
 * \param [in] 	bMute: 0: demute, 1: mute
 * \return 		none
 *
 * \note
 *
 ******************************************************************************/
void AudDspService_Set_Dsp_MuteSyncmode(QActive *pSender, UINT8 bMute);


/** @brief enable/disable smart mode
 * 	@param flag:[in] 0:disable,1:enable
 */
void AudDspService_Set_Classifier(QActive *pSender, UINT8 bEnable);

void AudDspService_Set_OutSampleRate(QActive *pSender, UINT32 value);
void AudDspService_Set_InSampleRate(QActive *pSender, UINT32 value);



void AudDspService_Prepare_VA(QActive *pSender, UINT8 ADC_Input, UINT8 bMixer);

/** @brief Get the format of the current play
 * @return return decoder1 codecid
 */
UINT16 AudDspService_Get_CurrFormat(QActive *pSender);

void AudDspService_Set_MediaEnable(QActive *pSender);

void AudDspService_Set_SoundMode(QActive *pSender,UINT32 Src_id);

void AudDspService_Set_VirtualX(QActive *pSender, UINT16 wIdx, UINT16 wCmd, INT32 dValue);

void AudDspService_Set_FIFO_Level(QActive *pSender, UINT16 wblock);

void AudDspService_Set_ADC_Channel_Swap(QActive *pSender, UINT16 wParam0); // wParam0: 0 :Swap L/R Channel |  1:Normal



UINT32 VAdBtoLineQ527(double dB);

/** @brief: Get pcm channel infomation at DSPH input
 * 	@param pSender :
 * 	@param src_channel_map 	:[in] get input source channel mapping Bit 0~15 is L/R/Ls/Rs/Center/LFE/Lrs/Rrs/Ltf/Rtf/Ltr/Rtr/Lw/Rw/Ltm/Rtm
 * 	@param out_channel_map	:[in] get out channel mapping, Bit 0~15 is L/R/Ls/Rs/Center/LFE/Lrs/Rrs/Ltf/Rtf/Ltr/Rtr/Lw/Rw/Ltm/Rtm
 * 	@param samplerate		:[in] get pcm sampling rate
 * 	@return none:
 */

void AudDspService_Get_Channel_Info(QActive *pSender, uint16_t *src_channel_map, uint16_t *out_channel_map, uint32_t *samplerate);

int AudDdpService_callback_register(AudDdpServiceChk_CB_t pstDesActive);

#endif /* AUD_DSP_SERVICE_h */
