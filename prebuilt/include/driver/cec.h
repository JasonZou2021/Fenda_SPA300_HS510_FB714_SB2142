#ifndef CEC_H
#define CEC_H



#define cec_printf(f, a...)  	LOGD(f, ##a)
#define cec_printf0(f, a...)  	LOGX(f, ##a)
#define cec_printf2(f, a...)  	LOGDT(f, ##a)
#define cec_printf_E(f, a...) 	LOGE(f, ##a)

#define CEC_TAR_ADDR_MASK           0x0f
#define CEC_SRC_ADDR_MASK           0xf0

// CEC Aditional Flag
#define CEC_AD_FLAG_FREE                4
#define CEC_AD_FLAG_ARB                 5
#define CEC_AD_FLAG_ACK1                6
#define CEC_AD_FLAG_ACK0                7
#define CEC_AD_FLAG_TEST                0xff

#define CMD_HDMI_ENABLE        7
#define IOP_HDMI_ENABLE        1


typedef struct _stCecFrameData
{
	UINT8 Header;
	UINT8 Opcode;
	UINT8 Operand[14];
	UINT8 MsgLen;
} stCecFrameData;

typedef struct _stTimeChkHandler
{
	UINT32 Current;
	UINT32 Previous;
	UINT32 Interval;
} stTimeChkHandler;

typedef struct _stBusBusyChk
{
	stTimeChkHandler Time;
	UINT8 BusyRW;
	UINT32 Cec_PollingTime;
} stBusBusyChk;

enum CEC_ERROR_HANDLE_STATUS
{
	CEC_ERR_HANDLE_OK = 0,
	CEC_ERR_HANDLE_WAITING_READ_FLG,
	CEC_ERR_HANDLE_SENDING_FRAME,
	CEC_ERR_HANDLE_RESENDING_FRAME,
	CEC_ERR_HANDLE_RESEND_TIMEOUT,
	CEC_ERR_HANDLE_PING_SELF_NO_ACK,
	CEC_ERR_HANDLE_FATAL_ERROR = 0xff,
};

UINT8 Cec_GetAdFlag(UINT8 tbAdFlag);
UINT8 Cec_GetLogAddr1(void);
UINT8 Cec_GetLogAddr2(void);
void Cec_SetLoglAddr1(UINT8 tbAddr);
void Cec_SetLoglAddr2(UINT8 tbAddr);
UINT8 Cec_BusBusyChk(stBusBusyChk *BusBusyChk);
UINT8 Cec_CMDReadListGet(stCecFrameData *cec_cmd);
UINT8 Cec_ReadFrame(stCecFrameData *pstRx, UINT8 power_status);
void Cec_SendFrame(stCecFrameData *pstTx);
UINT8 HDMI_GET_BUSY_FLG(void);
UINT8 HDMI_GET_READ_FLG(void);
UINT8 HDMI_GET_WRITE_FLG(void);
UINT8 HDMI_CLR_BUSY_FLG(void);
UINT8 HDMI_CLR_READ_FLG(void);
UINT8 HDMI_CLR_WRITE_FLG(void);
UINT8 IOP_HDMISendCmd(UINT8 Source,UINT8 Cmd);






#endif

