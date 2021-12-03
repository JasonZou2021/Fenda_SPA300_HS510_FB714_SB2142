/*****************************************************

Copyright (C) 2019 Sunplus technology co.Ltd.All right reserved!

 ******************************************************
File Name:PCM5242.c
Version:
Author:bj.cheng
Created:2019/05/20
Last Modified:
Description:
Public Functions:

History Information Description:
Date                    Author                 Modification
2019/05/20          bj.cheng              Created file


******************************************************/


/*----------------------------------------------*
  * INCLUDE  DECLARATIONS
  *----------------------------------------------*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "pinmux.h"
//#include "i2c_driver.h"
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
#define PCM5242_RESET_GPIO (7)
#define SUBVOLUME_EXGAIN 0xC
#define SETDEVICEADDR  (1)

//Define PCM5242 Reg Init Value
#define PAGE0_REG3_DEFAULT 0x00
#define PAGE0_REG60_DEFAULT 0x00
#define PAGE0_REG61_DEFAULT 0x30
#define PAGE0_REG62_DEFAULT 0x30

//Define one bit mask
#define BIT0 0x01
#define BIT1 0x02
#define BIT2 0x04
#define BIT3 0x08
#define BIT4 0x10
#define BIT5 0x20
#define BIT6 0x40
#define BIT7 0x80

/*----------------------------------------------*
  * EXTERNAL REFERENCES
  *----------------------------------------------*/

/*----------------------------------------------*
  * DATA TYPE  DECLARATIONS
  *----------------------------------------------*/
typedef enum _ePCM5242LRChl
{
	eLOnly,
	eROnly,
}ePCM5242LRChl;

typedef enum _ePCM5242RegAddr
{
#if 0
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
#else
//PAGE 0
    ePageSelect = 0x00,
    eSoftMute = 0x03,
    eDtlVolCtl = 0x3C,
    eLDtlVol = 0x3D,
    eRDtlVol = 0x3E,

#endif
}ePCM5242RegAddr;

typedef union
{
    UINT8        DataByte;
    struct
    {
        unsigned  Mute_R:1;
        unsigned  Reserve_bit1_3:3;
        unsigned  Mute_L:1;
        unsigned  Reserve_bit5_7:3;
    };
}unPCM5242_reg0x3_t;

/*----------------------------------------------*
  * FUNCTION DECLARATIONS
  *----------------------------------------------*/


/*----------------------------------------------*
  * GLOBAL VARIABLES
  *----------------------------------------------*/
//INT32 gTas5711fd = -1;


int PCM5242_AMP_INIT(AmpList *pstAmpList);
int PCM5242_AMP_SET_VOL(AmpList *pstAmpList, BYTE  bVolume );
int PCM5242_AMP_SET_LR_VOL(AmpList *pstAmpList, BYTE  bVolume );
int PCM5242_AMP_SET_SUB_VOL(AmpList *pstAmpList, BYTE  bVolume );
int PCM5242_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume );
int PCM5242_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume );
int PCM5242_AMP_MUTE_ON(AmpList *pstAmpList);
int PCM5242_AMP_MUTE_OFF(AmpList *pstAmpList);
int PCM5242_AMP_LR_CHANNEL( AmpList *pstAmpList);
int PCM5242_AMP_SET_SUBMODE( AmpList *pstAmpList);
int PCM5242_AMP_SET_STEREOMODE( AmpList *pstAmpList);
int PCM5242_AMP_SET_PBTLMODE( AmpList *pstAmpList);
int PCM5242_AMP_DRC_CTRL( AmpList *pstAmpList);
int PCM5242_AMP_POWER_RESET( AmpList *pstAmpList);

AMPOP_T	 g_stPCM5242 =
{
	PCM5242_AMP_INIT,
	PCM5242_AMP_SET_VOL,
	PCM5242_AMP_SET_LR_VOL,
	PCM5242_AMP_SET_SUB_VOL,
	PCM5242_AMP_SET_CH1_VOL,
	PCM5242_AMP_SET_CH2_VOL,
	PCM5242_AMP_MUTE_ON,
	PCM5242_AMP_MUTE_OFF,
	PCM5242_AMP_LR_CHANNEL,
	PCM5242_AMP_SET_SUBMODE,
	PCM5242_AMP_SET_STEREOMODE,
	PCM5242_AMP_SET_PBTLMODE,
	PCM5242_AMP_DRC_CTRL,
	PCM5242_AMP_POWER_RESET,
    NULL,
	NULL,
	NULL,
	NULL,
};

void PCM5242_AMP_delay_ms(int ms)
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
2019/05/20            Created              bj.cheng
Others:

*****************************************************/
static BOOL PCM5242Write(AmpModuleConf stIdx, UINT32 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:PCM5242Read
Description:
Input:UINT8 bRegAddr
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/05/20            Created              bj.cheng
Others:

*****************************************************/
BOOL PCM5242Read(AmpModuleConf stIdx, UINT32 bRegAddr, void *bBuf, UINT32 dLen)
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
Function:PowerOnPCM5242
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/05/20            Created              bj.cheng
Others:

*****************************************************/
static void PowerOnPCM5242(void)
{
	//GPIO_Output_Write(TAS5711_PDN_GPIO, IO_LOW);

	//PCM5242_AMP_delay_ms(2);

	GPIO_Output_Write(PCM5242_RESET_GPIO, IO_LOW);

	PCM5242_AMP_delay_ms(2);

	//GPIO_Output_Write(TAS5711_PDN_GPIO, IO_HIGH);

	//PCM5242_AMP_delay_ms(30);

	GPIO_Output_Write(PCM5242_RESET_GPIO, IO_HIGH);

	PCM5242_AMP_delay_ms(30);//wait at least 12ms
}


static void PowerOffPCM5242(void)
{
	//GPIO_Output_Write(TAS5711_PDN_GPIO, IO_LOW);

	//PCM5242_AMP_delay_ms(5);//at least 2ms

	GPIO_Output_Write(PCM5242_RESET_GPIO, IO_LOW);

    PCM5242_AMP_delay_ms(5);//at least 2ms
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
static void SetPCM5242Output( AmpList*  pstAmpList,eAmpIdx eIdx)
{
	BYTE bPageData;
    BYTE bRegData;

#if 0
	abData = 0x0;
	PCM5242Write(pstAmpList->peList[eIdx], eOscillatorTrim, &abData, sizeof(abData));
	PCM5242_AMP_delay_ms(100);//wait at least 50ms

	abData = 0xff; // mute master vol
	PCM5242Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

	abData = 0xd1;// sub channel vol = register 0xA
	PCM5242Write(pstAmpList->peList[eIdx], eVolCfg, &abData, sizeof(abData));

	abData = 0x42;// set master vol
	PCM5242Write(pstAmpList->peList[eIdx], eMasterVol, &abData, sizeof(abData));

	abData = 0x0;// exit shut down mode
	PCM5242Write(pstAmpList->peList[eIdx], eSysCtl2, &abData, sizeof(abData));
#else
    bPageData = 0x00; // mute select page
    PCM5242Write(pstAmpList->peList[eIdx], ePageSelect, &bPageData, sizeof(bPageData));

    bRegData = 0xFF;// set left digital volume
    PCM5242Write(pstAmpList->peList[eIdx], eLDtlVol, &bRegData, sizeof(bRegData));

    bRegData = 0xFF;// set right digital volume
    PCM5242Write(pstAmpList->peList[eIdx], eRDtlVol, &bRegData, sizeof(bRegData));

	bRegData = PAGE0_REG60_DEFAULT ;// this bits control the behavior of the digital volumel
    PCM5242Write(pstAmpList->peList[eIdx], eDtlVolCtl, &bRegData, sizeof(bRegData));

    bRegData = PAGE0_REG61_DEFAULT;// set left digital volume
    PCM5242Write(pstAmpList->peList[eIdx], eLDtlVol, &bRegData, sizeof(bRegData));

    bRegData = PAGE0_REG62_DEFAULT;// set right digital volume
    PCM5242Write(pstAmpList->peList[eIdx], eRDtlVol, &bRegData, sizeof(bRegData));
#endif
}

static void SetPCM5242Mode2_1(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx)
{
    return;
}

static void SetPCM5242Mode2_0(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx)
{
    return;
}

static void SetPCM5242PBTLMode(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx)
{
    return;
}

static void SetPCM5242DRCCtrl(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx)
{
    return;
}

//Input multiplexer register
static void SetPCM5242LRChannel(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED ePCM5242LRChl eLR)
{
	return;
}

static void SetPCM5242Volume( AmpList*  pstAmpList,eAmpIdx eIdx, BYTE bGain)
{
    PCM5242Write(pstAmpList->peList[eIdx], eLDtlVol, &bGain, sizeof(bGain));
	PCM5242Write(pstAmpList->peList[eIdx], eRDtlVol, &bGain, sizeof(bGain));
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
static void SetPCM5242LRVolume( AmpList*  pstAmpList,eAmpIdx eIdx, BYTE bGain)
{
	PCM5242Write(pstAmpList->peList[eIdx], eLDtlVol, &bGain, sizeof(bGain));
	PCM5242Write(pstAmpList->peList[eIdx], eRDtlVol, &bGain, sizeof(bGain));
}

static void SetPCM5242Ch1Volume( AmpList*  pstAmpList,eAmpIdx eIdx, BYTE bGain)
{
	PCM5242Write(pstAmpList->peList[eIdx], eLDtlVol, &bGain, sizeof(bGain));
}

static void SetPCM5242Ch2Volume( AmpList*  pstAmpList,eAmpIdx eIdx, BYTE bGain)
{
    PCM5242Write(pstAmpList->peList[eIdx], eRDtlVol, &bGain, sizeof(bGain));
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

static void SetPCM5242SubVolume(__UNUSED AmpList*  pstAmpList, __UNUSED eAmpIdx eIdx, __UNUSED BYTE bGain)
{
	return;
}

static void SetPCM5242Mute( AmpList*  pstAmpList)
{
	UINT32 i = 0;
    unPCM5242_reg0x3_t stPCM5242_Reg0x3;

    stPCM5242_Reg0x3.DataByte=PAGE0_REG3_DEFAULT;
    stPCM5242_Reg0x3.Mute_R=1;
    stPCM5242_Reg0x3.Mute_L=1;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		PCM5242Write(pstAmpList->peList[i], eSoftMute, &stPCM5242_Reg0x3.DataByte, 1);
	}
}

static void SetPCM5242DeMute( AmpList*  pstAmpList)
{
	UINT32 i = 0;
    unPCM5242_reg0x3_t stPCM5242_Reg0x3;

    stPCM5242_Reg0x3.DataByte=PAGE0_REG3_DEFAULT;
    stPCM5242_Reg0x3.Mute_R=0;
    stPCM5242_Reg0x3.Mute_L=0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		PCM5242Write(pstAmpList->peList[i], eSoftMute, &stPCM5242_Reg0x3.DataByte, 1);
	}
}

#if 0
int GetTas5711ErrStatus( AmpList* pstAmpList, BYTE* pStatus)
{
    UINT32 i = 0;
    BYTE bData = 0xff;

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        bData = 0xff;
        Tas5711Read(pstAmpList->peList[i], eErrStat, &bData, sizeof(bData));
        pStatus[i] = bData;
    }

    return (0);
}
#endif

/****************************************************
Function:PCM5242Init
Description:
Input:void
Output: None
Return Value:
Calls:
Called By:

History:
Date                      Modification        Author
2019/05/21            Created              bj.cheng
Others:

*****************************************************/
static BOOL PCM5242Init(AmpList* pstAmpList)
{
	BOOL bRet = FALSE;
        //char * pbModule = NULL;
        //char bModuleName[MODULENAMELEN] = {0};
	UINT32 i = 0;

	//printfw("%s\n",__FUNCTION__);

	// init PCM5242 reset pin
	if(Pinmux_GPIO_CheckStatus(PCM5242_RESET_GPIO) == 0)
	{
		if(Pinmux_GPIO_Alloc(PCM5242_RESET_GPIO,IO_GPIO,IO_RISC)==0)
		{
			GPIO_OE_Set(PCM5242_RESET_GPIO, IO_OUTPUT);
		}
	}
	else
	{
		return FALSE;
	}

#if 0 // do not open i2c here
        for(i=0;i<pstAmpList->pstAmpCfg->AmpNum;i++)
        {
            if(pbModule != NULL)
            {
                if(strcmp(pstAmpList->peList[i].pbI2cModule,pbModule) == 0)
                {
                    //printf("[%s] pbI2cModule = %s \n",__FUNCTION__,pstAmpList->peList[i].pbI2cModule);
                    pstAmpList->peList[i].sdI2cHandle =  pstAmpList->peList[i-1].sdI2cHandle;
                    continue;
                }
            }

            pbModule = pstAmpList->peList[i].pbI2cModule;
            //printf("[%s] pbModule = %s \n",__FUNCTION__,pbModule);
            sprintf( bModuleName, "C:/%s",pbModule);
            //printf("[%s] i2c path = %s \n",__FUNCTION__,bModuleName);

            pstAmpList->peList[i].sdI2cHandle = open(bModuleName,O_RDWR,0);
            if(pstAmpList->peList[i].sdI2cHandle < 0)
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
        PowerOnPCM5242();

        for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
        {
            SetPCM5242Output(pstAmpList, pstAmpList->peList[i].eAmpID);
        }
#if 0
        PCM5242_AMP_delay_ms(300);

        BYTE pState[2];

        GetTas5711ErrStatus(pstAmpList, pState);

        bRet = TRUE;

        if ((pState[0] != 0) || (pState[1] != 0))
        {
            LOGE("Tas5711 error!! error code: 0x%x, 0x%x\n", pState[0], pState[1]);
        }
#else
        bRet = TRUE;
#endif
	//}
	return (bRet);
}

int PCM5242_AMP_INIT( AmpList* pstAmpList)
{
	INT32 sdRet = -1;
	if(PCM5242Init(pstAmpList)==TRUE)
	{
		sdRet = 0;
	}
	return sdRet;
}

int PCM5242_AMP_SET_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
    BYTE* bVolTable;
    BYTE bStepNum;

    bVolTable = pstAmpList->pstAmpCfg->MainGainTbl;
    bStepNum = pstAmpList->pstAmpCfg->MainGainStepNum;

    if (bVolume >= bStepNum)
    {
        return -1;
    }

    //printfw("AMP bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

    UINT32 i = 0;

//BJ 20190523 vv
#if 1
    BYTE bDecVol=0x28;//for small speaker to decrease 20db

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        if(bVolTable[bVolume] >= 0xFF)
            SetPCM5242Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
        else
            SetPCM5242Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]+bDecVol);
    }
#else
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        SetPCM5242Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
    }
#endif
//BJ 20190523 ^^
    return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929
int PCM5242_AMP_SET_LR_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
    UINT32 i = 0;
    BYTE* bVolTable;
    BYTE bStepNum;

    bVolTable = pstAmpList->pstAmpCfg->Ch1_GainTbl;
    bStepNum = pstAmpList->pstAmpCfg->Ch1GainStepNum;

    if(bVolume >= bStepNum)
    {
        return -1;
    }

    //printfw("AMP LR_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

//BJ 20190523 vv
#if 1
    BYTE bDecVol=0x28;//for small speaker to decrease 20db

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        if(bVolTable[bVolume] >= 0xFF)
            SetPCM5242LRVolume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
        else
            SetPCM5242LRVolume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]+bDecVol);
    }
#else
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        SetPCM5242LRVolume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
    }
#endif
//BJ 20190523 ^^
    return 0;
}

int PCM5242_AMP_SET_CH1_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
    UINT32 i = 0;
    BYTE* bVolTable;
    BYTE bStepNum;

    bVolTable = pstAmpList->pstAmpCfg->Ch1_GainTbl;
    bStepNum = pstAmpList->pstAmpCfg->Ch1GainStepNum;

    if(bVolume >= bStepNum)
    {
        return -1;
    }

    LOGD("AMP CH1_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

//BJ 20190523 vv
#if 1
    BYTE bDecVol=0x28;//for small speaker to decrease 20db

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        if(bVolTable[bVolume] >= 0xFF)
            SetPCM5242Ch1Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
        else
            SetPCM5242Ch1Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]+bDecVol);
    }
#else
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        SetPCM5242Ch1Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
    }
#endif
//BJ 20190523 ^^
    return 0;
}

int PCM5242_AMP_SET_CH2_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
    UINT32 i = 0;
    BYTE* bVolTable;
    BYTE bStepNum;

    bVolTable = pstAmpList->pstAmpCfg->Ch2_GainTbl;
    bStepNum = pstAmpList->pstAmpCfg->Ch2GainStepNum;

    LOGD("AMP CH2_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

    if(bVolume >= bStepNum)
    {
        return -1;
    }

//BJ 20190523 vv
#if 1
    BYTE bDecVol=0x28;//for small speaker to decrease 20db

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        if(bVolTable[bVolume] >= 0xFF)
            SetPCM5242Ch2Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
        else
            SetPCM5242Ch2Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]+bDecVol);
    }
#else
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        SetPCM5242Ch2Volume(pstAmpList,pstAmpList->peList[i].eAmpID,bVolTable[bVolume]);
    }
#endif
//BJ 20190523 ^^
    return 0;
}

int PCM5242_AMP_SET_SUB_VOL( AmpList *pstAmpList, BYTE  bVolume )
{
    UINT32 i = 0;
    BYTE* bVolTable;
    BYTE bStepNum;

    bVolTable = pstAmpList->pstAmpCfg->Ch3_GainTbl;
    bStepNum = pstAmpList->pstAmpCfg->Ch3GainStepNum;

    if (bVolume >= bStepNum)
    {
        return -1;
    }

    //printfw("AMP SUB_bVolume=%d 0x%x \n",bVolume,bVolTable[bVolume]);

//BJ 20190523 vv
#if 1
    BYTE bDecVol=0x28;//for small speaker to decrease 20db

    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        if(bVolTable[bVolume] >= 0xFF)
            SetPCM5242SubVolume(pstAmpList,pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
        else
            SetPCM5242SubVolume(pstAmpList,pstAmpList->peList[i].eAmpID, bVolTable[bVolume]+bDecVol);
    }
#else
    for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
    {
        SetPCM5242SubVolume(pstAmpList,pstAmpList->peList[i].eAmpID, bVolTable[bVolume]);
    }
#endif
    return 0;
}

//jun.yang for volume adjustment system (mantis:0262891)20150929

int PCM5242_AMP_MUTE_ON( AmpList* pstAmpList)
{
	SetPCM5242Mute(pstAmpList);

	return (0);
}

int PCM5242_AMP_MUTE_OFF( AmpList* pstAmpList)
{
	SetPCM5242DeMute(pstAmpList);

	return (0);
}

int PCM5242_AMP_LR_CHANNEL( AmpList* pstAmpList)
{
	UINT32 i = 0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetPCM5242LRChannel(pstAmpList, pstAmpList->peList[i].eAmpID, eLOnly);
	}

	return (0);
}

int PCM5242_AMP_SET_SUBMODE( AmpList* pstAmpList)
{
	UINT32 i = 0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetPCM5242Mode2_1(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int PCM5242_AMP_SET_STEREOMODE( AmpList* pstAmpList)
{
	UINT32 i = 0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetPCM5242Mode2_0(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int PCM5242_AMP_SET_PBTLMODE( AmpList* pstAmpList)
{
	UINT32 i = 0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetPCM5242PBTLMode(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int PCM5242_AMP_DRC_CTRL( AmpList* pstAmpList)
{
	UINT32 i = 0;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		SetPCM5242DRCCtrl(pstAmpList, pstAmpList->peList[i].eAmpID);
	}

	return (0);
}

int PCM5242_AMP_POWER_RESET( AmpList* pstAmpList)
{

	UINT32 i = 0;
	//BYTE bData = 0x40;

	for(i=0; i<pstAmpList->pstAmpCfg->AmpNum; i++)
	{
		//PCM5242Write(pstAmpList->peList[i], eSysCtl2, &bData, sizeof(bData));		//first enter shutdown mode
	}

	PCM5242_AMP_delay_ms(170); // get delay time by reg 0x1a

	PowerOffPCM5242();

	return (0);
}
