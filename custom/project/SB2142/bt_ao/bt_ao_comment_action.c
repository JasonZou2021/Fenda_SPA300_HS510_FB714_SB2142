#ifndef __BT_AO_COMMENT_ACTION_C__
#define __BT_AO_COMMENT_ACTION_C__
/******************************************************************************/
/**
*
* \file	bt_ao.c
*
* \brief	8107 bt middle ware ao implement .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author	 jie.jia@sunplus.com
* \version	 v0.01
* \date 	 2016/06/28
******************************************************************************/
#include <stdio.h>
#include "types.h"
#include "initcall.h"
#include "std_interfaces.h"
#define LOG_EXTRA_STR  "[AO]"
#define LOG_MODULE_ID  ID_BT
#include "log_utils.h"
#include "bt_mw_core.h"
#include "bt_ao_comment_action.h"
#include "bt_ao.h"
#include "bt_ao_service_api.h"


//extern void BtRegServiceCBK(void);
//extern void BtUnRegServiceCBK(void);



void BtDoRfPowerSet(DBM_LEVEL_e level)
{
	//BtServiceRfPowerSet(level);
	(void)level;
}


int BtDoUdtTest(void)
{
	//return BtServiceUdtTestOn();
	return 0;
}


void BtDoStartSearch(void)
{
	//BtServiceStartSearch();
	SDK_BASE_IF_StartSearch();
}


void BtDoStopSearch(void)
{
	//BtServiceStopSearch();
	SDK_BASE_IF_StopSearch();
}


void BtDoPairStart(BtAddr_t *pBtAddr)
{
	//BtServicePairStart(pBtAddr);
	t_bdaddr *pAddress = (t_bdaddr *)pBtAddr;
	SDK_BASE_IF_StartPair(pAddress);
}


void BtDoUnpair(BtAddr_t *pBtAddr)
{
	//BtServiceUnpair(pBtAddr);
	(void)pBtAddr;

}

void BtDoAddBondDev(BTPairDev *pPairDev)
{
	BTPairDev stPairDev = *pPairDev;
	LOGD("BtDoAddBondDev Link_Key:0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\n",
				stPairDev.linkKey[0],stPairDev.linkKey[1],stPairDev.linkKey[2],stPairDev.linkKey[3],
				stPairDev.linkKey[4],stPairDev.linkKey[5],stPairDev.linkKey[6],stPairDev.linkKey[7],
				stPairDev.linkKey[8],stPairDev.linkKey[9],stPairDev.linkKey[10],stPairDev.linkKey[11],
				stPairDev.linkKey[12],stPairDev.linkKey[13],stPairDev.linkKey[14],stPairDev.linkKey[15]);
	LOGD("BtDoAddBondDevaddr...0x%02x%02x%02x%02x%02x%02x...\n",
				stPairDev.bd_addr.address[0],stPairDev.bd_addr.address[1],
				stPairDev.bd_addr.address[2],stPairDev.bd_addr.address[3],
				stPairDev.bd_addr.address[4],stPairDev.bd_addr.address[5]);
	LOGD("master:%d\n",stPairDev.master);
	LOGD("cod:%d\n",stPairDev.cod);
	LOGD("linkKeyType:%d\n",stPairDev.linkKeyType);
	SDK_BASE_AddBondedDevice(pPairDev);

}

void BtDoClearBondDev(void)
{
	//BtServiceBtDoClearBondDev();
	SDK_BASE_ClearBondedDevice();
}

void BtDoSppConnect(BtAddr_t *pBtAddr)
{
	//BtServiceSppConnect(pBtAddr);
	(void)pBtAddr;
}

int BtDoSppConnectSamsung(BtAddr_t *pBtAddr,unsigned char pin)
{
	u_int8 SDP_SPP_UUID_12[16] = {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0x12};
	u_int8 SDP_SPP_UUID_11[16] = {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0x11};
	//u_int8 SDP_SPP_UUID_FB[16] = {0x00,0x00,0x11,0x01,0x00,0x00,0x10,0x00,0x80,0x00,0x00,0x80,0x5F,0x9B,0x34,0xFB};
	//int iRet = BtServiceSppConnectSamsung(pBtAddr,pin);

	if(pin == 0x12)
	{
		bt_ao_printf("BtDoSppConnectSamsung... pin:%d...\n",pin);
		SDK_SPP_IF_Connect(pBtAddr, 0, SDP_SPP_UUID_12);
	}
	else if(pin == 0x11)
	{
		bt_ao_printf("BtDoSppConnectSamsung... pin:%d...\n",pin);
		SDK_SPP_IF_Connect(pBtAddr, 0, SDP_SPP_UUID_11);
	}
	/*if(iRet == -1)
	{
		bt_ao_error_printf("Spp Connect Fail,Please Check !!!");
	}
	else if(iRet == -2)
	{
		bt_ao_error_printf("Spp Connect Fail,Please pair first !!!");
	}*/
	//return iRet;
	return 0;
}


void BtDoSppDisconnect(BtAddr_t *pBtAddr)
{
	//BtServiceSppDisconnect(pBtAddr);
	(void)pBtAddr;
	Connected_Device *stConnected_Device = NULL;
	int i;
	Connected_Device *pConnected_Device_List = Connected_Device_List_Get();

	for(i=0;i<Connected_Device_Max;i++)
	{
		if(!(memcmp(&pConnected_Device_List[i].bt_addr,pBtAddr,6)))
		{
			stConnected_Device = &pConnected_Device_List[i];
			break;
		}
	}


	if(stConnected_Device == NULL)
	{
		bt_ao_ntc_printf("can not find device!!!");
		return;
	}

	bt_ao_ntc_printf("[%s],spp_port:%d",__FUNCTION__,stConnected_Device->spp_port);
	SDK_SPP_IF_Disconnect(stConnected_Device->spp_port);
}

void BtDoSppDataOut(void* pParam)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	stBtSppData*  pstBtSppData = (stBtSppData*)pParam;
	//Sunplus//BtServiceSppDataOut((void*)pParam)
	Connected_Device *stConnected_Device = NULL;
	int i;
	Connected_Device *pConnected_Device_List = Connected_Device_List_Get();

	for(i=0;i<Connected_Device_Max;i++)
	{
		BLOCKPRINTF(&pConnected_Device_List[i].bt_addr,6);
		BLOCKPRINTF(&pstBtSppData->BtAddr,6);
		if(!(memcmp(&pConnected_Device_List[i].bt_addr,&pstBtSppData->BtAddr,6)))
		{
			stConnected_Device = &pConnected_Device_List[i];
			break;
		}
	}


	if(stConnected_Device == NULL)
	{
		bt_ao_ntc_printf("can not find device!!!");
		return;
	}

	bt_ao_ntc_printf("[%s],spp_port:%d",__FUNCTION__,stConnected_Device->spp_port);
	SDK_SPP_IF_Send (pstBtSppData->pBtData, pstBtSppData->len,stConnected_Device->spp_port);
}


void BtDoScanModeSet(SCAN_MODE_e mode)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//BtServiceSetScanMode(mode);
	if(mode == SCAN_NONE)
	{
		SDK_BASE_IF_Change_Connectable(OFF);
		SDK_BASE_IF_Change_Discoverable(OFF);
	}
	else if(mode == ONLY_CONNECTABLE)
	{
		SDK_BASE_IF_Change_Connectable(ON);
		SDK_BASE_IF_Change_Discoverable(OFF);
	}
	else if(mode == BOTH_DISCOVERY_CONNECTABLE)
	{
		SDK_BASE_IF_Change_Discoverable((ON));
		SDK_BASE_IF_Change_Connectable(ON);
	}
}


void BtDoScanModeGet(SCAN_MODE_e *pMode)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//BtServiceGetScanMode(pMode);
    (void)pMode;
}


int BtDoInit(void)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(BtServiceInit() == SUCCESS)
	{
		BtRegServiceCBK();
	}
	else
	{
		bt_ao_error_printf("BtDoInit: _InitBt fail !");
		return FAIL;
	}
	return SUCCESS;*/
	init_cbk_task();
	return SUCCESS;
}


int BtDoDeInit(void)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(SUCCESS == BtServiceDelInit())
	{
		BtUnRegServiceCBK();
	}
	else
	{
		bt_ao_error_printf("BtDoDeInit fail !");
		return FAIL;
	}

	return SUCCESS;*/
	return SUCCESS;
}

//device id
#define BT_LOCALEVICE_VENDORID			0x04E8
#define BT_LOCALDEVICE_PRODUCTID		0x9000
#define BT_LOCALDEVICE_VERSION			0x0000
#define BT_LOCALDEVICE_VERDORIDSOURCE 	0x0100

#define BD_ADDRESS \
0x02,0x00,0x10,0x83,0x15,0x00,\


#define LICENSE \
0x0a,0xca,0x94,0xa0,\
0x79,0x40,0x62,0xed,\
0x52,0x69,0x95,0xfb,\
0xbc,0x37,0xb8,0x76,\
0x40,0xbf,0x31,0x11,\
0x4c,0xf1,0x2f,0xcd,\
0x87,0x6b,0x81,0x22,\
0x80,0x1c,0x93,0xe5,\
0xf7,0x8a,0x81,0xb4,\
0xf8,0xad,0xe9,0xac,\
0xb5,0x90,0x2f,0xa4,\
0xc7,0xe1,0x4b,0xb6,\

typedef struct
{
	UINT8 license[48];
    UINT8 name[16];
	UINT8 addr[6];
}BtToolSetting;

extern INT32 SettingManagerBT_Info(void *ptr, int len);

int BtDoEnable(int *pResult,Init_Parm st_Init_Parm)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);

    //SDK_BASE_IF_SetLogLevel(0);  //level = 0 for closing lib_bluetooth.a log
    //SDK_Music_IF_EnableA2DPAAC(0); //is_enable = 0 for disabling AAC

	UINT8 eir[240];///version

	UINT8 eir_len = 0;

	///set eir data of 2 items
	/*UINT16 uuid16[3] = {BT_SERVICE_CLASS_AVRCP_CT, BT_SERVICE_CLASS_AUDIO_SINK, BT_SERVICE_CLASS_SERIAL_PORT};
	eir_len += SDK_BASE_IF_Make_EIRServiceUUID(eir+eir_len, sizeof(eir)-eir_len, BT_EIR_DATATYPE_COMPLETE_LIST_OF_16BIT_SERVICE_UUIDS,
										       uuid16, 3);
	SDK_BASE_IF_Set_EIRData(eir, eir_len);*/

	UINT8 addr_swap;
	BtToolSetting BtInfo;
	BtToolSetting BtSetting;
	BtToolSetting TempBtInfo;

	memset(&BtInfo,0x00,sizeof(BtToolSetting));
	memset(&BtSetting,0x00,sizeof(BtToolSetting));
	memset(&TempBtInfo,0xff,sizeof(BtToolSetting));

	SettingManagerBT_Info(&BtInfo,sizeof(BtToolSetting));

	if((memcmp(TempBtInfo.addr,BtInfo.addr,sizeof(BtInfo.addr)) != 0)
		&&(memcmp(BtSetting.addr,BtInfo.addr,sizeof(BtInfo.addr)) != 0))
	{
		   addr_swap = BtInfo.addr[0];
					BtInfo.addr[0] = BtInfo.addr[5];
					BtInfo.addr[5] = addr_swap;

		   addr_swap = BtInfo.addr[1];
					BtInfo.addr[1] = BtInfo.addr[4];
					BtInfo.addr[4] = addr_swap;

		   addr_swap = BtInfo.addr[2];
					BtInfo.addr[2] = BtInfo.addr[3];
					BtInfo.addr[3] = addr_swap;

		bt_ao_ntc_printf("Tool bt address: \n");
		SDK_BASE_IF_Set_LocalBtAddr(BtInfo.addr);
		BLOCKPRINTF(BtInfo.addr,ADDR_LENGTH);
	}
	else
	{
	    bt_ao_ntc_printf("bt address: \n");
		SDK_BASE_IF_Set_LocalBtAddr(st_Init_Parm.LocalAddr.address);
        BLOCKPRINTF(st_Init_Parm.LocalAddr.address,ADDR_LENGTH);
	}

	if((memcmp(TempBtInfo.license,BtInfo.license,sizeof(BtInfo.license)) != 0)
		&&(memcmp(BtSetting.license,BtInfo.license,sizeof(BtInfo.license)) != 0))
	{
	   bt_ao_ntc_printf("Tool bt licnese: \n");
	   SDK_BASE_IF_Write_License(BtInfo.license);
	   BLOCKPRINTF(BtInfo.license,48);
	}
	else
	{
	    bt_ao_ntc_printf("bt licnese: \n");
		SDK_BASE_IF_Write_License(st_Init_Parm.license);
		BLOCKPRINTF(st_Init_Parm.license,48);
	}

    if((memcmp(TempBtInfo.name,BtInfo.name,sizeof(BtInfo.name)) != 0)
		&&(memcmp(BtSetting.name,BtInfo.name,sizeof(BtInfo.name)) != 0))
	{
	   BtInfo.name[16] = 0x0;
	   bt_ao_ntc_printf("Tool bt name: %s\n",BtInfo.name);

       SDK_BASE_IF_Change_LocalName(BtInfo.name);
	   eir_len += SDK_BASE_IF_Make_EIRLocalName(eir+eir_len,sizeof(eir)-eir_len,BtInfo.name,strlen(BtInfo.name ));
	   SDK_BASE_IF_Set_EIRData(eir, eir_len);
	}
	else
	{
    	bt_ao_ntc_printf("bt name: %s\n",st_Init_Parm.LocalName);
		SDK_BASE_IF_Change_LocalName(st_Init_Parm.LocalName);
		eir_len += SDK_BASE_IF_Make_EIRLocalName(eir+eir_len,sizeof(eir)-eir_len,st_Init_Parm.LocalName,strlen(st_Init_Parm.LocalName));
		SDK_BASE_IF_Set_EIRData(eir, eir_len);
	}

	///set eir Device ID
	UINT16 vendor_id_src =st_Init_Parm.verdoridSource;
	UINT16 vendor_id= st_Init_Parm.BtDeviceId_Vid;
	UINT16 product_id= st_Init_Parm.BtDeviceId_Pid;
	UINT16 version= st_Init_Parm.BtDeviceId_Version;

	eir_len +=  SDK_BASE_IF_Make_EIRDeviceID(eir+eir_len, sizeof(eir)-eir_len, vendor_id_src, vendor_id, product_id, version);

	SDK_BASE_IF_Set_EIRData(eir, eir_len);

	//SDK_BASE_IF_Set_LocalBtName(st_Init_Parm.LocalName);
	//SDK_AVRCP_CtProviderName(st_Init_Parm.avrcpCtProviderName, strlen(st_Init_Parm.avrcpCtProviderName));
	//SDK_AVRCP_CtServiceName(st_Init_Parm.avrcpCtServiceName, strlen(st_Init_Parm.avrcpCtServiceName));

	if(TRUE == SDK_BASE_IF_BtServerInit())
	{
		*pResult = SUCCESS;
	}
	else
	{
		bt_ao_error_printf("BtDoEnable: BtServiceEnable fail!");
		*pResult = FAIL;
		return FAIL;
	}

	return SUCCESS;
}




int BtDoDisable(int *pResult)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(SUCCESS == BtServiceDisable())
	{
		*pResult = SUCCESS;
	}
	else
	{
		bt_ao_error_printf("BtDoDisable: BtServiceDisable fail!");
		*pResult = FAIL;
		return FAIL;
	}

	return SUCCESS;*/
	(void)*pResult;

	SDK_Bchs_IF_DisableBT();
	#if 0
	if(SUCCESS == BtServiceDisable())
	{
		*pResult = SUCCESS;
	}
	else
	{
		bt_ao_error_printf("BtDoDisable: BtServiceDisable fail!");
		*pResult = FAIL;
		return FAIL;
	}
	#endif
	return SUCCESS;
}





int BtDoGetProfileState(BtAddr_t *pBtAddr,BT_PROFILE_e profile,BT_PROFILE_STATE_e *pstate)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(pBtAddr != NULL)
	{
		return BtServiceGetProfileState(pBtAddr,profile,pstate);
	}

	return FAIL;*/
	(void)pBtAddr;
	(void)profile;
	(void)pstate;
	return 0;
}


int BtDoGetRemoteDevInfo(BtAddr_t *pBtAddr,stBtDevInfo *pBtDev)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(pBtAddr != NULL)
	{
		return BtServiceGetRemoteDevInfo(pBtAddr,pBtDev);
	}

	return FAIL;*/
	(void)pBtAddr;
	(void)pBtDev;
	return 0;
}


int BtDoGetRemoteDevNum(int *pNum)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//return BtServiceGetRemoteDevNum(pNum);
	(void)pNum;
	return 0;
}


int BtDoGetBondDevNum(int *pBondedNum)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//return BtServiceBondNum(pBondedNum);
	(void)pBondedNum;
	return 0;

}


int BtDoGetBondByIdex(int sdBondedNum,stBtDevInfo *pBtDev)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//return BtServiceGetBondByIdex(sdBondedNum,pBtDev);
	(void)sdBondedNum;
	(void)pBtDev;
	return 0;
}


void BtDoSendUserCmd(BtAddr_t *pBtAddr,UINT8 length,UINT8 *pUserCmdData)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//BtServiceSendUserCmd(pBtAddr,length,pUserCmdData);
	(void)pBtAddr;
	(void)length;
	(void)pUserCmdData;
}

int BtDoGetRemoteDevByIdex(int dex,stBtDevInfo *pBtDev)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	//Sunplus//return BtServiceGetRemoteDevByIdex(dex,pBtDev);
	(void)dex;
	(void)pBtDev;
	return 0;
}


int BtDoLocalNameGet(char *pName)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	/*if(pName != NULL)
	{
		BtServiceGetLocalName(pName);
		return SUCCESS;
	}

	return FAIL;*/
	(void)pName;
	return 0;
}


int BtDoLocalNameSet(char *pName)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pName != NULL)
	{
		//BtServiceSetLocalName(pName);
		//SDK_BASE_IF_Set_LocalBtName(pName);
		return SUCCESS;
	}

	return FAIL;
}

int BtDoLocalLmpNameSet(char *pName)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pName != NULL)
	{
		//BtServiceSetLocalLmpName(pName);
		SDK_BASE_IF_Change_LocalLmpName(pName);
		return SUCCESS;
	}

	return FAIL;
}


int BtDoLocalDeviceClassSet(unsigned int *pDeviceClass)
{
	/*if(pDeviceClass != NULL)
	{
		BtServiceSetLocalDeviceClass(pDeviceClass);
		return SUCCESS;
	}

	return FAIL;*/
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	SDK_BASE_IF_Set_ClassOfDevice(*pDeviceClass);

	return 0;
}

int BtDoRemoveSdpDeviceIDGet(BtAddr_t*pBtAddr)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pBtAddr != NULL)
	{
		LOGD("[0x%02x][0x%02x][0x%02x][0x%02x][0x%02x][0x%02x]......\n",
			pBtAddr->address[0],pBtAddr->address[1],pBtAddr->address[2],
			pBtAddr->address[3],pBtAddr->address[4],pBtAddr->address[5]);
		//BtServiceGetRemoveSdp_DeviceID(pBtAddr);
		SDK_SDP_IF_Remote_DeviceID(pBtAddr);
		return SUCCESS;
	}

	return FAIL;
}


int BtDoLocalAddrGet(BtAddr_t*pBtAddr)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pBtAddr != NULL)
	{
		//BtServiceGetLocalAddr(pBtAddr);
		SDK_BASE_IF_Get_LocalBtAddr();
		return SUCCESS;
	}

	return FAIL;
}


int BtDoLocalAddrSet(BtAddr_t *pBtAddr)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pBtAddr != NULL)
	{
		//BtServiceSetLocalAddr(pBtAddr);
		return SUCCESS;
	}

	return FAIL;
}


int BtDoGetLocalInfo(BtLocalInfo *pdev)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pdev != NULL)
	{
		//BtServiceGetLocalInfo(pdev);
		return SUCCESS;
	}

	return FAIL;
}


int BtSetPinCode(char *pPinCode)
{
	int res = FAIL;

	if(pPinCode != NULL)
	{
		//res = BtServiceSetPinCode(pPinCode);
	}

	return res;
}

int BtReplyPinCode(BtAddr_t* pBtAddr,char *pPinCode)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	int res = FAIL;

	if(pPinCode != NULL && pPinCode != NULL)
	{
		//res = BtServiceReplyPinCode(pBtAddr,pPinCode);
		t_pincode_info stPincode;
		stPincode.bdAddress = *(t_bdaddr *)pBtAddr;
		stPincode.pincode_lenth = strlen(pPinCode);
		memcpy(stPincode.pincode,pPinCode,stPincode.pincode_lenth);

		res = SDK_BASE_IF_pincode_response(&stPincode);
	}

	return res;
}


int BtDoGetAudioState(BtAddr_t *pBtAddr,BT_IR_CMD_e *pstate)
{
	bt_ao_ntc_printf("[%s] call in",__FUNCTION__);
	if(pBtAddr != NULL && pstate != NULL)
	{
		//BtServiceGetAudioState(pBtAddr,pstate);
		SDK_Music_IF_PlayStatus();
		return SUCCESS;
	}

	return FAIL;
}


int IsBtDoSearching(int *pIsTrue)
{
	/*if(BtServiceIsSearching())
	{
		*pIsTrue = TRUE;
	}
	else
	{
		*pIsTrue = FALSE;
	}*/

	return *pIsTrue;
}


int BtDenyConnect(BtAddr_t *pBtAddr,ENABLE_STATE_e state)
{
	/*if(pBtAddr != NULL)
	{
		return BtServiceSetDenyConnect(pBtAddr,state);
	}
	return FAIL;*/
	(void)pBtAddr;
	(void)state;
	return 0;

}

int BtDoGetRemoteDevRssi(BtAddr_t *pBtAddr)
{
	/*if(pBtAddr != NULL)
	{
		return BtServiceGetRemoteDeviceRssi(pBtAddr);
	}

	return FAIL;*/
	(void)pBtAddr;
	return 0;
}

#endif

