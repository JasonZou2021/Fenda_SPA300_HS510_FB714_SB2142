/*${.::UserApp.h} ..............................................................*/

#ifndef iopscanner_h

#define iopscanner_h

#include "qp_pub.h"
#include "cfg_sdk.h"
//#include "iop_user.h"
#include "multi_ir.h"


#define KEY_BUF_SIZE 10


/*${Events::UserAppEvt} ......................................................*/
typedef struct
{
	/* protected: */
	QEvt super;

	/* public: */
	//uint8_t scan;
} IopScannerEvt;

typedef struct
{
	UINT16 adc_data;
	UINT8 adc_ch;
} stADC_DATA_t;

typedef struct
{
	//QEvt super;
	//char *pbName;
	//char *pbBufRet;
	//int sdLen;

	SYNCEVT sync;
	int ch;
} stADC_SYNCEVT;

typedef struct
{
	QEvt stEvt;
	int  switch_src_time;
	int  power_off_time;
	int  power_on_time;
} Burn_AutoSrcTestEvt_t;

typedef struct
{
	QEvt stEvt;
	int  private_signal_number;
	int  private_signal_repeat_time;
} Burn_AutoSignalTestEvt_t;


enum
{
	eKeyPress,
	eKeyRelease,
	eKeyLongPress,
	eKeyLongRelease,
	eKeyLongTick,
	eKeyStateNum,
};

enum
{
	eKeySrc_IR = 0,
	eKeySrc_VFD_Key,
	eKeySrc_ADC_0_Key,
	eKeySrc_ADC_1_Key,
	eKeySrc_ADC_2_Key,
	eKeySrc_ADC_3_Key,
	eKeySrc_CEC_CMD,
	eKeySrc_PWR_Key,
	eKeySrc_GPIO_Key,
	eKeySrc_IR_L_Key,
	eKeySrc_Num,
};

typedef struct
{
	int key;                // key code after judgment
	int key_cnt;         // key counter
	int key_pre;        // the previous key we get
	int key_current; // the key we get currently
	char keysrc;       // the key from which source
	char keysrc_pre;// the previous key from which source
	char key_type;  // press, release, long press, long release,or tick
	int IR_Fmt;
	UINT32 IR_Cus_ID;
	char iTickCnt;
	UINT32 Key_STCStamp;
	UINT8 product_data;
} stKeyInfo_t;

typedef struct
{
	int MonkeyTest_Enable;
	int KeyIntervalTick;
	int KeyPressTick;
	int KeyData;
	char keysrc;       // the key from which source
	char *pIR_SupportedKey;
	int IRKeyCodeSupportedNum;
	char *pCEC_SupportedCmd;
	int CEC_CmdSupportedNum;
	int KeyIntervalTickMax;
	int KeyPressTickMax;
	char KeySrc_tbl[eKeySrc_Num];
	char KeySrc_num;
} stMonkeyTest_t;

typedef struct
{
	stKeyInfo_t buffer[KEY_BUF_SIZE];
	int head;
	int tail;
	int maxLen;
	int KeyNum; // how many key in this key ring buf
} hCircBuf_t;

typedef struct
{
	QEvt stEvt;
	UINT8 bVol; // unit: %
	UINT8 bMuteState;
} stAudInfoEvt_t; // for update audio info

//IR learning table
enum
{
	IR_L_KEY_0 = 0, //IR_L_KEY_0 is only for power key

	IR_L_KEY_1,		//qingyu.wu add for vizio need learning three sets of remote
    IR_L_KEY_2,
    IR_L_KEY_3,

    IR_L_KEY_1_2,
    IR_L_KEY_2_2,
    IR_L_KEY_3_2,

    IR_L_KEY_1_3,
    IR_L_KEY_2_3,
    IR_L_KEY_3_3,

	IR_L_KEY_NUM,
};

typedef struct
{
	QEvt stEvt;
	int LearningKey_Target;
} stSYS_IrLearningInfo_t;

typedef struct
{
	QEvt stEvt;
	UINT8 TV_Remote_Ready_Set;
} stIr_TV_Remote_Ready_Info_t;


typedef struct
{
	int IR_Fmt;
	UINT32 CustomerCode;
	UINT16 KeyCode;
} stIR_L_Info_t;

typedef struct
{
	stIR_L_Info_t stIR_info;
	UINT8 pIR_WaveForm[128]; //IR_RAWDATA_SIZE
} stSaveIRInfo_t;

/*${Events::MonkeyTestEvt} ......................................................*/
typedef struct
{
	QEvt stEvt;
	int  KeyIntervalTickMax; // unit: 100ms // QTimeEvt_armX(&me->timeEvt, TICKS_PER_100MS, 0);
	int  KeyPressTickMax; // unit: 100ms // QTimeEvt_armX(&me->timeEvt, TICKS_PER_100MS, 0);
} stMonkeyTestEvt_t;

typedef struct
{
	QEvt stEvt;
	int MuteState;
} stSYS_MuteReq_t;

extern hCircBuf_t circBuf;
extern stKeyInfo_t stKeyData;

int circBufPop(hCircBuf_t *c, stKeyInfo_t *data);

/*${AOs::IopScanner_ctor} .......................................................*/
void IopScanner_ctor(void);
QActive *IopScanner_get(void);
int IOSrv_ADCScan(void const *pSender, UINT8 ucADC_ch);

int PushKeyInfo(hCircBuf_t *c, stKeyInfo_t *data);
int KeyProcess(int KeyCode, int KeySrc, stKeyInfo_t *pstKeyData, CFG_KEY_T *pstKEY_cfg);
int IOSrv_getKeyCode_MultiIR(stIR_Info_t *pstIR_Info);

/******************************************************************************************/
/**
 * \fn          void IOSrv_IR_Learning_Start(QActive * const me, stSYS_IrLearningInfo_t *pParam, ARFunc_t pfFunc)
 *
 * \brief      start IR learning process
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : IR learning info structure pointer (structure type: stSYS_IrLearningInfo_t)
*                ARFunc_t pfFunc : callback function
 *
 * \return	    -1: fail, 0: success
 *
 *
 ******************************************************************************************/
int IOSrv_IR_Learning_Start(QActive *const me, stSYS_IrLearningInfo_t *pParam, ARFunc_t pfFunc);

/******************************************************************************************/
/**
 * \fn          void IOSrv_IR_Learning_Confirm(QActive * const me, void *pParam)
 *
 * \brief      confirm IR learning process
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : IR learning info structure pointer (structure type: stSYS_IrLearningInfo_t)
 *
 * \return	    -1: fail, 0: success
 *
 *
 ******************************************************************************************/
int IOSrv_IR_Learning_Confirm(QActive *const me, stSYS_IrLearningInfo_t *pParam);

/******************************************************************************************/
/**
 * \fn          void IOSrv_IR_Learning_Terminate(QActive * const me, void *pParam)
 *
 * \brief      terminate IR learning process
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *               void *pParam : IR learning info structure pointer (structure type: stSYS_IrLearningInfo_t)
 *
 * \return	    -1: fail, 0: success
 *
 *
 ******************************************************************************************/
int IOSrv_IR_Learning_Terminate(QActive *const me, stSYS_IrLearningInfo_t *pParam);

/******************************************************************************************/
/**
 * \fn          void IOSrv_IR_Learning_Erase(QActive * const me, void *pParam)
 *
 * \brief      erase IR learning data
 *
 * \param   QActive * const me:(Input) AO handler ,\n
 *
 * \return	    -1: fail, 0: success
 *
 *
 ******************************************************************************************/
int IOSrv_IR_Learning_Erase(QActive *const me);


/******************************************************************************************/
/**
 * \fn          void IOSrv_TV_Remote_Ready_Set(QActive * const me, stIr_TV_Remote_Ready_Info_t *pParam)
 *
 * \brief      to set IR TV remote ready function (enable/disable)
 *
 * \param       QActive * const me:(Input) AO handler ,\n
 *               void *pParam : 1 to enable, 0 to disable (structure type: stIr_TV_Remote_Ready_Info_t)
 *
 * \return	    -1: fail, 0: success
 *
 *
 ******************************************************************************************/
int IOSrv_TV_Remote_Ready_Set(QActive *const me, stIr_TV_Remote_Ready_Info_t *pParam);

int Save_IR_WaveformArray(stSaveIRInfo_t *pBuf, int BufNum);
int Read_IR_WaveformArray(stSaveIRInfo_t *pBuf, int BufNum);
int Read_IR_WaveformPower(stSaveIRInfo_t *pBuf);

int IOSrv_SAR_ADC_GET_DATA(int ADC_CH);
int IOSrv_SAR_ADC_DISABLE(void);
int IOSrv_AUD_ADC_DAC_DISABLE(void);
void IOSrv_CEC_KEY_State(INT16 CEC_Key);

#endif /* iopscanner_h */
