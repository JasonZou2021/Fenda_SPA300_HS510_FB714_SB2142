#ifndef __TASK_ATTRIBUTE_USER_H__
#define __TASK_ATTRIBUTE_USER_H__

#include "task_attribute.h"
#include "task_priority_user.h"

/*
* Note1:task name is combine by 3 sections:category,module,name,please be sure the total characters is less than 14
# Note2:task priority is match the value of doc\Task Priority\[8107 SDK] Task Priority.xlsx
# Note3:you shoule use 'TSK_XXXX_NAME' 'TSK_XXXX_PRIO' 'TSK_XXXX_STACK_DEPTH' to create thread
*/

/*AO userapp service atrributes*/
#define TSK_USERAPP_NAME			(TASK_NAME(USER, APP, AO))
#define TSK_USERAPP_PRIO			(PRIO_USER_LV_2)
#define TSK_USERAPP_STACK_DEPTH		(STACK_BYTE2DEPTH(10 * 1024))//(STACK_BYTE2DEPTH(2 * 1024))


/*AO Display service atrributes*/
#define TSK_DISPLAYSRV_NAME				(TASK_NAME(DISPLAY, SRV, AO))
#define TSK_DISPLAYSRV_PRIO				(PRIO_USER_LV_3)
#define TSK_DISPLAYSRV_STACK_DEPTH		(STACK_BYTE2DEPTH(5 * 512))

/*bluetooth interface config atrributes*/
#define TSK_BTAO_NAME	        (TASK_NAME(BT, SRV, AO))
#define TSK_BTAO_PRIO         (17)//(PRIO_MW_M3)
#define TSK_BTAO_STACK_DEPTH  (STACK_BYTE2DEPTH(4 * 1024))

/*AO persistence service atrributes*/
#define TSK_VASRV_NAME			(TASK_NAME(VA, SRV, AO))
#define TSK_VASRV_PRIO			(PRIO_USER_LV_3)
#define TSK_VASRV_STACK_DEPTH		(STACK_BYTE2DEPTH(4 * 1024))

/*AO HDMI service atrributes*/
#define TSK_HDMISRV_NAME			(TASK_NAME(HDMI, SRV, AO))
#define TSK_HDMISRV_PRIO			(PRIO_USER_LV_H3)
#define TSK_HDMISRV_STACK_DEPTH		(STACK_BYTE2DEPTH(3 * 1024))

/*AO persistence service atrributes*/
#define TSK_TOUCH_NAME			(TASK_NAME(TOUCH, SRV, AO))
#define TSK_TOUCH_PRIO			(PRIO_USER_LV_3)
#define TSK_TOUCH_STACK_DEPTH		(STACK_BYTE2DEPTH(4 * 1024))

/*AO persistence service atrributes*/
#define TSK_UARTAPP_NAME			(TASK_NAME(UART, SRV, AO))
#define TSK_UARTAPP_PRIO			(PRIO_USER_LV_H)
#define TSK_UARTAPP_STACK_DEPTH		(STACK_BYTE2DEPTH(4 * 1024))

#endif

