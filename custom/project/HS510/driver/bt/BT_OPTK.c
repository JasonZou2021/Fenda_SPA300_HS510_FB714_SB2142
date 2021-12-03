#include <string.h>
#include "qpc.h"
#include "uart_app.h"
#include "userapp.h"
#include "pinmux.h"
#include "fifo.h"
#include "busy_delay.h"
#include "exstd_interfaces.h"
#include "datarammap.h"
#define LOG_MODULE_ID  ID_APUSR
#define LOG_EXTRA_STR  "[BT]"
#include "log_utils.h"
#include "qp_pub.h"
#include "bt_cmd.h"
#include "AudDspService.h"
#include "fsi_api.h"
#include "uart_interface.h"


#define SUPPPORT_BT_UPG

#define UART_BUF_MAX	128

extern unsigned int GetSTC(void);
extern void AudDspService_Set_OutSampleRate(QActive *pSender, UINT32 value);


static uint8_t *packet_buf;

#define BT_POWER_PIN	 4
#define BT_POWER_KEY	 3

#define BT_TX_PIN	 48
#define BT_RX_PIN	 49

typedef struct 
{
	int fd;
	BYTE vol;
	BYTE play;
	BYTE mode;
	BYTE power;
	BYTE link;
	BYTE reconnect_timer;
	BYTE power_on_timer;
	BYTE src;
	UINT16 src_timer;
	BYTE bt_on;
	BYTE bt_ver;
	BYTE bt_ver_cnt;
	BYTE bt_upg;
	BYTE reset;
	BYTE reset_cnt;
	UINT16 bt_off_cnt;

}BT_HANDLE;


#define BT_SW_SRC_TIMEOUT		50
#define BT_RECONNECT_TIME		100

QActive *UserApp_get(void);

#define BT_MCU_BDR	 	115200
#define BT_UPG_BDR	 	115200*4


int BT_FIFO_fd;
PACKET_FIFO BT_FIFO;
BT_HANDLE bt_handle = {.fd = -1, .src = BT_MODE_AUX, .bt_upg = 0};


static void BT_PIN_SetLow(void)
{
	Pinmux_Module_Free(I2S_IN_2CH);
	Pinmux_GPIO_Free(56);
	Pinmux_GPIO_Alloc(56, IO_GPIO, IO_RISC);
	GPIO_OE_Set(56, IO_OUTPUT);
	GPIO_Output_Write(57, IO_LOW);
	Pinmux_GPIO_Free(57);
	Pinmux_GPIO_Alloc(57, IO_GPIO, IO_RISC);
	GPIO_OE_Set(57, IO_OUTPUT);
	GPIO_Output_Write(57, IO_LOW);
	Pinmux_GPIO_Free(58);
	Pinmux_GPIO_Alloc(58, IO_GPIO, IO_RISC);
	GPIO_OE_Set(58, IO_OUTPUT);
	GPIO_Output_Write(58, IO_LOW);
	Pinmux_GPIO_Free(59);
	Pinmux_GPIO_Alloc(59, IO_GPIO, IO_RISC);
	GPIO_OE_Set(59, IO_OUTPUT);
	GPIO_Output_Write(59, IO_LOW);

	Pinmux_Module_Free(BT);

	Pinmux_GPIO_Free(BT_RX_PIN);
	Pinmux_GPIO_Alloc(BT_RX_PIN, IO_GPIO, IO_RISC);
	GPIO_OE_Set(BT_RX_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_RX_PIN, IO_LOW);

	Pinmux_GPIO_Free(BT_TX_PIN);
	Pinmux_GPIO_Alloc(BT_TX_PIN, IO_GPIO, IO_RISC);
	GPIO_OE_Set(BT_TX_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_TX_PIN, IO_LOW);
	
}


static int BTMcu_Send2Uart(pBT_PACKET packet)
{
	BT_HANDLE *handle = &bt_handle;
	BYTE send_buf[10];

	if(packet == NULL)
		return -1;

	send_buf[0] = packet->head1;
	send_buf[1] = packet->head2;
	send_buf[2] = packet->len;
	send_buf[3] = packet->cmd;
	send_buf[4] = packet->data[0];
	if(packet->len == 5)
	{
		send_buf[5] = packet->data[1];
		send_buf[6] = packet->chks;
		send_buf[7] = 0;
	}
	else
	{
		send_buf[5] = packet->chks;
		send_buf[6] = 0;
	}

	if(handle->fd != -1)
	{
		LOGD(" %s, len %x, cmd %x, data0 %d, data1 %d\n ", __func__, packet->len,  packet->cmd, packet->data[0],packet->data[1]);
		write(handle->fd , send_buf, packet->len+3);
	}
	
	return 0;
}

static int BTMcu_SendCmd(BYTE cmd,  BYTE data0, BYTE data1, BYTE len, BYTE mode)
{
	FRAME_PACKET packet;
	BT_PACKET BT;
	
	memset(&BT, 0, sizeof(BT_PACKET));

	BT.head1 = BT_DSP_HEAD1;
	BT.head2 = BT_DSP_HEAD2;
	BT.cmd = cmd;
	BT.len = len+3;
	BT.chks = BT.cmd+BT.len;
	BT.data[0] = data0;
	BT.data[1] = data1;
	BT.chks += data0+data1;

	packet.data = (BYTE *)&BT;
	packet.data_len = sizeof(BT);

	if(mode == EXTERNALMCU_PACKET_QUEUE_SEND)
		Fifo_Add_Queue(BT_FIFO_fd, &packet);
	else
		BTMcu_Send2Uart(&BT);

	return 0;
}



static int Polling_Send_BTPacket(void)
{

	FRAME_PACKET packet;
	BT_PACKET BT;

	memset(&BT, 0, sizeof(BT_PACKET));
	packet.data = (BYTE *)&BT;
	
	if(Fifo_Get_Queue(BT_FIFO_fd, &packet) < 0)
		return -1;

	BTMcu_Send2Uart((pBT_PACKET)packet.data);
	
	return 0;

}

static int Polling_Recv_BTPacket(BT_PACKET packet)
{
	BT_HANDLE *handle = &bt_handle;

	ap_printf(" %s : %x-%x-%x\n ", __func__, packet.cmd, packet.data[0], packet.data[1]);

	switch(packet.cmd)
	{
		case BT_CMD_RECV:
			switch(packet.data[0])
			{
				case BT_RECV_CONNECT_STATUS:
					if(packet.data[1] == 1)
					{
						ap_printf(" BT connect\n ");
						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), NULL);
						handle->link = 1;
					}
					else
					{
						if(handle->src == BT_MODE_BT)
						{
							ap_printf(" BT disconnect\n ");
							QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_READY_SOUNDHINT_SIG), NULL);
						}
						else
						{
							ap_printf(" BT disconnect, cut bt power\n ");
							close(handle->fd);
							handle->fd = -1;
							BT_PIN_SetLow();
							GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
						}
							
						handle->link = 0;
					}
					break;
					
				case BT_RECV_PLAY_STATUS:
					LOGD(" BT play %d\n ", packet.data[1]);
					handle->play = packet.data[1];
					BTEvt *evt = Q_NEW(BTEvt, BT_PLAY_STATUS_SIG);
					evt->vol = packet.data[1];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break;
					
				case BT_RECV_VER:
					LOGD(" BT ver %d\n ", packet.data[1]);
					handle->bt_ver = packet.data[1];
					break;
					
				case BT_RECV_VOL:{
					LOGD(" BT vol up %d\n ", packet.data[1]);
					//BTEvt *evt = Q_NEW(BTEvt, BT_VOL_SIG);
					//evt->vol = packet.data[1];
					//QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break;
				}

				case BT_RECV_SAMPRATE:
					LOGD(" BT sample %d\n ", packet.data[1]);	
					if(packet.data[1] == 0)
						AudDspService_Set_OutSampleRate(NULL, 44100);
					else
						AudDspService_Set_OutSampleRate(NULL, 48000);
					break;
					
				case BT_RECV_AUX:
					//if(handle->src != BT_MODE_BT)
					//	handle->src_timer = 0;
					LOGD(" BT enter aux %d\n ", handle->src);	
						BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_SRC_BT, 0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
					break;
					
				case BT_RECV_BT:
					//if(handle->src == BT_MODE_BT)
					//	handle->src_timer = 0;
					if(handle->reset == 1)
						handle->reset_cnt = 20;
					handle->reset = 0;
					LOGD(" BT enter bt %d\n ", handle->src);	
				//	BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_VOL_SET, 17, 2,  EXTERNALMCU_PACKET_QUEUE_SEND);
					
					break;

				default:
					break;
			}
			break;
		default:
			break;
	}
	
	return 0;
}


static int BT_MCU_Init(QActive *me)
{
	BT_HANDLE *handle = &bt_handle;

	(void)me;

	packet_buf = malloc(256);
	if(packet_buf == NULL)
	{
		LOGD(" %s BT malloc fail\n ", __func__);
		return -1;
	}

	BT_FIFO_fd = Fifo_Register(&BT_FIFO);
	

	handle->bt_on = 0;
	handle->bt_ver_cnt = 20;

	delay_1ms(30);

	Pinmux_GPIO_Free(BT_POWER_PIN);
	Pinmux_GPIO_Alloc(BT_POWER_PIN,IO_GPIO,IO_RISC);
	GPIO_OE_Set(BT_POWER_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);


	Pinmux_GPIO_Free(BT_POWER_KEY);
	Pinmux_GPIO_Alloc(BT_POWER_KEY,IO_GPIO,IO_RISC);
	GPIO_OE_Set(BT_POWER_KEY, IO_OUTPUT);
	GPIO_Output_Write(BT_POWER_KEY, IO_LOW);

	handle->power = 0; 
	handle->bt_upg = 0;

	LOGD(" %s : fifo_fd %d, uart %d\n ", __func__, BT_FIFO_fd, handle->fd);
	
	return BT_MCU_BDR;
}


static int BT_MCU_Read(QActive *me, unsigned char *buf, unsigned int size)
{
	//BT_HANDLE *handle = &bt_handle;
	unsigned int i=0;
	UINT32 data;
	static BYTE rx_state = RECV_RX_HD_H;
	static UINT16 data_len = 0;
	static BYTE chksum;
	static BT_PACKET packet;
	(void)me;

	if(size <= 0)
		return 0;
	
	LOGD(" %s : size %d : 0x%x\n ", __func__, size, buf[0]);

	for(i=0; i<size; i++)
	{
		data = buf[i];	
		
		switch(rx_state)
		{
			case RECV_RX_HD_H:
			
				if(data == BT_HEAD1)
				{
					//LOGD("BT_HEAD_H: %x\n", data);
					data_len = 0;
					chksum = 0;					
					rx_state = RECV_RX_HD_L;
				}
				break;

			case RECV_RX_HD_L:
				if(data == BT_HEAD1 )
				{
					rx_state = RECV_RX_HD_L;
				}
				else if(data == BT_HEAD2)
				{
					//LOGD("BT_HEAD_L: %x\n", data);
					rx_state = RECV_RX_LEN;
				}
				else
					rx_state = RECV_RX_HD_H;
				break;

			case RECV_RX_LEN:
			//	LOGD("RECV_RX_LEN: %x\n", data);
				packet.len = data;
				if(packet.len > BT_PACKET_MAX)
					rx_state = RECV_RX_HD_H;
				else
					rx_state = RECV_RX_CMD;	
				chksum += data;
				break;

			case RECV_RX_CMD:
			//	LOGD("RECV_RX_CMD: %x\n", data);
				packet.cmd = data;
				rx_state = RECV_RX_DAT;
				chksum += data;
				break;

			case RECV_RX_DAT:
			//	LOGD("BT_HEAD_DAT: %x\n", data);
				if(data_len < packet.len-3)
				{
					packet.data[data_len++] = data;
					chksum += data;
					if(data_len == packet.len-3)
					{
						rx_state = RECV_RX_CHK;
					}
				}
				
				break;
				
			case  RECV_RX_CHK:
				LOGD("BT_HEAD_CHK: %x, %x\n",chksum, data);
				if(chksum == data)
				{
					packet.chks = data;
					Polling_Recv_BTPacket(packet);
				}

				rx_state = RECV_RX_HD_H;

				break;

			default:
				rx_state = RECV_RX_HD_H;
				break;
		}
	}
	
	return 0;
	
}

static int BT_MCU_Polling(QActive *me)
{
	BT_HANDLE *handle = &bt_handle;
	(void)me;
	int len = 0;

	if(handle->bt_upg == 1)
		return 0;
	
	if(packet_buf != NULL && handle->fd > 0)
	{
		memset(packet_buf, 0, UART_BUF_MAX);
		len = read(handle->fd, packet_buf, UART_BUF_MAX);
		if(len > 0)
		{
			BT_MCU_Read(me, packet_buf, len);
		}
	}

	// check bt module has got the switch cmd 
	if(handle->src_timer > 0)
	{
		handle->src_timer--;

		if(handle->src_timer == 12) 
		{
			if(handle->src == BT_MODE_BT && handle->bt_on == 1)
			{
				GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
			}
		}
		if(handle->src_timer == 8) 
		{
			if(handle->src == BT_MODE_BT && handle->bt_on == 1)
			{
				GPIO_Output_Write(BT_POWER_KEY, IO_HIGH);
			}
		}
		else if(handle->src_timer == 0)
		{
			//handle->src_timer = BT_SW_SRC_TIMEOUT;
			handle->power = 1;
			LOGD("========== FIRST swicth SRC %d\n", handle->src);
			if(handle->src == BT_MODE_BT && handle->bt_on == 1 )
			{
				Pinmux_Module_Alloc(I2S_IN_2CH);
				Pinmux_Module_Alloc(BT);
				handle->fd = open("C:/uart2", O_RDWR, 0);
				ioctl(handle->fd, UART_NORMAL_MODE|DATA_BITS_8|STOP_BITS_1|PARITY_NONE|FLOW_CONTROL_NONE, BT_MCU_BDR);
				ioctl(handle->fd, UART_CMD_READ_TIMEOUT ,portMAX_DELAY);
			}
		}
	}

	if(handle->reset_cnt > 0)
	{
		handle->reset_cnt--;
		if(handle->reset_cnt == 0)
		{
			BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_RESET,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
		}
	}
		

	if(handle->bt_ver == 0 && handle->power == 1 && handle->src == BT_MODE_BT)
	{
		if(handle->bt_ver_cnt > 0)
		{
			handle->bt_ver_cnt--;
			if(handle->bt_ver_cnt == 0)
			{
				handle->bt_ver_cnt = 80;
				LOGD(" check bt version %d\n ", handle->fd);
				BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_VER, 0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);	
			}
		}
	}


	Polling_Send_BTPacket();
	
	
	return 0;
	
}


int BT_MCU_Key(QActive *me, int key, int para)
{
	BT_HANDLE *handle = &bt_handle;
	(void)me;

	LOGD(" %s. %d\n ", __func__, handle->bt_upg );

	if(handle->bt_upg == 1)
		return 0;

	switch(key)
	{
	
		case RESET_RELEASE_SIG:
			LOGD("=============== %s reset\n ", __func__);

			handle->reset = 1;
			if(handle->src == BT_MODE_BT) 
				BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_RESET,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			else 
			{
				//GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
				handle->src = BT_MODE_BT;	
				handle->src_timer = 20;
				handle->bt_on = 1;
			}
			break;
	
		case POWER_RELEASE_SIG:
			LOGD("=============== %s power off\n ", __func__);
			if(handle->link)
			{
				handle->src = BT_MODE_AUX;
				BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			}
			else
				GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
			break;
	
		case PAIR_RELEASE_SIG:
			LOGD("=============== %s pair\n ", __func__);
			BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
			
		case PRE_RELEASE_SIG:
			LOGD("=============== %s prev\n ", __func__);
			BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_PREV,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			
			break;
			
		case NEXT_RELEASE_SIG:
			LOGD("=============== %s next\n ", __func__);
			BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_NEXT,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			
			break;
			
		case PLAY_RELEASE_SIG:
			LOGD("=============== %s play %d\n ", __func__,handle->play);
			BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_PLAY, 0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case VOL_SET_SIG:
			LOGD("=============== %s vol set %d\n ", __func__,para);
			if(handle->src == BT_MODE_BT) 
			{	
				//if(para >= 17)
				//	para = 17;
				//BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_VOL_SET, para, 2,  EXTERNALMCU_PACKET_QUEUE_SEND);
			}
			break;

		case SOURCE_SW_RELEASE_SIG:{
			LOGD("=============== %s src %d[%d], power %d\n ", __func__, para, handle->src, handle->power);

			if(handle->power == 1)
			{
				handle->src_timer =  0;
				if(para == BT_MODE_BT && handle->src != BT_MODE_BT) // 3 is BT
				{
					LOGD("========== swicth BT\n");
					handle->src = BT_MODE_BT;	
				
					handle->src_timer = 20;
					handle->bt_on = 1;
				}
				else if(para != BT_MODE_BT && handle->src == BT_MODE_BT)
				{

					LOGD("========== swicth AUX, link %d\n", handle->link);

					if(handle->link)
					{
						BTMcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, 1, EXTERNALMCU_PACKET_QUEUE_SEND);
					}
					else
					{
						close(handle->fd);
						handle->fd = -1;
						BT_PIN_SetLow();
						
						GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
					}

					handle->bt_on = 0;
					handle->src = BT_MODE_AUX;
				
				}
			}
			else
			{
				handle->src_timer = BT_SW_SRC_TIMEOUT;
				if(para == BT_MODE_BT)
				{
					handle->bt_on = 1;
					//GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
					handle->src = BT_MODE_BT;
					BT_PIN_SetLow();
					
				}
				else
				{
				
					handle->bt_on = 0;
					BT_PIN_SetLow();
					GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
				}
			}
	
			handle->play = 0;
			
			break;
		}
		
	}

	return 0;
}

int BT_MCU_SetUartHandle(QActive *me, int fd)
{
	BT_HANDLE *handle = &bt_handle;
	(void)me;
	handle->fd = fd;

	return 0;
}

extern int BT_get_version(void)
{
	return bt_handle.bt_ver;
}

UartApp_Func BT_OPTK = 
{
	BT_MCU_Init,
	BT_MCU_Read,
	BT_MCU_Polling,
	BT_MCU_Key,
	BT_MCU_SetUartHandle,
	
};

// ok  checksum:0x4627f062


