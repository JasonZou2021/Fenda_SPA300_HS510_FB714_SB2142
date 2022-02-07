#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "qpc.h"
#include "cfg_sdk.h"
#include "types.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[TAPP]"
#include "log_utils.h"
#include "touch_app.h"
#include "initcall.h"
#include "task_attribute_user.h"
#include "custom_sig.h"
#include "hdmi_app.h"
#include "iop_scanner.h"


#define TICKS_PER_20MS  2

TOUCH_FUNC *_touch = &ET8L10C_touch;

int TouchKey_Init(void)
{
	int status = NORMAL;

	TOUCH_FUNC *func = _touch;

	if(func && func->init)
		func->init(status);

	return 0;
}

int TouchKey_StandbyInit(void)
{
	int status = STANDBY;

	TOUCH_FUNC *func = _touch;

	if(func && func->standby)
		func->standby(status);

	return 0;
}


int TouchKey_Polling(void)
{
	int ret = 0;

	TOUCH_FUNC *func = _touch;

	if(func && func->polling)
		ret = func->polling();

	return ret;

}


int TouchKey_GetVersion(void)
{
	int ret = 0;

	TOUCH_FUNC *func = _touch;

	if(func && func->version)
		ret = func->version(0);

	return ret;
}






/******************************************************************************************/
/**
 * \fn			QState MCU_IF_Active(stMCU_IF_t * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param		stMCU_IF_t * const me:(Input) the MCU_IF handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

static TouchSrv l_TouchSrv;
QActive *const AO_TouchSrv = &l_TouchSrv.super;


QActive *TouchApp_get(void)
{
	return AO_TouchSrv;
}

int Hdmi_SetInitSig(QActive *send, int src, int vol, int volmax)
{
	
	HdmiCtlEvt *evt = Q_NEW(HdmiCtlEvt, HDMI_INIT_SIG);
	evt->para1[0]= src;
	evt->para1[1]= vol;
	evt->para1[2]= volmax;
	QACTIVE_POST(AO_TouchSrv,  (QEvt *)evt,  send);
	
	return 0;
}


int Hdmi_SetHandleSig(QActive *send, int cmd, int para1, int para2)
{

	HdmiCtlEvt *evt = Q_NEW(HdmiCtlEvt, HDMI_HANDLE_SIG);
	evt->para1[0]= cmd;
	evt->para1[1]= para1;
	evt->para1[2]= para2;
	QACTIVE_POST(AO_TouchSrv,  (QEvt *)evt,  send);
	
	return 0;
}


int Hdmi_SetUpgSig(QActive *send)
{

	QACTIVE_POST(AO_TouchSrv,  Q_NEW(QEvt, HDMI_UPG_SET_SIG),  send);
	
	return 0;
}

void Set_device_id0(void);
QState TouchKey_Active(TouchSrv *const me, QEvt const *const e)
{
	QState status;
	
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("TouchKey ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("TouchKey EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("TouchKey INIT; \n");
			Set_device_id0();
			status = Q_HANDLED();
			break;
		}

		case HDMI_INIT_SIG:{
			HdmiCtlEvt *evt = (HdmiCtlEvt *)e;

			//LOGD(" >>>>>>>> HDMI_INIT_SIG\n");

			hdmi_cfg(HDMI_SYS_POWER_ON, evt->para1[0],  evt->para1[1], evt->para1[2]);
			hdmi_handle(CMD_POWER_ON, evt->para1[0], 0);

			QTimeEvt_armX(&me->timeEvtTouch, TICKS_PER_100MS, 0);
			//if(evt->para1[0] == CMD_MODE_BT)
			//	hdmi_handle(evt->para1[0], 0, 0);
			
			status = Q_HANDLED();
			break;
		}

		case HDMI_HANDLE_SIG:{
			HdmiCtlEvt *evt = (HdmiCtlEvt *)e;

			//LOGD(" >>>>>>>> HDMI_HANDLE_SIG\n");
			
			hdmi_handle(evt->para1[0], evt->para1[1], evt->para1[2]);
			
			status = Q_HANDLED();
			break;
		}

		case HDMI_UPG_SET_SIG:
			QTimeEvt_disarmX(&me->timeEvtTouch);
			hdmi_upg();
			status = Q_HANDLED();
			break;

		case HDMI_SEND_VENDOR_CMD_SIG:{
			LOGD("HDMI_VENDOR_CMD_SIG \n");
			stUserCECEvt_t *evt = (stUserCECEvt_t *)e;
			hdmi_VendorSend(evt);
			status = Q_HANDLED();
			break;
		}
		
		case TOUCH_TIME_SIG:
			QTimeEvt_armX(&me->timeEvtTouch, TICKS_PER_100MS, 0);

			hdmi_get_status(HDMI_SYS_POWER_ON);

			TouchKey_Polling();
			
			status = Q_HANDLED();
			break;
	
		default:
			status = Q_SUPER(&QHsm_top);
			break;
		
	}

	return status;
}

QState TouchKey_initial(TouchSrv *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */

	TouchKey_Init();

	

	return Q_TRAN(&TouchKey_Active);
}



/******************************************************************************************/
/**
 * \fn			void MCU_IF_ctor(LEDDisplay * const me)
 *
 * \brief		initial the MCU_IF AO paramater
 *
 * \param		LEDDisplay * const me
 *
 * \return
 *
 *
 ******************************************************************************************/

void TouchKey_ctor(void)
{
	//ap_printf("TouchKey_ctor ENTRY; \n");

	TouchSrv *const me = &l_TouchSrv;
	
	QTimeEvt_ctorX(&me->timeEvtTouch, &me->super, TOUCH_TIME_SIG, 0U);
	
	QActive_ctor(&me->super, Q_STATE_CAST(&TouchKey_initial));
}


void TouchKey_Start(void)
{
	static QEvt const *pstTouchQueueSto[20];

	TouchKey_ctor();

	QACTIVE_START(AO_TouchSrv,           /*!< AO to start */
				  TSK_TOUCH_PRIO, 			/*!< QP priority of the AO */
				  pstTouchQueueSto,      /*!< event queue storage */
				  Q_DIM(pstTouchQueueSto), /*!< queue length [events] */
				  TSK_TOUCH_NAME,             /*!< AO name */
				  TSK_TOUCH_STACK_DEPTH,       /*!< size of the stack [bytes] */
				  (QEvt *)0);             /*!< initialization event */
}

AO_INIT(TouchKey_Start);


