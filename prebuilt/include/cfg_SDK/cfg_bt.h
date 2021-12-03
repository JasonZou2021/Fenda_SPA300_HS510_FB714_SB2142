/******************************************************************************/
/**
*
* \file	    cfg_bt.h
*
* \brief	bt SDK configuration sample code .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   wenqi@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/01/09
******************************************************************************/

#ifndef CFG_BT_H
#define CFG_BT_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*
 *    MACRO DECLARATIONS
 *---------------------------------------------------------------------------*/
//#define BT_SDK_RANDOM
#define ADDR_LENGTH 6
#define BT_HEADPHONE_NUN 6
#define BT_SUB_NUN 1
#define RECODE_REMOTE_DEV_NUN 7
#define NAME_LEN   48//30
#define LINKKEY_SIZE   16
#define BT_SINK_DEV_NUM		1
#define BT_SOURCE_DEV_NUM	1
#define BT_SPP_DEV_NUM	1

/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
typedef enum
{
	BT_MAIN,
	BT_SUB,
} BT_PLATFORM_ROLE_e;


typedef enum ENABLE_STATE
{
	OFF,
	ON,
} ENABLE_STATE_e;


typedef enum AUTOCONN_DEX
{
	AUTO_NONE,
	AUTO_PHONE, //8107sub: for mian;  8107main: for phone
	AUTO_TWS,
	AUTO_PHONE_TWS,
} AUTOCONN_DEX_e;


typedef enum
{
	SINK_NONE,
	WSS,
	HEADPHONE,
} SINK_MODE_e;



typedef struct st_BtMainConf
{
	//bt local info config
	char 			address[ADDR_LENGTH];//only work for not in BtRamdom
	char			LocalName[NAME_LEN];
	//if no to disconnect phone when platform switch source
	char 			IsIgnoreSwitchSrc;
	//if 8107mian to  connect back when phone leave away from the platform
	char			IsIgnoreDevTimeout;
	//auto config
	char 			AutoRetryTimes;//retry times
	int 			AutoLatency;  //connect delay times tick :0.5s
	ENABLE_STATE_e 	AutoWorkState;// auto connect work on or off
	AUTOCONN_DEX_e 	Autorole;//set auto connect target role:headphone or phone
	//tws surport config
	ENABLE_STATE_e  TwsWork;
	SINK_MODE_e   	SinkMode;//decide a2dpsource:headphone or wss
	char 			IsConnectedSetDelay; //if  8107mian set speakout delay after  sub connected success
	//headphone list
	const char 		**pHeadphoneListName;
	const char 		**pSubListName;
	//bt hint play config
	ENABLE_STATE_e	HintWorkState;
	//bt transmode config
	ENABLE_STATE_e	TransMode;
} CFG_BTMAIN_T;



typedef struct st_BtSubConf
{
	//bt local info config
	char 			address[ADDR_LENGTH];
	char			LocalName[NAME_LEN];
	char 			DesName[NAME_LEN];
	//auto config
	char 			AutoRetryTimes;//retry times
	int 			AutoLatency;  //connect delay times tick :0.5s
	ENABLE_STATE_e 	AutoWorkState;// auto connect work on or off
	AUTOCONN_DEX_e 	Autorole;//set auto connect target role:Main
	//bt hint play config
	ENABLE_STATE_e	HintWorkState;
	//standby config
	ENABLE_STATE_e   StanbyMode;
	//bt transmode config
	ENABLE_STATE_e	TransMode;
} CFG_BTSUB_T;


typedef union
{
	CFG_BTMAIN_T BtMainCfg;
	CFG_BTSUB_T  BtSubCfg;
} BT_ROLE_CFG;


typedef struct st_BT_CFG
{
	BT_PLATFORM_ROLE_e PlatformRole;
	ENABLE_STATE_e	BtAutoWork;
	char 	IsSurportTwsSwitch;
	BT_ROLE_CFG	BtSdkCfg;
} CFG_BT_T;

typedef struct st_BT_PRIVATECFG
{
	unsigned int VerdorIdSource;
	unsigned int vid;
	unsigned int pid;
	unsigned int version;

	//avrcp
	char* avrcpCtServiceName;	//<25
	char* avrcpCtProviderName;	//<25

	char license[48];
}CFGPRIVATE_BT_T;


#endif

