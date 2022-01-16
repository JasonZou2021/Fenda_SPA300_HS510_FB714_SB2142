/*****************************************************

Copyright (C) 2020 Sunplus technology co.Ltd.All right reserved!

 ******************************************************
File Name:TAS5825M.c
Version:
Author:yuanwei.liang
Created:2020/11/19
Last Modified:
Description:
Public Functions:

History Information Description:
Date                    Author                 Modification
2020/11/19       yuanwei.liang                Created file


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
#include "hw_gpio_list.h"
#include "i2c_api.h"
#include "user_def.h"

/*----------------------------------------------*
  * MACRO DECLARATIONS
  *----------------------------------------------*/
//#define TAS5825M_PDN_GPIO  (21)
//#define TAS5825M_RESET_GPIO (20)
//#define SUBVOLUME_EXGAIN 0xC
#define SETDEVICEADDR  (1)

#ifdef Q70_DEMO
	#define TAS5825M_PDN_GPIO  (eHW_IO_9)
#else
	#define TAS5825M_PDN_GPIO  (eHW_IO_8)
#endif
#define TAS5825M_HW_MUTE_GPIO  (eHW_IO_35)
#define TAS5825M_FAULT_GPIO  (eHW_IO_10)


/*----------------------------------------------*
  * EXTERNAL REFERENCES
  *----------------------------------------------*/

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/





typedef enum _eTAS5825MRegAddr
{
    //page 0x00, book 0x00
    ePage_Select = 0x00,
	eResetCtrl = 0x01,
	eDevCtrl1,
	eDevCtrl2,

	eI2cPageInc = 0x0f,

	eSigChCtrl = 0x28,
	eClockDetCtrl,
	eSDoutSel,
	eI2SCtrl,

	eSapCtrl1=0x33,
	eSapCtrl2,
	eSapCtrl3,

	eFSMon = 0x37,
	eBCKMon,
	eClkDetStatus,

	eMasterVol = 0X4c,
	eDigVol2,
	eDigVol3,
	eAutoMuteCtrl,
	eAutoMuteTime,

	eANACtrl = 0x53,
	eAGain,

	eBQWrCtrl1 = 0x5c,
	eDACCtrl,

	eADRPinCtrl = 0x60,
	eADRPinConf,

	eDspMisc = 0x66,
	eDIEID,
	ePowerState,
	eAutoMuteState,
	ePhaseCtrl,
	eSSCtrl0,
	eSSCtrl1,
	eSSCtrl2,
	eSSCtrl3,
	eSSCtrl4,
	eChanFault,
	eGlobalFault1,
	eGlobalFault2,
	eOTWarning,
	ePinCtrl1,
	ePinCtrl2,
	eMiscCtrl,

	eFaultClear = 0x78,

	eBook_Select = 0x7f,

	//page :0x0b, book: 0x8c
	eLchVol = 0x0c,
	eRchVol = 0x10,

}eTAS5825MRegAddr;



/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/
//INT32 gTas5825Mfd = -1;


int Tas5825M_AMP_INIT(AmpList *pstAmpList);
int Tas5825M_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5825M_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5825M_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5825M_AMP_SET_CH1_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5825M_AMP_SET_CH2_VOL(AmpList *pstAmpList, BYTE  bVolume );
int Tas5825M_AMP_MUTE_ON(AmpList *pstAmpList);
int Tas5825M_AMP_MUTE_OFF(AmpList *pstAmpList);
int Tas5825M_AMP_LR_CHANNEL(AmpList *pstAmpList);
int Tas5825M_AMP_SET_SUBMODE(AmpList *pstAmpList);
int Tas5825M_AMP_SET_STEREOMODE(AmpList *pstAmpList);
int Tas5825M_AMP_SET_PBTLMODE(AmpList *pstAmpList);
int Tas5825M_AMP_DRC_CTRL(AmpList *pstAmpList);
int Tas5825M_AMP_POWER_OFF(AmpList *pstAmpList);

AMPOP_T	 g_stTas5825M =
{
	Tas5825M_AMP_INIT,
	Tas5825M_AMP_SET_VOL,
	Tas5825M_AMP_SET_LR_VOL,
	Tas5825M_AMP_SET_SUB_VOL,
	Tas5825M_AMP_SET_CH1_VOL,
	Tas5825M_AMP_SET_CH2_VOL,
	Tas5825M_AMP_MUTE_ON,
	Tas5825M_AMP_MUTE_OFF,
	NULL,//Tas5825M_AMP_LR_CHANNEL,
	Tas5825M_AMP_SET_SUBMODE,
	Tas5825M_AMP_SET_STEREOMODE,
	Tas5825M_AMP_SET_PBTLMODE,
	Tas5825M_AMP_DRC_CTRL,
	Tas5825M_AMP_POWER_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
};

void Tas5825M_AMP_delay_ms(int ms)
{
	msleep(ms);
}

/****************************************************
Function:Tas5825MWrite
Description:
Input:UINT8 bRegAddr
         UINT8 bData
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2020/11/19           Created             yuanwei.liang
Others:

*****************************************************/
static BOOL Tas5825MWrite(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:PowerOnTas5825M
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2020/11/19           Created

Others:

*****************************************************/
static void PowerOnTas5825M(void)
{
	GPIO_Output_Write(TAS5825M_PDN_GPIO, IO_HIGH);
	delay_1ms(50);
	//GPIO_Output_Write(TAS5825M_PDN_GPIO, IO_LOW); //delete if sound ok
	//delay_1ms(50);  //delete if sound ok
}


static void PowerOffTas5825M(void)
{
	GPIO_Output_Write(TAS5825M_PDN_GPIO, IO_LOW);
	delay_1ms(35);//at least 35ms
}

/****************************************************
Function:SetTas5825MOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2020/11/19           Created

Others:

*****************************************************/

static void SetTas5825MOutput( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;
	BYTE page ;
	BYTE book;
	//BYTE bGain;

	page=0x00;
	Tas5825MWrite(pstAmpList->peList[eIdx], ePage_Select, &page, sizeof(page));

	book = 0x00;
    Tas5825MWrite(pstAmpList->peList[eIdx], eBook_Select, &book, sizeof(book));

    abData = 0x10; // deep sleep mode
	Tas5825MWrite(pstAmpList->peList[eIdx], eDevCtrl2, &abData, sizeof(abData));

	abData = 0x02; // hiz mode
	Tas5825MWrite(pstAmpList->peList[eIdx], eDevCtrl2, &abData, sizeof(abData));
	delay_1ms(10);//at least 5ms

	abData = 0x80;//
	Tas5825MWrite(pstAmpList->peList[eIdx], eFaultClear, &abData, sizeof(abData));

	abData = 0x03;//play mode
	Tas5825MWrite(pstAmpList->peList[eIdx], eDevCtrl2, &abData, sizeof(abData));


}
static void SetTas5825MMode2_1( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
      return;

}

static void SetTas5825MMode2_0( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
      return;

}

static void SetTas5825MPBTLMode( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
    UINT32 i __UNUSED = 0;
	BYTE bData = 0x04;
	BYTE page = 0x00;
	BYTE book = 0x00;

#ifdef Q70_DEMO
	Tas5825MWrite(pstAmpList->peList[eIdx], ePage_Select, &page, sizeof(page));
	Tas5825MWrite(pstAmpList->peList[eIdx], eBook_Select, &book, sizeof(book));
	Tas5825MWrite(pstAmpList->peList[eIdx], eDevCtrl1, &bData, sizeof(bData));
#else
	//printfw("%s \n",__FUNCTION__);
	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5825MWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));
		Tas5825MWrite(pstAmpList->peList[i], eBook_Select, &book, sizeof(book));
		Tas5825MWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));

		Tas5825MWrite(pstAmpList->peList[i], eDevCtrl1, &bData, sizeof(bData));
	}
#endif
}



static void SetTas5825MDRCCtrl( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
	  return;

}
//Input multiplexer register

#if 0
static void SetTas5825MLRChannel( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, eTas5825MLRChl eLR)
{
      return;
}

#endif


static void SetTas5825MVolume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{

	Tas5825MWrite(pstAmpList->peList[eIdx], eMasterVol, &bGain, sizeof(bGain));
}


//jun.yang for volume adjustment system (mantis:0262891)20150929
static void SetTas5825MLRVolume( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
 #if 0
    BYTE page ;
	BYTE book;
	//BYTE fault;

	page=0x00;
	Tas5825MWrite(pstAmpList->peList[eIdx], ePage_Select, &page, sizeof(page));

	book = 0x8c;
    Tas5825MWrite(pstAmpList->peList[eIdx], eBook_Select, &book, sizeof(book));

	page=0x0b;
	Tas5825MWrite(pstAmpList->peList[eIdx], ePage_Select, &page, sizeof(page));

    BYTE *Gain = (BYTE *)&bGain;
	BYTE vol[4] = {*(Gain+3),*(Gain+2),*(Gain+1),*(Gain)};

	//LOGD("aGain:0x%x,%x,%x,%x:%d\n",*vol,*(vol+1),*(vol+2),*(vol+3),sizeof(vol));
	Tas5825MWrite(pstAmpList->peList[eIdx], eLchVol, vol, sizeof(vol));

	Tas5825MWrite(pstAmpList->peList[eIdx], eRchVol, vol, sizeof(vol));

 #endif
}

static void SetTas5825MCh1Volume( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
      return;
}

static void SetTas5825MCh2Volume( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
	  return;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

static void SetTas5825MSubVolume( __UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
      return;
}


static void SetTas5825MMute( AmpList  *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x0b;
	BYTE page ;
	BYTE book;

	page=0x00;
	book = 0x00;

	//printfw("%s \n",__FUNCTION__);
	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5825MWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));
		Tas5825MWrite(pstAmpList->peList[i], eBook_Select, &book, sizeof(book));
		Tas5825MWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));

		Tas5825MWrite(pstAmpList->peList[i], eDevCtrl2, &bData, sizeof(bData));
	}

}

static void SetTas5825MDeMute( AmpList  *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x03;
    BYTE page ;
	BYTE book;

	page=0x00;
	book = 0x00;
	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
	    Tas5825MWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));
		Tas5825MWrite(pstAmpList->peList[i], eBook_Select, &book, sizeof(book));
		//Tas5805mWrite(pstAmpList->peList[i], ePage_Select, &page, sizeof(page));

		Tas5825MWrite(pstAmpList->peList[i], eDevCtrl2, &bData, sizeof(bData));
	}


}



/****************************************************
Function:Tas5825MInit
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2020/11/19           Created

Others:

*****************************************************/
static BOOL Tas5825MInit(AmpList *pstAmpList)
{
	BOOL bRet = FALSE;

	UINT32 i = 0;
	//char * pbModule = NULL;
	//char bModuleName[MODULENAMELEN] = {0};
	//UINT32 i = 0;

	//printfw("%s\n",__FUNCTION__);

	// init TAS5825M reset pin and pdn pin
	if (Pinmux_GPIO_CheckStatus(TAS5825M_PDN_GPIO) == 0)
	{
		if (Pinmux_GPIO_Alloc(TAS5825M_PDN_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(TAS5825M_PDN_GPIO, IO_OUTPUT);
			LOGD("========= %s =====\n", __FUNCTION__);
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
	PowerOnTas5825M();
#ifdef Q70_DEMO
	Tas5825M_AMP_SET_PBTLMODE(pstAmpList);
#endif
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
     {
         SetTas5825MOutput(pstAmpList, pstAmpList->peList[i].eAmpID);

     }
	//BYTE bData = 0x4;

	//Tas5825MWrite(pstAmpList->peList[0], eDevCtrl1, &bData, sizeof(bData)); //write amp0 for pbtl mode
	//Tas5825MWrite(pstAmpList->peList[4], eDevCtrl1, &bData, sizeof(bData)); //write amp4 for pbtl mode

	return (bRet);
}

int Tas5825M_AMP_INIT( AmpList *pstAmpList)
{
	INT32 sdRet =  -1;
	if (Tas5825MInit(pstAmpList) == TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}

int Tas5825M_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	BYTE *bVolTable;
	BYTE bStepNum;
	LOGD("========= Tas5825M_AMP_SET_VOL =====\n");

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
		SetTas5825MVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
int Tas5825M_AMP_SET_LR_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetTas5825MLRVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5825M_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetTas5825MCh1Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5825M_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetTas5825MCh2Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int Tas5825M_AMP_SET_SUB_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetTas5825MSubVolume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}
//jun.yang for volume adjustment system (mantis:0262891)20150929

int Tas5825M_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetTas5825MMute(pstAmpList);

	return (0);
}

int Tas5825M_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetTas5825MDeMute(pstAmpList);

	return (0);
}

#if 0

int Tas5825M_AMP_LR_CHANNEL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5825MLRChannel(pstAmpList, pstAmpList->peList[i].eAmpID, eLOnly);
	}

	return (0);
}
#endif

int Tas5825M_AMP_SET_SUBMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5825MMode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}



int Tas5825M_AMP_SET_STEREOMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5825MMode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int Tas5825M_AMP_SET_PBTLMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

#ifdef Q70_DEMO
	SetTas5825MPBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
#else
	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5825MPBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
	}
#endif

	return (0);
}

int Tas5825M_AMP_DRC_CTRL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetTas5825MDRCCtrl(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int Tas5825M_AMP_POWER_OFF( AmpList *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x02;//hiz mode

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		Tas5825MWrite(pstAmpList->peList[i], eDevCtrl2, &bData, sizeof(bData));
	}


	PowerOffTas5825M();

	return (0);
}

