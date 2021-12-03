#ifndef _UART_APP_h

#define _UART_APP_h

#include "qp_pub.h"

#define UART_APP_DBG


#ifdef UART_APP_DBG
#define uartapp_printf1(f, a...)  LOG(DISP,1 , f, ##a)
#define uartapp_printf2(f, a...)  LOG(DISP, 2, f, ##a)
#define uartapp_printf_E(f, a...) LOG(ERR, 1, "[DISP][%s][%d]"f, __FUNCTION__, __LINE__, ##a)
#else
#define uartapp_printf1(f, a...)  ((void)0)
#define uartapp_printf2(f, a...)  ((void)0)
#define uartapp_printf_E(f, a...) ((void)0)
#endif


typedef struct _UartApp_Func
{
	int (*UartApp_Init)(QActive *);
	int (*UartApp_read)(QActive *, unsigned char *buf, unsigned int size);
	int (*UartApp_polling)(QActive *);
	int (*UartApp_Key)(QActive *, int, int);
	int (*UartApp_SetHandle)(QActive *, int);

}UartApp_Func;


enum UARTAPP_DEVICE
{
	UARTAPP_UART0,
	UARTAPP_UART1,
	UARTAPP_UART2,
	UARTAPP_MAX,
};


typedef struct
{
	QEvt stEvt;
	int key;
	int src;
} UartKeyEvt;

typedef struct
{
	QEvt stEvt;
	int sig;
	int para;

}UartCallEvt;


QActive *UartApp_get(void);
int UartApp_SetKey_Sig(void *  sender,int src,  int sig);
int UartApp_Display(int Str_ID);
int UartApp_DisplayBlink(int Str_ID , uint16_t bright_time ,uint16_t dark_time);

#define BT_UPG_OK	0XFF
#define BT_UPG_NG	0XFE

#define portMAX_DELAY 10

typedef struct 
{
	QEvt stEvt;
	BYTE vol;
	BYTE mute;
} BTEvt;

enum
{
	UART_CTL_NULL,
	UART_CTL_ENTRY,
	UART_CTL_EXIT,	
	UART_CTL_SRC,
	UART_CTL_PLAY,
};

extern UartApp_Func BT_ATS2819;

#endif

