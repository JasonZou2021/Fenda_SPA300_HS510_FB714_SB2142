/******************************************************************************/
/**
*
* \file	 semaphore.h
*
* \brief	This file contains the implementation of the POSIX semaphore functions. .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*
* \author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __POSIX_SEMAPHORE_H__
#define __POSIX_SEMAPHORE_H__
#include <time.h>

/**
*\brief semaphone struct
*/
typedef struct _sem_t
{
	void *ptSem;	/*!< for FreeRTOS Semaphone Handle*/
} sem_t;

/******************************************************************************************/
/**
 * \fn		 int sem_init(sem_t *sem, int shared , unsigned int value)
 *
 * \brief		 initializes an unnamed semaphore and sets its initial value
 *
 * \param		sem : (Input) A pointer to the storage of an uninitialized unnamed semaphore. This semaphore is initialized
 * \param		shared : (Input)An indication to the system of how the semaphore is going to be used
 * \param		value : (Input) The value used to initialize the value of the semaphore
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e ENOSPC No space available. \n
 *
 * \sa 		sem_destroy ()
 *
 ******************************************************************************************/
int sem_init(
	sem_t         *sem,
	int            shared,
	unsigned int   value
);

/******************************************************************************************/
/**
 * \fn		 int sem_destroy(sem_t *sem)
 *
 * \brief		destroys an unnamed semaphore
 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EBUSY Resource busy. \n
 *
 * \sa 		sem_destroy ()
 *
 ******************************************************************************************/
int sem_destroy(
	sem_t *sem
);

/******************************************************************************************/
/**
 * \fn		 int sem_wait(sem_t *sem)
 *
 * \brief		decrements by one the value of the semaphore
 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		sem_post ()
 *
 ******************************************************************************************/
int sem_wait(
	sem_t *sem
);

/******************************************************************************************/
/**
 * \fn		 int sem_timedwait(sem_t *sem, const struct timespec *abstime)
 *
 * \brief		 lock a semaphore

 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 * \param		abstime : (Input) timeout time value
 *
 * \return		\e 0 on success. \n
 *                  \e EINVAL the abstime parameter specified a nanoseconds field value less than zero
 *                        or greater than or equal to 1000 million.. \n
 *
 * \sa 		sem_post ()
 *
 ******************************************************************************************/
int sem_timedwait(sem_t *sem, const struct timespec *abstime);


/******************************************************************************************/
/**
 * \fn		 int sem_trywait(sem_t *sem)
 *
 * \brief		 attempts to decrement the value of the semaphore
 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *            	                  \e EAGAIN Operation would have caused the process to be suspended. \n
 *
 * \sa 		sem_post ()
 *
 ******************************************************************************************/
int sem_trywait(
	sem_t *sem
);

/******************************************************************************************/
/**
 * \fn		 int sem_post(sem_t *sem)
 *
 * \brief		posts to a semaphore, incrementing its value by one
 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		sem_wait ()  sem_trywait()
 *
 ******************************************************************************************/
int sem_post(
	sem_t  *sem
);

/******************************************************************************************/
/**
 * \fn		 int sem_getvalue(sem_t *sem, int *sval)
 *
 * \brief		 get the value of a semaphore
 *
 * \param		sem : (Input) A pointer to an initialized unnamed semaphore
 * \param		sval : (Output) timeout time value
 *
 * \return		\e 0 on success. \n
 *                  \e other fail. \n
 *
 *
 ******************************************************************************************/
int sem_getvalue(sem_t *sem, int *sval);

#endif //__POSIX_SEMAPHORE_H__

