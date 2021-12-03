/******************************************************************************/
/**
*
* \file	 Display_Srv.c
*
* \brief	This file provides the iop scanner middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/9
******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "task_attribute_user.h"
#include "qpc.h"
#include "Display_Srv.h"
#include "VFD_Srv.h"
#include "KEY_Srv.h"
#include "LED_Srv.h"

#include "initcall.h"
#define LOG_MODULE_ID ID_DISP
#include "log_utils.h"
#include "LED_Drv.h"

#include "qf_sync.h"
#include "custom_sig.h"
#include "Repeater_Srv.h"
#include "cfg_sdk.h"


#if 1

Q_DEFINE_THIS_FILE;

/*..........................................................................*/
typedef struct       /* the Display active object */
{
	QActive super;   /* inherit QActive */
	QTimeEvt timeEvtVFD; /* private time event generator */
	VfdDisplay *stVfdDisplay; /* VfdDisplay orthogonal component */
	KeySrv *stKeySrv; /* key orthogonal component */
	LEDDisplay *stLEDDisplay;/*LED orthogonal component */
	LED_CFG_T *pstLEDCfg;/*LED config pointer*/
	VFD_CFG_T *pstVFDCfg;/*VFD config pointer*/
	int Vfd_GpioInit_Flag;
	#ifdef USERAPP_SUPPORT_REPEATER
	RepeaterSrv_t* pstRepeaterSrv;/*HDMI repeater */
	#endif
	//int Vfd_GpioInit_Flag;
	//QTimeEvt timeEvt_RepeaterPolling;
} DisplaySrv;

static DisplaySrv l_DisplaySrv; /* the DisplayScanner active object */
static VfdDisplay l_VfdDisplay; /* the VFD active object */
static KeySrv l_KeySrv;/* the KEY active object */
static LEDDisplay l_LEDDisplay; /* the LED active object */

#ifdef USERAPP_SUPPORT_REPEATER

/************* HDMI repeater ********************/

RepeaterSrv_t l_Repeater; /* the Repeater active object */

#endif
/**********************************************/

QActive *const AO_DisplaySrv = &l_DisplaySrv.super;

/* hierarchical state machine ... */
void DisplaySrv_ctor(void);
QState DisplaySrv_initial(DisplaySrv *const me, QEvt const *const e);
QState DisplaySrv_active(DisplaySrv *const me, QEvt const *const e);
void Display_VFD(QActive *super);


extern void *memcpy(void *pDst, const void *pSrc, UINT32 uiLen);
extern void *memset(void *pDst, int iFillValue, UINT32 uiLen);

/******************************************************************************************/
/**
 * \fn			QActive *DisplaySrv_get(void)
 *
 * \brief		get receiver handler
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
QActive *DisplaySrv_get(void)
{
	return AO_DisplaySrv;
}
/******************************************************************************************/
/**
 * \fn			void DisplaySrv_ctor(void)
 *
 * \brief		initial the Display_scanner AO paramater
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
void DisplaySrv_ctor(void)
{

	LOGD("DisplaySrv_ctor in; \n");

	DisplaySrv *const me = &l_DisplaySrv;

	QActive_ctor(&me->super, Q_STATE_CAST(&DisplaySrv_initial));

	QTimeEvt_ctorX(&me->timeEvtVFD, &me->super, DISPLAY_TICK_SIG, 0U);

	me->Vfd_GpioInit_Flag = 0;

	//Configure LED process parameter

	me->pstLEDCfg = GET_DISPLAY_CFG(LED_CFG_T);

	//Configure VFD process parameter

	me->pstVFDCfg = GET_DISPLAY_CFG(VFD_CFG_T);


	//*****VFD_Srv*****//
	if (me->pstVFDCfg->Cfg_Enable == 1)
	{
		// me->Vfd_GpioInit_Flag = Vfd_Drv_Init();
		me->Vfd_GpioInit_Flag = 0;// init vfd by POWERON_INIT()

		if (me->Vfd_GpioInit_Flag == 0)
		{
			me->stVfdDisplay = &l_VfdDisplay;

			VfdDisplay_ctor(me->stVfdDisplay); /* VFD orthogonal component ctor */
		}
		else
		{
			LOGD("DisplaySrv_ctor :VFD GPIO Init Fail...\n");
			LOGD("me->Vfd_GpioInit_Flag = %d\n", me->Vfd_GpioInit_Flag);
		}
	}

	//*****KEY_Srv*****//
	me->stKeySrv = &l_KeySrv;

	Key_ctor(me->stKeySrv);/* KEY orthogonal component ctor */

	#ifdef USERAPP_SUPPORT_REPEATER
	//*****Repeater service*****//
		me->pstRepeaterSrv = &l_Repeater;
		me->pstRepeaterSrv->superAO = &me->super;
		me->pstRepeaterSrv->stRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
		me->pstRepeaterSrv->stRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
		//me->pstRepeaterSrv->ptimeEvt_RepeaterPolling = &me->timeEvt_RepeaterPolling;
		RepeaterSrv_ctor(me->pstRepeaterSrv);/* repeater orthogonal component ctor */
	#endif

	//*****LED_Srv*****//

	if (me->pstLEDCfg->Cfg_Enable == 1)
	{
		InitLed();

		me->stLEDDisplay = &l_LEDDisplay;

		LEDDisplay_ctor(me->stLEDDisplay);
	}
}

/******************************************************************************************/
/**
 * \fn			QState DisplaySrv_initial(DisplayScanner * const me, QEvt const * const e)
 *
 * \brief		initial the display_scanner AO paramater
 *
 * \param		DisplayScanner * const me:(Input) the DisplayScanner handler here\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return		\e transform to DisplayScanner_IR \n
 *
 *
 ******************************************************************************************/
QState DisplaySrv_initial(DisplaySrv *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */

	LOGD("DisplaySrv_initial; \n");

	QTimeEvt_armX(&me->timeEvtVFD, TICKS_PER_100MS, 0);

	//*****VFD_Srv*****//

	if ((me->pstVFDCfg->Cfg_Enable == 1) && (me->Vfd_GpioInit_Flag == 0))
	{
		QMSM_INIT(&me->stVfdDisplay->super, (QEvt *)0);/* take the top-most initial transition in the component */
	}
   #ifdef USERAPP_SUPPORT_REPEATER
    //*****RepeaterSrv*****//

    QMSM_INIT(&me->pstRepeaterSrv->super, (QEvt *)0);/* take the top-most initial transition in the component */

   #endif
	//*****KEY_Srv*****//
	QMSM_INIT(&me->stKeySrv->super, (QEvt *)0);/* take the top-most initial transition in the component */


	//*****LED_Srv*****//

	if (me->pstLEDCfg->Cfg_Enable == 1)
	{
		QMSM_INIT(&me->stLEDDisplay->super, (QEvt *)0);/* take the top-most initial transition in the component */
	}
	return Q_TRAN(&DisplaySrv_active);
}

/******************************************************************************************/
/**
 * \fn			void DisplaySrv_Start(void)
 *
 * \brief		initial the Display_scanner AO paramater
 *
 * \param
 *
 * \return
 *
 *
 ******************************************************************************************/
void DisplaySrv_Start(void)
{
	static QEvt const *pstDisplayScannerQueueSto[15];

	DisplaySrv_ctor();

	QACTIVE_START(AO_DisplaySrv,           /*!< AO to start */
				  TSK_DISPLAYSRV_PRIO, 			/*!< QP priority of the AO */
				  pstDisplayScannerQueueSto,      /*!< event queue storage */
				  Q_DIM(pstDisplayScannerQueueSto), /*!< queue length [events] */
				  TSK_DISPLAYSRV_NAME,             /*!< AO name */
				  TSK_DISPLAYSRV_STACK_DEPTH,       /*!< size of the stack [bytes] */
				  (QEvt *)0);             /*!< initialization event */
}
AO_INIT(DisplaySrv_Start);


/******************************************************************************************/
/**
 * \fn			QState DisplaySrv_active(DisplayScanner * const me, QEvt const * const e)
 *
 * \brief	    Display scanner state machine
 *
 * \param		DisplayScanner * const me(Input): the DisplayScanner handler here\n
 *              QEvt const * const e :event structure of signal\n
 *
 * \return		\e status : Q_HANDLED() in DisplayScanner_active state \n
 *			    \e status : Q_SUPER() trans to father state \n
 *
 ******************************************************************************************/
QState DisplaySrv_active(DisplaySrv *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("DisplaySrv_active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("DisplaySrv_active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("DisplaySrv_active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_TICK_SIG:
		{
			LOGDT("DisplaySrv_active:DISPLAY_TICK_SIG; \n");
			QTimeEvt_armX(&me->timeEvtVFD, TICKS_PER_100MS, 0);

			//*****send VFD counter signal*****//

			if ((me->pstVFDCfg->Cfg_Enable == 1) && (me->Vfd_GpioInit_Flag == 0))
			{
				if (((me->stVfdDisplay->show_type) > eVfd_playtime) && ((me->stVfdDisplay->show_type) < eVfd_max))
				{
					QACTIVE_POST(&me->super, Q_NEW(QEvt, DISPLAY_VFD_COUNTER_SIG), (void *)0);
				}
			}

			//*****send Key counter signal*****//

			QACTIVE_POST(&me->super, Q_NEW(QEvt, DISPLAY_GET_KEY_SIG), (void *)0);


			//*****send LED counter signal*****//

			if (me->pstLEDCfg->Cfg_Enable == 1)
			{
				if (((me->stLEDDisplay->show_type) > eLED_null) && ((me->stLEDDisplay->show_type) < eLED_max))
				{
					QACTIVE_POST(&me->super, Q_NEW(QEvt, DISPLAY_LED_COUNTER_SIG), (void *)0);
				}
			}
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_VFD_SHOWDARK_SIG:
		case DISPLAY_VFD_Play_Time_SIG:
		case DISPLAY_VFD_COUNTER_SIG:
		case DISPLAY_VFD_POP_SIG:
		case DISPLAY_VFD_DISPLAY_SIG:
		case DISPLAY_VFD_ALIGN_SIG:
		case DISPLAY_VFD_CONST_SIG:
		case DISPLAY_VFD_SCOLL_SIG:
		case DISPLAY_VFD_BLINK_SIG:
	case DISPLAY_VFD_SCOLLONE_CONST_SIG:
	case DISPLAY_VFD_BLINKONE_CONST_SIG:
		case DISPLAY_VFD_DOT_SIG:
		{

			LOGDT("get VFD Signal...DisplaySrv_active \n");
			if ((me->pstVFDCfg->Cfg_Enable == 1) && (me->Vfd_GpioInit_Flag == 0))
			{
				QMSM_DISPATCH((QMsm *)me->stVfdDisplay, e); //tingwei
			}
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_GET_KEY_SIG:
		{

			LOGDT("get Display Key...DisplaySrv_active \n");

			QMSM_DISPATCH((QMsm *)me->stKeySrv, e); //tingwei

			status = Q_HANDLED();
			break;
		}
		case DISPLAY_LED_DISPLAY_SIG:
		case DISPLAY_LED_CONST_SIG:
		case DISPLAY_LED_BLINK_SIG:
		case DISPLAY_LED_COUNTER_SIG:
		case DISPLAY_LED_SHOWDARK_SIG:
		{

			LOGDT("get LED Signal...DisplaySrv_active \n");
			if (me->pstLEDCfg->Cfg_Enable == 1)
			{
				QMSM_DISPATCH((QMsm *)me->stLEDDisplay, e); //tingwei
			}
			status = Q_HANDLED();
			break;
		}
#ifdef USERAPP_SUPPORT_REPEATER

		case REPEATER_EX_INTR_0_SIG:
        case REPEATER_SRC_SWITCH_SIG:
        case REPEATER_POWER_UP_SIG:
        case REPEATER_POWER_DOWN_SIG:
		//case REPEATER_ECO_POWER_DOWN_SIG:
        case REPEATER_VOL_SET_SIG:
        case REPEATER_MUTE_SET_SIG:
        case REPEATER_REG_PRINTF_SIG:
        case REPEATER_AUD_INFO_PRINTF_SIG:
        case REPEATER_AUD_INFO_SEND_REQ_SIG:
		case REPEATER_DSPSourceID_SIG:
		case REPEATER_eARCSource_SIG:
        case REPEATER_I2C_Ready_Polling_SIG:
		case REPEATER_MUTE_SYN_SIG:
		case REPEATER_EP_UPGRADE_SIG:
		//case REPEATER_CEC_MUTE_SIG:
	    //case REPEATER_VOL_CEC_MUTE_SIG:
		//case REPEATER_CEC_POWER_SIG:
		//case REPEATER_CEC_FAKE_POWER_ON_SIG:
		//case REPEATER_CEC_VOL_UP_SIG:
		//case REPEATER_CEC_VOL_DOWN_SIG:
        {
            //LOGD("Dispatch to repeater service\n");

            QMSM_DISPATCH((QMsm *)me->pstRepeaterSrv, e);

            status = Q_HANDLED();
            break;
        }
#endif
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}
#endif

void Display_VFD(QActive *super)
{
	VFD_Service_Display(super, "EPUP");
	VFD_Service_Align(super, 2);
	VFD_Service_Constant(super);

}


int vfd_is_scroll(void)
{
	if(l_VfdDisplay.show_type == eVfd_scrollone_const || l_VfdDisplay.show_type == eVfd_scroll)
		return 1;

	return 0;
}

