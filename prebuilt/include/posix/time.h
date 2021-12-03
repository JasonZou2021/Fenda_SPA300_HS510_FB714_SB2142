/******************************************************************************/
/**
*
* \file	 time.h
*
* \brief	POSIX time implementation .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/06/06
******************************************************************************/
#ifndef __POSIX_TIME_H__
#define __POSIX_TIME_H__


#include <sys/types.h>
#include <sys/../time.h>  //we did this because if when we #include<time.h> which belong to the libc the compiler
//always find the time.h in posix first. Sometimes, we want to use the libc time.h,
//we can then only include time.h in posix
#include "libc/time.h"

/**
 * \brief The defalt value of the expiration time
 */
#define TIMER_DEFAULT_EXPIRED_SEC	100

/**
 * \brief The IDs of the various system clocks (for POSIX.1b interval timers):
 */
#ifndef CLOCK_REALTIME
	#define CLOCK_REALTIME			0
#endif
#define CLOCK_MONOTONIC			1
#define CLOCK_PROCESS_CPUTIME_ID	2
#define CLOCK_THREAD_CPUTIME_ID		3
#define CLOCK_MONOTONIC_RAW		4
#define CLOCK_REALTIME_COARSE		5
#define CLOCK_MONOTONIC_COARSE		6
#define CLOCK_BOOTTIME			7
#define CLOCK_REALTIME_ALARM		8
#define CLOCK_BOOTTIME_ALARM		9

/**
 *  \brief  The IDs of various hardware clocks:
 */
#define CLOCK_SGI_CYCLE			10
#define MAX_CLOCKS			16
#define CLOCKS_MASK			(CLOCK_REALTIME | CLOCK_MONOTONIC)
#define CLOCKS_MONO			CLOCK_MONOTONIC


/**
 *   \brief  sigevent definitions
 *
 * It seems likely that SIGEV_THREAD will have to be handled from
 * userspace, pthread transmuting it to SIGEV_SIGNAL, which the
 * thread manager then catches and does the appropriate nonsense.
 * However, everything is written out here so as to not get lost.
 */
#define SIGEV_SIGNAL	0	/* notify via signal */
#define SIGEV_NONE	1	/* other notification: meaningless */
#define SIGEV_THREAD	2	/* deliver via thread creation */
#define SIGEV_THREAD_ID 4	/* deliver to thread */

typedef int   clockid_t;
typedef unsigned int   timer_t;


union sigval            /* Data passed with notification */
{
	int     sival_int;         /* Integer value */
	void   *sival_ptr;         /* Pointer value */
};

#ifndef __time_t_defined
typedef _TIME_T_ time_t;
#define __time_t_defined

/* Time Value Specification Structures, P1003.1b-1993, p. 261 */

struct timespec
{
	time_t  tv_sec;   /* Seconds */
	long    tv_nsec;  /* Nanoseconds */
};

struct itimerspec
{
	struct timespec  it_interval;  /* Timer period */
	struct timespec  it_value;     /* Timer expiration */
};
#endif

/**
*\brief The  sigevent  structure is used by various APIs to describe the way  Provide a default struct describe the way a process is to be notified
* about an event (e.g., completion of an asynchronous request, expiraion of a timer, or the arrival of a message).
*/
struct sigevent
{
	int          sigev_notify; /* Notification method */
	int          sigev_signo;  /* Notification signal */
	union sigval sigev_value;  /* Data passed with
				 notification */
	void       (*sigev_notify_function) (union sigval);
	/* Function used for thread
	   notification (SIGEV_THREAD) */
	void        *sigev_notify_attributes;
	/* Attributes for notification thread
	   (SIGEV_THREAD) */
};

typedef struct TimerControlBlock
{
	timer_t timerid;
	char owner[24];
	unsigned int tickOnce;
	unsigned int tickPeriod;
	unsigned char needChangePeriod;
	unsigned int  newTicks;
	unsigned char needAutoReload;
	void (*sigev_notify_function) (union sigval);
} TimerControlBlock_t;

/******************************************************************************************/
/**
 * \fn		int nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
 *
 * \brief		pthread sleep time
 *
 * \param		rqtp : (Input) sleep time struct point
 * \param		rmtp : (Input/out) NULL
 *
 * \return		0: OK.  -1: Fail
 *
 ******************************************************************************************/
int nanosleep(const struct timespec *rqtp, struct timespec *rmtp);

/******************************************************************************************/
/**
 * \fn		int clock_gettime(clockid_t clock_id, struct timespec *tp)
 *
 * \brief		clock and timer functions
 *
 * \param		clock_id : (Input) only for CLOCK_REALTIME
 * \param		tp : (Output) time struct point
 *
 * \return		0: OK.  -1: Fail
 *
 ******************************************************************************************/
int clock_gettime(clockid_t clock_id, struct timespec *tp);

/******************************************************************************************/
/**
 * \fn		int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid)
 *
 * \brief		 Create a timer based on the given clock.
 *
 * \param		clockid : (Input) specifies the clock that the new timer uses to measure time.  It can be specified as one of the following values:
 *                                                       CLOCK_REALTIME,CLOCK_MONOTONIC,CLOCK_PROCESS_CPUTIME_ID,CLOCK_THREAD_CPUTIME_ID
 * \param		sevp : (Input) points to a sigevent structure that specifies how the caller should be notified when the timer expires.
 *		           For the definition and general details  of this structure, see sigevent.
  * \param		timerid : (Input) The ID of the new timer is returned in the buffer pointed to by timerid.
 *
 * \return	\e 0 on success. \n
 *            \e EINVAL The value specified for the argument is not correct. \n
 *            \e ENOMEM The system cannot allocate the resources required to create the mutex.\n
 *
 * \sa 		timer_delete ()
 *
 ******************************************************************************************/
int timer_create(clockid_t clockid, struct sigevent *sevp, timer_t *timerid);

/******************************************************************************************/
/**
 * \fn		int timer_delete(timer_t timerid)
 *
 * \brief		deletes the timer whose ID is given in timerid
 *
 * \param	timerid : (Input) point to timer be destroyed
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EBUSY The mutex is currently owned by another thread. \n
 *
 * \sa 		timer_create ()
 *
 ******************************************************************************************/
int timer_delete(timer_t timerid);


/******************************************************************************************/
/**
 * \fn		int timer_settime(timer_t timerid, int flags, const struct itimerspec *new_value, struct itimerspec * old_value)
 *
 * \brief		arm/disarm  the timer identified by timerid and then start (reset) the timer
 *
 * \param	timerid : (Input) point to timer to be arm
 * \param	flags : (Input) point to timer to be arm
 * \param	new_value : (Input) point to structure itimerspec that specifies the new initial value and the new
 *                                    interval for the timer
 * \param	old_value : (Input) If old_value is not NULL, then it returns the previous interval of the timer (in old_value->it_interval) and the amount of time until the timer would previously have
 *                                   next expired (in old_value->it_value)
 *
 * \return		\e 0 on success. \n
 *            	                  \e ERECURSE The recursive mutex cannot be recursively locked again. \n
 *
 * \sa 		timer_gettime ()
 *
 ******************************************************************************************/

int timer_settime(timer_t timerid, int flags,
				  const struct itimerspec *new_value,
				  struct itimerspec *old_value);

void *timer_parameter(timer_t *timerid);

#endif //__POSIX_TIME_H__

