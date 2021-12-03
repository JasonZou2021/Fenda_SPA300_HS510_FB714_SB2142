/******************************************************************************/
/**
*
* \file	powerservice.h
*
* \brief	powerservice for power down/up service.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   tp.jiang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/06/28
******************************************************************************/

#ifndef POWERSERVICE_H
#define POWERSERVICE_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

#include "qp_pub.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/

typedef int32_t (*PwrSrvChk_CB_t)(uint8_t src);

typedef int (*PwrSrv_DisableDevicePwr_CB_t)(void);


typedef struct _PowerServiceEvt
{
	/* protected: */
	QEvt stEvt;

	/* public: */
	QActive   *REGISTER_AO;
} PowerServiceEvt;


typedef struct
{
	QHsm super;
	QTimeEvt stPowerServiceTimeEvt;
} POWERSERVICE;

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

/******************************************************************************************/
/**
 * \fn		void PowerService_Ctor(POWERSERVICE * const me)
 *
 * \brief		PowerService constructor
 *
 * \param	 	me : (Input) the PowerService hierarchical state machine
 *
 * \return		none.
 *
 * \note		do HSM constructor here.
 *
 ******************************************************************************************/

void PowerService_Ctor(POWERSERVICE *const me);

/******************************************************************************************/
/**
 * \fn		int PowerService_RegPowerOffLSR(QActive * const me, ARFunc_t pfFunc)
 *
 * \brief		regist SYSTEM_START_POWER_OFF_SIG & pfFunc to AO, AO will run pfFunc if
 *				receive SYSTEM_START_POWER_OFF_SIG,
 *
 * \param 		me : (Input) the AO to regist, it's register AO's handler here
 * \param 		pfFunc : (Input) the listener function
 *
 * \return		\e ERROR on fail. \n
 *            	\e SUCCESS on success \n
 *
 * \note		example for listener.
 *
 * \sa
 *
 ******************************************************************************************/

int PowerService_RegPowerOffLSR(QActive *const me, ARFunc_t pfFunc);

/******************************************************************************************/
/**
 * \fn		void PowerService_FinishPowerOff(QActive * const me)
 *
 * \brief		when AO have been handle power off event,it will run this function,
 *				just to post SYSTEM_START_POWER_OFF_SIG to systemservice,which tell systemservice
 *				into power down.
 *
 * \param 		me : (Input) the AO, it's register AO's handler here
 *
 *
 * \note		example for listener.
 *
 * \sa
 *
 ******************************************************************************************/

void PowerService_FinishPowerOff(QActive *const me);

void PowerService_SysPowerOff(QActive *const me);

void PowerService_SysUpgrade(QActive *const me);

/******************************************************************************************/
/**
 * \fn		int32_t PowerService_RegPwrUpChkFun(PwrSrvChk_CB_t CB_fun)
 *
 * \brief		register a callback function to add a power on condition in standby mode
 *
 * \param 		CB_fun : callback function, it will be called in standby mode.
 *
 * \note		if CB_fun return a value > 0, system will be woken up
 *
 * \sa
 *
 ******************************************************************************************/
int32_t PowerService_RegPwrUpChkFun(PwrSrvChk_CB_t CB_fun);

/******************************************************************************************/
/**
 * \fn		void PowerService_Reset(QActive * const me)
 *
 * \brief		restart system directly after shutdown
 *
 * \param 		 * \param 		me : (Input) caller AO
 *
 * \note		this API will not shut down the system directly, still need to call PowerService_SysPowerOff() to shut down the system
 *
 * \sa
 *
 ******************************************************************************************/
void PowerService_Reset(QActive *const me);

/******************************************************************************************/
/**
 * \fn		void PowerService_RegDisableDevicePwrFun(PwrSrvChk_CB_t CB_fun)
 *
 * \brief		register a callback function to disable 5V & 3.3V device power
 *
 * \param 		CB_fun : callback function, it will be called in standby mode.
 *
 * \note		if CB_fun return a value > 0, system will be woken up
 *
 * \sa
 *
 ******************************************************************************************/
int PowerService_RegDisableDevicePwrFun(PwrSrv_DisableDevicePwr_CB_t CB_fun);

/******************************************************************************************/
/**
 * \fn		void PowerService_GetWakeupInfo(void)
 *
 * \brief		provide system wakeup info
 *
 * \param 		void
 *
 * \note		to get system wakeup info, refer to "eWakeupInfo_t"
 *
 * \return		system wakeup info, refer to "eWakeupInfo_t"
 *
 * \sa
 *
 ******************************************************************************************/
int PowerService_GetWakeupInfo(void);

#endif

