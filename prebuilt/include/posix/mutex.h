/******************************************************************************/
/**
*
* \file	 mutex.h
*
* \brief	POSIX pthread implementation .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __POSIX_MUTEX_H__
#define __POSIX_MUTEX_H__

#include "time.h"
#include "pthread.h"

/**
*\brief mutex struct
*/
typedef struct _pthread_mutex_t
{
	void  *ptMutex;
	pthread_t m_owner;
} pthread_mutex_t;

typedef void pthread_mutexattr_t;

/**
*\brief cond struct
*/
typedef struct _pthread_cond_t
{
	void *ptCond;
	unsigned int nWaitting;
} pthread_cond_t;

typedef void pthread_condattr_t;

/******************************************************************************************/
/**
 * \fn		int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutex_attr)
 *
 * \brief		 initializes a mutex with the specified attributes for use.
 *
 * \param		mutex : (Input) The address of the variable to contain a mutex object.
 * \param		mutex_attr : (Input) The address of the variable containing the mutex attributes object.
 *
 * \return	\e 0 on success. \n
 *            \e EINVAL The value specified for the argument is not correct. \n
 *            \e ENOMEM The system cannot allocate the resources required to create the mutex.\n
 *
 * \sa 		pthread_mutex_destroy ()
 *
 ******************************************************************************************/
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutex_attr);

/******************************************************************************************/
/**
 * \fn		int pthread_mutex_destroy (pthread_mutex_t *mutex)
 *
 * \brief		destroys the named mutex
 *
 * \param		mutex : (Input) Address of the mutex to be destroyed
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EBUSY The mutex is currently owned by another thread. \n
 *
 * \sa 		pthread_mutex_init ()
 *
 ******************************************************************************************/
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/******************************************************************************************/
/**
 * \fn		int pthread_mutex_lock (pthread_mutex_t *mutex)
 *
 * \brief		acquires ownership of the mutex specified
 *
 * \param		mutex : (Input) The address of the mutex to lock
 *
 * \return		\e 0 on success. \n
 *            	                  \e ERECURSE The recursive mutex cannot be recursively locked again. \n
 *								  \e EDEADLK Current thread try to acquire the lock again before unlock the it. \n
 *
 * \sa 		pthread_mutex_unlock ()
 *
 ******************************************************************************************/
int pthread_mutex_lock(pthread_mutex_t *mutex);

/******************************************************************************************/
/**
 * \fn		 int pthread_mutex_unlock(pthread_mutex_t *mutex)
 *
 * \brief		unlocks the mutex specified
 *
 * \param		mutex : (Input) Address of the mutex to unlock
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EPERM The mutex is not currently held by the caller. \n
 *
 * \sa 		pthread_mutex_lock()
 *
 ******************************************************************************************/
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/******************************************************************************************/
/**
 * \fn		int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr)
 *
 * \brief		 initializes a condition variable object with the specified attributes for use.
 *
 * \param		cond : (Output) The address of the condition variable to initialize .
 * \param		attr : (Input) The address of the condition attributes object to use for initialization.
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e ENOMEM The system cannot allocate the resources required to create the mutex.\n
 *
 * \sa 		pthread_cond_destroy ()
 *
 ******************************************************************************************/
int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);

/******************************************************************************************/
/**
 * \fn		int pthread_cond_destroy(pthread_cond_t *cond)
 *
 * \brief		destroys the condition variable specified by cond
 *
 * \param		cond : (Input) Address of the condition variable to destroy.
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EBUSY The condition variable was in use.\n
 *
 * \sa 		pthread_cond_init ()
 *
 ******************************************************************************************/
int pthread_cond_destroy(pthread_cond_t *cond);

/******************************************************************************************/
/**
 * \fn		int pthread_cond_signal(pthread_cond_t *cond)
 *
 * \brief		 wakes up at least one thread that is currently waiting on the condition variable specified by cond
 *
 * \param		cond : (Input) Address of the condition variable to be signaled .
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		pthread_cond_wait()
 *
 ******************************************************************************************/
int pthread_cond_signal(pthread_cond_t *cond);

/******************************************************************************************/
/**
 * \fn		int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
 *
 * \brief		blocks the calling thread, waiting for the condition specified by cond to be signaled
 *
 * \param		cond : (Input) Address of the condition variable to wait on
 * \param		mutex : (Input) Address of the mutex associated with the condition variable
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e ENOTLOCKED The mutex associated with the condition variable is not locked \n
 *
 * \sa 		pthread_cond_signal()
 *
 ******************************************************************************************/
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);

/******************************************************************************************/
/**
 * \fn      int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
 *
 * \brief       blocks the calling thread, waiting for the condition specified by cond to be signaled
 *
 * \param       cond : (Input) Address of the condition variable to wait on
 * \param       mutex : (Input) Address of the mutex associated with the condition variable
 *
 * \return      \e 0 on success. \n
 *                                \e EINVAL The value specified for the argument is not correct. \n
 *                                \e ENOTLOCKED The mutex associated with the condition variable is not locked \n
 *                                \e ETIMEDOUT The time specified by abstime to pthread_cond_timedwait() has passed. \n
 *								  \e EPERM The current thread doesn't own the specified lock \n
 *
 * \sa      pthread_cond_signal()
 *
 ******************************************************************************************/
int pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime);

/******************************************************************************************/
/**
 * \fn      int pthread_cond_broadcast(pthread_cond_t *)
 *
 * \brief      unblock threads blocked on a condition specified variable.
 *
 * \param       cond : (Input) Address of the condition variable to wait on
 *
 * \return      \e 0 on success. \n
 *                                \e EINVAL The value specified for the argument is not correct. \n
 *                                \e ENOTLOCKED The mutex associated with the condition variable is not locked \n
 *								  \e EPERM The current thread doesn't own the specified lock. \n
 *
 * \sa      pthread_cond_wait()
 *
 ******************************************************************************************/
int pthread_cond_broadcast(pthread_cond_t *cond);


#endif //__POSIX_MUTEX_H__

