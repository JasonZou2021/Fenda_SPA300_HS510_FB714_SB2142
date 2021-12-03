#ifndef __VFD_H__
#define __VFD_H__


#include "types.h"

#define BIT_N(n) 			(1UL<<(n))

typedef enum
{
	VFD_7SEG_TBL,
	VFD_14SEG_TBL,	
}VFD_TBL_TYPE;

typedef struct _vfd_func
{
	UINT16 *seg_bit_tbl;
	BYTE seg_bit_tbl_size;
	int (*init)(void);
	int (*standby_init)(void);
	int (*show)(void);
	int (*set)(const char *);
	int (*dimmer)(BYTE);
	int (*dot)(UINT32, UINT32);
	UINT32 (*key)(void);
	int (*off)(void);
	
}VFD_FUNC;

typedef struct _vfd_map
{
	UINT16 *digit_tbl;
	UINT16 *char_tbl;
	UINT16 *char_lowercase_tbl;
	
}VFD_SEG_MAP;


typedef struct _vfd_cfg
{
	VFD_TBL_TYPE seg_tbl;
	VFD_FUNC *vfd_func;
	BYTE vfd_len;

}VFD_CFG;


extern VFD_FUNC vfd_sunplus;	
extern VFD_FUNC vfd_aip1944;
extern VFD_FUNC vfd_tm1640;
extern VFD_FUNC vfd_TM1628;
	
extern int vfd_init(void);
extern int vfd_show(void);
extern int vfd_set_str(const char *s);
extern int vfd_dimmer(BYTE level);
extern int vfd_dot(UINT32 p1, UINT32 p2);
extern UINT32 vfd_read_key(void);
extern int vfd_off(void);
extern int vfd_standby_init(void);

extern void aip1944_heng(void);
extern void aip1944_shu(void);
extern void aip1944_xie(void);
extern void aip1944_null(void);
extern void aip1944_Full(void);
extern void aip1944_bt_upg(void);

//  from vfd_seg_map.c 

extern UINT16 _VFD_SEG[14];

#define VFD_7SEG_SA 	_VFD_SEG[0]
#define VFD_7SEG_SB 	_VFD_SEG[1]
#define VFD_7SEG_SC 	_VFD_SEG[2]
#define VFD_7SEG_SD 	_VFD_SEG[3]
#define VFD_7SEG_SE 	_VFD_SEG[4]
#define VFD_7SEG_SF 	_VFD_SEG[5]
#define VFD_7SEG_SG 	_VFD_SEG[6]
#define VFD_7SEG_SH 	_VFD_SEG[7]
#define VFD_7SEG_SI 	_VFD_SEG[8]
#define VFD_7SEG_SJ 	_VFD_SEG[9]
#define VFD_7SEG_SK	 	_VFD_SEG[10]
#define VFD_7SEG_SL 	_VFD_SEG[11]
#define VFD_7SEG_SM 	_VFD_SEG[12]
#define VFD_7SEG_SN 	_VFD_SEG[13]

extern void vfd_set_seg_table(void);
extern VFD_SEG_MAP *vfd_get_seg_table(void);


enum
{                 // {{Added by Heaven
	DIMMER_HIGH,
	DIMMER_MIDDLE,
	DIMMER_LOW,
	DIMMER_OFF,
};


//  		7 SLEEP  15 alarm
//            7 shuffle  15 rep
//            7 all       15 A-B
//            7 hour   15 prog
//		 7 intro   15 DSC
//		 7 RDS   15 DOT
//           7 MIN     15 STEREO
//           7 TEMP   15 BT
enum
{
	ICON_SLEEP,
	ICON_ALARM,
	ICON_SHUFFLE,
	ICON_REPEAT,
	ICON_ALL,
	ICON_AB,
	ICON_HOUR,
	ICON_PROG,
	ICON_INTRO,
	ICON_DSC,
	ICON_RDS,
	ICON_DOT,
	ICON_MINUTE,
	ICON_STEREO,
	ICON_TEMP,
	ICON_BT,
	ICON_MAX,
};


#define ICON_DOT1 	(1<<0)
#define ICON_DOT2 	(1<<1)
#define ICON_RED 	(1<<2)
#define ICON_GREEN 	(1<<3)
#define ICON_BLUE 	(1<<4)


#define ICON_SET	0
#define ICON_CLEAN	1

#endif



