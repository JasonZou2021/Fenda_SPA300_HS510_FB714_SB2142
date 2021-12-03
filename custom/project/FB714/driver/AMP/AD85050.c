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
#define AD85050_AMP_SDB_GPIO	(8)
#define AD85050_FAULT_GPIO		(7)
#define SUBVOLUME_EXGAIN		0xC
#define SETDEVICEADDR			(1)

/*----------------------------------------------*
  * EXTERNAL REFERENCES
  *----------------------------------------------*/
//#define Mode2_1_CH
#define INVERT_R_CH

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/

typedef struct _AmpAD85050
{
	UINT8 bAmpIdx;//eAmp1, eAmp2 ...    eAmpIdx bAmpIdx
	UINT8 bAD85050Addr;
} AmpAD85050;

typedef enum _eAD85050LRChl
{
	eLOnly,
	eROnly,
} eAD85050LRChl;

typedef enum _eAD85050RegAddr
{
	eStateCtl1 = 0x00,
	eStateCtl2,
	eStateCtl3,
	eMasterVol_,
	eCh1Vol,
	eCh2Vol,
	eCh3Vol,
	eCh4Vol,
	eCh5Vol,
	eCh6Vol,
	eBasstone,
	eTrebletone,
	eStateCtl4,

	eCh1Cfg = 0x0d,
	eCh2Cfg,
	eCh3Cfg,
	eCh4Cfg,
	eCh5Cfg,
	eCh6Cfg,
	eCh7Cfg,
	eCh8Cfg,
	eDrc1_ath_rth_rate = 0x15,
	eDrc2_ath_rth_rate,
	eDrc3_ath_rth_rate,
	eDrc4_ath_rth_rate,
	eStateCtl5 = 0x19,
	eStateCtl6 = 0x1a,
	eStateCtl7 = 0x1b,
	eStateCtl8 = 0x1c,

	//0x1d--0x2d for ram
	eCoef_BaseReg_Cfg = 0x1d,
	eCoef_A1_Top,
	eCoef_A1_Mid,
	eCoef_A1_Bot,
	eCoef_A2_Top,
	eCoef_A2_Mid,
	eCoef_A2_Bot,

	eCoef_B1_Top,
	eCoef_B1_Mid,
	eCoef_B1_Bot,
	eCoef_B2_Top,
	eCoef_B2_Mid,
	eCoef_B2_Bot,

	eCoef_A0_Top,
	eCoef_A0_Mid,
	eCoef_A0_Bot,

	eCoef_WR_Cfg = 0x2d,

	eVolTune1 = 0x34,
	eVolTune2,
	eDac_Gain,
	elvlmeter_clear = 0x42,
	epowermeter_clear,
	eCheckSum_status = 0x5d,

	//0x14 - 0x24	user-defined

	eRegmax,
} eAD85050RegAddr;

/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/

//INT32 gAD85050fd = -1;


int AD85050_AMP_INIT(AmpList *pstAmpList);
int AD85050_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD85050_AMP_SET_CH1_VOL(AmpList *pstAmpList, BYTE  bVolume );
int	AD85050_AMP_SET_CH2_VOL(AmpList *pstAmpList, BYTE  bVolume );
//int AD85050_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
//int AD85050_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD85050_AMP_MUTE_ON(AmpList *pstAmpList);
int AD85050_AMP_MUTE_OFF(AmpList *pstAmpList);
//int AD85050_AMP_LR_CHANNEL( AmpList *pstAmpList);
int AD85050_AMP_SET_SUBMODE( AmpList *pstAmpList);
int AD85050_AMP_SET_STEREOMODE( AmpList *pstAmpList);
int AD85050_AMP_SET_PBTLMODE( AmpList *pstAmpList);
int AD85050_AMP_DRC_CTRL( AmpList *pstAmpList);
int AD85050_AMP_POWER_OFF( AmpList *pstAmpList);
int	AD85050_AMP_SET_CH3_VOL(AmpList *pstAmpList, BYTE  bVolume );
int	AD85050_AMP_SET_CH4_VOL(AmpList *pstAmpList, BYTE  bVolume );
int	AD85050_AMP_SET_CH5_VOL(AmpList *pstAmpList, BYTE  bVolume );
int	AD85050_AMP_SET_CH6_VOL(AmpList *pstAmpList, BYTE  bVolume );
//int AD85050_Check_Amp_State(AmpList* pstAmpList);

//int AD85050_AMP_SELECT(AmpList* pstAmpList);
AMPOP_T	 g_stAD85050 =
{
	AD85050_AMP_INIT,
	AD85050_AMP_SET_VOL,
	NULL,
	NULL,
	AD85050_AMP_SET_CH1_VOL,
	AD85050_AMP_SET_CH2_VOL,
	AD85050_AMP_MUTE_ON,
	AD85050_AMP_MUTE_OFF,
	NULL,
	AD85050_AMP_SET_SUBMODE,
	AD85050_AMP_SET_STEREOMODE,
	AD85050_AMP_SET_PBTLMODE,
	AD85050_AMP_DRC_CTRL,
	AD85050_AMP_POWER_OFF,
	AD85050_AMP_SET_CH3_VOL,
	AD85050_AMP_SET_CH4_VOL,
	AD85050_AMP_SET_CH5_VOL,
	AD85050_AMP_SET_CH6_VOL,
	// AD85050_Check_Amp_State,
};




/****************************************************
Function:SetAD85050DOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/8/9         Created             yuanwei.liang
Others:

*****************************************************/

static int AD85050Write(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:SetAD85050DOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/8/9         Created             yuanwei.liang
Others:

*****************************************************/
static void PowerOnAD85050(void)
{
	GPIO_Output_Write(AD85050_AMP_SDB_GPIO, IO_LOW);
	delay_1ms(3);
	GPIO_Output_Write(AD85050_AMP_SDB_GPIO, IO_HIGH);
	delay_1ms(22);
}

static void PowerOffAD85050(void)
{
	GPIO_Output_Write(AD85050_AMP_SDB_GPIO, IO_LOW);
	delay_1ms(5);
}

/****************************************************
Function:SetAD85050DOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/8/9         Created             yuanwei.liang
Others:

*****************************************************/
static void SetAD85050Output(const AmpList *const pstAmpList, eAmpIdx eIdx)
{
	BYTE abData;

	abData = 0x0;
	AD85050Write(pstAmpList->peList[eIdx], eStateCtl1, &abData, sizeof(abData));

	abData = 0x0;
	AD85050Write(pstAmpList->peList[eIdx], eStateCtl3, &abData, sizeof(abData));

	#ifdef INVERT_R_CH
	//invert PWM out
	//abData = 0x40;
	//AD85050Write(pstAmpList->peList[eIdx], eStateCtl8, &abData, sizeof(abData));

	//invert pre-scale
	//set ram bank #1, addr 0x4d(Channel-2 Prescale, C2POS) as 0xf80000
	abData = 0x4d;
	AD85050Write(pstAmpList->peList[eIdx], eCoef_BaseReg_Cfg, &abData, sizeof(abData));
	abData = 0xf8;
	AD85050Write(pstAmpList->peList[eIdx], eCoef_A1_Top, &abData, sizeof(abData));
	abData = 0x00;
	AD85050Write(pstAmpList->peList[eIdx], eCoef_A1_Mid, &abData, sizeof(abData));
	abData = 0x00;
	AD85050Write(pstAmpList->peList[eIdx], eCoef_A1_Bot, &abData, sizeof(abData));
	abData = 0x41;  //set bank 1 and W1 bit
	AD85050Write(pstAmpList->peList[eIdx], eCoef_WR_Cfg, &abData, sizeof(abData));
    #endif

	abData = 0x48;
	AD85050Write(pstAmpList->peList[eIdx], eMasterVol_, &abData, sizeof(abData));

	/*abData = 0x00;//for sub
	AD85050Write(pstAmpList->peList[1], eCh2Vol, &abData, sizeof(abData));

	AD85050Write(pstAmpList->peList[1], eCh4Vol, &abData, sizeof(abData));

	AD85050Write(pstAmpList->peList[1], eCh6Vol, &abData, sizeof(abData));*/

	//abData = 0x0;
	//AD85050Write(pstAmpList->peList[1], eDac_Gain, &abData, sizeof(abData));
	//abData = 0x80;
	//AD85050Write(pstAmpList->peList[1], eStateCtl4, &abData, sizeof(abData));*/

	delay_1ms(5);
}




static void SetAD85050Mode2_1(__UNUSED const AmpList *const pstAmpList, __UNUSED eAmpIdx eIdx)
{
	return;
}

static void SetAD85050Mode2_0(__UNUSED const AmpList *const pstAmpList, __UNUSED eAmpIdx eIdx)
{
	return;

}

static void SetAD85050PBTLMode(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{
	return;
}


static void SetAD85050DRCCtrl(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx)
{


}


static void SetAD85050LRChannel(__UNUSED AmpList  *pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED eAD85050LRChl eLR)
{
	return;
}



static void SetAD85050Volume(const AmpList *const pstAmpList, eAmpIdx eIdx,  BYTE bGain)
{



	AD85050Write(pstAmpList->peList[eIdx], eMasterVol_, &bGain, sizeof(bGain));



}



static void SetAD85050Ch1Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh1Vol, &bGain, sizeof(bGain));
}

static void SetAD85050Ch2Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh2Vol, &bGain, sizeof(bGain));
}


static void SetAD85050Ch4Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh4Vol, &bGain, sizeof(bGain));
}

static void SetAD85050Ch5Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh5Vol, &bGain, sizeof(bGain));
}

static void SetAD85050Ch6Volume( AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh6Vol, &bGain, sizeof(bGain));
}

static void SetAD85050Ch3Volume(AmpList  *pstAmpList, eAmpIdx eIdx, BYTE bGain)
{
	AD85050Write(pstAmpList->peList[eIdx], eCh3Vol, &bGain, sizeof(bGain));
}

static void SetAD85050Mute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x7f;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD85050Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


static void SetAD85050DeMute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x00;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD85050Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


/****************************************************
Function:SetAD85050DOutput
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/8/9         Created             yuanwei.liang
Others:

*****************************************************/

static BOOL AD85050Init(AmpList *pstAmpList)
{
	BOOL bRet = FALSE;
	//char * pbModule = NULL;
	//char bModuleName[MODULENAMELEN] = {0};
	UINT32 i = 0;
	LOGD("AD85050Init\n");

	//LOGE("%s\n",__FUNCTION__);

	// init AD85050 reset pin and pdn pin
	if ((Pinmux_GPIO_CheckStatus(AD85050_AMP_SDB_GPIO) == 0) && (Pinmux_GPIO_CheckStatus(AD85050_FAULT_GPIO) == 0))
	{
		if (Pinmux_GPIO_Alloc(AD85050_AMP_SDB_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD85050_AMP_SDB_GPIO, IO_OUTPUT);
		}

		if (Pinmux_GPIO_Alloc(AD85050_FAULT_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD85050_FAULT_GPIO, IO_INPUT);
		}
	}
	else
	{
		return FALSE;
	}

	PowerOnAD85050();

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Output(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	bRet = TRUE;

	return (bRet);
}

int AD85050_AMP_INIT( AmpList *pstAmpList)
{
	INT32 sdRet =  -1;
	if (AD85050Init(pstAmpList) == TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}

int AD85050_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD85050Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}



	return 0;
}


int AD85050_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD85050Ch1Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int AD85050_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD85050Ch2Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}
int AD85050_AMP_SET_CH4_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch4_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch4GainStepNum;

	LOGD("AMP CH4_bVolume=%d 0x%x \n", bVolume, bVolTable[bVolume]);

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Ch4Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}
int AD85050_AMP_SET_CH5_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch5_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch5GainStepNum;

	LOGD("AMP CH5_bVolume=%d 0x%x \n", bVolume, bVolTable[bVolume]);

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Ch5Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}
int AD85050_AMP_SET_CH6_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
	UINT32 i = 0;
	BYTE *bVolTable;
	BYTE bStepNum;

	bVolTable = pstAmpList->pstAmpCfg->Ch6_GainTbl;
	bStepNum = pstAmpList->pstAmpCfg->Ch4GainStepNum;

	LOGD("AMP CH6_bVolume=%d 0x%x \n", bVolume, bVolTable[bVolume]);

	if (bVolume >= bStepNum)
	{
		return -1;
	}

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Ch6Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}

	return 0;
}

int AD85050_AMP_SET_CH3_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD85050Ch3Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}
	return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

int AD85050_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetAD85050Mute(pstAmpList);

	return (0);
}

int AD85050_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetAD85050DeMute(pstAmpList);

	return (0);
}

int AD85050_AMP_LR_CHANNEL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050LRChannel(pstAmpList, pstAmpList->peList[i].eAmpID, eLOnly);
	}

	return (0);
}

int AD85050_AMP_SET_SUBMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Mode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD85050_AMP_SET_STEREOMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050Mode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD85050_AMP_SET_PBTLMODE( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050PBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int AD85050_AMP_DRC_CTRL( AmpList *pstAmpList)
{
	UINT32 i = 0;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetAD85050DRCCtrl(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}


int AD85050_AMP_POWER_OFF( AmpList *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x8f;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD85050Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));			//first enter shutdown mode
	}

	delay_1ms(170); // get delay time by reg 0x1a

	PowerOffAD85050();

	return (0);
}
int AD85050_Check_Amp_State(__UNUSED AmpList *pstAmpList)
{
	int state_fault;


	state_fault = GPIO_Intput_Read(AD85050_FAULT_GPIO);
	//LOGD("=========check state =======\n");
	//LOGD("state_sd :%d   state_otw: %d\n",state_sd,state_otw);
	if (state_fault == 0)
	{
		LOGD(" ====waring AMP fault=====\n");
		return 0;
	}

	return 1;


}

/*int AD85050_AMP_SELECT(AmpList* pstAmpList)

{
    static int i = 1;


	i = i%2;
    BYTE bData;
	if(i==0)
	{
	   LOGD(" ====eAMP3 power up =====\n");
	   bData = 0x18;
	   AD85050Write(pstAmpList->peList[1], eCh1Cfg, &bData, sizeof(bData));
	   delay_1ms(1);
	   AD85050Write(pstAmpList->peList[1], eCh3Cfg, &bData, sizeof(bData));
	   delay_1ms(1);
	   AD85050Write(pstAmpList->peList[1], eCh5Cfg, &bData, sizeof(bData));
	   bData = 0x70;
	   AD85050Write(pstAmpList->peList[2], eStateCtl6, &bData, sizeof(bData));
	   i++;
	   return (0);
	}

    if(i==1)
	{
	   LOGD(" ====eAMP3 power down=====\n");
	   bData = 0xff;
	   AD85050Write(pstAmpList->peList[1], eCh1Cfg, &bData, sizeof(bData));
	   delay_1ms(1);
	   AD85050Write(pstAmpList->peList[1], eCh3Cfg, &bData, sizeof(bData));
	   delay_1ms(1);
	   AD85050Write(pstAmpList->peList[1], eCh5Cfg, &bData, sizeof(bData));
	   bData = 0x30;
	   AD85050Write(pstAmpList->peList[2], eStateCtl6, &bData, sizeof(bData));
	   i++;
	   return (0);
	}
	 return 0;
}*/


