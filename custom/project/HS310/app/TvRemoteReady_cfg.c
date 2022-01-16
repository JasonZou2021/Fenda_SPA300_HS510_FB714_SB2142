/******************************************************************************/
/**
*
* \file	    TvRemoteReady_cfg.c
*
* \brief	configuration for MultiIR.\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author   Daniel.yeh@sunplus.com
* \version	 v0.01
* \date 	 2017/06/22
******************************************************************************/


/*---------------------------------------------------------------------------*
 *                            INCLUDE   DECLARATIONS                         *
 *---------------------------------------------------------------------------*/
#include "TvRemoteReady_cfg.h"
#include "ir_key.h"

/*---------------------------------------------------------------------------*
    IR config
*---------------------------------------------------------------------------*/

//IR UPD6121
const UINT32 CusID_UPD6121[] = {0x04FB, 0x7108, 0x837A, 0x609F, 0x00FF, 0x15EA, 0x28D7, 0x40BF, 0x38C7, 0x7F, 0x4BA, 0xB34C, IR_Cus_ID_TBL_END}; //the custmer ID we support in UPD6121 // put "IR_Cus_ID_TBL_END" in the end of this table
const UINT16 KeyCode_0x4BA[] = {0x1A, 0x0E, 0x14, IR_KEY_CODE_TBL_END}; //the Key code table for customer ID: 0x4BA // put "IR_KEY_CODE_TBL_END" in the end of this table
const UINT16 KeyCode_0xB34C[] = {0x80, 0x81, 0x9C, IR_KEY_CODE_TBL_END}; //the Key code table for customer ID: 0xB34C // put "IR_KEY_CODE_TBL_END" in the end of this table

const UINT16 KeyCode_0x04FB[] = {0x2, 0x3, 0x9, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x7108[] = {0x2, 0x3, 0x9, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x837A[] = {0x2, 0x3, 0x9, IR_KEY_CODE_TBL_END};

const UINT16 KeyCode_0x609F[] = {0xc, 0xd, 0x7, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x00FF[] = {0x1f, 0x18, 0x0, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x15EA[] = {0x16, 0x17, 0x15, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x28D7[] = {0xe, 0xf, 0x10, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x40BF[] = {0x1a, 0x1e, 0x10, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x38C7[] = {0xe, 0xf, 0x18, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x7F[] = {0x1d, 0x9, 0x7, IR_KEY_CODE_TBL_END};// Sunplus IR400


//IR TC9012
const UINT32 CusID_TC9012[] = {0x0707, 0x2c2c, IR_Cus_ID_TBL_END}; //the custmer ID we support in TC9012 // put "IR_Cus_ID_TBL_END" in the end of this table
const UINT16 KeyCode_0x0707[] = {0x07, 0x0B, 0x0F, IR_KEY_CODE_TBL_END};//the Key code table for customer ID: 0x0707 // put "IR_KEY_CODE_TBL_END" in the end of this table
const UINT16 KeyCode_0x2c2c[] = {0x17, 0x16, 0x1F, IR_KEY_CODE_TBL_END};//the Key code table for customer ID: 0x0707 // put "IR_KEY_CODE_TBL_END" in the end of this table

//IR M50560
const UINT32 CusID_M50560[] = {0x0002, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x0002[] = {0x3, 0x4, 0x9, IR_KEY_CODE_TBL_END};

//IR MATC6D6
const UINT32 CusID_MATC6D6[] = {0x0001, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_MATC6D6_0x0001[] = {0x14, 0x15, 0x17, IR_KEY_CODE_TBL_END};

//IR SONYD7C5
const UINT32 CusID_SONYD7C5[] = {0x01, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x01[] = {0x12, 0x13, 0x14, IR_KEY_CODE_TBL_END};

//IR SONYD7C8
const UINT32 CusID_SONYD7C8[] = {0x54, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x54[] = {0x12, 0x13, 0x14, IR_KEY_CODE_TBL_END};

//IR AHEA48
const UINT32 CusID_AHEA48[] = {0x00802002, 0x00a02002, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x00802002[] = {0xA020, 0xA121, 0xB232, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x00a02002[] = {0x8020, 0x8121, 0x9232, IR_KEY_CODE_TBL_END};


//IR MATC5D6
const UINT32 CusID_MATC5D6[] = {0x001F, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x001F[] = {0x24, 0x25, 0x00, IR_KEY_CODE_TBL_END};

//IR RC6
const UINT32 CusID_RC6[] = {0x00, 0x10, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_0x00[] = {0x10, 0x11, 0x0D, IR_KEY_CODE_TBL_END};
const UINT16 KeyCode_0x10[] = {0x10, 0x11, 0x0D, IR_KEY_CODE_TBL_END};

//IR SHARP
const UINT32 CusID_SHARP[] = {0x0001, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_SHARP_0x0001[] = {0x14, 0x15, 0x17, IR_KEY_CODE_TBL_END};

//IR SAMSUNG
const UINT32 CusID_SAMSUNG[] = {0xf30, IR_Cus_ID_TBL_END};
const UINT16 KeyCode_SAMSUNG_0x0001[] = {0x77, 0x78,0x71, 0x82, 0x83, 0x70, 0x75, 0x7b, IR_KEY_CODE_TBL_END};

// SAMSUNG key cmd table
const UINT8 KeyCmd[] = {IRC_VOLUME_UP, IRC_VOLUME_DN, IRC_MUTE, IRC_WOOFER_VOL_UP, IRC_WOOFER_VOL_DOWN, IRC_POWER, IRC_SRC, IRC_3D, 0xFF}; // the key cmd we support

// UPD6121 key cmd table
//const UINT8 KeyCmd[] = {IRC_VOLUME_UP, IRC_VOLUME_DN, IRC_MUTE, 0xFF}; // the key cmd we support

/*---------------------------------------------------------------------------*
    cfg structure
*---------------------------------------------------------------------------*/

//
stIR_KeyTbl_T gstIRCfg_TC9012 =
{
	.IR_CusId = CusID_TC9012,
	.pIR_KeyCode = {KeyCode_0x0707, KeyCode_0x2c2c},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_UPD6121 =
{
	.IR_CusId = CusID_UPD6121,
	.pIR_KeyCode = {KeyCode_0x04FB, KeyCode_0x7108, KeyCode_0x837A, KeyCode_0x609F, KeyCode_0x00FF, KeyCode_0x15EA, KeyCode_0x28D7, KeyCode_0x40BF, KeyCode_0x38C7, KeyCode_0x7F, KeyCode_0x4BA, KeyCode_0xB34C},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_M50560 =
{
	.IR_CusId = CusID_M50560,
	.pIR_KeyCode = {KeyCode_0x0002},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_MATC6D6 =
{
	.IR_CusId = CusID_MATC6D6,
	.pIR_KeyCode = {KeyCode_MATC6D6_0x0001},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_SONYD7C5 =
{
	.IR_CusId = CusID_SONYD7C5,
	.pIR_KeyCode = {KeyCode_0x01},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_SONYD7C8 =
{
	.IR_CusId = CusID_SONYD7C8,
	.pIR_KeyCode = {KeyCode_0x54},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_AHEA48 =
{
	.IR_CusId = CusID_AHEA48,
	.pIR_KeyCode = {KeyCode_0x00802002, KeyCode_0x00a02002},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_MATC5D6 =
{
	.IR_CusId = CusID_MATC5D6,
	.pIR_KeyCode = {KeyCode_0x001F},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_RC6 =
{
	.IR_CusId = CusID_RC6,
	.pIR_KeyCode = {KeyCode_0x00,KeyCode_0x10},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_SHARP =
{
	.IR_CusId = CusID_SHARP,
	.pIR_KeyCode = {KeyCode_SHARP_0x0001},
	.pIR_KeyCmd = KeyCmd,
};

stIR_KeyTbl_T gstIRCfg_SAMSUNG =
{
	.IR_CusId = CusID_SAMSUNG,
	.pIR_KeyCode = {KeyCode_SAMSUNG_0x0001},
	.pIR_KeyCmd = KeyCmd,
};


static const stIR_KeyTbl_T *gpstIRConfigTbl[IR_FMT_MAX] =
{
	&gstIRCfg_TC9012,
	&gstIRCfg_UPD6121,
	&gstIRCfg_M50560,
	&gstIRCfg_MATC6D6,
	&gstIRCfg_SONYD7C5,
	&gstIRCfg_SONYD7C8,
	&gstIRCfg_AHEA48,
	&gstIRCfg_MATC5D6,
	&gstIRCfg_RC6,
	&gstIRCfg_SHARP,
	&gstIRCfg_SAMSUNG,
};


/*---------------------------------------------------------------------------*
 *                          FUNCTION   DECLARATIONS                          *
 *---------------------------------------------------------------------------*/

stIR_KeyTbl_T const *GetIR_Tbl(int IR_Config)
{
	if (IR_Config >= IR_FMT_MAX)
		return NULL;

	return (stIR_KeyTbl_T *)gpstIRConfigTbl[IR_Config];
}

