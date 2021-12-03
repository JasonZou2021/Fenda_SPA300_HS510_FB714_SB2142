
#include <stdio.h>
#include <string.h>
#define LOG_MODULE_ID ID_APUSR
#include "log_utils.h"
#include "dsp_hint_sound.h"
#include "qpc.h"
#include "userapp.h"
#include "project_cfg.h"

#define hint_printf(f, a...) LOGD(f, ##a)

#if (SUPPORT_FW_VERSION)
DSP_HINT_SOUND_P * getVerNumSrcData(char versionNum)
{
	DSP_HINT_SOUND_P *VerNumSrc;
	switch(versionNum)
	{
		case '0':{
			VerNumSrc = hint_data_map[eHintData_Zero];
			break;
		}
		case '1':{
			VerNumSrc = hint_data_map[eHintData_One];
			break;
		}
		case '2':{
			VerNumSrc = hint_data_map[eHintData_Two];
			break;
		}
		case '3':{
			VerNumSrc = hint_data_map[eHintData_Three];
			break;
		}
		case '4':{
			VerNumSrc = hint_data_map[eHintData_Four];
			break;
		}
		case '5':{
			VerNumSrc = hint_data_map[eHintData_Five];
			break;
		}
		case '6':{
			VerNumSrc = hint_data_map[eHintData_Six];
			break;
		}
		case '7':{
			VerNumSrc = hint_data_map[eHintData_Seven];
			break;
		}
		case '8':{
			VerNumSrc = hint_data_map[eHintData_Eight];
			break;
		}
		case '9':{
			VerNumSrc = hint_data_map[eHintData_Nine];
			break;
		}
		case '.':{
			VerNumSrc = hint_data_map[eHintData_Dot];
			break;
		}
		default:{
			VerNumSrc = NULL;
			break;
		}
	}

	return VerNumSrc;
}

int FwVersionProc()
{
	int ret;
	int i = 0;
	char *verStr;
	DSP_HINT_SOUND_P *pCurHint;

	verStr = getFwVersion();
	int verLen = strlen(verStr);
	hint_printf("verStr:%s, verLen:%d\n", verStr, verLen);

	char verChar[verLen+1];
	for(i = 0; i < verLen; i++)
	{
		verChar[i] = verStr[i];
		hint_printf("%c\n",verChar[i]);
		pCurHint = getVerNumSrcData(verChar[i]);
		if(pCurHint == NULL)
		{
			hint_printf("Version hint data error!\n");
			return -1;
		}
		ret = Hint_SingleDataAdd(pCurHint, DEFAULT_INTEVAL_TIME);
		if(ret < 0)
		{
			hint_printf("hint data init error\n");
			return ret;
		}
	}
	return 0;
}
#endif

#if (SUPPORT_INFO)
int InfoProc()
{
	int ret = 0;
	DSP_HINT_SOUND_P *pHintCnf;
	int src_id = getCurUserSrc();
	switch(src_id){
		case eAp_USB_Src:
			pHintCnf = hint_data_map[eHintData_SrcUSB];
			break;
		case eAp_CARD_Src:
			pHintCnf = hint_data_map[eHintData_SrcCard];
			break;
#if (!SUPPORT_VA)
		case eAp_AUX_Src:
			pHintCnf = hint_data_map[eHintData_SrcAux];
			break;
#endif
		case eAp_LINE_Src:
			pHintCnf = hint_data_map[eHintData_SrcLine];
			break;
		case eAp_I2S_Src:
			pHintCnf = hint_data_map[eHintData_SrcI2S];
			break;
		case eAp_SPDIF_Optical_Src:
			pHintCnf = hint_data_map[eHintData_SrcOpt];
			break;
#if defined(CFG_COAX_ENABLE) && (CFG_COAX_ENABLE == 1)
		case eAp_SPDIF_Coaxial_Src:
			pHintCnf = hint_data_map[eHintData_SrcCoax];
			break;
#endif
		case eAp_ARC_Src:
			pHintCnf = hint_data_map[eHintData_SrcARC];
			break;
#ifdef USERAPP_SUPPORT_REPEATER
		case eAp_HDMI0_Src:
			pHintCnf = hint_data_map[eHintData_SrcHDMI0];
			break;
		case eAp_HDMI1_Src:
			pHintCnf = hint_data_map[eHintData_SrcHDMI1];
			break;
		case eAp_HDMI2_Src:
			pHintCnf = hint_data_map[eHintData_SrcHDMI2];
			break;
#endif
#ifdef USERAPP_SUPPORT_BT
		case eAp_Bt_Src:
			pHintCnf = hint_data_map[eHintData_SrcBT];
			break;
#endif
		default:
			hint_printf("src error!\n");
			return -1;
			break;
	}
	if(pHintCnf == NULL)
	{
		hint_printf("src hint data error!\n");
		return -1;
	}
	ret = Hint_SingleDataAdd(pHintCnf, DEFAULT_INTEVAL_TIME);
	if(ret < 0)
	{
		hint_printf("hint data init error\n");
		return ret;
	}

	hint_printf("getAudioFormat=%x\n",getAudioFormat());

	UINT16 codec_type = getAudioFormat();
    if(codec_type)
    {
        switch((codec_type & 0xff))
        {
			case AUDIO_FORMAT_NONE:
				hint_printf("NO Audio Signal\n");
				return ret;
			case AUDIO_FORMAT_PCM:
	        case AUDIO_FORMAT_PCM_MCH:
	        {
				pHintCnf = hint_data_map[eHintData_PCM];
	            break;
	        }
	        case AUDIO_FORMAT_DOLBY_DIGITAL:
	        {
				pHintCnf = hint_data_map[eHintData_DolbyDigital];
	            break;
	        }
			case AUDIO_FORMAT_DD_PLUS:
	        {
				pHintCnf = hint_data_map[eHintData_DolbyDigitalPlus];
	            break;
	        }
			case AUDIO_FORMAT_DOLBY_TRUEHD:
	        {
				pHintCnf = hint_data_map[eHintData_DolbyTrueHD];
	            break;
	        }
			case AUDIO_FORMAT_DOLBY_MAT:
	        {
				pHintCnf = hint_data_map[eHintData_DolbyMAT];
	            break;
	        }
			case AUDIO_FORMAT_DTS:
	        {
				pHintCnf = hint_data_map[eHintData_DTSDigitalSurr];
	            break;
	        }
			case AUDIO_FORMAT_DTS_HD:
	        {
				pHintCnf = hint_data_map[eHintData_DTSDigitalSurr];
	            break;
	        }
			case AUDIO_FORMAT_DTS_X:
	        {
				pHintCnf = hint_data_map[eHintData_DTSDigitalSurr];
	            break;
	        }
			case AUDIO_FORMAT_HDCD:
			case AUDIO_FORMAT_DSD:
			case AUDIO_FORMAT_WMA_PRO:
			case AUDIO_FORMAT_MPEG:
			case AUDIO_FORMAT_AAC:
			case AUDIO_FORMAT_WMA:
			case AUDIO_FORMAT_RA:
			case AUDIO_FORMAT_OGG:
			case AUDIO_FORMAT_FLAC:
			case AUDIO_FORMAT_APE:
			case AUDIO_FORMAT_ALAC:
			case AUDIO_FORMAT_SBC:
	        default:
				pHintCnf = hint_data_map[eHintData_Stereo];
	            break;

        }
		ret = Hint_SingleDataAdd(pHintCnf, DEFAULT_INTEVAL_TIME);
		if(ret < 0)
		{
			hint_printf("hint data init error\n");
			return ret;
		}
    }

	return ret;
}
#endif

