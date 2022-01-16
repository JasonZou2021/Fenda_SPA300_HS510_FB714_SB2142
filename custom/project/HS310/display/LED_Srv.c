/******************************************************************************/
/**
*
* \file	 LED_Srv.c
*
* \brief	This file provides the leddrv middleware state machine framework \n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/08/8
******************************************************************************/
#include <string.h>
#include "qpc.h"
#include "log_utils.h"
#include "LED_Srv.h"
#include "LED_Drv.h"

#include "custom_sig.h"

Q_DEFINE_THIS_FILE

stledinfo_t stLedShowInfo ;

/* hierarchical state machine ... */
QState LEDDisplay_initial(LEDDisplay *const me, QEvt const *const e);
QState LEDDisplay_Active(LEDDisplay *const me, QEvt const *const e);
QState LEDDisplay_Const(LEDDisplay *const me, QEvt const *const e);
QState LEDDisplay_Blink(LEDDisplay *const me, QEvt const *const e);




/******************************************************************************************/
/**
 * \fn			void LEDDisplay_ctor(LEDDisplay * const me)
 *
 * \brief		initial the LED AO paramater
 *
 * \param		LEDDisplay * const me
 *
 * \return
 *
 *
 ******************************************************************************************/

void LEDDisplay_ctor(LEDDisplay *const me)
{
	leddrv_printf1("LEDDisplay_ctor ENTRY; \n");

	QHsm_ctor(&me->super, Q_STATE_CAST(&LEDDisplay_initial));
}



/******************************************************************************************/
/**
 * \fn			QState LEDDisplay_initial(LEDDisplay * const me, QEvt const * const e)
 *
 * \brief		initial the LED AO paramater
 *
 * \param		LEDDisplay * const me , QEvt const * const e
 *
 * \return		\e transform to LEDDisplay_active \n
 *
 *
 ******************************************************************************************/

QState LEDDisplay_initial(LEDDisplay *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */

	me->stLedShowInfo = &stLedShowInfo;
	me->show_type = eLED_null;
	return Q_TRAN(&LEDDisplay_Active);
}

/******************************************************************************************/
/**
 * \fn			void LED_Set_Control_ID(LEDDisplay * const me, LED_SHOW_ID Show_id)
 *
 * \brief		set led control ID
 *
 * \param		LEDDisplay * const me , QEvt const * const e
 *
 * \return		\e transform to LEDDisplay_active \n
 *
 *
 ******************************************************************************************/

void LED_Set_Control_ID(LEDDisplay *const me, LED_SHOW_ID Show_id)
{
	leddrv_printf2("LED_Display: Show_id = %d\n", Show_id);
	switch (Show_id)
	{
#ifdef Q70_DEMO
		case LED_SHOW_USB:
			me->stLedShowInfo->led_id = Led_0;
			break;
		case LED_SHOW_OPTICAL:
			me->stLedShowInfo->led_id = Led_3;
			break;
		case LED_SHOW_ARC:
		case LED_SHOW_EARC:
			me->stLedShowInfo->led_id = Led_4;
			break;
		case LED_SHOW_HDMI0:
		case LED_SHOW_HDMI1:
		case LED_SHOW_HDMI2:
			me->stLedShowInfo->led_id = Led_2;
			break;
		case LED_SHOW_BT:
			me->stLedShowInfo->led_id = Led_1;
			break;
#else
		case LED_SHOW_LINEIN:
			me->stLedShowInfo->led_id = Led_0;			//led0
			break;
		case LED_SHOW_USB:
			me->stLedShowInfo->led_id = Led_1;			//led1
			break;
		case LED_SHOW_SPDIFIN3:
			me->stLedShowInfo->led_id = Led_2;			//led2
			break;
		case LED_SHOW_SPDIFIN2:
			me->stLedShowInfo->led_id = Led_3;			//led3
			break;
		//case LED_SHOW_WIFI:
		//	me->stLedShowInfo->led_id = Led_4;			//led4
			break;
		case LED_SHOW_BT:
			me->stLedShowInfo->led_id = Led_5;			//led5
			break;
		//case LED_SHOW_WSS_BT:
		//	me->stLedShowInfo->led_id = Led_6;			//led6
			break;
		case LED_SHOW_ARC:
			me->stLedShowInfo->led_id = (Led_1 | Led_0); //(Led_5|Led_4|Led_3|Led_2|Led_1|Led_0);
			break;
		//case LED_SHOW_AirPlay:
		//	me->stLedShowInfo->led_id = (Led_4 | Led_0);
			break;
		case LED_SHOW_DLNA:
			me->stLedShowInfo->led_id = (Led_5 | Led_4);
			break;
#endif
		default:
			Set_AllLED_OFF();
			break;
	}
	return;
}


/******************************************************************************************/
/**
 * \fn			void LED_Blink(LEDDisplay * const me,uint16_t bright_time,uint16_t dark_time)
 *
 * \brief		set led show in blink mode
 *
 * \param		LEDDisplay * const me , QEvt const * const e
 *
 * \return		\e transform to LEDDisplay_active \n
 *
 *
 ******************************************************************************************/

void LED_Blink(LEDDisplay *const me, uint16_t bright_time, uint16_t dark_time)
{

	me->stLedShowInfo->bright_time = bright_time;

	me->stLedShowInfo->dark_time = dark_time;

	leddrv_printf1("LED_Blink:bright_time = %d,dark_time = %d\n", me->stLedShowInfo->bright_time, me->stLedShowInfo->dark_time);

	Set_AllLED_OFF();

	return;
}


/******************************************************************************************/
/**
 * \fn			void LightUp_Led(UINT8 led_id)
 *
 * \brief		light up led
 *
 * \param		which led \n
 *
 * \return		\e none \n

 ******************************************************************************************/

void LightUp_Led(UINT16 led_id)
{
	leddrv_printf2("LightUp_Led :led_id = 0x%x\n", led_id);

	Set_AllLED_OFF();

	Set_Led_Light(led_id);
}


/******************************************************************************************/
/**
 * \fn			void LightOff_Led(UINT8 led_id)
 *
 * \brief		light up led
 *
 * \param		which led \n
 *
 * \return		\e none \n

 ******************************************************************************************/

void LightOff_Led(UINT16 led_id)
{
	leddrv_printf2("LightOff_Led :led_id = 0x%x\n", led_id);

	Set_Led_Off(led_id);
}


/******************************************************************************************/
/**
 * \fn			QState LEDDisplay_active(LEDDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param		LEDDisplay * const me:(Input) the LED handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

QState LEDDisplay_Active(LEDDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			leddrv_printf1("LedDisplay_active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			leddrv_printf1("LedDisplay_active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			leddrv_printf1("LedDisplay_active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_LED_DISPLAY_SIG:
		{
			leddrv_printf2("DISPLAY_LED_DISPLAY_SIG ; \n");
			LEDDisplayEvt *pLEDDisplayEvt = ((LEDDisplayEvt *)e) ;
			LED_Set_Control_ID(me, pLEDDisplayEvt->show_id);
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_LED_CONST_SIG:
		{
			leddrv_printf2("DISPLAY_LED_CONST_SIG ; \n");
			status = Q_TRAN(&LEDDisplay_Const);
			break;
		}
		case DISPLAY_LED_BLINK_SIG:
		{
			leddrv_printf2("DISPLAY_LED_BLINK_SIG ; \n");
			LEDBlinkEvt *pLEDBlinkEvt = ((LEDBlinkEvt *)e) ;
			LED_Blink(me, pLEDBlinkEvt->bright_time, pLEDBlinkEvt->dark_time);
			status = Q_TRAN(&LEDDisplay_Blink);
			break;
		}
		case DISPLAY_LED_SHOWDARK_SIG:
		{
			leddrv_printf2("DISPLAY_LED_SHOWDARK_SIG ; \n");
			LightOff_Led(me->stLedShowInfo->led_id);
			status = Q_HANDLED();
			break;
		}
		default:
			status = Q_SUPER(&QHsm_top);
			break;

	}
	return status;
}



/******************************************************************************************/
/**
 * \fn			QState LEDDisplay_const(LEDDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which LED segment display show constant style
 *
 * \param		LEDDisplay * const me:(Input) the LED handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in const state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/
QState LEDDisplay_Const(LEDDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			leddrv_printf2("LedDisplay_const ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			leddrv_printf2("LedDisplay_const EXIT; \n");
			me->LED_history = Q_STATE_CAST(&LEDDisplay_Const);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			leddrv_printf2("LedDisplay_const INIT; \n");
			me->show_type = eLED_constant;
			LightUp_Led(me->stLedShowInfo->led_id);
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&LEDDisplay_Active);
			break;
		}
	}
	return status;
}

/******************************************************************************************/
/**
 * \fn			QState LEDDisplay_blink(LEDDisplay * const me, QEvt const * const e)
 *
 * \brief		the state which LED segment display show blink style
 *
 * \param		LEDDisplay * const me:(Input) the LED handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in blink state \n
 *			    \e status : Q_SUPER() trans to active state \n
 *
 ******************************************************************************************/

QState LEDDisplay_Blink(LEDDisplay *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			leddrv_printf2("LedDisplay_blink ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			leddrv_printf2("LedDisplay_blink EXIT; \n");
			me->LED_history = Q_STATE_CAST(&LEDDisplay_Blink);
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			leddrv_printf2("LedDisplay_blink INIT; \n");
			me->show_type = eLED_blink;
			me->count = 0;
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_LED_COUNTER_SIG:
		{
			leddrv_printf2("DISPLAY_LED_COUNTER_SIG; \n");
			if ((me->count) == 0)
			{
				leddrv_printf2("LED Bright start\n");
				LightUp_Led(me->stLedShowInfo->led_id);
			}
			else if (((me->count) == (me->stLedShowInfo->bright_time / TICKS_PER_100MS)))
			{
				leddrv_printf2("LED OFF start\n");
				LightOff_Led(me->stLedShowInfo->led_id);
			}
			me->count++;

			if (((me->count) > ((me->stLedShowInfo->bright_time + me->stLedShowInfo->dark_time) / TICKS_PER_100MS)))
			{
				leddrv_printf2("me->count:%d\n", me->count);
				me->count = 0;
			}
			status = Q_HANDLED();
			break;
		}
		default:
		{
			status = Q_SUPER(&LEDDisplay_Active);
			break;
		}
	}
	return status;
}

