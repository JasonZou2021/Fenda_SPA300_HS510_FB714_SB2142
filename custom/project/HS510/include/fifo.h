
#ifndef __FIFO__H__
#define __FIFO__H__

#define FIFO_PACKET_MAX 	10
#define FIFO_MAX			5

typedef struct
{
	UINT16 data_len;
	BYTE *data;
	 
}FRAME_PACKET, *pFRAME_PACKET;


typedef struct _PACKET_FIFO
{
	BYTE start;
	BYTE end;
	FRAME_PACKET packet[FIFO_PACKET_MAX];

}PACKET_FIFO;


typedef struct _fifo
{
	BYTE index;
	PACKET_FIFO fifo[FIFO_MAX];

}FIFO;


extern int Fifo_Add_Queue(int fifo_fd, pFRAME_PACKET packet);
extern int Fifo_Get_Queue(int fifo_fd, pFRAME_PACKET packet);
extern int Fifo_Register(PACKET_FIFO *fifo);

#endif

