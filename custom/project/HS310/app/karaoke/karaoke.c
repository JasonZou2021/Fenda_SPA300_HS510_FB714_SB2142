/******************************************************************************/
/**
*
* \file	 Karaoke.c
*
* \brief	This file provides the karaoke middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2017/08/21
******************************************************************************/
#include <string.h>
#include "custom_sig.h"
#include "karaoke.h"
#include "AudDspService.h"
#include "Usersetting.h"
#include "Display_Srv.h"
#include "karaoke_vfd_str_tbl.inc"
#define LOG_MODULE_ID  ID_KOK
#include "log_utils.h"

static const UINT16 bBgmVolTable[6] =
{
	0, 300, 340, 360, 380, 400,  //400: 0dB ,0.1dB/step
};

static const UINT32 VocalCancelLevelTable[4] =
{
	0x7FFFFF, 	// 0dB
	0x0CCCCC, 	// -20dB
	0x0147AE,	// -40dB
	0x0020C4,	// -60dB
};


Q_DEFINE_THIS_FILE


/*..........................................................................*/
void KaraokeBgmVolumeSet(int vol);
void KaraokeMic1VolumeSet(int vol);
void KaraokeMic2VolumeSet(int vol);
void KaraokeKeyShiftSet(int key);
void KaraokeEchoSet(int echo);
void KaraokeThresholdSet(int threshold);
void KaraokeVcLevelSet(int vc_level);
void KaraokeNrStateSet(KOK_NR_STATE enable);




/* hierarchical state machine ... */
QState Karaoke_initial(Karaoke_Effect *const me, QEvt const *const e);
QState Karaoke_home(Karaoke_Effect *const me, QEvt const *const e);
QState Karaoke_active(Karaoke_Effect *const me, QEvt const *const e);
QState Karaoke_inactive(Karaoke_Effect *const me, QEvt const *const e);


extern int UserAppNum2Str(char *strBuf, int num);
extern char *strcat(char *dest, const char *src);



/******************************************************************************************/
/**
 * \fn			void Karaoke_ctor(Karaoke_Effect * const me)
 *
 * \brief		initial the karaoke AO paramater
 *
 * \param		Karaoke_Effect * const me
 *
 * \return
 *
 ******************************************************************************************/
void Karaoke_ctor(Karaoke_Effect *const me)
{
	LOGD("Karaoke_ctor\n");

	UserSetting_T UserSetting;
	UserSetting_Init(&UserSetting);

	me->Kok_Bgm_Vol = UserSetting.KokBgm_Vol;
	me->Kok_Mic1_Vol = UserSetting.KokMic1_Vol;
	me->Kok_Mic2_Vol = UserSetting.KokMic2_Vol;
	me->Kok_KeyShift = UserSetting.Kok_KeyShift;
	me->Kok_Echo = UserSetting.Kok_Echo;
	me->Kok_Threshold = UserSetting.Kok_Threshold;
	me->Kok_Vc_Level = 2;
	me->Kok_Nr = KOK_NR_OFF;

	QHsm_ctor(&me->super, Q_STATE_CAST(&Karaoke_initial));
}

/******************************************************************************************/
/**
 * \fn			QState Karaoke_initial(Karaoke_Effect * const me, QEvt const * const e)
 *
 * \brief		initial the karaoke AO paramater
 *
 * \param		Karaoke_Effect * const me, QEvt const * const e
 *
 * \return		\e transform to Karaoke_home \n
 *
 *
 ******************************************************************************************/
QState Karaoke_initial(Karaoke_Effect *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */


	Karaoke_Usersetting_init(me);

	return Q_TRAN(&Karaoke_home);
}

/******************************************************************************************/
/**
 * \fn			QState Karaoke_home(Karaoke_Effect * const me, QEvt const * const e)
 *
 * \brief		karaoke home takes charge of on/off of karaoke
 *
 * \param		Karaoke_Effect * const me:(Input) the karaoke handler here,\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return	    \e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *
 ******************************************************************************************/
QState Karaoke_home(Karaoke_Effect *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("Karaoke_home ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("Karaoke_home EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("Karaoke_home INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case KOK_TURN_ON_SIG:
		{
			LOGD("KOK_ON trans to Karaoke_active; \n");
			status = Q_TRAN(&Karaoke_active);
			break;
		}
		case KOK_TURN_OFF_SIG:
		{
			LOGD("KOK_OFF trans to Karaoke_inactive; \n");
			status = Q_TRAN(&Karaoke_inactive);
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}

	return status;

}

/******************************************************************************************/
/**
 * \fn			int KaraokeDisplay(Karaoke_Effect * const me, int Str_ID, int val)
 *
 * \brief		       Api for user to show current karaoke setting by VFD
 *
 * \param		Karaoke_Effect * const me:(Input) the karaoke handler here,\n
 *              int Str_ID : source ID \n
 *
 * \return	    \e  0 on success. \n
 *
 *
 ******************************************************************************************/
int KaraokeDisplay(Karaoke_Effect *const me, int Str_ID, int val)
{
	char strBuf1[4] = {0};
	char strBuf[4] = {0};

	memcpy(strBuf, kok_vfd_str[Str_ID], strlen(kok_vfd_str[Str_ID]));
	if (Str_ID == STR_NR)
	{
		if (val == KOK_NR_ON)
		{
			memcpy(strBuf1, kok_vfd_str[STR_ON], strlen(kok_vfd_str[STR_ON]));
		}
		else
		{
			memcpy(strBuf1, kok_vfd_str[STR_OFF], strlen(kok_vfd_str[STR_OFF]));
		}
	}
	else
	{
		UserAppNum2Str(strBuf1, val);
	}

	if (strlen(strBuf1) < 2)
	{
		strcat(strBuf, " ");
	}

	strcat(strBuf, strBuf1);

	LOGDT("display pop str: %s, strlen(strBuf):%d\n ", strBuf, strlen(strBuf));

	VFD_Service_POP(me, strBuf, 2, RIGHT);

	return 0;
}

void Karaoke_Usersetting_init(Karaoke_Effect *const me)
{
	KaraokeBgmVolumeSet(me->Kok_Bgm_Vol);
	KaraokeMic1VolumeSet(me->Kok_Mic1_Vol);
	KaraokeMic2VolumeSet(me->Kok_Mic2_Vol);
	KaraokeKeyShiftSet(me->Kok_KeyShift);
	KaraokeEchoSet(me->Kok_Echo);
	KaraokeThresholdSet(me->Kok_Threshold);
	KaraokeVcLevelSet(VocalCancelLevelTable[me->Kok_Vc_Level]);
	KaraokeNrStateSet(me->Kok_Nr);
}

/******************************************************************************************/
/**
 * \fn		KaraokeBgmVolumeSet(int vol)
 *
 * \brief		used to set bgm volume
 *
 * \param	int vol : (input)vol
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeBgmVolumeSet(int vol)
{
(void)vol;
	//AudDspService_Set_VolumeTool(NULL, bBgmVolTable[vol]);
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic1VolumeSet(int vol)
 *
 * \brief		used to set mic1 volume
 *
 * \param	int vol : 40(max) , 0(min)	(+10 dB ~ -9.5 dB, mute /step: 0.5dB)
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic1VolumeSet(int vol)
{
	(void)vol;
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic2VolumeSet(int vol)
 *
 * \brief		used to set mic2 volume
 *
 * \param	int vol : 40(max) , 0(min)	(10 dB ~ -9.5 dB, mute /step: 0.5dB)
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic2VolumeSet(int vol)
{
	(void)vol;
}

/******************************************************************************************/
/**
 * \fn		KaraokeKeyShiftSet(int key)
 *
 * \brief		used to set bgm keyshift
 *
 * \param	int key : 15(max) , 1(min)
 *
 *                  1~7(key-down level) ->8(no key shift) ->9~15(key-up level)
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeKeyShiftSet(int key)
{
	(void)key;
}

/******************************************************************************************/
/**
 * \fn             KaraokeEchoSet(int echo)
 *
 * \brief		used to set bgm volume
 *
 * \param	int echo : 13(max) , 0(min)
 *
 *                  0~13 (light echo -> heavy echo, 0=echo off)
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeEchoSet(int echo)
{
	(void)echo;
}

/******************************************************************************************/
/**
 * \fn             KaraokeThresholdSet(int threshold)
 *
 * \brief		used to set the high/low threshold for mic input energy
 *
 * \param	int threshold_h/threshold_l : 0xffffff(max) , 0x000000(min)
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeThresholdSet(int threshold)
{
	(void)threshold;
}

/******************************************************************************************/
/**
 * \fn             KaraokeVcLevelSet(int vc_level)
 *
 * \brief		used to set karaoke voice cancel level
 *
 * \param	int vc_level : 3(max) , 0(min)
 *
 *                  0= voice cancel off
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeVcLevelSet(int vc_level)
{
	(void)vc_level;
}

/******************************************************************************************/
/**
 * \fn             KaraokeNrStateSet(int enable)
 *
 * \brief		used to set noise reduction control on/off
 *
 * \param	int enable: 0 ~ 1
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeNrStateSet(KOK_NR_STATE enable)
{
	(void)enable;
}

/******************************************************************************************/
/**
 * \fn		KaraokeBgmVolumeUp(Karaoke_Effect * const me)
 *
 * \brief		used to set bgm volume up
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeBgmVolumeUp(Karaoke_Effect *const me)
{

	if (me->Kok_Bgm_Vol < KOK_BGM_VOL_MAX)
	{
		me->Kok_Bgm_Vol ++;
	}
	else
	{
		LOGE("KaraokeBgmVolumeUp FAIL: %d\n", me->Kok_Bgm_Vol);
		return;
	}

	LOGDT("KaraokeBgmVolumeUp: %d\n", me->Kok_Bgm_Vol);

	KaraokeBgmVolumeSet(me->Kok_Bgm_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeBgmVolumeDn(Karaoke_Effect * const me)
 *
 * \brief		used to set bgm volume down
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeBgmVolumeDn(Karaoke_Effect *const me)
{

	if (me->Kok_Bgm_Vol > KOK_BGM_VOL_MIN)
	{
		me->Kok_Bgm_Vol --;
	}
	else
	{
		LOGE("KaraokeBgmVolumeDn FAIL: %d\n",  me->Kok_Bgm_Vol);
		return;
	}

	LOGDT("KaraokeBgmVolumeDn: %d\n", me->Kok_Bgm_Vol );

	KaraokeBgmVolumeSet(me->Kok_Bgm_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic1VolumeUp(Karaoke_Effect * const me)
 *
 * \brief		used to set mic1 volume up
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic1VolumeUp(Karaoke_Effect *const me)
{

	if (me->Kok_Mic1_Vol < KOK_MIC_VOL_MAX)
	{
		me->Kok_Mic1_Vol ++;
	}
	else
	{
		LOGE("KaraokeMic1VolumeUp FAIL: %d\n", me->Kok_Mic1_Vol);
		return;
	}

	LOGDT("KaraokeMic1VolumeUp: %d\n", me->Kok_Mic1_Vol );

	KaraokeMic1VolumeSet(me->Kok_Mic1_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic1VolumeDn(Karaoke_Effect * const me)
 *
 * \brief		used to set mic1 volume down
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic1VolumeDn(Karaoke_Effect *const me)
{

	if (me->Kok_Mic1_Vol > KOK_MIC_VOL_MIN)
	{
		me->Kok_Mic1_Vol --;
	}
	else
	{
		LOGE("KaraokeMic1VolumeDn FAIL: %d\n",  me->Kok_Mic1_Vol);
		return;
	}

	LOGDT("KaraokeMic1VolumeDn: %d\n", me->Kok_Mic1_Vol);

	KaraokeMic1VolumeSet(me->Kok_Mic1_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic2VolumeUp(Karaoke_Effect * const me)
 *
 * \brief		used to set mic2 volume up
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic2VolumeUp(Karaoke_Effect *const me)
{

	if (me->Kok_Mic2_Vol < KOK_MIC_VOL_MAX)
	{
		me->Kok_Mic2_Vol ++;
	}
	else
	{
		LOGE("KaraokeMic2VolumeUp FAIL: %d\n", me->Kok_Mic2_Vol);
		return;
	}

	LOGDT("KaraokeMic2VolumeUp: %d\n",  me->Kok_Mic2_Vol );

	KaraokeMic2VolumeSet(me->Kok_Mic2_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeMic2VolumeDn(Karaoke_Effect * const me)
 *
 * \brief		used to set mic2 volume down
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeMic2VolumeDn(Karaoke_Effect *const me)
{
	if (me->Kok_Mic2_Vol > KOK_MIC_VOL_MIN)
	{
		me->Kok_Mic2_Vol --;
	}
	else
	{
		LOGE("KaraokeMic2VolumeDn FAIL: %d\n",  me->Kok_Mic2_Vol);
		return;
	}

	LOGDT("KaraokeMic2VolumeDn: %d\n",  me->Kok_Mic2_Vol);

	KaraokeMic2VolumeSet(me->Kok_Mic2_Vol);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeKeyShiftUp(Karaoke_Effect * const me)
 *
 * \brief		used to set bgm keyshift up
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeKeyShiftUp(Karaoke_Effect *const me)
{
	if (me->Kok_KeyShift < KOK_KEY_SHIFT_MAX)
	{
		me->Kok_KeyShift ++;
	}
	else
	{
		LOGE("KaraokeKeyShiftUp FAIL: %d\n", me->Kok_KeyShift);
		return;
	}

	LOGDT("KaraokeKeyShiftUp: %d\n", me->Kok_KeyShift );

	KaraokeKeyShiftSet(me->Kok_KeyShift);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeKeyShiftDn(Karaoke_Effect * const me)
 *
 * \brief		used to set bgm keyshift down
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeKeyShiftDn(Karaoke_Effect *const me)
{
	if (me->Kok_KeyShift > KOK_KEY_SHIFT_MIN)
	{
		me->Kok_KeyShift --;
	}
	else
	{
		LOGE("KaraokeKeyShiftDn FAIL: %d\n",  me->Kok_KeyShift);
		return;
	}

	LOGDT("KaraokeKeyShiftDn: %d\n", me->Kok_KeyShift);

	KaraokeKeyShiftSet(me->Kok_KeyShift);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeEchoUp(Karaoke_Effect * const me)
 *
 * \brief		used to set echo heavy
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeEchoUp(Karaoke_Effect *const me)
{
	if (me->Kok_Echo < KOK_ECHO_MAX)
	{
		me->Kok_Echo ++;
	}
	else
	{
		LOGE("KaraokeEchoUp FAIL: %d\n", me->Kok_Echo );
		return;
	}

	LOGDT("KaraokeEchoUp: %d\n", me->Kok_Echo);

	KaraokeEchoSet(me->Kok_Echo);
	return;
}

/******************************************************************************************/
/**
 * \fn		KaraokeEchoUp(Karaoke_Effect * const me)
 *
 * \brief		used to set echo light
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeEchoDn(Karaoke_Effect *const me)
{
	if (me->Kok_Echo > KOK_ECHO_MIN)
	{
		me->Kok_Echo --;
	}
	else
	{
		LOGE("KaraokeEchoDn FAIL: %d\n",  me->Kok_Echo );
		return;
	}

	LOGDT("KaraokeEchoDn: %d\n", me->Kok_Echo);

	KaraokeEchoSet(me->Kok_Echo);
	return;
}

/******************************************************************************************/
/**
 * \fn             KaraokeVoiceCancel(Karaoke_Effect * const me)
 *
 * \brief		used to set karaoke voice cancel level
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeVoiceCancel(Karaoke_Effect *const me)
{
	if (me->Kok_Vc_Level < KOK_VC_LVL_MAX)
	{
		me->Kok_Vc_Level ++;
	}
	else
	{
		me->Kok_Vc_Level = KOK_VC_LVL_MIN;
	}


	LOGDT("KaraokeVoiceCancel: %d\n", me->Kok_Vc_Level);

	KaraokeVcLevelSet(VocalCancelLevelTable[me->Kok_Vc_Level]);

}

/******************************************************************************************/
/**
 * \fn             KaraokeNoiseReduction(Karaoke_Effect * const me)
 *
 * \brief		used to set karaoke noise reduction control
 *
 * \param	Karaoke_Effect * const me : (Input) the karaoke handler here,\n
 *
 * \return
 *
 *
 ******************************************************************************************/
void KaraokeNoiseReduction(Karaoke_Effect *const me)
{
	if (me->Kok_Nr == KOK_NR_OFF)
	{
		me->Kok_Nr = KOK_NR_ON;
	}
	else
	{
		me->Kok_Nr = KOK_NR_OFF;
	}

	LOGDT("KaraokeNoiseReduction: %d\n", me->Kok_Nr);

	KaraokeNrStateSet(me->Kok_Nr);

}

/******************************************************************************************/
/**
 * \fn		QState Karaoke_active(Karaoke_Effect * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param       Karaoke_Effect * const me :(Input) the karaoke handler here,\n
 *                  QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in karaoke active state \n
 *			\e status : Q_TRAN() trans to other sub_state \n
 *                  \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

QState Karaoke_active(Karaoke_Effect *const me, QEvt const *const e)
{
	QState status;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("Karaoke_active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("Karaoke_active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("Karaoke_active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case KOK_BGM_VOL_UP_TICK_SIG:
		case KOK_BGM_VOL_UP_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_BGM_VOL_UP\n");

			KaraokeBgmVolumeUp(me);
			UserSetting_SaveKokBgmVolume(me->Kok_Bgm_Vol);

			KaraokeDisplay(me, STR_BGMV, me->Kok_Bgm_Vol);
			status = Q_HANDLED();
			break;
		}
		case KOK_BGM_VOL_DN_TICK_SIG:
		case KOK_BGM_VOL_DN_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_BGM_VOL_DN\n");

			KaraokeBgmVolumeDn(me);
			UserSetting_SaveKokBgmVolume(me->Kok_Bgm_Vol);

			KaraokeDisplay(me, STR_BGMV, me->Kok_Bgm_Vol);
			status = Q_HANDLED();
			break;
		}
		case KOK_MIC1_VOL_UP_TICK_SIG:
		case KOK_MIC1_VOL_UP_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_MIC1_VOL_UP\n");

			KaraokeMic1VolumeUp(me);
			UserSetting_SaveKokMic1Volume(me->Kok_Mic1_Vol);

			KaraokeDisplay(me, STR_MIC1V, me->Kok_Mic1_Vol);
			status = Q_HANDLED();
			break;
		}
		case KOK_MIC1_VOL_DN_TICK_SIG:
		case KOK_MIC1_VOL_DN_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_MIC1_VOL_DN\n");

			KaraokeMic1VolumeDn(me);
			UserSetting_SaveKokMic1Volume(me->Kok_Mic1_Vol);

			KaraokeDisplay(me, STR_MIC1V, me->Kok_Mic1_Vol);
			status = Q_HANDLED();
			break;
		}
		case KOK_MIC2_VOL_UP_TICK_SIG:
		case KOK_MIC2_VOL_UP_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_MIC2_VOL_UP\n");

			KaraokeMic2VolumeUp(me);
			UserSetting_SaveKokMic2Volume(me->Kok_Mic2_Vol);

			KaraokeDisplay(me, STR_MIC2V, me->Kok_Mic2_Vol);
			status = Q_HANDLED();
			break;
		}
		case KOK_MIC2_VOL_DN_TICK_SIG:
		case KOK_MIC2_VOL_DN_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_MIC2_VOL_DN\n");

			KaraokeMic2VolumeDn(me);
			UserSetting_SaveKokMic2Volume(me->Kok_Mic2_Vol);

			KaraokeDisplay(me, STR_MIC2V, me->Kok_Mic2_Vol);
			status = Q_HANDLED();
			break;
		}
		case SEVEN_TICK_SIG:      //KOK_ECHO_UP_TICK_SIG:
		case SEVEN_RELEASE_SIG:  //KOK_ECHO_UP_RELEASE_SIG: {
		{
			LOGDT("Karaoke_active: KOK_ECHO_UP\n");

			KaraokeEchoUp(me);
			UserSetting_SaveKokEcho(me->Kok_Echo);

			KaraokeDisplay(me, STR_ECHO, me->Kok_Echo);
			status = Q_HANDLED();
			break;
		}
		case KOK_ECHO_DN_TICK_SIG:
		case KOK_ECHO_DN_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_ECHO_DN\n");

			KaraokeEchoDn(me);
			UserSetting_SaveKokEcho(me->Kok_Echo);

			KaraokeDisplay(me, STR_ECHO, me->Kok_Echo);
			status = Q_HANDLED();
			break;
		}
		case NINE_TICK_SIG:       //KOK_KEY_SHIFT_UP_TICK_SIG:
		case NINE_RELEASE_SIG:    //KOK_KEY_SHIFT_UP_RELEASE_SIG:{
		{
			LOGDT("Karaoke_active: KOK_KEY_SHIFT_UP\n");

			KaraokeKeyShiftUp(me);
			UserSetting_SaveKokKey(me->Kok_KeyShift);

			KaraokeDisplay(me, STR_KEY, me->Kok_KeyShift);
			status = Q_HANDLED();
			break;
		}
		case KOK_KEY_SHIFT_DN_TICK_SIG:
		case KOK_KEY_SHIFT_DN_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_KEY_SHIFT_DN\n");

			KaraokeKeyShiftDn(me);
			UserSetting_SaveKokKey(me->Kok_KeyShift);

			KaraokeDisplay(me, STR_KEY, me->Kok_KeyShift);
			status = Q_HANDLED();
			break;
		}
		case KOK_VC_LVL_RELEASE_SIG:
		case KOK_VC_LVL_TICK_SIG:
		{
			LOGDT("Karaoke_active: KOK_VC_LVL\n");

			KaraokeVoiceCancel(me);

			KaraokeDisplay(me, STR_VC, me->Kok_Vc_Level);
			status = Q_HANDLED();
			break;
		}
		case KOK_NR_RELEASE_SIG:
		{
			LOGDT("Karaoke_active: KOK_NR\n");

			KaraokeNoiseReduction(me);

			KaraokeDisplay(me, STR_NR, me->Kok_Nr);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			LOGD("Karaoke_active back to home\n");
			status = Q_SUPER(&Karaoke_home);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn	       QState Karaoke_inactive(Karaoke_Effect * const me, QEvt const * const e)
 *
 * \brief	       the karaoke inactive state
 *
 * \param       Karaoke_Effect * const me(Input): the karaoke handler here\n
 *                  QEvt const * const e :event structure of signal\n
 *
 * \return	       \e status : Q_HANDLED() in karaoke inactive state \n
 *	              \e status : Q_SUPER() trans to father state \n
 *
 ******************************************************************************************/

QState Karaoke_inactive(Karaoke_Effect *const me, QEvt const *const e)
{

	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("Karaoke_inactive ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("Karaoke_inactive EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("Karaoke_inactive INIT\n");
			status = Q_HANDLED();
			break;
		}
		default :
		{
			LOGD("Karaoke_inactive back to home \n");
			status = Q_SUPER(&Karaoke_home);
			break;
		}
	}
	return status;

}


