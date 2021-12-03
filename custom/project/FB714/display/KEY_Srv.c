/******************************************************************************/
/**
*
* \file	 KEY_Srv.c
*
* \brief	This file provides the key framework in Display \n
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
#define LOG_MODULE_ID ID_VFD
#include "log_utils.h"
#include "KEY_Srv.h"
#include "custom_sig.h"
#include "KEY_gpio.h"

Q_DEFINE_THIS_FILE

#if 1

stKeyInfo_t stDisplayKeyData;
hCircBuf_t hDisplayKeyBuf;

QState Key_initial(KeySrv *const me, QEvt const *const e);
QState Key_active(KeySrv *const me, QEvt const *const e);


/******************************************************************************************/
/**
 * \fn			int ConvertDisplay_VFDKeyCode(UINT32* pVFD_ValTabPtr, int VFD_KeyNum, const UINT32 dVFD_val)
 *
 * \brief		Convert VFD Key Code
 *
 * \param		UINT32* pVFD_ValTabPtr(Input): vfd key value pointer\n
 *              int VFD_KeyNum(Input):vfd key number \n
 *              const UINT32 dVFD_val(Input): vfd key value\n
 *
 * \return		\e other on success. \n
 *			    \e -1 on error. \n
 *
 ******************************************************************************************/
int ConvertDisplay_VFDKeyCode(UINT32 *pVFD_ValTabPtr, int VFD_KeyNum, const UINT32 dVFD_val)
{
	LOGDT("dVFD_val = 0x%x\n", dVFD_val);

	int cnt = 0;

	for (cnt = 0; cnt < VFD_KeyNum; cnt++)
	{
		if (pVFD_ValTabPtr[cnt] == dVFD_val)
			break;
	}

	if (cnt >= VFD_KeyNum)
		cnt = -1;

	return (cnt);
}


/******************************************************************************************/
/**
 * \fn			void Key_ctor(VfdDisplay * const me)
 *
 * \brief		initial the key AO paramater
 *
 * \param		KeySrv * const me
 *
 * \return
 *
 *
 ******************************************************************************************/

void Key_ctor(KeySrv *const me)
{
	LOGD("Key_ctor ENTRY; \n");
	QHsm_ctor(&me->super, Q_STATE_CAST(&Key_initial));
}



/******************************************************************************************/
/**
 * \fn			QState Key_initial(KeySrv * const me, QEvt const * const e)
 *
 * \brief		initial the key AO paramater
 *
 * \param		KeySrv * const me , QEvt const * const e
 *
 * \return		\e transform to VfdDisplay_active \n
 *
 *
 ******************************************************************************************/

QState Key_initial(KeySrv *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */
	LOGD("Key_initial Entry; \n");

	me->pstKEY_GPIO_cfg = GET_DISPLAY_CFG(KEY_GPIO_CFG_T);

	if (me->pstKEY_GPIO_cfg->KeyGpio_Enable)
		InitKeyGpioTbl(me->pstKEY_GPIO_cfg->pKeyGpio_Tbl, me->pstKEY_GPIO_cfg->KeyGpio_Num);

	me->stDisplayKeyData = &stDisplayKeyData;

	me->hKeyBuf = &circBuf;

	me->hKeyBuf->maxLen = KEY_BUF_SIZE;

	me->iDisplayTickCnt = 0;

	me->pstKEY_cfg = GET_SDK_CFG(CFG_KEY_T);

	me->pstVFD_cfg = GET_DISPLAY_CFG(VFD_CFG_T);

	return Q_TRAN(&Key_active);
}

/******************************************************************************************/
/**
 * \fn			QState Key_active(KeySrv * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param		KeySrv * const me:(Input) the key handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

QState Key_active(KeySrv *const me, QEvt const *const e)
{
	QState status;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("Key_active ENTRY; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("Key_active EXIT; \n");
			status = Q_HANDLED();
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("Key_active INIT; \n");
			status = Q_HANDLED();
			break;
		}
		case DISPLAY_GET_KEY_SIG:
		{
			LOGDT("Key_active :DISPLAY_GET_KEY_SIG; \n");
			int result = 0;

			int iVFD_Key = -1;
			int iGPIO_Key = -1;
			int iKeySrc = -1;
			int iKeyCode = -1;

			if (me->pstVFD_cfg->Cfg_Enable == 1) // if VFD is enable
			{
				iVFD_Key = ConvertDisplay_VFDKeyCode(me->pstVFD_cfg->pVFD_val, me->pstVFD_cfg->VFD_KeyNum, Vfd_ReadVFDKeys());
			}

			if (me->pstKEY_GPIO_cfg->KeyGpio_Enable) // if gpio key is enable
			{
				iGPIO_Key = ReadKeyGpioTbl(me->pstKEY_GPIO_cfg->pKeyGpio_Tbl, me->pstKEY_GPIO_cfg->KeyGpio_Num, me->pstKEY_GPIO_cfg->KeyGpio_ActiveLv);
			}

			if (iVFD_Key != -1) // get a VFD key
			{
				//LOGD("iVFD_Key != -1 in!!!!!! \n");
				iKeyCode = iVFD_Key;
				iKeySrc = eKeySrc_VFD_Key;
			}
			else if (iGPIO_Key != -1) // get a gpio key
			{
				iKeyCode = iGPIO_Key;
				iKeySrc = eKeySrc_GPIO_Key;
			}

			if ((me->pstVFD_cfg->Cfg_Enable == 1) || (me->pstKEY_GPIO_cfg->KeyGpio_Enable == 1)) // if VFD or gpio key is enable
			{
				result = KeyProcess(iKeyCode, iKeySrc, me->stDisplayKeyData,  me->pstKEY_cfg);
			}

			if (result == 1)
			{
				//LOGD("KeyProcess result == 1 in!!!!!! \n");
				result = PushKeyInfo(me->hKeyBuf, me->stDisplayKeyData);
			}

			status = Q_HANDLED();
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

#endif



