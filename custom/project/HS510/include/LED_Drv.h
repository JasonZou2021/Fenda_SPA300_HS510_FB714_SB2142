#ifndef _LED_Drv_H
#define _LED_Drv_H

#include "types.h"
#include "user_def.h"
#include "hw_gpio_list.h"


//#define LED_DBG

#ifdef LED_DBG
	#define leddrv_printf1(f, a...) LOGD(f, ##a)
	#define leddrv_printf2(f, a...) LOGDT(f, ##a)
	#define leddrv_printf_E(f, a...) LOGE(f, ##a)
#else
	#define leddrv_printf1(f, a...) (void)0
	#define leddrv_printf2(f, a...) (void)0
	#define leddrv_printf_E(f, a...) (void)0
#endif

#define Led_0 (1 << 0)
#define Led_1 (1 << 1)
#define Led_2 (1 << 2)
#define Led_3 (1 << 3)
#define Led_4 (1 << 4)
#define Led_5 (1 << 5)
#define Led_6 (1 << 6)


void Set_AllLED_OFF(void);

void Set_AllLED_ON(void);

void Set_Led_Light(UINT16);

void Set_Led_Off(UINT16);


void InitLed(void);

#endif

