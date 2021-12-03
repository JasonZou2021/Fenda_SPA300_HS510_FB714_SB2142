#ifndef __TOUCH_AO__
#define __TOUCH_AO__


#include "qpc.h"

#define STANDBY	1
#define NORMAL	0

typedef int (*touch_func)(int);
typedef int (*touch_poll)(void);
typedef struct _touch_func
{
	touch_func init;
	touch_func standby;
	touch_poll polling;
	touch_func version;
	
}TOUCH_FUNC;


typedef struct
{
	QEvt stEvt;
	int sig;
	int type;
	int key;

}TouchCallEvt;


typedef struct
{
	QActive super;
	QTimeEvt timeEvtTouch;
} TouchSrv;


extern TOUCH_FUNC ET8L10C_touch;

QActive *TouchApp_get(void);
int TouchKey_Init(void);
int TouchKey_StandbyInit(void);
int TouchKey_Polling(void);
int TouchKey_GetVersion(void);

typedef enum
{
	SP_TOUCH,
	SP_HDMI,
	
}eStandbyPoll;


#endif

