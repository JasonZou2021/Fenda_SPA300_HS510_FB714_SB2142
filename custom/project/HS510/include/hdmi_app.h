#ifndef __HDMI_H__
#define __HDMI_H__


#include "types.h"
#include "qp_pub.h"

#define HDMI_POLL_TIME					2  // 150ms
#define HDMI_STANDBY_POLL_TIME		1  // 100ms
#define VOL_TIMEOUT_CNT				8  // 500ms

#define HDMI_STANDBY_SYS_AUDIO_TIME		20  // 2s

typedef struct _hdmi_func
{
	int (*cfg)(BYTE, BYTE,BYTE, BYTE);
	int (*init)(BYTE, BYTE);
	int (*status)(BYTE);
	int (*handle)(BYTE , BYTE, BYTE);
	int (*upg)(void);
	int (*check_eARC)(void);
	int (*check_ADOType)(void);
	int (*ver)(void);
	
}HDMI_FUNC;

extern int hdmi_cfg(BYTE mode, BYTE src, BYTE vol, BYTE volmax);
extern int hdmi_init(BYTE mode, BYTE src);
extern int hdmi_get_status(BYTE mode);
extern int hdmi_handle(BYTE cmd, BYTE para1, BYTE para2);
extern int hdmi_upg(void);
extern int hdmi_CheckEARC(void);
extern int hdmi_CheckADOType(void);
extern int hdmi_GetVersion(void);

extern int Hdmi_SetHandleSig(QActive *send, int cmd, int para1, int para2);
extern int Hdmi_SetInitSig(QActive *send, int src, int vol, int volmax);
extern int Hdmi_SetUpgSig(QActive *send);

extern HDMI_FUNC hdmi_rep_func;
extern HDMI_FUNC ite_rep;

#define CMD_ARC_SYS_CLOSE	(0x53)
#define CMD_POWER_STANDBY	(0x50)
#define CMD_PASSTHROUGH		(0x88)
#define CMD_POWER_ON		(0x48)
#define CMD_POWER_OFF		(0xC8)
#define CMD_DUMP_I2C		(0xE0)
//#define CMD_CEC_CTL			(0x30)
#define CMD_VOLUME_MUTE		(0x58)
#define CMD_VOLUME_UP		(0xD8)
#define CMD_VOLUME_DOWN		(0x32)
#define CMD_MODE_HDMI_1		(0xC2)
#define CMD_MODE_HDMI_2		(0xD0)
#define CMD_MODE_ARC		(0x22)
#define CMD_MODE_BT			(0x3A)
#define CMD_ADO_SYS			(0x28)
#define CMD_EARC			(0xA8)
#define CMD_SB_CEC			(0x68)
#define CMD_TX_AUDIO		(0x18)
#define CMD_TX_OUTPUT		(0xE8)
#define CMD_HDCP_RPT		(0xB8)

#define SB_VOL_MAX		(31)
#define SB_VOL_DEFAULT	(10)

enum
{
	HDMI_SYS_POWER_ON,
	HDMI_SYS_STANDBY,	
};	

enum
{
	HDMI_WAKEUP_HDMI_1 = 0x1234,	
	HDMI_WAKEUP_HDMI_2,
	HDMI_WAKEUP_ARC,
};

enum
{
	ADO_TYPE_LPCM,
	ADO_TYPE_NLPCM,	
	ADO_TYPE_HBR,
	ADO_TYPE_DSD,
};


#define HDMI_UPG_OK	0XFF
#define HDMI_UPG_NG	0XFE

typedef struct 
{
	QEvt stEvt;
	BYTE vol;
	BYTE mute;
} HdmiEvt;


typedef struct 
{
	QEvt stEvt;
	BYTE para1[3];
} HdmiCtlEvt;

#endif

