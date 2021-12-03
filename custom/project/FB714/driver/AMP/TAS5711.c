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
int Tas5711_AMP_MUTE_ON(AmpList *pstAmpList);
int Tas5711_AMP_MUTE_OFF(AmpList *pstAmpList);
int Tas5711_AMP_DRC_CTRL( AmpList *pstAmpList);
int Tas5711_AMP_POWER_OFF( AmpList *pstAmpList);

AMPOP_T	 g_stTas5711 =
{
	Tas5711_AMP_INIT,
	Tas5711_AMP_SET_VOL,
	NULL,
	NULL,
	NULL,
	NULL,
	Tas5711_AMP_MUTE_ON,
	Tas5711_AMP_MUTE_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	Tas5711_AMP_POWER_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
};

#define EQ_DATA_MAX	21
#define EQ_REG_MAX		22

BYTE LR_EQ_data[EQ_DATA_MAX*EQ_REG_MAX] = 
{
0X29,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X30,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X2A,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X2B,0X00,0X7D,0XA6,0XE3,0X0F,0X04,0XB2,0X39,0X00,0X7D,0XA6,0XE3,0X00,0XFB,0X42,0XC1,0X0F,0X84,0XA7,0X33,
0X2C,0X00,0X80,0X00,0X00,0X0F,0XC5,0X45,0XEA,0X00,0X19,0X75,0XF5,0X00,0X3A,0XBA,0X16,0X0F,0XE6,0X8A,0X0B,
0X2D,0X00,0X80,0X00,0X00,0X0F,0X21,0XD6,0X3F,0X00,0X6A,0X9D,0X31,0X00,0XDE,0X29,0XC1,0X0F,0X95,0X62,0XCF,
0X2E,0X00,0X80,0X00,0X00,0X0F,0X38,0X05,0X2E,0X00,0X63,0X8E,0X3C,0X00,0XC7,0XFA,0XD2,0X0F,0X9C,0X71,0XC4,
0X2F,0X00,0X80,0X00,0X00,0X0F,0X10,0XAD,0X1B,0X00,0X74,0X03,0X2F,0X00,0XEF,0X52,0XE5,0X0F,0X8B,0XFC,0XD1,
0X58,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X59,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X31,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X32,0X00,0X7D,0XA6,0XE3,0X0F,0X04,0XB2,0X39,0X00,0X7D,0XA6,0XE3,0X00,0XFB,0X42,0XC1,0X0F,0X84,0XA7,0X33,
0X33,0X00,0X80,0X00,0X00,0X0F,0XC5,0X45,0XEA,0X00,0X19,0X75,0XF5,0X00,0X3A,0XBA,0X16,0X0F,0XE6,0X8A,0X0B,
0X34,0X00,0X80,0X00,0X00,0X0F,0X21,0XD6,0X3F,0X00,0X6A,0X9D,0X31,0X00,0XDE,0X29,0XC1,0X0F,0X95,0X62,0XCF,
0X35,0X00,0X80,0X00,0X00,0X0F,0X38,0X05,0X2E,0X00,0X63,0X8E,0X3C,0X00,0XC7,0XFA,0XD2,0X0F,0X9C,0X71,0XC4,
0X36,0X00,0X80,0X00,0X00,0X0F,0X10,0XAD,0X1B,0X00,0X74,0X03,0X2F,0X00,0XEF,0X52,0XE5,0X0F,0X8B,0XFC,0XD1,
0X5C,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X5D,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X5E,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X5F,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
0X5A,0X00,0X00,0X05,0X83,0X00,0X00,0X0B,0X06,0X00,0X00,0X05,0X83,0X00,0XFB,0X42,0XC1,0X0F,0X84,0XA7,0X33,
0X5B,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
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

static void SetTas5711DRCCtrl( AmpList  *pstAmpList, eAmpIdx eIdx)
{
	BYTE i=0;
	#define DRC_DATA_MAX	16

	BYTE LR_drc_data[DRC_DATA_MAX][18] = 
	{

{0X3A,8,0X00,0X0C,0XA9,0X91,0X00,0X73,0X56,0X6B,},  
{0X3B,8,0X00,0X02,0XA3,0X99,0X00,0X7D,0X5C,0X65,},  
{0X3C,8,0X00,0X00,0X06,0XD3,0X00,0X7F,0XF9,0X2B,},  
{0X40,4,0XFD,0XCC,0X99,0XDB,}, 
{0X41,4,0X0F,0X81,0X4A,0XFF,}, 
{0X42,4,0X00,0X08,0X42,0X10,}, 
{0X39,8,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,},  
{0X3D,8,0X00,0X0C,0XA9,0X91,0X00,0X73,0X56,0X6B,},  
{0X3E,8,0X00,0X02,0XA3,0X99,0X00,0X7D,0X5C,0X65,},  
{0X3F,8,0X00,0X00,0X06,0XD3,0X00,0X7F,0XF9,0X2B,},  
{0X43,4,0XFD,0XCC,0X99,0XDB,}, 
{0X44,4,0X0F,0X81,0X4A,0XFF,}, 
{0X45,4,0X00,0X08,0X42,0X10,}, 
{0X46,4,0X00,0X00,0X00,0X03,}, 
{0X53,16,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X00,0X00,}, 
{0X54,16,0X00,0X80,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X80,0X00,0X00,}, 

	};

	if(eIdx == eAmp1)  // 0x36 LR
	{
		for(i=0; i<DRC_DATA_MAX; i++)
			Tas5711Write(pstAmpList->peList[eIdx], LR_drc_data[i][0], &LR_drc_data[i][2], LR_drc_data[i][1]);
	}


}


static void SetTas5711EQ( AmpList  *pstAmpList, BYTE eq)
{
		BYTE i=0, j=0;
	BYTE *eq_data;
	BYTE EQ_SW[] = {0X50, 0X0F, 0X70, 0X80, 0X00};

	eq = eq;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		if(i == eAmp1)
		{
			Tas5711Write(pstAmpList->peList[i], EQ_SW[0], &EQ_SW[1], 4);
	
			eq_data = LR_EQ_data;

			//amp_printf_E(" mode %d, addr : 0x%x,  LR : 0x%x, C : 0x%x\n",mode, eq_data, Movie_LR_EQ, Movie_C_EQ);
		
			for(j=0; j<EQ_REG_MAX; j++)
				Tas5711Write(pstAmpList->peList[i], eq_data[j*EQ_DATA_MAX], &eq_data[j*EQ_DATA_MAX+1], EQ_DATA_MAX-1);

		}
	}
}


static void SetTas5711Output( AmpList  *pstAmpList, eAmpIdx eIdx)
{

	BYTE abData;
	BYTE LR_data[] = {0x01, 0x02, 0x13, 0x45};  // 0x25
	BYTE C_data[] = {0x01, 0x10, 0x32, 0x45};

	BYTE inputmix[] = {0x00, 0x01, 0x77, 0x72};  // 0x20
	BYTE ch4srcsel[] = {0x00, 0x00, 0x42, 0x03};  // 0x21
	BYTE EQ_SW[] = {0X50, 0X0F, 0X70, 0X80, 0X00};

	
	abData = 0x0;
	Tas5711Write(pstAmpList->peList[eIdx], eOscillatorTrim, &abData, sizeof(abData));
	
	Tas5711_AMP_delay_ms(100);//wait at least 50ms

	abData = 0X07;
	Tas5711Write(pstAmpList->peList[eIdx], eSoftMute, &abData, sizeof(abData));	
	
	abData = 0xff; // mute master vol
	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

	abData = 0x91;// sub channel vol = register 0xA
	Tas5711Write(pstAmpList->peList[eIdx], eVolCfg, &abData, sizeof(abData));

	Tas5711Write(pstAmpList->peList[eIdx], eInputMux, inputmix, sizeof(inputmix));

	Tas5711Write(pstAmpList->peList[eIdx], EQ_SW[0], &EQ_SW[1], 4);
	

	if(eIdx == eAmp1)  // 0x36 LR
	{
		abData = 0x30;
		Tas5711Write(pstAmpList->peList[eIdx], ePWMSDG, &abData, sizeof(abData));

		Tas5711Write(pstAmpList->peList[eIdx], ePWMMux, LR_data, sizeof(LR_data));

		abData = 0x95;
		Tas5711Write(pstAmpList->peList[eIdx], eStartStopPeriod, &abData, sizeof(abData));
		Tas5711Write(pstAmpList->peList[eIdx], eCh4SrcSel, ch4srcsel, sizeof(ch4srcsel));
		
		abData = 0x20; 
		Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &abData, sizeof(abData));
		Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &abData, sizeof(abData));
	}
	else  // 0x34  sub
	{ 
		abData = 0x20;
		Tas5711Write(pstAmpList->peList[eIdx], ePWMSDG, &abData, sizeof(abData));

		Tas5711Write(pstAmpList->peList[eIdx], ePWMMux, C_data, sizeof(C_data));

		abData = 0x20;
		Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &abData, sizeof(abData));
		Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &abData, sizeof(abData));
#if 0
// BD mode
	abData = 0xB8;
	Tas5711Write(pstAmpList->peList[eIdx], 0X11, &abData, sizeof(abData));
	abData = 0x60;
	Tas5711Write(pstAmpList->peList[eIdx], 0X12, &abData, sizeof(abData));
	abData = 0xA0;
	Tas5711Write(pstAmpList->peList[eIdx], 0X13, &abData, sizeof(abData));
	abData = 0x48;
	Tas5711Write(pstAmpList->peList[eIdx], 0X14, &abData, sizeof(abData));
#endif


	}
	
	SetTas5711DRCCtrl(pstAmpList, eIdx);
	SetTas5711EQ(pstAmpList, 0);
	
	abData = 0x80; //0;// exit shut down mode
	Tas5711Write(pstAmpList->peList[eIdx], eSysCtl2, &abData, sizeof(abData));

	abData = 0x30; // mute master vol
	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

}

static void SetTas5711Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{

	Tas5711Write(pstAmpList->peList[eIdx], eMasterVol, &bGain, sizeof(bGain));
	//Tas5711Write(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
	//Tas5711Write(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
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

	LOGD("%s\n",__FUNCTION__);

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

int Tas5711_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetTas5711Mute(pstAmpList);

	return (0);
}

int Tas5711_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetTas5711DeMute(pstAmpList);

	LOGD(" %s\n ", __func__);

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

