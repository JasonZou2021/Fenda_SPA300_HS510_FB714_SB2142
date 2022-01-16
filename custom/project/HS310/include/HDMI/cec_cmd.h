// *****************************************************************************
// *    CEC process                                                           *
// *    Create by ECL                                                         *
// *    2007/10/01                                                            *
// *****************************************************************************
#ifndef CEC_CMD_H
#define CEC_CMD_H

#define CEC_CMD_NONE 				0xff
#define CEC_MAX_PING_SELF_TIMES     2
#define CEC_REQ_SAC_TIMEOUT			100//unit:10ms
#define CEC_REQ_ARC_TIMEOUT			200//unit:10ms

// CEC command
typedef enum _eCecCommandType
{
	CEC_CMD_IDLE = 0,
	CEC_CMD_ACTIVE_SOURCE,
	CEC_CMD_INACTIVE_SOURCE,
	CEC_CMD_PING_AUD_SYS,
	CEC_CMD_PING_PLAYBACK,
	CEC_CMD_ONE_TOUCH_PLAY,
	CEC_CMD_POWER_ON,
	CEC_CMD_POWER_OFF,
	CEC_CMD_REPORT_POWER_STATUS,//10
	CEC_CMD_REPORT_PHYSICAL_ADDRESS,
	CEC_CMD_FEATURE_ABORT,
	CEC_CMD_PING_AND_REPORT_AUD_SYS, //audio system
	CEC_CMD_PING_AND_REPORT_PLAYBACK, //playback device
	CEC_CMD_CEC_VERSION,
	CEC_CMD_SET_OSD_NAME,
	CEC_CMD_DEVICE_VENDOR_ID,//20
	CEC_CMD_GET_TV_POWER_STATUS,
	CEC_CMD_REQUEST_ACTIVE_SOURCE,//25
	CEC_CMD_SET_SYS_AUDIO_MODE,
	CEC_CMD_REPORT_AUDIO_STATUS,
	CEC_CMD_REPORT_SYS_AUDIO_MODE_STATUS,
	CEC_CMD_REPORT_SHORT_AUDIO_DESCRIPTOR,
	CEC_CMD_INITIATE_ARC,
	CEC_CMD_TERMINATE_ARC,
	CEC_CMD_GIVE_PHYSICAL_ADDRESS,
	CEC_CMD_GIVE_OSD_NAME,
	//CEC_CMD_NONE = 0xff, //spike 20080703 it is used for set last cmd none because ARB //20080816 now no use
} eCecCommandType;

enum CEC_COMMAND_PROCESS_STEP
{
	PROCESS_STEP1 = 1,
	PROCESS_STEP2,
	PROCESS_STEP3,
	PROCESS_STEP4,
	PROCESS_STEP5,
	PROCESS_STEP6,
	PROCESS_STEP7,
	PROCESS_STEP8,
	PROCESS_STEP9,
	PROCESS_STEP10,
};

enum CEC_COMMAND_SEQUENCE
{
	CEC_FIRST_CMD = 0,
	CEC_SECOND_CMD,
	CEC_THIRD_CMD,
};

void CMD_OneTouchPlay(stCecHandler *pstHandler);
void CMD_ActiveSource(stCecHandler *pstHandler);
void CMD_InactiveSource(stCecHandler *pstHandler);
void CMD_SetSysAudMode(stCecHandler *pstHandler);
void CMD_ReportAudStatus(stCecHandler *pstHandler);
void CMD_ReportSysAudModeStatus(stCecHandler *pstHandler);
void CMD_InitiateArc(stCecHandler *pstHandler);
void CMD_TerminateArc(stCecHandler *pstHandler);
void CMD_ReportPhyAddr(stCecHandler *pstHandler);
void CMD_SetOsdName(stCecHandler *pstHandler);
void CMD_RequestActiveSrc(stCecHandler *pstHandler);
void CMD_DevVendorId(stCecHandler *pstHandler);
void CMD_FeatureAbort(stCecHandler *pstHandler);
void CMD_UserCtrlPowerOnTv(stCecHandler *pstHandler);
void CMD_ReportPwrStatus(stCecHandler *pstHandler);
void CMD_CecVersion(stCecHandler *pstHandler);
void CMD_GetTvPwrStatus(stCecHandler *pstHandler);
void CMD_ReportShortAudDescriptor(stCecHandler *pstHandler);
void CMD_PingAudSys(stCecHandler *pstHandler);
void CMD_PingPlayback(stCecHandler *pstHandler);
void CMD_PingAndReportAudSys(stCecHandler *pstHandler);
void CMD_PingAndReportPlayback(stCecHandler *pstHandler);

#endif

