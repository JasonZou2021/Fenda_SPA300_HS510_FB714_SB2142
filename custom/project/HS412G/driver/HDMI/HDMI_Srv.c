#include <string.h>
#include <stdlib.h>

#include "task_attribute_user.h"
#include "powerservice.h"
#include "systemservice.h"
#include "qf_sync.h"
#include "qf_response.h"

//#include "qpc.h"
#include "initcall.h"

#include "HDMI_Srv.h"
#include "malloc.h"
//#include "stc.h"
#include "cec_pub.h"
#include "hdmi_pub.h"
#include "log_utils.h"
#include "userapp.h"
#include "datarammap.h"


#define CEC_POLLING_FREQ (TICKS_PER_SEC/50) // 20ms
#define CEC_KEY_POLLING_FREQ (TICKS_PER_SEC/10) // 20ms
#define TERMINATE_TIMEOUT (15) //15*100ms
#define RESEND_SIG_CNT 5 // if key buf full, we should resend key sig to userapp

/*..........................................................................*/
typedef struct       /* the IopScanner active object */
{
	QActive super;   /* inherit QActive */
	QTimeEvt timeEvt; /* private time event generator */
	stKeyInfo_t *stCECKeyData;
	hCircBuf_t *stcecKeybuf;
	CFG_KEY_T *pstCECKEY_cfg;
	QTimeEvt timeEvt_cec; /* private time event generator */
	stCecHandler *pstCEC_handler;
	UINT8 ucICBodingFlag;
	HDMI_OP_T *pHdmi_op_fn;
	//int iTerminatePorcessEnable;
	int iArcMuteState;
	int iPollingCnt;
	CFG_CEC_T *pstCEC_cfg;
	CFG_HDMI_T *pstHDMI_cfg;
	int HDMI_Tx;
	int Last_CEC_Keycode;
	int Next_CEC_Keycode;
	int CEC_Keystate;
	UINT8 TV_Remote_Ready_state;
} HDMISrv;

static HDMISrv l_HDMISrv; /* the IopScanner active object */

QActive *const AO_HDMISrv = &l_HDMISrv.super;
//stKeyInfo_t stCECKeyData;

Q_DEFINE_THIS_FILE;

// Hdmi op functions


void HDMISrv_Start(void);

void HDMISrv_ctor(void);
QState HDMISrv_initial(HDMISrv *const me, QEvt const *const e);
int HDMISrv_HPDBroadcast(void const *pSender, UINT8 bState);
void HDMISrv_HDMIKeyPolling(HDMISrv *const me);

QState HDMISrv_Home(HDMISrv *const me, QEvt const *const e);
int HDMISrv_RegCB(void);

static int HDMISrv_CEC_Standby(QActive *const me);
static int HDMISrv_CEC_ARCSource (QActive *const me,UINT8 ARC_SRC);
static int HDMISrv_CEC_SetDSP(QActive *const me,UINT8 DSPState);


int HDMISrv_HDMI_PlugIn(void const *pSender);
int HDMISrv_HDMI_PlugOut(void const *pSender);
int HDMISrv_CECEvt_CbFunc(BYTE opcode, BYTE *buf, int len);
int HDMISrv_CecEvtBroadcast(void const *pSender, stCecCmdInfo_t *pstCecInfo);
void HDMISrv_Terminate(QActive *const me, void *pParam);

int HDMISrv_HDMI_disable(HDMI_OP_T *pHdmi_op_fn);
int HDMISrv_HDMI_480p(HDMI_OP_T *pHdmi_op_fn);
int HDMISrv_DVI_480p(HDMI_OP_T *pHdmi_op_fn);

int HDMISrv_HDMI_576p(HDMI_OP_T *pHdmi_op_fn);
int HDMISrv_DVI_576p(HDMI_OP_T *pHdmi_op_fn);
void HDMISrv_AudioInfo_Listener(QActive *const me, void *pParam);
int HDMISrv_HDMI_mode(HDMI_OP_T *pHdmi_op_fn, UINT8 *pEdidData);
int HDMISrv_TV_resolution(HDMI_OP_T *pHdmi_op_fn, UINT8 *pEdidData);

/******************************************************************************************/
/**
 * \fn			QActive *HDMISrv_get(void)
 *
 * \brief		get receiver handler
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
QActive *HDMISrv_get(void)
{
	return AO_HDMISrv;
}

/******************************************************************************************/
/**
 * \fn			void HDMISrv_Start(void)
 *
 * \brief		initial the HDMISrv AO paramater
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
void HDMISrv_Start(void)
{
	static QEvt const *pstHDMIQueueSto[15];

	HDMISrv_ctor();

	QACTIVE_START(AO_HDMISrv,           /*!< AO to start */
				  TSK_HDMISRV_PRIO, 			/*!< QP priority of the AO */
				  pstHDMIQueueSto,      /*!< event queue storage */
				  Q_DIM(pstHDMIQueueSto), /*!< queue length [events] */
				  TSK_HDMISRV_NAME,             /*!< AO name */
				  TSK_HDMISRV_STACK_DEPTH,       /*!< size of the stack [bytes] */
				  (QEvt *)0);             /*!< initialization event */
}


#ifndef BUILT4ISP2
AO_INIT(HDMISrv_Start);
#endif
void HDMISrv_ctor(void)
{
	HDMISrv *const me = &l_HDMISrv;
	QActive_ctor(&me->super, Q_STATE_CAST(&HDMISrv_initial));

	//me->ucICBodingFlag = IC_Type_Classify(GetICBonding());
	//LOGD("IC bounding Group check: %d\n", me->ucICBodingFlag);
	//me->iTerminatePorcessEnable = 0


	//Configure CEC
	me->pstCEC_cfg = GET_SDK_CFG(CFG_CEC_T);

	me->pstCEC_handler = CEC_GetCecHandler();
	me->pstCEC_handler->CecKeyCode = -1;

	LOGD("initial pstCEC_cfg: %d\n", me->pstCEC_cfg->Cfg_Enable);
	if ((me->pstCEC_cfg->Cfg_Enable == 1))
	{
		me->pstCECKEY_cfg = GET_SDK_CFG(CFG_KEY_T);
		me->stCECKeyData= &stKeyData;
		me->stCECKeyData->key = -1;
		me->stCECKeyData->key_pre = -1;
		me->stCECKeyData->key_current = -1;
		me->stcecKeybuf = &circBuf;
		//me->stcecKeybuf->maxLen = 10;
		QTimeEvt_ctorX(&me->timeEvt, &me->super, HDMI_KEYPOLLING_SIG, 0U);

	    LOGD("me->ucICBodingFlag: %d\n", me->ucICBodingFlag);
		me->pstCEC_handler->BusBusyChk.Cec_PollingTime = CEC_POLLING_FREQ;
		me->pstCEC_handler->HwRdy = 0;
		me->pstCEC_handler->CecEnable = 0;
		me->pstCEC_handler->SysSrc = eSysSrc_Others;
		me->pstCEC_handler->CECStandby = 0;
		me->pstCEC_handler->ACKretry  = 0;
		me->Last_CEC_Keycode = -1;
		me->Next_CEC_Keycode = -1;
		me->CEC_Keystate = CEC_Status_UNREADY;
		me->iArcMuteState = 0;
		me->pHdmi_op_fn = NULL;
		CEC_HwInit(me->pstCEC_handler);
		me->pstCEC_handler->pCECEvt_CbFunc = HDMISrv_CECEvt_CbFunc; // cec event callback function
		QTimeEvt_ctorX(&me->timeEvt_cec, &me->super, HDMI_PRI_CEC_SCAN_SIG, 0U);
		HDMISrv_RegCB();
	}

	me->pstHDMI_cfg = GET_SDK_CFG(CFG_HDMI_T);

	//LOGD("initial g_pstHDMI: 0x%x\n", g_pstHDMI);
	if (me->pHdmi_op_fn != NULL)
	{
		HDMISrv_HDMI_disable(me->pHdmi_op_fn);
	}
	else
	{
		LOGE("g_pstHDMI is NULL\n");
	}
}

QState HDMISrv_initial(HDMISrv *const me, QEvt const *const e)
{
	(void)e;

	if ((me->pstCEC_cfg->Cfg_Enable == 1))
	{
	    QTimeEvt_armX(&me->timeEvt_cec, CEC_POLLING_FREQ, 0);
		PowerService_RegPowerOffLSR(&me->super, HDMISrv_Terminate);
		QTimeEvt_armX(&me->timeEvt, CEC_KEY_POLLING_FREQ, 0);
		//QActive_subscribe(&me->super, IOP_AUDIO_INFO_REPORT_SIG);
	}

	return Q_TRAN(&HDMISrv_Home);

}


QState HDMISrv_Home(HDMISrv *const me, QEvt const *const e)
{
	QState status;
		switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGDT("HDMISrv_Home ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGDT("HDMISrv_Home EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGDT("HDMISrv_Home INIT; \n");
			SystemService_RegAudioInfoListener(&me->super, HDMISrv_AudioInfo_Listener);
			status = Q_HANDLED();
			break;

		}
		case IOP_AUDIO_INFO_REPORT_SIG:
		{
			stSYS_AudioInfo_t *pstAudInfoEvt = (stSYS_AudioInfo_t *)e;
			LOGD("IOP_AUDIO_INFO_REPORT_SIG bVol:%d, mute:%d\n", pstAudInfoEvt->Vol, pstAudInfoEvt->MuteState);
			me->pstCEC_handler->SysAudCtrl.Volume =  CEC_VolConvert(pstAudInfoEvt->Vol, pstAudInfoEvt->MaxVol);;
			me->pstCEC_handler->SysAudCtrl.MuteState = pstAudInfoEvt->MuteState;
			LOGD("IOP_AUDIO_INFO_REPORT_SIG:SysAudCtrl.MuteState: %d\n", me->pstCEC_handler->SysAudCtrl.MuteState);

			if (me->pstCEC_handler->CecEnable == 1)
				CEC_AudioStatusReport(me->pstCEC_handler);

			status = Q_HANDLED();
			break;
		}
		case HDMI_PRI_CEC_SCAN_SIG:
		{
			int iHPD_res = -1;
			QTimeEvt_armX(&me->timeEvt_cec, CEC_POLLING_FREQ, 0);
			iHPD_res = HDMI_PollingHpd(me->pstCEC_handler);

			if ((iHPD_res == 0) || (iHPD_res == 1))
			{
				HDMISrv_HPDBroadcast(me, iHPD_res);
				LOGD("HDMI HPD plug!!!:%d\n", iHPD_res);
				if (iHPD_res == 1)
					HDMISrv_HDMI_PlugIn(me);
				else
					HDMISrv_HDMI_PlugOut(me);
			}

#ifdef CEC_POLLING_TIME_CHECK
			UINT32 time_tick, time_tick2;
			static UINT32 time_tick_old = 0;

			EnterCritical();
			time_tick = GetSTC();
#endif
			if (CEC_TerminatedCheck(me->pstCEC_handler) != 1)// && (me->pstCEC_handler->HpdStatus == 1))
			{
				HDMI_PollingCec(me->pstCEC_handler);// 1 ~ 2 tick, max:5 ticks when the cec cmd is read (no cec debug message)
			}

			if(CEC_StandbyChk(me->pstCEC_handler)== 1)
			{
				HDMISrv_CEC_Standby(&me->super);
			}
			if(CEC_ARCSourceChk(me->pstCEC_handler) == 1)
			{
				HDMISrv_CEC_ARCSource (&me->super,me->pstCEC_handler->CEC_SourceState);

			}
			if(CEC_SetDSPChk(me->pstCEC_handler) == 1)
			{
				HDMISrv_CEC_SetDSP(&me->super,me->pstCEC_handler->CEC_SET_DSP_ON);

			}
			if(me->pstCEC_handler->CecKeyCode !=-1)
			{
				me->CEC_Keystate = CEC_Status_READY;
			}

			IOSrv_CEC_KEY_State(me->CEC_Keystate);
#ifdef CEC_CTS_TEST
			if (me->pstCEC_handler->SysAudCtrl.MuteState != me->iArcMuteState)
			{
				stSYS_MuteReq_t *mq;
				LOGD("IOP_PRI_CEC_SCAN_SIG:SysAudCtrl.MuteState: %d\n", me->pstCEC_handler->SysAudCtrl.MuteState);
				me->iArcMuteState = me->pstCEC_handler->SysAudCtrl.MuteState;
				mq = Q_NEW(stSYS_MuteReq_t, IOP_MUTE_REQ_SIG);
				mq->MuteState = me->iArcMuteState;
				QF_PUBLISH(&mq->stEvt, me);
			}
#endif

#ifdef CEC_POLLING_TIME_CHECK
			time_tick2 = GetSTC();
			ExitCritical();

			time_tick2 -= time_tick;
			if (time_tick_old < time_tick2)
			{
				LOGD("CEC polling time_tick:%d\n", time_tick2);
				time_tick_old = time_tick2;
			}
#endif
			status = Q_HANDLED();
			break;
		}

		case HDMI_PRI_CEC_INIT_SIG:
		{
			LOGD("IOP_PRI_CEC_INIT_SIG :%d \n",me->pstCEC_handler->SysSrc);
			CEC_NormalInitiate(me->pstCEC_handler);
			LOGD("CEC Normal Initiate\n");
			status = Q_HANDLED();
			break;
		}

		case HDMI_PRI_CEC_TERMINATE_SIG:
		{
			CEC_NormalTerminate(me->pstCEC_handler);
			LOGD("CEC terminate\n");
			status = Q_HANDLED();
			break;
		}

		case HDMI_PRI_CEC_SET_SAC_SIG:
		{
			stCecSetSac_t *pstCecSetSac = ((stCecSetSac_t *)e) ;
			CEC_SetSac(me->pstCEC_handler, pstCecSetSac->bEnble);
			LOGD("IOP_PRI_CEC_SET_SAC_SIG: %d\n", pstCecSetSac->bEnble);
			status = Q_HANDLED();
			break;
		}

		case HDMI_PRI_CEC_SET_SRC_SIG:
		{
			LOGD("IOP_PRI_CEC_SET_SRC_SIG :%d \n",me->pstCEC_handler->SysSrc);
			stCecSetSrc_t *pstCecSetSrc = ((stCecSetSrc_t *)e) ;
			CEC_SetSysSrc(me->pstCEC_handler, pstCecSetSrc->bCecSrc);
			LOGD("IOP_PRI_CEC_SET_SRC_SIG :%d \n",me->pstCEC_handler->SysSrc);
			LOGD("IOP_PRI_CEC_SET_SRC_SIG: %d\n", pstCecSetSrc->bCecSrc);
			status = Q_HANDLED();
			break;
		}

		case HDMI_CEC_HPD_BROADCAST_REQ_SIG:
		{
			LOGD("HDMI_CEC_HPD_BROADCAST_REQ_SIG\n");
			HDMISrv_HPDBroadcast(&me->super, me->pstCEC_handler->HpdStatus);
			status = Q_HANDLED();
			break;
		}
		// this four cases are for HDMI TX {
		case HDMI_PRI_HDMI_TX_INIT_SIG:
		{
			LOGD("HDMI_TX_INIT\n");
			UINT8 *pEdidData = NULL;
			me->HDMI_Tx = 1;
			if (HDMI_get_HPD(me->pstCEC_handler) == 1)
			{
				int HdmiMode = 0;
				int TvResolution = 0;
				//hdmitx_power_on();
				me->pHdmi_op_fn = HDMITX_Power_ON();

				pEdidData = HDMI_get_EDID(me->pstCEC_handler);

				if (pEdidData == NULL)
				{
					LOGE("EDID err\n");
				}
				else
				{
					HdmiMode = HDMISrv_HDMI_mode(me->pHdmi_op_fn, pEdidData);
					TvResolution = HDMISrv_TV_resolution(me->pHdmi_op_fn, pEdidData);
					LOGD("HdmiMode = %d\n", HdmiMode);
					LOGD("TvResolution = %d\n", TvResolution);
					if ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_480P)
							|| ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_AUTO) && (TvResolution != eHDMI_TV_RES_576p)))
					{
						if (HdmiMode == eHDMI_HDMI_mode)
						{
							HDMISrv_HDMI_480p(me->pHdmi_op_fn);
							LOGD("IOSrv_HDMI_480p\n");
						}
						else if (HdmiMode == eHDMI_DVI_mode)
						{
							HDMISrv_DVI_480p(me->pHdmi_op_fn);
							LOGD("IOSrv_DVI_480p\n");
						}
						else
						{
							LOGE("Hdmi Mode err\n");
						}
					}
					else if ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_576P)
							 || ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_AUTO) && (TvResolution == eHDMI_TV_RES_576p)))
					{
						if (HdmiMode == eHDMI_HDMI_mode)
						{
							HDMISrv_HDMI_576p(me->pHdmi_op_fn);
							LOGD("IOSrv_HDMI_576p\n");
						}
						else if (HdmiMode == eHDMI_DVI_mode)
						{
							HDMISrv_DVI_576p(me->pHdmi_op_fn);
							LOGD("IOSrv_DVI_576p\n");
						}
						else
						{
							LOGE("Hdmi Mode err\n");
						}
					}
					//HDMI_embedded_tone();
					HDMI_embedded_API();
				}
			}

			status = Q_HANDLED();
			break;
		}
		case HDMI_PRI_HDMI_TX_TERMINATE_SIG:
		{
			LOGD("HDMI_TX_TERMINATE\n");
			//call hdmi tx terminate API here
			me->HDMI_Tx = 0;
			if(me->pHdmi_op_fn != NULL)
			{
				HDMISrv_HDMI_disable(me->pHdmi_op_fn);
				me->pHdmi_op_fn = HDMITX_Power_OFF();
			}
			//CEC_SYN_LoglAddr(me->pstCEC_handler);

			status = Q_HANDLED();
			break;
		}
		case HDMI_PRI_HDMI_PLUG_IN_SIG:
		{
			LOGD("HDMI_PLUG_IN\n");
			UINT8 *pEdidData = NULL;

			if (me->HDMI_Tx) // in hdmi tx source
			{
				pEdidData = HDMI_get_EDID(me->pstCEC_handler);
				if (HDMI_get_HPD(me->pstCEC_handler) == 1)
				{
					int HdmiMode = 0;
					int TvResolution = 0;
					me->pHdmi_op_fn = HDMITX_Power_ON();


					pEdidData = HDMI_get_EDID(me->pstCEC_handler);

					if (pEdidData == NULL)
					{
						LOGE("EDID err\n");
					}
					else
					{
						HdmiMode = HDMISrv_HDMI_mode(me->pHdmi_op_fn, pEdidData);
						TvResolution = HDMISrv_TV_resolution(me->pHdmi_op_fn, pEdidData);
						LOGD("HdmiMode = %d\n", HdmiMode);
						LOGD("TvResolution = %d\n", TvResolution);
						if ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_480P)
								|| ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_AUTO) && (TvResolution != eHDMI_TV_RES_576p)))
						{
							if (HdmiMode == eHDMI_HDMI_mode)
							{
								HDMISrv_HDMI_480p(me->pHdmi_op_fn);
								LOGD("IOSrv_HDMI_480p\n");
							}
							else if (HdmiMode == eHDMI_DVI_mode)
							{
								HDMISrv_DVI_480p(me->pHdmi_op_fn);
								LOGD("IOSrv_DVI_480p\n");
							}
							else
							{
								LOGE("Hdmi Mode err\n");
							}
						}
						else if ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_576P)
								 || ((me->pstHDMI_cfg->Cfg_HdmiOutputMode == eHDMI_AUTO) && (TvResolution == eHDMI_TV_RES_576p)))
						{
							if (HdmiMode == eHDMI_HDMI_mode)
							{
								HDMISrv_HDMI_576p(me->pHdmi_op_fn);
								LOGD("IOSrv_HDMI_576p\n");
							}
							else if (HdmiMode == eHDMI_DVI_mode)
							{
								HDMISrv_DVI_576p(me->pHdmi_op_fn);
								LOGD("IOSrv_DVI_576p\n");
							}
							else
							{
								LOGE("Hdmi Mode err\n");
							}
						}
					 HDMI_embedded_API();
					}
				}
			}

			status = Q_HANDLED();
			break;
		}
		case HDMI_PRI_HDMI_PLUG_OUT_SIG:
		{
			LOGD("HDMI_PLUG_OUT\n");
			// disable HDMI tx when HPD plug out
			if (me->HDMI_Tx == 1)
				HDMISrv_HDMI_disable(me->pHdmi_op_fn);

			status = Q_HANDLED();
			break;
		}
		case HDMI_PRI_CEC_EVENT_SIG:
		{
			stCECEvt_t *pstCEC_Evt = ((stCECEvt_t *)e) ;

			LOGD("cec opcode: 0x%x, size: %d\n", pstCEC_Evt->stCecInfo.opcode, pstCEC_Evt->stCecInfo.size);

			HDMISrv_CecEvtBroadcast(&me->super, &pstCEC_Evt->stCecInfo);

			status = Q_HANDLED();
			break;
		}
		case HDMI_KEYPOLLING_SIG:
		{
			//int iRes = 0;
			//me->iPollingCnt++;

			QTimeEvt_armX(&me->timeEvt, CEC_KEY_POLLING_FREQ, 0);
			HDMISrv_HDMIKeyPolling(me);
			status = Q_HANDLED();
			break;

		}
			// this four cases are for HDMI TX }
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}

		return status;

}
void HDMISrv_HDMIKeyPolling(HDMISrv *const me)
{
	int result = 0;
	int iCEC_Key = -1;
	int iKeyCode = -1;
	int iKeySrc = -1;
	// 1.get keys from all source
	//CEC key code cmd check
	if (me->pstCEC_cfg->Cfg_Enable == 1 && me->CEC_Keystate == CEC_Status_READY)
	{
		iCEC_Key = me->pstCEC_handler->CecKeyCode;

		iKeyCode = iCEC_Key;
		iKeySrc = eKeySrc_CEC_CMD;

		if((iKeyCode == -1)&&(me->Next_CEC_Keycode == -1))
		{
			me->CEC_Keystate = CEC_Status_UNREADY;

		}
		if(iKeyCode != -1)
		{
			if(me->Last_CEC_Keycode == -1){
				me->Last_CEC_Keycode = iKeyCode;
			}
			if(me->Last_CEC_Keycode != iKeyCode)
			{
				me->Next_CEC_Keycode = iCEC_Key;
			}
			else if(me->Next_CEC_Keycode != -1)
			{
				me->Last_CEC_Keycode = iCEC_Key;
			}
		}
		else
		{
			if(me->Next_CEC_Keycode != -1)
			{
				me->stCECKeyData->key_cnt = 1;
				me->stCECKeyData->key_pre = me->Last_CEC_Keycode;
			}
		}

		if((me->Last_CEC_Keycode == -1)&&(me->Next_CEC_Keycode != -1))
		{
			iKeyCode = me->Next_CEC_Keycode;
			iKeySrc = eKeySrc_CEC_CMD;
			me->stCECKeyData->key_cnt = 0;
			me->Next_CEC_Keycode = -1;
		}
		// 3. key type process
		//if(me->Last_CEC_Keycode == me->Next_CEC_Keycode)
		result = KeyProcess(iKeyCode, iKeySrc, me->stCECKeyData,  me->pstCECKEY_cfg);

		// 4. push key info to buf
		if (result == 1)
		result = PushKeyInfo(me->stcecKeybuf, me->stCECKeyData);
		if(me->stCECKeyData->key_type == eKeyRelease)
		{
			me->Last_CEC_Keycode = -1;
		}
	}

}

void HDMISrv_AudioInfo_Listener(QActive *const me, void *pParam)
{
	(void)me;

	stSYS_AudioInfo_t *pstAudioInfo = (stSYS_AudioInfo_t * )pParam;



	stSYS_AudioInfo_t *e = Q_NEW(stSYS_AudioInfo_t, IOP_AUDIO_INFO_REPORT_SIG);

	//memcpy(e, pstAudioInfo, sizeof(stSYS_AudioInfo_t)-);

	e->Vol = pstAudioInfo->Vol;
	e->MuteState = pstAudioInfo->MuteState;
	e->MaxVol = pstAudioInfo->MaxVol;
	e->MinVol = pstAudioInfo->MinVol;

	QACTIVE_POST(HDMISrv_get(), &e->stEvt, HDMISrv_get());

	LOGD("IOSrv_AudioInfo_Listener callback: %d, %d\n", e->MuteState, e->Vol);
}


static int HDMISrv_CEC_Standby(QActive *const me)

{
	QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, HDMI_CEC_Standby_SIG), (void *)me);
	LOGD("HDMISrv_CEC_Standby\n");

	return 0;
}
static int HDMISrv_CEC_ARCSource (QActive *const me,UINT8 ARC_SRC)
{
	stCecSetSrc_t *pstCecSetSrc = Q_NEW(stCecSetSrc_t, HDMI_CEC_ARCSOURCE_SIG);
	pstCecSetSrc->bCecSrc = ARC_SRC;
	QACTIVE_POST(UserApp_get(),&pstCecSetSrc->stEvt, (void *)me);
	LOGD("HDMISrv_CEC_ARCSource\n");

	return 0;
}

static int HDMISrv_CEC_SetDSP(QActive *const me,UINT8 DSPState)
{
	stCecSetDSP_t *pstCecSetSrc = Q_NEW(stCecSetDSP_t, HDMI_CEC_SETDSP_SIG);
	pstCecSetSrc->DSP_state = DSPState;
	QACTIVE_POST(UserApp_get(),&pstCecSetSrc->stEvt, (void *)me);
	LOGD("HDMISrv_CEC_SetDSP\n");

	return 0;
}

/******************************************************************************************/
/**
 * \fn			int HDMISrv_Check_ARCWakeUp(void))
 *
 * \brief		get power on state
 *
 * \param
 *
 * \return		 int power on state
 *
 *
 ******************************************************************************************/

int HDMISrv_Check_ARCWakeUp(void)
{
    volatile int *HDMI_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	int ARC_WakeUp_Flag=0;
	if(*HDMI_Wakeup_Flag == 1)
	{
       ARC_WakeUp_Flag = *HDMI_Wakeup_Flag;
	}
	*HDMI_Wakeup_Flag=0;
	return ARC_WakeUp_Flag;
}

/******************************************************************************************/
/**
 * \fn			int IOSrv_CECEvt_CbFunc(BYTE opcode, BYTE *buf, int len)
 *
 * \brief		CEC CMD
 *
 * \param
 *
 * \return		 int power on state
 *
 *
 ******************************************************************************************/

int HDMISrv_CECEvt_CbFunc(BYTE opcode, BYTE *buf, int len)
{
	if (buf == NULL)
		return -1;

	stCECEvt_t *e = Q_NEW(stCECEvt_t, HDMI_PRI_CEC_EVENT_SIG);
	e->stCecInfo.opcode = opcode;
	memcpy(e->stCecInfo.pCEC_Data, buf, len);
	e->stCecInfo.size = len;

	QACTIVE_POST(HDMISrv_get(), (QEvt *)e, HDMISrv_get());
	return 0;
}

int HDMISrv_HDMI_PlugIn(void const *pSender)
{
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_HDMI_PLUG_IN_SIG), pSender);
	return 0;
}

int HDMISrv_HDMI_PlugOut(void const *pSender)
{
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_HDMI_PLUG_OUT_SIG), pSender);
	return 0;
}


int HDMISrv_ARC_Initiate(void const *pSender)
{
	(void)pSender;
	int iRes = 0;

	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_CEC_INIT_SIG), pSender);

	//iRes = CECInit(l_IopScanner.pstCEC_handler);

	return iRes;
}

int HDMISrv_ARC_Terminate(void const *pSender)
{
	(void)pSender;
	int iRes = 0;

	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_CEC_TERMINATE_SIG), pSender);

	//iRes = CEC_Terminate(l_IopScanner.pstCEC_handler);

	return iRes;
}

int HDMISrv_CEC_Set_SAC(void const *pSender, int iEnable)
{
	(void)pSender;
	int iRes = 0;

	stCecSetSac_t *e = Q_NEW(stCecSetSac_t, HDMI_PRI_CEC_SET_SAC_SIG);
	e->bEnble = iEnable;
	QACTIVE_POST(HDMISrv_get(), &e->stEvt, pSender);
	LOGD("IOSrv_CEC_Set_SAC: %d\n", e->bEnble);

	return iRes;
}


int HDMISrv_CEC_SetSrc(void const *pSender, UINT8 pParam)
{
	if (pParam >= eIOP_CecSrc_Max)
	{
		LOGE(" Fail\n");
		return -1;
	}

	stCecSetSrc_t *e = Q_NEW(stCecSetSrc_t, HDMI_PRI_CEC_SET_SRC_SIG);
	e->bCecSrc = pParam;
	QACTIVE_POST(HDMISrv_get(), &e->stEvt, pSender);

	LOGD("IOSrv_CEC_SetSrc: %d\n", e->bCecSrc);

	return 0;
}

int HDMISrv_HDMI_TX_Initiate(void const *pSender)
{
	(void)pSender;
	int iRes = 0;

	LOGD("IOSrv_HDMI_TX_Initiate\n");
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_CEC_INIT_SIG), pSender);
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_HDMI_TX_INIT_SIG), pSender);

	return iRes;
}

int HDMISrv_HDMI_TX_Terminate(void const *pSender)
{
	(void)pSender;
	int iRes = 0;

	LOGD("IOSrv_HDMI_TX_Terminate\n");
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_CEC_TERMINATE_SIG), pSender);
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_PRI_HDMI_TX_TERMINATE_SIG), pSender);

	return iRes;
}


void HDMISrv_Terminate(QActive *const me, void *pParam)
{
	(void)me;  /*!< suppress the "unused parameter" compiler warning */
	(void)pParam;  /*!< suppress the "unused parameter" compiler warning */
	LOGD("[%s]\n", __FUNCTION__);

#ifdef WD_TICK_ENABLE
	iop_wd_disable();
#endif

	//l_HDMISrv.iTerminatePorcessEnable = 1;// start terminate process

	CEC_StandbyTerminate(l_HDMISrv.pstCEC_handler);

	//LOGD("[%s][%d] IOSrv_Terminate end.\n", __FUNCTION__, __LINE__);
}

int HDMISrv_Terminate_Check(HDMISrv *const me)
{
	int res = 0;

	if (me->pstCEC_cfg->Cfg_Enable == 1)
	{
		if (HDMI_get_HPD(me->pstCEC_handler))
		{
			res = CEC_TerminatedCheck(me->pstCEC_handler);
		}
		else
		{
			res = 1;
		}
	}
	else
	{
		res = 1;
	}

	//if (me->pstCEC_handler->CecEnable == 0)
	//    res = 1;

	return res;
}

int HDMISrv_TerminateProcessFinished(void)
{
	LOGD("[%s][%d].\n", __FUNCTION__, __LINE__);

	PowerService_FinishPowerOff(HDMISrv_get());

	return 0;
}

/******************************************************************************************/
/**
 * \fn              int IOSrv_FinishHPDBroadcastFree(void *pParam)
 *
 * \brief          free HPD info structure
 *
 * \param       void *pParam: the structure pointer which have to free \n
 *
 * \return	       \e none \n
 *
 *
 ******************************************************************************************/
void HDMISrv_FinishHPDBroadcastFree(void *pParam)
{
	stHDMI_HPDInfo_t *pstHDMI_HPDInfo = (stHDMI_HPDInfo_t *)pParam;
	free(pstHDMI_HPDInfo);
}

int HDMISrv_HPDBroadcast_Request(void const *pSender)
{
	QACTIVE_POST(HDMISrv_get(), Q_NEW(QEvt, HDMI_CEC_HPD_BROADCAST_REQ_SIG), pSender);

	return 0;
}

/******************************************************************************************/
/**
 * \fn              int IOSrv_HPDBroadcast(void const * pSender, UINT8 bState)
 *
 * \brief          Broadcast HPD state
 *
 * \param       void const * pSender: AO handler of sender \n
 *                  UINT8 bState: HPD state \n
 *
 * \return	       \e Broadcast result \n
 *
 *
 ******************************************************************************************/
int HDMISrv_HPDBroadcast(void const *pSender, UINT8 bState)
{
	AUTORETURNEVT *pstARE;
	stHDMI_HPDInfo_t *pstHDMI_HPD;

	pstHDMI_HPD = (stHDMI_HPDInfo_t *)malloc(sizeof(stHDMI_HPDInfo_t));

	(void)pSender;

	if (bState > 1)
	{
		return -1;
	}

	pstHDMI_HPD->bState = bState;

	LOGD("IOSrv_HPDBroadcast HPD bState: %d\n", pstHDMI_HPD->bState);

	Q_NEW_RESPONSE(pstARE, IOP_CEC_HPD_SIG, pstHDMI_HPD, HDMISrv_FinishHPDBroadcastFree);
	QF_PUBLISH(&pstARE->super, pSender);

	return 0;
}


/******************************************************************************************/
/**
 * \fn              int IOSrv_RegHPDListener(QActive * const me, ARFunc_t pfFunc)
 *
 * \brief          register HPD listener
 *
 * \param       QActive* const me(Input): AO handler of register\n
 *                  ARFunc_t pfFunc(Input): callback function \n
 *
 * \return	       \e register result \n
 *
 *
 ******************************************************************************************/
int HDMISrv_RegHPDListener(QActive *const me, ARFunc_t pfFunc)
{
	int iRes = 0;
	LOGD("[%s] ;\n", __FUNCTION__);

	if ((pfFunc == NULL) || (me == NULL))
	{
		LOGE(" error;\n");

		return -1;
	}

	iRes = QF_ListenerInit(me, IOP_CEC_HPD_SIG, pfFunc);

	//IOSrv_HPDBroadcast(&l_IopScanner.super, l_IopScanner.pstCEC_handler->HpdStatus);
	HDMISrv_HPDBroadcast_Request(&me->super);
	return iRes;
}

int HDMISrv_UnRegHPDListener(QActive *const me)
{
	int iRes = 0;
	LOGD("[%s] ;\n", __FUNCTION__);

	if (me == NULL)
	{
		LOGE(" error;\n");

		return -1;
	}

	iRes = QF_ListenerDelete(me, IOP_CEC_HPD_SIG);

	return iRes;
}

/******************************************************************************************/
/**
 * \fn              int IOSrv_CecEvtBroadcastFree(void *pParam)
 *
 * \brief          free CEC event info structure
 *
 * \param       void *pParam: the structure pointer which have to free (stCecCmdInfo_t) \n
 *
 * \return	       \e none \n
 *
 *
 ******************************************************************************************/
void HDMISrv_CecEvtBroadcastFree(void *pParam)
{
	stCecCmdInfo_t *pstCecInfo = (stCecCmdInfo_t *)pParam;
	free(pstCecInfo);
}


/******************************************************************************************/
/**
 * \fn              int IOSrv_CecEvtBroadcast(void const * pSender, UINT8 bState)
 *
 * \brief          Broadcast specific CEC event
 *
 * \param       void const * pSender: AO handler of sender \n
 *                  stCecCmdInfo_t* pstCecInfo: cec info \n
 *
 * \return	       \e Broadcast result \n
 *
 *
 ******************************************************************************************/
int HDMISrv_CecEvtBroadcast(void const *pSender, stCecCmdInfo_t *pstCecInfo)
{
	AUTORETURNEVT *pstARE;
	stCecCmdInfo_t *pstCEC_info;

	pstCEC_info = (stCecCmdInfo_t *)malloc(sizeof(stCecCmdInfo_t));

	if (pstCecInfo == NULL)
	{
		return -1;
	}

	memcpy(pstCEC_info, pstCecInfo, sizeof(stCecCmdInfo_t));

	LOGD("HDMISrv_CecEvtBroadcast opcode: 0x%x\n", pstCEC_info->opcode);

	Q_NEW_RESPONSE(pstARE, IOP_CEC_EVT_SIG, pstCEC_info, HDMISrv_CecEvtBroadcastFree);
	QF_PUBLISH(&pstARE->super, pSender);

	return 0;
}


/******************************************************************************************/
/**
 * \fn              int IOSrv_RegCecEvtListener(QActive * const me, ARFunc_t pfFunc)
 *
 * \brief          register CEC event listener
 *
 * \param       QActive* const me(Input): AO handler of register\n
 *                  ARFunc_t pfFunc(Input): callback function \n
 *
 * \return	       \e register result \n
 *
 *
 ******************************************************************************************/
int HDMISrv_RegCecEvtListener(QActive *const me, ARFunc_t pfFunc)
{
	int iRes = 0;
	LOGD("[%s] ;\n", __FUNCTION__);

	if ((pfFunc == NULL) || (me == NULL))
	{
		LOGE(" error;\n");

		return -1;
	}

	iRes = QF_ListenerInit(me, IOP_CEC_EVT_SIG, pfFunc);

	return iRes;
}

int HDMISrv_UnRegCecEvtListener(QActive *const me)
{
	int iRes = 0;
	LOGD("[%s] ;\n", __FUNCTION__);

	if (me == NULL)
	{
		LOGE(" error;\n");

		return -1;
	}

	iRes = QF_ListenerDelete(me, IOP_CEC_EVT_SIG);

	return iRes;
}

int HDMISrv_HDMI_mode(HDMI_OP_T *pHdmi_op_fn, UINT8 *pEdidData)
{
	int iRet = -1;
	LOGE("HDMISrv_HDMI_mode :pHdmi_op_fn %d\n",pHdmi_op_fn);

	if (pHdmi_op_fn != NULL)
	{
		iRet = pHdmi_op_fn->pfn_hdmi_mode(pEdidData);
	}
	else
	{
		LOGE("pfn_hdmi_mode err\n");
	}

	return iRet;
}

int HDMISrv_TV_resolution(HDMI_OP_T *pHdmi_op_fn, UINT8 *pEdidData)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		iRet = pHdmi_op_fn->pfn_tv_resolution(pEdidData);
	}
	else
	{
		LOGE("pfn_tv_resolution err\n");
	}

	return iRet;
}

int HDMISrv_HDMI_480p(HDMI_OP_T *pHdmi_op_fn)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		pHdmi_op_fn->pfn_hdmi_480p();
		iRet = 0;
	}
	else
	{
		LOGE("pfn_hdmi_480p err\n");
	}

	return iRet;
}

int HDMISrv_DVI_480p(HDMI_OP_T *pHdmi_op_fn)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		pHdmi_op_fn->pfn_dvi_480p();
		iRet = 0;
	}
	else
	{
		LOGE("pfn_dvi_480p err\n");
	}

	return iRet;
}

int HDMISrv_HDMI_576p(HDMI_OP_T *pHdmi_op_fn)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		pHdmi_op_fn->pfn_hdmi_576p();
		iRet = 0;
	}
	else
	{
		LOGE("pfn_hdmi_576p err\n");
	}

	return iRet;
}

int HDMISrv_DVI_576p(HDMI_OP_T *pHdmi_op_fn)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		pHdmi_op_fn->pfn_dvi_576p();
		iRet = 0;
	}
	else
	{
		LOGE("pfn_dvi_576p err\n");
	}

	return iRet;
}

int HDMISrv_HDMI_disable(HDMI_OP_T *pHdmi_op_fn)
{
	int iRet = -1;

	if (pHdmi_op_fn != NULL)
	{
		pHdmi_op_fn->pfn_hdmi_disable();
		iRet = 0;
	}
	else
	{
		LOGE("pfn_hdmi_disable err\n");
	}

	return iRet;
}

int HDMISrv_RegCB(void)
{
	HDMI_RegCB(Power_HDMI_Standby_init,CEC_StandbyInitiate);
    HDMI_RegCB(Power_HDMI_GetPowerStatet,CEC_GetPowerState);
	return 0;
}

