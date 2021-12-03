/*****************************************************

Copyright (C) 2014 Sunplus technology co.Ltd.All right reserved!

 ******************************************************
File Name:AD83584D.c
Version:
Author:qingyu.wu
Created:2016/10/24
Last Modified:
Description:
Public Functions:

History Information Description:
Date                    Author                 Modification
2016/10/24        qingyu.wu                Created file


******************************************************/


/*----------------------------------------------*
  * INCLUDE  DECLARATIONS
  *----------------------------------------------*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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
#define AD83584D_PDN_GPIO  (21)
#define AD83584D_RESET_GPIO (20)
#define SUBVOLUME_EXGAIN 0xC
#define SETDEVICEADDR  (1)

/*----------------------------------------------*
  * EXTERNAL REFERENCES
  *----------------------------------------------*/
//#define Mode2_1_CH
/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/

typedef struct _AmpAD83584D
{
	UINT8 bAmpIdx;//eAmp1, eAmp2 ...    eAmpIdx bAmpIdx
	UINT8 bAD83584DAddr;
} AmpAD83584D;

typedef enum _eAD83584DLRChl
{
	eLOnly,
	eROnly,
} eAD83584DLRChl;

typedef enum _eAD83584DRegAddr
{
	eStateCtl1 = 0x00,
	eStateCtl2,
	eStateCtl3,
	eMasterVol,
	eCh1Vol,
	eCh2Vol,
	eCh3Vol,
	eBasstone,
	eTrebletone,
	eBassManagement,
	eStateCtl4,
	eCh1Cfg,
	eCh2Cfg,
	eCh3Cfg,
	eDrcRate = 0x0e,

	eStateCtl5 = 0x11,
	ePvddVoltage,
	eAddrNoiseGain,

	//0x14 - 0x24	user-defined


	ePowerSaving = 0x2A,
	eVolumeFineTune,
} eAD83584DRegAddr;

/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/

//INT32 gAD83584Dfd = -1;


int AD83584D_AMP_INIT(AmpList *pstAmpList);
int AD83584D_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD83584D_AMP_SET_CH1_VOL(AmpList *pstAmpList, BYTE  bVolume );
int	AD83584D_AMP_SET_CH2_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD83584D_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD83584D_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD83584D_AMP_MUTE_ON(AmpList *pstAmpList);
int AD83584D_AMP_MUTE_OFF(AmpList *pstAmpList);
int AD83584D_AMP_LR_CHANNEL( AmpList *pstAmpList);
int AD83584D_AMP_SET_SUBMODE( AmpList *pstAmpList);
int AD83584D_AMP_SET_STEREOMODE( AmpList *pstAmpList);
int AD83584D_AMP_SET_PBTLMODE( AmpList *pstAmpList);
int AD83584D_AMP_DRC_CTRL( AmpList *pstAmpList);
int AD83584D_AMP_POWER_OFF( AmpList *pstAmpList);


AMPOP_T	 g_stAD83584D =
{
	AD83584D_AMP_INIT,
	AD83584D_AMP_SET_VOL,
	AD83584D_AMP_SET_LR_VOL,
	AD83584D_AMP_SET_SUB_VOL,
	AD83584D_AMP_SET_CH1_VOL,
	AD83584D_AMP_SET_CH2_VOL,
	AD83584D_AMP_MUTE_ON,
	AD83584D_AMP_MUTE_OFF,
	AD83584D_AMP_LR_CHANNEL,
	AD83584D_AMP_SET_SUBMODE,
	AD83584D_AMP_SET_STEREOMODE,
	AD83584D_AMP_SET_PBTLMODE,
	AD83584D_AMP_DRC_CTRL,
	AD83584D_AMP_POWER_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
};




/****************************************************
Function:AD83584DWrite
Description:
Input:UINT8 bRegAddr
         UINT8 bData
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2016/10/24           Created             qingyu.wu
Others:

*****************************************************/
static int AD83584DWrite(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:PowerOnAD83584D
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2016/10/24           Created            qingyu.wu
Others:

*****************************************************/
static void PowerOnAD83584D(void)
{

	GPIO_Output_Write(AD83584D_RESET_GPIO, IO_LOW);

	GPIO_Output_Write(AD83584D_PDN_GPIO, IO_LOW);

	delay_1ms(20);

	GPIO_Output_Write(AD83584D_RESET_GPIO, IO_HIGH);

	delay_1ms(20);

	GPIO_Output_Write(AD83584D_PDN_GPIO, IO_HIGH);
	delay_1ms(20);
}


static void PowerOffAD83584D(void)
{
	GPIO_Output_Write(AD83584D_PDN_GPIO, IO_LOW);
	delay_1ms(35);//at least 35ms

	GPIO_Output_Write(AD83584D_RESET_GPIO, IO_LOW);
}

/****************************************************
Function:SetAD84584DOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2016/10/24          Created             qingyu.wu
Others:

*****************************************************/
static void SetAD83584DOutput(const AmpList *const pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;

	abData = 0x0;
	AD83584DWrite(pstAmpList->peList[eIdx], eStateCtl1, &abData, sizeof(abData));

	abData = 0x0;
	AD83584DWrite(pstAmpList->peList[eIdx], eStateCtl3, &abData, sizeof(abData));

	abData = 0x34;
	AD83584DWrite(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));
	abData = 0x0;
	AD83584DWrite(pstAmpList->peList[eIdx], eCh1Vol, &abData, sizeof(abData));
	abData = 0x0;
	AD83584DWrite(pstAmpList->peList[eIdx], eCh2Vol, &abData, sizeof(abData));
	abData = 0x0;
	AD83584DWrite(pstAmpList->peList[eIdx], eCh3Vol, &abData, sizeof(abData));
	delay_1ms(100);//wait at least 50ms

}




static void SetAD83584DMode2_1(const AmpList *const pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;

	abData = 0x3a;
	AD83584DWrite(pstAmpList->peList[eIdx], eStateCtl5, &abData, sizeof(abData));
}

static void SetAD83584DMode2_0(const AmpList *const pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;

	abData = 0x32;
	AD83584DWrite(pstAmpList->peList[eIdx], eStateCtl5, &abData, sizeof(abData));

}

static void SetAD83584DPBTLMode(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
	return;
}


static void SetAD83584DDRCCtrl(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
	return;

}


static void SetAD83584DLRChannel(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED eAD83584DLRChl eLR)
{
	return;
}



static void SetAD83584DVolume(const AmpList *const pstAmpList, eAmpIdx eIdx, BYTE bGain)
{

	AD83584DWrite(pstAmpList->peList[eIdx], eMasterVol, &bGain, sizeof(bGain));
	//	AD83584DWrite(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
	//	AD83584DWrite(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));

}


//jun.yang for volume adjustment system (mantis:0262891)20150929
static void SetAD83584DLRVolume(const AmpList *const pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD83584DWrite(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
	AD83584DWrite(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}

static void SetAD83584DCh1Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD83584DWrite(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
}

static void SetAD83584DCh2Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD83584DWrite(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}


//jun.yang for volume adjustment system (mantis:0262891)20150929

static void SetAD83584DSubVolume(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
	return;
}

static void SetAD83584DMute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x0f;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD83584DWrite(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


static void SetAD83584DDeMute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x00;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD83584DWrite(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


/****************************************************
Function:AD83584DInit
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2016/11/30           Created             qingyu.wu
Others:

*****************************************************/
static BOOL AD83584DInit(AmpList *pstAmpList)
{
	BOOL bRet = FALSE;
	//char * pbModule = NULL;
	//char bModuleName[MODULENAMELEN] = {0};
	UINT32 i = 0;
	LOGD("AD83584DInit\n");

	//LOGE("%s\n",__FUNCTION__);

	// init AD83584D reset pin and pdn pin
	if ((Pinmux_GPIO_CheckStatus(AD83584D_PDN_GPIO) == 0) && (Pinmux_GPIO_CheckStatus(AD83584D_RESET_GPIO) == 0))
	{
		if (Pinmux_GPIO_Alloc(AD83584D_PDN_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD83584D_PDN_GPIO, IO_OUTPUT);
		}

		if (Pinmux_GPIO_Alloc(AD83584D_RESET_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD83584D_RESET_GPIO, IO_OUTPUT);
		}
	}
	else
	{
		return FALSE;
	}

#if 0// do not open i2c here
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
	//gAD83584D = open("C:/i2c2_sw",O_RDWR,0);
	//if(gAD83584Dfd > 0)
	//{
	PowerOnAD83584D();

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DOutput(pstAmpList, pstAmpList->peList[i].eAmpID);

#ifdef Mode2_1_CH
		SetAD83584DMode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
#else
		SetAD83584DMode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
#endif
	}

	bRet = TRUE;
	//}
	return (bRet);
}

int AD83584D_AMP_INIT( AmpList *pstAmpList)
{
	INT32 sdRet =  -1;
	if (AD83584DInit(pstAmpList) == TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}

int AD83584D_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	BYTE *bVolTable;
	BYTE bStepNum;


	bVolTable = pstAmpList->pstAmpCfg->MainGainTbl;
	bStepNum = pstAmpList->pstAmpCfg->MainGainStepNum;

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}



	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
int AD83584D_AMP_SET_LR_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD83584DLRVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}
	return 0;
}

int AD83584D_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD83584DCh1Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int AD83584D_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD83584DCh2Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int AD83584D_AMP_SET_SUB_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD83584DSubVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}
	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

int AD83584D_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetAD83584DMute(pstAmpList);

	return (0);
}

int AD83584D_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetAD83584DDeMute(pstAmpList);

	return (0);
}

int AD83584D_AMP_LR_CHANNEL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DLRChannel(pstAmpList, pstAmpList->peList[i].eAmpID, eLOnly);
	}

	return (0);
}

int AD83584D_AMP_SET_SUBMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DMode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD83584D_AMP_SET_STEREOMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DMode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD83584D_AMP_SET_PBTLMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DPBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD83584D_AMP_DRC_CTRL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD83584DDRCCtrl(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}


int AD83584D_AMP_POWER_OFF( AmpList *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x8f;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD83584DWrite(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));			//first enter shutdown mode
	}

	delay_1ms(170); // get delay time by reg 0x1a

	PowerOffAD83584D();

	return (0);
}


