/******************************************************************************/
/**
*
* \file	systemservice.h
*
* \brief	provide system service.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   tp.jiang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/06/27
******************************************************************************/


#ifndef SYSTEMSERVICE_H
#define SYSTEMSERVICE_H


/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qp_pub.h"
#include "powerservice.h"
#include "upgradeservice.h"
#include "upgradefileloader.h"

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/


/******************************************************************************************/
/**
 * \fn		QActive * SystemService_GetHandler(void)
 *
 * \brief		get SystemService's handler
 *
 * \param  	none
 *
 * \return		SystemService's handler.
 *
 * \note		none.
 *
 ******************************************************************************************/

QActive *SystemService_GetHandler(void);

/******************************************************************************************/
/**
 * \fn		int SystemService_RegVolume(QActive * const me, ARFunc_t pfFunc)
 *
 * \brief		regist SYSTEM_VOLUME_CHANGE_SIG & pfFunc to 'me', system will run pfFunc if receive SYSTEM_VOLUME_CHANGE_SIG
 *			to notify all AO that volume insert or plug-out
 *
 * \param 		me : (Input) the AO' handler to regist
 * \param 		pfFunc : (Input) the listener function
 *
 * \return		\e ERROR on fail. \n
 *            	\e SUCCESS on success \n
 *
 * \sa          SystemService_UnregVolumeListener()
 *
 ******************************************************************************************/
int SystemService_RegVolumeListener(QActive *const me, ARFunc_t pfFunc);

/******************************************************************************************/
/**
 * \fn		int SystemService_UnregVolumeListener(QActive * const me, ARFunc_t pfFunc)
 *
 * \brief		unregist SYSTEM_VOLUME_CHANGE_SIG & pfFunc to 'me'
 *
 * \param 		me : (Input) the AO' handler to unregist
 * \param 		pfFunc : (Input) the listener function
 *
 * \return		\e ERROR on fail. \n
 *            	\e SUCCESS on success \n
 *
 * \sa          SystemService_RegVolumeListener()
 *
 ******************************************************************************************/
int SystemService_UnregVolumeListener(QActive *const me, ARFunc_t pfFunc);

/*****************************************************************************
*
 * \fn		 void* RomCpy(void *pwData,void *pwid,UINT32 dSize)
 *
 * \brief	get the data from flash
 * \param  pwiid:src data addr,  dSize data size
 *            pwData dist data addr
 * \return	UINT16* data addr
*
 * \note		none.
 * ******************************************************************************/
void *RomCpy(void *pwData, void *pwid, UINT32 dSize);

typedef struct _UpgradePro_S
{
	QEvt super;				/*!< qp evt parent */
	void *pstSender;
	int(* getPercent)(void *);			/*!<callback to get UI progress*/
} UPGRADEPRO;

typedef struct _UPGBINLIST_S  			/*!< qp evt parent */
{
	UINT8 *pbList;			/*!<AP upgrade bin list*/
} UPGBINLIST_T;

typedef struct _UPGRETURN_S  			/*!< qp evt parent */
{
	INT32 sdRen;			/*!<AP upgrade UI return*/
} UPGRETURN_T;

typedef struct _UPGRADESELEVT_S
{
	QEvt super;				/*!< qp evt parent */
	UINT8 type;			/*!<AP upgrade  bin type*/
} UPGRADESELEVT_T;

typedef struct _UPGRADESTARTEVT_S
{
	QEvt super;				/*!< qp evt parent */
	QActive *pstSender;
	UINT8 erase_usersetting_flag;
} UPGRADESTARTEVT_T;


#define SYS_AUDIO_INFO_REQ (0)
#define SYS_VOL_CHG (1 << 0)
#define SYS_MUTE_CHG (1 << 1)

typedef struct
{
	QEvt stEvt;
	UINT8 BroadcastState;
	INT16 Vol;
	INT8 MuteState;
	INT16 MaxVol;
	INT16 MinVol;
} stSYS_AudioInfo_t;

// int UpgradeService_RegGetProlLSR(void *const sender, ARFunc_t pfFunc);
#if(0)
void UpgradeService_Start(void *const sender);
void UpgradeService_Select(void *const sender, UINT8 dShowType);
void UpgradeService_StartScan(void const *pSender, ARFunc_t pfFunc);
void UpgradeService_UpgradeFinish(void const *pSender, ARFunc_t pfFunc);
#endif

int SystemService_RegAudioInfoListener(QActive *const me, ARFunc_t pfFunc);
int SystemService_UnRegAudioInfoListener(QActive *const me);
int SystemService_AudioInfoBroadcast(void const *pSender,
									 UINT8 type,
									 INT8 MuteState,
									 INT16 Vol,
									 INT16 MaxVol,
									 INT16 MinVol);

int SystemService_RegAudioInfoBroadcastListener(QActive *const me, ARFunc_t pfFunc);
int SystemService_UnRegAudioInfoBroadcastListener(QActive *const me);
int SystemService_AudioInfoBroadcastRequest(void const *pSender);


#endif
