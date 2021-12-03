/******************************************************************************/
/**
*
* \file	 mqueue.h
*
* \brief	This file provides the implementation for POSIX message queues .\n
*
* \note  	Copyright (c) 2008 Sunplus Technology Co., Ltd. \n
*			All rights reserved.
*
*\author       email@sunplus.com
*\version	 v0.01
*\date 	 2016/04/15
******************************************************************************/
#ifndef __POSIX_MQUEUE_H__
#define __POSIX_MQUEUE_H__

#include <sys/types.h>

/**
*\brief  File access modes used for open() and fnctl()
*/
#define O_RDONLY     (1<<0)   /* Open for reading only */
#define O_WRONLY     (1<<1)   /* Open for writing only */
#define O_RDWR       (O_RDONLY|O_WRONLY) /* Open for reading and writing */
#define O_CREAT      (1<<3)    /* Create file it it does not exist */
#define O_EXCL       (1<<4)    /* Exclusive use */
#define O_NONBLOCK   (1<<9)    /* No delay */

#define QUEUENAMELEN (24)

/**
*\brief msg default count , msg default length
*/
#define QUEUEDEFAULTCOUNT	8
#define MSGDEFAULTLENGTH 	16

/**
*\brief mutex struct
*/
typedef struct _mqd_t_
{
	void 	*pstMqueuehandle;
	unsigned long	slMsgsize;
} _mqd_t, *mqd_t;


/**
*\brief  queue name struct
*/
struct _mq_name
{
	char abQueuename[QUEUENAMELEN];	/*!< FreeRTOS queue name */
	mqd_t pstMq_handle;	/*!< FreeRTOS queue handle */
	int	 open_count;
	struct _mq_name *pstNext;  /*!< next struct */
};

/**
*\brief  queue attribute struct
*/
struct mq_attr
{
	long mq_flags;    /*!< mqueue flags */
	long mq_maxmsg;   /*!< max number of messages */
	long mq_msgsize;  /*!< max message size */
	long mq_curmsgs;  /*!< number of messages currently queued */
};

/******************************************************************************************/
/**
 * \fn		 mqd_t mq_open(const char *name, int oflag, ...)
 *
 * \brief		 open a message queue (REALTIME)
 *
 * \param		name : (Input) The name of the message queue that you want to open
 * \param		oflag : (Input) The supported values of oflag shall include O_RDONLY , O_WRONLY .O_RDWR, and O_CREAT ...
 *
 * \return		\e mqd_t A valid message queue descriptor \n
 *            	                  \e ENAMETOOLONG  The length of name exceeds PATH_MAX. \n
 *            	                  \e ENFILE  Too many message queues are open in the system \n
 *            	                  \e ENOSPC The message queue server has run out of memory \n
 *
 * \sa 		mq_close ()
 *
 ******************************************************************************************/
mqd_t mq_open(
	const char *name,
	int         oflag,
	...
);

/******************************************************************************************/
/**
 * \fn		 int mq_close(mqd_t  mqdes)
 *
 * \brief		close a message queue (REALTIME)
 *
 * \param		mqdes : (Input) A valid message queue descriptor
 *
 * \return		\e 0 on success. \n
 *            	                  \e EBADF The mqdes argument is not a valid message queue descriptor. \n
 *
 * \sa 		mq_open ()
 *
 ******************************************************************************************/
int mq_close(
	mqd_t  mqdes
);

/******************************************************************************************/
/**
 * \fn		int mq_unlink(const char *name)
 *
 * \brief		remove a message queue (REALTIME)
 *
 * \param		name : (Input) The name of the message queue that you want to unlink
 *
 * \return	\e 0 on success. \n
 *            \e EBADF The mqdes argument is not a valid message queue descriptor. \n
 *
 * \sa 		mq_open ()
 *
 ******************************************************************************************/
int mq_unlink(const char *name);

/******************************************************************************************/
/**
 * \fn		 int mq_send(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int  msg_prio)
 *
 * \brief		send a message to a message queue (REALTIME)
 *
 * \param		mqdes : (Input) A valid message queue descriptor
 * \param		msg_ptr : (Input) the message pointed
 * \param		msg_len : (Input) the message len
 * \param		msg_prio : (Input) the message is inserted into the message queue at the position indicated
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		mq_receive ()
 *
 ******************************************************************************************/
int mq_send(
	mqd_t         mqdes,
	const char   *msg_ptr,
	size_t        msg_len,
	unsigned int  msg_prio
);

/******************************************************************************************/
/**
 * \fn		 ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio)
 *
 * \brief		Receives a message from a message queue (REALTIME)
 *
 * \param		mqdes : (Input) A valid message queue descriptor
 * \param		msg_ptr : (Input) the message  copied to buffer pointed
 * \param		msg_len : (Input) the message len
 * \param		msg_prio : (Input) the priority of the selected message is stored in the location referenced
 *
 * \return		\e size the length of the selected message in bytes . \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		mq_send ()
 *
 ******************************************************************************************/
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio);



/******************************************************************************************/
/**
 * \fn		 ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned int *msg_prio)
 *
 * \brief		Receives a message from a message queue (REALTIME)
 *
 * \param		mqdes : (Input) A valid message queue descriptor
 * \param		msg_ptr : (Input) the message  copied to buffer pointed
 * \param		msg_len : (Input) the message len
 * \param		msg_prio : (Input) the priority of the selected message is stored in the location referenced
 * \param		abs_timeout : (Input) the abs_timeout has already expired by the time of the call, mq_timedreceive() returns immediately
 *
 * \return		\e size the length of the selected message in bytes . \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		mq_send ()
 *
 ******************************************************************************************/

ssize_t mq_timedreceive(mqd_t mqdes, char *msg_ptr,
						size_t msg_len, unsigned *msg_prio,
						const struct timespec *abs_timeout);



/******************************************************************************************/
/**
 * \fn		 int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned int  msg_prio)
 *
 * \brief		send a message to a message queue (REALTIME)
 *
 * \param		mqdes : (Input) A valid message queue descriptor
 * \param		msg_ptr : (Input) the message pointed
 * \param		msg_len : (Input) the message len
 * \param		msg_prio : (Input) the message is inserted into the message queue at the position indicated
 * \param		abs_timeout : (Input) the abs_timeout has already expired by the time of the call, mq_timedsend() returns immediately
 *
 * \return		\e 0 on success. \n
 *            	                  \e EINVAL The value specified for the argument is not correct. \n
 *
 * \sa 		mq_timedreceive ()
 *
 ******************************************************************************************/

int mq_timedsend(mqd_t mqdes, const char *msg_ptr, size_t msg_len, unsigned msg_prio, const struct timespec *abs_timeout);

#endif //__POSIX_MQUEUE_H__
