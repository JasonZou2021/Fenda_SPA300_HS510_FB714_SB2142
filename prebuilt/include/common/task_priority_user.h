#ifndef __USER_TASK_PRIORITY_H__
#define __USER_TASK_PRIORITY_H__

typedef enum USER_TASK_PRIORITY_e
{
	/*Priority 0-2 defined in system_task_priority.h,they are should be serial number with here*/
	PRIO_USER_LV_L = 3,
	PRIO_USER_LV_1 = 4,
	PRIO_USER_LV_2 = 5,
	PRIO_USER_LV_3 = 6,
	PRIO_USER_LV_H = 7,
	PRIO_USER_LV_H3 = 23
					 /*Priority 8-31 defined in system_task_priority.h,they are should be serial number with here*/
} USER_TASK_PRIORITY_t;

#endif

