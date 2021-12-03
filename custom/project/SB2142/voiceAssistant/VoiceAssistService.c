
#include "qpc.h"
#include "initcall.h"
#define LOG_MODULE_ID  ID_APVA
#define LOG_EXTRA_STR  "[VA]"
#include "log_utils.h"
#include "AudDspService.h"
#include "spXeffect_api.h"
#include "cfg_audtool.h"
#include "amp.h"
#include <string.h>
#include "cfg_sdk.h"
#include "sdk_cfg.h"
#include "systemservice.h"
#include "task_attribute_user.h"
#include "qf_response.h"
#include "project_cfg.h"
#include "pinmux.h"
#include "hw_gpio_list.h"
#include "custom_sig.h"
#include "VoiceAssistService.h"
#include "userapp.h"


#define va_printf(f, a...) LOGD(f, ##a)
#define va_printf2(f, a...) LOGDT(f, ##a)
#define va_printf_T(f, a...) LOGD("[%s][%d]"f, __FUNCTION__, __LINE__, ##a)

/*..........................................................................*/
typedef struct       /* the VoiceAssist active object */
{
	QActive super;   /* inherit QActive */

	QTimeEvt timeEvt;

	QTimeEvt timeBGMMuteEvt; /* ducking 10s mute */
	QTimeEvt timeResumeBGMEvt; /* va stop, bgm resume */
	QTimeEvt timeResumeBtVAEvt; /* auxva stop, btva resume */
	QTimeEvt timeWaitEnergyEvt; /* wait audio service energy state return */
	QTimeEvt TimePolling; /* VA AO Polling*/

	uint8_t btva_status;	/* bt va on/off status */
	uint8_t auxva_status;	/* aux va on/off status */
	uint8_t auxva_energy_state;		/* aux va play state */
	uint8_t btva_energy_state;		/* bt va play state */
	uint8_t bgm_energy_state;		/* bgm play state */
	uint8_t bgm_duck_state;		/* va play, bgm decrease to 20% */
	uint8_t bgm_mute_state;		/* va play, bgm mute state*/
	uint32_t BGMMuteTime;
	uint32_t BGMResumeTime;
	uint8_t src;
	uint8_t va_en_flag;

	OVoiceAssistant VA;
} VoiceAssist_t;

static VoiceAssist_t gVoiceAssist; /* the VoiceAssist active object */

QActive *const AO_VoiceAssist = &gVoiceAssist.super;

QState VoiceAssist_Top(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOn(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnBGMDuck(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnBGMResume(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnBTResume(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOff(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOffBGMDuck(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOffBGMResume(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOffBTOn(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOffBTOnBGMDuck(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOffBTOnBGMResume(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOffBTOff(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnAuxActBGMDuck(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnBTActBGMDuck(VoiceAssist_t *const me, QEvt const *const e);
QState VoiceAssist_AuxOnBTOnInit(VoiceAssist_t *const me);
QState VoiceAssist_AuxOnBTOffInit(VoiceAssist_t *const me);
QState VoiceAssist_AuxOffBTOnInit(VoiceAssist_t *const me);
QState VoiceAssist_AuxOffBTOffInit(VoiceAssist_t *const me);
QState VoiceAssist_AuxOnBTOnProcess(VoiceAssist_t *const me);
QState VoiceAssist_AuxOnBTOffProcess(VoiceAssist_t *const me);
QState VoiceAssist_AuxOffBTOnProcess(VoiceAssist_t *const me);

/******************************************************************************************/
/**
 * \fn			QActive *VoiceAssist_get(void)
 *
 * \brief		get receiver handler
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
QActive *VoiceAssist_get(void)
{
	return AO_VoiceAssist;
}

QState VoiceAssitAOInit(VoiceAssist_t *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */

	va_printf("%s \n", __FUNCTION__);

	return Q_TRAN(&VoiceAssist_Top);
}

void VoiceAssist_Ctor(void)
{
	VoiceAssist_t *const me = &gVoiceAssist;
	/* instantiate the va active objects */
	QActive_ctor(&me->super, Q_STATE_CAST(&VoiceAssitAOInit));


	CFG_VA_T *pstVA_Cfg = GET_SDK_CFG(CFG_VA_T);
	me->BGMMuteTime = pstVA_Cfg->BGMMuteTime;
	me->BGMResumeTime = pstVA_Cfg->BGMResumeTime;

	QTimeEvt_ctorX(&me->timeBGMMuteEvt, &me->super, VA_TIME_BGM_MUTE_SIG, 0U);
	QTimeEvt_ctorX(&me->timeResumeBGMEvt, &me->super, VA_TIME_BGM_RESUME_SIG, 0U);
	QTimeEvt_ctorX(&me->timeResumeBtVAEvt, &me->super, VA_TIME_BT_RESUME_SIG, 0U);
	QTimeEvt_ctorX(&me->timeWaitEnergyEvt, &me->super, VA_TIME_WAIT_ENERGY_SIG, 0U);
    QTimeEvt_ctorX(&me->TimePolling, &me->super, VA_TIME_POLLING_SIG, 0U);
}

/******************************************************************************************/
/**
 * \fn			void VoiceAssist_Start(void)
 *
 * \brief		Start the VoiceAssist AO
 * \param
 *
 * \return	    \e none \n
 *
 *
 ******************************************************************************************/
void VoiceAssist_Start(void)
{
	static QEvt const *pstVoiceAssistQueueSto[20];

	VoiceAssist_Ctor();

	QACTIVE_START(AO_VoiceAssist,           /*!< AO to start */
				  TSK_VASRV_PRIO, 			/*!< QP priority of the AO */
				  pstVoiceAssistQueueSto,      /*!< event queue storage */
				  Q_DIM(pstVoiceAssistQueueSto), /*!< queue length [events] */
				  TSK_VASRV_NAME,             /* AO name */
				  TSK_VASRV_STACK_DEPTH,      /*!< size of the stack [bytes] */
				  (QEvt *)0);             /*!< initialization event */
}


#if (SUPPORT_VA)
AO_INIT(VoiceAssist_Start);
#endif

void VoiceAssi_Active_Detecte(VoiceAssist_t *const me)
{
    UINT32 energy_aux_va, energy_bt_va, energy_bgm;


	energy_aux_va = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_VA_Enr, VA_AUX, 0);
	energy_bt_va = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_VA_Enr, VA_BT, 0);
	energy_bgm = AudDspService_VoiceAssist_Contrl(NULL, VA_Get_BGM_Enr, 0, 0);

    if(1)
    {
        if(energy_aux_va>=ENERGY_AUX_THD)
        {
            me->VA.Aux.wEnergyCounter++;
            if(me->VA.Aux.wEnergyCounter>ActivDtectedTime)
            {
                me->VA.Aux.wEnergyCounter = ActivDtectedTime;
                me->auxva_energy_state = 1;
                if(!me->VA.Aux.bState && me->auxva_status)
                {
					QACTIVE_POST(VoiceAssist_get(), Q_NEW(QEvt, AUXVA_ENERGY_ENABLE), (void*)0);
                }
                me->VA.Aux.bState = 1;
                me->VA.Aux.wActivCounter++;
                if(me->VA.Aux.wActivCounter>CounterMaxTime)
                {
                    me->VA.Aux.wActivCounter = CounterMaxTime;
                }
                me->VA.Aux.wInActivCounter = 0;
            }

        }
        else
        {
            if(me->VA.Aux.wEnergyCounter>0)
                me->VA.Aux.wEnergyCounter--;
            else
            {
                me->auxva_energy_state = 0;
                if(me->VA.Aux.bState && me->auxva_status)
                {
					QACTIVE_POST(VoiceAssist_get(), Q_NEW(QEvt, AUXVA_ENERGY_DISABLE), (void*)0);
                }
                me->VA.Aux.bState = 0;
                me->VA.Aux.wInActivCounter++;
                if(me->VA.Aux.wInActivCounter>CounterMaxTime)
                {
                    me->VA.Aux.wInActivCounter = CounterMaxTime;
                }
                me->VA.Aux.wActivCounter = 0;
            }
        }
    }
    else
    {
        me->VA.Aux.wEnergyCounter = 0;
        me->VA.Aux.bState = 0;
        me->VA.Aux.wActivCounter = 0;
        me->VA.Aux.wInActivCounter = 0;
    }


    if(1)
    {
        if(energy_bt_va>=ENERGY_BT_THD)
        {
            me->VA.Bt.wEnergyCounter++;
            if(me->VA.Bt.wEnergyCounter>ActivDtectedTime)
            {
                me->VA.Bt.wEnergyCounter = ActivDtectedTime;
                me->btva_energy_state = 1;
                if(!me->VA.Bt.bState && me->btva_status)
                {
					QACTIVE_POST(VoiceAssist_get(), Q_NEW(QEvt, BTVA_ENERGY_ENABLE), (void*)0);
                }
                me->VA.Bt.bState = 1;
                me->VA.Bt.wActivCounter++;
                if(me->VA.Bt.wActivCounter>CounterMaxTime)
                {
                    me->VA.Bt.wActivCounter = CounterMaxTime;
                }
                me->VA.Bt.wInActivCounter = 0;
            }
        }
        else
        {
            if(me->VA.Bt.wEnergyCounter>0)
                me->VA.Bt.wEnergyCounter--;
            else
            {
                me->btva_energy_state = 0;
                if(me->VA.Bt.bState && me->btva_status)
                {
					QACTIVE_POST(VoiceAssist_get(), Q_NEW(QEvt, BTVA_ENERGY_DISABLE), (void*)0);
                }
                me->VA.Bt.bState = 0;
                me->VA.Bt.wInActivCounter++;
                if(me->VA.Bt.wInActivCounter>CounterMaxTime)
                {
                    me->VA.Bt.wInActivCounter = CounterMaxTime;
                }
                me->VA.Bt.wActivCounter = 0;
            }
        }
    }
    else
    {
        me->VA.Bt.wEnergyCounter = 0;
        me->VA.Bt.bState = 0;
        me->VA.Bt.wActivCounter = 0;
        me->VA.Bt.wInActivCounter = 0;

    }

	if((me->btva_energy_state || me->auxva_energy_state) && !me->va_en_flag)
	{
		me->va_en_flag = 1;
		QACTIVE_POST(VoiceAssist_get(), Q_NEW(QEvt, VA_ENERGY_ENABLE), (void*)0);
	}
}

void VoiceAssist_SetAuxVAVol(double dB)
{
    UINT32 Gain;
    Gain = VAdBtoLineQ527(dB);
	AudDspService_VoiceAssist_Contrl(NULL, VA_Set_VAGain, VA_AUX, Gain);
}

void VoiceAssist_SetBtVAVol(double dB)
{
    UINT32 Gain;
    Gain = VAdBtoLineQ527(dB);
	AudDspService_VoiceAssist_Contrl(NULL, VA_Set_VAGain, VA_BT, Gain);
}

void VoiceAssist_SetBGMVol(double dB)
{
    UINT32 Gain;
    Gain = VAdBtoLineQ527(dB);
	AudDspService_VoiceAssist_Contrl(NULL, VA_Set_BGMGain, 0, Gain);
}

void VoiceAssist_SetBTDuck(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_BT)
	{
		VoiceAssist_SetBGMVol(DUCK_VOL);
	}
	else
	{
		VoiceAssist_SetBtVAVol(DUCK_VOL);
	}
}

void VoiceAssist_SetBTMute(VoiceAssist_t *const me)
{
	(void)me;
	va_printf_T("mute bt\n");
	if(me->src == SRC_BT)
	{
		VoiceAssist_SetBGMVol(MUTE_VOL);
	}
	else
	{
		VoiceAssist_SetBtVAVol(MUTE_VOL);
	}
}

void VoiceAssist_SetBTResume(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_BT)
	{
		VoiceAssist_SetBGMVol(NORMAL_VOL);
	}
	else
	{
		VoiceAssist_SetBtVAVol(NORMAL_VOL);
	}
}

void VoiceAssist_SetAuxDuck(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_AUX)
	{
		VoiceAssist_SetBGMVol(DUCK_VOL);
	}
	else
	{
		VoiceAssist_SetAuxVAVol(DUCK_VOL);
	}
}

void VoiceAssist_SetAuxMute(VoiceAssist_t *const me)
{
	(void)me;
	va_printf_T("mute aux\n");
	if(me->src == SRC_AUX)
	{
		VoiceAssist_SetBGMVol(MUTE_VOL);
	}
	else
	{
		VoiceAssist_SetAuxVAVol(MUTE_VOL);
	}
}

void VoiceAssist_SetAuxResume(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_AUX)
	{
		VoiceAssist_SetBGMVol(NORMAL_VOL);
	}
	else
	{
		VoiceAssist_SetAuxVAVol(NORMAL_VOL);
	}
}

void VoiceAssist_SetAuxBtMute(VoiceAssist_t *const me)
{
	(void)me;
	va_printf_T("mute aux and bt\n");
	VoiceAssist_SetAuxMute(me);
	VoiceAssist_SetBTMute(me);
}

void VoiceAssist_SetAuxBtResume(VoiceAssist_t *const me)
{
	(void)me;
	va_printf_T("resume aux and bt\n");
	VoiceAssist_SetAuxResume(me);
	VoiceAssist_SetBTResume(me);
}

void VoiceAssist_SetBGMDuck(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_AUX && me->btva_status && !me->auxva_status)
	{
		VoiceAssist_SetAuxDuck(me);
	}
	else if(me->src == SRC_BT && !me->btva_status && me->auxva_status)
	{
		VoiceAssist_SetBTDuck(me);
	}
	else if(me->src == SRC_AUX && me->auxva_status)
	{
		va_printf_T("do nothing\n");
	}
	else if(me->src == SRC_BT && me->btva_status)
	{
		va_printf_T("do nothing\n");
	}
	else
	{
		VoiceAssist_SetBGMVol(DUCK_VOL);
	}
}

void VoiceAssist_SetBGMMute(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_AUX && me->btva_status && !me->auxva_status)
	{
		VoiceAssist_SetAuxMute(me);
	}
	else if(me->src == SRC_BT && !me->btva_status && me->auxva_status)
	{
		VoiceAssist_SetBTMute(me);
	}
	else if(me->src == SRC_AUX && me->auxva_status)
	{
		va_printf_T("do nothing\n");
	}
	else if(me->src == SRC_BT && me->btva_status)
	{
		va_printf_T("do nothing\n");
	}
	else
	{
		VoiceAssist_SetBGMVol(MUTE_VOL);
	}
}

void VoiceAssist_StatusSwClear(VoiceAssist_t *const me)
{
	va_printf_T("clear va status\n");
	QTimeEvt_disarmX(&me->timeBGMMuteEvt);
	QTimeEvt_disarmX(&me->timeResumeBGMEvt);
	QTimeEvt_disarmX(&me->timeResumeBtVAEvt);
	QTimeEvt_disarmX(&me->timeWaitEnergyEvt);
	//me->bgm_duck_state = 0;
	//me->bgm_mute_state = 0;
}

void VoiceAssist_SetBGMResume(VoiceAssist_t *const me)
{
	(void)me;
	if(me->src == SRC_AUX && me->btva_status && !me->auxva_status)
	{
		VoiceAssist_SetAuxResume(me);
	}
	else if(me->src == SRC_BT && !me->btva_status && me->auxva_status)
	{
		VoiceAssist_SetBTResume(me);
	}
	else
	{
		VoiceAssist_SetBGMVol(NORMAL_VOL);
	}
}

QState VoiceAssist_Top(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");

			QTimeEvt_disarmX(&me->TimePolling);
			QTimeEvt_armX(&me->TimePolling, TICKS_PER_100MS, 0);
			status =  Q_HANDLED();
			break;
		}
		case VA_STATUS_SIG:
		{
			va_printf_T("receive VA_STATUS_SIG\n");

			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);
			status =  Q_HANDLED();
			break;
		}
		case VA_STATUS_RESET_SIG:
		{
			va_printf_T("receive VA_STATUS_RESET_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 0;

			status =  Q_HANDLED();
			break;
		}
		case VA_ENERGY_ENABLE:
		{
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			// AUX VA input -> Ain1
			// VA use mixer 0
			AudDspService_Prepare_VA(&me->super, AIN0, 0);

			if(me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOn);
			}
			else if(me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOff);
			}
			else if(!me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOn);
			}
			else if(!me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOff);
			}
			else{
				status =  Q_HANDLED();
			}
			break;
		}
		case VA_SOURCE_SIG:
		{
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->src = pVAStatusEvt->src;
			va_printf("src:%d\n", me->src);

			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_MUTE_SIG:
		{
			va_printf_T("timer VA_TIME_BGM_MUTE_SIG\n");
			//bgm entirely mute
			VoiceAssist_SetBGMMute(me);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_POLLING_SIG:
		{
			VoiceAssi_Active_Detecte(me);
			QTimeEvt_armX(&me->TimePolling, TICKS_PER_100MS, 0);
			status =  Q_HANDLED();
			break;
		}
		case VA_SET_AUX_MUTE_SIG:
		{
			VoiceAssist_SetAuxMute(me);
			status =  Q_HANDLED();
			break;
		}
		case VA_SET_AUX_RESUME_SIG:
		{
			VoiceAssist_SetAuxResume(me);
			status =  Q_HANDLED();
			break;
		}
		case VA_SET_BT_MUTE_SIG:
		{
			VoiceAssist_SetBTMute(me);
			status =  Q_HANDLED();
			break;
		}
		case VA_SET_BT_RESUME_SIG:
		{
			VoiceAssist_SetBTResume(me);
			status =  Q_HANDLED();
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

QState VoiceAssist_AuxOnBTOnAuxActBGMDuck(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_MUTE_SIG:
		{
			va_printf_T("bgm mute\n");
			//mute bgm
			VoiceAssist_SetBGMMute(me);
			me->bgm_duck_state = 0;
			me->bgm_mute_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;

			if(me->btva_energy_state && me->btva_status)
			{
				QTimeEvt_disarmX(&me->timeResumeBtVAEvt);
				QTimeEvt_armX(&me->timeResumeBtVAEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
				status =  Q_TRAN(VoiceAssist_AuxOnBTOnBTResume);
			}
			else
			{
				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_disarmX(&me->timeResumeBGMEvt);
				QTimeEvt_armX(&me->timeResumeBGMEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
				status =  Q_TRAN(VoiceAssist_AuxOnBTOnBGMResume);
			}
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;
			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOnBTActBGMDuck(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_MUTE_SIG:
		{
			va_printf_T("bgm mute\n");
			//mute bgm
			VoiceAssist_SetBGMMute(me);
			me->bgm_duck_state = 0;
			me->bgm_mute_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			QTimeEvt_disarmX(&me->timeResumeBGMEvt);
			QTimeEvt_armX(&me->timeResumeBGMEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
			status =  Q_TRAN(VoiceAssist_AuxOnBTOnBGMResume);
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			VoiceAssist_SetBTMute(me);
			QTimeEvt_disarmX(&me->timeResumeBGMEvt);
			QTimeEvt_disarmX(&me->timeResumeBtVAEvt);

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("do nothing\n");
				status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			}
			else
			{
				//bgm decrease
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOnBGMResume(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_RESUME_SIG:
		{
			va_printf_T("VA_TIME_BGM_RESUME_SIG\n");
			QTimeEvt_disarmX(&me->timeBGMMuteEvt);
			VoiceAssist_SetBGMResume(me);
			me->bgm_mute_state = 0;
			me->bgm_duck_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			VoiceAssist_SetBTMute(me);
			QTimeEvt_disarmX(&me->timeResumeBGMEvt);

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("do nothing\n");
				status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			}
			else
			{
				//bgm decrease
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			}
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				status =  Q_TRAN(VoiceAssist_AuxOnBTOnBTActBGMDuck);
			}
			else
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOnBTActBGMDuck);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOnBTResume(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BT_RESUME_SIG:
		{
			va_printf_T("VA_TIME_BT_RESUME_SIG\n");
			VoiceAssist_SetBTResume(me);
			//me->bgm_mute_state = 0;
			//me->bgm_duck_state = 0;
			status =  Q_TRAN(VoiceAssist_AuxOnBTOnBTActBGMDuck);
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			QTimeEvt_disarmX(&me->timeResumeBtVAEvt);
			me->auxva_energy_state = 1;
			VoiceAssist_SetBTMute(me);

			status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;

			QTimeEvt_disarmX(&me->timeResumeBGMEvt);
			QTimeEvt_armX(&me->timeResumeBGMEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
			status = Q_TRAN(VoiceAssist_AuxOnBTOnBGMResume);
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOnInit(VoiceAssist_t *const me)
{
	QState status = 0;
	va_printf_T("auxva_state:%d, btva_state:%d\n", me->auxva_energy_state, me->btva_energy_state);
	VoiceAssist_StatusSwClear(me);
	if(me->auxva_energy_state)
	{
		va_printf_T("only resume aux\n");
		VoiceAssist_SetBTMute(me);
		VoiceAssist_SetAuxResume(me);
	}
	else
	{
		VoiceAssist_SetAuxBtResume(me);
	}

	status = VoiceAssist_AuxOnBTOnProcess(me);

	return status;

}

QState VoiceAssist_AuxOnBTOnProcess(VoiceAssist_t *const me)
{
	QState status = 0;
	va_printf_T("auxva_state:%d, btva_state:%d\n", me->auxva_energy_state, me->btva_energy_state);
	if(me->auxva_energy_state){
		if(me->bgm_mute_state || me->bgm_duck_state)
		{
			va_printf_T("bgm state is duck or mute, do nothing\n");
		}
		else
		{
			//bgm decrease
			VoiceAssist_SetBGMDuck(me);
			me->bgm_duck_state = 1;
			me->bgm_mute_state = 0;
		}
		VoiceAssist_SetBTMute(me);

		QTimeEvt_disarmX(&me->timeBGMMuteEvt);
		QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
		status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
	}
	else{
		if(me->btva_energy_state){
			if(me->bgm_mute_state || me->bgm_duck_state)
			{
				va_printf_T("bgm state is duck or mute, do nothing\n");
			}
			else
			{
				//bgm decrease
				VoiceAssist_SetBGMDuck(me);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
			}

			QTimeEvt_disarmX(&me->timeBGMMuteEvt);
			QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
			status = Q_TRAN(VoiceAssist_AuxOnBTOnBTActBGMDuck);
		}
		else{
			va_printf_T("do nothing\n");
			status = Q_HANDLED();
		}
	}

	return status;
}

QState VoiceAssist_AuxOnBTOn(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status = VoiceAssist_AuxOnBTOnInit(me);
			break;
		}
		case VA_STATUS_SIG:
		{
			va_printf_T("receive VA_STATUS_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			if(me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOff);
			}
			else if(!me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOn);
			}
			else if(!me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOff);
			}
			else{
				va_printf("The cur state are the tar state are the same, don't switch..\n");
				status =  Q_HANDLED();
			}
			break;
		}
		case VA_STATUS_RESET_SIG:
		{
			va_printf_T("receive VA_STATUS_RESET_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 0;

			status = VoiceAssist_AuxOnBTOnInit(me);
			break;
		}
		case VA_TIME_WAIT_ENERGY_SIG:
		{
			va_printf("VA_TIME_WAIT_ENERGY_SIG\n");

			status =  VoiceAssist_AuxOnBTOnProcess(me);
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			VoiceAssist_SetBGMDuck(me);
			VoiceAssist_SetBTMute(me);

			QTimeEvt_disarmX(&me->timeBGMMuteEvt);
			QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
			me->bgm_duck_state = 1;
			me->bgm_mute_state = 0;
			status = Q_TRAN(VoiceAssist_AuxOnBTOnAuxActBGMDuck);
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;

			if(!me->auxva_energy_state)
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOnBTActBGMDuck);
			}
			else
			{
				status = Q_HANDLED();
			}
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_Top);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOffBGMDuck(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_MUTE_SIG:
		{
			va_printf_T("bgm mute\n");
			//mute bgm
			VoiceAssist_SetBGMMute(me);
			me->bgm_duck_state = 0;
			me->bgm_mute_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;

			if(me->btva_energy_state && me->btva_status)
			{
				va_printf_T("do nothing\n");
				status =  Q_HANDLED();
			}
			else
			{
				QTimeEvt_disarmX(&me->timeResumeBGMEvt);
				QTimeEvt_armX(&me->timeResumeBGMEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
				status =  Q_TRAN(VoiceAssist_AuxOnBTOffBGMResume);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOff);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOffBGMResume(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_RESUME_SIG:
		{
			va_printf_T("VA_TIME_BGM_RESUME_SIG\n");
			QTimeEvt_disarmX(&me->timeBGMMuteEvt);
			VoiceAssist_SetBGMResume(me);
			me->bgm_mute_state = 0;
			me->bgm_duck_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			QTimeEvt_disarmX(&me->timeResumeBGMEvt);

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("do nothing\n");
				status = Q_TRAN(VoiceAssist_AuxOnBTOffBGMDuck);
			}
			else
			{
				//bgm decrease
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOffBGMDuck);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOnBTOff);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOnBTOffInit(VoiceAssist_t *const me)
{
	QState status = 0;
	VoiceAssist_StatusSwClear(me);
	VoiceAssist_SetAuxResume(me);

	if(me->src != SRC_BT)
	{
		VoiceAssist_SetBTMute(me);
	}

	status = VoiceAssist_AuxOnBTOffProcess(me);

	return status;

}

QState VoiceAssist_AuxOnBTOffProcess(VoiceAssist_t *const me)
{
	QState status = 0;

	va_printf_T("auxva_state:%d\n", me->auxva_energy_state);

	if(me->auxva_energy_state){
		if(me->bgm_mute_state || me->bgm_duck_state)
		{
			va_printf_T("bgm state is duck or mute, do nothing\n");
		}
		else
		{
			//bgm decrease
			VoiceAssist_SetBGMDuck(me);
			me->bgm_duck_state = 1;
			me->bgm_mute_state = 0;
		}

		va_printf_T("bgm duck, start timer\n");
		QTimeEvt_disarmX(&me->timeBGMMuteEvt);
		QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
		status = Q_TRAN(VoiceAssist_AuxOnBTOffBGMDuck);
	}
	else{
		//do nothing
		status = Q_HANDLED();
	}


	return status;
}

QState VoiceAssist_AuxOnBTOff(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status = VoiceAssist_AuxOnBTOffInit(me);
			break;
		}
		case VA_STATUS_SIG:
		{
			va_printf_T("receive VA_STATUS_SIG\n");

			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			if(me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOn);
			}
			else if(!me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOn);
			}
			else if(!me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOff);
			}
			else{
				va_printf("The cur state are the tar state are the same, don't switch..\n");
				status =  Q_HANDLED();
			}
			break;
		}
		case VA_STATUS_RESET_SIG:
		{
			va_printf_T("receive VA_STATUS_RESET_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 0;

			status = VoiceAssist_AuxOnBTOffInit(me);
			break;
		}
		case VA_TIME_WAIT_ENERGY_SIG:
		{
			va_printf("VA_TIME_WAIT_ENERGY_SIG\n");

			status =  VoiceAssist_AuxOnBTOffProcess(me);
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			QTimeEvt_disarmX(&me->timeResumeBGMEvt);

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("do nothing\n");
				status =  Q_HANDLED();
			}
			else
			{
				//bgm decrease
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOnBTOffBGMDuck);
			}
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_Top);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOffBTOnBGMDuck(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_MUTE_SIG:
		{
			va_printf_T("bgm mute\n");
			//mute bgm
			VoiceAssist_SetBGMMute(me);
			me->bgm_duck_state = 0;
			me->bgm_mute_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			if(!me->auxva_status && me->btva_status)
			{
				QTimeEvt_disarmX(&me->timeResumeBGMEvt);
				QTimeEvt_armX(&me->timeResumeBGMEvt, (TICKS_PER_SEC * me->BGMResumeTime), 0);
				status =  Q_TRAN(VoiceAssist_AuxOffBTOnBGMResume);
			}
			else
			{
				va_printf_T("do nothing\n");
				status =  Q_HANDLED();
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOffBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOffBTOnBGMResume(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status =  Q_HANDLED();
			break;
		}
		case VA_TIME_BGM_RESUME_SIG:
		{
			va_printf_T("VA_TIME_BGM_RESUME_SIG\n");
			QTimeEvt_disarmX(&me->timeBGMMuteEvt);
			VoiceAssist_SetBGMResume(me);
			me->bgm_mute_state = 0;
			me->bgm_duck_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				status =  Q_TRAN(VoiceAssist_AuxOffBTOnBGMDuck);
			}
			else
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOffBTOnBGMDuck);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_AuxOffBTOn);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOffBTOnInit(VoiceAssist_t *const me)
{
	QState status = 0;
	VoiceAssist_StatusSwClear(me);
	VoiceAssist_SetBTResume(me);

	if(me->src != SRC_AUX)
	{
		VoiceAssist_SetAuxMute(me);
	}

	status = VoiceAssist_AuxOffBTOnProcess(me);
	return status;
}

QState VoiceAssist_AuxOffBTOnProcess(VoiceAssist_t *const me)
{
	QState status = 0;

	va_printf_T("btva_state:%d\n", me->btva_energy_state);
	va_printf_T("bgm_mute_state:%d, bgm_duck_state:%d\n", me->bgm_mute_state, me->bgm_duck_state);

	if(me->btva_energy_state){

		if(me->bgm_mute_state || me->bgm_duck_state)
		{
			va_printf_T("bgm state is duck or mute, do nothing\n");
		}
		else
		{
			//bgm decrease
			VoiceAssist_SetBGMDuck(me);
			me->bgm_duck_state = 1;
			me->bgm_mute_state = 0;
		}

		QTimeEvt_disarmX(&me->timeBGMMuteEvt);
		QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
		status = Q_TRAN(VoiceAssist_AuxOffBTOnBGMDuck);
	}
	else{
		//do nothing
		status = Q_HANDLED();
	}


	return status;
}

QState VoiceAssist_AuxOffBTOn(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status = VoiceAssist_AuxOffBTOnInit(me);
			break;
		}
		case VA_STATUS_SIG:
		{
			va_printf_T("receive VA_STATUS_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			if(me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOn);
			}
			else if(me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOff);
			}
			else if(!me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOff);
			}
			else{
				va_printf("The cur state are the tar state are the same, don't switch..\n");
				status =  Q_HANDLED();
			}
			break;
		}
		case VA_STATUS_RESET_SIG:
		{
			va_printf_T("receive VA_STATUS_RESET_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 0;

			status = VoiceAssist_AuxOffBTOnInit(me);
			break;
		}
		case VA_TIME_WAIT_ENERGY_SIG:
		{
			va_printf("VA_TIME_WAIT_ENERGY_SIG\n");

			status =  VoiceAssist_AuxOffBTOnProcess(me);
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;

			if(me->bgm_mute_state == 1 && me->bgm_duck_state == 0)
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				status = Q_TRAN(VoiceAssist_AuxOffBTOnBGMDuck);
			}
			else
			{
				va_printf_T("bt resume\n");
				VoiceAssist_SetBTResume(me);
				VoiceAssist_SetBGMDuck(me);

				QTimeEvt_disarmX(&me->timeBGMMuteEvt);
				QTimeEvt_armX(&me->timeBGMMuteEvt, (TICKS_PER_SEC * me->BGMMuteTime), 0);
				me->bgm_duck_state = 1;
				me->bgm_mute_state = 0;
				status = Q_TRAN(VoiceAssist_AuxOffBTOnBGMDuck);
			}
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_Top);
			break;
		}
	}
	return status;
}

QState VoiceAssist_AuxOffBTOffInit(VoiceAssist_t *const me)
{
	QState status = Q_HANDLED();
	(void)me;
	VoiceAssist_StatusSwClear(me);
	if(me->src == SRC_OTHER)
	{
		VoiceAssist_SetAuxBtMute(me);
	}
	else if(me->src == SRC_AUX)
	{
		VoiceAssist_SetBTMute(me);
		VoiceAssist_SetAuxResume(me);
	}
	else if(me->src == SRC_BT)
	{
		VoiceAssist_SetAuxMute(me);
		VoiceAssist_SetBTResume(me);
	}
	VoiceAssist_SetBGMResume(me);
	me->bgm_mute_state = 0;
	me->bgm_duck_state = 0;

	return status;
}

QState VoiceAssist_AuxOffBTOff(VoiceAssist_t *const me, QEvt const *const e)
{
	QState status = 0;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			va_printf_T("ENTRY\n");

			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			va_printf_T("EXIT\n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			va_printf_T("INIT\n");
			status = VoiceAssist_AuxOffBTOffInit(me);
			break;
		}
		case VA_STATUS_SIG:
		{
			va_printf_T("receive VA_STATUS_SIG\n");

			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			if(me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOn);
			}
			else if(me->auxva_status && !me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOnBTOff);
			}
			else if(!me->auxva_status && me->btva_status){
				status =  Q_TRAN(VoiceAssist_AuxOffBTOn);
			}
			else{
				va_printf("The cur state are the tar state are the same, don't switch..\n");
				status =  Q_HANDLED();
			}
			break;
		}
		case VA_STATUS_RESET_SIG:
		{
			va_printf_T("receive VA_STATUS_RESET_SIG\n");
			VAStatusEvt *pVAStatusEvt = ((VAStatusEvt *)e);
			me->auxva_status = pVAStatusEvt->auxva_status;
			me->btva_status = pVAStatusEvt->btva_status;
			va_printf("auxva_status:%d, btva_status:%d\n", me->auxva_status, me->btva_status);

			me->bgm_duck_state = 0;
			me->bgm_mute_state = 0;

			status = VoiceAssist_AuxOffBTOffInit(me);
			break;
		}
		case AUXVA_ENERGY_ENABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_ENABLE\n");
			me->auxva_energy_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case AUXVA_ENERGY_DISABLE:
		{
			va_printf_T("receive AUXVA_ENERGY_DISABLE\n");
			me->auxva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_ENABLE:
		{
			va_printf_T("receive BTVA_ENERGY_ENABLE\n");
			me->btva_energy_state = 1;
			status =  Q_HANDLED();
			break;
		}
		case BTVA_ENERGY_DISABLE:
		{
			va_printf_T("receive BTVA_ENERGY_DISABLE\n");
			me->btva_energy_state = 0;
			status =  Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&VoiceAssist_Top);
			break;
		}
	}
	return status;
}


