// *****************************************************************************
// *    CEC process                                                           *
// *    Create by ECL                                                         *
// *    2007/10/01                                                            *
// *****************************************************************************
#ifndef CEC_MAIN_H
#define CEC_MAIN_H
#include "cec_pub.h"
//#include "cec.h"

#define LOG_MODULE_ID  ID_CEC

#include "log_utils.h"


#define CEC_DBG

#ifdef CEC_DBG
	#define cec_printf(f, a...)  	LOGD(f, ##a)
	#define cec_printf0(f, a...)  	LOGX(f, ##a)
	#define cec_printf2(f, a...)  	LOGDT(f, ##a)
	#define cec_printf_E(f, a...) 	LOGE(f, ##a)
	#define cec_printf_W(f, a...) 	LOGW(f, ##a)
#else
	#define cec_printf(f, a...)  	do {} while (0)
	#define cec_printf0(f, a...)  	do {} while (0)
	#define cec_printf2(f, a...)  	do {} while (0)
	#define cec_printf_E(f, a...) 	do {} while (0)
	#define cec_printf_W(f, a...) 	do {} while (0)
#endif

//for EDID
#define EDID_AD                                0xa0
#define EDID_CEA_EXT_OFFSET        		128
#define EXT_FLAG_OFFSET             	0x7e
#define EDID_BLOCK_LEN                  (0x80)
#define EDID_READ_RETRY                 8

//

#define CEC_TERMINATED_DELAY_TIME 500			//STC time,unit:1ms     about 500ms

#define CEC_MAX_OSD_NAME_LEN 14
#define CEC_VOL_MASK    0x7f


typedef enum _EDIDI2C3Chk
{
	EDID_I2C_OK,
	EDID_I2C_Fail,

} EDIDI2C3Chk; //feature status

enum CEC_LOGICAL_ADDRESS
{
	CEC_LOG_ADDR_TV = 0,
	CEC_LOG_ADDR_RECORDING_DEVICE1,
	CEC_LOG_ADDR_RECORDING_DEVICE2,
	CEC_LOG_ADDR_STB,
	CEC_LOG_ADDR_DVD1,
	CEC_LOG_ADDR_AUDIO_SYSTEM,
	CEC_LOG_ADDR_STB2,
	CEC_LOG_ADDR_STB3,
	CEC_LOG_ADDR_DVD2,
	CEC_LOG_ADDR_RECORDING_DEVICE3,
	CEC_LOG_ADDR_STB4,
	CEC_LOG_ADDR_DVD3,
	CEC_LOG_ADDR_RESERVED1,
	CEC_LOG_ADDR_RESERVED2,
	CEC_LOG_ADDR_FREEUSE,
	CEC_LOG_ADDR_UNREGISTERED, //0b1111 //broadcast
	CEC_LOG_ADDR_MAX,
};
#define CEC_LOG_ADDR_BROADCAST       CEC_LOG_ADDR_UNREGISTERED

extern void invalidate_dcache(void);

// ===================================================================
// Function interface
// ===================================================================
void Cec_HandlerInit(stCecHandler *pstHandler);
void Cec_CmdProcess(stCecHandler *pstHandler);
void Cec_OpcProcess(stCecHandler *pstHandler);
void Cec_SendCecCmd(stCecHandler *pstHandler, UINT8 tbCmd);
UINT8 Cec_CmdListInsert(stCecHandler *pstHandler, UINT8 tbCmd);
void Cec_SetPowerStatus(stCecHandler *pstHandler, UINT8 tbPwrStatus); //cec power status
void Cec_SetSysAudioCtrlStatus(stCecHandler *, eCecStatus);
void Cec_SysAudCtrlInit(stCecHandler *pstHandler);
void Cec_SetFuncActive(stCecHandler *pstHandler);
UINT8 Cec_GetAutoPwrOn(stCecHandler *pstHandler);
void Cec_SetAutoPwrOn(stCecHandler *pstHandler, UINT8 tbAction);

int Cec_GetVol(void);
int Cec_GetMuteState(void);
void Cec_GetPhysicalAddr(stCecHandler *pstHandler);
UINT8 Edid_ReadEdid(stCecHandler *pstHandler, BYTE *pEdidData);
void Edid_ParseExtension(stCecHandler *pstHandler);
UINT8 Hpd_DetectLevelByGpio(stCecHandler *pstHandler);
void Cec_CmdRead(stCecHandler *pstHandler);
void Cec_CmdRead_BUS(stCecHandler *pstHandler);



UINT8 Cec_InitiationARCChk(stCecHandler *pstHandler);






#endif

