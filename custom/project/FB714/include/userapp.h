/*${.::UserApp.h} ..............................................................*/

#ifndef UserApp_h
#define UserApp_h

//=============macro for function config==============

//=============macro for function config==============

#include "custom_sig.h"
#include "iop_scanner.h"
#include "systemservice.h"
#include "power.h"
#include "user_def.h"
#include "sdk_cfg.h"


//#define FENDA_TUNNING_TEST


#define USERAPP_VOL_MAX (31)
#define USERAPP_VOL_MIN (0)

//#define AP_DBG
#define ap_printf0(f, a...) LOGX(f, ##a)
#define ap_printf(f, a...) LOGD(f, ##a)
#define ap_printf2(f, a...) LOGDT(f, ##a)
#define ap_printf_E(f, a...) LOGE(f, ##a)

#define ap_BurnTest_log(f, s...)  do{                                                 \
		if(me->stAutoSrcTest->burn_log_enable == 1)                               \
			LOGD("[TEST][%s][%d]"f, __FUNCTION__, __LINE__, ##s);\
		else                                                \
			((void)0);                                      \
	}while(0)
#ifndef SDK_RELEASE
#define ap_Memory_check()   do{                             \
		if(me->Memory_Check == 1) \
			UserAppMemoryCheck();       \
		else                       \
			((void)0);                  \
	}while(0)
#endif

//SUNPLUS support all the sources as below
// DO NOT remove any member of it
//if you want to adjust the order of sources or remove some of them, adjust the gUserSrc_tbl in userapp
enum UserAppSrc
{
	//jeff if no switch source error ,please delete// eAp_UserappSrc_start,

	eAp_USB_Src,

	eAp_CARD_Src,

#if (!SUPPORT_VA)
	eAp_AUX_Src,
#endif

	eAp_LINE_Src,

	eAp_I2S_Src,
	
#if 1 //def SB714
	eAp_USC_Src,
#endif
	
	eAp_SPDIF_Optical_Src,

	eAp_SPDIF_Coaxial_Src,

	eAp_ARC_Src,

	eAp_HDMI0_Src,

	eAp_HDMI1_Src,

	eAp_HDMI2_Src,

#ifdef USERAPP_SUPPORT_BT
	eAp_Bt_Src,
#endif

#ifdef USERAPP_SUPPORT_HDMI_TX
	eAp_HDMI_TX_Src,
#endif

	eAp_UserappSrc_end,

	eApUserAppSrc_Num,
};

typedef enum
{

	eUser_ARCSRC = 0,
    eUser_eARCSRC,

}eUser_EARCSrc_t;

typedef enum
{
	eRes_err = -1,
	eRes_success = 0,
	eRes_specific1 = 1,
} eRes_t;

typedef enum {
	Source_ARC = 1,
	Source_HDMISTD = 2,
	Source_HDMIHBR = 3,
}eSoundID_t;



/*${Events::UserAppEvt} ......................................................*/
typedef struct
{
	/* protected: */
	QEvt super;

	/* public: */
	//uint8_t scan;
} UserAppEvt;

typedef struct
{
	QEvt super;
	QActive *pstSender;
	UPGRETURN_T *pUPGRETURN_T;
} UserAppUpgradeEvt;


typedef struct
{
	QEvt super;
	UINT8 User_KeyCode;
	UINT8 User_KeyCodeSrc;
	stKeyInfo_t *pstKeyInfo_t;
} UserAppKeyEvt;



/*${User Key structure} ......................................................*/
typedef struct
{
	UINT8 UserKeyCode;
	UINT8 UserKeyCodeSrc;
	stKeyInfo_t stKeyInfo;
} stUserKeyInfo_t;

typedef struct
{
	int  switch_src_time;
	int  power_off_time;
	int  power_on_time;
	int  burn_log_enable;
} AutoSrcTest_t;


typedef struct
{
	int  private_signal_number;
	int  private_signal_repeat_time;
} AutoRepeatSignalTest_t;

enum mutestate
{
	eMute_off = 0,
	eMute_on,

};

typedef enum
{
	BT_CONNECT_READY,
	BT_CONNECT_SUCCESS,
} BtHintType_e;

typedef enum
{
	VA_OFF = 0,
	VA_ON,
} eVAStatus_t;


typedef enum
{
	eInput_2p0_ch = 0,
	eInput_5p1_ch,
	eInput_7p1_ch,
	eInput_5p1p2_ch,
	eInput_5p1p4_ch,
	eInput_7p1p2_ch,
	eInput_7p1p4_ch,
}eInputChannelNum;

typedef enum
{
	APP_SRC_ARC,
	APP_SRC_HDMI1,
	APP_SRC_BT,	
	APP_SRC_OPT,
	APP_SRC_AUX,
	APP_SRC_USB,
#if 1 //def SB714
	APP_SRC_USC,
#endif	
//
	APP_SRC_MAX,
	APP_SRC_HDMI2,
	APP_SRC_COAX,
}eAPP_SRC;

typedef enum
{
	eSoundTreble,		//高音
	eSoundFront,		//前左，前右通道
	eSoundCenter,		//中置
	eSoundLsRs,			//RSL RSR(后左右环�?
#if 1 //def SB714	
	eSoundFTLR,			//FTL FTR(前左顶，前右�?
	eSoundFSLR,			//FSL FSR(前左环绕，前右环�?
	eSoundRTLR,			//RTL RTR(后左顶，后右�?			
#endif	
	eSoundMax,
}eSoundMenu;


/*${AOs::UserApp_ctor} .......................................................*/
void UserApp_ctor(void);
int SendDataBySerial(UINT8 *data ,UINT8 cmd,UINT16 datalength);
int Userapp_Get_System_Memory_Source(void);
int Userapp_Set_Source_id(UINT8 source_id);
UINT8 Userapp_Get_Source_id();
int UserAppNum2Str(char *strBuf, int num);



/*${AOs::MediaScanner_ctor} .......................................................*/
//void IopScanner_ctor(void);


QActive *UserApp_get(void);

//===== daniel For Trace=====
#define DANIEL_SCAN_PRINTF
#ifdef DANIEL_SCAN_PRINTF
	#define GREEN "\033[0;32;32m"
	#define RED "\033[0;32;31m"
	#define BLUE "\033[0;32;34m"
	#define YELLOW "\033[1;33m"
	#define NONE_COLOR "\033[m"
	#define daniel_G_printf(fmt, arg...) ap_printf( GREEN "[daniel][%d]"fmt NONE_COLOR ,__LINE__,##arg)
	#define daniel_R_printf(fmt, arg...) ap_printf( RED "[daniel][%d]"fmt NONE_COLOR ,__LINE__,##arg)
	#define daniel_B_printf(fmt, arg...) ap_printf( BLUE "[daniel][%d]"fmt NONE_COLOR ,__LINE__,##arg)
	#define daniel_Y_printf(fmt, arg...) ap_printf( YELLOW "[daniel][%d]"fmt NONE_COLOR ,__LINE__,##arg)
#endif
//===== daniel For Trace=====

#endif /* UserApp_h */
