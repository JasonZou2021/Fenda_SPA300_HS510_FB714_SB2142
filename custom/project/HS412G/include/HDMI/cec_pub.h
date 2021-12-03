// *****************************************************************************
// *    CEC process                                                           *
// *    Create by ECL                                                         *
// *    2007/10/01                                                            *
// *****************************************************************************
#ifndef CEC_PUB_H
#define CEC_PUB_H
#include "cec.h"

#define CEC_CMD_LIST_BUF_MAX      16

// ===================================================================
// Variable interface
// ===================================================================
typedef enum _eCecStatus
{
	CEC_STS_INACTIVE,
	CEC_STS_ACTIVE,
} eCecStatus; //feature status

typedef enum _eCecAudioStatus
{
	CEC_SYS_Audio_OFF,
	CEC_SYS_Audio_ON,
} eCecAudioStatus;

typedef enum _eCecChangeStatus
{

	Cec_Change_OFF,
	Cec_Change_ON,
} _eCecChangeStatus;

typedef enum _eCecMuteState
{
	CEC_SYS_DeMute,
	CEC_SYS_Mute,
} eCecMuteState;


typedef struct _stCecCustVar
{
	UINT8 HpdWakeUpEnable;
	UINT32 VendorID;
	UINT8 *pOsdNameStr;
	UINT8 OsdNameStrLen;
	UINT8 HpdVolThreshold;
	UINT8 *PwrOnOpcodeTbl;
	UINT8 PwrOnOpcodeNum;
	UINT8 HpdDetectFunc;
	UINT8 CecAlwaysRun;
	char *pbI2cDevStr;
	UINT8 *ShortAudDescriptor;
	UINT8 ShortAudDescriptorNum;
	UINT8 (*ShortAudDescriptorData)[3];
} stCecCustVar;

typedef struct _stCecCmdVar
{
	UINT8 Id;
	UINT8 List[CEC_CMD_LIST_BUF_MAX];
	UINT8 Count;
	UINT8 State;
} stCecCmdVar;



typedef struct _stCecTrmtChk
{
	stTimeChkHandler Time;
	UINT8 BusFree;
	UINT8 TrmtSucceed;
} stCecTrmtChk;

typedef struct _stCecWakeChk
{
	stTimeChkHandler Time;
	UINT8 BusState;
} stCecWakeChk;


//////////feature control//////////////
typedef struct _stSysAudCtrlFetr
{
	eCecStatus SysAudModeSts; //on:1 or off:0
	UINT8 DestAddr[4];
	UINT8 FlowStep;
	UINT8 RequestSacTimeout;
	UINT8 ActiveSrcPhyAddrAB;
	UINT8 ActiveSrcPhyAddrCD;
	UINT8 TvSupportSac;
	UINT8 Volume;
	UINT8 MuteState;
	UINT16 AudFormatRec;
} stSysAudCtrlFetr;

typedef struct _stGiveDevPwrStsFetr
{
	UINT8 DestAddr;
	UINT8 PwrStatus;
	UINT8 GetTvPwrTimeout;
} stGiveDevPwrStsFetr;

typedef struct _stArcCtrlFetr
{
	UINT8 RequstArcTimeout;
	UINT8 TvSupportArc;
} stArcCtrlFetr;

typedef struct _stDevOsdNameXfeFetr
{
	UINT8 DestAddr;
} stDevOsdNameXfeFetr; //device osd name transfer

typedef struct _stFeatureAbortFetr
{
	UINT8 DestAddr;
	UINT8 AbortOpcode;
	UINT8 AbortReason;
} stFeatureAbortFetr;

typedef enum _eCecUserCtrlReleasedStatus
{
	CEC_Status_UNREADY,
	CEC_Status_READY,
}eCecUserCtrlReleasedStatus;

typedef struct _stSysInfoFetr
{
	UINT8 DestAddr;
} stSysInfoFetr;

//////////////////////////

typedef int (*CECEvt_t)(BYTE opcode, BYTE *buf, int len);

//CEC source type
typedef enum _eSysSrcType
{
	eSysSrc_ARC,
	eSysSrc_HDMI_Tx,
	eSysSrc_Others,
	eSysSrc_Max,
} eSysSrcType_t; //system source info

// CEC handler
typedef struct
{
	UINT8 HwRdy;
	UINT8 PhyAddrAB;
	UINT8 PhyAddrCD;
	UINT8 PhyAddrRdy;
	UINT8 LogAddr;
	UINT8 LogAddrRdy;
	UINT8 PingLogAddrCont;
	UINT8 AutoPwrOn;
	UINT8 HpdStatus;
	UINT8 DeviceActive;

	UINT8 EDID_PhyaddrChange;
	UINT8 HPDLOWTimeout;
	unsigned int HPDLOWTime;
	unsigned int HPDHighTime;

	BYTE EdidData[512];

	UINT8 CecKeyCnt;
	INT16 CecKeyCode;
	UINT8 PhyAddrRed;
	UINT8 StandbyStatus;
	UINT8 PingMSG_Num;
    UINT8 ACKretry;
	eCecStatus CecEnable; //for iop scanner polling cec

	// ARC retry num 4
	UINT8 retryNum;

	//pinmux
	int HdmiCecPinNum;
	int HdmiHpdPinNum;
	int HdmiHpdAdcCh;

	//UINT32 CecPollingTime;

	stCecCustVar 	CustVar;
	stCecCmdVar 	Cmd;

	//Rx Tx data
	stCecFrameData Rx;
	stCecFrameData Tx;

	//Bus busy check
	stBusBusyChk BusBusyChk;

	//cec terminate check
	stCecTrmtChk CecTrmtChk;

	//cec wake up check
	stCecWakeChk CecWakeChk;

	//CEC feature
	stDevOsdNameXfeFetr DevOsdNameXfe;
	stGiveDevPwrStsFetr GiveDevPwrSts;
	stSysAudCtrlFetr 	SysAudCtrl;
	stArcCtrlFetr 		ArcCtrl;
	stFeatureAbortFetr 	FeatureAbort;
	stSysInfoFetr		SysInfo;

	// cec event callback function
	CECEvt_t pCECEvt_CbFunc;

	// system source type
	eSysSrcType_t SysSrc;
	stCecFrameData CecReadList[CEC_CMD_LIST_BUF_MAX];
	UINT8 CecReadList_Start;
	UINT8 CecReadList_End;
	UINT8 CECStandby;
	UINT8 CEC_state_change;
	UINT8 CEC_SourceState;
	UINT8 CEC_SET_DSP_ON;
	UINT8 CEC_SET_DSP_change;
	UINT8 UserCtrlReleased;
} stCecHandler;

// ===================================================================
// Function interface
// ===================================================================
int HDMI_PollingCec(stCecHandler *pstHandler);
int CEC_NormalInitiate(stCecHandler *pstHandler);
int CEC_HwInit(stCecHandler *pstHandler);
int HDMI_PollingHpd(stCecHandler *pstHandler);
int CEC_AudioStatusReport(stCecHandler *pstHandler);
int CEC_VolConvert(UINT8 bVol, UINT8 bMaxVol);
int CEC_NormalTerminate(stCecHandler *pstHandler);
int CEC_StandbyTerminate(stCecHandler *pstHandler);
int CEC_StandbyInitiate(void);
int CEC_GetPowerState(void);
int CEC_TerminatedCheck(stCecHandler *pstHandler);
UINT8 *HDMI_get_EDID(stCecHandler *pstHandler);
int HDMI_get_HPD(stCecHandler *pstHandler);
int CEC_SetSac(stCecHandler *pstHandler, UINT8 Status);
stCecHandler *CEC_GetCecHandler(void);
int CEC_SetSysSrc(stCecHandler *pstHandler, UINT8 SysSrcParam);
int CEC_StandbyChk(stCecHandler *pstHandler);
int CEC_ARCSourceChk(stCecHandler *pstHandler);
int CEC_SetDSPChk(stCecHandler *pstHandler);
int CEC_SYN_LoglAddr(stCecHandler *pstHandler);





#endif

