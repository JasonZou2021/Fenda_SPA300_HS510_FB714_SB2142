// *****************************************************************************
// *    CEC process                                                           *
// *    Create by ECL                                                         *
// *    2007/10/01                                                            *
// *****************************************************************************
#ifndef CEC_OP_H
#define CEC_OP_H

// CEC opcode
#define CEC_OPCODE_FEATURE_ABORT                                    (0x00)
#define CEC_OPCODE_IMAGE_VIEW_ON                                    (0x04)
#define CEC_OPCODE_TEXT_VIEW_ON                                     (0x0d)
#define CEC_OPCODE_GIVE_DECK_STATUS                                 (0x1a)
#define CEC_OPCODE_DECK_STATUS                                      (0x1b)
#define CEC_OPCODE_SET_MENU_LANGUAGE                                (0x32)
#define CEC_OPCODE_STANDBY                                          (0x36)
#define CEC_OPCODE_PLAY                                             (0x41)
#define CEC_OPCODE_DECK_CONTROL                                     (0x42)
#define CEC_OPCODE_USR_CTL_PRESSED                                  (0x44)
#define CEC_OPCODE_USR_CTL_RELEASED                                 (0x45)
#define CEC_OPCODE_GIVE_OSD_NAME                                    (0x46)
#define CEC_OPCODE_SET_OSD_NAME                                     (0x47)
#define CEC_OPCODE_SYS_AUDIO_MODE_REQUEST			                (0x70)
#define CEC_OPCODE_GIVE_AUDIO_STATUS				                (0x71)
#define CEC_OPCODE_SET_SYS_AUDIO_MODE				                (0x72)
#define CEC_OPCODE_REPORT_AUDIO_STATUS				                (0x7A)
#define CEC_OPCODE_GIVE_SYS_AUDIO_MODE_STATUS		                (0x7D)
#define CEC_OPCODE_SYS_AUDIO_MODE_STATUS			                (0x7E)
#define CEC_OPCODE_ROUTING_CHANGE                                   (0x80)
#define CEC_OPCODE_ROUTING_INFORMATION                              (0x81)
#define CEC_OPCODE_ACTIVE_SOURCE                                    (0x82)
#define CEC_OPCODE_GIVE_PHYSICAL_ADDRESS                            (0x83)
#define CEC_OPCODE_REPORT_PHYSICAL_ADDRESS                          (0x84)
#define CEC_OPCODE_REQUEST_ACTIVE_SOURCE                            (0x85)
#define CEC_OPCODE_SET_STREAM_PATH                                  (0x86)
#define CEC_OPCODE_DEVICE_VENDOR_ID                                 (0x87)
#define CEC_OPCODE_VENDOR_CMD					                    (0x89)
#define CEC_OPCODE_GIVE_DEVICE_VENDOR_ID                            (0x8c)
#define CEC_OPCODE_MENU_REQUEST                                     (0x8d)
#define CEC_OPCODE_MENU_STATUS                                      (0x8e)
#define CEC_OPCODE_GIVE_DEVICE_PWR_STATUS                           (0x8f)
#define CEC_OPCODE_REPORT_PWR_STATUS                                (0x90)
#define CEC_OPCODE_GET_MENU_LANGUAGE                                (0x91)
#define CEC_OPCODE_INACTIVE_SOURCE                                  (0x9d)
#define CEC_OPCODE_CEC_VERSION                                      (0x9e)
#define CEC_OPCODE_GET_CEC_VERSION                                  (0x9f)
#define CEC_OPCODE_VENDOR_CMD_WITH_ID                               (0xA0)
#define CEC_OPCODE_REPORT_SHORT_AUDIO_DESCRIPTOR			        (0xA3)
#define CEC_OPCODE_REQUEST_SHORT_AUDIO_DESCRIPTOR			        (0xA4)
#define CEC_OPCODE_INITIATE_ARC						                (0xC0)
#define CEC_OPCODE_REPORT_ARC_INITIATED				                (0xC1)
#define CEC_OPCODE_REPORT_ARC_TERMINATED			                (0xC2)
#define CEC_OPCODE_REQUEST_ARC_INITIATION			                (0xC3)
#define CEC_OPCODE_REQUEST_ARC_TERMINATION			                (0xC4)
#define CEC_OPCODE_TERMINATE_ARC					                (0xC5)
#define CEC_OPCODE_ABORT                                            (0xff)

enum CEC_OPERAND_ABORT_REASON //SpikeAdd 20080522
{
	ABORT_REASON_UNRECOGNIZED_OPCODE = 0,
	ABORT_REASON_NOT_IN_CORRECT_MODE_TO_RESPOND,
	ABORT_REASON_CANNOT_PROVIDE_SOURCE,
	ABORT_REASON_INVALID_OPERAND,
	ABORT_REASON_REFUSED,
};

enum CEC_OPERAND_CEC_VERSION //spike 20080714
{
	CEC_VERSION_V1_1 = 0,
	CEC_VERSION_V1_2,
	CEC_VERSION_V1_2a,
	CEC_VERSION_V1_3,
	CEC_VERSION_V1_3a,
	CEC_VERSION_V1_4a,
};

enum CEC_OPERAND_DEVICE_TYPE //SpikeAdd 20080526
{
	DEVICE_TYPE_TV = 0,
	DEVICE_TYPE_REOCRDING_DEVICE,
	DEVICE_TYPE_RESERVED,
	DEVICE_TYPE_TUNER,
	DEVICE_TYPE_PLAYBACK_DEVICE,
	DEVICE_TYPE_AUDIO_SYSTEM,
};

enum CEC_OPERAND_POWER_STATUS
{
	POWER_STATUS_POWER_ON = 0,
	POWER_STATUS_STANDBY,
	POWER_STATUS_STANDBY_TO_ON = 2,
	POWER_STATUS_ON_TO_STANDBY,
};

enum CEC_OPERAND_AUDIO_FORMAT_CODE
{
	AFC_RESERVED = 0,
	AFC_LPCM,
	AFC_AC3,
	AFC_MPEG1,
	AFC_MP3=4,
	AFC_MPEG2,
	AFC_AAC = 6,
	AFC_DTS = 7,
	AFC_DDP = 10,
	AFC_UNUSE = 15,
};

enum CEC_OPERAND_UI_COMMAND
{
	UI_CMD_SELECT = 0,
	UI_CMD_UP,
	UI_CMD_DOWN,
	UI_CMD_LEFT,
	UI_CMD_RIGHT,
	UI_CMD_RIGHT_UP,
	UI_CMD_RIGHT_DOWN,
	UI_CMD_LEFT_UP,
	UI_CMD_LEFT_DOWN,
	UI_CMD_ROOT_MENU,
	UI_CMD_SETUP_MENU,
	UI_CMD_CONTENTS_MENU,
	UI_CMD_FAVORITE_MENU,
	UI_CMD_EXIT,
	RESERVE_0x0E_0x1F = 0x1F,
	UI_CMD_NUMBER_0,
	UI_CMD_NUMBER_1,
	UI_CMD_NUMBER_2,
	UI_CMD_NUMBER_3,
	UI_CMD_NUMBER_4,
	UI_CMD_NUMBER_5,
	UI_CMD_NUMBER_6,
	UI_CMD_NUMBER_7,
	UI_CMD_NUMBER_8,
	UI_CMD_NUMBER_9,
	UI_CMD_DOT,
	UI_CMD_ENTER,
	UI_CMD_CLEAR,
	RESERVE_0x2D_0x2F = 0x2F,
	UI_CMD_SOUND_SELECT = 0x33,
	UI_CMD_DISPLAY_INFOMATION = 0x35,
	RESERVE_0x39_0x3F = 0x3F,
	UI_CMD_POWER,
	UI_CMD_VOLUME_UP,
	UI_CMD_VOLUME_DOWN,
	UI_CMD_MUTE,
	UI_CMD_PLAY,
	UI_CMD_STOP,
	UI_CMD_PAUSE,
	UI_CMD_RECORD,
	UI_CMD_REWIND,
	UI_CMD_FAST_FORWARD,
	UI_CMD_EJECT,
	UI_CMD_FORWARD,
	UI_CMD_BACKWARD,
	RESERVE_0x4F_0x4F = 0x4F,
	UI_CMD_ANGLE = 0x50,
	UI_CMD_SUB_PICTURE = 0x51,
	RESERVE_0x56_0x5F = 0x5F,
	UI_CMD_MUTE_FUNCT = 0x65,
	UI_CMD_UNMUTE_FUNCT = 0x66,
	UI_CMD_SEL_MEDIA_FUNC = 0x68,
	UI_CMD_SEL_AV_INPIT_FUN = 0x69,
	UI_CMD_SEL_AUD_INPIT_FUN = 0x6A,
	UI_CMD_POWER_TOGGLE_FUNCT = 0x6B,
	UI_CMD_POWER_OFF_FUNCT = 0x6C,
	UI_CMD_POWER_ON_FUNCT = 0x6D,
	RESERVE_0x6E_0x70 = 0x70,
	UI_CMD_F1,
	UI_CMD_F2,
	UI_CMD_F3,
	UI_CMD_F4,
};

void OPC_UserCtrlPressed(stCecHandler *pstHandler);
void OPC_GiveSysAudModeSts(stCecHandler *pstHandler);
void OPC_GiveOsdName(stCecHandler *pstHandler);
void OPC_GiveDevVendorId(stCecHandler *pstHandler);
void OPC_ReportArcInitiated(stCecHandler *pstHandler);
void OPC_ReportArcTerminated(stCecHandler *pstHandler);
void OPC_GiveAudStatus(stCecHandler *pstHandler);
void OPC_FeatureAbort(stCecHandler *pstHandler);
void OPC_UserCtrlReleased(stCecHandler *pstHandler);
void OPC_RequestArcInitiation(stCecHandler *pstHandler);
void OPC_ReportPwrStatus(stCecHandler *pstHandler);
void OPC_GiveDevPwrStatus(stCecHandler *pstHandler);
void OPC_RequestArcTermination(stCecHandler *pstHandler);
void OPC_SysAudModeRequest(stCecHandler *pstHandler);
void OPC_GivePhyAddr(stCecHandler *pstHandler);
void OPC_GetCecVersion(stCecHandler *pstHandler);
void OPC_SetStreamPath(stCecHandler *pstHandler);
void OPC_ActiveSource(stCecHandler *pstHandler);
void OPC_RequestActiveSource(stCecHandler *pstHandler);
void OPC_Standby(stCecHandler *pstHandler);
//void OPC_SetStreamPath(stCecHandler* pstHandler);
void OPC_ActiveSource(stCecHandler *pstHandler);
void OPC_RequestShortAudDescriptor(stCecHandler *pstHandler);
void GoAbort(stCecHandler *, UINT8);
void OPC_SetAudMode(stCecHandler *pstHandler);


#endif

