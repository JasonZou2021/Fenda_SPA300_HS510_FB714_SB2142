
#ifndef _VFD_Srv
#include "qp_pub.h"
#include "Display_Srv.h"

#define _VFD_Srv

#define VFD_STR_MAX	33

typedef struct
{
	QEvt stEvt;
} vfdEvt;

//event for signal DISPLAY_VFD_SHOWDARK_SIG
typedef struct
{
	vfdEvt stType;

} VfdShowDarkEvt;

//event for signal DISPLAY_VFD_DISPLAY_SIG
typedef struct
{
	vfdEvt stType;
	char *Svfd_src;//char Svfd_src[14];
	int Ssrc_len;

} VfdDisplayEvt;
//event for signal DISPLAY_VFD_ALIGN_SIG
typedef struct
{
	vfdEvt stType;
	int SAlign_Type;

} VfdAlignEvt;
//event for signal DISPLAY_VFD_CONST_SIG

typedef struct
{
	vfdEvt stType;

} VfdConstantEvt;
//event for signal DISPLAY_VFD_SCOLL_SIG
typedef struct
{
	vfdEvt stType;
	int Sscroll_mode;
	int Sstr_offset;

	eShowOncemode_t scrollover_back;
	uint16_t bright_time;
    	uint16_t dark_time;
	char* const_vfd;	
	
} VfdScrollEvt;
//event for signal DISPLAY_VFD_BLINK_SIG

typedef struct
{
	vfdEvt stType;
	uint16_t active;
	
} VfdMuteBlinkEvt;

typedef struct
{
	vfdEvt stType;
		BYTE blink_time;
		uint16_t once_bright_time;
		uint16_t once_dark_time;
	uint16_t Sbright_time;
    uint16_t Sdark_time;
	eShowOncemode_t blinkover_back;
	char* const_vfd;
} VfdBlinkEvt;

//event for signal DISPLAY_VFD_POP_SIG

typedef struct
{
	vfdEvt stType;
	char *Svfd_popsrc; //char Svfd_popsrc[14];
	int Stime;
	int SAlign_Type;
} VfdPopEvt;


//event for signal DISPLAY_VFD_Play_Time_SIG

typedef struct
{
	vfdEvt stType;
	int Ssecond;
	int Sminute;
	int Shour;
} VfdPlayTimeEvt;

typedef struct
{
	vfdEvt stType;
	int DotType;
	int Op;
} VfdDotCtrlEvt_t;

enum vfdshowtype
{
	eVfd_null = 0,
	eVfd_constant = 1,
	eVfd_playtime = 2,
	eVfd_scroll = 3,
	eVfd_blink = 4,
	eVfd_pop = 5,
	eVfd_scrollone_const = 6,
	eVfd_blinkone_const = 7,
	eVfd_max,

};

typedef struct
{
	char vfd_src[VFD_STR_MAX];
	char vfd_align_src[VFD_STR_MAX];
	int src_len;
	int Align_Len ;
	uint16_t blink_once_bright_time;
    uint16_t blink_once_dark_time;
	uint8_t blink_time;
	uint16_t bright_time;
	uint16_t dark_time;
	int scrolling_mode;
	int str_offset;
	int Align_type;
	int time;
	char *once_back_const_vfd;
	eShowOncemode_t once_back_mode; //back mode  0: const  1: blink  2:srcoll

} stinfo_t;



/*VFD Display..........................................................................*/
typedef struct       /* the VFD structure */
{
	QHsm super;
	uint8_t count; /* show counter */
	stinfo_t *showinfo; //determine source and type information
	stinfo_t *popshowinfo; //determine pop source information
	int show_type;/* show type:blink , scroll, constant , pop */
	char PlayTime[14];
	QStateHandler vfd_history; //use to get last vfd state
} VfdDisplay;

void VfdDisplay_ctor(VfdDisplay *const me);
void VFD_Display(VfdDisplay *const me, char *vfd_src, int src_len);
int VFD_Scroll (VfdDisplay *const me, int scroll_mode, int str_offset);
int VFD_Scroll_One_Const (VfdDisplay * const me  , int scroll_mode , int str_offset, char *const_vfd, eShowOncemode_t back_mode, uint16_t bright_time ,uint16_t dark_time);
int VFD_Blink_One_Const (VfdDisplay * const me  ,char *const_vfd, eShowOncemode_t back_mode,BYTE blink_time, uint16_t once_bright_time ,uint16_t once_dark_time, uint16_t bright_time ,uint16_t dark_time);
void VFD_Blink (VfdDisplay *const me, uint16_t bright_time, uint16_t dark_time);
int VFD_Align(VfdDisplay *const me, int Align_Type);
int VFD_POP (VfdDisplay *const me, char *vfd_src, int time, int Align_Type);
int VFD_SCROLLING(VfdDisplay *const me);
int VFD_SCROLLING_ONCE(VfdDisplay * const me);
int VFD_POP_Align(VfdDisplay *const me, int Align_Type);
void VFD_PlayTime(VfdDisplay *const me, int min, int sec, int hr);
void VFD_SHOW_STRING(char *string);
void VFD_SHOW_DARK(void);

int VFD_DotCtrl(void *pSender, UINT8 Dot, UINT8 Op);

#endif

