#ifndef __HDMI_REPEATER_H__
#define __HDMI_REPEATER_H__

//==============upgrade ===================
#define HDMI_BLOCK_LEN	512

typedef struct {

	int part_addr;
	UINT32 RcvData;
	UINT32 SndData;
	UINT32 CodeSize;
}EPCodeSrv_t;



#define EP_UPGRADE_WRITE_START (0x01)
#define EP_UPGRADE_WRITE_END (0x02)

#define EP_UPGRADE_CHECK_0x0f (0x0f)

#define EP_UPGRADE_BLACK_HANDLER (0x10)

#define EP_UPGRADE_WRITE_DATA_REG (0x41)



#define EP_UPGRADE_CHECK_0xc1 (0xC1)

#define EP_UPGRADE_SLAVEADDR (0x50)

#define EP_UPGRADE_CHECK_SLAVEADDR (0xc9)

//================Upgrade END=================

//=========================================

#define REPEATER_ADDR_MAX (0x2D)

//=========================================
#define EP_ADDR_GEN_INFO_0x08 (0x08)
typedef union
{
    UINT8 DataByte;
    struct
    {
    	unsigned ARC_ON: 1;
        unsigned eARC_ON: 1;
		unsigned eARC_Mute: 1;
        unsigned Reserve_bit1_2: 2;
        unsigned Need_ACV: 1;
        unsigned Tx_HDMI: 1;
        unsigned HPD:1;
    };
}unEP_BasicInfo_reg0x08_t;

//=========================================
#define EP_ADDR_GEN_INFO_0x09 (0x09)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned ADO_CHF: 1;
        unsigned CEC_ECF: 1;
        unsigned Reserve_bit2_7: 6;
    };
}unEP_BasicInfo_reg0x09_t;

//=========================================
#define EP_ADDR_GEN_CTRL_0x10 (0x10)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned ARC_EN: 1;
        unsigned CEC_Mute: 1;
        unsigned CEC_DIS: 1;
        unsigned Video_Path: 1;
        unsigned A_Reset: 1;
        unsigned Audio_Path: 1;
        unsigned eARC_EN: 1;
        unsigned Power: 1;
    };
}unEP_GenCtrl_reg0x10_t;

enum
{
    eAudio_Path_TV = 0,
    eAudio_Path_External_Speaker,

    eAudio_Path_Max,
};

//=========================================
#define EP_ADDR_GEN_CTRL_0x11 (0x11)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned Primary_Sel: 4;
        unsigned Reserve_bit4_7: 4;
    };
}unEP_GenCtrl_reg0x11_t;

enum
{
    ePrimary_Sel_Port0 = 0,
    ePrimary_Sel_Port1,
    ePrimary_Sel_Port2,
    ePrimary_Sel_None_ARC = 5, // None HDMI function without Video Mute, can response routing info to TV routing change (TV, ARC...)
    ePrimary_Sel_None_Inactive, // None HDMI function with Video Mute cannot response routing info to TV routing change (Bluetooth, FM... when don't want interrupted by TV)

    ePrimary_Sel_Max,
};

//=========================================
#define EP_ADDR_GEN_INFO_PHY_ADDR_H (0x0A)

//=========================================
#define EP_ADDR_GEN_INFO_PHY_ADDR_L (0x0B)

//=========================================
#define EP_ADDR_GEN_CTRL_0x12 (0x12)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned Source_EN: 1;
        unsigned Secondary_Sel: 3;
        unsigned Reserve_bit4: 1;
        unsigned eARC_DIS: 1;
        unsigned ARC_DIS: 1;
        unsigned Reserve_bit7: 1;
    };
}unEP_GenCtrl_reg0x12_t;

//=========================================
#define EP_ADDR_CEC_VOLUME (0x13)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned VOLUME: 8;

    };
}unEP_BasicInfo_reg0x13_t;

#define EP_ADDR_CEC_EVENT_CODE (0x15)
#define EP_ADDR_CEC_EVENT_PARAM_1 (0x16)
#define EP_ADDR_CEC_EVENT_PARAM_2 (0x17)
#define EP_ADDR_CEC_EVENT_PARAM_3 (0x18)
#define EP_ADDR_CEC_EVENT_PARAM_4 (0x19)

//{// CEC parameter
#define EP_CEC_EVT_ACTIVE_SOURCE (3)
#define EP_CEC_EVT_GO_TO_STANDBY (5)
#define EP_CEC_EVT_USER_CTRL (12)
#define EP_CEC_EVT_SET_STREAM_PATH (16)
#define EP_CEC_EVT_AUD_MODE (26)
#define EP_CEC_EVT_ROUTING_OFF (27)
#define EP_CEC_EVT_ROUTING_ON (29)



#define EP_CEC_FROM_TV (0)
#define EP_CEC_FROM_DVD (4)
#define EP_CEC_SYSTEM_AUDIO_ON  (1)
#define EP_CEC_SYSTEM_AUDIO_OFF (0)
#define EP_CEC_PORT_NUMBER_ZOOM (0)


#define EP_CEC_VOL_UP (0x41)
#define EP_CEC_VOL_DOWN (0x42)
#define EP_CEC_MUTE (0x43)
#define EP_CEC_POWER (0x40)
//}

#define EP_ADDR_AUDIO_INFO_START (0x20)
typedef union

{
	UINT8 DataByte;
		struct
		{
			//unsigned Reserve_bit0: 1;
			unsigned LAYOUT: 1;
			unsigned DTS_DOUBLE: 1;
			unsigned eARC_MUTE:  1;
			unsigned Reserve_bit3: 1;
			unsigned HDMI: 1;
			unsigned AVMUTE: 1;
			unsigned MLCK_OK:1;
			unsigned Reserve_bit7: 1;
		};

}unEP_AudStatus_reg0x20_t;

#define EP_AUDIO_INFO_REG_MUN (10)

//=========================================
#define EP_ADDR_AUDIO_STATUS (0x22)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned SAMP_FREQ: 3;
        unsigned STD_ADO: 1;
        unsigned DSD_ADO: 1;
        unsigned HBR_ADO: 1;
        unsigned DST_ADO:1;
        unsigned Reserve_bit7: 1;
    };
}unEP_AudStatus_reg0x22_t;

//=========================================
#define EP_ADDR_AUDIO_CH_STATUS_0x23 (0x23)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned PRO: 1;
        unsigned PCM: 1;
        unsigned COPY: 1;
        unsigned PRE: 3;
        unsigned MODE:2;
    };
}unEP_ChStatus_reg0x23_t;

//=========================================
#define EP_ADDR_AUDIO_CH_STATUS_0x25 (0x25)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned SRC_NUM: 4;
        unsigned CH_NUM: 4;
    };
}unEP_ChStatus_reg0x25_t;

//=========================================
#define EP_ADDR_AUDIO_INFO_0x29 (0x29)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned CC: 3;
        unsigned Reserve_bit3: 1;
        unsigned CT: 4;
    };
}unEP_AudInfo_reg0x29_t;

//=========================================

#define EP_ADDR_AUDIO_INFO_0x2C (0x2C)
typedef union
{
    UINT8 DataByte;
    struct
    {
        unsigned LFE: 1;
        unsigned FC: 1;
        unsigned BC_LS: 1;
		unsigned RS_FLC: 1;
		unsigned RLC_FLC: 1;
		unsigned TpFc: 1;
		unsigned FLC_TPFL: 1;
		unsigned FRC_TpFc: 1;

    };
}unEP_AudInfo_reg0x2C_t;

enum
{
    audio_chmap_Disable = 0,
    audio_chmap_Enable = 1,

};

typedef union
{
    UINT32 DataByte;
    struct
    {
        unsigned FL: 1;
        unsigned FR: 1;
		unsigned LS: 1;
		unsigned RS: 1;
		unsigned FC: 1;
        unsigned LFE: 1;
		unsigned Lrs: 1;
		unsigned Rrs: 1;
		unsigned TPFL: 1;
		unsigned TpFR: 1;

    };
}unDSP_AudInfo_t;

//=========================================

typedef struct
{
    char* pCtlStr;
    unsigned char SlaveAddr;
    unsigned char pRegTbl[REPEATER_ADDR_MAX];
	UINT8 DSP_AUD_FIFO;
}stHdmiRepeatHandler_t;

//=========================================
typedef int (*I2C_Chk_CB_t)(void);
int HDMI_I2C_errcheck_RegCB(I2C_Chk_CB_t cb_t);
int I2C_check_err(void);
int HDMI_Repeat_Set(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen);
int HDMI_Repeat_Get_info(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen);
int HDMI_Repeat_init(stHdmiRepeatHandler_t* pstRepeatHandler);
int HDMI_Upgrade_process(UINT8 Upgrade_key);
int HDMI_GetVersion(stHdmiRepeatHandler_t *pstRepeatHandler);
int Repeater_EP_BB_ICP(stHdmiRepeatHandler_t *pstRepeatHandler,UINT32 SndData);
int Repeater_EP_UPgrading(stHdmiRepeatHandler_t *pstRepeatHandler,EPCodeSrv_t *pstCodeHandler);
UINT32 Read_flash_EPcode(UINT32 ptr_add,char *buffer,int Read_Lengh );
void HDMI_Upgrade_VER(stHdmiRepeatHandler_t *pstRepeatHandler);







#endif

