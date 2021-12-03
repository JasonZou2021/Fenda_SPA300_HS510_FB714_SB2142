/*${.::Display_Srv.h} ..............................................................*/

#ifndef _Display_Srv_h

#define _Display_Srv_h

#include "qp_pub.h"
#include "VFD.h"

#define VFD_MAX_NUM (9999)

typedef enum
{
	LEFT = 0,
	RIGHT = 1,
	CENTER = 2,
	NONE = 3,
} eStringAlignType_t;

typedef enum
{
	R2L = 0,
	L2R,

} eScrollmode_t;

typedef enum  // back mode  0: const  1: blink  2:scroll
{
	BACK_CONST = 0,
	BACK_BLINK,
	BACK_SCROLL,
	BACK_USER,

} eShowOncemode_t;

typedef enum
{
	LED_SHOW_USB = 0,
	LED_SHOW_CARD,
	LED_SHOW_AUXIN,
	LED_SHOW_LINEIN,
	LED_SHOW_SPDIFIN2,
	LED_SHOW_SPDIFIN3,
	LED_SHOW_COAXIAL,
	LED_SHOW_OPTICAL,
	LED_SHOW_ARC,
	LED_SHOW_EARC,
	LED_SHOW_DLNA,
	LED_SHOW_BT,
	LED_SHOW_HDMI,
	LED_SHOW_I2SIN,
	LED_SHOW_GOODBYE,
	LED_SHOW_RAWDATA,
	LED_SHOW_UNSUPPORT,

	LED_SHOW_PLAY,
	LED_SHOW_PAUSE,
	LED_SHOW_STOP,
	LED_SHOW_NEXT,
	LED_SHOW_PREV,
	LED_SHOW_ERR,
	LED_SHOW_OK,
	LED_SHOW_DSPBIN,
	LED_SHOW_ALLBIN,
	LED_SHOW_APPBIN,
	LED_SHOW_NOBIN,
	LED_SHOW_NOUSB,
	LED_SHOW_NOCARD,
	LED_SHOW_NONE,
	LED_SHOW_ALL,
	LED_SHOW_ONE,
	LED_SHOW_DIR,
	LED_SHOW_OFF,
	LED_SHOW_ON,
	LED_SHOW_LOAD,

	LED_SHOW_UPGRADE,
	LED_SHOW_EQ0,
	LED_SHOW_EQ1,
	LED_SHOW_EQ2,
	LED_SHOW_EQ3,
	LED_SHOW_SU0,
	LED_SHOW_SU1,
	LED_SHOW_SU2,
	LED_SHOW_SU3,
	LED_SHOW_EAR,
	LED_SHOW_SUB,
	LED_SHOW_IR,
	LED_SHOW_HDMI0,
	LED_SHOW_HDMI1,
	LED_SHOW_HDMI2,
	LED_SHOW_NULL,
#ifdef BUILT4ISP2
	LED_SHOW_ISPBOOOTBIN,
#endif
	LED_SHOW_ATMOS_UPMIX,
	LED_SHOW_ATMOS_TUNING,
	LED_SHOW_ATMOS_XML,
	LED_SHOW_ATMOS_VIRT,
	LED_SHOW_ATMOS_HFT,
	LED_SHOW_ATMOS_CNTSPD,
	LED_SHOW_ATMOS_VLAMP,
	LED_SHOW_ATMOS_LM,
	LED_SHOW_ATMOS_DIRECT,
	LED_SHOW_ATMOS_POST,
	LED_SHOW_ATMOS_DRC,
	LED_SHOW_ATMOS_DRC_OFF,
	LED_SHOW_ATMOS_DRC_ON,
	LED_SHOW_ATMOS_DRC_AUTO,
	LED_SHOW_ATMOS_PROFILE_OFF,
	LED_SHOW_ATMOS_PROFILE_MOVIE,
	LED_SHOW_ATMOS_PROFILE_MUSIC,
	LED_SHOW_ATMOS_PROFILE_NIGHT,
	LED_SHOW_ATMOS_PROFILE_DYNAMIC,
	LED_SHOW_ATMOS_PROFILE_VOICE,
	LED_SHOW_ATMOS_PROFILE_GAME,
	LED_SHOW_ATMOS_PROFILE_PERSONALIZE,
	LED_SHOW_ATMOS_PROFILE_CUSTO_A,
	LED_SHOW_ATMOS_PROFILE_CUSTO_B,
	LED_SHOW_ATMOS_PROFILE_CUSTO_C,
	LED_SHOW_ATMOS_PROFILE_CUSTO_D,
} LED_SHOW_ID;


//************Service for VFD Display ******************//
int VFD_Service_Display(void   *sender, char *string );
void VFD_Service_Constant(void   *sender);
void VFD_Service_Align(void   *sender, eStringAlignType_t Align_Type);
void VFD_Service_Scroll (void   *sender, eScrollmode_t scroll_mode);
void VFD_Service_Blink (void   *sender, uint16_t bright_time, uint16_t dark_time);
void VFD_Service_ScrollOne_Constant (void *  sender  , char *Const_vfd, eScrollmode_t scroll_mode, eShowOncemode_t is_blink, uint16_t bright_time, uint16_t dark_time);
void VFD_Service_BlinkOne_Constant(void *  sender  , char *Const_vfd, BYTE  blink_time, eShowOncemode_t is_blink_or_scroll,uint16_t once_bright_time ,uint16_t once_dark_time, uint16_t bright_time, uint16_t dark_time);
int VFD_Service_POP (void   *sender,  char *string, int time, eStringAlignType_t Align_Type);
void VFD_Service_Play_Time(void   *sender, int sec);
void VFD_Service_Show_Dark(void   *sender);
int VFD_DotCtrl(void *pSender, UINT8 Dot, UINT8 Op);

//************Service for LED Display ******************//

int LED_Service_Display(void   *sender, LED_SHOW_ID led_id );
void LED_Service_Show_Dark(void   *sender);
void LED_Service_Constant(void   *sender);
void LED_Service_Blink (void   *sender, uint16_t bright_time, uint16_t dark_time);


/*${AOs::DisplaySrv_ctor} .......................................................*/
void DisplaySrv_ctor(void);
QActive *DisplaySrv_get(void);

#endif
