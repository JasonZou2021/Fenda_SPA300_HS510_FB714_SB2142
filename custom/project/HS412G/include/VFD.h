#ifndef _VFD_Drv_H
#define _VFD_Drv_H

#include "types.h"


enum
{
	VFD_DOT_DISC = 0,
	VFD_DOT_USB,
	VFD_DOT_CARD,
	VFD_DOT_IPOD,
	VFD_DOT_BT		,
	VFD_DOT_FM		,
	VFD_DOT_STEREO	,
	VFD_DOT_MUTE	,
	VFD_DOT_REPEAT,
	VFD_DOT_COLON1,  //9
	VFD_DOT_COLON2	,//10
	VFD_DOT_COLON3,
	VFD_DOT_COLON4,
	VFD_DOT_DOT1	,
	VFD_DOT_DOT2	,
	VFD_DOT_DOT3	,
	VFD_DOT_DOT4,
	VFD_DOT_DOT5,
	VFD_DOT_DOT6,
	VFD_DOT_DOT7,
	VFD_DOT_MHZ,

	VFD_DOT_PWR,
	VFD_DOT_CIRCLE1,
	VFD_DOT_CIRCLE2,
	VFD_DOT_PLAY,
	VFD_DOT_PAUSE,
	VFD_DOT_CLOCK,
	VFD_DOT_MONEY,
	VFD_DOT_DOLBY,
	VFD_DOT_TXT,
	VFD_DOT_HD,
	VFD_DOT_EPG,
	VFD_DOT_SUB,
};

enum
{
	VFD_DOT_OP_SET = 0,
	VFD_DOT_OP_CLR,
};


typedef int (*vfdop_init)(void);
typedef int (*vfdop_set_dot)( UINT32 p, UINT32 method);
typedef int (*vfdop_set_str)(const char *s);
typedef int (*vfdop_Get_Seven_Seq_Len)( void);
typedef UINT32 (*vfdop_ReadVFDKeys)(void);

typedef struct _VFDOP_S {
    vfdop_init              pfn_vfd_init;
    vfdop_set_dot           pfn_set_dot;
    vfdop_set_str           pfn_set_str;
    vfdop_Get_Seven_Seq_Len pfn_Get_Seven_Seq_Len;
    vfdop_ReadVFDKeys       pfn_ReadVFDKeys;
    void (*pfn_deinit)(void);
} VFDOP_T;

/******************************************************************************************/
/**
 * \fn		int Vfd_ReadVFDKeys(void)
 *
 * \brief	read vfd key
 *
 * \return
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_ReadVFDKeys(void);


/******************************************************************************************/
/**
 * \fn		int Vfd_Get_Seven_Seq_Len(VfdList* pstVfdList)
 *
 * \brief	get 7 sequence length
 *
 * \return
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Get_Seven_Seq_Len(void);


/******************************************************************************************/
/**
 * \fn		int Vfd_Set_Str(const char *s)
 *
 * \brief	VFD set str
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Set_Str(const char *s);


/******************************************************************************************/
/**
 * \fn		int Vfd_Set_Dot(UINT32 p, UINT32 method)
 *
 * \brief	VFD set dot
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Set_Dot(UINT32 p, UINT32 method);


/******************************************************************************************/
/**
 * \fn		int VfdInit(void)
 *
 * \brief		Init VFD Driver
 *
 * \return    \e 0 ok,other fail
 *
 * \note		none.
 *
 ******************************************************************************************/
int Vfd_Drv_Init(void);

int Vfd_Drv_Deinit(void);

#endif
