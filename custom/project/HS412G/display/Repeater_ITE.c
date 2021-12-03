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
#include "log_utils.h"
#include "Repeater_Srv.h"
#include "custom_sig.h"
#include "Display_Srv.h"
#include "gpio_intr.h"
#include "hw_gpio_list.h"
#include "pinmux.h"
#include "MCU_customer_cmd.h"
#include "datarammap.h"
#include "interrupt_interfaces.h"
#include "cfg_sdk.h"
#include "i2c_api.h"
#include "iop_user.h"
#include "user_def.h"

Q_DEFINE_THIS_FILE;

#define cec_printf(f, a...)  	LOGD(f, ##a)
#define cec_printf0(f, a...)  	LOGX(f, ##a)
#define cec_printf2(f, a...)  	LOGDT(f, ##a)
#define cec_printf_E(f, a...) 	LOGE(f, ##a)

#include "ITEMT_Sys.h"
#include "ITEMT_SysS.h"

int gCurSysStatus = 0;
extern UINT8 g_u8I2cSlaveRBufB[0x100];
extern UINT8 g_u8I2cSlaveUpdateB;
UINT8 g_u8Cmd = 0;
iTE_u8 u8TEST=0;

QState RepeaterSrv_initial(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_Home(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e);
QState RepeaterSrv_inactive(RepeaterSrv_t * const me, QEvt const * const e);

int RepeaterSrv_WakeUpCheck(UINT8* HPD_State,UINT8* Repeater_State);
void RepeaterSrv_Intr_Callback_func0(void);

extern unsigned int GetMSFromSTC(void);
extern void TaskDelayMs(unsigned int sdMSec);

void RepeaterSrv_ctor(RepeaterSrv_t * const me) {
    cec_printf("RepeaterSrv_ctor ENTRY: \n");
	QHsm_ctor(&me->super, Q_STATE_CAST(&RepeaterSrv_initial));
}

QState RepeaterSrv_initial(RepeaterSrv_t *const me, QEvt const *const e)
{
    (void)e;/* avoid compiler warning about unused parameter */
    cec_printf("RepeaterSrv_initial iTE Entry: \n");

    /* iTE wakeup Initial setting*/
    //Power_Repeater_ITE_Init(RepeaterSrv_ITE_Init);
    //HDMI_Repeat_init(&me->stRepeatHandler);

	/* Setup Repeater initial state */
    me->TV_Switch_Source = 0;
    me->Repeater_power_state = eRepeater_Power_off;
    me->Mute_state = eMute_off;
	me->eARC_state = eRepeater_ARCSRC;
	me->RepeaterSrc = eRepeater_Src_Other;

    /* Interrupt init */
    RegIntrCallback(me, RepeaterSrv_Intr_Callback_func0);// register cb function for repeater intr
    IOP_RepeaterIntrPollingStart();// iop polling gpio, and send intr

	if (IT66322_SysInit()) {
		//RepeaterSrv_UserSwitch(0);
		if (gCurSysStatus<2) {
			gCurSysStatus = 2;
		}
	}

    return Q_TRAN(&RepeaterSrv_Home);
}
//****************************************************************************
void DumpAllBankReg(iTE_u8 u8Adr)
{
	iTE_u8	i, u8RegData[0x10];
	for(i=0; i < 0x10; i++){
		if((i == 0x03) && ((u8Adr == TX0_ADR) || (u8Adr == TX1_ADR) || (u8Adr == TX2_ADR))){
			iTE_I2C_ReadBurst(u8Adr, 0x31, 0x0F, &u8RegData[1]);
		}else if((u8Adr == eARCRxAddr) && (i == 0x04)){
			iTE_I2C_ReadBurst(u8Adr, 0x40, 0x05, u8RegData);
			iTE_I2C_ReadBurst(u8Adr, 0x46, 0x0A, &u8RegData[0x06]);
		}else{
			iTE_I2C_ReadBurst( u8Adr, i << 4, 0x10, u8RegData);
		}
		cec_printf("0x%02X: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X\r\n",
			i<<4, u8RegData[0], u8RegData[1], u8RegData[2], u8RegData[3], u8RegData[4], u8RegData[5], u8RegData[6], u8RegData[7], u8RegData[8], u8RegData[9], u8RegData[10], u8RegData[11], u8RegData[12], u8RegData[13], u8RegData[14], u8RegData[15] );
	}
}
//****************************************************************************
void ChangeBank(iTE_u8 u8Adr, iTE_u8 u8Mask, iTE_u8 u8Bank)
{
	iTE_u8	u8RegData;
	iTE_I2C_ReadBurst(u8Adr, 0x0F, 1, &u8RegData);
	u8RegData &= u8Mask;
	u8RegData |= u8Bank;
	iTE_I2C_ReadBurst(u8Adr, 0x0F, 1, &u8RegData);
}
//****************************************************************************
iTE_u1 HoldSystem(void)
{
	static iTE_u8	u8DumpReg = 0;
	if(1){
		if(u8DumpReg){
#if 1
			iTE_u8	u8Cnt1;

			for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
				cec_printf("HDMI_Sw Bank%d:\r\n", u8Cnt1);
				ChangeBank(SW_ADR, 0xFC, u8Cnt1);
				DumpAllBankReg(SW_ADR);
			}
			ChangeBank(SW_ADR, 0xFC, 0x00);


			for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
				cec_printf("HDMI_Rx0 Bank%d:\r\n", u8Cnt1);
				ChangeBank(RX0_ADR, 0xF8, u8Cnt1);
				DumpAllBankReg(RX0_ADR);
			}
			ChangeBank(RX0_ADR, 0xF8, 0x00);

			for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
				cec_printf("HDMI_Rx1  Bank%d:\r\n", u8Cnt1);
				ChangeBank(RX1_ADR, 0xF8, u8Cnt1);
				DumpAllBankReg(RX1_ADR);
			}
			ChangeBank(RX1_ADR, 0xF8, 0x00);
			#ifndef IT66322
			for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
				cec_printf("HDMI_Rx2  Bank%d:\r\n", u8Cnt1);
				ChangeBank(RX2_ADR, 0xF8, u8Cnt1);
				DumpAllBankReg(RX2_ADR);
			}
			ChangeBank(RX2_ADR, 0xF8, 0x00);

			for(u8Cnt1 = 0; u8Cnt1 < 4; u8Cnt1++){
				cec_printf("HDMI_Rx3  Bank%d:\r\n", u8Cnt1);
				ChangeBank(RX3_ADR, 0xF8, u8Cnt1);
				DumpAllBankReg(RX3_ADR);
			}
			ChangeBank(RX3_ADR, 0xF8, 0x00);
			#endif
			#ifdef IT66322
			cec_printf("HDMI_Tx0 :\r\n");
				DumpAllBankReg(TX0_ADR);
			for(u8Cnt1 = 0; u8Cnt1 < 2; u8Cnt1++){
				cec_printf("eARC Bank%d:\r\n", u8Cnt1);
				ChangeBank(eARCRxAddr, 0xFC, u8Cnt1);
				DumpAllBankReg(eARCRxAddr);
			}
			ChangeBank(eARCRxAddr, 0xFC, 0x00);
			#else
			cec_printf("HDMI_Tx0 :\r\n");
				DumpAllBankReg(TX0_ADR);

			cec_printf("HDMI_Tx1 :\r\n");
				DumpAllBankReg(TX1_ADR);

			cec_printf("HDMI_Tx2 :\r\n");
				DumpAllBankReg(TX2_ADR);
			#endif
#endif
			u8DumpReg = 0;

		}
			cec_printf("Hold\\\r");
			cec_printf("Hold-\r");
			cec_printf("Hold/\r");
			cec_printf("Hold|\r");

		return iTE_TRUE;
	}else{
		u8DumpReg = 1;
		return iTE_FALSE;
	}
}
//****************************************************************************

QState RepeaterSrv_Home(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            cec_printf("RepeaterSrv_home ENTRY: \n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            cec_printf("RepeaterSrv_home EXIT: \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            cec_printf("RepeaterSrv_home INIT: \n");
			status = Q_TRAN(&RepeaterSrv_inactive);
            break;
        }

		case REPEATER_EX_INTR_0_SIG: {
			cec_printf("REPEATER_EX_INTR_0_SIG  gCurSysStatus=%x\n",gCurSysStatus);

            me->EX_INTR_New= GetMSFromSTC();
			/* Set INTR events*/
			me->EX_INTR_last = GetMSFromSTC();
            /* update cec state*/
#if 1
			switch (gCurSysStatus)
			{
				case 0x01:
					if (IT66322_SysInit()) {
						//RepeaterSrv_UserSwitch(0);
						if (gCurSysStatus<2) {
							gCurSysStatus = 2;
						}
					}
					break;
				case 0x02:
#if 1
					if (u8TEST==60) {
						u8TEST = 70;
						RepeaterSrv_SetAudFmt(&me->stRepeatHandler);
					}
					else if (u8TEST<60) {
						u8TEST++;
					}
#endif
					//if (g_u8Cmd||g_u8IntEvent) {
						//g_u8IntEvent = 0;
						IT66322_SysIrq();
						SB_I2cIrq();
						if (g_u8Cmd) {
							cec_printf("HostControl:0x%02x\r\n",g_u8Cmd);
							SB_HostControl(g_u8Cmd);
							g_u8Cmd = 0;
						}
						if (IT66322_INT_Status()||g_u8I2cSlaveUpdateB) {
							cec_printf("IT66322_INT_Status g_u8I2cSlaveUpdateB=%x\n",g_u8I2cSlaveUpdateB);
							//g_u8IntEvent = 1;
							QEvt *e = Q_NEW(QEvt, REPEATER_EX_INTR_0_SIG);
							QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);
						}
					//}
					break;
				default:
					gCurSysStatus = 1;
					break;
			}
#endif


            status = Q_HANDLED();
            break;
        }

		case REPEATER_SRC_SWITCH_SIG: {
            cec_printf(" REPEATER_SRC_SWITCH_SIG: \n");

            RepeaterSrcEvt_t* stSrcEvt;
            stSrcEvt = (RepeaterSrcEvt_t*)e;
            me->RepeaterSrc = stSrcEvt->src_sel;
            /* Setup iTE switch src register Map */
			/* Setup iTE switch source check point */

			if ((stSrcEvt->src_sel) < eRepeater_Src_Other)
			{
				status = Q_TRAN(RepeaterSrv_active);
            }
			else
			{
				status = Q_TRAN(RepeaterSrv_inactive);
			}
            break;
        }

		case REPEATER_VOL_SET_SIG:{
			UINT8 Vol;
			RepeaterVolEvt_t *VolEvt = (RepeaterVolEvt_t*) e;
			Vol = VolEvt->vol;

			/* Setup iTE mute check point */
			/* Setup iTE Volume control */

			status = Q_HANDLED();
			break;
		}

		case REPEATER_MUTE_SET_SIG:{
			RepeaterMuteEvt_t *MuteEvt = (RepeaterMuteEvt_t*) e;

			/* Setup iTE mute check point */
			if(MuteEvt->mute != me->Mute_state)
			{
				me->Mute_state = MuteEvt->mute;
				if (MuteEvt->mute == eMute_on)
				{
				}
				else
				{
				}
			}
			status = Q_HANDLED();
			break;
		}

		case REPEATER_AUDIO_PATH_SET_SIG:{
			RepeaterAudPathEvt_t *Aud_path = (RepeaterAudPathEvt_t*) e;

			/* Setup iTE audio path check point */
			if (Aud_path->Aud_path == eRepeater_Audio_System)
			{
			}
			else
			{
			}

			status = Q_HANDLED();
			break;
		}

        case REPEATER_AUD_INFO_SEND_REQ_SIG: {
            cec_printf("REPEATER_AUD_INFO_SEND_REQ_SIG: \n");

			/* Setup iTE audio info */
            UINT8 pData[2];
            UINT8 SrcData = 0;
            //pData[0] = eUserCmdAudCtrl_HBR;
            //pData[1] = unReg0x22.HBR_ADO;

			if(pData[1] == 1)
	        {
	            SrcData = Source_HDMIHBR;
				if( SrcData !=  Userapp_Get_Source_id())
				{
					#ifdef MCU_IF_FEATURE
					SendDataBySerial(&SrcData, eUserCmdSrc, 1);
					#endif
					Userapp_Set_Source_id(SrcData);
				}
	        }
	        else
	        {
	            SrcData = Source_HDMISTD;
				if( SrcData !=  Userapp_Get_Source_id())
				{
					#ifdef MCU_IF_FEATURE
					SendDataBySerial(&SrcData, eUserCmdSrc, 1);
					#endif
					Userapp_Set_Source_id(SrcData);
				}
	        }

			/* Setup iTE setting */
            //pData[0] = eUserCmdAudCtrl_Samplerate;
            //pData[1] = pSamplerateMap[unReg0x22.SAMP_FREQ];
            //pData[0] = eUserCmdAudCtrl_ChNum;
            //pData[1] = unReg0x29.CC + 1;

            #ifdef MCU_IF_FEATURE
            SendDataBySerial(pData, eUserCmdAudInfo, 2);
            #endif

            status = Q_HANDLED();
            break;
        }

        case REPEATER_AUD_INFO_PRINTF_SIG:{
            cec_printf("HDMI repeater info\n");

            /* iTE needed print information */

            status = Q_HANDLED();
            break;
        }

        case REPEATER_POWER_DOWN_SIG:{
			cec_printf("REPEATER_POWER_DOWN_SIG!");

            /* setup iTE power down check point */
			me->Repeater_power_state = eRepeater_Power_off;
            RepeaterSrv_Standby();

            status = Q_HANDLED();
            break;
        }

        case REPEATER_POWER_UP_SIG:{
			cec_printf("REPEATER_POWER_UP_SIG!");

			/* setup iTE power up check point */

            status = Q_HANDLED();
            break;
        }

        case REPEATER_REG_PRINTF_SIG:{
			/* print all iTE regs */

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

QState RepeaterSrv_active(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            cec_printf("RepeaterSrv_active ENTRY; \n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            cec_printf("RepeaterSrv_active EXIT; \n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            cec_printf("RepeaterSrv_active INIT; \n");
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

QState RepeaterSrv_inactive(RepeaterSrv_t * const me, QEvt const * const e) {
    QState status;
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            cec_printf("RepeaterSrv_inactive ENTRY:\n");
            status = Q_HANDLED();
            break;
        }
		case Q_EXIT_SIG: {
            cec_printf("RepeaterSrv_inactive EXIT:\n");
            status = Q_HANDLED();
            break;
        }
        case Q_INIT_SIG: {
            cec_printf("RepeaterSrv_inactive INIT:\n");

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

void RepeaterSrv_Intr_Callback_func0(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_EX_INTR_0_SIG);
    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);
    cec_printf("RepeaterSrv_Intr_Callback_func0:\n");
}

int RepeaterSrv_PowerUp(void)
{
    QEvt *e = Q_NEW(QEvt, REPEATER_POWER_UP_SIG);
    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);
    return 0;
}

int RepeaterSrv_UserPowerUp(RepeaterWakeup_t *Repeater_State)
{
	cec_printf("================== RepeaterSrv_UserPowerUp=================\n");

	//volatile int *ITE_Wakeup_Flag = (int * )SRAM_ADDR_ITE_WAKE_UP_FLAG;
	volatile int *Repeater_saved_src = (int * )SRAM_ADDR_REPEATER_SAVED_SRC;

	//Repeater_State->power_state = *ITE_Wakeup_Flag;
	Repeater_State->Source= *Repeater_saved_src;

	//cec_printf("Repeater_State->power_state :%d; \n",Repeater_State->power_state);
	cec_printf("Repeater_State->Source :%d; \n",Repeater_State->Source);

	cec_printf("================== RepeaterSrv_UserPowerUp=================\n");

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

int RepeaterSrv_AudOutput(eRepeater_AudioPath_t audio_output)
{
    RepeaterAudOutputEvt_t *e = Q_NEW(RepeaterAudOutputEvt_t, REPEATER_MUTE_AUDIO_OUTPUT_SIG);
	e->Aud_output =  audio_output;
    QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);
    return 0;
}

int RepeaterSrv_SrcSwitch(eRepeater_Src_t src_sel)
{
	cec_printf("Userapp RepeaterSrv_SrcSwitch:\n");
    RepeaterSrcEvt_t *e = Q_NEW(RepeaterSrcEvt_t, REPEATER_SRC_SWITCH_SIG);
    e->src_sel = src_sel;
    QACTIVE_POST(DisplaySrv_get(), (QEvt *)e, (void*)0);
    return 0;
}

int RepeaterSrv_UserSwitch(eRepeater_Src_t src)
{
	cec_printf("RepeaterSrv_UserSwitch %d ********\n", src);
	QState status;
    RepeaterSrcEvt_t *e = Q_NEW(RepeaterSrcEvt_t, REPEATER_USER_SWITCHSRC_SIG);
    switch(src){
        case  eRepeater_Src_HDMI_P0:
				cec_printf("eRepeater_Src_HDMI_P0:\n");
				e->src_user = eRepeater_userSrc_HDMI_P0;
		break;
		case  eRepeater_Src_HDMI_P1:
			    cec_printf("eRepeater_Src_HDMI_P1:\n");
				e->src_user = eRepeater_userSrc_HDMI_P1;
		break;
		case  eRepeater_Src_HDMI_P2:
			    cec_printf("eRepeater_Src_HDMI_P2:\n");
				e->src_user= eRepeater_userSrc_HDMI_P2;
		break;
		case  eRepeater_Src_ARC:
			    cec_printf("eRepeater_Src_ARC: %d\n",eRepeater_uesrSrc_ARC);
				e->src_user = eRepeater_uesrSrc_ARC;
		break;
		case eRepeater_Src_Other:
        default:
			cec_printf("Uerswitch error!!!\n");
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
	UINT8 New_dspfifo;
	cec_printf("RepeaterSrv_SetAudFmt********\n");
	/* Setup iTE Audio format */
	if (0)
	{
		New_dspfifo =  Source_HDMIHBR;
	}
	else
	{
		New_dspfifo =  Source_HDMISTD;
	}

	if(pstRepeatHandler ->DSP_AUD_FIFO != New_dspfifo )
	{
		RepeaterAudSrcEvt_t *RepeaterAudSrcEvt;
		RepeaterAudSrcEvt = Q_NEW(RepeaterAudSrcEvt_t,REPEATER_AUD_SRC_SIG);
		RepeaterAudSrcEvt->Aud_I2S_Lane = New_dspfifo;
		QACTIVE_POST(UserApp_get(), (QEvt *)RepeaterAudSrcEvt, (void*)0);
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
	//e->Arc_status = Arc_status;

    cec_printf("Hpd_status:%d Arc_status:%d\n", Hpd_status, Arc_status);
    QACTIVE_POST(UserApp_get(), (QEvt *)e, (void*)0);

    return 0;
}

int RepeaterSrv_WakeUpCheck(UINT8 *HPD_State,UINT8 *Repeater_State)
{
	(void) *HPD_State;
	(void) *Repeater_State;
    int iRes = 0;
    //UINT8 pBuf[10];
    //extern RepeaterSrv_t l_Repeater;
	cec_printf("RepeaterSrv_UserPowerUp:\n");

	/* Setup iTE wakeup info
	volatile int *EP_Wakeup_Flag = (int * )SRAM_ADDR_EP_WAKE_UP_FLAG;
	volatile int *EP_Wakeup_Source = (int * )SRAM_ADDR_WAKE_UP_SRC;

	*HPD_State = unEP_BasicInfo_reg0x08.HPD;
	*Repeater_State = unEP_GenCtrl_reg0x11.Primary_Sel;

	switch(Repeater_State){
		case  eRepeater_Src_HDMI_P0:
			cec_printf("eRepeater_Src_HDMI_P0:\n");
			*Repeater_State = eRepeater_userSrc_HDMI_P0;
		break;
		case  eRepeater_Src_HDMI_P1:
				cec_printf("eRepeater_Src_HDMI_P1:\n");
				*Repeater_State = eRepeater_userSrc_HDMI_P1;
		break;
		case  eRepeater_Src_HDMI_P2:
				cec_printf("eRepeater_Src_HDMI_P2:\n");
				*Repeater_State = eRepeater_userSrc_HDMI_P2;
		break;
		case  eRepeater_Src_ARC:
				cec_printf("eRepeater_Src_ARC: %d\n",eRepeater_uesrSrc_ARC);
				*Repeater_State = eRepeater_uesrSrc_ARC;
		break;
		case eRepeater_Src_Other:
		default:
			*Repeater_State = eRepeater_userSrc_Other;
		break;
	}*/

	/* Setup iTE CEC wakeup check point
	if ( == 1)
    {
        if (pBuf[0] == EP_CEC_EVT_AUD_MODE)
        {
            iRes = 1;
			*ITE_Wakeup_Flag = 1;
			*ITE_Wakeup_Source = Primary_Sel;
            cec_printf("CEC wake up\n");
        }
    }*/

	/* Setup iTE power wakeup check point
	if (u == 1)
	{
		iRes = 1;
		cec_printf("power bit = 1 && Audio_path = 1! \n");
	}*/

    return iRes;
}

int RepeaterSrv_Standby(void)
{
    GPIO_DeInitIntr(eIntr_0);
    return 0;
}

int RepeaterSrv_ITE_Init(void)
{
    //extern RepeaterSrv_t l_Repeater;
	//l_Repeater.stRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
	//l_Repeater.stRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;

	/* Setup iTE initial settings */

    return 0;
}

int RepeaterSrv_SourceIDChk(RepeaterSrcState_t *repeaterstatus)
{
    //stHdmiRepeatHandler_t pstRepeatHandler;
    //pstRepeatHandler.pCtlStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
    //pstRepeatHandler.SlaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;

	/* Check iTE play fotmat and state */
    UINT8 New_dspfifo;

    if (1)
    {
        New_dspfifo = Source_HDMIHBR;
    } else
    {
        New_dspfifo = Source_HDMISTD;
    }
    repeaterstatus->Source_ID = New_dspfifo;
    return 0;
}

int RepeaterSrv_WakeupCheckHandler(void)
{
    //UINT8 buf;
    //UINT8 *i2cStr = GET_SDK_CFG(CFG_CEC_T)->pbI2C_DevStr;
    //I2cData_t i2cData;
	//volatile int *ITE_Wakeup_Flag = (int * )SRAM_ADDR_ITE_WAKE_UP_FLAG;
	//volatile int *Sys_memory_src = (int * )SRAM_ADDR_SYS_MEMORY_SRC;

	/* Setup iTE i2c info
    i2cData.slaveAddr = GET_SDK_CFG(CFG_CEC_T)->repeaterAddr;
    i2cData.buf = &epReg0x10.DataByte;
    i2cData.cmd = EP_ADDR_GEN_CTRL_0x10;
    i2cData.cmdLen = 1;
    i2cData.dataLen = 1;
    i2cData.showMsg = 0;
    if(I2C_Read(i2cStr, &i2cData) == FALSE)
    {
        LOGX("i2c read fail...\n");
        return -1;
    }*/

	/* Setup iTE reg source
	switch(iTE.DataByte)
	{
		case  eRepeater_Src_HDMI_P0:
		{
			*Sys_memory_src  = eRepeater_userSrc_HDMI_P0;
			break;
		}
		case  eRepeater_Src_HDMI_P1:
		{
			*Sys_memory_src  = eRepeater_userSrc_HDMI_P1;
			break;
		}
		case  eRepeater_Src_HDMI_P2:
		{
			*Sys_memory_src  = eRepeater_userSrc_HDMI_P2;
			break;
		}
		case  eRepeater_Src_ARC:
		{
			*Sys_memory_src  = eRepeater_uesrSrc_ARC;
			break;
		}
		case  eRepeater_Src_Other:
		{
			*Sys_memory_src  = eRepeater_Src_Other;
			break;
		}
	}*/

	/* Check iTE CEC event
	*ITE_Wakeup_Flag = Power;

    if(epReg0x10.Power == 1)
    {
		return TRUE;
    }

    if(I2C_Read(i2cStr, &i2cData) == FALSE)
    {
        LOGX("i2c read fail...\n");
        return -1;
    }

	if (buf == EP_CEC_EVT_AUD_MODE)
    {
		*ITE_Wakeup_Flag = ;
        return TRUE;
    }*/

    return FALSE;
}

int RepeaterSrv_WakeupFlash(void)
{
	//volatile int *ITE_Wakeup_Flag = (int * )SRAM_ADDR_ITE_WAKE_UP_FLAG;
	volatile int *Repeater_saved_src = (int * )SRAM_ADDR_REPEATER_SAVED_SRC;
	//*ITE_Wakeup_Flag = 0;
	*Repeater_saved_src = 0;
	return 0;
}
