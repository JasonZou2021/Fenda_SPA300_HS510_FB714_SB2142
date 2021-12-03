/*****************************************************

Copyright (C) 2014 Sunplus technology co.Ltd.All right reserved!

 ******************************************************
File Name:TAS5711.c
Version:
Author:chaofa.yan
Created:2014/06/03
Last Modified:
Description:
Public Functions:

History Information Description:
Date                    Author                 Modification
2014/06/03        chaofa.yan                Created file


******************************************************/


/*----------------------------------------------*
  * INCLUDE  DECLARATIONS
  *----------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "pinmux.h"
//#include "i2c_sw_driver.h"
#include "fsi_api.h"
//#include "std_interfaces.h"
#include "busy_delay.h"
#include "amp.h"
#include "exstd_interfaces.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[AMP]"
#include "log_utils.h"
#include "i2c_api.h"

/*----------------------------------------------*
  * MACRO DECLARATIONS
  *----------------------------------------------*/
#define SUBVOLUME_EXGAIN 0xC
#define SETDEVICEADDR  (1)
#define TAS5711_PDN_GPIO (8)
#define TAS5711_RESET_GPIO (7)
/*----------------------------------------------*
  * EXTERNAL REFERENCES
  *----------------------------------------------*/

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/

typedef enum _eTas5711LRChl
{
	eLOnly,
	eROnly,
} eTas5711LRChl;

typedef enum _eTas5711RegAddr
{
	eClkCtl = 0x00,
	eDevID,
	eErrStat,
	eSysCtl1,
	eSerialData,
	eSysCtl2,
	eSoftMute,
	eMasterVol,

	eCh1Vol,
	eCh2Vol,
	eCh3Vol,
	eVolCfg = 0x0e,

	eModulationLimit = 0x10,
	eICDelayCh1,
	eICDelayCh2,
	eICDelayCh3,
	eICDelayCh4,

	ePWMSDG = 0x19,

	eStartStopPeriod = 0x1A,
	eOscillatorTrim,
	eBKNDErr,

	eInputMux = 0x20,
	eCh4SrcSel,
	ePWMMux = 0x25,

	eCh1Bq0 = 0x29,
	eCh1Bq1,
	eCh1Bq2,
	eCh1Bq3,
	eCh1Bq4,
	eCh1Bq5,
	eCh1Bq6,

	eCh2Bq0 = 0x30,
	eCh2Bq1,
	eCh2Bq2,
	eCh2Bq3,
	eCh2Bq4,
	eCh2Bq5,
	eCh2Bq6,

	eDrc1AE = 0x3A,
	eDrc1AA,
	eDrc1AD,
	eDrc2AE,
	eDrc2AA,
	eDrc2AD,

	eDrcCtrl = 0x46,

	eCh1OutPutMixer = 0x51,
	eCh2OutPutMixer = 0X52,


	eSubChBQ0 = 0x5A,
	eSubChBQ1,
} eTas5711RegAddr;

/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/
//INT32 gTas5711fd = -1;


int Tas5711_AMP_INIT(AmpList *pstAmpList);
int Tas5711_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5711_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5711_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5711_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume );
int Tas5711_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume );
int Tas5711_AMP_MUTE_ON(AmpList *pstAmpList);
int Tas5711_AMP_MUTE_OFF(AmpList *pstAmpList);
int Tas5711_AMP_LR_CHANNEL( AmpList *pstAmpList);
int Tas5711_AMP_SET_SUBMODE( AmpList *pstAmpList);
int Tas5711_AMP_SET_STEREOMODE( AmpList *pstAmpList);
int Tas5711_AMP_SET_PBTLMODE( AmpList *pstAmpList);
int Tas5711_AMP_DRC_CTRL( AmpList *pstAmpList);
int Tas5711_AMP_POWER_OFF( AmpList *pstAmpList);

AMPOP_T	 g_stTas5711 =
{
	Tas5711_AMP_INIT,
	Tas5711_AMP_SET_VOL,
	Tas5711_AMP_SET_LR_VOL,
	Tas5711_AMP_SET_SUB_VOL,
	Tas5711_AMP_SET_CH1_VOL,
	Tas5711_AMP_SET_CH2_VOL,
	Tas5711_AMP_MUTE_ON,
	Tas5711_AMP_MUTE_OFF,
	Tas5711_AMP_LR_CHANNEL,
	Tas5711_AMP_SET_SUBMODE,
	Tas5711_AMP_SET_STEREOMODE,
	Tas5711_AMP_SET_PBTLMODE,
	Tas5711_AMP_DRC_CTRL,
	Tas5711_AMP_POWER_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
};

void Tas5711_AMP_delay_ms(int ms)
{
	msleep(ms);
}

/****************************************************
Function:Tas5711Write
Description:
Input:UINT8 bRegAddr
         UINT8 bData
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/06/09           Created             chaofa.yan
Others:

*****************************************************/
static int Tas5711Write(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
{
    I2cData_t i2cData;

    i2cData.buf = bBuf;
    i2cData.cmd = bRegAddr;
    i2cData.cmdLen = 1;
    i2cData.dataLen = dLen;
    i2cData.slaveAddr = stIdx.SlaveAddr;
    i2cData.showMsg = 0;

    return I2C_Write(stIdx.pbI2cModule, &i2cData);
}

/****************************************************
Function:Tas5711Read
Description:
Input:UINT8 bRegAddr
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/06/09           Created             chaofa.yan
Others:

*****************************************************/
static int Tas5711Read(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
{
    I2cData_t i2cData;

    i2cData.buf = bBuf;
    i2cData.cmd = bRegAddr;
    i2cData.cmdLen = 1;
    i2cData.dataLen = dLen;
    i2cData.slaveAddr = stIdx.SlaveAddr;
    i2cData.showMsg = 0;

    return I2C_Read(stIdx.pbI2cModule, &i2cData);
}

/****************************************************
Function:PowerOnTas5711
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/06/09           Created             chaofa.yan
Others:

*****************************************************/
static void PowerOnTas5711(void)
{
	GPIO_Output_Write(TAS5711_PDN_GPIO, IO_LOW);

	Tas5711_AMP_delay_ms(2);

	GPIO_Output_Write(TAS5711_RESET_GPIO, IO_LOW);

	Tas5711_AMP_delay_ms(2);

	GPIO_Output_Write(TAS5711_PDN_GPIO, IO_HIGH);

	Tas5711_AMP_delay_ms(30);

	GPIO_Output_Write(TAS5711_RESET_GPIO, IO_HIGH);

	Tas5711_AMP_delay_ms(30);//wait at least 12ms
}


static void PowerOffTas5711(void)
{
	GPIO_Output_Write(TAS5711_PDN_GPIO, IO_LOW);

	Tas5711_AMP_delay_ms(5);//at least 2ms

	GPIO_Output_Write(TAS5711_RESET_GPIO, IO_LOW);
}

/****************************************************
Function:SetTas5711Output
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/06/09           Created             chaofa.yan
Others:

*****************************************************/
static void SetTas5711Output( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;

	abData = 0x0;
	Tas5711Write(pstAmpList->peList[eIdx], eOscillatorTrim, &abData, sizeof(abData));

	Tas5711_AMP_delay_ms(100);//wait at least 50ms


	abData = 0xff; // mute master vol
	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

	abData = 0xd1;// sub channel vol = register 0xA
	Tas5711Write(pstAmpList->peList[eIdx], eVolCfg, &abData, sizeof(abData));

	abData = 0x42;// set master vol
	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

	abData = 0x0;// exit shut down mode
	Tas5711Write(pstAmpList->peList[eIdx], eSysCtl2, &abData, sizeof(abData));

}


static void SetTas5711Mode2_1( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData = 0x84;
	BYTE abData4[4] = {0x1, 0x1, 0x32, 0x45};
	BYTE _abData4[4] = {0x0, 0x0, 0x42, 0x03};
	//BYTE abData20[20] = {0x0,0x0,0x01,0xff,0x0,0x0,0x3,0xff,0x0,0x0,0x1,0xff,0x0,0xfd,0x27,0xff,0x0f,0x82,0xd0,0x01};//low pass 120HZ
	//BYTE abData20[20] = {0x0,0x0,0x05,0x83,0x0,0x0,0xb,0x06,0x0,0x0,0x5,0x83,0x0,0xfb,0x42,0xc1,0xf,0x84,0xa7,0x33};// low pass 200HZ
	BYTE abData20[20] = {0x0, 0x0, 0x08, 0x92, 0x0, 0x0, 0x11, 0x25, 0x0, 0x0, 0x8, 0x92, 0x0, 0xfa, 0x13, 0x89, 0xf, 0x85, 0xca, 0x2b}; // low pass 250HZ

	Tas5711Write(pstAmpList->peList[eIdx], ePWMMux, abData4, sizeof(abData4));

	Tas5711Write(pstAmpList->peList[eIdx], eCh4SrcSel, _abData4, sizeof(_abData4));

	Tas5711Write(pstAmpList->peList[eIdx], eSubChBQ0, abData20, sizeof(abData20));

	Tas5711Write(pstAmpList->peList[eIdx], eSysCtl2, &abData, sizeof(abData));

	Tas5711Read(pstAmpList->peList[eIdx], eSubChBQ0, abData20, sizeof(abData20));

}

static void SetTas5711Mode2_0( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData = 0x0;
	BYTE abData4[4] = {0x1, 0x2, 0x13, 0x45};

	Tas5711Write(pstAmpList->peList[eIdx], ePWMMux, abData4, sizeof(abData4));

	Tas5711Write(pstAmpList->peList[eIdx], eSysCtl2, &abData, sizeof(abData));

}

static void SetTas5711PBTLMode( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData = 0x3a;
	BYTE abData4[4] = {0x1, 0x10, 0x32, 0x45};
	BYTE abData12[12] = {0x0, 0x40, 0x0, 0x0, 0x0, 0x40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

	Tas5711Write(pstAmpList->peList[eIdx], ePWMSDG, &abData, sizeof(abData));

	Tas5711Write(pstAmpList->peList[eIdx], ePWMMux, abData4, sizeof(abData4));

	Tas5711Write(pstAmpList->peList[eIdx], eCh1OutPutMixer, abData12, sizeof(abData12));

	Tas5711Write(pstAmpList->peList[eIdx], eCh2OutPutMixer, abData12, sizeof(abData12));

}


static void SetTas5711DRCCtrl( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData4[4] = {0x0, 0x0, 0x0, 0x3};

	Tas5711Write(pstAmpList->peList[eIdx], eDrcCtrl, abData4, sizeof(abData4));

	Tas5711Read(pstAmpList->peList[eIdx], eDrcCtrl, abData4, sizeof(abData4));


}

//Input multiplexer register
static void SetTas5711LRChannel( AmpList  *pstAmpList, eAmpIdx eIdx, eTas5711LRChl eLR)
{
	BYTE abData4[4] = {0};

	if (eLR == eLOnly)
	{
		abData4[0] = 0x0;
		abData4[1] = 0x0;
		abData4[2] = 0x77;
		abData4[3] = 0x72;
		Tas5711Write(pstAmpList->peList[eIdx], eInputMux, abData4, sizeof(abData4));
	}
	else if (eLR == eROnly)
	{
		abData4[0] = 0x0;
		abData4[1] = 0x11;
		abData4[2] = 0x77;
		abData4[3] = 0x72;
		Tas5711Write(pstAmpList->peList[eIdx], eInputMux, abData4, sizeof(abData4));
	}
}



static void SetTas5711Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{

	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &bGain, sizeof(bGain));
	//Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
	//Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
static void SetTas5711LRVolume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
	Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}

static void SetTas5711Ch1Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
}

static void SetTas5711Ch2Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

static void SetTas5711SubVolume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	Tas5711Write(pstAmpList->peList[eIdx], eCh3Vol, &bGain, sizeof(bGain));
}

static void SetTas5711Mute( AmpList  *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x7;

	//printfw("%s \n",__FUNCTION__);
	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5711Write(pstAmpList->peList[i], eSoftMute, &bData, sizeof(bData));
	}
}

static void SetTas5711DeMute( AmpList  *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5711Write(pstAmpList->peList[i], eSoftMute, &bData, sizeof(bData));
	}
}

int GetTas5711ErrStatus( AmpList *pstAmpList, BYTE *pStatus)
{
	UINT32 i = 0;
	BYTE bData = 0xff;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		bData = 0xff;
		Tas5711Read(pstAmpList->peList[i], eErrStat, &bData, sizeof(bData));
		pStatus[i] = bData;
	}

	return (0);
}

/****************************************************
Function:Tas5711Init
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2014/06/09           Created             chaofa.yan
Others:

*****************************************************/
static BOOL Tas5711Init(AmpList *pstAmpList)
{
	BOOL bRet = FALSE;
	//char * pbModule = NULL;
	//char bModuleName[MODULENAMELEN] = {0};
	UINT32 i = 0;

	//printfw("%s\n",__FUNCTION__);

	if((TAS5711_PDN_GPIO == -1) || (TAS5711_RESET_GPIO == -1))
		return FALSE;

	// init TAS5711 reset pin and pdn pin
	if ((Pinmux_GPIO_CheckStatus(TAS5711_PDN_GPIO) == 0) && (Pinmux_GPIO_CheckStatus(TAS5711_RESET_GPIO) == 0))
	{
		if (Pinmux_GPIO_Alloc(TAS5711_PDN_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(TAS5711_PDN_GPIO, IO_OUTPUT);
		}

		if (Pinmux_GPIO_Alloc(TAS5711_RESET_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(TAS5711_RESET_GPIO, IO_OUTPUT);
		}
	}
	else
	{
		return FALSE;
	}

#if 0 // do not open i2c here
	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		if (pbModule != NULL)
		{
			if (strcmp(pstAmpList->peList[i].pbI2cModule, pbModule) == 0)
			{
				//printf("[%s] pbI2cModule = %s \n",__FUNCTION__,pstAmpList->peList[i].pbI2cModule);
				pstAmpList->peList[i].sdI2cHandle =  pstAmpList->peList[i - 1].sdI2cHandle;
				continue;
			}
		}

		pbModule = pstAmpList->peList[i].pbI2cModule;
		//printf("[%s] pbModule = %s \n",__FUNCTION__,pbModule);
		sprintf( bModuleName, "C:/%s", pbModule);
		//printf("[%s] i2c path = %s \n",__FUNCTION__,bModuleName);

		pstAmpList->peList[i].sdI2cHandle = open(bModuleName, O_RDWR, 0);
		if (pstAmpList->peList[i].sdI2cHandle < 0)
		{
			LOGE("open %s module false !\n", pstAmpList->peList[i].pbI2cModule);
		}
		else
		{
			//printf("sdI2cHandle = %d \n",pstAmpList->peList[i].sdI2cHandle);
		}
	}
#endif
	//gTas5711fd = open("C:/i2c2",O_RDWR,0);
	//if(gTas5711fd > 0)
	//{
	PowerOnTas5711();

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Output(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	Tas5711_AMP_delay_ms(300);

	BYTE pState[2];

	GetTas5711ErrStatus(pstAmpList, pState);

	bRet = TRUE;

	if ((pState[0] != 0) || (pState[1] != 0))
	{
		LOGE("Tas5711 error!! error code: 0x%x, 0x%x\n", pState[0], pState[1]);
	}


	//}
	return (bRet);
}

int Tas5711_AMP_INIT( AmpList *pstAmpList)
{
	INT32 sdRet =  -1;
	if (Tas5711Init(pstAmpList) == TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}

int Tas5711_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->MainGainTbl;
	bStepNum = pstAmpList->pstAmpCfg->MainGainStepNum;


	if (bVolume >= bStepNum)
	{
		return -1;
	}

	//printfw("AMP bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
int Tas5711_AMP_SET_LR_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch1_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch1GainStepNum;

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	//printfw("AMP LR_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711LRVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5711_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch1_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch1GainStepNum;

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	LOGD("AMP CH1_bVolume=%d 0x%x \n", bVolume, bVolTable[bVolume]);

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Ch1Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5711_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch2_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch2GainStepNum;

	LOGD("AMP CH2_bVolume=%d 0x%x \n", bVolume, bVolTable[bVolume]);

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Ch2Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5711_AMP_SET_SUB_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch3_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch3GainStepNum;

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	//printfw("AMP SUB_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711SubVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

int Tas5711_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetTas5711Mute(pstAmpList);

	return (0);
}

int Tas5711_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetTas5711DeMute(pstAmpList);

	return (0);
}

int Tas5711_AMP_LR_CHANNEL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711LRChannel(pstAmpList, pstAmpList->peList[i].eAmpID, eLOnly);
	}

	return (0);
}

int Tas5711_AMP_SET_SUBMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Mode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int Tas5711_AMP_SET_STEREOMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711Mode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int Tas5711_AMP_SET_PBTLMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711PBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int Tas5711_AMP_DRC_CTRL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5711DRCCtrl(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}


int Tas5711_AMP_POWER_OFF( AmpList *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x40;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5711Write(pstAmpList->peList[i], eSysCtl2, &bData, sizeof(bData));		//first enter shutdown mode
	}

	Tas5711_AMP_delay_ms(170); // get delay time by reg 0x1a

	PowerOffTas5711();

	return (0);
}

