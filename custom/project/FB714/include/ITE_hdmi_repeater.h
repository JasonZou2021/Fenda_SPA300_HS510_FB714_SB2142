#ifndef __HDMI_REPEATER_H__
#define __HDMI_REPEATER_H__

#define REPEATER_ADDR_MAX (0x2D)

typedef struct
{
    char* pCtlStr;
    unsigned char SlaveAddr;
    unsigned char pRegTbl[REPEATER_ADDR_MAX];
	UINT8 DSP_AUD_FIFO;
}stHdmiRepeatHandler_t;

//=========================================
int HDMI_Repeat_Set(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen);
int HDMI_Repeat_Get_info(stHdmiRepeatHandler_t* pstRepeatHandler, UINT32 RegAddr, void *bBuf, UINT32 dLen);
int HDMI_Repeat_init(stHdmiRepeatHandler_t* pstRepeatHandler);

#endif

