/******************************************************************************/
/**
*
* \file	 amp.c
*
* \brief	8107 Class-D Amp Api .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#include "amp.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[AMP]"
#include "log_utils.h"

extern AMPOP_T	 g_stTas5711;
extern AMPOP_T	 g_stAD83584D;
extern AMPOP_T   g_stAD85050;
extern AMPOP_T   g_stTas5825M;
extern AMPOP_T   g_stPCM5242;

//amp driver
static AMPOP_T *g_pstAmp[MAX_AMP] =
{
	NULL,
	NULL,
	NULL,
};

/******************************************************************************************/
/**
 * \fn		INT32 AmpInit(UINT32 dAmp)
 *
 * \brief		Init AMP Driver
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpInit(AmpList *pstAmpList)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];

	if (NULL != pfnampop && pfnampop->pfn_init)
	{
		pfnampop->pfn_init(pstAmpList);
		sdRet = 0;
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_vol)
	{
		sdRet = pfnampop->pfn_set_vol(pstAmpList, bVolume);
	}

	LOGD("vol set result: %d\n", sdRet);

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_LR_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set L/R Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_LR_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_lrvol)
	{
		sdRet = pfnampop->pfn_set_lrvol(pstAmpList, bVolume);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_SUB_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set sub Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_SUB_Volume( AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_subvol)
	{
		sdRet = pfnampop->pfn_set_subvol(pstAmpList, bVolume);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch1_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_Ch1_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop&&pfnampop->pfn_set_ch1vol)
	{
		sdRet = pfnampop->pfn_set_ch1vol(pstAmpList, bVolume);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch2_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_Ch2_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop&&pfnampop->pfn_set_ch2vol)
	{
		sdRet = pfnampop->pfn_set_ch2vol(pstAmpList, bVolume);
	}

	return sdRet;
}
//
INT32 AmpSet_Ch3_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_ch3vol)
	{
		sdRet = pfnampop->pfn_set_ch3vol(pstAmpList, bVolume);
	}

	return sdRet;
}
INT32 AmpSet_Ch4_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_ch4vol)
	{
		sdRet = pfnampop->pfn_set_ch4vol(pstAmpList, bVolume);
	}

	return sdRet;
}
INT32 AmpSet_Ch5_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_ch5vol)
	{
		sdRet = pfnampop->pfn_set_ch5vol(pstAmpList, bVolume);
	}

	return sdRet;
}
INT32 AmpSet_Ch6_Volume(AmpList *pstAmpList, BYTE  bVolume )

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_ch6vol)
	{
		sdRet = pfnampop->pfn_set_ch6vol(pstAmpList, bVolume);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_MUTE_ON( AmpList *pstAmpList)
 *
 * \brief		AMP Driver enable mute mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_MUTE_ON( AmpList *pstAmpList)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_mute_on)
	{
		sdRet = pfnampop->pfn_mute_on(pstAmpList);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_MUTE_OFF( AmpList *pstAmpList)
 *
 * \brief		AMP Driver disable mute mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_MUTE_OFF( AmpList *pstAmpList)
{

	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_mute_off)
	{
		sdRet = pfnampop->pfn_mute_off(pstAmpList);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_LRChannel( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set L/R channel
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_LRChannel( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_lrchannel)
	{
		sdRet = pfnampop->pfn_set_lrchannel(pstAmpList);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_SubMode( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set sub mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_SubMode( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_submode)
	{
		sdRet = pfnampop->pfn_set_submode(pstAmpList);
	}

	return sdRet;
}


/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_StereoMode( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set stereo mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_StereoMode( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_stereomode)
	{
		sdRet = pfnampop->pfn_set_stereomode(pstAmpList);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_PBTLMode( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set pbtl mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_PBTLMode( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_pbtlmode)
	{
		sdRet = pfnampop->pfn_set_pbtlmode(pstAmpList);
	}

	return sdRet;
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_DRCCtrl( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set drc ctrl
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_DRCCtrl( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_drc_ctrl)
	{
		sdRet = pfnampop->pfn_drc_ctrl(pstAmpList);
	}

	return sdRet;
}
INT32 AmpSet_EQ( AmpList *pstAmpList, BYTE eq)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_eq)
	{
		sdRet = pfnampop->pfn_set_eq(pstAmpList, eq);
	}

	return sdRet;
}

INT32 AmpSet_TrebleVol( AmpList *pstAmpList, BYTE vol)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_set_treble)
	{
		sdRet = pfnampop->pfn_set_treble(pstAmpList, vol);
	}

	return sdRet;
}

INT32 AmpSet_SetSrcGain( AmpList *pstAmpList, BYTE src)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_src_gain)
	{
		sdRet = pfnampop->pfn_src_gain(pstAmpList, src);
	}

	return sdRet;
}

INT32 AmpSet_SetSurroundGain( AmpList *pstAmpList, BYTE status)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_surround_gain)
	{
		sdRet = pfnampop->pfn_surround_gain(pstAmpList, status);
	}

	return sdRet;
}


/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_PowerOff( AmpList *pstAmpList)
 *
 * \brief		AMP Driver set sub mode
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_PowerOff( AmpList *pstAmpList)

{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop && pfnampop->pfn_power_off)
	{
		sdRet = pfnampop->pfn_power_off(pstAmpList);
	}

	return sdRet;
}

INT32 AmpSet_bypass( AmpList *pstAmpList, BYTE mode)
{
	AMPOP_T *pfnampop = NULL;
	INT32 sdRet = -1;

	if (pstAmpList->pstAmpCfg->AmpModule >= MAX_AMP)
	{
		return sdRet;
	}

	pfnampop = g_pstAmp[pstAmpList->pstAmpCfg->AmpModule];
	if (NULL != pfnampop &&  pfnampop->pfn_set_bypass)
	{
		sdRet = pfnampop->pfn_set_bypass(pstAmpList, mode);
	}

	return sdRet;
	
}

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_enable_dap_automute( AmpList *pstAmpList)
 *
 *\yuanwei
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
void main_amptest(void)
{
	//amp cfg
	AmpModuleConf stTestAmp[] =
	{
		{eAmp1, "i2c2_sw", 0x34, -1},
		{eAmp2, "i2c2_sw", 0x36, -1},
		{eAmp3, "i2c1_sw", 0x34, -1},
	};

	AmpList stList =
	{
		.peList = stTestAmp,
	};

	AmpInit((AmpList *)(&stList));

	AmpSet_MUTE_ON(&stList);

	AmpSet_MUTE_OFF(&stList);
}


void Insert_AD85050drv(void)
{
	g_pstAmp[eAD85050] = &g_stAD85050;
}

void Insert_TAS5711drv(void)
{
	g_pstAmp[eTas5711] = &g_stTas5711;
}


