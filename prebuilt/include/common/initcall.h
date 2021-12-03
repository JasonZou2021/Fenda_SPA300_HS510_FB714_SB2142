/******************************************************************************/
/**
*
* \file	 initcall.h
*
* \brief	define initcall type & macros here.
*
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author     clhuang@sunplus.com
*\version	  v0.01
*\date 	  2016/05/20
******************************************************************************/
#ifndef __INITCALL_H__
#define __INITCALL_H__

/**
*\brief     Driver fs Struct
*/
#define DEVICENAMELEN  16
#define DEVICEREADY    0
#define DEVICEOPENED   1
#define DEVICEUNINIT   0xff
#define DEVICEINITING  0xfe

#define MULTYOPEN      0
#define ONLYONCEOPEN   1

#define FAST_CONNECT_CARD
//#define CONFIG_USB_LOGO_TEST

typedef struct _MODULE_DEV
{
    const char abDevName[DEVICENAMELEN];
    int Devfs_dirop;
    int DevState;
    int DevOpenFlag;
    void *pData;
} MODULE_DEV;

typedef enum SECTIONS_E
{
	SECTION_DRIVER = (1 << 0),
	SECTION_AO = (1 << 1),
	SECTION_CMD = (1 << 2),
	SECTION_TEST = (1 << 3),
	SECTION_POWERON = (1 << 4)
} SECTIONS_T;

/**
*\brief     module init define
*/
typedef void (*PINITCALL_T)(void);

#define __DEFINE_DEVICE(st, level) \
	MODULE_DEV* __initdevice_##st \
	__attribute__((__unused__)) \
	__attribute__((__section__(".__init." level))) = (&st)

#define DEVICE_INIT(x) __DEFINE_DEVICE(x, "device")

#define __DEFINE_INITCALL(fn, level) \
	PINITCALL_T __initcall_##fn \
	__attribute__((__unused__)) \
	__attribute__((__section__(".__init." level))) = (PINITCALL_T)fn

#define DRIVER_INIT(x)			__DEFINE_INITCALL(x, "driver")
#define AO_INIT(x)				__DEFINE_INITCALL(x, "ao")
#define AO_INIT_SYNC(x)			__DEFINE_INITCALL(x, "ao_sync")
#define AO_INIT_ARG(x)			__DEFINE_INITCALL(x, "aoarg")
#define IDLE_HOOK(x)			__DEFINE_INITCALL(x, "idlehook")
#define EXCEPTON_DUMP_HOOK(x)	__DEFINE_INITCALL(x, "edhook")
#define POWERON_INIT(x)			__DEFINE_INITCALL(x, "poweron")
#define PRIORITY_MALLOC(x)		__DEFINE_INITCALL(x, "priomalloc")

#define CMD_INIT(x) void Regiter##x(void)	\
	{						\
		FreeRTOS_CLIRegisterCommand(&x);	\
	}						\
	__DEFINE_INITCALL(Regiter##x, "cmd");

#define TEST_INIT(x)		__DEFINE_INITCALL(x, "test")

void InitcallInitialize(int dEnableSection);

#endif /* __INITCALL_H__ */
