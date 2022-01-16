
#include "qpc.h"
#include <string.h>
#include "fifo.h"
#include "log_utils.h"

//#define FIFO_DBG

#ifdef FIFO_DBG
#define FIFO_printf1(f, a...)  LOG(DISP,1 , f, ##a)
#define FIFO_printf2(f, a...)  LOG(DISP, 2, f, ##a)
#define FIFO_printf_E(f, a...) LOG(ERR, 1, "[DISP][%s][%d]"f, __FUNCTION__, __LINE__, ##a)
#else
#define FIFO_printf1(f, a...)  ((void)0)
#define FIFO_printf2(f, a...)  ((void)0)
#define FIFO_printf_E(f, a...) ((void)0)
#endif

static FIFO __fifo = 
{
	.index = 0,
};

static int Fifo_IsEmpty(PACKET_FIFO *fifo)
{
	if(fifo == NULL)
		return -1;

	if(fifo->end == fifo->start)
	{
		//FIFO_printf1(" %s \n ", __func__);
		return -1;
	}
	return 0;
}

static int Fifo_IsFull(PACKET_FIFO *fifo)
{
	if(fifo == NULL)
		return -1;

	if((fifo->end + 1) % FIFO_PACKET_MAX == fifo->start)
	{
		//FIFO_printf1(" %s \n ", __func__);
		return -1;
	}
	
	return 0;
}

static int Fifo_Push(PACKET_FIFO *fifo, pFRAME_PACKET packet)
{
	if(fifo == NULL || packet == NULL)
		return -1;

	if(Fifo_IsFull(fifo) < 0)
		return -1;

	if(packet->data == NULL)
	{
		FIFO_printf1(" fifo push : packet data is  NULL\n ");
		return -1;
	}

	fifo->packet[fifo->end].data_len = packet->data_len;
	fifo->packet[fifo->end].data = (BYTE *)malloc(packet->data_len);

	if(fifo->packet[fifo->end].data == NULL)
	{
		FIFO_printf1(" fifo : malloc error size %d\n ", packet->data_len);
		return -1;
	}
	
	memcpy(fifo->packet[fifo->end].data, packet->data, fifo->packet[fifo->end].data_len);

	fifo->end = (fifo->end+1)%FIFO_PACKET_MAX;

	return 0;
}

static int Fifo_Pop(PACKET_FIFO *fifo, pFRAME_PACKET packet)
{
	if(fifo == NULL || packet == NULL)
		return -1;

	if(Fifo_IsEmpty(fifo) < 0)
		return -1;

	if(packet->data == NULL)
	{
		FIFO_printf1(" fifo pop : packet data is  NULL\n ");
		return -1;
	}

	if(fifo->packet[fifo->start].data == NULL)
	{
		FIFO_printf1(" fifo pop : fifo packet data is  NULL\n ");
		return -1;
	}
		
	//printf(" 111 %s, %d\n ", __func__, fifo->start);

	memcpy(packet->data, fifo->packet[fifo->start].data, fifo->packet[fifo->start].data_len);
	packet->data_len = fifo->packet[fifo->start].data_len;

	free(fifo->packet[fifo->start].data);

	fifo->start = (fifo->start+1)%FIFO_PACKET_MAX;

	//printf(" 222 %s, %d\n ", __func__, fifo->start);

	return 0;
}

int Fifo_Add_Queue(int fifo_fd, pFRAME_PACKET packet)
{
	FIFO *_fifo = &__fifo;

	if(fifo_fd <=0 )
		return -1;

	return Fifo_Push(&_fifo->fifo[fifo_fd-1], packet);
}

int Fifo_Get_Queue(int fifo_fd, pFRAME_PACKET packet)
{
	FIFO *_fifo = &__fifo;

	if( fifo_fd <=0 )
		return -1;

	return Fifo_Pop(&_fifo->fifo[fifo_fd-1], packet);
}

int Fifo_Register(PACKET_FIFO *fifo)
{
	FIFO *_fifo = &__fifo;

	if(_fifo->index == FIFO_MAX)
		return -1;

	memcpy(&_fifo->fifo[_fifo->index], fifo, sizeof(PACKET_FIFO));
	memset(&_fifo->fifo[_fifo->index], 0, sizeof(PACKET_FIFO));
	_fifo->index++;
	
	return _fifo->index;
}
 
