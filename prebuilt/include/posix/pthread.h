/******************************************************************************/
/**
*
* \file	 pthread.h
*
* \Descriptions:  This header contains all the definitions needed to support
*                        pthreads under eCos. The reader is referred to the POSIX
*	       standard or equivalent documentation for details of the
*	       functionality contained herein.
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __POSIX_PTHREAD_H__
#define __POSIX_PTHREAD_H__
#include <stdlib.h>

#include "sched.h"

/**
*\brief  Values for detachstate
*/
#define PTHREAD_CREATE_JOINABLE	        (1)
#define PTHREAD_CREATE_DETACHED	        (2)

/**
*\brief  Values for scope
*/
#define PTHREAD_SCOPE_SYSTEM            (1)
#define PTHREAD_SCOPE_PROCESS           (2)

/**
*\brief  Values for inheritsched
*/
#define PTHREAD_INHERIT_SCHED           (1)
#define PTHREAD_EXPLICIT_SCHED          (2)
#define PTHREAD_DEFALT_STACK          (1024)

// Initializer for pthread_once_t instances
#define PTHREAD_ONCE_INIT       (0)

// cancelation define
#define PTHREAD_CANCEL_ENABLE           (0)
#define PTHREAD_CANCEL_DISABLE          (1)
#define PTHREAD_CANCEL_DEFERRED         (0)
#define PTHREAD_CANCEL_ASYNCHRONOUS     (1)

#ifndef THREAD_GETMEM
	#define THREAD_GETMEM(descr, member) descr->member
#endif
#ifndef THREAD_GETMEM_NC
	#define THREAD_GETMEM_NC(descr, member) descr->member
#endif
#ifndef THREAD_SETMEM
	#define THREAD_SETMEM(descr, member, value) descr->member = (value)
#endif
#ifndef THREAD_SETMEM_NC
	#define THREAD_SETMEM_NC(descr, member, value) descr->member = (value)
#endif

typedef void *pthread_t;
typedef int pthread_key_t;
typedef int pthread_once_t;

/**
*\brief sched_param struct
*/
struct sched_param
{
	int sched_priority;	  /*!< FreeRTOS task priority*/
};

/**
*\brief pthread attribute struct
*/
typedef struct pthread_attr_t
{
	unsigned int        detachstate: 2,
			 scope: 2,
			 inheritsched: 2,
			 schedpolicy: 2,
			 stackaddr_valid: 1,
			 stacksize_valid: 1;
	struct sched_param  schedparam;
	void                *stackaddr;
	size_t              stacksize;
} pthread_attr_t;

typedef struct pthread_descr_struct *pthread_descr;
struct pthread_descr_struct
{
	pthread_t p_tid;            /* Thread identifier */
	//char p_terminated;          /* true if terminated e.g. by pthread_exit */
	//char p_exited;              /* true if the assoc. process terminated */
	//char p_detached;            /* true if detached */
	//pthread_descr p_joining;    /* thread joining on that thread or NULL */
	char p_cancelstate;         /* cancellation state */
	char p_canceltype;          /* cancellation type (deferred/async) */
	char p_canceled;            /* cancellation request pending */
	//char p_woken_by_cancel;     /* cancellation performed wakeup */
};


/******************************************************************************************/
/**
 * \fn		 int pthread_create(pthread_t *pthread,  const pthread_attr_t *attr,  void *(*start_routine) (void *),  void *arg)
 *
 * \brief		Thread creation and management.
 *
 * \param		pthread : (Output) Pthread handle to the created thread .
 * \param 		attr:(Input) The thread attributes object containing the attributes to be associated with the newly created thread. If NULL, the default thread attributes are used.
 * \param		start_routine:(Input) The function to be run as the new threads start routine
 * \param		arg:(Input) An address for the argument for the threads start routine
 *
 * \return		\e 0 on success. \n
 *            	                     \e EINVAL The value specified for the argument is not correct. \n
 *		\e EAGAIN The system did not have enough resources to create another thread . \n
 *		\e EBUSY The system cannot allow thread creation in this process at this time. . \n
 *
 * \sa 		pthread_exit ()
 *
 ******************************************************************************************/
int pthread_create (pthread_t *pthread,
					const pthread_attr_t *attr,
					void *(*start_routine) (void *),
					void *arg);

/******************************************************************************************/
/**
 * \fn		pthread_t pthread_self (void)
 *
 * \brief		get the calling thread ID.
 *
 * \param		none
 *
 * \return		 the Pthread handle of the calling thread
 *
 ******************************************************************************************/
pthread_t pthread_self (void);

/******************************************************************************************/
/**
 * \fn		pthread_t pthread_self (void)
 *
 * \brief		get the calling thread ID.
 *
 * \param		t1 : (in) Pthread handle to the created thread .
 * \param		t2 : (in) Pthread handle to the created thread .
 *
 * \return		 a non-zero value if t1 and t2 are equal, otherwise, zero shall be returned.
 *
 ******************************************************************************************/
int pthread_equal(pthread_t t1, pthread_t t2);

/******************************************************************************************/
/**
 * \fn		void pthread_exit(void *value_ptr)
 *
 * \brief		terminate the calling thread
 *
 * \param		value_ptr : (Input) exit status of the thread
 *
 * \return		pthread_exit() does not return.
 *
 * \sa 		pthread_create ()
 *
 ******************************************************************************************/
void pthread_exit(void *value_ptr);

/******************************************************************************************/
/**
 * \fn		int pthread_cancel(pthread_t thread)
 *
 * \brief		terminate the thread
 *
 * \param		pthread : (in) Pthread handle to the created thread .
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_cancel(pthread_t thread);

/******************************************************************************************/
/**
 * \fn		int pthread_join(pthread_t thread, void **value_ptr)
 *
 * \brief		wait for thread termination
 *
 * \param		thread : (in) Pthread handle to the created thread .
 * \param		value_ptr : (output) exit status of the thread
 *
 * \return		0 : success  other: fail
  *              \e EDEADLK Current thread try to do the operation on themselves. \n
 *
 ******************************************************************************************/
int pthread_join(pthread_t thread, void **value_ptr);
/******************************************************************************************/
/**
 * \fn      int pthread_detach(pthread_t thread);
 *
 * \brief      marks the thread identified by thread as detached.
 *
 * \param       thread : (Input) Thread id used to identify one thread.
 *
 * \return      \e 0 on success. \n
 *                                \e EINVAL The value specified for the argument is not correct. \n
 *                                \e ENOTLOCKED The mutex associated with the condition variable is not locked \n
 *
 * \sa      pthread_cond_wait()
 *
 ******************************************************************************************/
int pthread_detach(pthread_t thread);

/******************************************************************************************/
/**
 * \fn		void pthread_cleanup_push(void (*cleanUpRoutine)(void *) , void *arg)
 *
 * \brief		establish cancellation handlers
 *
 * \param		cleanUpRoutine : (in) thread Clean up function .
 * \param		arg : (in) thread clean up function parameter
 *
 * \return		pthread_cleanup_push does not return.
 *
 ******************************************************************************************/
void pthread_cleanup_push(void (*cleanUpRoutine)(void *), void *arg);

/******************************************************************************************/
/**
 * \fn		void pthread_cleanup_push(void (*cleanUpRoutine)(void *) , void *arg)
 *
 * \brief		establish cancellation handlers
 *
 * \param		excute : (in) optionally invoke clean up function  .
 *
 * \return		pthread_cleanup_pop does not return.
 *
 ******************************************************************************************/
void pthread_cleanup_pop(int excute);

/******************************************************************************************/
/**
 * \fn		int pthread_once(pthread_once_t *once_control,void (*init_routine)(void))
 *
 * \brief		dynamic package initialization
 *
 * \param		once_control : (in) control variable.
 * \param		init_routine : (in) init function.
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_once(pthread_once_t *once_control, void (*init_routine)(void));

/******************************************************************************************/
/**
 * \fn		int pthread_key_create(pthread_key_t *key, void (*destructor)(void*))
 *
 * \brief		thread-specific data key creation
 *
 * \param		key : (out) a thread-specific data key .
 * \param		destructor : (in) destructor function .
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_key_create(pthread_key_t *key, void (*destructor)(void *));

/******************************************************************************************/
/**
 * \fn		int pthread_key_delete (pthread_key_t key)
 *
 * \brief		thread-specific data key deletion
 *
 * \param		key : (in) a thread-specific data key .
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_key_delete (pthread_key_t key);

/******************************************************************************************/
/**
 * \fn		int pthread_setspecific (pthread_key_t key, const void *value)
 *
 * \brief		thread-specific data management
 *
 * \param		key : (in) a thread-specific data key .
 * \param		value : (in) a thread-specific value .
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_setspecific (pthread_key_t key, const void *value);

/******************************************************************************************/
/**
 * \fn		void *pthread_getspecific(pthread_key_t key)
 *
 * \brief		thread-specific data management
 *
 * \param		key : (in) a thread-specific data key .
 *
 * \return		success: thread-specific value
 *
 ******************************************************************************************/
void *pthread_getspecific(pthread_key_t key);

/******************************************************************************************/
/**
 * \fn			int pthread_setschedparam (pthread_t *pthread, int policy, const struct sched_param *param)
 *
 * \brief		set scheduling policy and parameters for the thread
 *
 * \param		pthread : (in) Pthread handle to the created thread .
 * \param		policy : (Input) The supported values of policy shall include SCHED_FIFO, SCHED_RR, and SCHED_OTHER
 * \param		param : (Input) Address of the variable containing the scheduling parameters
 *
 * \return		0 : success  other: fail
 *
 ******************************************************************************************/
int pthread_setschedparam (pthread_t *pthread, int policy, const struct sched_param *param);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_init (pthread_attr_t *attr)
 *
 * \brief		initializes a thread attributes object to the default thread attributes
 *
 * \param		attr : (Input/Output) The address of the thread attributes object to be initialized
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		pthread_create ()  pthread_attr_destroy()
 *
 ******************************************************************************************/
int pthread_attr_init (pthread_attr_t *attr);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_init (pthread_attr_t *attr)
 *
 * \brief		destroys a thread attributes object
 *
 * \param		attr : (Input) The address of the thread attributes object to be destroyed
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL Invalid Argument Specified  \n
 *
 * \sa 		pthread_create ()  pthread_attr_init()
 *
 ******************************************************************************************/
int pthread_attr_destroy (pthread_attr_t *attr);

/******************************************************************************************/
/**
 * \fn			int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate)
 *
 * \brief		set the detachstate attribute
 *
 * \param		attr : (Input) The address of the thread attributes object to be destroyed
 * \param		detachstate : (Input) the mode PTHREAD_CREATE_JOINABLE or PTHREAD_CREATE_DETACHED
 *
 * \return		\e 0 on success. \n
 *            	\e EINVAL Invalid Argument Specified  \n
 ******************************************************************************************/
int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize)
 *
 * \brief		set the stacksize attribute
 *
 * \param		attr : (Input) The address of the thread attributes object
 * \param		stacksize : (Input) the minimum stack size (in bytes) allocated for the created threads stack.
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value of stacksize is less than {PTHREAD_STACK_MIN} or exceeds a system-imposed limit.\n
 *
 * \sa 		pthread_attr_getstacksize()
 *
 ******************************************************************************************/
int pthread_attr_setstacksize (pthread_attr_t *attr, size_t stacksize);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy)
 *
 * \brief		set the schedpolicy attribute (REALTIME THREADS)
 *
 * \param		attr : (Input) The address of the thread attributes object
 * \param		policy : (Input) The supported values of policy shall include SCHED_FIFO, SCHED_RR, and SCHED_OTHER
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value of policy is not valid  \n
 *            	                  \e ENOTSUP An attempt was made to set the attribute to an unsupported value \n
 *
 * \sa 		pthread_attr_getschedpolicy()
 *
 ******************************************************************************************/
int pthread_attr_setschedpolicy (pthread_attr_t *attr, int policy);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_setschedparam (pthread_attr_t *attr, const struct sched_param *param)
 *
 * \brief		sets the scheduling parameters in the thread attributes object
 *
 * \param		attr : (Input) The address of the thread attributes object
 * \param		param : (Input) Address of the variable containing the scheduling parameters
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value of policy is not valid  \n
 *            	                  \e ENOTSUP An attempt was made to set the attribute to an unsupported value \n
 *
 * \sa 		pthread_attr_getschedpolicy()
 *
 ******************************************************************************************/
int pthread_attr_setschedparam (pthread_attr_t *attr, const struct sched_param *param);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_setthreadname (pthread_attr_t *attr, const char *param);
 *
 * \brief		sets the thread name for debug use
 *
 * \param		attr : (Input) The address of the thread attributes object
 * \param		param : (Input) the thread name buffer address point
 *
 * \return		\e 0 on success. \n
 *            	\e other: fail  \n
 *
 ******************************************************************************************/
int pthread_attr_setthreadname (pthread_attr_t *attr, const char *param);

/******************************************************************************************/
/**
 * \fn		int pthread_attr_getthreadname (pthread_t thread, char *name);
 *
 * \brief		get the thread name for debug use
 *
 * \param		thread : (Input) The id of current thread.
 * \param		name : (Output) The name of current thread.
 *
 * \return		\e 0 on success. \n
 *            	\e other: fail  \n
 *
 * \Note:	The length of buffer name should always >= configMAX_TASK_NAME_LEN
 *
 ******************************************************************************************/
int pthread_attr_getthreadname (pthread_t thread, char *name);


/******************************************************************************************/
/**
 * \fn			int sched_get_priority_max(int policy)
 *
 * \brief		get OS Max priority
 *
 * \param		policy : (Input) The supported values of policy shall include SCHED_FIFO, SCHED_RR, and SCHED_OTHER
 *
 * \return		Max priority
 *
 ******************************************************************************************/
int sched_get_priority_max(int policy);

/******************************************************************************************/
/**
 * \fn			int sched_get_priority_min(int policy)
 *
 * \brief		get OS Min priority
 *
 * \param		policy : (Input) The supported values of policy shall include SCHED_FIFO, SCHED_RR, and SCHED_OTHER
 *
 * \return		min priority
 *
 ******************************************************************************************/
int sched_get_priority_min(int policy);

/******************************************************************************************/
/**
 * \fn          int pthread_setcancelstate(int state, int *oldstate)
 *
 * \brief       set the calling thread's cancelability state to the indicated state
 *              and return the previous cancelability state at the location referenced by oldstate
 *
 * \param       state : (Input) the cancel state to be setted
 *              oldstate: (Output) the old cancel state.
 *
 * \return      0 on sucess, -1 on error.
 *              \e EINVAL The value specified for the argument is not correct. \n
 *
 *
 ******************************************************************************************/
int pthread_setcancelstate(int state, int *oldstate);

/******************************************************************************************/
/**
 * \fn          pthread_setcanceltype(int type, int *oldtype)
 *
 * \brief       set the calling thread's cancelability type to the indicated type
 *              and return the previous cancelability type at the location referenced by oldtype
 *
 * \param       type : (Input) the cancel type to be setted
 *              oldtype: (Output) the old cancel type.
 *
 * \return      0 on sucess, -1 on error.
 *              \e EINVAL The value specified for the argument is not correct. \n
 *
 *
 ******************************************************************************************/
int pthread_setcanceltype(int type, int *oldtype);

/******************************************************************************************/
/**
 * \fn          void pthread_testcancel(void)
 *
 * \brief       test wheather the calling thread was cancelled, if cancelled, do exit.
 *
 * \param       None
 *
 * \return      NOne
 *
 *
 ******************************************************************************************/
void pthread_testcancel(void);

/******************************************************************************************/
/**
 * \fn             pthread_descr thread_self(void)
 *
 * \brief       Return pthread description of the calling thread.
 *
 * \param       param:(Input) void.
 *
 * \return      the pthread_descr of the calling thread. NULL on error.
 *
 ******************************************************************************************/
pthread_descr thread_self(void);

/******************************************************************************************/
/**
 * \fn		void PosixWrapperInit(void)
 *
 * \brief		Init Posix global value;
 *
 * \param		no
 *
 * \return		no
 *
 ******************************************************************************************/
void PosixWrapperInit(void);

#endif //__POSIX_PTHREAD_H__

