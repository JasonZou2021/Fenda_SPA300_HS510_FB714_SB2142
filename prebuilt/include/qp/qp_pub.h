/******************************************************************************/
/**
*
* \file	qp_pub.h
*
* \brief	QP callback function & declarations.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   yangf.wang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/05/05
******************************************************************************/


#ifndef QP_PUB_H
#define QP_PUB_H

/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "qpc.h"

/*---------------------------------------------------------------------------*
*                            MACRO   DECLARATIONS                            *
*----------------------------------------------------------------------------*/

#define TICKS_PER_SEC   (100U)

#define TICKS_PER_100MS   (TICKS_PER_SEC/10)
#define TICKS_PER_200MS   (TICKS_PER_SEC/5)
#define TICKS_PER_250MS   (TICKS_PER_SEC/4)
#define TICKS_PER_500MS   (TICKS_PER_SEC/2)

//reserve some signals for customer
#define CUSTOMER_PUB_SIG_NUM (10)
#define CUSTOMER_PRIVATE_SIG_NUM (1000)

/*!< small pool */
#define POOL_SML_SIZE   (8)
#define POOL_SML_NUM    (64)
/*!< medium pool */
#define POOL_MED_SIZE   (16)
#define POOL_MED_NUM    (32)
/*!< large pool */
#define POOL_LAR_SIZE   (32)
#define POOL_LAR_NUM    (32)

/*!< the beginning of publish signal in each module */
#define PUB_GRP_START Q_USER_SIG

#define CUSTOMER_PUB_GRP_START (80)


/*!< the beginning of private signal in each module */
#define PRI_GRP_START (200)

#define SYS_PRI_GRP_START PRI_GRP_START
#define FL_PRI_GRP_START (300)
#define IOP_PRI_GRP_START (400)
#define LOG_PRI_GRP_START (500)
#define DSP_PRI_GRP_START (600)
#define MFP_PRI_GRP_START (700)
#define BT_PRI_GRP_START (800)
#define NET_PRI_GRP_START (900)
#define DMR_PRI_GRP_START (1000)
#define AIRPLAY_PRI_GRP_START (1100)
#define EXEMPLE_PRI_GRP_START (1200)
#define HDMI_PRI_GRP_START (1300)

/*!< the beginning of private signal in customer layer */
#define CUSTOMER_PRI_SIG_START (3000)


/*---------------------------------------------------------------------------*
 *                               DATE    TYPES                               *
 *---------------------------------------------------------------------------*/
/**
*\brief volume action
*/
typedef enum VOLUMEACT_S
{
 	VOLUME_REMOVE = 0,
 	VOLUME_ADD = 1,
    VOLUME_ERR_OUT = 2,
    VOLUME_ERR_IN  = 3,
    VOLUME_ERR_HANDLE_STOP = 4,
}VOLUMEACT;

/**
*\brief volume param
*/
typedef struct
{
	char *psbDevName;
	char *psbDiskName;
	VOLUMEACT eAction;
} VOLUMEPARAM;

/**
*\brief define QP event signal index, contain:
*		1. publish
*		2. listener: belong to part of publish
*		3. callback: belong to part of private
*		4. private
*/
enum QEVTSIG
{

/*---------------------------------------------------------------------------*
 *                         publish signal area                               *
 *---------------------------------------------------------------------------*/
    /*!< add AO public signal from here : */
    /*.................................system service publish signal.........................................*/
    SYSTEM_START_POWER_OFF_SIG = PUB_GRP_START,
    SYSTEM_UPGRADE_PUTPIPE_SIG,
    SYSTEM_UPGRADE_FREEPIPE_SIG,
    SYSTEM_UPGRADE_PROGRESS_SIG,
    SYSTEM_UPGRADE_GET_PROGRESS_SIG,
    SYSTEM_UPGRADE_FAIL_SIG,
    SYSTEM_UPGRADE_END_SIG,
    SYSTEM_VOLUME_CHANGE_SIG,
    SYSTEM_AUDIO_INFO_SIG,
    SYSTEM_AUDIO_BROADCAST_SIG,
#ifdef BUILT4ISP2
    SYSTEM_BUILT4ISP2_SIG,
#endif

    /*.................................audioplayer publish signal.........................................*/
    /** input */
    MFP_PREPARE_SIG, 			/* published by user to perpare player */
    MFP_PLAY_SIG,				/* published by user to play music */
    MFP_STOP_SIG,				/* published by user to stop player */
    MFP_STOP_OVER_SIG,		    /* published to user after stop player */
    MFP_PAUSE_SIG,				/* published by user to pause player */
    MFP_SEEK_SIG,				/* published by user to seek player */
    MFP_TRICKY_SIG,				/* published by user to tricky player */
    MFP_CONTROL_SIG,			/* published by player to user for play control */
    MFP_SWITCH_FILE_SIG,		/* published by user to switch file playback */
    MFP_DESTORY_SIG,			/* published by user to player destoryed*/

    /*.................................audio DSP service publish signal.........................................*/
    ODSP_NOTIFY_SIG,
    ODSP_ERR_SIG,            /* ODSP report error */
    ODSP_MESSAGE_SIG,
    ODSP_SOUNDHINT_FINISH_SIG, /* ODSP Hint Sound MP3 */
    ODSP_RESET_START_SIG,
    ODSP_RESET_END_SIG,
    ODSP_HINTSOUND_END_SIG,
	ODSP_FOAMAT_CHANGE_SIG,	 /* ODSP get audio format */
    /*.................................iop publish signal.........................................*/
    IOP_KEY_SIG,
    IOP_CEC_HPD_SIG,
    IOP_CEC_EVT_SIG,
    IOP_MUTE_REQ_SIG,
    IOP_AUDIO_INFO_REPORT_SIG,
    IOP_IR_LEARNING_GET_NEW_SIG,

    /*.............................BURN_TEST.............................................*/
    AUTOSRC_TEST_OPEN_SIG,
    MONKEY_TEST_OPEN_SIG,
    SYSTEM_MEMORY_CHECK_OPEN_SIG,
    AUTO_REPEAT_SIGNAL_TEST_OPEN_SIG,
    BURN_TEST_CLOSE_SIG,

    /*.............................MCU interface public signal.............................................*/
    MCUIF_RCV_PKT_SIG, /* a packet has received */
    MCUIF_SND_PKT_SIG, /* to send a packet */
    MCUIF_TIMEEVT_SIG, /* TimeEvt signal */
    MCUIF_SYS_UPGRD_SIG, /* Request to upgrade the DSP firmware */
    MCUIF_ISP_UPGRD_SIG, /* Request to upgrade the DSP firmware via ISP*/
    MCUIF_SEND_DATA_SIG, /* Send upgrade bin file to DSP */
    MCUIF_UPGRD_END_SIG, /* inform MCUIF upgrade flow is terminated */

    /*.............................BT.............................................*/
    BT_STANDBY_SIG,

    /*.............................DMR.............................................*/
    DMR_VOL_CHANGE_REQ,
    DMR_SRC_EXIT,

    /*.............................WIFI.............................................*/
    WIFI_EVENT_SIG,

    /*.............................airplay.............................................*/
    Airplay_VOL_CHANGE_REQ,
    AIRPLAY_SRC_EXIT,

    /*.............................Exemple: new signal.............................................*/
    // add new publish signal below!!{
#if 0 // exemple
    /*.................................exemple ao publish signal.........................................*/
    EXEMPLE_PUB_A_SIG,
    EXEMPLE_PUB_B_SIG,
    EXEMPLE_PUB_C_SIG,
    EXEMPLE_PUB_D_SIG,
    //........

    /*..........................................................................*/
#endif

    // add new publish signal above!!}

    SDK_MAX_PUB_SIG, // It is the last one of SDK publish signals

    /*.............................customer.............................................*/
    //these signals are reserved for customer, and must put in the last of public signals
    // DO NOT add any signal here, customer signals sould put in custom_sig.h
    CUSTOMER_PUB_SIG_START = CUSTOMER_PUB_GRP_START,
    CUSTOMER_PUB_SIG_END = CUSTOMER_PUB_SIG_START + CUSTOMER_PUB_SIG_NUM - 1,

    /*.................................end........................................*/
    MAX_PUB_SIG, // the end of publish signals



/*---------------------------------------------------------------------------*
 *                         private signal area                               *
 *---------------------------------------------------------------------------*/
    /*!< add AO private signal from here : */
    /*.................................system service private signal.........................................*/
    SYSTEM_POWER_STANDBY_SIG = SYS_PRI_GRP_START,
    SYSTEM_FINISH_POWER_OFF_SIG,
    SYSTEM_POWER_RESET_SIG,
    SYSTEM_START_POWER_OFF_TIMEOUT_SIG,
    SYSTEM_POWER_UPGRADE_PREWORK_SIG,
    SYSTEM_UPGRADE_TIMEOUT_SIG,
    SYSTEM_UPGRADE_VERSION_CHECK_SIG,
    SYSTEM_UPGRADE_USB_INSERT_SIG,
    SYSTEM_UPGRADE_USB_SIG,
    SYSTEM_UPGRADE_BT_SIG,
    SYSTEM_UPGRDLOADER_START_SIG,
    SYSTEM_UPGRDLOADER_SEND_DATA_SIG,
    SYSTEM_UPGRDLOADER_TIMEOUT_SIG,
    SYSTEM_UPGRDLOADER_END_SIG,
    SYSTEM_UPGRADE_REGUEST_SIG,
    SYSTEM_UPGRADE_START_SIG,
    SYSTEM_UPGRADE_PREWORK_DONE_SIG,
    SYSTEM_UPGRADE_WORK_SIG,
    SYSTEM_UPGRADE_PREPARE_SIG,
    SYSTEM_UPGRADE_ACK_SIG,
    SYSTEM_UPGRADE_SCAN_SIG,
    SYSTEM_SAVEDATA_SYNC,
    UPGRADE_SCAN_FINISH_SIG,
    SYSTEM_UPGRADE_SELECT_SIG,
    UPGRADE_WORK_FINISH_SIG,
    SYSTEM_HEART_BEAT_SIG,
    SYSTEM_UPGRADE_PIPIREDY_SIG,
    SYSTEM_DIRTYSECTOR_RECYCLE,

    SYSTEM_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................filelist private signal.........................................*/
    FL_SCAN_FILE_SIG = FL_PRI_GRP_START,
    FL_STOP_SCAN_FILE_SIG,
    FL_SCAN_FILE_OVER_SIG,
    FL_SCAN_FILE_COUNT_SIG,
    FL_FILE_COUNT_UPDATE_SIG,
    FL_MODE_SET_SIG,
    FL_CUR_SET_SIG,
    FL_MODE_GET_SIG,
    FL_MODE_UPDATE_SIG,
    FL_NEXT_GET_SIG,
	FL_NEXT_FILE_GET_SIG,
    FL_NEXT_UPDATE_SIG,
	FL_PREV_GET_SIG,
	FL_PREV_FILE_GET_SIG,
    FL_PREV_UPDATE_SIG,
    FL_NEXT_FLODER_GET_SIG,
    FL_NEXT_FLODER_UPDATE_SIG,
    FL_PREV_FLODER_GET_SIG,
    FL_PREV_FLODER_UPDATE_SIG,
    FL_GET_CUR_FILE_SIG,
    FL_ID3_GET_SIG,
    FL_ID3_UPDATE_SIG,
    FL_JUMP_NEXT_SIG,
    FL_FILE_SRC_OUT_SIG,

    FL_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................iop scanner private signal.........................................*/
    IOP_PRI_SCAN_SIG = IOP_PRI_GRP_START,
    IOP_PRI_VFD_CONST_SIG,
    IOP_PRI_VFD_SCOLL_SIG,
    IOP_PRI_VFD_BLINK_SIG,
    IOP_PRI_VFD_DISPLAY_SIG,
    IOP_PRI_VFD_ALIGN_SIG,
    IOP_PRI_VFD_POP_SIG,
    IOP_PRI_VFD_COUNTER_SIG,
    IOP_PRI_VFD_Play_Time_SIG,
    IOP_PRI_VFD_SHOWDARK_SIG,
    IOP_IR_LEARNING_START_SIG,
    IOP_IR_LEARNING_TERMINATE_SIG,
    IOP_IR_LEARNING_CONFIRM_SIG,
    IOP_IR_LEARNING_ERASE_SIG,
    IOP_TV_REMOTE_READY_SIG,
    IOP_ADC_DATA_GET_SIG,

    IOP_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................audio LOG service private signal.........................................*/
    OLOG_PREPARE_SIG = LOG_PRI_GRP_START,
    OLOG_FEEDBACK_SIG,
    OLOG_FRAME_READY_SIG,
    OLOG_CLOSE_SIG,

    LOG_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................audio DSP service private signal.........................................*/
    ODSP_KARAOKE_SIG = DSP_PRI_GRP_START,
    ODSP_SINE_SIG,
    ODSP_XEFCONFIG_SIG,            /* ODSP Xeffect */
    ODSP_XEFPOLLING_SIG,
    ODSP_ENC_ERR_SIG,            /* ODSP report error */
    ODSP_ENC_PREPARE_SIG,
    ODSP_ENC_ENCODE_SIG,
    ODSP_ENC_PAUSE_SIG,
    ODSP_ENC_STOP_SIG,
    ODSP_ENC_PREPARE_END_SIG,    /* ODSP report ready to play */
    ODSP_ENC_OVER_SIG,           /* ODSP report STOP */
    ODSP_ENC_RETRIEVE_SIG,       /* ODSP request player to fill up ring buffer */
    ODSP_ENC_PRV_STOP_SIG,
    ODSP_ENC_FINISH_RETRIEVE_SIG,

    ODSP_DSR_SIG,


    ODSP_REPORT_MSMARTM_SIG,     //!report main smartmode
    //Add spdif
    ODSP_SPDIF_PREPARE_SIG,
    ODSP_SPDIF_ACTIVE_SIG,
    ODSP_SPDIF_INACTIVE_SIG,
    ODSP_SPDIF_POLLING_SIG,
    ODSP_SPDIF_CHANGE_SIG,
    ODSP_SPDIF_PARSING_SIG,
    ODSP_SPDIF_DEBUG_SIG,

    ODSP_PREPARE_SIG,
    ODSP_PLAY_SIG,
    ODSP_PAUSE_SIG,
    ODSP_CONTINUE_SIG,
    ODSP_FASTFORWARD_SIG,
    ODSP_STOP_SIG,
    ODSP_SEEK_SIG,
    ODSP_FINISH_FILL_SIG,    /* player report the result of data filling  */
    ODSP_SET_PARAM_SIG,      /* set parameter */
    ODSP_GET_PARAM_SIG,      /* get parameter */
    ODSP_SET_APPFILTER_SIG,      /* set parameter */

    ODSP_REPORT_CNT_SIG,     /* ODSP report count */
    ODSP_PREPARE_END_SIG,    /* ODSP report ready to play */
    ODSP_OVER_SIG,           /* ODSP report STOP */
    ODSP_FILL_SIG,           /* ODSP request player to fill up ring buffer */
    ODSP_GET_PARAM_FEEDBACK_SIG, /* ODSP report get parameter */
    ODSP_CODEC_UNSUPPORT_SIG, /* ODSP report unsupport */
    ODSP_STATE_CHANGE,
    ODSP_SET_MEDIA_SIG,

    ODSP_PRV_DATA_READ_SIG,  /* ODSP private */
    ODSP_PRV_DATA_IDLE_SIG,  /* ODSP private */
    ODSP_PRV_NEXT_SIG,       /* ODSP private */
    ODSP_PRV_TIME_SIG,       /* ODSP private */
    ODSP_PRV_POLLING_SIG,    /* ODSP private */
    ODSP_PRV_CONS_SIG,       /* ODSP private */
    ODSP_PRV_REST_SIG,       /* ODSP private */
    ODSP_PRV_PLAY_SIG,       /* ODSP private */
    ODSP_PRV_STOP_SIG,
    ODSP_CLI_SIG,            /* ODSP private for test */
    ODSP_TERMINATE_SIG,
    ODSP_PROGRESS_SIG,       /* ODSP report progress */
    ODSP_PRI_SIG_MAX,       // It is the last one in this group
    ODSP_PREPARE_VA,        /* VA prepare sig */
    ODSP_SET_VA_SIG,        /* Set va param sig */
    ODSP_SMART_MODE_SIG,    /* Set SmartMode sig */
    ODSP_SET_APPMODE_SIG,
    ODSP_RESET_SIG,

    //Add HDSP sig
	HDSP_LOADCODE_SIG,
	HDSP_CONFIG_SIG,
	HDSP_PARAM_SIG,
	HDSP_PLAY_SIG,
	HDSP_STOP_SIG,
	HDSP_RESET_SIG,
	HDSP_POLLING_SIG,
	HDSP_READY_SIG,
	HDSP_SET_PARAM_SIG,
	HDSP_GET_PARAM_SIG,
	HDSP_GET_FORMAT_SIG,
	HDSP_POWER_ON_SIG,
	HDSP_POWER_OFF_SIG,
	HDSP_TERMINATE_SIG,
	HDSP_PRI_SIG_MAX,

    /*..........................................................................*/


    /*.................................audioplayer private signal.........................................*/
    MFP_TIMEOUT_SIG = MFP_PRI_GRP_START,			/* used by player for time events */
    MFP_NOTIFY_SIG,
    MFP_STATE_SIG,
    MFP_TERMINATE_SIG,			/* used by BSP to terminate the application */
    MFP_PLAYBACK_ERR_SIG,		/* used by player to user for playback error */
    MFP_SWITCHTO_PLAY_SIG,      /* used by player to user for playback error */
    MFP_SWITCHTO_STOP_SIG,      /* used by player to user for playback error */
    MFP_SOURCE_OUT_SIG,
    MFP_SOURCE_IN_SIG,
    MFP_WAIT_SIG,
    MFP_SWITCH_NEXT_SIG,
    MFP_TRICKYTO_PLAY_SIG,
    MFP_STOPTO_PLAY_SIG,
    MFP_AUTO_PLAY_SIG,
    MFP_DEVICE_ERR_SIG,
    SHOW_PLAYING_TIME,
    SHOW_TRICKY_TIME,
    MFP_USBIF_SOURCE_IN_SIG,
    MFP_USB_STOP_SIG,

    MFP_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.............................BT.............................................*/
    BT_BASE_SIG = BT_PRI_GRP_START,//main and sub
    BT_COMM_SIG,//for role 8107main:headphoe or sub ;
    BT_SERVICE_CBK_SIG,
    BT_TRANSMIT_SIG,
    BT_TRANSMIT_CBK_SIG,
    BT_APP_SIG,
    BT_TIME_CHECK_SIG,
    BT_TIME_DELAY_SIG,
    BT_AO_AUDIO_SIG,
    BT_AO_SERVICE_MODULE_SIG,

    BT_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................net ao private signal.....................................*/
    NET_INIT_SIG = NET_PRI_GRP_START,
    NET_IDLE_SIG,
    NET_ACTIVE_SIG,

    NET_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................dmr service private signal.........................................*/
    DMR_SRV_CREATE = DMR_PRI_GRP_START,
	DMR_SRV_DESTROY,
	DMR_SRV_DETACH,
	DMR_SRV_SET_URL,
	DMR_SRV_PREPARE,
	DMR_SRV_PLAY,
	DMR_SRV_PAUSE,
	DMR_SRV_STOP,
	DMR_SRV_SEEK,
	DMR_SRV_MUTE_SET,
	DMR_SRV_VOL_SET,
	DMR_SRV_TMR,

    DMR_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.................................airplay ao private signal.........................................*/
    AIRPLAY_SRV_CREATE = AIRPLAY_PRI_GRP_START,
    AIRPLAY_SRV_DESTROY,
    AIRPLAY_SRV_PREPARE,
    AIRPLAY_SRV_pLAY,
    AIRPLAY_SRV_PAUSE,
    AIRPLAY_SRV_STOP,
    AIRPLAY_SRV_SEEK,
    AIRPLAY_SRV_MUTE,
    AIRPLAY_SRV_DEMUTE,
    AIRPLAY_SRV_VOL_SET,
    AIRPLAY_SRV_MUTE_SET,
    AIRPLAY_SRV__TMR,

    AIRPLAY_SRV_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/


    /*.............................new module.............................................*/
// add new private signal group below!!{

#if 0 // exemple
    /*.................................exemple ao private signal.........................................*/
    EXEMPLE_PRI_A_SIG = EXEMPLE_PRI_GRP_START,
    EXEMPLE_PRI_B_SIG,
    EXEMPLE_PRI_C_SIG,
    EXEMPLE_PRI_D_SIG,
    //........

    EXEMPLE_PRI_SIG_MAX, // It is the last one in this group
    /*..........................................................................*/
#endif
/*.............................. HDMI SRV signal .........................................*/

	HDMI_KEYPOLLING_SIG = HDMI_PRI_GRP_START,
	HDMI_PRI_CEC_SCAN_SIG,
	HDMI_PRI_CEC_INIT_SIG,
	HDMI_PRI_CEC_TERMINATE_SIG,
	HDMI_PRI_CEC_SET_SAC_SIG,
	HDMI_PRI_CEC_SET_SRC_SIG,
	HDMI_PRI_CEC_EVENT_SIG,
	HDMI_PRI_HDMI_TX_INIT_SIG,
	HDMI_PRI_HDMI_TX_TERMINATE_SIG,
	HDMI_PRI_HDMI_PLUG_IN_SIG,
	HDMI_PRI_HDMI_PLUG_OUT_SIG,
	HDMI_CEC_HPD_BROADCAST_REQ_SIG,
	HDMI_CEC_Standby_SIG,
	HDMI_CEC_ARCSOURCE_SIG,
	HDMI_CEC_SETDSP_SIG,


	HDMI_PRI_SIG_MAX, // It is the last one in this group

/*.................................some new signal.........................................*/
	FL_UMOUNT_STOP_SCAN_SIG,
	MFP_BREAK_PLAY_SIG,

	/*.................................voice assist service signal.........................................*/
	VA_STATUS_SIG,
	VA_TIME_BGM_MUTE_SIG,
	VA_TIME_BGM_RESUME_SIG,
	VA_TIME_BT_RESUME_SIG,
	VA_TIME_WAIT_ENERGY_SIG,
	VA_TIME_POLLING_SIG,
	VA_SOURCE_SIG,
	VA_STATUS_RESET_SIG,
	VA_SET_AUX_MUTE_SIG,
	VA_SET_AUX_RESUME_SIG,
	VA_SET_BT_MUTE_SIG,
	VA_SET_BT_RESUME_SIG,

	AUXVA_ENERGY_ENABLE,
	AUXVA_ENERGY_DISABLE,
	BTVA_ENERGY_ENABLE,
	BTVA_ENERGY_DISABLE,
	VA_ENERGY_ENABLE,

	/*.................................hint sound qp signal.........................................*/
	HINT_END_SIG,
	HINT_NEXT_SIG,

// add new private signal group above!!}

    /*..........................................................................*/
    SDK_PRI_SIG_MAX,// It is the last one of SDK private signal group

    /*.............................customer signal.............................................*/
    //DO NOT add any signal here, customer signals sould put in custom_sig.h
    //these signals are reserved for customer, and must put in the last of SDK signals

    CUSTOMER_PRIVATE_SIG_START = CUSTOMER_PRI_SIG_START,
    CUSTOMER_PRIVATE_SIG_END = CUSTOMER_PRIVATE_SIG_START + CUSTOMER_PRIVATE_SIG_NUM,
    /*..........................................................................*/

    MAX_SIG,
};

#ifdef Q_SPY
/**
*\brief application specific trace records
*/
enum QSAPPREC
{
	/* QS_USER is for print */
	QS_EXAMPLE_PALYER = QS_USER + 1,
};
#endif

/**
*\brief AO priority, it must between 1 and 30
*/
enum AOPRIO
{
	/*!< low priority */
	PRIO_EXAMPLE_PALYER = 5,
	PRIO_EXAMPLE_MEDIASCANNER,
	PRIO_EXAMPLE_POWER_TESTCODE,
	PRIO_EXAMPLE_POWER_TEST2,
	/*!< mudium priority */
	PRIO_FILELIST,
	PRIO_MEDIASCANNER,
	PRIO_NET_AO,

	PRIO_AUDIOCONTROL,
	PRIO_AUDIO_PLAYER,
	PRIO_USERAPP,
	PRIO_BT_AO,

	/*!< high priority */
	PRIO_AUDIODSP,
	PRIO_IOPSCANNER,
	PRIO_SYSTEMSERVICE,
	PRIO_UPGRADEPIPE,

};

/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

/******************************************************************************************/
/**
 * \fn		void QP_Init()
 *
 * \brief		do QP initial things
 *
 * \param  	none.
 *
 * \return		nine.
 *
 ******************************************************************************************/
void QP_Init();

/******************************************************************************************/
/**
 * \fn		void QP_Terminate(int16_t wResult)
 *
 * \brief		terminate QP
 *
 * \param  	result : (Input) result value
 *
 * \return		nine.
 *
 ******************************************************************************************/
void QP_Terminate(int16_t wResult);

#endif
