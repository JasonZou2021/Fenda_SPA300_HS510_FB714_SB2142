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
#include "AD85050_Para.c"

/*----------------------------------------------*
  * MACRO DECLARATIONS
  *----------------------------------------------*/
#define AD82128_AMP_SDB_GPIO	(7)		//SPA300的GPIO7号引脚控制AMP的12号引脚(PD脚)
#define AD82128_FAULT_GPIO		(8)		//SPA300的GPIO7号引脚控制AMP的3号引脚(ERROR脚)
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

typedef struct _AmpAD82128
{
	UINT8 bAmpIdx;//eAmp1, eAmp2 ...    eAmpIdx bAmpIdx
	UINT8 bAD82128Addr;
} AmpAD82128;

typedef enum _eAD82128LRChl
{
	eLOnly,
	eROnly,
} eAD82128LRChl;

typedef enum _eAD82128RegAddr
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
} eAD82128RegAddr;

/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/

//INT32 gAD82128fd = -1;


int AD82128_AMP_INIT(AmpList *pstAmpList);
int AD82128_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
//int AD82128_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
//int AD82128_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int AD82128_AMP_MUTE_ON(AmpList *pstAmpList);
int AD82128_AMP_MUTE_OFF(AmpList *pstAmpList);
int AD82128_AMP_POWER_OFF( AmpList *pstAmpList);
//int AD82128_Check_Amp_State(AmpList* pstAmpList);

int AD82128_AMP_SET_EQ(AmpList* pstAmpList, BYTE eq);
int AD82128_AMP_SET_TREBLE(AmpList* pstAmpList, BYTE vol);
int AD82128_AMP_SET_SRC_GAIN(AmpList *pstAmpList, BYTE src);
int AD82128_AMP_SET_SURROUND_GAIN(AmpList *pstAmpList, BYTE status);
/*由于在工程文件夹中的驱动文件夹的Makefile加载的是AD82128的驱动，所以该驱动的函数结构体全局变量有效*/
AMPOP_T	 g_stAD85050 =
{
	AD82128_AMP_INIT,
	AD82128_AMP_SET_VOL,
	NULL,
	NULL,
	NULL,
	NULL,
	AD82128_AMP_MUTE_ON,
	AD82128_AMP_MUTE_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	AD82128_AMP_POWER_OFF,
	NULL,
	NULL,
	NULL,
	NULL,
	// AD82128_Check_Amp_State,

	AD82128_AMP_SET_EQ,
	AD82128_AMP_SET_TREBLE,
	AD82128_AMP_SET_SRC_GAIN,
	NULL,
	NULL,
};

/****************************************************
Function:SetAD82128DOutput
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

static int AD82128Write(AmpModuleConf stIdx, UINT8 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:SetAD82128DOutput
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
/*
	AD82128的上电时序
*/
static void PowerOnAD82128(void)
{

	//根据上电时序，PD脚拉低并延时20ms
	GPIO_Output_Write(AD82128_AMP_SDB_GPIO, IO_LOW);
	delay_1ms(50);
	//根据上电时序，PD脚拉高并延时200ms
	GPIO_Output_Write(AD82128_AMP_SDB_GPIO, IO_HIGH);
	delay_1ms(400);

}


static void PowerOffAD82128(void)
{
	GPIO_Output_Write(AD82128_AMP_SDB_GPIO, IO_LOW);
	delay_1ms(5);
}

/****************************************************
Function:SetAD82128DOutput
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
/*
	通过对AMP82128功放寄存器的操作，来设置它的输出状态(其中包括它的EQ，主音量，以及各通道的音量等...)
*/
static void SetAD82128Output(const AmpList *const pstAmpList, eAmpIdx eIdx)
{
	
	const BYTE *setting = NULL;
	BYTE write_enable = CFUD_W1;
	BYTE i=0, j=0;
		

	for(i=0; i<AMP_INIT_MAX; i++)
	{
		if(eIdx == eAmp1)  // LR
		{
			AD82128Write(pstAmpList->peList[eIdx], AMP_INIT_LR[i][0], &AMP_INIT_LR[i][1], 1);
		}
		else if(eIdx == eAmp2)  // C
		{
			AD82128Write(pstAmpList->peList[eIdx], AMP_INIT_C[i][0], &AMP_INIT_C[i][1], 1);
		}
	}

	setting = DRC_Map[eIdx];
	for(i=0; i<CHANNEL_SETTING_MAX; i++)
	{
		AD82128Write(pstAmpList->peList[eIdx], 0X1D, (void *)(setting+CHANNEL_SETTING_DATA * i), CHANNEL_SETTING_DATA);
		AD82128Write(pstAmpList->peList[eIdx], 0x32, (void *)&write_enable, 1);
	}

	if(eIdx == eAmp2)
	{
		BYTE center_amp_Mix[2*CHANNEL_SETTING_DATA] = 
		{
			0x50, 0x00, 0x00, 0x00,0x00,//##Channel_1_Mixer1 
	               0x51, 0x00, 0x00, 0x00,0x00,//##Channel_1_Mixer2 
		};

		write_enable = CFUD_W2;
		
		for(j=0; j<2; j++)
		{
			AD82128Write(pstAmpList->peList[eIdx], 0X1D, (void *)(&center_amp_Mix[CHANNEL_SETTING_DATA * j]), CHANNEL_SETTING_DATA);
			AD82128Write(pstAmpList->peList[eIdx], 0X32, (void *)&write_enable, 1);
		}
	}
	
	delay_1ms(100);

}



static void SetAD82128Volume(const AmpList *const pstAmpList, eAmpIdx eIdx,  BYTE bGain)
{
	AD82128Write(pstAmpList->peList[eIdx], eMasterVol_, &bGain, sizeof(bGain));
}


static void SetAD82128Mute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x40;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD82128Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


static void SetAD82128DeMute(const AmpList *const pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x00;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD82128Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));
	}
}


/****************************************************
Function:SetAD82128DOutput
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

static BOOL AD82128Init(AmpList *pstAmpList)
{
	BOOL bRet = FALSE;
	//char * pbModule = NULL;
	//char bModuleName[MODULENAMELEN] = {0};
	UINT32 i = 0;
	LOGD("AD82128Init\n");

	//LOGE("%s\n",__FUNCTION__);

	// init AD82128 reset pin and pdn pin
	/*SPA300的GPIO7号引脚控制AMP的12号引脚(PD脚为AMP的使能脚)，GPIO8号引脚控制AMP的3号引脚(ERROR脚为AMP的错误引脚，是双功能引脚)*/
	/*ERROR引脚：该引脚是双功能引脚。 一种是上电期间的 I2C 地址设置。 另一种是错误状态报告（低电平有效），由地址0x1C B[6]的A_SEL_FAULT寄存器置位使能*/
	/*0x1C寄存器的bit[6]控制ERROR引脚的双功能，当 B[6] 设置为低时，它被视为 I2C 选择输入。当 B[6] 设置为高时，它将成为 ERROR 输出引脚*/
	/*用的是硬件I2C,SCL-->GPIO13,SDA-->GPIO12*/
	if ((Pinmux_GPIO_CheckStatus(AD82128_AMP_SDB_GPIO) == 0) && (Pinmux_GPIO_CheckStatus(AD82128_FAULT_GPIO) == 0))
	{
		if (Pinmux_GPIO_Alloc(AD82128_AMP_SDB_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD82128_AMP_SDB_GPIO, IO_OUTPUT);
		}

		if (Pinmux_GPIO_Alloc(AD82128_FAULT_GPIO, IO_GPIO, IO_RISC) == 0)
		{
			GPIO_OE_Set(AD82128_FAULT_GPIO, IO_INPUT);
		}

	}
	else
	{
		//return FALSE;
	}

	//AD82128功放上电时序(相当于使能AD82128功放)
	PowerOnAD82128();

	/*设置左右声道以及中置的功放的输出初始状态*/
	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		/*控制左右通道以及中置通道功放的初始化配置*/
		SetAD82128Output(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	bRet = TRUE;

	return (bRet);
}

/*函数功能：AD82128功放驱动初始化，成功返回0，失败返回-1*/
int AD82128_AMP_INIT( AmpList *pstAmpList)
{
	INT32 sdRet =  -1;
	if (AD82128Init(pstAmpList) == TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}


/*
	no use, need set dsp volume, amp default MAX output
*/
int AD82128_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
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
		SetAD82128Volume(pstAmpList, pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
	}



	return 0;
}

int AD82128_AMP_MUTE_ON( AmpList *pstAmpList)
{
	SetAD82128Mute(pstAmpList);

	return (0);
}

int AD82128_AMP_MUTE_OFF( AmpList *pstAmpList)
{
	SetAD82128DeMute(pstAmpList);

	return (0);
}


int AD82128_AMP_POWER_OFF( AmpList *pstAmpList)
{
	UINT32 i = 0;
	BYTE bData = 0x40;

	for (i = 0; i < pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		AD82128Write(pstAmpList->peList[i], eStateCtl3, &bData, sizeof(bData));			//first enter shutdown mode
	}

	delay_1ms(170); // get delay time by reg 0x1a

	PowerOffAD82128();

	return (0);
}


int AD82128_AMP_SET_EQ( AmpList *pstAmpList, BYTE eqMode)
{
	UINT32 i, j= 0;	
	const BYTE *eq;
	BYTE write_enable = CFUD_W1;

	//return 0;

	if(eqMode >= EQ_MAX)
		return -1;

	LOGD(" %s : eq mode %d\n ", __func__, eqMode);

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{	
		eq = EQ_Map[i][eqMode];
	
		for(j=0; j<EQ_DATA_LEVEL; j++)
		{
			AD82128Write(pstAmpList->peList[i], 0X1D, (void *)(eq+EQ_DATA_MAX * j), EQ_DATA_MAX);
			AD82128Write(pstAmpList->peList[i], 0x32, (void *)&write_enable, 1);
		}
	}	

	return 0;
}


int AD82128_AMP_SET_TREBLE( AmpList *pstAmpList, BYTE vol)
{
	UINT32 i;	

	if(vol >= sizeof(treble_vol))
		return -1;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{	
		AD82128Write(pstAmpList->peList[i], eTrebletone, &treble_vol[vol], 1);
		
	}	

	return 0;
}

/*
	eAp_USB_Src,

	eAp_AUX_Src,

	eAp_SPDIF_Optical_Src,

	eAp_I2S_Src,

	eAp_ARC_Src,

	eAp_HDMI0_Src,

	eAp_HDMI1_Src,

*/


BYTE src_gain[] = {0x11,0x0e,0x11,0x11,0x11,0x11,0x11};

int AD82128_AMP_SET_SRC_GAIN(AmpList *pstAmpList, BYTE src)
{
	UINT32 i;	
	BYTE *gain;

	return 0;

	LOGD(" %s : src %d\n ", __func__, src);
	gain = src_gain;
	
	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{	
		AD82128Write(pstAmpList->peList[i], eMasterVol_, &src_gain[src], 1);
	}

	return 0;
}

