#ifndef HDMI_SRV_H
#define HDMI_SRV_H

#include "qp_pub.h"
#include "cfg_sdk.h"

#define CEC_PACKET_SIZE_MAX (16)

typedef struct
{
	/* protected: */
	QEvt super;

	/* public: */
	//uint8_t scan;
} HDMISrvEvt;

typedef struct
{
	UINT8 opcode;
	UINT8 pCEC_Data[CEC_PACKET_SIZE_MAX];
	UINT8 size;
} stCecCmdInfo_t;

typedef struct
{
	QEvt stEvt;
	stCecCmdInfo_t stCecInfo;
} stCECEvt_t;

typedef struct
{
	QEvt stEvt;
	UINT8 bCecSrc;
} stCecSetSrc_t;

typedef struct
{
	QEvt stEvt;
	UINT8 DSP_state;
} stCecSetDSP_t;


typedef struct
{
	QEvt stEvt;
	UINT8 bEnble;
} stCecSetSac_t;

enum
{
	eIOP_CecSrc_ARC = 0,
	eIOP_CecSrc_HDMI_TX,
	eIOP_CecSrc_Others,
	eIOP_CecSrc_Max,
};



typedef struct
{
	UINT8 bState;                // key code after judgment
} stHDMI_HPDInfo_t;

QActive *HDMISrv_get(void);
int HDMISrv_Check_ARCWakeUp(void);

int HDMISrv_ARC_Initiate(void const *pSender);
int HDMISrv_ARC_Terminate(void const *pSender);
int HDMISrv_HPDBroadcast_Request(void const *pSender);



int HDMISrv_HDMI_TX_Initiate(void const *pSender);
int HDMISrv_HDMI_TX_Terminate(void const *pSender);
int HDMISrv_CEC_SetSrc(void const *pSender, UINT8 pParam);
int HDMISrv_CEC_Set_SAC(void const *pSender, int iEnable);
int HDMISrv_RegHPDListener(QActive *const me, ARFunc_t pfFunc);

//int HDMISrv_CEC_Standby(QActive *const me);
//int HDMISrv_CEC_ARCSource (QActive *const me,UINT8 ARC_SRC);
//int HDMISrv_CEC_SetDSP(QActive *const me,UINT8 DSPState);



#endif
