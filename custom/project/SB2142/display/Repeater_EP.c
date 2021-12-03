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
#define LOG_MODULE_ID ID_CEC
#include "log_utils.h"
#include "Repeater_Srv.h"
#include "custom_sig.h"
#include "Display_Srv.h"
#include "gpio_intr.h"
#include "hw_gpio_list.h"
#include "pinmux.h"
#include "datarammap.h"
#include "interrupt_interfaces.h"
#include "cfg_sdk.h"
#include "i2c_api.h"
#include "iop_user.h"
#include "user_def.h"
#include "AudDspService.h"
#include "exstd_interfaces.h"

//#include "flash.h"

Q_DEFINE_THIS_FILE;

//#define SIZE_OF_0x09_TO_0x13 (4)
#define VOLUME_TIMEOUT (100)

QState RepeaterSrv_initial(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_Home(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_inactive(RepeaterSrv_t * const me, QEvt const * const e);
static UINT32 setChannelMapping(unEP_AudInfo_reg0x2C_t inChMap);

int RepeaterSrv_WakeUpCheck(UINT8* HPD_State,UINT8* Repeater_State);
void RepeaterSrv_Intr_Callback_func0(uint32_t arg);

extern unsigned int GetMSFromSTC(void);
extern int UserSetting_ReadSrc(UINT16 *pSrc);
extern int First_Power_Up(void);
extern int GetPartitionInfo(char *part_name, int *part_addr, int *part_size, int *file_size);
extern void Display_VFD(QActive *super);


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

void RepeaterSrv_ctor(RepeaterSrv_t * const me) {
    LOGD("RepeaterSrv_ctor ENTRY; \n");
	QHsm_ctor(&me->super, Q_STATE_CAST(&RepeaterSrv_initial));
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

QState RepeaterSrv_initial(RepeaterSrv_t *const me, QEvt const *const e)
{
    (void)e;/* avoid compiler warning about unused parameter */
    LOGD("RepeaterSrv_initial Entry; \n");
	me->First_PowerUp_Flag = First_Power_Up();
   // Power_Repeater_EP_Init(RepeaterSrv_EP_Init);
    RepeaterSrv_EP_RegCB();
    HDMI_I2C_errcheck_RegCB(RepeaterSrv_EP_ErroCheck);

    HDMI_Repeat_init(&me->stRepeatHandler);
	me->EP_check_Counter = 0;
    me->TV_Switch_Source = 0;
    me->Repeater_power_state = eRepeater_Power_on;
    me->Mute_state = eMute_off;
	me->eARC_state = eRepeater_ARCSRC;
	me->Audio_path = 0;
	me->AudSys_TV_control = 0;
	me->AudSys_User_control = 0;

    unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;

    HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
    //HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &stEP_GenCtrl_reg0x10.DataByte, 1);
    stEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];

    LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);

    if(stEP_GenCtrl_reg0x10.Power == 0) {
        stEP_GenCtrl_reg0x10.Power = 1;
        stEP_GenCtrl_reg0x10.ARC_EN = 0;

        stEP_GenCtrl_reg0x10.CEC_Mute = 0;
        // stEP_GenCtrl_reg0x10.Audio_Path = 0;
        HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
        LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);
    }
#if 1
    LOGD("first_PowerUp:- %d -\n",me->First_PowerUp_Flag);
    QTimeEvt_ctorX(&me->timeEvt_I2cPolling,DisplaySrv_get(),REPEATER_I2C_Ready_Polling_SIG,0U);

	if(me->First_PowerUp_Flag == 1)
	{
	     QTimeEvt_armX(&me->timeEvt_I2cPolling, TICKS_PER_100MS, 0);
	}
	else
	{
        /****************************** interrupt init*****************************/
        IOP_RegIntr0Callback(INTR0_EVT_GPIO25_TRIG, RepeaterSrv_Intr_Callback_func0, (uint32_t)me);// register cb function for repeater intr
		IOP_RepeaterIntrPollingStart();// iop polling gpio, and send intr
        /**************************************************************************/
	}

#endif
    unEP_BasicInfo_reg0x08_t unEP_BasicInfo_reg0x08;

    unEP_BasicInfo_reg0x08.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_INFO_0x08];

    me->HPD_state = unEP_BasicInfo_reg0x08.HPD;
    me->RepeaterSrc = eRepeater_Src_Other;
	me->stRepeatHandler.DSP_AUD_FIFO = Source_HDMISTD;
    return Q_TRAN(&RepeaterSrv_Home);
}

/******************************************************************************************/
/**
 * \fn			QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e)
 *
 * \brief		the state which transform to other sub_status by different SIG
 *
 * \param		RepeaterSrv_t * const me:(Input) the RepeaterSrv handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/
extern void TaskDelayMs(unsigned int sdMSec);

QState RepeaterSrv_Home(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGD("RepeaterSrv_home ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            LOGD("RepeaterSrv_home EXIT; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGD("RepeaterSrv_home INIT; \n");
			/*

			volatile int *EP_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
			volatile int *Sys_memory_src = (int * )SRAM_ADDR_SYS_MEMORY_SRC;

			LOGD("sys_memory_source is %d; \n",*Sys_memory_src);

			if((*EP_Wakeup_Flag == 1)||((*Sys_memory_src >= eAp_ARC_Src)&&(*Sys_memory_src <= eAp_HDMI2_Src)))
			{
				status = Q_HANDLED();
			}
			else*/
			{
				status = Q_TRAN(&RepeaterSrv_inactive);
			}


            break;
        }

		case REPEATER_I2C_Ready_Polling_SIG:{
				   LOGD("REPEATER_I2C_Ready_Polling_SIG,Counter:--%d--\n",me->EP_check_Counter);
				   if(me->EP_check_Counter < 5)
				   {
				      QTimeEvt_armX(&me->timeEvt_I2cPolling,TICKS_PER_100MS, 0);
				   }
				   else
                   {
                      LOGD("-------I2C_ERR Time Out!!!-------\n");
					  me->First_PowerUp_Flag = 0;
				   }
				   me->EP_check_Counter++;
				   unEP_BasicInfo_reg0x08_t unBasicInfo;
				   int res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unBasicInfo.DataByte , 1);
				   if(res ==1)
				   {
					LOGD("--------------I2C_READY-------first_Power: %d -----\n",me->First_PowerUp_Flag);

					   if(me->First_PowerUp_Flag == 1)
					   {

					    int Standby_Mode=GET_SDK_CFG(CFG_PMC_T)->bFstPwrOnFlag;
						if(Standby_Mode == 0)
						Standby_Mode = 1;
						else
						Standby_Mode = 0;
					    RepeaterSrv_EP_Reinit();
                        /****************************** interrupt init*****************************/
						#ifdef CFG_DISABLE_BOOT_STANDBY_MODE
                        IOP_RegIntr0Callback(INTR0_EVT_GPIO25_TRIG, RepeaterSrv_Intr_Callback_func0, (uint32_t)me);// register cb function for repeater intr
						IOP_RepeaterIntrPollingStart(); // iop polling gpio, and send intr
						LOGD("INTR_1\n");
						 #elif Standby_Mode
                        IOP_RegIntr0Callback(INTR0_EVT_GPIO25_TRIG, RepeaterSrv_Intr_Callback_func0, (uint32_t)me);// register cb function for repeater intr
						#ifdef USERAPP_EP_UPGRADE
							 HDMI_Upgrade_VER(&me->stRepeatHandler);
						#endif
						IOP_RegIntr0Callback(INTR0_EVT_GPIO25_TRIG, RepeaterSrv_Intr_Callback_func0, (uint32_t)me);// register cb function for repeater intr
						IOP_RepeaterIntrPollingStart(); // iop polling gpio, and send intr
						LOGD("INTR_2\n");
						#else
						(void)Standby_Mode;
						LOGD("INTR_3\n");
						#endif
                        /**************************************************************************/
					   me->First_PowerUp_Flag = 0;
					   }
					QTimeEvt_disarmX(&me->timeEvt_I2cPolling);
				   }
				 status = Q_HANDLED();
				 break;
			   }

		case REPEATER_EX_INTR_0_SIG: {
            //int cnt = 0;
            //UINT8 pBuf[10];
             me->EX_INTR_New= GetMSFromSTC();
            int res;

            LOGD("REPEATER_EX_INTR_0_SIG\n");

            unEP_BasicInfo_reg0x08_t unBasicInfo;
            res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unBasicInfo.DataByte , 1);
            LOGD("HPD : %d\n", unBasicInfo.HPD);
			if ((unBasicInfo.HPD == 0) && (me->HPD_state == 1))
			{ // update all register info if hdmi plug in
				//LOGD("============== unBasicInfo.HPD =============\n");
				RepeaterSrv_HPDStatus(unBasicInfo.HPD, unBasicInfo.ARC_ON);
			}
			if ((unBasicInfo.HPD == 1) && (me->HPD_state == 0))
            {
				TaskDelayMs(300); // Waiting for eARC  connection

				// update all register info if hdmi plug in
                HDMI_Repeat_init(&me->stRepeatHandler);
				RepeaterSrv_HPDStatus(unBasicInfo.HPD, unBasicInfo.ARC_ON);
                LOGD("PhyAddr : 0x%x\n", (me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_INFO_PHY_ADDR_H] << 8) | (me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_INFO_PHY_ADDR_L]));

				unBasicInfo.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_INFO_0x08];
				//res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unBasicInfo.DataByte , 1);

				if(unBasicInfo.eARC_ON == 1)
				{
					LOGD("eARC Enable !!!\n");

					unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
					unEP_GenCtrl_reg0x11_t stEP_GenCtrl_reg0x11;
    				stEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
					stEP_GenCtrl_reg0x11.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x11];

					if( stEP_GenCtrl_reg0x11.DataByte == ePrimary_Sel_None_ARC)
					{
						stEP_GenCtrl_reg0x10.eARC_EN = 1;
						HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
	               		LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);
					}

				}
            }
#ifndef CFG_DISABLE_BOOT_STANDBY_MODE
			//if(unBasicInfo.HPD == 1)
#else
			if(unBasicInfo.HPD == 1)
#endif
			{
				// BYTE RegBuf[SIZE_OF_0x09_TO_0x13];
				 unEP_GenCtrl_reg0x10_t stEP_BasicInfo_reg0x10_pre;
				 stEP_BasicInfo_reg0x10_pre.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
				 unEP_BasicInfo_reg0x13_t stEP_BasicInfo_reg0x13_pre;
				 stEP_BasicInfo_reg0x13_pre.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_CEC_VOLUME];

				// HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &RegBuf, SIZE_OF_0x09_TO_0x13);


                unEP_BasicInfo_reg0x09_t stEP_BasicInfo_reg0x9;
                 HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x09, &stEP_BasicInfo_reg0x9.DataByte, 1);
               // LOGD("CEC_ECF: %d\n",stEP_BasicInfo_reg0x9.CEC_ECF);

				unEP_GenCtrl_reg0x10_t stEP_BasicInfo_reg0x10;
			   HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);
               // stEP_BasicInfo_reg0x10.DataByte = RegBuf[EP_ADDR_GEN_CTRL_0x10 - EP_ADDR_GEN_CTRL_0x10];
                //LOGD("stEP_BasicInfo_reg0x10:0x%x\n",stEP_BasicInfo_reg0x10.DataByte);

				unEP_GenCtrl_reg0x11_t stEP_BasicInfo_reg0x11;
				HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &stEP_BasicInfo_reg0x11.DataByte, 1);
				//stEP_BasicInfo_reg0x11.DataByte = RegBuf[EP_ADDR_GEN_CTRL_0x11 - EP_ADDR_GEN_CTRL_0x10];
				//LOGD("stEP_BasicInfo_reg0x11: 0x%x\n",stEP_BasicInfo_reg0x11.DataByte);

				unEP_GenCtrl_reg0x12_t stEP_BasicInfo_reg0x12;
				HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x12, &stEP_BasicInfo_reg0x12.DataByte, 1);
				//stEP_BasicInfo_reg0x12.DataByte = RegBuf[EP_ADDR_GEN_CTRL_0x12 - EP_ADDR_GEN_CTRL_0x10];
				//LOGD("stEP_BasicInfo_reg0x12:0x%x\n",stEP_BasicInfo_reg0x12.DataByte);

				unEP_BasicInfo_reg0x13_t stEP_BasicInfo_reg0x13_t;
				HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_CEC_VOLUME, &stEP_BasicInfo_reg0x13_t.DataByte, 1);
				//stEP_BasicInfo_reg0x13_t.DataByte = RegBuf[EP_ADDR_CEC_VOLUME - EP_ADDR_GEN_CTRL_0x10];
				//LOGD("stEP_BasicInfo_reg0x13:0x%x\n",stEP_BasicInfo_reg0x13_t.DataByte);

				LOGD("me->RepeaterSrc: %d\n",me->RepeaterSrc);
				if(stEP_BasicInfo_reg0x11.DataByte != me->RepeaterSrc )
				{

					if(stEP_BasicInfo_reg0x11.Primary_Sel== ePrimary_Sel_None_ARC)
					{

					    if(stEP_BasicInfo_reg0x11.DataByte != (me->RepeaterSrc+2))
					    {
					       me->TV_Switch_Source = 1;
					       LOGD("EP new Source to ARC Source! %d\n",stEP_BasicInfo_reg0x11.DataByte);
						   RepeaterSrv_UserSwitch(eRepeater_Src_ARC);
					    }
					}
					else if(stEP_BasicInfo_reg0x11.DataByte < eRepeater_Src_ARC)
					{

						me->TV_Switch_Source = 1;
						if(stEP_BasicInfo_reg0x10.ARC_EN == 1)
						{
							stEP_BasicInfo_reg0x10.ARC_EN = 0;
							HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);
					    }
						if(stEP_BasicInfo_reg0x10.eARC_EN == 1)
						{
							stEP_BasicInfo_reg0x10.eARC_EN = 0;
							HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);
						}
					    LOGD("EP new Source  port %d Source!\n",stEP_BasicInfo_reg0x11.DataByte);
						RepeaterSrv_UserSwitch(stEP_BasicInfo_reg0x11.DataByte);
					}
				}
				if(me->Audio_path != stEP_BasicInfo_reg0x10.Audio_Path)
				{
					if(!stEP_BasicInfo_reg0x10.Audio_Path)
					{
						me->AudSys_TV_control = 1;
						//LOGD("========== me->AudSys_TV_control = 1\n");
					}
					else
					{
						me->AudSys_TV_control = 0;
						//LOGD("========== me->AudSys_TV_control = 0\n");

					}
					me->Audio_path = stEP_BasicInfo_reg0x10.Audio_Path;
				}

				if(me->eARC_state != unBasicInfo.eARC_ON)
				{
					 me->eARC_state = unBasicInfo.eARC_ON;
					 if((me->eARC_state == eRepeater_ARCSRC) && (me->RepeaterSrc == ePrimary_Sel_None_ARC))
					 {
					     LOGD("========== eRepeater_ARCSRC = 0\n");
						 stEP_BasicInfo_reg0x10.ARC_EN = 1;
						 HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);
					 }
					 if((me->eARC_state == eRepeater_eARCSRC) && (me->RepeaterSrc == ePrimary_Sel_None_ARC))
					 {
					 	 LOGD("========== eRepeater_eARCSRC = 0\n");
						 stEP_BasicInfo_reg0x10.eARC_EN = 1;
						 stEP_BasicInfo_reg0x10.ARC_EN = 0;
						 HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);

					 }
					 RepeaterSrv_eARCSRC(unBasicInfo.eARC_ON);
				}
				//Mute Amplifier if TV Speaker output audio
				if(((stEP_BasicInfo_reg0x11.DataByte < ePrimary_Sel_None_Inactive)&& (stEP_BasicInfo_reg0x10.Audio_Path == 0 )&& (stEP_BasicInfo_reg0x10.Power == 1))
					||((stEP_BasicInfo_reg0x11.Primary_Sel == ePrimary_Sel_None_ARC)&&(unBasicInfo.eARC_ON == 0)&&(unBasicInfo.ARC_ON == 0)))
				{

					LOGD("TV Speaker output audio!!!\n");
					RepeaterSrv_AudOutput( eRepeater_TV_Speaker);

				}
				else if(stEP_BasicInfo_reg0x10.CEC_Mute == 1 )
				{
					LOGD("Audio System was silenced! \n");

				}
				else
				{

					LOGD("Audio System output audio!!!\n");
					RepeaterSrv_AudOutput(eRepeater_Audio_System);

				}



				if (stEP_BasicInfo_reg0x10.CEC_Mute != me->Mute_state)
				{
					if(me->RepeaterSrc != ePrimary_Sel_None_Inactive)
					{
						me->Mute_state = stEP_BasicInfo_reg0x10.CEC_Mute;
						QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MUTE_RELEASE_SIG), (void*)0); // mute
					}
				}

				if(stEP_BasicInfo_reg0x10.Power != stEP_BasicInfo_reg0x10_pre.Power)
				{

					if((stEP_BasicInfo_reg0x10.Power == eRepeater_Power_off)&&(me->Repeater_power_state == eRepeater_Power_on)
						&&(stEP_BasicInfo_reg0x11.DataByte < ePrimary_Sel_None_Inactive)) //modify for 8107 Source not to standby
					{
					    LOGD("Repeater Power == eRepeater_Power_off\n");
						QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, POWER_RELEASE_SIG), (void*)0);//power down

					}
					me->Repeater_power_state = stEP_BasicInfo_reg0x10.Power;
				}



				if(stEP_BasicInfo_reg0x13_pre.DataByte != stEP_BasicInfo_reg0x13_t.DataByte)
				{
				int temp = 0;
				temp = me->EX_INTR_New - me ->EX_INTR_last;
				    //LOGD("=========== temp : %d\n",temp);
					if(temp > VOLUME_TIMEOUT)
					{
						if(stEP_BasicInfo_reg0x13_pre.DataByte < stEP_BasicInfo_reg0x13_t.DataByte)
						{
							LOGD("EP_CEC_VOL_UP\n");
							QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_UP_RELEASE_SIG), (void*)0);

						}
						else
						{
							LOGD("EP_CEC_VOL_DOWN\n");
							QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_DOWN_RELEASE_SIG), (void*)0);

						}
					}

				}

/*
                if (stEP_BasicInfo_reg0x9.CEC_ECF == 1)
                {
                    res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_CEC_EVENT_CODE, pBuf , 5);
                    LOGD("CEC event code: %d\n", pBuf[0]);
                    for (cnt = 1; cnt < 5; cnt++)
                    {
                        LOGD("param[%d]: 0x%x\n", cnt, pBuf[cnt]);
                    }

                    if ((me->stRepeatHandler.pRegTbl[EP_ADDR_CEC_EVENT_CODE] == EP_CEC_EVT_USER_CTRL) //user control from TV
                        && (me->stRepeatHandler.pRegTbl[EP_ADDR_CEC_EVENT_PARAM_1] == EP_CEC_FROM_TV)
                        )
                    {
                        switch (me->stRepeatHandler.pRegTbl[EP_ADDR_CEC_EVENT_PARAM_2])
                        {
                            case EP_CEC_VOL_UP:
								    LOGD("EP_CEC_VOL_UP\n");
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_UP_RELEASE_SIG), (void*)0);
									break;

								case EP_CEC_VOL_DOWN:
									LOGD("EP_CEC_VOL_UP\n");
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, VOL_DOWN_RELEASE_SIG), (void*)0);
									break;

								case EP_CEC_POWER:
									LOGD("CEC standby cmd\n");
									QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, POWER_RELEASE_SIG), (void*)0);
									break;

								default:
									LOGD("unsupport CEC cmd\n");
									break;
							}

						}
						else if ((me->stRepeatHandler.pRegTbl[EP_ADDR_CEC_EVENT_CODE] == EP_CEC_EVT_GO_TO_STANDBY)
							      && (stEP_BasicInfo_reg0x11.DataByte < ePrimary_Sel_None_Inactive)) //modify for 8107 Source not to standby
						{
							LOGD("******************CEC standby cmd\n");
							QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, POWER_RELEASE_SIG), (void*)0);
						}

                }
*/
                LOGD("ADO_CHF: %d\n",stEP_BasicInfo_reg0x9.ADO_CHF);

					if((stEP_BasicInfo_reg0x11.Primary_Sel!= ePrimary_Sel_None_ARC) && (me->RepeaterSrc != ePrimary_Sel_None_Inactive))
					{
						if ((stEP_BasicInfo_reg0x9.ADO_CHF == 1) && (stEP_BasicInfo_reg0x11.Primary_Sel != ePrimary_Sel_None_Inactive) )
						{

							RepeaterSrv_SetAudFmt(&me->stRepeatHandler);
						}
					}

					else if((stEP_BasicInfo_reg0x11.Primary_Sel== ePrimary_Sel_None_ARC))
					{
						if(me->eARC_state == eRepeater_ARCSRC)
						{
							//me->stRepeatHandler.DSP_AUD_FIFO = SPDIF_SOURCE;
							if(me->stRepeatHandler.DSP_AUD_FIFO != Source_ARC)
							{
								me->stRepeatHandler.DSP_AUD_FIFO = Source_ARC;
								//RepeaterSrv_UserDSPSRCID(me->stRepeatHandler.DSP_AUD_FIFO);
								RepeaterSrv_User_ARC_DSPSRCID(me->stRepeatHandler.DSP_AUD_FIFO);
							}
							if(stEP_BasicInfo_reg0x10.ARC_EN ==eRepeater_ARCSRC)
							{
								LOGD("EP ARC register erro  set ARC 1\n");
								stEP_BasicInfo_reg0x10.ARC_EN = 1;
								HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);

							}
						}
						else
						{
							if ((stEP_BasicInfo_reg0x9.ADO_CHF == 1) && (stEP_BasicInfo_reg0x11.Primary_Sel != ePrimary_Sel_None_Inactive) )
							{

								RepeaterSrv_SetAudFmt(&me->stRepeatHandler);
							}
							if(stEP_BasicInfo_reg0x10.eARC_EN == eRepeater_ARCSRC)
							{
								LOGD("EP ARC register erro  set eARC 1\n");
								stEP_BasicInfo_reg0x10.eARC_EN = 1;
								HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_BasicInfo_reg0x10.DataByte, 1);

							}

						}
					}
				}



			 me->EX_INTR_last = GetMSFromSTC();

            me->HPD_state = unBasicInfo.HPD; // update cec state
            status = Q_HANDLED();
            break;
        }

		case REPEATER_SRC_SWITCH_SIG: {
            LOGD(" REPEATER_SRC_SWITCH_SIG; \n");

            RepeaterSrcEvt_t* stSrcEvt;
			unEP_BasicInfo_reg0x08_t stBasicInfo_reg0x08;


            unEP_GenCtrl_reg0x11_t stEP_GenCtrl_reg0x11;
            unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
            unEP_GenCtrl_reg0x12_t stEP_GenCtrl_reg0x12;

		    stEP_GenCtrl_reg0x12.DataByte = 0x00;


            stSrcEvt = (RepeaterSrcEvt_t*)e;

			//stBasicInfo_reg0x08.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_INFO_0x08];
			HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &stBasicInfo_reg0x08.DataByte , 1);
            stEP_GenCtrl_reg0x11.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x11];
            stEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
            me->RepeaterSrc = stSrcEvt->src_sel;

			LOGD("repeater register 0x08: 0x%x\n", stBasicInfo_reg0x08.DataByte);
			LOGD("stSrcEvt->src_sel: %d\n", stSrcEvt->src_sel);
            if( stEP_GenCtrl_reg0x10.Audio_Path == eRepeater_AudSrc_Disable)
            {
				me->TV_Switch_Source = 0;
			}

			if(!me->AudSys_TV_control)
			{
				 me->AudSys_User_control = 1;
				  //LOGD("================ me->AudSys_User_control = 1\n");
			}
			else
			{
				 me->AudSys_User_control = 0;
				  //LOGD("================= me->AudSys_User_control = 0\n");
			}
			if(stSrcEvt->src_sel != eRepeater_Src_ARC||stSrcEvt->src_sel != eRepeater_Src_Other)
			{
				if(!me->AudSys_TV_control)
				{
					stEP_GenCtrl_reg0x10.Audio_Path = 1;
					me->Audio_path = 1;
				}
			}
            if (stSrcEvt->src_sel == eRepeater_Src_HDMI_P0)
            {
           	 	LOGD("eRepeater_Src_HDMI_P0 :\n");
                stEP_GenCtrl_reg0x10.ARC_EN = 0;
				stEP_GenCtrl_reg0x10.eARC_EN =0;
				//stEP_GenCtrl_reg0x10.Audio_Path = 1;
				me->Audio_path = 1;
                stEP_GenCtrl_reg0x11.Primary_Sel = ePrimary_Sel_Port0;
            }
            else if (stSrcEvt->src_sel == eRepeater_Src_HDMI_P1)
            {
            	LOGD("eRepeater_Src_HDMI_P1 :\n");
                stEP_GenCtrl_reg0x10.ARC_EN = 0;
				stEP_GenCtrl_reg0x10.eARC_EN =0;
				//stEP_GenCtrl_reg0x10.Audio_Path = 1;
				me->Audio_path = 1;
                stEP_GenCtrl_reg0x11.Primary_Sel = ePrimary_Sel_Port1;
            }
            else if (stSrcEvt->src_sel == eRepeater_Src_HDMI_P2)
            {
           		LOGD("eRepeater_Src_HDMI_P2 :\n");
                stEP_GenCtrl_reg0x10.ARC_EN = 0;
				stEP_GenCtrl_reg0x10.eARC_EN =0;
				//stEP_GenCtrl_reg0x10.Audio_Path = 1;
				me->Audio_path = 1;
                stEP_GenCtrl_reg0x11.Primary_Sel = ePrimary_Sel_Port2;
            }
            else if (stSrcEvt->src_sel == eRepeater_Src_ARC)
            {
            	LOGD("eRepeater_Src_ARC :\n");
            	if(stBasicInfo_reg0x08.eARC_ON == 1)
            	{
					stEP_GenCtrl_reg0x10.eARC_EN = 1;
				}
				else
				{
					stEP_GenCtrl_reg0x10.ARC_EN = 1;
				}
				//RepeaterSrv_eARCScyn(stBasicInfo_reg0x08.eARC_ON);

				stEP_GenCtrl_reg0x10.Audio_Path = 1;
				me->Audio_path = 1;
                stEP_GenCtrl_reg0x11.Primary_Sel = ePrimary_Sel_None_ARC;
            }
            else if (stSrcEvt->src_sel == eRepeater_Src_Other)
            {
				LOGD("eRepeater_Src_Other :\n");

                stEP_GenCtrl_reg0x10.ARC_EN = 0;
				stEP_GenCtrl_reg0x10.Audio_Path = 0;
				me->Audio_path = 0;
				stEP_GenCtrl_reg0x11.Primary_Sel =  ePrimary_Sel_None_Inactive;
            }
            else
            {
                LOGE("repeater port set err : 0x%x\n", stSrcEvt->src_sel);

            }
			LOGD("me->TV_Switch_Source: 0x%x\n", me->TV_Switch_Source);

				if(me->TV_Switch_Source == 0)  // 8107 Switches Source
				{
				    // to set repeater register
				    int res = 0;
	                HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
	                LOGD("REPEATER_SRC_SWITCH_SIG register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);
	                HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &stEP_GenCtrl_reg0x11.DataByte, 1);
	                LOGD("REPEATER_SRC_SWITCH_SIG register 0x11: 0x%x\n", stEP_GenCtrl_reg0x11.DataByte);
				  res = HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x12, &stEP_GenCtrl_reg0x12.DataByte, 1);
		            LOGD("REPEATER_SRC_SWITCH_SIG register 0x12: 0x%x\n", stEP_GenCtrl_reg0x12.DataByte);
					me->TV_Switch_Source = 0;
					if(res == 1)
					{
						LOGD("============== I2C wrist OK ");
					}
					else
					{
						LOGD("============== I2C wrist NG ");

					}
				}
				else
				{
					if (stSrcEvt->src_sel == eRepeater_Src_ARC)
					{

		    			HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
	               		LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);
		            }

					 me->TV_Switch_Source = 0;
				}

			if ((stSrcEvt->src_sel < eRepeater_Src_Other))
			{

				status = Q_TRAN(RepeaterSrv_active);

            }
			else
			{
				status = Q_TRAN(RepeaterSrv_inactive);
			}

            break;
        }
		case REPEATER_MUTE_SYN_SIG:{
				RepeaterMuteEvt_t *src_Mute = (RepeaterMuteEvt_t*)e;

				LOGD("REPEATER_MUTE_SYN_SIG,USERAPP_Mute:%d,Repeater_Mute:%d",src_Mute->mute,me->Mute_state);
                if(me->RepeaterSrc!=eRepeater_Src_Other)
                {
					if(src_Mute->mute == 1&& me->Mute_state == 0)
					{
					   QACTIVE_POST(UserApp_get(), Q_NEW(QEvt, MUTE_RELEASE_SIG), (void*)0);
					}
                }
			status = Q_HANDLED();
			break;
		}

		case REPEATER_VOL_SET_SIG:{
			   UINT8 Vol;
			   LOGD("REPEATER_VOL_SET_SIG; \n");
			   unEP_GenCtrl_reg0x10_t unEP_GenCtrl_reg0x10;
			   unEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
			   if (unEP_GenCtrl_reg0x10.CEC_Mute == 1)
			   	{
				   unEP_GenCtrl_reg0x10.CEC_Mute = 0;
				   me->Mute_state = unEP_GenCtrl_reg0x10.CEC_Mute;

			   	}

			   RepeaterVolEvt_t *VolEvt = (RepeaterVolEvt_t*) e;

			   Vol = VolEvt->vol;
			   HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_CEC_VOLUME, &Vol, 1);
			   HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);


			   status = Q_HANDLED();
			   break;
		   }
		case REPEATER_MUTE_SET_SIG:{
			LOGD("REPEATER_MUTE_SET_SIG; \n");
			   unEP_GenCtrl_reg0x10_t unEP_GenCtrl_reg0x10;
			   RepeaterMuteEvt_t *MuteEvt = (RepeaterMuteEvt_t*) e;

			   unEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];

			   if(MuteEvt->mute != me->Mute_state)
			   {

					me->Mute_state = MuteEvt->mute;

					if (MuteEvt->mute == eMute_on)
						unEP_GenCtrl_reg0x10.CEC_Mute = 1;
					else
						unEP_GenCtrl_reg0x10.CEC_Mute = 0;


				   HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);
			   }

			   status = Q_HANDLED();
			   break;
		   }
		case REPEATER_AUDIO_PATH_SET_SIG:{
			   unEP_GenCtrl_reg0x10_t unEP_GenCtrl_reg0x10;
			   RepeaterAudPathEvt_t *Aud_path = (RepeaterAudPathEvt_t*) e;

			   unEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];

			   if (Aud_path->Aud_path == eRepeater_Audio_System )
				   unEP_GenCtrl_reg0x10.Audio_Path = 1;
			   else
				   unEP_GenCtrl_reg0x10.Audio_Path = 0;

			   HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);

			   status = Q_HANDLED();
			   break;
		   }

        case REPEATER_AUD_INFO_SEND_REQ_SIG: {
            LOGD("REPEATER_AUD_INFO_SEND_REQ_SIG; \n");
            status = Q_HANDLED();
            break;
        }

        case REPEATER_AUD_INFO_PRINTF_SIG:{
            LOGD("HDMI repeater info\n");
// for test {
            int cnt = 0;
            UINT8 pBuf[10];
            int res;

            if (e->sig == REPEATER_EX_INTR_0_SIG)
            {
                LOGD("request from HDMI repeater interrupt\n");
            }
            else
            {
                LOGD("request from user\n");
            }

            HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x09, pBuf, 1);
            LOGD("repeater data 0x09: 0x%x\n", pBuf[0]);
            unEP_BasicInfo_reg0x09_t stEP_BasicInfo_reg0x9;
            stEP_BasicInfo_reg0x9.DataByte = pBuf[0];
            LOGD("ADO_CHF: %d\n",stEP_BasicInfo_reg0x9.ADO_CHF);

            HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, pBuf, 1);
            LOGD("repeater data 0x10: 0x%x\n", pBuf[0]);
            unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
            stEP_GenCtrl_reg0x10.DataByte = pBuf[0];
            LOGD("power: %d\n",stEP_GenCtrl_reg0x10.Power);

            if (stEP_GenCtrl_reg0x10.Power == 0)
            {
                stEP_GenCtrl_reg0x10.Power = 1;
                HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);

                HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, pBuf, 1);
                LOGD("repeater data 0x10 after: 0x%x\n", pBuf[0]);
            }

            HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, pBuf, 1);
            LOGD("repeater data 0x11: 0x%x\n", pBuf[0]);
            unEP_GenCtrl_reg0x11_t stEP_GenCtrl_reg0x11;
            stEP_GenCtrl_reg0x11.DataByte = pBuf[0];
            LOGD("Primary_Sel: %d\n",stEP_GenCtrl_reg0x11.Primary_Sel);

            res = HDMI_Repeat_Get_info(&me->stRepeatHandler, 0x20, pBuf, 10);
            LOGD("repeater data from 0x20 to 0x29: \n");

            unEP_AudStatus_reg0x22_t stAudStatus_reg0x22;
            stAudStatus_reg0x22.DataByte = pBuf[EP_ADDR_AUDIO_STATUS - 0x20];
            LOGD("addr[0x%x] samp freq: 0x%x\n", EP_ADDR_AUDIO_STATUS, stAudStatus_reg0x22.SAMP_FREQ);
            LOGD("STD_ADO: 0x%x\n", stAudStatus_reg0x22.STD_ADO);
            LOGD("DSD_ADO: 0x%x\n", stAudStatus_reg0x22.DSD_ADO);
            LOGD("HBR_ADO: 0x%x\n", stAudStatus_reg0x22.HBR_ADO);
            LOGD("DST_ADO: 0x%x\n", stAudStatus_reg0x22.DST_ADO);

            unEP_ChStatus_reg0x23_t stEP_ChStatus_reg0x23;
            stEP_ChStatus_reg0x23.DataByte = pBuf[EP_ADDR_AUDIO_CH_STATUS_0x23 - 0x20];
            LOGD("addr[0x%x] PCM: 0x%x (0 for PCM, 1 for compressed)\n", EP_ADDR_AUDIO_CH_STATUS_0x23, stEP_ChStatus_reg0x23.PCM);

            unEP_AudInfo_reg0x29_t stEP_AudInfo_reg0x29;
            stEP_AudInfo_reg0x29.DataByte = pBuf[EP_ADDR_AUDIO_INFO_0x29 - 0x20];
            LOGD("addr[0x%x] channel number: 0x%x\n", EP_ADDR_AUDIO_INFO_0x29, stEP_AudInfo_reg0x29.CC);

            for (cnt = 0; cnt < 10; cnt++)
            {
                LOGD("addr[0x%x]: 0x%x\n", 0x20 + cnt, pBuf[cnt]);
            }

            unEP_BasicInfo_reg0x08_t unBasicInfo;
            res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unBasicInfo.DataByte , 1);
            LOGD("HPD : %d\n", unBasicInfo.HPD);

            if (unBasicInfo.HPD == 1)
            {
                HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_GEN_INFO_PHY_ADDR_H, pBuf, 2);
                LOGD("PhyAddr : 0x%x\n", ((pBuf[0] << 8) | pBuf[1]));
            }

            LOGD("CEC_ECF: %d\n",stEP_BasicInfo_reg0x9.CEC_ECF);
            res = HDMI_Repeat_Get_info(&me->stRepeatHandler, EP_ADDR_CEC_EVENT_CODE, pBuf , 5);
            LOGD("CEC event code: %d, param: 0x%x, 0x%x, 0x%x, 0x%x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4]);

// for test}
            status = Q_HANDLED();
            break;
        }

        case REPEATER_POWER_DOWN_SIG:{


			LOGD("REPEATER_POWER_DOWN_SIG!!!!!");
            unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
            stEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
            stEP_GenCtrl_reg0x10.Power = 0;
			stEP_GenCtrl_reg0x10.Audio_Path = 0;
			me->Repeater_power_state = eRepeater_Power_off;
            HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
            LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);
            RepeaterSrv_Standby();
            status = Q_HANDLED();
            break;
        }

        case REPEATER_POWER_UP_SIG:{

            unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
            stEP_GenCtrl_reg0x10.DataByte = me->stRepeatHandler.pRegTbl[EP_ADDR_GEN_CTRL_0x10];
            stEP_GenCtrl_reg0x10.Power = 1;
            HDMI_Repeat_Set(&me->stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
            LOGD("repeater register 0x10: 0x%x\n", stEP_GenCtrl_reg0x10.DataByte);

            status = Q_HANDLED();
            break;
        }

        case REPEATER_REG_PRINTF_SIG:{// print all regs of repeater
            int cnt;
            UINT8 pBuf[REPEATER_ADDR_MAX+1];
            HDMI_Repeat_Get_info(&me->stRepeatHandler, 0, pBuf, REPEATER_ADDR_MAX+1);

            for (cnt = 0; cnt < REPEATER_ADDR_MAX+1; cnt++)
            {
                LOGD("addr[0x%x]: 0x%x\n", cnt, pBuf[cnt]);
            }
            status = Q_HANDLED();
            break;
        }

		case REPEATER_DSPSourceID_SIG:
		{
		 	RepeatereDSPSrcIDEvt_t *dspSrcID = (RepeatereDSPSrcIDEvt_t*) e;
			 me->stRepeatHandler.DSP_AUD_FIFO = dspSrcID->DSPSourceID;
			status = Q_HANDLED();
			break;
		}
		case REPEATER_eARCSource_SIG:
		{
		 	RepeatereARCSrcEvt_t *eARCSrc = (RepeatereARCSrcEvt_t*) e;
			 me->eARC_state = eARCSrc->eARCSRC;
			status = Q_HANDLED();
			break;

		}
		case REPEATER_EP_UPGRADE_SIG:
		{
			LOGD("=========== REPEATER_EP_UPGRADE_SIG ==========\n");
			RepeaterUPgradeEvt_t *eUpgrade_mode = (RepeaterUPgradeEvt_t*) e;
			HDMI_Upgrade_process(eUpgrade_mode->UPgrade_Mode);
			status = Q_HANDLED();
			break;
		}

        default: {
            status = Q_SUPER(&QHsm_top);
            break;
        }
    }
    return status;
}


/******************************************************************************************/
/**
 * \fn			QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e)
 *
 * \brief		the state to handle the signal in repeater source active mode, such as HDMI TX (ARC), HDMI RX
 *
 * \param		RepeaterSrv_t * const me:(Input) the RepeaterSrv handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/
QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGD("RepeaterSrv_active ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            LOGD("RepeaterSrv_active EXIT; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGD("RepeaterSrv_active INIT; \n");
            status = Q_HANDLED();
            break;
        }
        default: {
            status = Q_SUPER(&RepeaterSrv_Home);
            break;
        }
    }
    return status;
}

/******************************************************************************************/
/**
 * \fn			QState RepeaterSrv_inactive(RepeaterSrv_t * const me, QEvt const * const e)
 *
 * \brief		the state to handle the signal in repeater source inactive mode, such as BT, line in...
 *
 * \param		RepeaterSrv_t * const me:(Input) the RepeaterSrv handler here,\n
 *              QEvt const * const e :event structure of signal \n
 *
 * \return		\e status : Q_HANDLED() in active state \n
 *			    \e status : Q_TRAN() trans to other sub_state \n
 *              \e status : Q_SUPER() trans to father state \n
 ******************************************************************************************/

QState RepeaterSrv_inactive(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            LOGD("RepeaterSrv_inactive ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            LOGD("RepeaterSrv_inactive EXIT; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            LOGD("RepeaterSrv_inactive INIT; \n");

            status = Q_HANDLED();
            break;
        }


        default: {
            status = Q_SUPER(&RepeaterSrv_Home);
            break;
        }
    }
    return status;
}

void RepeaterSrv_Intr_Callback_func0(uint32_t arg)
{
    (void)arg;

    QEvt *e = Q_NEW(QEvt, REPEATER_EX_INTR_0_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);
    LOGD("RepeaterSrv_Intr_Callback_func0; \n");
}

int RepeaterSrv_PowerUp(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_POWER_UP_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_UserPowerUp(RepeaterWakeup_t *Repeater_State)
{
	LOGD("================== RepeaterSrv_UserPowerUp 1=================; \n");

	extern RepeaterSrv_t l_Repeater;
	unEP_BasicInfo_reg0x08_t unEP_BasicInfo_reg0x08 = {0};
	int res = 0;
	res = HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unEP_BasicInfo_reg0x08.DataByte, 1);
	LOGD("Repeater_State->HPD_State :%d; \n",unEP_BasicInfo_reg0x08.HPD);

	volatile int *EP_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	volatile int *Repeater_saved_src = (int * )SRAM_ADDR_REPEATER_SAVED_SRC;
	if(*EP_Wakeup_Flag > 1)
	{
		if(res != 1)
		{
			//Repeater_State->EP_Reday = 0;
		}
		*EP_Wakeup_Flag = 0;
		*Repeater_saved_src = 6;
	}
	Repeater_State->power_state = *EP_Wakeup_Flag;
	Repeater_State->Source= *Repeater_saved_src;
	Repeater_State->HPD_state = unEP_BasicInfo_reg0x08.HPD;
	Repeater_State->eARC_state = unEP_BasicInfo_reg0x08.eARC_ON;
	LOGD("Repeater_State->eARC_state  :%d; \n",Repeater_State->eARC_state );
	LOGD("Repeater_State->power_state :%d; \n",Repeater_State->power_state);
	LOGD("Repeater_State->Source :%d; \n",Repeater_State->Source);
	RepeaterSrv_eARCScyn(Repeater_State->eARC_state);

#ifdef USERAPP_EP_UPGRADE
   HDMI_Upgrade_VER(&l_Repeater.stRepeatHandler);

#endif

	LOGD("================== RepeaterSrv_UserPowerUp 2=================; \n");

    return 0;
}

int RepeaterSrv_PowerDown(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_POWER_DOWN_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}


int RepeaterSrv_SendAudInfo(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_AUD_INFO_SEND_REQ_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_EPUpgrade(UINT8 ugrade_mode)
{
	RepeaterUPgradeEvt_t *e = Q_NEW(RepeaterUPgradeEvt_t, REPEATER_EP_UPGRADE_SIG);

    e->UPgrade_Mode = ugrade_mode;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}


int RepeaterSrv_AudOutput(eRepeater_AudioPath_t audio_output)
{
    RepeaterAudOutputEvt_t *e = Q_NEW(RepeaterAudOutputEvt_t, REPEATER_MUTE_AUDIO_OUTPUT_SIG);

	e->Aud_output =  audio_output;

    QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);

    return 0;
}



int RepeaterSrv_SrcSwitch(eRepeater_Src_t src_sel)
{
	LOGD("Userapp RepeaterSrv_SrcSwitch; \n");

    RepeaterSrcEvt_t *e = Q_NEW(RepeaterSrcEvt_t, REPEATER_SRC_SWITCH_SIG);

    e->src_sel = src_sel;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}
int RepeaterSrv_UserSwitch(eRepeater_Src_t src)
{
	QState status;
    RepeaterSrcEvt_t *e = Q_NEW(RepeaterSrcEvt_t, REPEATER_USER_SWITCHSRC_SIG);
    switch(src){
        case  eRepeater_Src_HDMI_P0:
				LOGD("eRepeater_Src_HDMI_P0; \n");
				e->src_user = eRepeater_userSrc_HDMI_P0;
		break;
		case  eRepeater_Src_HDMI_P1:
			    LOGD("eRepeater_Src_HDMI_P1; \n");
				e->src_user = eRepeater_userSrc_HDMI_P1;
		break;
		case  eRepeater_Src_HDMI_P2:
			    LOGD("eRepeater_Src_HDMI_P2; \n");
				e->src_user= eRepeater_userSrc_HDMI_P2;
		break;
		case  eRepeater_Src_ARC:
			    LOGD("eRepeater_Src_ARC : %d ; \n",eRepeater_uesrSrc_ARC);
				e->src_user = eRepeater_uesrSrc_ARC;
		break;
		case eRepeater_Src_Other:
        default:
			LOGD("Uerswitch error!!!\n");
              status = Q_HANDLED();
         break;
	}
	 QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
	return 0;
}

int RepeaterSrv_VolSet(int vol)
{
    RepeaterVolEvt_t *e = Q_NEW(RepeaterVolEvt_t, REPEATER_VOL_SET_SIG);

    e->vol = vol;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}
int RepeaterSrv_MuteSYN(int Mute)
{
	RepeaterMuteEvt_t *e = Q_NEW(RepeaterMuteEvt_t, REPEATER_MUTE_SYN_SIG);

	e->mute = Mute;

	QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

	return 0;

}

int RepeaterSrv_MuteSet(int Mute)
{
    RepeaterMuteEvt_t *e = Q_NEW(RepeaterMuteEvt_t, REPEATER_MUTE_SET_SIG);

    e->mute = Mute;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_SetAudPath(int Audio_path)
{
    RepeaterAudPathEvt_t *e = Q_NEW(RepeaterAudPathEvt_t, REPEATER_AUDIO_PATH_SET_SIG);

    e->Aud_path = Audio_path;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}


int RepeaterSrv_RegInfo(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_REG_PRINTF_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_AudInfo(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_AUD_INFO_PRINTF_SIG);

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}
int RepeaterSrv_eARCSRC(int eARCSRC)
{
    RepeatereARCSrcEvt_t *e = Q_NEW(RepeatereARCSrcEvt_t, REPEATER_eARCSRC_CHANGE_SIG);

    e->eARCSRC = eARCSRC;

    QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_SetAudFmt(stHdmiRepeatHandler_t* pstRepeatHandler)
{

	UINT8 pBuf[10];

    HDMI_Repeat_Get_info(pstRepeatHandler, EP_ADDR_AUDIO_INFO_START, pBuf, EP_AUDIO_INFO_REG_MUN);
    LOGD("repeater data from 0x20 to 0x29: \n");
	unEP_AudStatus_reg0x20_t stAudStatus_reg0x20;

	stAudStatus_reg0x20.DataByte = pBuf[EP_ADDR_AUDIO_INFO_START - EP_ADDR_AUDIO_INFO_START];
	LOGD("LAYOUT: 0x%x\n", stAudStatus_reg0x20.LAYOUT);

    unEP_AudStatus_reg0x22_t stAudStatus_reg0x22;
    stAudStatus_reg0x22.DataByte = pBuf[EP_ADDR_AUDIO_STATUS - EP_ADDR_AUDIO_INFO_START];
    LOGD("addr[0x%x] samplerate: 0x%x\n", EP_ADDR_AUDIO_STATUS, stAudStatus_reg0x22.SAMP_FREQ);
    LOGD("STD_ADO: 0x%x\n", stAudStatus_reg0x22.STD_ADO);
    LOGD("DSD_ADO: 0x%x\n", stAudStatus_reg0x22.DSD_ADO);
    LOGD("HBR_ADO: 0x%x\n", stAudStatus_reg0x22.HBR_ADO);
    LOGD("DST_ADO: 0x%x\n", stAudStatus_reg0x22.DST_ADO);

    unEP_ChStatus_reg0x23_t stEP_ChStatus_reg0x23;
    stEP_ChStatus_reg0x23.DataByte = pBuf[EP_ADDR_AUDIO_CH_STATUS_0x23 - EP_ADDR_AUDIO_INFO_START];
    LOGD("addr[0x%x] PCM: 0x%x (0 for PCM, 1 for compressed)\n", EP_ADDR_AUDIO_CH_STATUS_0x23, stEP_ChStatus_reg0x23.PCM);

    unEP_ChStatus_reg0x25_t stEP_ChStatus_reg0x25;
    stEP_ChStatus_reg0x25.DataByte = pBuf[EP_ADDR_AUDIO_CH_STATUS_0x25 - EP_ADDR_AUDIO_INFO_START];
    LOGD("addr[0x%x] channel number (SPDIF): 0x%x\n", EP_ADDR_AUDIO_CH_STATUS_0x25, stEP_ChStatus_reg0x25.CH_NUM);

    unEP_AudInfo_reg0x29_t stEP_AudInfo_reg0x29;
    stEP_AudInfo_reg0x29.DataByte = pBuf[EP_ADDR_AUDIO_INFO_0x29 - EP_ADDR_AUDIO_INFO_START];
   // LOGD("addr[0x%x] channel number (HDMI): 0x%x\n", EP_ADDR_AUDIO_INFO_0x29, stEP_AudInfo_reg0x29.CC);
   UINT8 New_dspfifo;

     if((stAudStatus_reg0x22.HBR_ADO == eRepeater_AudSrc_Enable)||
	 	((stEP_ChStatus_reg0x23.PCM == eRepeater_Aud_PCM)&&(stAudStatus_reg0x20.LAYOUT == eRepeater_AudSrc_Enable)) )
     {
		 New_dspfifo =  Source_HDMIHBR;

	 }
	 else
	 {
		 New_dspfifo =  Source_HDMISTD;

	 }
	 if(pstRepeatHandler ->DSP_AUD_FIFO != New_dspfifo )
	 {
		 RepeaterSrv_UserDSPSRCID(New_dspfifo);
	 }

	 if(stEP_ChStatus_reg0x23.PCM == eRepeater_Aud_PCM)
	  {
		  UINT8 channel_num = 0;
		  UINT32 channel_mapping = 0;
		  unEP_AudInfo_reg0x2C_t unEP_AudInfo_reg0x2C;
		  HDMI_Repeat_Get_info(pstRepeatHandler, EP_ADDR_AUDIO_INFO_0x2C, &unEP_AudInfo_reg0x2C.DataByte, 1);
		  if(stEP_AudInfo_reg0x29.CC != 0)
		  channel_num = stEP_AudInfo_reg0x29.CC + 1;
		  channel_mapping = setChannelMapping(unEP_AudInfo_reg0x2C);
		  RepeaterSrv_PCMChannel(channel_num,channel_mapping);
	  }
	 pstRepeatHandler ->DSP_AUD_FIFO = New_dspfifo;
	return 0;

}


int RepeaterSrv_VolConvert(UINT8 tbVol, UINT8 tbMaxVol)
{
    int iRes = 0;

    iRes = (tbVol*100/tbMaxVol);

    return iRes;
}
int RepeaterSrv_HPDStatus(int Hpd_status, int Arc_status)
{
    RepeaterHPDEvt_t *e = Q_NEW(RepeaterHPDEvt_t, REPEATER_HDMI_HPD_SIG);

    e->Hpd_status= Hpd_status;
   // e->Arc_status = Arc_status;
    LOGD("Hpd_status:%d Arc_status:%d\n", Hpd_status, Arc_status);

    QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);

    return 0;
}
int RepeaterSrv_DSPSRCID(int SRCID)
{
    RepeatereDSPSrcIDEvt_t *e = Q_NEW(RepeatereDSPSrcIDEvt_t, REPEATER_DSPSourceID_SIG);

    e->DSPSourceID = SRCID;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_PCMChannel(UINT8 LPcm_channel,UINT32 LPCM_ch_mapping)
{
	LOGD("RepeaterSrv_PCMChannel:%d LPCM_ch_mapping :%x \n", LPcm_channel,LPCM_ch_mapping);
	RepeatereLPCMNumEvt_t *e =  Q_NEW(RepeatereLPCMNumEvt_t, REPEATER_LPCMCHANNEL_NUM_SIG);
	e->LPCM_channelNum = LPcm_channel;
	e->LPCM_ChannelMapping = LPCM_ch_mapping;
	QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);

return 0;

}
static UINT32 setChannelMapping(unEP_AudInfo_reg0x2C_t inChMap)
{
	unDSP_AudInfo_t unDSP_AudInfo ;
	unDSP_AudInfo.DataByte = 0;
	unDSP_AudInfo.FL = audio_chmap_Enable;
	unDSP_AudInfo.FR = audio_chmap_Enable;
	unDSP_AudInfo.LFE = inChMap.LFE;
	unDSP_AudInfo.FC = inChMap.FC;
	if(inChMap.TpFc == audio_chmap_Enable)
	{
		unDSP_AudInfo.LS = audio_chmap_Enable;
		unDSP_AudInfo.RS = audio_chmap_Enable;
		if(((inChMap.FC|| inChMap.RS_FLC || inChMap.RLC_FLC) == audio_chmap_Disable)&& (inChMap.BC_LS == audio_chmap_Enable))
		{
			unDSP_AudInfo.TPFL = audio_chmap_Enable;
			unDSP_AudInfo.TpFR = audio_chmap_Enable;
		}
	}
	else
	{
		if(inChMap.RLC_FLC == audio_chmap_Disable)
		{

			//LOGD("inChMap.FC  :%x \n",inChMap.FC );
			if(inChMap.RS_FLC == audio_chmap_Enable)
			{
				unDSP_AudInfo.LS = audio_chmap_Enable;
				unDSP_AudInfo.RS = audio_chmap_Enable;
			}
		}
		else
		{
			if( (inChMap.RS_FLC == audio_chmap_Enable && inChMap.BC_LS == audio_chmap_Enable)||
				(inChMap.RS_FLC == audio_chmap_Disable && inChMap.BC_LS == audio_chmap_Disable))
			{
				unDSP_AudInfo.LS = audio_chmap_Enable;
				unDSP_AudInfo.RS = audio_chmap_Enable;
			}
			if((inChMap.RS_FLC == audio_chmap_Disable)&&(inChMap.BC_LS == audio_chmap_Disable))
			{
				unDSP_AudInfo.Lrs = audio_chmap_Enable;
				unDSP_AudInfo.Rrs = audio_chmap_Enable;
			}
		}

	}
		//LOGD("unDSP_AudInfo.DataByte :%x \n", unDSP_AudInfo.DataByte);
	return unDSP_AudInfo.DataByte;

}


int RepeaterSrv_eARCScyn(int SRCID)
{
    RepeatereARCSrcEvt_t *e = Q_NEW(RepeatereARCSrcEvt_t, REPEATER_eARCSource_SIG);

    e->eARCSRC = SRCID;

    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);

    return 0;
}


int RepeaterSrv_UserDSPSRCID(int SRCID)
{
	RepeaterAudSrcEvt_t *RepeaterAudSrcEvt;
	RepeaterAudSrcEvt = Q_NEW(RepeaterAudSrcEvt_t,REPEATER_AUD_SRC_SIG);
	RepeaterAudSrcEvt->Aud_I2S_Lane = SRCID;
	QACTIVE_POST(UserApp_get(), (QEvt *)RepeaterAudSrcEvt, (void*)0);

    return 0;
}
int RepeaterSrv_User_ARC_DSPSRCID(int SRCID)
{

    RepeaterAudSrcEvt_t *RepeaterAudSrcEvt;
	RepeaterAudSrcEvt = Q_NEW(RepeaterAudSrcEvt_t,REPEATER_AUD_ARC_SRC_SIG);
	RepeaterAudSrcEvt->Aud_I2S_Lane = SRCID;
	QACTIVE_POST(UserApp_get(), (QEvt *)RepeaterAudSrcEvt, (void*)0);

    return 0;

}

//l_Repeater
#ifdef USERAPP_SUPPORT_REPEATER

int RepeaterSrv_WakeUpCheck(UINT8 *HPD_State,UINT8 *Repeater_State)
{
    int iRes = 0;
    UINT8 pBuf[10];
    extern RepeaterSrv_t l_Repeater;
	LOGD("RepeaterSrv_UserPowerUp; \n");

	//volatile int *EP_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	//volatile int *EP_Wakeup_Source = (int * )SRAM_ADDR_WAKE_UP_SRC;


    unEP_BasicInfo_reg0x09_t stEP_BasicInfo_reg0x9 = {0};
    HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_INFO_0x09, &stEP_BasicInfo_reg0x9.DataByte , 1);

	unEP_BasicInfo_reg0x08_t unEP_BasicInfo_reg0x08 = {0};
	HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unEP_BasicInfo_reg0x08.DataByte, 1);

	unEP_GenCtrl_reg0x10_t unEP_GenCtrl_reg0x10 = {0};
    HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);

	unEP_GenCtrl_reg0x11_t unEP_GenCtrl_reg0x11 = {0};
    HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &unEP_GenCtrl_reg0x11.DataByte, 1);

    LOGD("CEC_ECF: %d\n",stEP_BasicInfo_reg0x9.CEC_ECF);
	LOGD("power bit: %d\n",unEP_GenCtrl_reg0x10.Power);
	LOGD("EP Source: %d\n",unEP_GenCtrl_reg0x11.Primary_Sel);

	*HPD_State = unEP_BasicInfo_reg0x08.HPD;
	*Repeater_State = unEP_GenCtrl_reg0x11.Primary_Sel;
	switch(unEP_GenCtrl_reg0x11.Primary_Sel){
			case  eRepeater_Src_HDMI_P0:
					LOGD("eRepeater_Src_HDMI_P0; \n");
					*Repeater_State = eRepeater_userSrc_HDMI_P0;
			break;
			case  eRepeater_Src_HDMI_P1:
					LOGD("eRepeater_Src_HDMI_P1; \n");
					*Repeater_State = eRepeater_userSrc_HDMI_P1;
			break;
			case  eRepeater_Src_HDMI_P2:
					LOGD("eRepeater_Src_HDMI_P2; \n");
					*Repeater_State = eRepeater_userSrc_HDMI_P2;
			break;
			case  eRepeater_Src_ARC:
					LOGD("eRepeater_Src_ARC : %d ; \n",eRepeater_uesrSrc_ARC);
					*Repeater_State = eRepeater_uesrSrc_ARC;
			break;
			case eRepeater_Src_Other:
			default:
				*Repeater_State = eRepeater_userSrc_Other;
			break;
		}

	if (stEP_BasicInfo_reg0x9.CEC_ECF==  1)
    {
        HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_CEC_EVENT_CODE, pBuf , 5);
        LOGD("CEC event code: %d, param: 0x%x, 0x%x, 0x%x, 0x%x\n", pBuf[0], pBuf[1], pBuf[2], pBuf[3], pBuf[4]);

        if (pBuf[0] == EP_CEC_EVT_AUD_MODE)
        {
            iRes = 1;
			//*EP_Wakeup_Flag = 1;
			//*EP_Wakeup_Source = unEP_GenCtrl_reg0x11.Primary_Sel;
            LOGD("CEC wake up\n");
        }

    }

	if (unEP_GenCtrl_reg0x10.Power == 1)
	{
		iRes = 1;
		LOGD("power bit = 1 && Audio_path = 1! \n");
	}

    return iRes;
}

int RepeaterSrv_WakeupCheckHandler(void)
{
    UINT8 buf;
    unEP_BasicInfo_reg0x09_t epReg0x09 = {0};
    unEP_GenCtrl_reg0x10_t epReg0x10 = {0};
    unEP_GenCtrl_reg0x11_t epReg0x11 = {0};
    UINT8 *i2cStr = (UINT8 *)GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
    I2cData_t i2cData;
    volatile int *EP_Wakeup_Flag = (int *)SRAM_ADDR_EP_WAKE_UP_FLAG;
    volatile int *Repeater_saved_src = (int *)SRAM_ADDR_REPEATER_SAVED_SRC;

    i2cData.slaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
    i2cData.buf = &epReg0x10.DataByte;
    i2cData.cmd = EP_ADDR_GEN_CTRL_0x10;
    i2cData.cmdLen = 1;
    i2cData.dataLen = 1;
    i2cData.showMsg = 0;
    if(I2C_Read(i2cStr, &i2cData) == FALSE) {
        LOGX("i2c read fail...\n");
        return -1;
    }

    LOGD("ep reg 0x10 = 0x%02x\n", epReg0x10.DataByte);

    i2cData.buf = &epReg0x11.DataByte;
    i2cData.cmd = EP_ADDR_GEN_CTRL_0x11;
    if(I2C_Read(i2cStr, &i2cData) == FALSE) {
        LOGX("i2c read fail...\n");
        return -1;
    }
    LOGD("ep reg 0x11 = 0x%02x\n", epReg0x11.DataByte);
    switch(epReg0x11.DataByte) {
    case  eRepeater_Src_HDMI_P0:
    {
        *Repeater_saved_src = eRepeater_userSrc_HDMI_P0;
        break;
    }
    case  eRepeater_Src_HDMI_P1:
    {
        *Repeater_saved_src = eRepeater_userSrc_HDMI_P1;
        break;
    }
    case  eRepeater_Src_HDMI_P2:
    {
        *Repeater_saved_src = eRepeater_userSrc_HDMI_P2;
        break;
    }
    case  eRepeater_Src_ARC:
    {
        *Repeater_saved_src = eRepeater_uesrSrc_ARC;
        break;
    }
    case  eRepeater_Src_Other:
    {
        *Repeater_saved_src = eRepeater_Src_Other;
        break;
    }
    }

    *EP_Wakeup_Flag = epReg0x10.Power;

    if(epReg0x10.Power == 1) {
        // wakeup
        LOGI("wakeup by repeater\n");
        return TRUE;
    }

    i2cData.cmd = EP_ADDR_GEN_INFO_0x09;
    i2cData.buf = &epReg0x09.DataByte;
    if(I2C_Read(i2cStr, &i2cData) == FALSE) {
        LOGX("i2c read fail...\n");
        return -1;
    }

    LOGD("ep reg 0x09 = 0x%02x\n", epReg0x09.DataByte);

    if(epReg0x09.CEC_ECF == 1) {
        i2cData.cmd = EP_ADDR_CEC_EVENT_CODE;
        i2cData.buf = &buf;
        if(I2C_Read(i2cStr, &i2cData) == FALSE) {
            LOGX("i2c fail...\n");
            return -1;
        }

        LOGD("ep reg 0x15 = 0x%02x\n", buf);

        if(buf == EP_CEC_EVT_AUD_MODE) {
            // wakeup
            LOGI("wakeup by repeater\n");
            *EP_Wakeup_Flag = epReg0x10.Power;
            return TRUE;
        }
    }

    return FALSE;
}

int RepeaterSrv_WakeupFlash(void)
{
    volatile int *EP_Wakeup_Flag = (int *)SRAM_ADDR_EP_WAKE_UP_FLAG;
    volatile int *Repeater_saved_src = (int *)SRAM_ADDR_REPEATER_SAVED_SRC;

    *EP_Wakeup_Flag = 0;
    *Repeater_saved_src = 0;
    return 0;
}

#else
int RepeaterSrv_WakeUpCheck(UINT8 *HPD_State, UINT8 *Repeater_State)
{
	(void)HPD_State;
	(void)Repeater_State;

	return 0;
}

int RepeaterSrv_WakeupCheckHandler(void)
{
    return TRUE;
}

int RepeaterSrv_WakeupFlash(void)
{
    return 0;
}

#endif

int RepeaterSrv_Standby(void)
{
    GPIO_DeInitIntr(eIntr_0);

    return 0;
}

int RepeaterSrv_EP_Init(void)
{
    extern RepeaterSrv_t l_Repeater;
	l_Repeater.stRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
	l_Repeater.stRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;

	volatile int *EP_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	volatile int *Repeater_saved_src = (int * )SRAM_ADDR_REPEATER_SAVED_SRC;

	*Repeater_saved_src = Userapp_Get_System_Memory_Source();
	LOGD("sys_memory_source is %d; \n",*Repeater_saved_src);

	unEP_GenCtrl_reg0x10_t unEP_GenCtrl_reg0x10;

    HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);


	unEP_GenCtrl_reg0x10.Power = 1;

	if((*EP_Wakeup_Flag == 1)||((*Repeater_saved_src >= eAp_ARC_Src)&&(*Repeater_saved_src <= eAp_HDMI2_Src)))
	{
		unEP_GenCtrl_reg0x10.Audio_Path = 1;
	}


	HDMI_Repeat_Set(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &unEP_GenCtrl_reg0x10.DataByte, 1);
	LOGD("repeater register 0x10: 0x%x\n", unEP_GenCtrl_reg0x10.DataByte);


    return 0;
}
int RepeaterSrv_SourceIDChk(RepeaterSrcState_t *repeaterstatus)
{
    stHdmiRepeatHandler_t pstRepeatHandler;
    pstRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
    pstRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
	unEP_BasicInfo_reg0x08_t unEP_BasicInfo_reg0x08;
	unEP_GenCtrl_reg0x11_t unEP_GenCtrl_reg0x11;

    // UINT8 pBuf[4];

    // HDMI_Repeat_Get_info(pstRepeatHandler, EP_ADDR_AUDIO_INFO_START, pBuf,4);
    UINT8 pBuf[10];

    HDMI_Repeat_Get_info(&pstRepeatHandler, EP_ADDR_AUDIO_INFO_START, pBuf, EP_AUDIO_INFO_REG_MUN);
    LOGD("repeater data from 0x20 to 0x29: \n");
    unEP_AudStatus_reg0x20_t stAudStatus_reg0x20;

    stAudStatus_reg0x20.DataByte = pBuf[EP_ADDR_AUDIO_INFO_START - EP_ADDR_AUDIO_INFO_START];
    LOGD("LAYOUT: 0x%x\n", stAudStatus_reg0x20.LAYOUT);

    unEP_AudStatus_reg0x22_t stAudStatus_reg0x22;
    stAudStatus_reg0x22.DataByte = pBuf[EP_ADDR_AUDIO_STATUS - EP_ADDR_AUDIO_INFO_START];
    LOGD("STD_ADO: 0x%x\n", stAudStatus_reg0x22.STD_ADO);
    LOGD("DSD_ADO: 0x%x\n", stAudStatus_reg0x22.DSD_ADO);
    LOGD("HBR_ADO: 0x%x\n", stAudStatus_reg0x22.HBR_ADO);
    LOGD("DST_ADO: 0x%x\n", stAudStatus_reg0x22.DST_ADO);

    unEP_ChStatus_reg0x23_t stEP_ChStatus_reg0x23;
    stEP_ChStatus_reg0x23.DataByte = pBuf[EP_ADDR_AUDIO_CH_STATUS_0x23 - EP_ADDR_AUDIO_INFO_START];
    LOGD("addr[0x%x] PCM: 0x%x (0 for PCM, 1 for compressed)\n", EP_ADDR_AUDIO_CH_STATUS_0x23, stEP_ChStatus_reg0x23.PCM);
	HDMI_Repeat_Get_info(&pstRepeatHandler, EP_ADDR_GEN_INFO_0x08, &unEP_BasicInfo_reg0x08.DataByte, 1);
	HDMI_Repeat_Get_info(&pstRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &unEP_GenCtrl_reg0x11.DataByte, 1);

    UINT8 New_dspfifo;
	repeaterstatus->eARC_SRC = unEP_BasicInfo_reg0x08.eARC_ON;
	RepeaterSrv_eARCScyn(repeaterstatus->eARC_SRC);

    if((stAudStatus_reg0x22.HBR_ADO == eRepeater_AudSrc_Enable) ||
       ((stEP_ChStatus_reg0x23.PCM == eRepeater_Aud_PCM) && (stAudStatus_reg0x20.LAYOUT == eRepeater_AudSrc_Enable))) {
        New_dspfifo = Source_HDMIHBR;
    } else {
        New_dspfifo = Source_HDMISTD;
    }
	if(unEP_GenCtrl_reg0x11.Primary_Sel == ePrimary_Sel_None_ARC)
	{
		if(unEP_BasicInfo_reg0x08.ARC_ON == eRepeater_ARC_ON)
		{
            LOGD("=============== Source_ARC ================== \n");
			New_dspfifo = Source_ARC;
		}
	}
	RepeaterSrv_DSPSRCID(New_dspfifo);
    // pstRepeatHandler->DSP_AUD_FIFO = New_dspfifo;

    repeaterstatus->Source_ID = New_dspfifo;

    return 0;
}

void RepeaterSrv_EP_Reinit(void)
{
	   UINT16 pSrc;
	   extern RepeaterSrv_t l_Repeater;
	   int Standby_Mode=GET_SDK_CFG(CFG_PMC_T)->bFstPwrOnFlag;

	   l_Repeater.stRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
	   l_Repeater.stRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;

	   unEP_BasicInfo_reg0x08_t stBasicInfo_reg0x08;
	   unEP_GenCtrl_reg0x10_t stEP_GenCtrl_reg0x10;
	   unEP_GenCtrl_reg0x11_t stEP_GenCtrl_reg0x11;
	   unEP_GenCtrl_reg0x12_t stEP_GenCtrl_reg0x12;

	   HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_INFO_0x08, &stBasicInfo_reg0x08.DataByte , 1);
	   HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
	   HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &stEP_GenCtrl_reg0x11.DataByte, 1);
	   HDMI_Repeat_Get_info(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x12, &stEP_GenCtrl_reg0x12.DataByte, 1);

	   UserSetting_ReadSrc(&pSrc);

	  switch(pSrc){
        case  eRepeater_userSrc_HDMI_P0:
				LOGD("Src_HDMI_P0; \n");
				stEP_GenCtrl_reg0x11.DataByte = eRepeater_Src_HDMI_P0;
		break;
		case  eRepeater_userSrc_HDMI_P1:
			    LOGD("Src_HDMI_P1; \n");
				stEP_GenCtrl_reg0x11.DataByte = eRepeater_Src_HDMI_P1;
		break;
		case  eRepeater_userSrc_HDMI_P2:
			    LOGD("Src_HDMI_P2; \n");
				stEP_GenCtrl_reg0x11.DataByte = eRepeater_Src_HDMI_P2;
		break;
		case  eRepeater_uesrSrc_ARC:
			    LOGD("Src_ARC; \n");
				stEP_GenCtrl_reg0x11.DataByte = eRepeater_Src_ARC;
		break;
        default:
			LOGD("Other_Source;\n");
            stEP_GenCtrl_reg0x11.DataByte = eRepeater_Src_Other;
        break;
	}


#if defined CFG_DISABLE_BOOT_STANDBY_MODE
	(void)Standby_Mode;

	if(stEP_GenCtrl_reg0x10.Power == 0)
	      {
	       stEP_GenCtrl_reg0x10.Power = 1;
	      }
		  if(stEP_GenCtrl_reg0x11.DataByte == eRepeater_Src_Other)
		  {
			LOGD("Other Src,Set Audio Path off!!!\n");
			stEP_GenCtrl_reg0x10.Audio_Path = 0;
		  }
		  if (stEP_GenCtrl_reg0x11.DataByte !=eRepeater_Src_ARC)
		  {
			LOGD("ON ARC Source,off ARC and eARC Bit!!!\n");
			stEP_GenCtrl_reg0x10.ARC_EN = 0;
			stEP_GenCtrl_reg0x10.eARC_EN =0;
		  }
		  else
		  {
			LOGD("eRepeater_Src_ARC :\n");
			  if(stBasicInfo_reg0x08.eARC_ON == 1)
			  {
				stEP_GenCtrl_reg0x10.eARC_EN = 1;
			  }
			  else
			  {
				stEP_GenCtrl_reg0x10.ARC_EN = 1;
			  }

				stEP_GenCtrl_reg0x10.Audio_Path = 1;
	       }
		  if(stEP_GenCtrl_reg0x11.DataByte < eRepeater_Src_ARC)
		  {
			  stEP_GenCtrl_reg0x10.Audio_Path = 1;
		  }
	  HDMI_Repeat_Set(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x12, &stEP_GenCtrl_reg0x12.DataByte, 1);
#else
	  if(Standby_Mode == 1)
	  {
			  LOGD("standby mode,Set Audio Path off!!!\n");
			  stEP_GenCtrl_reg0x10.Audio_Path = 0;
	  }

	  if(Standby_Mode == 0)
	  {
	      if(stEP_GenCtrl_reg0x10.Power == 0)
	      {
	       stEP_GenCtrl_reg0x10.Power = 1;
	      }
		  if(stEP_GenCtrl_reg0x11.DataByte == eRepeater_Src_Other)
		  {
			LOGD("Other Src,Set Audio Path off!!!\n");
			stEP_GenCtrl_reg0x10.Audio_Path = 0;
		  }
		  if (stEP_GenCtrl_reg0x11.DataByte !=eRepeater_Src_ARC)
		  {
			LOGD("ON ARC Source,off ARC and eARC Bit!!!\n");
			stEP_GenCtrl_reg0x10.ARC_EN = 0;
			stEP_GenCtrl_reg0x10.eARC_EN =0;
		  }
		  else
		  {
			LOGD("eRepeater_Src_ARC :\n");
			  if(stBasicInfo_reg0x08.eARC_ON == 1)
			  {
				stEP_GenCtrl_reg0x10.eARC_EN = 1;
			  }
			  else
			  {
				stEP_GenCtrl_reg0x10.ARC_EN = 1;
			  }

				stEP_GenCtrl_reg0x10.Audio_Path = 1;
	       }
		  if(stEP_GenCtrl_reg0x11.DataByte < eRepeater_Src_ARC)
		  {
			  stEP_GenCtrl_reg0x10.Audio_Path = 1;
		  }
	  HDMI_Repeat_Set(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x12, &stEP_GenCtrl_reg0x12.DataByte, 1);
	  }
#endif
      HDMI_Repeat_Set(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x10, &stEP_GenCtrl_reg0x10.DataByte, 1);
	  HDMI_Repeat_Set(&l_Repeater.stRepeatHandler, EP_ADDR_GEN_CTRL_0x11, &stEP_GenCtrl_reg0x11.DataByte, 1);
}

int RepeaterSrv_EP_ErroCheck(void)
{
    extern RepeaterSrv_t l_Repeater;
    return l_Repeater.First_PowerUp_Flag;

}


int RepeaterSrv_EP_RegCB(void)
{
	HDMI_RegCB(Power_EP_INIT,RepeaterSrv_EP_Init);
    HDMI_RegCB(Power_EP_WakeChk_Handler,RepeaterSrv_WakeupCheckHandler);
	HDMI_RegCB(Power_EP_WakeChkFlash,RepeaterSrv_WakeupFlash);
	return 0;
}


#define HDMI_SND_LEN	16

void HDMI_Upgrade_VER(stHdmiRepeatHandler_t *pstRepeatHandler)
{
	int EP_Version;
	int EP_codeVersion = 0;
	char EP_binname[16] = "ep_firmware.bin";
	int part_addr;
	int part_size;
	int file_size;
	int bres = 0;
	EP_Version	= HDMI_GetVersion(pstRepeatHandler);
	bres = GetPartitionInfo(EP_binname,&part_addr,&part_size,&file_size);
	if(bres == -1)
	{
		return;
	}
	//LOGD("=============== part_addr: %x ; part_size : %d;file_size :%d ================== \n",part_addr,part_size,file_size);
	UINT32 *ptr = (UINT32 *)part_addr;
	//Read_flash_EPcode(part_addr,pstCodeHandler.hdmi_upgrade_bin,HDMI_BLOCK_LEN);
	EP_codeVersion = *ptr&0x000000ff;
	if(EP_codeVersion > EP_Version)
	{
		Display_VFD(DisplaySrv_get());
		RepeaterSrv_EPUpgrade(eRepeater_Upgrade_VER);
	}
	return;

}

int HDMI_Upgrade_process(UINT8 Upgrade_key)
{
    stHdmiRepeatHandler_t pstRepeatHandler;
    pstRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
    pstRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
	int EP_Version;
	int EP_codeVersion = 0;
	char EP_binname[16] = "ep_firmware.bin";
	int part_addr;
	int part_size;
	int file_size;
	EPCodeSrv_t pstCodeHandler;
	pstCodeHandler.SndData = 0x40;

	EP_Version	= HDMI_GetVersion(&pstRepeatHandler);

    if (EP_Version == 0) {
       // return 0;
    }

	switch(Upgrade_key)
	{
		case eRepeater_Upgrade_EN:
		{
			LOGD("=============== eRepeater_Upgrade_EN ================== \n");
			while(1)
			{
				GetPartitionInfo(EP_binname,&part_addr,&part_size,&file_size);
				Repeater_EP_BB_ICP(&pstRepeatHandler,pstCodeHandler.SndData);
				//if(iRes == 1)//if EP in ISP mode and NG
				{
					pstCodeHandler.part_addr = part_addr;
					pstCodeHandler.CodeSize = file_size;
					Repeater_EP_UPgrading(&pstRepeatHandler,&pstCodeHandler);
					LOGD(" EP ISP Mode\n",EP_Version);
				}
				break;
			}

		break;
		}
		case eRepeater_Upgrade_VER:
		{
			int bres = 0;
			LOGD("=============== eRepeater_Upgrade_VER ================== \n");
			while(1)
			{
				bres = GetPartitionInfo(EP_binname,&part_addr,&part_size,&file_size);
				if(bres == -1)
				{
					return 0;
				}
				//LOGD("=============== part_addr: %x ; part_size : %d;file_size :%d ================== \n",part_addr,part_size,file_size);
				UINT32 *ptr = (UINT32 *)part_addr;
				//Read_flash_EPcode(part_addr,pstCodeHandler.hdmi_upgrade_bin,HDMI_BLOCK_LEN);
				EP_codeVersion = *ptr&0x000000ff;
				if(EP_codeVersion > EP_Version)
				{
					//LOGD("=============== EP_codeVersion: V%d ; EP_Version : V%d ================== \n",EP_codeVersion,EP_Version);
					Display_VFD(DisplaySrv_get());

					int iRes = Repeater_EP_BB_ICP(&pstRepeatHandler,pstCodeHandler.SndData);
					//if(iRes == 1)
					{
						pstCodeHandler.part_addr = part_addr;
						pstCodeHandler.CodeSize = file_size;
						iRes = Repeater_EP_UPgrading(&pstRepeatHandler,&pstCodeHandler);
						LOGD(" EP ISP Mode\n",EP_Version);
					}
					if(iRes == 0)
					{
					 	RepeaterSrv_UserSwitch(eRepeater_Src_ARC);
						break;
					}
					else if(iRes == -1)
					{
						LOGD(" EP code upgrade error \n");
						break;
					}

				}
				else
				{
					LOGD("=============== This is new EP code V%d ================== \n",EP_Version);
					break;
				}

			}

		break;
		}
		default:
			LOGD("=============== Not upgrade EP code ================== \n");
		break;
	}

	return 0;
}

int HDMI_GetVersion(stHdmiRepeatHandler_t *pstRepeatHandler)
{
	int HdmiBinVer = 0;
	//I2C_Read(pstRepeatHandler->pCtlStr, pstRepeatHandler->SlaveAddr, 0x04, &HdmiBinVer, 1);
	HDMI_Repeat_Get_info(pstRepeatHandler,0x04,&HdmiBinVer,1);
	LOGD("HDMI Version:%d\n", HdmiBinVer);

	return HdmiBinVer;
}

int Repeater_EP_BB_ICP(stHdmiRepeatHandler_t *pstRepeatHandler,UINT32 SndData)
{
	int bRet = 0;
	UINT8 RetryCnt=0;
		while((RetryCnt<50)&&(bRet != 1))
		{
			msleep(5);
			//bRet = I2C_Write(pstRepeatHandler.pCtlStr,0xC8,0x0f,SndData,1);
			//pstRepeatHandler->SlaveAddr = 0xC8;

			bRet = HDMI_Repeat_Set(pstRepeatHandler,EP_UPGRADE_CHECK_0x0f,&SndData,1);
			if (!bRet)
			{
				LOGD("I2C write faild\n");
			}
			msleep(100);
			/*//bRet = I2C_Read(pstRepeatHandler.pCtlStr,0x50,0xc1,RcvData,1);
			pstRepeatHandler->SlaveAddr = 0x50;
			bRet = HDMI_Repeat_Get_info(pstRepeatHandler,0xC1,&RcvData,1);
			if (!bRet)
			{
				LOGD("I2C read faild\n");
			}*/
			//LOGD("HDMI 50 RcvData = %x,retry=%d\n",RcvData,RetryCnt);
			RetryCnt++;
		}
		return bRet;

}

int Repeater_EP_UPgrading(stHdmiRepeatHandler_t *pstRepeatHandler,EPCodeSrv_t *pstCodeHandler)
{
		BOOL bRet = FALSE;
		UINT8 blocknum = 0;
		UINT8 sndnum = 0;
		UINT8 CycleCnt = pstCodeHandler->CodeSize/HDMI_BLOCK_LEN;
		UINT8 SndCnt = HDMI_BLOCK_LEN/HDMI_SND_LEN;
		char EP_buffer[HDMI_BLOCK_LEN]= {0};
		UINT8 HdmiUpgradePer = 0;
		UINT8 checkIIC = 1;
		UINT8 CodeData[HDMI_SND_LEN+1]={0x10};
		int read_lenth = 0;
		read_lenth = HDMI_BLOCK_LEN;
		UINT32 read_add = 0;
		LOGD("CycleCnt %d \n",CycleCnt);
		while(checkIIC)
		{
			//if(pstCodeHandler->RcvData==0x10)
			{
				for(blocknum=0;blocknum<CycleCnt;blocknum++)
				{
					//ÉèÖÃblock num
					LOGD("blocknum : %d \n",blocknum);
					HdmiUpgradePer = blocknum*5*4/CycleCnt;
					//SndData[0] = blocknum;
					pstCodeHandler->SndData = blocknum;
					//I2C_Write(HDMI_I2C,0x50,0x01,SndData,1);
					pstRepeatHandler->SlaveAddr = EP_UPGRADE_SLAVEADDR;
					bRet = HDMI_Repeat_Set(pstRepeatHandler,EP_UPGRADE_WRITE_START,&pstCodeHandler->SndData,1);
					//·¢ËÍcode
					//sdRet = read(sdfd,HdmiCodeBuf,HDMI_BLOCK_LEN);
					read_add = Read_flash_EPcode(pstCodeHandler->part_addr,&EP_buffer[0],read_lenth);
					for(sndnum=0;sndnum<SndCnt;sndnum++)
					{
							memcpy(&CodeData[1],&EP_buffer[HDMI_SND_LEN*sndnum],HDMI_SND_LEN);
							CodeData[0]=EP_UPGRADE_BLACK_HANDLER;
							pstRepeatHandler->SlaveAddr = EP_UPGRADE_SLAVEADDR;
							bRet = HDMI_Repeat_Set(pstRepeatHandler,EP_UPGRADE_WRITE_DATA_REG,CodeData,HDMI_SND_LEN+1);
							memset(&CodeData[1], 0, HDMI_SND_LEN);
					}
					 pstCodeHandler->part_addr = read_add ;
					msleep(60);

				}
			}
			/*
			else
			{
				LOGD("HDMI I2C UPGRADE RCV ERR!!\r\n");
				//FreeRTOS_I2C1_Ioctl(temp,SETDEVICECLOCK,0);
				return 1;
			}*/
			//exit BB_ICP mode
			pstCodeHandler->SndData = 0x00;
			//I2C_Write(pstRepeatHandler.pCtlStr,0x50,0x02,SndData,1);
			pstRepeatHandler->SlaveAddr = EP_UPGRADE_SLAVEADDR;
			bRet = HDMI_Repeat_Set(pstRepeatHandler,EP_UPGRADE_WRITE_END,&pstCodeHandler->SndData,1);
			msleep(6000);
			//int check_0xc9 = I2C_Read(pstRepeatHandler.pCtlStr,0xc9,0x0f,RcvData,1);
			pstRepeatHandler->SlaveAddr = EP_UPGRADE_CHECK_SLAVEADDR;
			int check_0xc9	= HDMI_Repeat_Get_info(pstRepeatHandler,EP_UPGRADE_CHECK_0x0f,&pstCodeHandler->RcvData,1);
			LOGD("HDMI c9 exit RcvData = %x\n",check_0xc9);
			if(check_0xc9 == 1)
			{
			   checkIIC = 0;
			}
			else
			{
			   //int check_0x50 = I2C_Read(pstRepeatHandler.pCtlStr,0x50,0xc1,RcvData,1);
				pstRepeatHandler->SlaveAddr = EP_UPGRADE_SLAVEADDR;
				int check_0x50	= HDMI_Repeat_Get_info(pstRepeatHandler,EP_UPGRADE_CHECK_0xc1,&pstCodeHandler->RcvData,1);
				if(check_0x50 == 1 )
				{
					checkIIC ++;
					if(checkIIC == 6)
						return -1;

				}
				else
				{
						 checkIIC = 0;
				}
			}
		}
	return 0;
}

UINT32 Read_flash_EPcode(UINT32 ptr_add,char *buffer,int Read_Lengh )
{
	UINT32 *ptr = (UINT32*)ptr_add;
	UINT32 buf = 0;
	int black = 0;
	int r_black = 0;
	black = Read_Lengh/4;
	while(black!= r_black)
	{
		buf = *ptr;
		buffer[0+r_black*4] = buf&0x000000ff;
		buffer[1+r_black*4] = buf>>8&0x000000ff;
		buffer[2+r_black*4] = buf>>16&0x000000ff;
		buffer[3+r_black*4] = buf>>24&0x000000ff;
		ptr++;
		//LOGD("buffer[%d] = %x \n",0+r_black*4,buffer[0+r_black*4]);
		//LOGD("buffer[%d] = %x \n",1+r_black*4,buffer[1+r_black*4]);
		//LOGD("buffer[%d] = %x \n",2+r_black*4,buffer[2+r_black*4]);
		//LOGD("buffer[%d] = %x \n",3+r_black*4,buffer[3+r_black*4]);
		r_black++;
	}
	UINT32 rerurn_ptr = (UINT32)ptr;
	return rerurn_ptr;
}

