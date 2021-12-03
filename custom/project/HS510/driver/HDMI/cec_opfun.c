/******************************************************************************

   Copyright (c) 2016 Sunplus technology co. Ltd.All right reserved!

******************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/

//#include "lbc.h"
//#include "iop.h"
#include "cec_user_ctl.h"
#include "types.h"
#include "cec.h"
#include "cec_main.h"
#include "cec_opfun.h"
#include "cec_cmd.h"



UINT8 UiCmdSupportedChk(stCecHandler *pstHandler)
{
	UINT8 mbOprandSupported = 0;
	//If many customer want to use simple CEC, please make each "switch case" by Function.
	if (pstHandler->SysSrc == eSysSrc_ARC)
	{
		switch (pstHandler->Rx.Operand[0])
		{
			case UI_CMD_POWER:
			case UI_CMD_VOLUME_UP:
			case UI_CMD_VOLUME_DOWN:
			case UI_CMD_MUTE:
			case UI_CMD_POWER_ON_FUNCT:
			case UI_CMD_MUTE_FUNCT:
			case UI_CMD_UNMUTE_FUNCT:
			case UI_CMD_POWER_OFF_FUNCT:
			case UI_CMD_POWER_TOGGLE_FUNCT:
				mbOprandSupported = 1;
				break;

			default:
				mbOprandSupported = 0;
				break;
		}
	}
	else if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
	{
		switch (pstHandler->Rx.Operand[0])
		{
			case UI_CMD_POWER:
			case UI_CMD_POWER_ON_FUNCT:
			case UI_CMD_POWER_OFF_FUNCT:
			case UI_CMD_POWER_TOGGLE_FUNCT:
				mbOprandSupported = 1;
				break;

			default:
				mbOprandSupported = 0;
				break;
		}
	}
	else if (pstHandler->SysSrc == eSysSrc_Others)
	{
		switch (pstHandler->Rx.Operand[0])
		{
			case UI_CMD_POWER:
			case UI_CMD_VOLUME_UP:
			case UI_CMD_VOLUME_DOWN:
			case UI_CMD_MUTE:
			case UI_CMD_POWER_ON_FUNCT:
			case UI_CMD_POWER_OFF_FUNCT:
			case UI_CMD_POWER_TOGGLE_FUNCT:
				mbOprandSupported = 1;
				break;

			default:
				mbOprandSupported = 0;
				break;
		}
	}
	return mbOprandSupported;
}

void GoAbort(stCecHandler *pstHandler, UINT8 AbortReason) //SpikeAdd 20080523 modify name to avoid same
{
	//dont reply for BROADCAST
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
	{
		pstHandler->FeatureAbort.DestAddr = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
		pstHandler->FeatureAbort.AbortOpcode = pstHandler->Rx.Opcode;
		pstHandler->FeatureAbort.AbortReason = AbortReason;
		Cec_CmdListInsert(pstHandler, CEC_CMD_FEATURE_ABORT);
	}
}

void OPC_UserCtrlPressed(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == (CEC_LOG_ADDR_BROADCAST << 4))
	{
		cec_printf("user control cmd unsupport L15...\n");
		return;
	}

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
	{
		cec_printf("user control cmd unsupport broadcast...\n");
		return;
	}

	if (!UiCmdSupportedChk(pstHandler))
	{
		cec_printf("[CEC][Customer]Platform Unsupport this CEC Oprand(User Control)!\n");
		GoAbort(pstHandler, ABORT_REASON_INVALID_OPERAND);
		return;
	}

	//CecKeyCnt only correct in normal,incorrect in standby
	pstHandler->CecKeyCnt = (USER_KEY_CMD_TIMEOUT / pstHandler->BusBusyChk.Cec_PollingTime);

	switch (pstHandler->Rx.Operand[0])
	{
		case UI_CMD_SELECT:
			cec_printf2("UI_CMD_SELECT\n");
			break;

		case UI_CMD_UP:
			cec_printf2("UI_CMD_UP\n");
			break;

		case UI_CMD_DOWN:
			cec_printf2("UI_CMD_DOWN\n");
			break;

		case UI_CMD_LEFT:
			cec_printf2("UI_CMD_LEFT\n");
			break;

		case UI_CMD_RIGHT:
			cec_printf2("UI_CMD_RIGHT\n");
			break;

		case UI_CMD_ROOT_MENU:
			cec_printf2("UI_CMD_ROOT_MENU\n");
			break;

		case UI_CMD_SETUP_MENU:
			cec_printf2("UI_CMD_SETUP_MENU\n");
			break;

		case UI_CMD_CONTENTS_MENU:
			cec_printf2("UI_CMD_CONTENTS_MENU\n");
			break;

		case UI_CMD_EXIT:
			cec_printf2("UI_CMD_EXIT\n");
			break;

		case UI_CMD_NUMBER_0:
			cec_printf2("UI_CMD_NUMBER_0\n");
			break;

		case UI_CMD_NUMBER_1:
			cec_printf2("UI_CMD_NUMBER_1\n");
			break;

		case UI_CMD_NUMBER_2:
			cec_printf2("UI_CMD_NUMBER_2\n");
			break;

		case UI_CMD_NUMBER_3:
			cec_printf2("UI_CMD_NUMBER_3\n");
			break;

		case UI_CMD_NUMBER_4:
			cec_printf2("UI_CMD_NUMBER_4\n");
			break;

		case UI_CMD_NUMBER_5:
			cec_printf2("UI_CMD_NUMBER_5\n");
			break;

		case UI_CMD_NUMBER_6:
			cec_printf2("UI_CMD_NUMBER_6\n");
			break;

		case UI_CMD_NUMBER_7:
			cec_printf2("UI_CMD_NUMBER_7\n");
			break;

		case UI_CMD_NUMBER_8:
			cec_printf2("UI_CMD_NUMBER_8\n");
			break;

		case UI_CMD_NUMBER_9:
			cec_printf2("UI_CMD_NUMBER_9\n");
			break;

		case UI_CMD_ENTER:
			cec_printf2("UI_CMD_ENTER\n");
			break;

		case UI_CMD_SOUND_SELECT:
			cec_printf2("UI_CMD_SOUND_SELECT\n");
			break;

		case UI_CMD_DISPLAY_INFOMATION:
			cec_printf2("UI_CMD_DISPLAY_INFOMATION\n");
			break;

		case UI_CMD_POWER:
			cec_printf2("UI_CMD_POWER\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_POWER;
			break;

		case UI_CMD_VOLUME_UP:
			cec_printf2("UI_CMD_VOLUME_UP\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_VOLUME_UP;
			break;

		case UI_CMD_VOLUME_DOWN:
			cec_printf2("UI_CMD_VOLUME_DOWN\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_VOLUME_DN;
			break;

		case UI_CMD_MUTE:
			cec_printf2("UI_CMD_MUTE\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_MUTE;
			break;

		case UI_CMD_PLAY:
			cec_printf2("UI_CMD_PLAY\n");
			break;

		case UI_CMD_STOP:
			cec_printf2("UI_CMD_STOP\n");
			break;

		case UI_CMD_PAUSE:
			cec_printf2("UI_CMD_PAUSE\n");
			break;

		case UI_CMD_REWIND:
			cec_printf2("UI_CMD_REWIND\n");
			break;

		case UI_CMD_FAST_FORWARD:
			cec_printf2("UI_CMD_FAST_FORWARD\n");
			break;

		case UI_CMD_EJECT:
			cec_printf2("UI_CMD_EJECT\n");
			break;

		case UI_CMD_FORWARD:
			cec_printf2("UI_CMD_FORWARD\n");
			break;

		case UI_CMD_BACKWARD:
			cec_printf2("UI_CMD_BACKWARD\n");
			break;

		case UI_CMD_ANGLE:
			cec_printf2("UI_CMD_ANGLE\n");
			break;

		case UI_CMD_SUB_PICTURE:
			cec_printf2("UI_CMD_SUB_PICTURE\n");
			break;

		case UI_CMD_MUTE_FUNCT:
			cec_printf2("UI_CMD_MUTE_FUNCT\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_MUTE_ON;
			break;

		case UI_CMD_UNMUTE_FUNCT:
			cec_printf2("UI_CMD_UNMUTE_FUNCT\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_MUTE_OFF;
			break;

		case UI_CMD_POWER_TOGGLE_FUNCT:
			cec_printf2("UI_CMD_POWER_TOGGLE_FUNCT\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_POWER;
			break;

		case UI_CMD_POWER_OFF_FUNCT:
			cec_printf2("UI_CMD_POWER_TOGGLE_FUNCT\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_POWER_OFF;
			break;

		case UI_CMD_POWER_ON_FUNCT:
			cec_printf2("UI_CMD_POWER_TOGGLE_FUNCT\n");
			pstHandler->CecKeyCode = USER_KEY_CMD_POWER_ON;
			break;
		case UI_CMD_SEL_AUD_INPIT_FUN:
			cec_printf2("UI_CMD_SEL_AUD_INPUT_FUN\n");
			break;

		default:
			cec_printf("UI_CMD is unsupported\n");
			GoAbort(pstHandler, ABORT_REASON_INVALID_OPERAND);
			break;
	}
}

void OPC_UserCtrlReleased(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;
	pstHandler->UserCtrlReleased = CEC_Status_READY;
}

void OPC_GiveSysAudModeSts(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;
	cec_printf("OPC_GiveSysAudModeSts\n");

	pstHandler->SysAudCtrl.DestAddr[2] = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
	Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_SYS_AUDIO_MODE_STATUS);
}

void OPC_GiveAudStatus(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;

	pstHandler->SysAudCtrl.DestAddr[1] = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
	Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_AUDIO_STATUS);
}

void OPC_SysAudModeRequest(stCecHandler *pstHandler)
{
	cec_printf("Rx.MsgLen=%d \n", pstHandler->Rx.MsgLen);
	pstHandler->CEC_state_change = Cec_Change_ON;
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.MsgLen != 2) && (pstHandler->Rx.MsgLen != 4))
		return;

	if ((pstHandler->CustVar.CecAlwaysRun ==CEC_STS_INACTIVE) &&(pstHandler->CecEnable == CEC_STS_INACTIVE))
		return;

	if (pstHandler->Rx.MsgLen == 2)
	{
		cec_printf("SysAudCtrl off\n");
		pstHandler->SysAudCtrl.TvSupportSac = 0;
		Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_INACTIVE);
		pstHandler->SysAudCtrl.MuteState = 1;
#ifdef CEC_CTS_TEST
		pstHandler->SysAudCtrl.MuteState = 1;
		cec_printf("SysAudCtrl.MuteState = %d\n", pstHandler->SysAudCtrl.MuteState);
#endif
	}
	else
	{
		//(((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK)>>4)==CEC_LOG_ADDR_STB)  this is Test 11.2.15_1 test CEC_LOG_ADDR_STB
		if ((((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4) == CEC_LOG_ADDR_TV))// || (((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4) == CEC_LOG_ADDR_STB))
		{
			pstHandler->SysAudCtrl.TvSupportSac = 1;
			pstHandler->SysAudCtrl.MuteState = 0; //1115
			pstHandler->CEC_SourceState = CEC_STS_ACTIVE;
		}
		else
		{
			pstHandler->SysAudCtrl.TvSupportSac = 0;
			pstHandler->SysAudCtrl.MuteState = 1;
			cec_printf("OPC_SysAudModeRequest TvSupportSac = 0\n");
		}
		pstHandler->SysAudCtrl.ActiveSrcPhyAddrAB = pstHandler->Rx.Operand[0];
		pstHandler->SysAudCtrl.ActiveSrcPhyAddrCD = pstHandler->Rx.Operand[1];
		Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_ACTIVE);
	}
}

void OPC_RequestShortAudDescriptor(stCecHandler* pstHandler)
{
	if((pstHandler->Rx.Header&CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;

    if (pstHandler->Rx.MsgLen == 2)// mantis: 274180
        return;

	UINT8 mbLength = pstHandler->Rx.MsgLen-2;
	UINT8 i,j,mbAudioFormatCode=0;
	UINT8 *pBuf = &(pstHandler->Rx.Operand[0]);

	pstHandler->SysAudCtrl.AudFormatRec = 0;
	for(i = 0; i<mbLength; i++)
	{
		if(((pBuf[i]&0xc0)>>6) ==0)
		{
			mbAudioFormatCode = (pBuf[i]&0x3f);
			for (j = 0;j < pstHandler->CustVar.ShortAudDescriptorNum; j++)
			{
				if( mbAudioFormatCode == pstHandler->CustVar.ShortAudDescriptor[j])
				{
                   break;
				}
				else if( (mbAudioFormatCode != pstHandler->CustVar.ShortAudDescriptor[j])&& (j == (pstHandler->CustVar.ShortAudDescriptorNum - 1)))
				{
					mbAudioFormatCode = AFC_UNUSE;
				}
			}
			switch(mbAudioFormatCode)
			{
				case AFC_LPCM:
					cec_printf("TV support audio format:AFC_LPCM................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_AC3:
					cec_printf("TV support audio format:AFC_AC3................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_MPEG1:
					cec_printf("TV support audio format:AFC_MPEG1................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_MP3:
					cec_printf("TV support audio format:AFC_MP3................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_MPEG2:
					cec_printf("TV support audio format:AFC_MPEG2................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_AAC:
					cec_printf("TV support audio format:AFC_AAC................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_DTS:
					cec_printf("TV support audio format:AFC_DTS................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;
				case AFC_DDP:
					cec_printf("TV support audio format:AFC_DDP................\n");
					pstHandler->SysAudCtrl.AudFormatRec |= 1<<mbAudioFormatCode;
					break;

				default:
					break;
			}
		}
	}

	if(!pstHandler->SysAudCtrl.AudFormatRec)
	{
		cec_printf("Error audio format,go abort................\n");
		GoAbort(pstHandler,ABORT_REASON_INVALID_OPERAND);
	}
	else
	{
		cec_printf("Got TV support audio format!\n");
		pstHandler->SysAudCtrl.DestAddr[3] = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK)>>4);
		Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_SHORT_AUDIO_DESCRIPTOR);
	}

}

void OPC_GiveOsdName(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
	{
		return;
	}
	if ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == (CEC_LOG_ADDR_BROADCAST << 4))
	{
		return;
	}
	pstHandler->DevOsdNameXfe.DestAddr = (pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4; //set who want the name
	Cec_CmdListInsert(pstHandler, CEC_CMD_SET_OSD_NAME);
}

void OPC_GiveDevVendorId(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
	{
		return;
	}
	Cec_CmdListInsert(pstHandler, CEC_CMD_DEVICE_VENDOR_ID);
}

void OPC_FeatureAbort(stCecHandler *pstHandler)
{
	if ((pstHandler->SysAudCtrl.RequestSacTimeout > 0) && (pstHandler->SysAudCtrl.FlowStep == 3)
			&& (pstHandler->Rx.Operand[0] == CEC_OPCODE_SET_SYS_AUDIO_MODE)
			&& (pstHandler->Rx.Operand[1] == ABORT_REASON_UNRECOGNIZED_OPCODE)
	   )
	{
		pstHandler->SysAudCtrl.FlowStep = 0;
		pstHandler->SysAudCtrl.RequestSacTimeout = 0;
		pstHandler->SysAudCtrl.SysAudModeSts = 0;
		pstHandler->SysAudCtrl.TvSupportSac = 0;
	}
	else if (pstHandler->ArcCtrl.RequstArcTimeout > 0
			 && (pstHandler->Rx.Operand[0] == CEC_OPCODE_INITIATE_ARC)
			)
	{
		//refer to HDMI CEC V4.2 C4.2.1
		if ((pstHandler->Rx.Operand[1] == ABORT_REASON_UNRECOGNIZED_OPCODE) || (pstHandler->Rx.Operand[1] == ABORT_REASON_REFUSED))
		{
			//If TV replies with <Feature Abort> [unrecongnized opcode] or [refused], ARC set up considered failed
			pstHandler->ArcCtrl.RequstArcTimeout = 0;
			pstHandler->ArcCtrl.TvSupportArc = 0;
			cec_printf("FeatureAbort! TvSupportArc == 0\n");
		}
		else if (pstHandler->Rx.Operand[1] == ABORT_REASON_NOT_IN_CORRECT_MODE_TO_RESPOND)
		{
			//Even if TV replies with <FA>[incorrent mode to respond], HTS shall still treat TV as supporting ARC feature
			pstHandler->ArcCtrl.RequstArcTimeout = 0;
			pstHandler->ArcCtrl.TvSupportArc = 1;
			cec_printf("Not in correct mode to respone! TvSupportArc == 1\n");
		}
	}
}

void OPC_RequestArcInitiation(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;

	cec_printf("OPC_RequestArcInitiation PhyAddrAB: 0x%x, CD: 0x%x \n", pstHandler->PhyAddrAB, pstHandler->PhyAddrCD);

	if ((pstHandler->PhyAddrAB & 0x0f) != 0x0) //pingjun.yu modify for cec Test ID 11.2.17-5 is Fail @2012-12-20
	{
		GoAbort(pstHandler, ABORT_REASON_REFUSED);
	}
	else
	{
		pstHandler->ArcCtrl.TvSupportArc = 0;
		Cec_CmdListInsert(pstHandler, CEC_CMD_INITIATE_ARC);
	}
}

void OPC_RequestArcTermination(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;

	Cec_CmdListInsert(pstHandler, CEC_CMD_TERMINATE_ARC);
}

void OPC_ReportArcInitiated(stCecHandler *pstHandler)
{
	if (pstHandler->ArcCtrl.RequstArcTimeout > 0)
	{
		pstHandler->retryNum = 0;
		pstHandler->ArcCtrl.RequstArcTimeout = 0;
		pstHandler->ArcCtrl.TvSupportArc = 1;
		cec_printf("ReportArcInitiated! TvSupportArc == 1\n");
	}
}

void OPC_ReportArcTerminated(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
		return;

	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
		return;
	pstHandler->retryNum = 0;

	pstHandler->ArcCtrl.TvSupportArc = 0;
	cec_printf("ReportArcTerminated! TvSupportArc == 0\n");
}

void OPC_ReportPwrStatus(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Operand[0] == POWER_STATUS_STANDBY)
			&& ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == (CEC_LOG_ADDR_TV << 4)))
	{
		//cec_printf(">>>>>>>>>>>>>>>>>>> Send CEC <UCP><AS> <<<<<<<<<<<<<<<<<<<<<<");
		//Cec_CmdListInsert(pstHandler, CEC_CMD_ONLY_ACTIVE_SOURCE);
	}
	else if ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == (CEC_LOG_ADDR_TV << 4))
	{
		//cec_printf(">>>>>>>>>>>>>>>>>>> CEC None <<<<<<<<<<<<<<<<<<<<<<\n");
	}
}

void OPC_GiveDevPwrStatus(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
	{
		pstHandler->GiveDevPwrSts.DestAddr = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
		Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_POWER_STATUS);
	}
}

void OPC_Standby(stCecHandler *pstHandler)
{
	pstHandler->CECStandby = 1;

}

void OPC_SetAudMode(stCecHandler *pstHandler)
{
	if ((((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4) == CEC_LOG_ADDR_TV))
	{
		if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_AUDIO_SYSTEM)
			return;
		if (pstHandler->Rx.Operand[0] == CEC_SYS_Audio_ON)
		{
			pstHandler->SysAudCtrl.TvSupportSac = 0;

		}
		else
		{
			pstHandler->SysAudCtrl.TvSupportSac = 1;

		}
	}

}

void OPC_GivePhyAddr(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
	{
		Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_PHYSICAL_ADDRESS);
	}
}

void OPC_GetCecVersion(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
	{
		pstHandler->SysInfo.DestAddr = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4); // mantis: 274180
		Cec_CmdListInsert(pstHandler, CEC_CMD_CEC_VERSION);
	}
}

void OPC_SetStreamPath(stCecHandler *pstHandler)
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
	{
		return;
	}

	if ((pstHandler->Rx.Operand[0] == pstHandler->PhyAddrAB) && (pstHandler->Rx.Operand[1] == pstHandler->PhyAddrCD))
	{
		Cec_CmdListInsert(pstHandler, CEC_CMD_ACTIVE_SOURCE);
	}
}

void OPC_ActiveSource(stCecHandler *pstHandler) //spike 20080708
{
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
	{
		if (((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == (CEC_LOG_ADDR_BROADCAST << 4))
				|| ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) == CEC_LOG_ADDR_FREEUSE)
		   )
		{
			cec_printf("active source unsupport L15...\n");
			return;
		}
		if((pstHandler->Rx.Operand[0] != pstHandler->PhyAddrAB) &&
		(pstHandler->Rx.Operand[0] != pstHandler->PhyAddrAB))
		{

			pstHandler->CEC_state_change = Cec_Change_ON;
			pstHandler->CEC_SourceState = CEC_STS_ACTIVE;
		}
		if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
		{
			pstHandler->DeviceActive = CEC_STS_INACTIVE;
		}
		else
		{
			if (pstHandler->SysAudCtrl.SysAudModeSts)
			{
				pstHandler->SysAudCtrl.ActiveSrcPhyAddrAB = pstHandler->Rx.Operand[0];
				pstHandler->SysAudCtrl.ActiveSrcPhyAddrCD = pstHandler->Rx.Operand[1];
				cec_printf("active source address:%x,%x\n", pstHandler->Rx.Operand[0], pstHandler->Rx.Operand[1]);
				Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_ACTIVE);
				return;
			}
		}
	}
}

void OPC_RequestActiveSource(stCecHandler *pstHandler)
{
	//only for BROADCAST
	if ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) == CEC_LOG_ADDR_BROADCAST)
	{
		cec_printf("Device active = %d\n", pstHandler->DeviceActive);
		if (pstHandler->DeviceActive == CEC_STS_ACTIVE)
		{
			Cec_CmdListInsert(pstHandler, CEC_CMD_ACTIVE_SOURCE);
		}
	}
}


