/******************************************************************************/
/**
*
* \file	 Amp.h
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
#ifndef __AMP_H__
#define __AMP_H__

/*----------------------------------------------*
  * INCLUDE  DECLARATIONS
  *----------------------------------------------*/
#include "types.h"
#include "amp_cfg.h"

//#define MODULENAMELEN    (10)

/**
* \brief	Amp struct
*/

typedef struct _AmpList
{
	AmpModuleConf *peList;//指向AmpModuleConf类型的结构体指针
	CFG_AMP_T *pstAmpCfg;//指向CFG_AMP_T类型的结构体指针
} AmpList;

typedef int (*ampop_init)( AmpList *pstAmpList);
typedef int (*ampop_set_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_lrvol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_subvol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_ch1_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_ch2_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_mute_on)( AmpList *pstAmpList);
typedef int (*ampop_mute_off)( AmpList *pstAmpList);
typedef int (*ampop_set_lrchannel)( AmpList *pstAmpList);
typedef int (*ampop_set_submode)( AmpList *pstAmpList);
typedef int (*ampop_set_stereomode)( AmpList *pstAmpList);
typedef int (*ampop_set_pbtlmode)( AmpList *pstAmpList);
typedef int (*ampop_drc_ctrl)( AmpList *pstAmpList);
typedef int (*ampop_power_off)( AmpList *pstAmpList);
typedef int (*ampop_set_ch3_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_ch4_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_ch5_vol)( AmpList *pstAmpList, BYTE  bVolume);
typedef int (*ampop_set_ch6_vol)( AmpList *pstAmpList, BYTE  bVolume);
//typedef int (*ampop_check_amp_state) ( AmpList* pstAmpList);
typedef int (*ampop_set_eq)( AmpList *pstAmpList, BYTE  eq);
typedef int (*ampop_set_treble)( AmpList *pstAmpList, BYTE  vol);
typedef int (*ampop_src_gain)( AmpList *pstAmpList, BYTE  src);
typedef int (*ampop_surround_gain)( AmpList *pstAmpList, BYTE status);
typedef int (*ampop_bypass)( AmpList *pstAmpList, BYTE status);

typedef struct _AMPOP_S
{
	ampop_init			pfn_init;
	ampop_set_vol		pfn_set_vol;
	ampop_set_lrvol		pfn_set_lrvol;
	ampop_set_subvol		pfn_set_subvol;
	ampop_set_ch1_vol       pfn_set_ch1vol;
	ampop_set_ch2_vol       pfn_set_ch2vol;
	ampop_mute_on		pfn_mute_on;
	ampop_mute_off		pfn_mute_off;
	ampop_set_lrchannel	pfn_set_lrchannel;
	ampop_set_submode	pfn_set_submode;
	ampop_set_stereomode	pfn_set_stereomode;
	ampop_set_pbtlmode	pfn_set_pbtlmode;
	ampop_drc_ctrl		pfn_drc_ctrl;
	ampop_power_off		pfn_power_off;
	ampop_set_ch3_vol   pfn_set_ch3vol;
	ampop_set_ch4_vol   pfn_set_ch4vol;
	ampop_set_ch5_vol   pfn_set_ch5vol;
	ampop_set_ch6_vol   pfn_set_ch6vol;

	ampop_set_eq   pfn_set_eq;
	ampop_set_treble pfn_set_treble;
	ampop_src_gain pfn_src_gain;
	ampop_surround_gain pfn_surround_gain;

	ampop_bypass  pfn_set_bypass;
	
	//ampop_check_amp_state  pfn_check_amp_state;
} AMPOP_T;

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
INT32 AmpInit(AmpList *pstAmpList);

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
INT32 AmpSet_Volume( AmpList *pstAmpList, BYTE  bVolume );

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
INT32 AmpSet_LR_Volume( AmpList *pstAmpList, BYTE  bVolume );

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
INT32 AmpSet_SUB_Volume( AmpList *pstAmpList, BYTE  bVolume );

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
INT32 AmpSet_Ch1_Volume(AmpList *pstAmpList, BYTE  bVolume );

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
INT32 AmpSet_Ch2_Volume(AmpList *pstAmpList, BYTE  bVolume );
/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch3_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/

INT32 AmpSet_Ch3_Volume(AmpList *pstAmpList, BYTE  bVolume );
/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch4_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
INT32 AmpSet_Ch4_Volume(AmpList *pstAmpList, BYTE  bVolume );
/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch5_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/

INT32 AmpSet_Ch5_Volume(AmpList *pstAmpList, BYTE  bVolume );

/******************************************************************************************/
/**
 * \fn		INT32 AmpSet_Ch6_Volume( AmpList *pstAmpList, BYTE  bVolume )
 *
 * \brief		AMP Driver Set Volume
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/

INT32 AmpSet_Ch6_Volume(AmpList *pstAmpList, BYTE  bVolume );

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
INT32 AmpSet_MUTE_ON( AmpList *pstAmpList);

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
INT32 AmpSet_MUTE_OFF( AmpList *pstAmpList);

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
INT32 AmpSet_LRChannel( AmpList *pstAmpList);

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
INT32 AmpSet_SubMode( AmpList *pstAmpList);

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
INT32 AmpSet_StereoMode( AmpList *pstAmpList);

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
INT32 AmpSet_PBTLMode( AmpList *pstAmpList);

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
INT32 AmpSet_DRCCtrl( AmpList *pstAmpList);

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
INT32 AmpSet_PowerOff( AmpList *pstAmpList);

INT32 AmpSet_EQ(AmpList *pstAmpList, BYTE eq);
INT32 AmpSet_TrebleVol( AmpList *pstAmpList, BYTE vol);
INT32 AmpSet_SetSrcGain( AmpList *pstAmpList, BYTE src);
INT32 AmpSet_SetSurroundGain( AmpList *pstAmpList, BYTE status);
INT32 AmpSet_bypass( AmpList *pstAmpList, BYTE mode);

extern AmpList stAmpList;

void Insert_TAS5711drv(void);

void Insert_AD83584Ddrv(void);

void Insert_AD85050drv(void);

void Insert_TAS5825Mdrv(void);

void Insert_PCM5242drv(void);



enum
{
	EQ_MOVIE,
	EQ_MUSIC,
	EQ_NEWS,
	EQ_SPORTS,
	EQ_CUST,
	EQ_MAX,
};

#endif // __AMP_H__

