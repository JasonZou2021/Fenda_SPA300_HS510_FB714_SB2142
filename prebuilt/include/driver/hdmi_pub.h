// *****************************************************************************
// *    HDMI TX process                                                           *
// *    Create by Leon                                                         *
// *    2018/1/2                                                            *
// *****************************************************************************
#ifndef HDMI_PUB_H
#define HDMI_PUB_H
//#include "hdmitx.h"


// ===================================================================
// Variable interface
// ===================================================================
enum
{
	eHDMI_DVI_mode = 0,
	eHDMI_HDMI_mode = 1,
	eHDMI_mode_max,
};

enum//tv resolution
{
	eHDMI_TV_RES_480p = 0,
	eHDMI_TV_RES_576p = 1,
	eHDMI_TV_RES_both = 2,//480p/576p
	eHDMI_TV_RES_max,
};

// ===================================================================
// Function interface
// ===================================================================
typedef void (*hdmiop_hdmi_480p)( void);
typedef void (*hdmiop_dvi_480p)( void);
typedef void (*hdmiop_hdmi_576p)(void);
typedef void (*hdmiop_dvi_576p)(void);
typedef void (*hdmiop_hdmi_disable)(void);
typedef int (*hdmiop_hdmi_mode)(BYTE *pEdidData);
typedef int (*hdmiop_tv_resolution)(BYTE *pEdidData);

typedef struct _HDMIOP_S
{
	hdmiop_hdmi_480p         pfn_hdmi_480p;
	hdmiop_dvi_480p          pfn_dvi_480p;
	hdmiop_hdmi_576p         pfn_hdmi_576p;
	hdmiop_dvi_576p          pfn_dvi_576p;
	hdmiop_hdmi_disable      pfn_hdmi_disable;
	hdmiop_hdmi_mode         pfn_hdmi_mode;
	hdmiop_tv_resolution     pfn_tv_resolution;
} HDMI_OP_T;

HDMI_OP_T *HDMITX_Power_ON(void);
HDMI_OP_T *HDMITX_Power_OFF(void);
void HDMI_embedded_API(void);

#endif

