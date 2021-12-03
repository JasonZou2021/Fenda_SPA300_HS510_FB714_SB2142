/******************************************************************************

   Copyright (c) 2016 Sunplus technology co. Ltd.All right reserved!

******************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/

//#include "lbc.h"
//#include "iop.h"

#include "types.h"

#include "cec_main.h"
#include "cec_cmd.h"
#include "cec_opfun.h"




#include "cec.h"

void CMD_OneTouchPlay(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_TV); //note: local_addrv should be the addr of hts
			pstHandler->Tx.Opcode = CEC_OPCODE_TEXT_VIEW_ON;
			pstHandler->Tx.MsgLen = 2;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST); //note: local_addrv should be the addr of hts
			pstHandler->Tx.Opcode = CEC_OPCODE_ACTIVE_SOURCE;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.MsgLen = 4;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP3;
			}
			break;

		case PROCESS_STEP3:
			cec_printf("PROCESS_STEP3..........\n");
			pstHandler->DeviceActive = CEC_STS_ACTIVE;
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_ActiveSource(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST); //note: local_addrv should be the addr of hts
			pstHandler->Tx.Opcode = CEC_OPCODE_ACTIVE_SOURCE;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.MsgLen = 4;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->DeviceActive = CEC_STS_ACTIVE;
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_InactiveSource(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_TV); //note: local_addrv should be the addr of hts
			pstHandler->Tx.Opcode = CEC_OPCODE_INACTIVE_SOURCE;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.MsgLen = 4;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->DeviceActive = CEC_STS_INACTIVE;
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_RequestActiveSrc(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST); //note: local_addrv should be the addr of hts
			pstHandler->Tx.Opcode = CEC_OPCODE_REQUEST_ACTIVE_SOURCE;
			pstHandler->Tx.MsgLen = 2;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_SetSysAudMode(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			cec_printf("SetSysAudModeDestAddr:%d,SysAudModeSts:%d\n", pstHandler->SysAudCtrl.DestAddr[0], pstHandler->SysAudCtrl.SysAudModeSts);
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->SysAudCtrl.DestAddr[0]);
			pstHandler->Tx.Opcode = CEC_OPCODE_SET_SYS_AUDIO_MODE;
			pstHandler->Tx.Operand[0] = pstHandler->SysAudCtrl.SysAudModeSts;
			pstHandler->Tx.MsgLen = 3;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			cec_printf("pstHandler->SysAudCtrl.FlowStep:%d\n", pstHandler->SysAudCtrl.FlowStep);
			if (pstHandler->SysAudCtrl.FlowStep)
			{
				if (pstHandler->SysAudCtrl.FlowStep == 4)
				{
					cec_printf("SysAudCtrl set on\n");
					pstHandler->SysAudCtrl.FlowStep++; //  5 means success
					pstHandler->SysAudCtrl.TvSupportSac = 1;
					pstHandler->SysAudCtrl.MuteState = CEC_SYS_DeMute;
#ifdef CEC_CTS_TEST
					pstHandler->SysAudCtrl.MuteState = CEC_SYS_DeMute;
					cec_printf("SysAudCtrl.MuteState = %d\n", pstHandler->SysAudCtrl.MuteState);
#endif
				}
				else if (pstHandler->SysAudCtrl.FlowStep == 2)
				{
					cec_printf("system audio mode FlowStep 2\n");
					pstHandler->SysAudCtrl.FlowStep++;
					pstHandler->SysAudCtrl.RequestSacTimeout = (CEC_REQ_SAC_TIMEOUT /pstHandler->BusBusyChk.Cec_PollingTime); //about .1s
				}
			}
			else
			{
				cec_printf("SysAudCtrl set off\n");
				pstHandler->SysAudCtrl.TvSupportSac = 0;
#ifdef CEC_CTS_TEST
				pstHandler->SysAudCtrl.MuteState = 1;
				cec_printf("SysAudCtrl.MuteState = %d\n", pstHandler->SysAudCtrl.MuteState);
#endif
			}
			#if 1
			if((pstHandler->SysAudCtrl.SysAudModeSts  == 1)&&(pstHandler->SysAudCtrl.TvSupportSac == 1))
			{
				pstHandler->CEC_SET_DSP_change = 1;
				pstHandler->CEC_SET_DSP_ON = 1;
			}
			#endif
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}


void CMD_ReportAudStatus(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->SysAudCtrl.DestAddr[1]);
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_AUDIO_STATUS;
			pstHandler->Tx.Operand[0] = ((pstHandler->SysAudCtrl.MuteState << 7) | (pstHandler->SysAudCtrl.Volume & 0x7f));
			cec_printf("Tx.Operand[0]:%2x\n", pstHandler->Tx.Operand[0]);
			pstHandler->Tx.MsgLen = 3;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_ReportSysAudModeStatus(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->SysAudCtrl.DestAddr[2]);
			pstHandler->Tx.Opcode = CEC_OPCODE_SYS_AUDIO_MODE_STATUS;
			pstHandler->Tx.Operand[0] = pstHandler->SysAudCtrl.SysAudModeSts;
			pstHandler->Tx.MsgLen = 3;
			cec_printf("pstHandler->Tx.Opcode=%2x\n", pstHandler->Tx.Opcode);
			cec_printf("pstHandler->Tx.Operand[0]=%2x\n", pstHandler->Tx.Operand[0]);
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_InitiateArc(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			if ((pstHandler->PhyAddrAB & 0x0f) != 0x0) //pingjun.yu modify for cec Test ID 11.2.17-5 is Fail @2012-12-21
			{
				cec_printf("None adjacent ARC Rx & Tx!\n");
				pstHandler->Cmd.Id = CEC_CMD_IDLE;
				break;
			}

			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_TV);
			pstHandler->Tx.Opcode = CEC_OPCODE_INITIATE_ARC;
			pstHandler->Tx.MsgLen = 2;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->ArcCtrl.RequstArcTimeout = (CEC_REQ_ARC_TIMEOUT /pstHandler->BusBusyChk.Cec_PollingTime); //about 1s
			cec_printf("Set_ARCtimeout:%d...........cmd_initiate_Arc\n", pstHandler->ArcCtrl.RequstArcTimeout);
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_TerminateArc(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_TV);
			pstHandler->Tx.Opcode = CEC_OPCODE_TERMINATE_ARC;
			pstHandler->Tx.MsgLen = 2;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->ArcCtrl.TvSupportArc = 0;
			cec_printf("TerminateArc! TvSupportArc == 0\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_ReportPhyAddr(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");

			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST);
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.Operand[2] = DEVICE_TYPE_AUDIO_SYSTEM;
			pstHandler->Tx.MsgLen = 5;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;
	}
}

//ribin modify for cec test@mantis:274180
void CMD_PingAndReportAudSys(stCecHandler *pstHandler)
{
	cec_printf("CMD_PingAndReportAudSys in \n");
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1.....%d .....\n",Cec_GetLogAddr1());
			if (Cec_GetLogAddr1() == CEC_LOG_ADDR_UNREGISTERED)
			{
				cec_printf("Ping Audio System Logical Address\n");
				pstHandler->LogAddr = CEC_LOG_ADDR_AUDIO_SYSTEM;
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);//advid IOP ack for braodcast addr
			}
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->LogAddr);
			pstHandler->Tx.MsgLen = 1;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->PingMSG_Num += 1;
			UINT8 iACK = Cec_GetAdFlag(CEC_AD_FLAG_ACK0);
			if((iACK == 0)&&(pstHandler->ACKretry < 1))
			{
				pstHandler->ACKretry += 1;
				return;

			}
			pstHandler->ACKretry  = 0;
			if (!Cec_GetAdFlag(CEC_AD_FLAG_ACK0))
			{
				cec_printf("Receive ACK.............................................\n");
				Cec_GetAdFlag(CEC_AD_FLAG_TEST);
				pstHandler->Cmd.State = PROCESS_STEP1;
				if(pstHandler->PingMSG_Num >= 5)
				{
					pstHandler->Cmd.Id = CEC_CMD_IDLE;
					pstHandler->PingMSG_Num = 0;

				}
				break;
			}
			else
			{
				cec_printf("No ACK return.....................................\n");
				pstHandler->Cmd.Count++;
				cec_printf("Cmd.Count = %d\n", pstHandler->Cmd.Count);
			}
			if (pstHandler->Cmd.Count >= CEC_MAX_PING_SELF_TIMES)
			{
				cec_printf("set AudSysLogAddr to %d......................\n", pstHandler->LogAddr);
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);
				pstHandler->Cmd.Count = 0;
				pstHandler->Cmd.State = PROCESS_STEP3;
				pstHandler->LogAddrRdy = 1;
			}
			break;

		case PROCESS_STEP3:
			cec_printf("PROCESS_STEP3..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST);               //Broadcast
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.Operand[2] = DEVICE_TYPE_AUDIO_SYSTEM;
			cec_printf("Tx.Operand[2]:%d\n", pstHandler->Tx.Operand[2]);
			pstHandler->Tx.MsgLen = 5;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.Id = CEC_CMD_IDLE;
				pstHandler->PingMSG_Num = 0;
			}
			break;

		default:
			cec_printf("default (CMD_PingAndReportAudSys)........................\n");
			break;
	}
}

void CMD_PingAndReportPlayback(stCecHandler *pstHandler)
{
	cec_printf("CMD_PingAndReportPlayback in \n");
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			if (Cec_GetLogAddr2() == CEC_LOG_ADDR_UNREGISTERED)
			{
				cec_printf("Ping Playback Device Logical Address\n");
				pstHandler->LogAddr = CEC_LOG_ADDR_DVD1;
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);//advid IOP ack for braodcast addr
			}
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->LogAddr);
			pstHandler->Tx.MsgLen = 1;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			UINT8 iACK = Cec_GetAdFlag(CEC_AD_FLAG_ACK0);
			if((iACK == 0)&&(pstHandler->ACKretry < 1))
			{
				pstHandler->ACKretry += 1;
				return;

			}
			pstHandler->ACKretry  = 0;
			if (!Cec_GetAdFlag(CEC_AD_FLAG_ACK0))
			{
				cec_printf("Receive ACK.............................................\n");
				Cec_GetAdFlag(CEC_AD_FLAG_TEST);
				if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD1)
				{
					pstHandler->LogAddr = CEC_LOG_ADDR_DVD2;
					Cec_SetLoglAddr1(pstHandler->LogAddr);
					Cec_SetLoglAddr2(pstHandler->LogAddr);
					pstHandler->PingLogAddrCont++;
					cec_printf("PlaybackLogAddr: DVD1 --> DVD2 .................................\n");
				}
				else if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD2)
				{
					pstHandler->LogAddr = CEC_LOG_ADDR_DVD3;
					Cec_SetLoglAddr1(pstHandler->LogAddr);
					Cec_SetLoglAddr2(pstHandler->LogAddr);
					pstHandler->PingLogAddrCont++;
					cec_printf("PlaybackLogAddr: DVD2 --> DVD3 .................................\n");
				}
				else if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD3)
				{
					pstHandler->PingLogAddrCont++;
					cec_printf("PingLogAddrCont = %d\n", pstHandler->PingLogAddrCont);
					if (pstHandler->PingLogAddrCont >= 3)
					{
						cec_printf("PlaybackLogAddr: DVD3 --> UNREGISTERED .................................\n");
						pstHandler->LogAddr = CEC_LOG_ADDR_UNREGISTERED;
						cec_printf("No any addr can use,set PlaybackLogAddr to = %d !\n", pstHandler->LogAddr);
						Cec_SetLoglAddr1(pstHandler->LogAddr);
						Cec_SetLoglAddr2(pstHandler->LogAddr);
						pstHandler->Cmd.State = PROCESS_STEP3;//pingjun.yu modify for CTS 10.2.3-3 @2013-1-22
						break;
					}
					else
					{
						pstHandler->LogAddr = CEC_LOG_ADDR_DVD1;
						Cec_SetLoglAddr1(pstHandler->LogAddr);
						Cec_SetLoglAddr2(pstHandler->LogAddr);
					}
				}
				pstHandler->Cmd.State = PROCESS_STEP1;
			}
			else
			{
				cec_printf("No ACK return.....................................\n");
				pstHandler->Cmd.Count++;
				cec_printf("Cmd.Count = %d\n", pstHandler->Cmd.Count);
			}

			if (pstHandler->Cmd.Count >= CEC_MAX_PING_SELF_TIMES)
			{
				cec_printf("set PlaybackLogAddr to %d......................\n", pstHandler->LogAddr);
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);
				pstHandler->PingLogAddrCont = 0;
				pstHandler->Cmd.Count = 0;
				pstHandler->Cmd.State = PROCESS_STEP3;
				pstHandler->LogAddrRdy = 1;
			}
			break;

		case PROCESS_STEP3:
			cec_printf("PROCESS_STEP3..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST);                //Broadcast
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_PHYSICAL_ADDRESS;
			pstHandler->Tx.Operand[0] = pstHandler->PhyAddrAB;
			pstHandler->Tx.Operand[1] = pstHandler->PhyAddrCD;
			pstHandler->Tx.Operand[2] = DEVICE_TYPE_PLAYBACK_DEVICE;
			cec_printf("Tx.Operand[2]:%d\n", pstHandler->Tx.Operand[2]);
			pstHandler->Tx.MsgLen = 5;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.Id = CEC_CMD_IDLE;
			}
			break;

		default:
			cec_printf("default (CMD_PingAndReportPlayback)........................\n");
			break;
	}
}


//ribin modify for cec test@mantis:274180
void CMD_PingAudSys(stCecHandler *pstHandler)//without report physical address
{
	cec_printf("CMD_PingAudSys in \n");
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1.....%d .....\n",Cec_GetLogAddr1());
			if (Cec_GetLogAddr1() != CEC_LOG_ADDR_AUDIO_SYSTEM)
			{
				cec_printf("Ping Audio System Logical Address\n");
				pstHandler->LogAddr = CEC_LOG_ADDR_AUDIO_SYSTEM;
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);//advid IOP ack for braodcast addr
			}

			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->LogAddr);
			pstHandler->Tx.MsgLen = 1;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			UINT8 iACK = Cec_GetAdFlag(CEC_AD_FLAG_ACK0);
			if((iACK == 0)&&(pstHandler->ACKretry < 3))
			{
				pstHandler->ACKretry += 1;
				return;

			}
			pstHandler->ACKretry  = 0;
			if (!Cec_GetAdFlag(CEC_AD_FLAG_ACK0))
			{
				cec_printf("Receive ACK.............................................\n");
				Cec_GetAdFlag(CEC_AD_FLAG_TEST);
				pstHandler->Cmd.State = PROCESS_STEP1;
			}
			else
			{
				pstHandler->Cmd.Count++;
				cec_printf("No ACK return, Cmd.Count = %d\n", pstHandler->Cmd.Count);
			}

			if (pstHandler->Cmd.Count >= CEC_MAX_PING_SELF_TIMES)
			{
				cec_printf("set AudSysLogAddr to %d..............\n", pstHandler->LogAddr);
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);
				pstHandler->Cmd.Id = CEC_CMD_IDLE;
				pstHandler->Cmd.Count = 0;
				pstHandler->LogAddrRdy = 1;
			}
			break;

		default:
			cec_printf("default (CMD_PingAudSys)........................\n");
			break;
	}
}

void CMD_PingPlayback(stCecHandler *pstHandler)//without report physical address
{
	cec_printf("CMD_PingPlayback in \n");
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			if (Cec_GetLogAddr2() == CEC_LOG_ADDR_UNREGISTERED)
			{
				cec_printf("Ping Playback Device Logical Address\n");
				pstHandler->LogAddr = CEC_LOG_ADDR_DVD1;
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);//advid IOP ack for braodcast addr
			}
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->LogAddr);
			pstHandler->Tx.MsgLen = 1;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			UINT8 iACK = Cec_GetAdFlag(CEC_AD_FLAG_ACK0);
			if((iACK == 0)&&(pstHandler->ACKretry < 3))
			{
				pstHandler->ACKretry += 1;
				return;

			}
			pstHandler->ACKretry  = 0;
			if (!Cec_GetAdFlag(CEC_AD_FLAG_ACK0))
			{
				cec_printf("Receive ACK.............................................\n");
				Cec_GetAdFlag(CEC_AD_FLAG_TEST);
				if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD1)
				{
					pstHandler->LogAddr = CEC_LOG_ADDR_DVD2;
					Cec_SetLoglAddr1(pstHandler->LogAddr);
					Cec_SetLoglAddr2(pstHandler->LogAddr);
					pstHandler->PingLogAddrCont++;
					cec_printf("PlaybackLogAddr: DVD1 --> DVD2 .................................\n");
				}
				else if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD2)
				{
					pstHandler->LogAddr = CEC_LOG_ADDR_DVD3;
					Cec_SetLoglAddr2(pstHandler->LogAddr);
					Cec_SetLoglAddr1(pstHandler->LogAddr);
					pstHandler->PingLogAddrCont++;
					cec_printf("PlaybackLogAddr: DVD2 --> DVD3 .................................\n");
				}
				else if (pstHandler->LogAddr == CEC_LOG_ADDR_DVD3)
				{
					pstHandler->PingLogAddrCont++;
					cec_printf("PingLogAddrCont = %d\n", pstHandler->PingLogAddrCont);
					if (pstHandler->PingLogAddrCont >= 3)
					{
						cec_printf("PlaybackLogAddr: DVD3 --> UNREGISTERED .................................\n");
						pstHandler->LogAddr = CEC_LOG_ADDR_UNREGISTERED;
						cec_printf("No any addr can use,set PlaybackLogAddr to = %d !\n", pstHandler->LogAddr);
						Cec_SetLoglAddr1(pstHandler->LogAddr);
						Cec_SetLoglAddr2(pstHandler->LogAddr);
						pstHandler->Cmd.State = PROCESS_STEP3;//pingjun.yu modify for CTS 10.2.3-3 @2013-1-22
						break;
					}
					else
					{
						pstHandler->LogAddr = CEC_LOG_ADDR_DVD1;
						Cec_SetLoglAddr1(pstHandler->LogAddr);
						Cec_SetLoglAddr2(pstHandler->LogAddr);
					}
				}
				pstHandler->Cmd.State = PROCESS_STEP1;
			}
			else
			{
				cec_printf("No ACK return.....................................\n");
				pstHandler->Cmd.Count++;
				cec_printf("Cmd.Count = %d\n", pstHandler->Cmd.Count);
			}

			if (pstHandler->Cmd.Count >= CEC_MAX_PING_SELF_TIMES)
			{
				cec_printf("set PlaybackLogAddr to %d......................\n", pstHandler->LogAddr);
				Cec_SetLoglAddr1(pstHandler->LogAddr);
				Cec_SetLoglAddr2(pstHandler->LogAddr);
				pstHandler->PingLogAddrCont = 0;
				pstHandler->Cmd.Count = 0;
				pstHandler->Cmd.Id = CEC_CMD_IDLE;
				pstHandler->LogAddrRdy = 1;
			}
			break;

		default:
			cec_printf("default (CMD_PingPlayback)........................\n");
			break;
	}
}

void CMD_SetOsdName(stCecHandler *pstHandler)
{
	UINT8 mbLenght = pstHandler->CustVar.OsdNameStrLen;
	int i;
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");

			//mantis: 274180
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->DevOsdNameXfe.DestAddr);
			pstHandler->Tx.Opcode = CEC_OPCODE_SET_OSD_NAME;

			for (i = 0; i < mbLenght; i++)
			{
				pstHandler->Tx.Operand[i] = pstHandler->CustVar.pOsdNameStr[i];
			}

			pstHandler->Tx.MsgLen = mbLenght + 2;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;
	}
}

void CMD_DevVendorId(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			if (pstHandler->CustVar.VendorID == 0)
			{
				cec_printf("Vendor ID is null! Not needed to send vendor ID!\n");
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			else
			{
				pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_BROADCAST);
				pstHandler->Tx.Opcode = CEC_OPCODE_DEVICE_VENDOR_ID;
				pstHandler->Tx.Operand[0] = (pstHandler->CustVar.VendorID >> 16);
				pstHandler->Tx.Operand[1] = (pstHandler->CustVar.VendorID >> 8) & 0x0000ff;
				pstHandler->Tx.Operand[2] = pstHandler->CustVar.VendorID & 0x0000ff;
				pstHandler->Tx.MsgLen = 5;

				if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
				{
					Cec_SendFrame(&pstHandler->Tx);
					pstHandler->Cmd.State = PROCESS_STEP2;
				}
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_FeatureAbort(stCecHandler *pstHandler) //SpikeAdd 20080522
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->FeatureAbort.DestAddr);
			pstHandler->Tx.Opcode = CEC_OPCODE_FEATURE_ABORT;
			pstHandler->Tx.Operand[0] = pstHandler->FeatureAbort.AbortOpcode;
			pstHandler->Tx.Operand[1] = pstHandler->FeatureAbort.AbortReason;
			pstHandler->Tx.MsgLen = 4;
			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			Cec_GetAdFlag(CEC_AD_FLAG_TEST);
			cec_printf("PROCESS_STEP2_A..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_ReportPwrStatus(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->GiveDevPwrSts.DestAddr);
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_PWR_STATUS;
			pstHandler->Tx.Operand[0] = pstHandler->GiveDevPwrSts.PwrStatus;
			pstHandler->Tx.MsgLen = 3;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_CecVersion(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			//mantis: 274180
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->SysInfo.DestAddr);
			pstHandler->Tx.Opcode = CEC_OPCODE_CEC_VERSION;
			pstHandler->Tx.Operand[0] = CEC_VERSION_V1_4a;
			pstHandler->Tx.MsgLen = 3;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_GetTvPwrStatus(stCecHandler *pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | CEC_LOG_ADDR_TV);
			pstHandler->Tx.Opcode = CEC_OPCODE_GIVE_DEVICE_PWR_STATUS;
			pstHandler->Tx.MsgLen = 2;

			if (!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

void CMD_ReportShortAudDescriptor(stCecHandler* pstHandler)
{
	switch (pstHandler->Cmd.State)
	{
		case PROCESS_STEP1:
			cec_printf("PROCESS_STEP1..........\n");
			pstHandler->Tx.Header = ((pstHandler->LogAddr << 4) | pstHandler->SysAudCtrl.DestAddr[3]);
			pstHandler->Tx.Opcode = CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR;
			UINT8 mbAudioFormatCode;
			UINT8 mbTableMaxNum =0;  //max 4 short audio des 4*3 =12 byte
			UINT8 i;
			for(i = 0; (i<=AFC_DDP)&&(mbTableMaxNum <4); i++)  //follow program refer to SPEC: CEA_861_D_Y2006
			{
				if(pstHandler->SysAudCtrl.AudFormatRec&(1<<i))
				{
					switch(i)
					{
						case AFC_LPCM: // 1
							 cec_printf("Set AFC_LPCM short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_LPCM)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_LPCM)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] =*(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_LPCM)+0);
							 mbTableMaxNum++;
							 break;

					    case AFC_AC3:  //
							 cec_printf("Set AFC_AC3 short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_AC3)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_AC3)+1);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_AC3)+2);
							 mbTableMaxNum++;
						break;
					    case AFC_MPEG1:  //
							 cec_printf("Set AFC_MPEG1 short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG1)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG1)+1);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG1)+2);
							 mbTableMaxNum++;
						break;

					    case AFC_MP3:  //
							 cec_printf("Set AFC_MP3 short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MP3)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MP3)+1);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MP3)+2);
							 mbTableMaxNum++;
						break;

					    case AFC_MPEG2:  //
							 cec_printf("Set AFC_MPEG2 short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG2)+0);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG2)+1);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_MPEG2)+2);
							 mbTableMaxNum++;
						break;

					    case AFC_AAC:
							 cec_printf("Set AFC_AAC short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData+AFC_AAC)+0);//(mbAudioFormatCode|0x04);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_AAC)+1);//0x1f;
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_AAC)+2);//0x28;
							 mbTableMaxNum++;
						break;

						case AFC_DTS:
							 cec_printf("Set AFC_DTS short audio desciptor\n");
							 mbAudioFormatCode = 0;
							 mbAudioFormatCode = i<<3;
							 pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DTS)+0);//(mbAudioFormatCode|0x04);
							 pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DTS)+1);//0x1f;
							 pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DTS)+2);//0x28;
							 mbTableMaxNum++;
							 break;

						case AFC_DDP:
							cec_printf("Set AFC_DDP short audio desciptor\n");
							mbAudioFormatCode = 0;
							mbAudioFormatCode = i<<3;
							pstHandler->Tx.Operand[mbTableMaxNum*3] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DDP)+0);//(mbAudioFormatCode|0x07);
							pstHandler->Tx.Operand[mbTableMaxNum*3+1] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DDP)+1);//0x07;
							pstHandler->Tx.Operand[mbTableMaxNum*3+2] = *(*(pstHandler->CustVar.ShortAudDescriptorData + AFC_DDP)+2);//0x00;
							mbTableMaxNum++;
							break;
						default:
							cec_printf("we are not support this short audio desciptor\n");
							break;
					}

				}
			}
			pstHandler->Tx.MsgLen = 2 + mbTableMaxNum*3;
			cec_printf("short audio desciptor num: %d\n",mbTableMaxNum);

			if(!Cec_BusBusyChk(&pstHandler->BusBusyChk))
			{
				Cec_SendFrame(&pstHandler->Tx);
				pstHandler->Cmd.State = PROCESS_STEP2;
			}
			break;

		case PROCESS_STEP2:
			cec_printf("PROCESS_STEP2..........\n");
			pstHandler->Cmd.Id = CEC_CMD_IDLE;
			break;

		default:
			cec_printf("default..........\n");
			break;
	}
}

