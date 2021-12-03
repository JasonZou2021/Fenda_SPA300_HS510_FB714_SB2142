/******************************************************************************

   Copyright (c) 2016 Sunplus technology co. Ltd.All right reserved!

******************************************************************************/
/*---------------------------------------------------------------------------*
*                            INCLUDE   DECLARATIONS                         *
*---------------------------------------------------------------------------*/
//#define EDID_DEBUG

#include <string.h>
#include <stdio.h>
#include "types.h"
#include "cec.h"
#include "cec_main.h"
#include "cec_opfun.h"
#include "cec_cmd.h"


#include "pinmux.h"
#include "initcall.h"

#include "fsi_api.h"
//#include "i2c_sw_general.h"

#include "cfg_sdk.h"
#include "exstd_interfaces.h"

//#include "adc.h"
#include "power.h"


//#include "ic_bonding.h"
#include "hw_gpio_list.h"
#include "i2c_api.h"
#include "datarammap.h"

#ifdef EDID_DEBUG
	#include "edid.h"
#endif



#ifndef NULL
	#define NULL 0
#endif

#define SEG_PTR_DDC_ADDR            (0x60)
#define HPD_TIMEOUT_CNT             (50) // 1s
#define HPD_TIMEOUT_MS_CNT          (800)// 1s
#define HPD_TIMEOUT_INIT            (500)// 1s

#define CEC_BUS_BUSY                0
#define CEC_BUS_FREE                1
#define CEC_BUS_FREE_FOR_LONG_TIME  2

// ===================================================================
// Local Variable
// ===================================================================

stCecHandler sstCecHandler;
extern unsigned int GetMSFromSTC(void);

int Cec_Evt_Post(stCecHandler *pstHandler);

static UINT8 gbCecMsgLen[256] =   //this length = header + OPCode + Oprands
{
	4, // 0 0x00 <Feature Abort>
	0, // 1 0x01
	0, // 2 0x02
	0, // 3 0x03
	0, // 4 0x04
	0, // 5 0x05
	0, // 6 0x06
	0, // 7 0x07
	0, // 8 0x08
	0, // 9 0x09
	0, // 10 0x0a
	0, // 11 0x0b
	0, // 12 0x0c
	0, // 13 0x0d
	0, // 14 0x0e
	0, // 15 0x0f
	0, // 16 0x10
	0, // 17 0x11
	0, // 18 0x12
	0, // 19 0x13
	0, // 20 0x14
	0, // 21 0x15
	0, // 22 0x16
	0, // 23 0x17
	0, // 24 0x18
	0, // 25 0x19
	3, // 26 0x1a <Give Deck Status>
	0, // 27 0x1b
	0, // 28 0x1c
	0, // 29 0x1d
	0, // 30 0x1e
	0, // 31 0x1f
	0, // 32 0x20
	0, // 33 0x21
	0, // 34 0x22
	0, // 35 0x23
	0, // 36 0x24
	0, // 37 0x25
	0, // 38 0x26
	0, // 39 0x27
	0, // 40 0x28
	0, // 41 0x29
	0, // 42 0x2a
	0, // 43 0x2b
	0, // 44 0x2c
	0, // 45 0x2d
	0, // 46 0x2e
	0, // 47 0x2f
	0, // 48 0x30
	0, // 49 0x31
	5, // 50 0x32 <Set Menu Language>
	0, // 51 0x33
	0, // 52 0x34
	0, // 53 0x35
	0, // 54 0x36
	0, // 55 0x37
	0, // 56 0x38
	0, // 57 0x39
	0, // 58 0x3a
	0, // 59 0x3b
	0, // 60 0x3c
	0, // 61 0x3d
	0, // 62 0x3e
	0, // 63 0x3f
	0, // 64 0x40
	3, // 65 0x41 <Play>
	3, // 66 0x42 <Deck Control>
	0, // 67 0x43
	3, // 68 0x44 <User Control Press>
	0, // 69 0x45
	2, // 70 0x46 <Give OSD Name>
	0, // 71 0x47
	0, // 72 0x48
	0, // 73 0x49
	0, // 74 0x4a
	0, // 75 0x4b
	0, // 76 0x4c
	0, // 77 0x4d
	0, // 78 0x4e
	0, // 79 0x4f
	0, // 80 0x50
	0, // 81 0x51
	0, // 82 0x52
	0, // 83 0x53
	0, // 84 0x54
	0, // 85 0x55
	0, // 86 0x56
	0, // 87 0x57
	0, // 88 0x58
	0, // 89 0x59
	0, // 90 0x5a
	0, // 91 0x5b
	0, // 92 0x5c
	0, // 93 0x5d
	0, // 94 0x5e
	0, // 95 0x5f
	0, // 96 0x60
	0, // 97 0x61
	0, // 98 0x62
	0, // 99 0x63
	0, // 100 0x64
	0, // 101 0x65
	0, // 102 0x66
	0, // 103 0x67
	0, // 104 0x68
	0, // 105 0x69
	0, // 106 0x6a
	0, // 107 0x6b
	0, // 108 0x6c
	0, // 109 0x6d
	0, // 110 0x6e
	0, // 111 0x6f
	0, // 112 0x70
	0, // 113 0x71
	0, // 114 0x72
	0, // 115 0x73
	0, // 116 0x74
	0, // 117 0x75
	0, // 118 0x76
	0, // 119 0x77
	0, // 120 0x78
	0, // 121 0x79
	0, // 122 0x7a
	0, // 123 0x7b
	0, // 124 0x7c
	0, // 125 0x7d
	0, // 126 0x7e
	0, // 127 0x7f
	0, // 128 0x80
	0, // 129 0x81
	4, // 130 0x82 <Active Source>
	0, // 131 0x83
	0, // 132 0x84
	0, // 133 0x85
	4, // 134 0x86 <Set Stream Path>
	0, // 135 0x87
	0, // 136 0x88
	0, // 137 0x89
	0, // 138 0x8a
	0, // 139 0x8b
	0, // 140 0x8c
	3, // 141 0x8d <Menu Request>
	3, // 142 0x8e <Menu Status>
	0, // 143 0x8f
	0, // 144 0x90
	0, // 145 0x91
	0, // 146 0x92
	0, // 147 0x93
	0, // 148 0x94
	0, // 149 0x95
	0, // 150 0x96
	0, // 151 0x97
	0, // 152 0x98
	0, // 153 0x99
	0, // 154 0x9a
	0, // 155 0x9b
	0, // 156 0x9c
	4, // 157 0x9d //ck modified for inactive source,2011-8-24
	0, // 158 0x9e
	0, // 159 0x9f
	0, // 160 0xa0 <Vendor Command With ID> 4+?
	0, // 161 0xa1
	0, // 162 0xa2
	0, // 163 0xa3
	0, // 164 0xa4
	0, // 165 0xa5
	0, // 166 0xa6
	0, // 167 0xa7
	0, // 168 0xa8
	0, // 169 0xa9
	0, // 170 0xaa
	0, // 171 0xab
	0, // 172 0xac
	0, // 173 0xad
	0, // 174 0xae
	0, // 175 0xaf
	0, // 176 0xb0
	0, // 177 0xb1
	0, // 178 0xb2
	0, // 179 0xb3
	0, // 180 0xb4
	0, // 181 0xb5
	0, // 182 0xb6
	0, // 183 0xb7
	0, // 184 0xb8
	0, // 185 0xb9
	0, // 186 0xba
	0, // 187 0xbb
	0, // 188 0xbc
	0, // 189 0xbd
	0, // 190 0xbe
	0, // 191 0xbf
	0, // 192 0xc0
	0, // 193 0xc1
	0, // 194 0xc2
	0, // 195 0xc3
	0, // 196 0xc4
	0, // 197 0xc5
	0, // 198 0xc6
	0, // 199 0xc7
	0, // 200 0xc8
	0, // 201 0xc9
	0, // 202 0xca
	0, // 203 0xcb
	0, // 204 0xcc
	0, // 205 0xcd
	0, // 206 0xce
	0, // 207 0xcf
	0, // 208 0xd0
	0, // 209 0xd1
	0, // 210 0xd2
	0, // 211 0xd3
	0, // 212 0xd4
	0, // 213 0xd5
	0, // 214 0xd6
	0, // 215 0xd7
	0, // 216 0xd8
	0, // 217 0xd9
	0, // 218 0xda
	0, // 219 0xdb
	0, // 220 0xdc
	0, // 221 0xdd
	0, // 222 0xde
	0, // 223 0xdf
	0, // 224 0xe0
	0, // 225 0xe1
	0, // 226 0xe2
	0, // 227 0xe3
	0, // 228 0xe4
	0, // 229 0xe5
	0, // 230 0xe6
	0, // 231 0xe7
	0, // 232 0xe8
	0, // 233 0xe9
	0, // 234 0xea
	0, // 235 0xeb
	0, // 236 0xec
	0, // 237 0xed
	0, // 238 0xee
	0, // 239 0xef
	0, // 240 0xf0
	0, // 241 0xf1
	0, // 242 0xf2
	0, // 243 0xf3
	0, // 244 0xf4
	0, // 245 0xf5
	0, // 246 0xf6
	0, // 247 0xf7
	0, // 248 0xf8
	0, // 249 0xf9
	0, // 250 0xfa
	0, // 251 0xfb
	0, // 252 0xfc
	0, // 253 0xfd
	0, // 254 0xfe
	2, // 255 0xFF <Abort>
};

void OutputBlock(const UINT8 *pBuf, UINT32 dBufLen)
{
	UINT32 i = 0;

	if (NULL == pBuf)
	{
		return;
	}

	for (i = 0; i < dBufLen; i++, pBuf++)
	{
		cec_printf0("0x%02x ", *pBuf);

		if ((i & 0xf) == 0xf)
		{
			cec_printf0("\n");
		}
	}

	cec_printf0("\n");
}

int Cec_GetTvPwrStatus(void)
{
	int res = 0;

	res = Cec_CmdListInsert(&sstCecHandler, CEC_CMD_GET_TV_POWER_STATUS);

	if (res)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

stCecHandler *CEC_GetCecHandler(void)
{
	return &sstCecHandler;
}
/*
stCecHandler *CEC_GetStandbyCecHandler(void)
{
	stCecHandler stCecHandler;
	stCecHandler.CecPollingTime = CEC_POLLING_FREQ;
	stCecHandler.HwRdy = 0;
	stCecHandler.CecEnable = 0;
	stCecHandler.SysSrc = eSysSrc_Others;
	CEC_HwInit(&stCecHandler);
	return &stCecHandler;
}
*/



void Cec_SysAudCtrlInit(stCecHandler *pstHandler)
{
	pstHandler->SysAudCtrl.SysAudModeSts = 0;
	pstHandler->SysAudCtrl.FlowStep = 0;
	pstHandler->SysAudCtrl.DestAddr[0] = CEC_LOG_ADDR_BROADCAST;
	pstHandler->SysAudCtrl.DestAddr[1] = CEC_LOG_ADDR_TV;
	pstHandler->SysAudCtrl.DestAddr[2] = CEC_LOG_ADDR_TV;
	pstHandler->SysAudCtrl.DestAddr[3] = CEC_LOG_ADDR_TV;
	pstHandler->SysAudCtrl.ActiveSrcPhyAddrAB = 0xff;
	pstHandler->SysAudCtrl.ActiveSrcPhyAddrCD = 0xff;
	cec_printf("pstHandler->SysAudCtrl.audio_mode:%d\n", pstHandler->SysAudCtrl.SysAudModeSts);
}

void Cec_LogAddrInit(stCecHandler *pstHandler)
{
	pstHandler->LogAddr = CEC_LOG_ADDR_UNREGISTERED;
	Cec_SetLoglAddr1(pstHandler->LogAddr);
	Cec_SetLoglAddr2(pstHandler->LogAddr);
	pstHandler->LogAddrRdy = 0;
	cec_printf("logical address init\n");
}

void Cec_PhyAddrInit(stCecHandler *pstHandler)
{
	if ((!pstHandler->PhyAddrRdy) && (!pstHandler->PhyAddrAB))
	{
		pstHandler->PhyAddrAB = 0x10;
		pstHandler->PhyAddrCD = 0x00;
	}
	cec_printf("Physical address init\n");
}

void Cec_CustVarInit(stCecHandler *pstHandler)
{
	CFG_CEC_T *pstCEC_cfg = GET_SDK_CFG(CFG_CEC_T);

	cec_printf("Customer var init\n");
	pstHandler->CustVar.HpdWakeUpEnable = pstCEC_cfg->CEC_HPD_WakeUp;
	pstHandler->CustVar.CecAlwaysRun = pstCEC_cfg->Cfg_CecAlwaysRun;
	pstHandler->CustVar.PwrOnOpcodeTbl = pstCEC_cfg->CEC_PwrOnOpcodeTbl;
	pstHandler->CustVar.PwrOnOpcodeNum = pstCEC_cfg->CEC_PwrOnOpcodeNum;
	pstHandler->CustVar.VendorID = pstCEC_cfg->CEC_VendorId;
	pstHandler->CustVar.pOsdNameStr = pstCEC_cfg->pbCEC_DevOsdName;
	pstHandler->CustVar.OsdNameStrLen = strlen(pstCEC_cfg->pbCEC_DevOsdName);
	pstHandler->CustVar.ShortAudDescriptor = pstCEC_cfg->CEC_ShortAudDescriptor;
    pstHandler->CustVar.ShortAudDescriptorNum = pstCEC_cfg->CEC_ShortAudDescriptorNum;
	pstHandler->CustVar.ShortAudDescriptorData = pstCEC_cfg->CEC_ShortAudDescriptorData;

	if (pstHandler->CustVar.OsdNameStrLen > CEC_MAX_OSD_NAME_LEN)
	{
		cec_printf_E("OSD name err: %d (max is 14)\n", pstHandler->CustVar.OsdNameStrLen);
		pstHandler->CustVar.OsdNameStrLen = CEC_MAX_OSD_NAME_LEN;
	}
	cec_printf("vendor ID:0x%x, OSD name: %s", pstHandler->CustVar.VendorID, pstHandler->CustVar.pOsdNameStr);
}

void Cec_ReadListInit(stCecHandler *pstHandler)
{
	memset(pstHandler->CecReadList, 0, sizeof(pstHandler->CecReadList));
	pstHandler->CecReadList_Start = 0;
	pstHandler->CecReadList_End = 0;

}

void Cec_ReadListInsert(stCecHandler *pstHandler, stCecFrameData *cec_cmd)
{
	if ((pstHandler->CecReadList_End + 1) % CEC_CMD_LIST_BUF_MAX == pstHandler->CecReadList_Start)
	{
		cec_printf(" Cec Read List Full\n ");
		return;
	}

	//cec_printf(" Read List Insert Header %x\n ", cec_cmd->Header);
	//cec_printf(" Read List Insert Opcode %x\n ", cec_cmd->Opcode);

	memcpy(&pstHandler->CecReadList[pstHandler->CecReadList_End], cec_cmd, sizeof(stCecFrameData));
	pstHandler->CecReadList_End = (pstHandler->CecReadList_End + 1) % CEC_CMD_LIST_BUF_MAX;
}

UINT8 Cec_ReadListGet(stCecHandler *pstHandler, stCecFrameData *cec_cmd)
{
	if (pstHandler->CecReadList_Start == pstHandler->CecReadList_End)
	{
		//cec_printf2(" Cec Read List Empty\n ");
		return 0;
	}

	memcpy(cec_cmd, &pstHandler->CecReadList[pstHandler->CecReadList_Start], sizeof(stCecFrameData) );
	pstHandler->CecReadList_Start = (pstHandler->CecReadList_Start + 1) % CEC_CMD_LIST_BUF_MAX;
	//cec_printf(" Read List Get Opcode %x\n ", cec_cmd->Opcode);
	return 1;
}

void Cec_CmdListInit(stCecHandler *pstHandler)
{
	int i;

	for (i = 0; i < CEC_CMD_LIST_BUF_MAX; i++)
	{
		pstHandler->Cmd.List[i] = CEC_CMD_NONE; //initial value
	}
	pstHandler->Cmd.Id = CEC_CMD_IDLE;
	pstHandler->Cmd.State = PROCESS_STEP1;
}

void Cec_CmdDelFirst(stCecHandler *pstHandler)
{
	int i;

	for (i = 0; i < (CEC_CMD_LIST_BUF_MAX - 1); i++)
	{
		pstHandler->Cmd.List[i] = pstHandler->Cmd.List[i + 1]; //EX: 1,2,3 ==> 2,3,CEC_CMD_NONE ==>3,CEC_CMD_NONE,CEC_CMD_NONE
	}

	pstHandler->Cmd.List[CEC_CMD_LIST_BUF_MAX - 1] = CEC_CMD_NONE;
}

UINT8 Cec_CmdListInsert(stCecHandler *pstHandler, UINT8 tbCmd)
{
	int i;

	cec_printf("Cec_CmdListInsert: 0x%x\n", tbCmd);

	for (i = 0; i < CEC_CMD_LIST_BUF_MAX; i++)
	{
		if (pstHandler->Cmd.List[i] == CEC_CMD_NONE)
		{
			pstHandler->Cmd.List[i] = tbCmd;
			return 1; //insert successfully
		}
	}

	return 0; //insert fail
}

int Cec_GetVol(void)
{
	return 10;
}

int Cec_GetMuteState(void)
{
	return 0;
}

UINT8 Cec_InitiationARCChk(stCecHandler *pstHandler)
{

	cec_printf("========daniel phy: 0x%x, 0x%x\n", pstHandler->PhyAddrAB, pstHandler->PhyAddrCD);
	cec_printf("========EDID_PhyaddrChange = %d\n", pstHandler->EDID_PhyaddrChange);
	cec_printf("========HPDLOWTimeout = %d\n", pstHandler->HPDLOWTimeout);
	int initARCCMD;
	initARCCMD = pstHandler->HPDHighTime - pstHandler->HPDLOWTime;
	cec_printf("========HPDLOWTimeout True= %d\n", initARCCMD);
	if (((pstHandler->PhyAddrAB & 0xf) == 0) && (pstHandler->PhyAddrCD == 0x00))
	{
		if (pstHandler->EDID_PhyaddrChange == 1)
		{
			pstHandler->ArcCtrl.TvSupportArc = 0;
			return 1;
		}
		else
		{
			if ((initARCCMD > HPD_TIMEOUT_MS_CNT) || (pstHandler->HPDLOWTimeout == 0))
			{
				pstHandler->ArcCtrl.TvSupportArc = 0;
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
		return 0;
	}
	/*
	     if(pstHandler->HPDLOWTimeout == 0)
	     {
	     	pstHandler->HPDLOWTimeout = HPD_TIMEOUT_CNT;
	     	 return 1;
		 }
		 else
		 {
		 	pstHandler->HPDLOWTimeout = HPD_TIMEOUT_CNT;
	        if(((pstHandler->PhyAddrAB & 0xf) == 0)&&(pstHandler->PhyAddrCD == 0x00))
	        {
	            if(pstHandler->EDID_PhyaddrChange == 1)
	            {
	               return 1;
				}
				else
				{
					return 0;
				}
			}
			else
			{
	           return 0;
			}
		 }*/
}
void Cec_SendCecCmd(stCecHandler *pstHandler, UINT8 tbCmd)
{
	if (pstHandler->Cmd.Id == CEC_CMD_IDLE)
	{
		pstHandler->Cmd.Id = tbCmd;
		Cec_CmdProcess(pstHandler); //if no this command cec test will fail in 10.x.x
	}
	else
	{
		cec_printf_E("CEC not ready!!\n");
	}
}

void Cec_SetFuncActive(stCecHandler *pstHandler)
{
	cec_printf("Cec_SetFuncActive\n");
	cec_printf("pstHandler->GiveDevPwrSts.PwrStatus = %d\n",pstHandler->GiveDevPwrSts.PwrStatus);

	if (pstHandler->GiveDevPwrSts.PwrStatus == POWER_STATUS_POWER_ON)
	{
		if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
		{
			Cec_CmdListInsert(pstHandler, CEC_CMD_PING_AND_REPORT_PLAYBACK);
			Cec_CmdListInsert(pstHandler, CEC_CMD_ONE_TOUCH_PLAY);
			Cec_CmdListInsert(pstHandler, CEC_CMD_DEVICE_VENDOR_ID);
		}
		else //if(pstHandler->PhyAddrRdy) mingjin 293596
		{
			if (pstHandler->SysSrc == eSysSrc_ARC)
			{
				Cec_CmdListInsert(pstHandler, CEC_CMD_PING_AND_REPORT_AUD_SYS);

				if (Cec_InitiationARCChk(pstHandler))
				{
					cec_printf2("set Function ARC Init\n");
					//Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_PHYSICAL_ADDRESS);
					Cec_CmdListInsert(pstHandler, CEC_CMD_INITIATE_ARC);
					Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_ACTIVE);
					Cec_CmdListInsert(pstHandler, CEC_CMD_REQUEST_ACTIVE_SOURCE);
				}



				Cec_CmdListInsert(pstHandler, CEC_CMD_DEVICE_VENDOR_ID);
			}

		}
		if (pstHandler->SysSrc == eSysSrc_Others)
		{
		    cec_printf("set Function pstHandler->SysSrc == eSysSrc_Others\n");
			Cec_CmdListInsert(pstHandler, CEC_CMD_PING_AND_REPORT_AUD_SYS);

		}
	}
	else
	{
		if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
			Cec_CmdListInsert(pstHandler, CEC_CMD_PING_PLAYBACK);
		else
		{
			cec_printf2("set Function not ARC Init\n");
			Cec_CmdListInsert(pstHandler, CEC_CMD_PING_AUD_SYS);
			if (pstHandler ->StandbyStatus == 1)
			{
				//Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_INACTIVE);
				pstHandler->StandbyStatus = 0;
			}
			Cec_CmdListInsert(pstHandler, CEC_CMD_GET_TV_POWER_STATUS);

		}
	}
}

void Cec_SetAutoPwrOn(stCecHandler *pstHandler, UINT8 tbAction)
{
	pstHandler->AutoPwrOn = tbAction;
}

UINT8 Cec_GetAutoPwrOn(stCecHandler *pstHandler)
{
	UINT8 tbAction = 0;
	tbAction = pstHandler->AutoPwrOn;
	//pstHandler->AutoPwrOn = 0;
	return tbAction;
}

static BOOL Cec_PwrOnOpcodeChk(stCecHandler *pstHandler)
{
	BOOL bRet = FALSE;
	UINT32 i = 0;
	UINT32 iPowerOnOpcodeNum = 0;

	iPowerOnOpcodeNum = pstHandler->CustVar.PwrOnOpcodeNum;

	do
	{

		if (NULL == pstHandler->CustVar.PwrOnOpcodeTbl)
		{
			break;
		}

		for (i = 0; i < iPowerOnOpcodeNum; i++)
		{
			if (pstHandler->Rx.Opcode == pstHandler->CustVar.PwrOnOpcodeTbl[i])
			{
				if (pstHandler->Rx.Opcode == CEC_OPCODE_SYS_AUDIO_MODE_REQUEST)
				{
					if (pstHandler->Rx.MsgLen > 3)
					{
						if ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4 == CEC_LOG_ADDR_TV)
						{
							pstHandler->SysAudCtrl.TvSupportSac = 1;
							cec_printf("TV Support SAC feature\n");
						}
						cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
						bRet = TRUE;
					}
					cec_printf(" SAC off\n");
				}
				else if (pstHandler->Rx.Opcode == CEC_OPCODE_USR_CTL_PRESSED) //mantis:278884
				{
					if ((pstHandler->Rx.Operand[0] == UI_CMD_POWER) || (pstHandler->Rx.Operand[0] == UI_CMD_POWER_ON_FUNCT) || (pstHandler->Rx.Operand[0] == UI_CMD_POWER_TOGGLE_FUNCT) || (pstHandler->Rx.Operand[0] == UI_CMD_POWER_TOGGLE_FUNCT))
					{
						cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
						bRet = TRUE;
					}
				}
				else if (pstHandler->Rx.Opcode == CEC_OPCODE_SET_STREAM_PATH)
				{
					if ((pstHandler->Rx.Operand[0] == pstHandler->PhyAddrAB) && (pstHandler->Rx.Operand[1] == pstHandler->PhyAddrCD))
					{
						cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
						bRet = TRUE;
					}
				}
				else if (pstHandler->Rx.Opcode == CEC_OPCODE_ACTIVE_SOURCE)
				{
					//if ((pstHandler->Rx.Operand[0] == 0) && (pstHandler->Rx.Operand[1] == 0))
					//{
						cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
						bRet = TRUE;
					//}
				}
				else if(pstHandler->Rx.Opcode == CEC_OPCODE_ROUTING_CHANGE)
				{
					if((pstHandler->Rx.Operand[0]&0xf0) == pstHandler->PhyAddrAB)
					{
						cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
						bRet = TRUE;
					}
				}
				else
				{
					cec_printf(" Match Poweron Opcode 0x%x\n", pstHandler->Rx.Opcode);
					bRet = TRUE;
				}

				break;
			}
		}

	}
	while (0);

	return (bRet);
}

UINT8 Cec_SupportOpcodeChk(stCecHandler *pstHandler)
{
	UINT8 mbOpCodeSupported = 0;

	//If many customer want to use simple CEC, please make each "switch case" by Function.
	if (((pstHandler->CustVar.CecAlwaysRun == 1 )||(pstHandler->SysSrc == eSysSrc_ARC))&&(pstHandler->SysSrc != eSysSrc_HDMI_Tx))
	{

		switch (pstHandler->Rx.Opcode)
		{
			case CEC_OPCODE_GIVE_DECK_STATUS:
			case CEC_OPCODE_DECK_CONTROL:
			case CEC_OPCODE_SET_MENU_LANGUAGE:
			case CEC_OPCODE_MENU_REQUEST:
			case CEC_OPCODE_VENDOR_CMD_WITH_ID:
			case CEC_OPCODE_DEVICE_VENDOR_ID:
			case CEC_OPCODE_PLAY:
			case CEC_OPCODE_ROUTING_CHANGE:
			case CEC_OPCODE_ROUTING_INFORMATION:
			case CEC_OPCODE_SET_STREAM_PATH:
			case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
			case CEC_OPCODE_REPORT_PHYSICAL_ADDRESS:
			case CEC_OPCODE_IMAGE_VIEW_ON:
			case CEC_OPCODE_TEXT_VIEW_ON:
				if(((pstHandler->GiveDevPwrSts.PwrStatus == POWER_STATUS_STANDBY)&&(pstHandler->Rx.Opcode == CEC_OPCODE_ROUTING_CHANGE))||
				   ((pstHandler->GiveDevPwrSts.PwrStatus == POWER_STATUS_STANDBY)&&(pstHandler->Rx.Opcode == CEC_OPCODE_SET_STREAM_PATH)))

				{
					mbOpCodeSupported = 1;
					break;
				}
				mbOpCodeSupported = 0;
				break;

			default:
				mbOpCodeSupported = 1;
				break;
		}
	}
	else if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
	{
		switch (pstHandler->Rx.Opcode)
		{
			case CEC_OPCODE_SET_STREAM_PATH:
			case CEC_OPCODE_STANDBY:
			case CEC_OPCODE_ACTIVE_SOURCE:
			case CEC_OPCODE_USR_CTL_PRESSED:
			case CEC_OPCODE_USR_CTL_RELEASED:
			case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
			case CEC_OPCODE_GET_CEC_VERSION:
			case CEC_OPCODE_GIVE_OSD_NAME:
			case CEC_OPCODE_FEATURE_ABORT:
			case CEC_OPCODE_GIVE_DEVICE_PWR_STATUS:
			case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:
			case CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:
			case CEC_OPCODE_ABORT:
				mbOpCodeSupported = 1;
				break;

			default:
				mbOpCodeSupported = 0;
				break;
		}
	}
	else if (pstHandler->SysSrc == eSysSrc_Others)
	{
		switch (pstHandler->Rx.Opcode)
		{
			case CEC_OPCODE_USR_CTL_PRESSED:
			case CEC_OPCODE_USR_CTL_RELEASED:
			case CEC_OPCODE_SYS_AUDIO_MODE_REQUEST:
			case CEC_OPCODE_GIVE_AUDIO_STATUS:
			case CEC_OPCODE_GIVE_SYS_AUDIO_MODE_STATUS:
			case CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR:
				mbOpCodeSupported = 1;
				break;

			default:
				mbOpCodeSupported = 0;
				break;
		}
	}

	return mbOpCodeSupported;
}

void Cec_SacRequestCounter(stCecHandler *pstHandler)
{
	if (pstHandler->SysAudCtrl.RequestSacTimeout > 0)
	{
		pstHandler->SysAudCtrl.RequestSacTimeout--;
	}
	else if (pstHandler->SysAudCtrl.FlowStep == 3)
	{
		pstHandler->SysAudCtrl.TvSupportSac = 1;
		Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_ACTIVE);
		cec_printf("unmute speaker: %d\n", pstHandler->SysAudCtrl.SysAudModeSts);
	}
}

void Cec_ArcRequestCounter(stCecHandler *pstHandler)
{
	if (pstHandler->ArcCtrl.RequstArcTimeout > 0)
		pstHandler->ArcCtrl.RequstArcTimeout--;

	if (pstHandler->ArcCtrl.RequstArcTimeout == 1 && (pstHandler->ArcCtrl.TvSupportArc == 0))
	{
		if ((pstHandler->retryNum < 4))
		{
			cec_printf("retry initiate ARC\n");
			pstHandler->retryNum += 1;
			Cec_CmdListInsert(pstHandler, CEC_CMD_INITIATE_ARC);
		}
		else
		{
			pstHandler->retryNum = 0;
			cec_printf("Timeout! TvSupportArc == 0\n");
			pstHandler->ArcCtrl.TvSupportArc = 0; //C4.2.1 If TV does not receive <Report ARC initiated> after timeout, ARC set up considered failed
		}
	}
}

void Cec_SetSysAudioCtrlStatus(stCecHandler *pstHandler, eCecStatus teStatus)
{
	UINT8 ret;
	if (teStatus)
	{
		if ( pstHandler->SysAudCtrl.TvSupportSac )
		{
			cec_printf("pstHandler->SysAudCtrl.TvSupportSac==%d!!!!!!!!!!!!\n", pstHandler->SysAudCtrl.TvSupportSac);
			pstHandler->SysAudCtrl.DestAddr[0] = CEC_LOG_ADDR_BROADCAST;
			pstHandler->SysAudCtrl.FlowStep = 4;
		}
		else
		{
			pstHandler->SysAudCtrl.DestAddr[0] = CEC_LOG_ADDR_TV;
			pstHandler->SysAudCtrl.FlowStep = 2;
		}
		pstHandler->SysAudCtrl.SysAudModeSts = 1;
	}
	else
	{
		cec_printf("Cec_SetSysAudioCtrlStatus off!!!!!!!!!!!\n");
		pstHandler->SysAudCtrl.DestAddr[0] = CEC_LOG_ADDR_BROADCAST;
		pstHandler->SysAudCtrl.FlowStep = 0;
		pstHandler->SysAudCtrl.SysAudModeSts = 0;
	}

	ret = Cec_CmdListInsert(pstHandler, CEC_CMD_SET_SYS_AUDIO_MODE);
	if (ret == 1)
		cec_printf("insert cmd success!\n");
	else
		cec_printf("insert cmd fail!\n");

}

void Cec_SetPowerStatus(stCecHandler *pstHandler, UINT8 tbPwrStatus) //cec power status
{
	pstHandler->GiveDevPwrSts.PwrStatus = tbPwrStatus;
}

//get edid array
UINT8 *HDMI_get_EDID(stCecHandler *pstHandler)
{

	if ((pstHandler == NULL) || (pstHandler->EdidData == NULL))
	{
		cec_printf_E("ptr err");
		return NULL;
	}

	return pstHandler->EdidData;
}

//return hpd state: 1 or 0
int HDMI_get_HPD(stCecHandler *pstHandler)
{
	return pstHandler->HpdStatus;
}

int edid_checksum(const uint8_t *const block)
{
	uint8_t checksum = 0;
	int i;

	for (i = 0; i < EDID_BLOCK_LEN; i++)
		checksum += block[i];

	return (checksum == 0);
}

void Edid_ParseExtension(stCecHandler *pstHandler)
{
	UINT16 mwOffset = EDID_CEA_EXT_OFFSET;
	mwOffset += 4;
	do
	{
		if ((pstHandler->EdidData[mwOffset] >> 5) == 0x03)
		{
			mwOffset += 4;
			pstHandler->EDID_PhyaddrChange = 0;
			if ((pstHandler->PhyAddrAB != pstHandler->EdidData[mwOffset]) || (pstHandler->PhyAddrCD != pstHandler->EdidData[mwOffset + 1]))
			{
				pstHandler->EDID_PhyaddrChange = 1;
			}
			pstHandler->PhyAddrAB = pstHandler->EdidData[mwOffset];
			pstHandler->PhyAddrCD = pstHandler->EdidData[mwOffset + 1];
			pstHandler->PhyAddrRdy = 1;
			int initARCCMD;
			initARCCMD = pstHandler->HPDHighTime - pstHandler->HPDLOWTime;
			if(initARCCMD > HPD_TIMEOUT_INIT)
			{
				Cec_HandlerInit(pstHandler);
			}
			Cec_SetFuncActive(pstHandler);

			cec_printf("get phy addr = 0x%x\n", (pstHandler->PhyAddrAB << 8) | pstHandler->PhyAddrCD);
			break;
		}
		else
		{
			mwOffset += pstHandler->EdidData[mwOffset] & 0x1f;
			mwOffset ++;
		}
	}
	while (mwOffset < (pstHandler->EdidData[EDID_CEA_EXT_OFFSET + 2] + EDID_CEA_EXT_OFFSET));

	cec_printf("phy addr = 0x%x\n", (pstHandler->PhyAddrAB << 8) | pstHandler->PhyAddrCD);

}

BOOL Edid_DdcRead(int stIdx, UINT8 SegPtr, UINT8 DevAddr, int bRegAddr, void *pBuf, UINT32 dLen)
{
	BOOL bRet = FALSE;
	int sdReadStat = -1;
	int res = 0;
    I2cData_t i2cData;

	if (stIdx > 0)
	{
		// 1. set i2c device addr
		// 2. set i2c clock to low speed
		//sdI2COriClk = ioctl(stIdx,GETDEVICECLOCK, 0);
		//cec_printf("get EDID ioctl i2c ori clk: %d \n",sdI2COriClk);

        i2cData.slaveAddr = DevAddr;
        i2cData.cmd = bRegAddr;
        i2cData.cmdLen = 1;
        ioctl(stIdx, SETPARAMETERS, (int32_t)(&i2cData));

		// 3. set i2c repeat start operation (for read 4 blocks EDID)
		if (SegPtr > 0)
		{
			res = ioctl(stIdx, SETREPEATSTART, 1);
			cec_printf("set i2c repeat start operation: %d \n", res);
			res = ioctl(stIdx, SETREPEATSTARTCMD0, SEG_PTR_DDC_ADDR);
			cec_printf("set i2c repeat start cmd 0: %d \n", res);
			res = ioctl(stIdx, SETREPEATSTARTCMD1, SegPtr);
			cec_printf("set i2c repeat start cmd 1: %d \n", res);
		}

#if 0
		res	= ioctl(stIdx, SETDEVICECLOCK, eLowSpd);
		cec_printf("EDID ioctl i2c clk low: %d \n", res);
#endif

		// 4. set i2c register addr

		// 5. read data
		sdReadStat = read(stIdx, pBuf, dLen);

		// 6. disable i2c repeat start operation (for read 4 blocks EDID)
		if (SegPtr > 0)
		{
			res = ioctl(stIdx, SETREPEATSTART, 0);
			cec_printf("set i2c repeat start operation: %d \n", res);
		}
#if 0
		// 5. set to original speed
		res	= ioctl(stIdx, SETDEVICECLOCK, sdI2COriClk);
		cec_printf("EDID ioctl i2c original clk: %d \n", res);
#endif

		if (dLen == (UINT32)sdReadStat)
		{
			bRet = TRUE;
			cec_printf("%s dev addr %x Reg %x OK\n", __func__, DevAddr, bRegAddr);
		}
		else
		{
			bRet = FALSE;
			cec_printf_E("dev addr %x Reg %x NG\n", DevAddr, bRegAddr);
		}
	}

	return (bRet);
}

UINT8 Edid_ReadEdid(stCecHandler *pstHandler, BYTE *pEdidData)
{
	UINT16 mwEdidLen;
	int iAddr;
	int iRetryCnt = 5;
	int I2C2_fd = 0;
	char mbModuleName[16] = {0};

	EDIDI2C3Chk meReadI2cRet = EDID_I2C_Fail;
	UINT8 result = 0;
	mwEdidLen = 0;
	iAddr = 0;

	//config i2c1 for reading EDID
	sprintf( mbModuleName, "C:/%s", pstHandler->CustVar.pbI2cDevStr);
	cec_printf("[%s] i2c path = %s \n", __FUNCTION__, mbModuleName);
	I2C2_fd = open(mbModuleName, O_RDWR, 0);
	cec_printf("open res: %d!!!\n", I2C2_fd);

	while ((I2C2_fd < 0) && (iRetryCnt > 0))
	{
		cec_printf_E("i2c open fail retry: %d!!\n", iRetryCnt);
		iRetryCnt--;
		FullEnvDelayMs(20);
		I2C2_fd = open(mbModuleName, O_RDWR, 0);
	}

	if (I2C2_fd < 0)
	{
		cec_printf_E("i2c open retry fail!!\n");
		return 0;
	}

	meReadI2cRet = Edid_DdcRead(I2C2_fd, 0, EDID_AD, 0, pEdidData, 128 * 2);

	cec_printf("read_i2c_ret: %d\n", meReadI2cRet);
	result = 1;

	if (pEdidData[EXT_FLAG_OFFSET] > 1)
	{
		BYTE BlockIndex = 0;
		BlockIndex = 1;

		meReadI2cRet = Edid_DdcRead(I2C2_fd, 1, EDID_AD, 0, &pEdidData[128 * 2], 128 * 2);
	}


	close(I2C2_fd);
	return result;
}

void dump_section1(const char *const name,
				   BYTE *buffer,
				   const BYTE offset,
				   const BYTE length)
{
	int i = 0;
	BYTE *value;

	value = (buffer + offset);

	if (name != NULL)
		cec_printf0("%s: ", name);

	for (i = 0 ; i < length; i++)
	{
		cec_printf0("0x%02x ", *value++);
	}

	cec_printf0("\n");
}

void dump_edid1(BYTE *buffer)
{
	dump_section1("header",                            buffer, 0x00, 0x08);
	dump_section1("vendor/product ID",     buffer, 0x08, 0x0a);
	dump_section1("EDID ver",      buffer, 0x12, 0x02);
	dump_section1("display param", buffer, 0x14, 0x05);
	dump_section1("color character",             buffer, 0x19, 0x0a);
	dump_section1("established timings",               buffer, 0x23, 0x03);
	dump_section1("std timing",    buffer, 0x26, 0x10);
	dump_section1("timing 0",                 buffer, 0x36, 0x12);
	dump_section1("timing 1",                 buffer, 0x48, 0x12);
	dump_section1("timing 2",                 buffer, 0x5a, 0x12);
	dump_section1("timing 3",                 buffer, 0x6c, 0x12);
	dump_section1("extensions",                        buffer, 0x7e, 0x01);
	dump_section1("checksum",                          buffer, 0x7f, 0x01);

	cec_printf0("\n");
}


UINT8 Edid_ParseEdid(stCecHandler *pstHandler)
{

	if ((pstHandler->EdidData[0] != 0) || (pstHandler->EdidData[1] != 0xff) || (pstHandler->EdidData[2] != 0xff)
			|| (pstHandler->EdidData[3] != 0xff) || (pstHandler->EdidData[4] != 0xff) || (pstHandler->EdidData[5] != 0xff)
			|| (pstHandler->EdidData[6] != 0xff) || (pstHandler->EdidData[7] != 0))
	{
		cec_printf_E("EDID ERROR: Not edid header!!\n");
		return 0;
	}

	if (pstHandler->EdidData[0x7e])
		Edid_ParseExtension(pstHandler);

	if (edid_checksum(pstHandler->EdidData) == 1)
	{
		dump_edid1(pstHandler->EdidData);
#ifdef EDID_DEBUG
		parse_edid_info(pstHandler->EdidData);
#endif
	}
	else
	{
		cec_printf_E("EDID ERROR: check err!!\n");
		return 0;
	}

	return 1;
}

UINT8 Hpd_DetectLevelByGpio(stCecHandler *pstHandler)
{
	UINT8 mbHpdState = 0;

	if (pstHandler->HdmiHpdPinNum == eHW_IO_25)
	{
		//regs0->sft_cfg29 |= (1 << 5); //ADI2 gpio output enable bar
		//regs0->sft_cfg29 &= 0xFFFFF7FB;//ADI2 clean gpio pull low
	}

	mbHpdState = GPIO_Intput_Read(pstHandler->HdmiHpdPinNum);

	if (pstHandler->HdmiHpdPinNum == eHW_IO_25){}
		//regs0->sft_cfg29 |= (1 << 11); //ADI2 gpio pull low

	return mbHpdState;
}
/*
UINT8 Hpd_DetectLevelByAdc(stCecHandler *pstHandler)
{
	INT32 sdRet = 0;
	uint32_t dData = 0;

	(void)pstHandler;

	if (pstHandler->HdmiHpdAdcCh >= eADC_CH0)
		sdRet = adc_read(pstHandler->HdmiHpdAdcCh, &dData);

	//cec_printf2("HPD vol:%d,, %d\n", HpdVol, pstHandler->CustVar.HpdVolThreshold);

	if (sdRet == ADC_SUCCESS && dData > pstHandler->CustVar.HpdVolThreshold)
		return 1;
	else
		return 0;
}
*/
int Hpd_Detect(stCecHandler *pstHandler)
{
	int iHpdState = -1;

	if (pstHandler->CustVar.HpdDetectFunc == eHPD_By_GPIO)
	{
		iHpdState = Hpd_DetectLevelByGpio(pstHandler);
	}
	else if (pstHandler->CustVar.HpdDetectFunc == eHPD_By_ADC)
	{
		//iHpdState = Hpd_DetectLevelByAdc(pstHandler);
	}
	else
	{
		cec_printf_E("HPD pin function error!! %d\n", pstHandler->CustVar.HpdDetectFunc);
	}

	if (iHpdState == 0)
	{
		if (pstHandler->HPDLOWTimeout > 0)
		{
			pstHandler->HPDLOWTimeout --;
			if (pstHandler->HPDLOWTimeout == 0)
			{

				//pstHandler->ArcCtrl.TvSupportArc = 0;
			}
		}
	}

	if ((pstHandler->HpdStatus == 0) && (iHpdState == 1)) //out -> in;
	{
		cec_printf("hdmi plug in: %d\n", pstHandler->HPDLOWTimeout);
		pstHandler->HPDHighTime = GetMSFromSTC();
		cec_printf("pstHandler->HPDHighTime: %d\n", pstHandler->HPDHighTime);
		Cec_GetPhysicalAddr(pstHandler);
		pstHandler->PhyAddrRed = 1;
		pstHandler->HpdStatus = iHpdState;
		return iHpdState;
	}
	else if ((pstHandler->HpdStatus == 1) && (iHpdState == 0)) //in -> out;
	{
		cec_printf("hdmi plug out: %d\n", pstHandler->HPDLOWTimeout);
		pstHandler->PhyAddrRdy = 0;
		pstHandler->LogAddrRdy = 0;
		pstHandler->HPDLOWTimeout = HPD_TIMEOUT_CNT;
		pstHandler->HPDLOWTime = GetMSFromSTC();
		cec_printf("pstHandler->HPDLOWTime: %d\n", pstHandler->HPDLOWTime);
		pstHandler->HpdStatus = iHpdState;
		return iHpdState;
	}

	return (-1);
}

void Cec_GetPhysicalAddr(stCecHandler *pstHandler)
{
	UINT8 mbEdidReadStatus = -1;
	UINT8 mbEdidParseStatus = -1;
	UINT8 loop;
	for (loop = 0; loop < EDID_READ_RETRY; loop++)
	{
		mbEdidReadStatus = Edid_ReadEdid(pstHandler, pstHandler->EdidData);

		if (mbEdidReadStatus == 1) //read edid through I2C successfully
		{
			cec_printf("\nCEC: edid data as follow\n");
			OutputBlock(pstHandler->EdidData, 512);
			mbEdidParseStatus = Edid_ParseEdid(pstHandler);
			if (mbEdidParseStatus == 1)
			{
				cec_printf("\nCEC: Parse edid finish\n");
				break;
			}
			else
			{
				cec_printf("\nCEC: Parse edid error,retry\n");
			}
		}
		else
		{
			OutputBlock(pstHandler->EdidData, 512);
			cec_printf("\nCEC: I2C Read edid error,retry\n");
		}
	}
}

// if pCECEvt_CbFunc is exist, call pCECEvt_CbFunc and post cec evt info to iop service. return 0
// if pCECEvt_CbFunc is null, do nothing and return -1
// in standby mode, we cannot post any event to middleware (iop service)
int Cec_Evt_Post(stCecHandler *pstHandler)
{
	if (pstHandler->pCECEvt_CbFunc != NULL)
	{
		pstHandler->pCECEvt_CbFunc(pstHandler->Rx.Opcode, pstHandler->Rx.Operand, pstHandler->Rx.MsgLen);
		cec_printf("post cec evt\n");
		return 0;
	}
	else
	{
		cec_printf("post cec evt fail\n");
		return -1;
	}
}

void Cec_OpcProcess(stCecHandler *pstHandler) //SpikeAdd Modify 2008
{
	cec_printf("source = %d\n", (pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
	cec_printf("target = %d\n", pstHandler->Rx.Header & CEC_TAR_ADDR_MASK);
	cec_printf("opcode = %2x\n", pstHandler->Rx.Opcode);
	UINT8 i = 0;

	for (i = 0; i < pstHandler->Rx.MsgLen - 2; i++)
	{
		cec_printf("operand[%d] = %2x\n", i, pstHandler->Rx.Operand[i]);
	}

	if (((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != CEC_LOG_ADDR_BROADCAST)
			&& ((pstHandler->Rx.Header & CEC_TAR_ADDR_MASK) != pstHandler->LogAddr))
	{
		return;
	}

	if ((gbCecMsgLen[pstHandler->Rx.Opcode] != 0) && (pstHandler->Rx.MsgLen != gbCecMsgLen[pstHandler->Rx.Opcode])) //if not zero it means that length have been set so it should flow the rule
	{
		//cec_printf("lenght error......................\n");
		return;
	}

	if (!Cec_SupportOpcodeChk(pstHandler))
	{
		cec_printf("Platform Unsupport this CEC Command!\n");
		return;
	}

	if (pstHandler->GiveDevPwrSts.PwrStatus == POWER_STATUS_STANDBY)
	{
		if (Cec_PwrOnOpcodeChk(pstHandler))
		{
			cec_printf("Cec_PwrOnOpcodeChk!\n");
			cec_printf("power_status:%d \n", pstHandler->GiveDevPwrSts.PwrStatus);
			Cec_SetAutoPwrOn(pstHandler, CEC_STS_ACTIVE);
		}
	}

	//Cec_Evt_Post(pstHandler);

	switch (pstHandler->Rx.Opcode)
	{
		case CEC_OPCODE_GIVE_SYS_AUDIO_MODE_STATUS:
			cec_printf("CEC_OPCODE_GIVE_SYS_AUDIO_MODE_STATUS................!!!!!!!\n");
			OPC_GiveSysAudModeSts(pstHandler);
			break;

		case CEC_OPCODE_GIVE_OSD_NAME:
			cec_printf("CEC_OPCODE_GIVE_OSD_NAME................!!!!!!!\n");
			OPC_GiveOsdName(pstHandler);
			break;

		case CEC_OPCODE_GIVE_DEVICE_VENDOR_ID:
			cec_printf("CEC_OPCODE_GIVE_DEVICE_VENDOR_ID................!!!!!!!\n");
			OPC_GiveDevVendorId(pstHandler);
			break;
		case CEC_OPCODE_SET_SYS_AUDIO_MODE:
			cec_printf("CEC_OPCODE_SET_SYS_AUDIO_MODE................!!!!!!!\n");
			OPC_SetAudMode(pstHandler);
			break;

		case CEC_OPCODE_REPORT_ARC_INITIATED:
			cec_printf("CEC_OPCODE_REPORT_ARC_INITIATED\n");
			OPC_ReportArcInitiated(pstHandler);
			break;

		case CEC_OPCODE_REPORT_ARC_TERMINATED:
			cec_printf("CEC_OPCODE_REPORT_ARC_TERMINATED..........\n");
			OPC_ReportArcTerminated(pstHandler);
			break;

		case CEC_OPCODE_REQUEST_ARC_INITIATION:
			cec_printf("CEC_OPCODE_REQUEST_ARC_INITIATION..........\n");
			OPC_RequestArcInitiation(pstHandler);
			break;

		case CEC_OPCODE_REQUEST_ARC_TERMINATION:
			cec_printf("CEC_OPCODE_REQUEST_ARC_TERMINATION.............\n");
			OPC_RequestArcTermination(pstHandler);
			break;

		case CEC_OPCODE_GIVE_AUDIO_STATUS:
			cec_printf("CEC_OPCODE_GIVE_AUDIO_STATUS................!!!!!!!\n");
			OPC_GiveAudStatus(pstHandler);
			break;

		case CEC_OPCODE_FEATURE_ABORT:                 //here maybe a bug
			cec_printf("CEC_OPCODE_FEATURE_ABORT..........\n");
			OPC_FeatureAbort(pstHandler);
			break;

		case CEC_OPCODE_USR_CTL_PRESSED:
			cec_printf("CEC_OPCODE_USR_CTL_PRESSED..........\n");
			OPC_UserCtrlPressed(pstHandler);
			break;

		case CEC_OPCODE_USR_CTL_RELEASED:
			cec_printf("CEC_OPCODE_USR_CTL_RELEASED................!!!!!!!\n");
			OPC_UserCtrlReleased(pstHandler);
			break;

		case CEC_OPCODE_GIVE_DEVICE_PWR_STATUS:
			cec_printf("CEC_OPCODE_GIVE_DEVICE_PWR_STATUS................\n");
			OPC_GiveDevPwrStatus(pstHandler);
			break;

		case CEC_OPCODE_REPORT_PWR_STATUS:
			cec_printf("CEC_OPCODE_REPORT_PWR_STATUS................!!!!!!!\n");
			OPC_ReportPwrStatus(pstHandler);
			break;

		case CEC_OPCODE_SYS_AUDIO_MODE_REQUEST:
			cec_printf("CEC_OPCODE_SYS_AUDIO_MODE_REQUEST.............\n");
			OPC_SysAudModeRequest(pstHandler);
			break;

		case CEC_OPCODE_STANDBY:
			cec_printf("CEC_OPCODE_STANDBY..........\n");
			OPC_Standby(pstHandler);
			break;

		case CEC_OPCODE_GIVE_PHYSICAL_ADDRESS:                //SpikeAdd 20080526 //cec fix b4
			cec_printf("CEC_OPCODE_GIVE_PHYSICAL_ADDRESS................\n");
			OPC_GivePhyAddr(pstHandler);
			break;

		case CEC_OPCODE_GET_CEC_VERSION:
			cec_printf("CEC_OPCODE_GET_CEC_VERSION................!!!!!!!\n");
			OPC_GetCecVersion(pstHandler);
			break;

		case CEC_OPCODE_SET_STREAM_PATH:
			cec_printf("CEC_OPCODE_SET_STREAM_PATH................!!!!!!!\n");
			OPC_SetStreamPath(pstHandler);
			break;

		case CEC_OPCODE_ACTIVE_SOURCE:
			cec_printf("CEC_OPCODE_ACTIVE_SOURCE................\n");                //cec fix b4
			OPC_ActiveSource(pstHandler);
			break;

		case CEC_OPCODE_REQUEST_ACTIVE_SOURCE:
			cec_printf("CEC_OPCODE_ACTIVE_SOURCE................\n");
			OPC_RequestActiveSource(pstHandler);
			break;

		case CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR:
			cec_printf("CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR................\n");
			OPC_RequestShortAudDescriptor(pstHandler);
			break;

		case CEC_OPCODE_ABORT:
			cec_printf("CEC_OPCODE_ABORT................\n");
			GoAbort(pstHandler, ABORT_REASON_REFUSED);
			break;
		case CEC_OPCODE_ROUTING_CHANGE:
		{
			cec_printf("CEC_OPCODE_ROUTING_CHANGE................\n");
			break;
		}
		default:
			cec_printf("default..........\n");
			cec_printf_E(" unsupport opcode: 0x%x\n", (pstHandler->Rx.Opcode));
			GoAbort(pstHandler, ABORT_REASON_UNRECOGNIZED_OPCODE);
			break;
	}

}

void Cec_CmdRead(stCecHandler *pstHandler)
{
	UINT8 mbReadRet = 0;
	//cec_printf("Cec_CmdRead : %d !!!\n",pstHandler->GiveDevPwrSts.PwrStatus);
	Cec_CmdRead_BUS(pstHandler);

	mbReadRet = Cec_ReadFrame(&pstHandler->Rx, pstHandler->GiveDevPwrSts.PwrStatus);
	if (mbReadRet == CEC_ERR_HANDLE_OK)
	{
		if ((gbCecMsgLen[pstHandler->Rx.Opcode] != 0) && (pstHandler->Rx.MsgLen != gbCecMsgLen[pstHandler->Rx.Opcode]))
			return;  // error length
		else
			Cec_ReadListInsert(pstHandler, &pstHandler->Rx);
	}
}

void Cec_CmdRead_BUS(stCecHandler *pstHandler)
{
	UINT8 mbReadRet = 0;
	//cec_printf("Cec_CmdRead : %d !!!\n",pstHandler->GiveDevPwrSts.PwrStatus);
	stCecFrameData RxData;

	mbReadRet =  Cec_CMDReadListGet(&RxData);
	if (mbReadRet == CEC_ERR_HANDLE_OK)
	{
		cec_printf("Cec_CmdRead_BUS : %d !!!\n",mbReadRet);
		if ((gbCecMsgLen[RxData.Opcode] != 0) && (RxData.MsgLen != gbCecMsgLen[RxData.Opcode]))
			return;  // error length
		else
			Cec_ReadListInsert(pstHandler, &RxData);
	}
}




void Cec_CmdProcess(stCecHandler *pstHandler)
{
	//UINT8 mbReadRet = 0;
    if (pstHandler->CecKeyCnt > 0) //for user key press
        pstHandler->CecKeyCnt--;
    else if (pstHandler->CecKeyCode != -1)
    {
        pstHandler->CecKeyCode = -1;
		pstHandler->UserCtrlReleased = CEC_Status_UNREADY;

    }
	if ((pstHandler->CecKeyCnt < 20) && (pstHandler->UserCtrlReleased == CEC_Status_READY)) //for user key press
	{
			pstHandler->CecKeyCode = -1;
			pstHandler->UserCtrlReleased = CEC_Status_UNREADY;
	}


	Cec_SacRequestCounter(pstHandler);
	Cec_ArcRequestCounter(pstHandler);//qiaohan 20140113 for changing to ARC must connect ARC HDMI.

	Cec_CmdRead(pstHandler);

	if (HDMI_GET_BUSY_FLG() || HDMI_GET_WRITE_FLG())//cec is busy
	{
		return;
	}


	switch (pstHandler->Cmd.Id)
	{
		case CEC_CMD_IDLE:
			//cec_printf("CMD IDLE!!!\n");
			//Before Read, it should clr all Argument
			pstHandler->Cmd.State = PROCESS_STEP1;    //spike 20080703 Be carefully,set Cmd.State & Cmd.Count must before reading cmd
			pstHandler->Cmd.Count = 0;               //because after reading than will send and polling again, so you will go to wrong flow
			pstHandler->Rx.MsgLen = 0;

			if (Cec_ReadListGet(pstHandler, &pstHandler->Rx))
			{
				//cec_printf("cec read cmd in IDLE!!!\n");
				Cec_OpcProcess(pstHandler);
			}
			else if (pstHandler->Cmd.List[CEC_FIRST_CMD] != CEC_CMD_NONE)
			{
				// cec_printf("cmd list working!!\n");
				pstHandler->Cmd.Id = pstHandler->Cmd.List[CEC_FIRST_CMD];
				Cec_CmdDelFirst(pstHandler);
			}
			break;

		case CEC_CMD_ACTIVE_SOURCE:
			cec_printf("CEC_CMD_ACTIVE_SOURCE..........\n");
			CMD_ActiveSource(pstHandler);
			break;

		case CEC_CMD_INACTIVE_SOURCE:
			cec_printf("CEC_CMD_INACTIVE_SOURCE..........\n");
			CMD_InactiveSource(pstHandler);
			break;

		case CEC_CMD_ONE_TOUCH_PLAY:
			cec_printf("CEC_CMD_ONE_TOUCH_PLAY..........\n");
			CMD_OneTouchPlay(pstHandler);
			break;

		case CEC_CMD_SET_SYS_AUDIO_MODE:
			cec_printf("CEC_CMD_SET_SYS_AUDIO_MODE..........\n");
			CMD_SetSysAudMode(pstHandler);
			break;

		case CEC_CMD_REPORT_AUDIO_STATUS:
			cec_printf("CEC_REPORT_AUDIO_STATUS..........\n");
			CMD_ReportAudStatus(pstHandler);
			break;

		case CEC_CMD_REPORT_SYS_AUDIO_MODE_STATUS:
			cec_printf("CEC_REPORT_SYS_AUDIO_STATUS..........\n");
			CMD_ReportSysAudModeStatus(pstHandler);
			break;

		case CEC_CMD_INITIATE_ARC:
			cec_printf("CEC_CMD_INITIATE_ARC..........\n");
			CMD_InitiateArc(pstHandler);
			break;

		case CEC_CMD_REPORT_PHYSICAL_ADDRESS:
			cec_printf("CEC_CMD_REPORT_PHYSICAL_ADDRESS..........\n");
			CMD_ReportPhyAddr(pstHandler);
			break;

		case CEC_CMD_SET_OSD_NAME:
			cec_printf("CEC_CMD_SET_OSD_NAME..........\n");
			CMD_SetOsdName(pstHandler);
			break;

		case CEC_CMD_REQUEST_ACTIVE_SOURCE:
			cec_printf("CEC_CMD_REQUEST_ACTIVE_SOURCE..........\n");
			CMD_RequestActiveSrc(pstHandler);
			break;

		case CEC_CMD_DEVICE_VENDOR_ID:
			cec_printf("CEC_CMD_DEVICE_VENDOR_ID..........\n");
			CMD_DevVendorId(pstHandler);
			break;

		//ribin modify for cec test@mantis:274180
		case CEC_CMD_PING_AND_REPORT_AUD_SYS:
			cec_printf("CEC_CMD_PING_AND_REPORT_AUD_SYS..........\n");
			CMD_PingAndReportAudSys(pstHandler);
			break;

		case CEC_CMD_PING_AND_REPORT_PLAYBACK:
			cec_printf("CEC_CMD_PING_AND_REPORT_PLAYBACK..........\n");
			CMD_PingAndReportPlayback(pstHandler);
			break;

		case CEC_CMD_FEATURE_ABORT:                 //SpikeAdd 20080521
			cec_printf("CEC_CMD_FEATURE_ABORT..........\n");
			CMD_FeatureAbort(pstHandler);
			break;

		//ribin modify for cec test@mantis:274180
		case CEC_CMD_PING_AUD_SYS:
			cec_printf("CEC_CMD_PING_AUD_SYS..........\n");
			CMD_PingAudSys(pstHandler);
			break;

		case CEC_CMD_PING_PLAYBACK:
			cec_printf("CEC_CMD_PING_PLAYBACK..........\n");
			CMD_PingPlayback(pstHandler);
			break;

		case CEC_CMD_REPORT_POWER_STATUS:
			cec_printf("CEC_CMD_REPORT_POWER_STATUS..........\n");
			CMD_ReportPwrStatus(pstHandler);
			break;

		case CEC_CMD_TERMINATE_ARC:
			cec_printf("CEC_CMD_TERMINATE_ARC..........\n");
			CMD_TerminateArc(pstHandler);
			break;

		case CEC_CMD_CEC_VERSION:
			cec_printf("CEC_CMD_CEC_VERSION..........\n");
			CMD_CecVersion(pstHandler);
			break;

		case CEC_CMD_GET_TV_POWER_STATUS:
			cec_printf("CEC_CMD_GET_TV_POWER_STATUS..........\n");
			CMD_GetTvPwrStatus(pstHandler);
			break;

		case CEC_CMD_REPORT_SHORT_AUDIO_DESCRIPTOR:
			cec_printf("CEC_REPORT_SHORT_AUDIO_DESCRIPTOR..........\n");
			CMD_ReportShortAudDescriptor(pstHandler);
			break;

		default:
			cec_printf("default..........in Cec_CmdProcess\n");
			cec_printf_E("unsupport cmd_id = 0x%x\n", pstHandler->Cmd.Id);
			break;
	}
}

void Cec_HandlerInit(stCecHandler *pstHandler)
{
	cec_printf("cec normal init...!!!\n");
	HDMI_CLR_READ_FLG();
	HDMI_CLR_WRITE_FLG();
	HDMI_CLR_BUSY_FLG();

	Cec_CmdListInit(pstHandler);
	Cec_LogAddrInit(pstHandler);
	Cec_PhyAddrInit(pstHandler);
	Cec_SysAudCtrlInit(pstHandler);
	Cec_CustVarInit(pstHandler);
	pstHandler->DevOsdNameXfe.DestAddr = 0;

	pstHandler->CecKeyCnt = 0;
	pstHandler->CecKeyCode = -1;

}

int Cec_PinMuxSet(stCecHandler *pstHandler)
{
	int iRes = 0;

	cec_printf("Cec_PinMuxSet\n");

	iRes = Pinmux_Module_Alloc(HDMI_PIN);

	cec_printf("CEC Pinmux_Module_Alloc result: %d\n", iRes);
	iRes = 0;

	// HDMI_HPD pinmux
	pstHandler->HdmiHpdPinNum = Pinmux_ModulePin_Find(HDMI_PIN, HDMI_HPD);
	cec_printf("Pinmux_ModulePin_Find: %d\n", pstHandler->HdmiHpdPinNum);

	if (pstHandler->CustVar.HpdDetectFunc == eHPD_By_GPIO)
	{
		iRes |= Pinmux_GPIO_Alloc(pstHandler->HdmiHpdPinNum, IO_GPIO, IO_RISC);
		//cec_printf("Pinmux_GPIO_Alloc: %d\n", iRes);

		iRes |= GPIO_OE_Set(pstHandler->HdmiHpdPinNum, IO_INPUT);
		//cec_printf("GPIO_OE_Set: %d\n", iRes);

		cec_printf("HPD (GPIO mode): %d, %d\n", iRes, pstHandler->HdmiHpdPinNum);
	}
	/*else if (pstHandler->CustVar.HpdDetectFunc == eHPD_By_ADC)
	{
		if (pstHandler->HdmiHpdPinNum == ADC_GPIO_NUM_CH0)
		{
			//iRes |= adc_init(eADC_CH0, eADC_MODE_POLLING, NULL);
			iRes |= Pinmux_GPIO_Alloc(pstHandler->HdmiHpdPinNum, IO_MODULEMODE, IO_RISC);
			pstHandler->HdmiHpdAdcCh = eADC_CH0;
		}
		else if (pstHandler->HdmiHpdPinNum == ADC_GPIO_NUM_CH1)
		{
			//iRes |= adc_init(eADC_CH1, eADC_MODE_POLLING, NULL);
			iRes |= Pinmux_GPIO_Alloc(pstHandler->HdmiHpdPinNum, IO_MODULEMODE, IO_RISC);
			pstHandler->HdmiHpdAdcCh = eADC_CH1;
		}
		else if (pstHandler->HdmiHpdPinNum == ADC_GPIO_NUM_CH2)
		{
			//iRes |= adc_init(eADC_CH2, eADC_MODE_POLLING, NULL);
			iRes |= Pinmux_GPIO_Alloc(pstHandler->HdmiHpdPinNum, IO_MODULEMODE, IO_RISC);
			pstHandler->HdmiHpdAdcCh = eADC_CH2;
		}
		else
		{
			iRes = -1;
			pstHandler->HdmiHpdAdcCh = -1;
			cec_printf_E("HPD (SAR ADC)init : fail\n");
		}

		cec_printf("HPD (SAR ADC): %d, %d\n", iRes, pstHandler->HdmiHpdPinNum);
	}*/
	else
	{
		cec_printf_E("HPD function error!! %d\n", pstHandler->CustVar.HpdDetectFunc);
	}

	iRes = 0;

	// HDMI_CEC pinmux
	pstHandler->HdmiCecPinNum = Pinmux_ModulePin_Find(HDMI_PIN, HDMI_CEC);
	cec_printf("Pinmux_ModulePin_Find: %d\n", pstHandler->HdmiCecPinNum);

	iRes |= Pinmux_GPIO_Alloc(pstHandler->HdmiCecPinNum, IO_GPIO, IO_IOP);
	cec_printf("Pinmux_GPIO_Alloc: %d\n", iRes);

	iRes |= GPIO_OE_Set(pstHandler->HdmiCecPinNum, IO_OUTPUT);
	cec_printf("GPIO_OE_Set: %d,pstHandler->HdmiCecPinNum :%d \n", iRes,pstHandler->HdmiCecPinNum);


	if (iRes != 0)
	{
		cec_printf_E("driver: CEC PinMux init Fail!!!\n");
	}
	else
	{
		cec_printf("driver: CEC PinMux init OK!!!\n");
	}

	return iRes;
}

int HDMI_PollingCec(stCecHandler *pstHandler)
{
	Cec_CmdProcess(pstHandler);

	return (0);
}

int HDMI_PollingHpd(stCecHandler *pstHandler)
{
	return Hpd_Detect(pstHandler);
}

//initialize CEC HW before CEC_SwInit()
int CEC_HwInit(stCecHandler *pstHandler)
{
	int iRes = 0;

	CFG_CEC_T *pstCEC_cfg = GET_SDK_CFG(CFG_CEC_T);
	//20170704 Leon
	// RGST_IOP_REG->iop_data[4] |= 0x0100 ;
    // IOP_HDMISendCmd(CMD_HDMI_ENABLE,IOP_HDMI_ENABLE);

	if ((pstCEC_cfg->pbI2C_DevStr == NULL) ||
	    (strlen(pstCEC_cfg->pbI2C_DevStr) > 8)) {
		cec_printf_E(" pbI2cDevStr err !!" );
		return -1;
	}

	pstHandler->CustVar.HpdDetectFunc = pstCEC_cfg->HPD_DetectFunc;
	pstHandler->CustVar.HpdVolThreshold = pstCEC_cfg->HPD_VolThreshold;
	pstHandler->CustVar.pbI2cDevStr = pstCEC_cfg->pbI2C_DevStr;

	iRes = Cec_PinMuxSet(pstHandler);

	if (iRes == 0) {
		pstHandler->HwRdy = 1;
		cec_printf("HW initial OK \n");
	} else {
		cec_printf_E("driver: CEC CEC_HwInit Fail!!!\n");
	}

	return iRes;
}

// this API only initializes CEC/ARC protocol, does not includes HW part of CEC bus
int CEC_NormalInitiate(stCecHandler *pstHandler)
{
	if (pstHandler->HwRdy != 1)
	{
		cec_printf_E("driver: CEC_NormalInitiate Fail!!!\n");
		return -1;
	}

	Cec_HandlerInit(pstHandler);
	pstHandler->HPDLOWTimeout = 0;

	if ((pstHandler->SysSrc == eSysSrc_Others) && (!pstHandler->CustVar.CecAlwaysRun))
	{
		pstHandler->CecEnable = CEC_STS_INACTIVE;
	}
	else
	{
		pstHandler->CecEnable = CEC_STS_ACTIVE;
	}
	pstHandler->PingMSG_Num = 0;
	Cec_SetPowerStatus(pstHandler, POWER_STATUS_POWER_ON); //set power status before Cec_SetFuncActive()
	Cec_SetFuncActive(pstHandler);
	Cec_SetAutoPwrOn(pstHandler, CEC_STS_INACTIVE);

	return 0;
}

int CEC_StandbyInitiate(void)
{
	stCecHandler *pstCECHandler_;

	pstCECHandler_ = CEC_GetCecHandler();

	CEC_HwInit(pstCECHandler_);
	Cec_HandlerInit(pstCECHandler_);
	Cec_SetPowerStatus(pstCECHandler_, POWER_STATUS_STANDBY); //set power status before Cec_SetFuncActive()
	Cec_SetFuncActive(pstCECHandler_);
	Cec_SetAutoPwrOn(pstCECHandler_, CEC_STS_INACTIVE);
	pstCECHandler_->pCECEvt_CbFunc = NULL;// in standby mode, we cannot post any event to middleware (iop service)

	return 0;
}

int Hpd_StandbyModeDetect(stCecHandler *pstHandler)
{
	cec_printf("hdmi plug in\n");
	Cec_GetPhysicalAddr(pstHandler);
	pstHandler->HpdStatus = 1;
	return 1;
}

int CEC_StandbyModeGetPowerState(void)
{
	//UINT8 mbPwrState = 0;
	stCecHandler *pstCECHandler_;
	int iRes = 0;

	int HPDdetectRes = 0;

	pstCECHandler_ = CEC_GetCecHandler();

	if (pstCECHandler_ == NULL)
	{
		cec_printf_E("CEC handler error !!!\n");
		return -1;
	}

	//pstCECHandler_->GiveDevPwrSts.PwrStatus = POWER_STATUS_POWER_ON;
	pstCECHandler_->PhyAddrRdy = 1;
	pstCECHandler_->SysSrc = eSysSrc_ARC;

	Cec_HandlerInit(pstCECHandler_);

	HPDdetectRes = Hpd_StandbyModeDetect(pstCECHandler_);


	//Cec_SetAutoPwrOn(pstCECHandler_, CEC_STS_ACTIVE);



	//Cec_CmdProcess(pstCECHandler_);

	//mbPwrState = Cec_GetAutoPwrOn(pstCECHandler_);

	cec_printf("HPD state : %d, detect : %d\n", pstCECHandler_->HpdStatus, HPDdetectRes);

	//pstCECHandler_->CecEnable = CEC_STS_ACTIVE;

	return iRes;

}

/*
int CEC_StandbyModeGetPowerSt(void)
{

    stCecHandler* pstCECHandler_;
   	int iRes = 0;


    pstCECHandler_ = CEC_GetCecHandler();

    if (pstCECHandler_ == NULL)
    {
        cec_printf_E("CEC handler error !!!\n");
        return -1;
    }

    Cec_CmdProcess(pstCECHandler_);

	return iRes;
}
*/
int CEC_AudioStatusReport(stCecHandler *pstHandler)
{
	int iRes = 0;

	pstHandler->SysAudCtrl.DestAddr[1] = ((pstHandler->Rx.Header & CEC_SRC_ADDR_MASK) >> 4);
	iRes = Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_AUDIO_STATUS);

	return iRes;
}

//return 1 means power on cmd is recived, 0 means nothing
int CEC_GetPowerState(void)
{
	UINT8 mbPwrState = 0;
	stCecHandler *pstCECHandler_;
	int iRes = CEC_RET_NO_ENTER_STANDBY;
	int HPDdetectRes = 0;
	volatile int *HDMI_Wakeup_Flag = (int *)SRAM_ADDR_EP_WAKE_UP_FLAG;
	//cec_printf("CEC_GetPowerState!!\n");

	pstCECHandler_ = CEC_GetCecHandler();
	HPDdetectRes = Hpd_Detect(pstCECHandler_);

	if (pstCECHandler_->CustVar.HpdWakeUpEnable == 1)
	{
		if (HPDdetectRes == 1) // HPD change to 1 from 0
		{
			Cec_SetAutoPwrOn(pstCECHandler_, CEC_STS_ACTIVE);
		}
		else if (HPDdetectRes == 0) // HPD change to 0 from 1
		{
			Cec_SetAutoPwrOn(pstCECHandler_, CEC_STS_INACTIVE);
		}
	}

	Cec_CmdProcess(pstCECHandler_);

	mbPwrState = Cec_GetAutoPwrOn(pstCECHandler_);

    if ((pstCECHandler_->Cmd.List[0] == CEC_CMD_NONE) && (HDMI_GET_WRITE_FLG() == 0)
        && (HDMI_GET_READ_FLG() == 0) && (pstCECHandler_->Cmd.Id == CEC_CMD_IDLE)) {       // 20200105 mingjin.he read CEC  CMD not CEC_GET_READ_FLG()
        if (pstCECHandler_->CecWakeChk.BusState == CEC_BUS_BUSY) {
            // cec_printf("cec bus is free!\n");
            pstCECHandler_->CecWakeChk.BusState = CEC_BUS_FREE;
            pstCECHandler_->CecWakeChk.Time.Previous = GetMSFromSTC();
            if (mbPwrState) {
                cec_printf("Get stc time,start counting to power on!!\n");
            }
        }
    } else {
        pstCECHandler_->CecWakeChk.BusState = CEC_BUS_BUSY;
        // cec_printf("Cec bus is not free! Wait!\n");
    }

    if (pstCECHandler_->CecWakeChk.BusState == CEC_BUS_FREE) {
        pstCECHandler_->CecWakeChk.Time.Current = GetMSFromSTC();
        pstCECHandler_->CecWakeChk.Time.Interval = pstCECHandler_->CecWakeChk.Time.Current - pstCECHandler_->CecWakeChk.Time.Previous;
        if (pstCECHandler_->CecWakeChk.Time.Interval > CEC_TERMINATED_DELAY_TIME) {
            pstCECHandler_->CecWakeChk.BusState = CEC_BUS_FREE_FOR_LONG_TIME;
        }
    }

    if (pstCECHandler_->CecWakeChk.BusState == CEC_BUS_FREE_FOR_LONG_TIME) {
        if (mbPwrState) {
            iRes = CEC_RET_WAKEUP;
            *HDMI_Wakeup_Flag = CEC_RET_WAKEUP;
            cec_printf("Cec wakeup system!!\n");
        } else {
            iRes = CEC_RET_ENTER_STANDBY;
			 *HDMI_Wakeup_Flag = CEC_RET_ENTER_STANDBY;
        }
    }

	return iRes;
}

int CEC_VolConvert(UINT8 tbVol, UINT8 tbMaxVol)
{
	int iRes = 0;

	iRes = ((tbVol & CEC_VOL_MASK) * 100 / tbMaxVol);

	return iRes;
}

int CEC_StandbyTerminate(stCecHandler *pstHandler)//standby will first run CEC_NormalTerminater(),then run CEC_StandbyTerminate()
{
	int iRes = 0;

	cec_printf("CEC_StandbyTerminate!!!\n");
	if (pstHandler->HpdStatus == 1)
	{
		if (pstHandler->ArcCtrl.TvSupportArc)
		{
			iRes = Cec_CmdListInsert(pstHandler, CEC_CMD_TERMINATE_ARC);	//mingjin
			//Cec_CmdListInsert(pstHandler, CEC_CMD_REPORT_PHYSICAL_ADDRESS);
		}
		if (pstHandler->SysAudCtrl.SysAudModeSts)
			Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_INACTIVE);
		if (pstHandler->DeviceActive)
			iRes = Cec_CmdListInsert(pstHandler, CEC_CMD_INACTIVE_SOURCE);
	}

	pstHandler->CecEnable = CEC_STS_INACTIVE;
	pstHandler->CecTrmtChk.TrmtSucceed = 0;
	pstHandler->ArcCtrl.RequstArcTimeout = 0;

	pstHandler->CecKeyCnt = 0;
	pstHandler->CecKeyCode = -1;
	cec_printf("HDP_state: %d\n", pstHandler->HpdStatus);

	return iRes;
}

int CEC_NormalTerminate(stCecHandler *pstHandler)//switch to other source or press"pause" key will run this function
{
	int iRes = 0;

	cec_printf("CEC_NormalTerminate!!!\n");
	if (pstHandler->HpdStatus == 1)
	{
		Cec_CmdListInit(pstHandler);
		if (pstHandler->SysSrc == eSysSrc_ARC)
		{
			iRes = Cec_CmdListInsert(pstHandler, CEC_CMD_TERMINATE_ARC);
			if (pstHandler->SysAudCtrl.SysAudModeSts)
			{
				Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_INACTIVE);
			}
		}
		else if (pstHandler->SysSrc == eSysSrc_HDMI_Tx)
		{
			if (pstHandler->DeviceActive)
				iRes = Cec_CmdListInsert(pstHandler, CEC_CMD_INACTIVE_SOURCE);
				//CEC_SYN_LoglAddr(pstHandler);
			Cec_CmdListInsert(pstHandler, CEC_CMD_PING_AUD_SYS);
		}
	}

	pstHandler->CecEnable = CEC_STS_INACTIVE;

	pstHandler->CecTrmtChk.TrmtSucceed = 0;
	pstHandler->ArcCtrl.RequstArcTimeout = 0;

	pstHandler->CecKeyCnt = 0;
	pstHandler->CecKeyCode = -1;

	return iRes;
}

// CEC/ARC is completely terminated if 1 is returned
int CEC_TerminatedCheck(stCecHandler *pstHandler)
{
	int iRes = 0;
	if(pstHandler->CustVar.CecAlwaysRun == CEC_STS_INACTIVE)
	{
		if (pstHandler->CecEnable == CEC_STS_INACTIVE)
		{
			if (!pstHandler->CecTrmtChk.TrmtSucceed)
			{
				if ((pstHandler->Cmd.List[0] == CEC_CMD_NONE) && (HDMI_GET_WRITE_FLG() == 0)
						&& (HDMI_GET_READ_FLG() == 0) && (pstHandler->Cmd.Id == CEC_CMD_IDLE))
				{
					//cec_printf("cec bus is free!\n");
					if (!pstHandler->CecTrmtChk.BusFree)
					{
						pstHandler->CecTrmtChk.BusFree = 1;//bus free
						pstHandler->CecTrmtChk.Time.Previous = GetMSFromSTC();
						cec_printf("Get stc time,start counting!!\n");
					}
				}
				else
				{
					pstHandler->CecTrmtChk.BusFree = 0;//bus not free
					//cec_printf("Cec bus is not free! Wait!\n");
				}

				if (pstHandler->CecTrmtChk.BusFree) //bus free
				{
					pstHandler->CecTrmtChk.Time.Current = GetMSFromSTC();
					pstHandler->CecTrmtChk.Time.Interval = pstHandler->CecTrmtChk.Time.Current - pstHandler->CecTrmtChk.Time.Previous;
					if (pstHandler->CecTrmtChk.Time.Interval > CEC_TERMINATED_DELAY_TIME)
					{
						iRes = 1;
						pstHandler->CecTrmtChk.TrmtSucceed = 1;
						cec_printf("CEC Terminated successfully!\n");
					}
					else
					{
						//cec_printf("GetTimeFlag = 1,keep waiting until terminated delay time!\n");
					}
				}
			}
			else
			{
				iRes = 1;
			}
		}
		else
		{
			iRes = -1;
		}
	}
	else
	{
		iRes = -1;
	}
	return iRes;
}

int CEC_StandbyChk(stCecHandler *pstHandler)
{
	int iRes = 0;
	iRes = pstHandler->CECStandby;
	pstHandler->CECStandby = 0;
	return iRes;
}

int CEC_ARCSourceChk(stCecHandler *pstHandler)
{
	int iRes = 0;
	iRes = pstHandler->CEC_state_change;
	 pstHandler->CEC_state_change = 0;
	return iRes;
}

int CEC_SetDSPChk(stCecHandler *pstHandler)
{
	int iRes = 0;
	iRes = pstHandler->CEC_SET_DSP_change;
	 pstHandler->CEC_SET_DSP_change = 0;
	return iRes;
}



int CEC_SetSac(stCecHandler *pstHandler, UINT8 Status)
{
	int iRes = 0;

	if (Status)
	{
		cec_printf("set system audio control on!!!\n");
		Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_ACTIVE);
	}
	else
	{
		cec_printf("set system audio control off!!\n");
		Cec_SetSysAudioCtrlStatus(pstHandler, CEC_STS_INACTIVE);
	}

	return iRes;
}

// set system source info to CEC handler. it will decide the role of cec logic address
//return:
//set success-> return the param you set
//set fail-> return -1
int CEC_SetSysSrc(stCecHandler *pstHandler, UINT8 SysSrcParam)
{
	if (SysSrcParam >= eSysSrc_Max)
	{
		cec_printf_E("error: %d\n", SysSrcParam);
		return -1;
	}

	pstHandler->SysSrc = SysSrcParam;
	return SysSrcParam;
}

int CEC_SYN_LoglAddr(stCecHandler *pstHandler)
{
	pstHandler->LogAddr = CEC_LOG_ADDR_AUDIO_SYSTEM;
	Cec_SetLoglAddr1(pstHandler->LogAddr);
	Cec_SetLoglAddr2(pstHandler->LogAddr);
	return 0;
}

