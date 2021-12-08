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




#define BT_POWER_PIN	 45
#define BT_EN_PIN	54

#define BT_MCU_BDR	 	115200


BYTE src_map[] = 
{

	BT_MODE_OPT,
	BT_MODE_COAX,
	BT_MODE_ARC,
	BT_MODE_BT,	
	BT_MODE_USB,
	BT_MODE_AUX,

	
};

BYTE *src_str[] = 
{	
	" OPT",
	" COAX",
	" ARC",
	" BT ",
	" USB",
	" AUX",
	
};



typedef struct 
{
	BYTE fd;
	BYTE vol;
	BYTE play;
	BYTE mode;
	BYTE power;
	BYTE link;
	BYTE subwoofer;
	BYTE reconnect_timer;
	BYTE power_on_timer;
	BYTE src;
	UINT16 src_timer;
	BYTE bt_on;
	BYTE bt_ver;
	BYTE bt_ver_cnt;
	BYTE bt_bass;
	BYTE bt_eq;
	BYTE bt_upg;
	BYTE bt_addr[6];
	UINT16 disconnect_cnt;

}BT_HANDLE;


#define BT_SW_SRC_TIMEOUT		40


QActive *UserApp_get(void);

static uint8_t *packet_buf;

int ATS2819_FIFO_fd;
PACKET_FIFO ATS2819_FIFO;
BT_HANDLE bt_handle;

static int ATS2819Mcu_Send2Uart(pATS2819_PACKET packet)
{
	BYTE i=0;
	BT_HANDLE *handle = &bt_handle;
	BYTE send_buf[16];
	if(packet == NULL)
		return -1;

	send_buf[0] = packet->head;
	send_buf[1] = packet->len;
	send_buf[2] = packet->func;
	send_buf[3] = packet->cmd;
	for(i=0; i<packet->len-5; i++)
		send_buf[4+i] = packet->data[i];
	send_buf[4+i] = packet->chks;

	LOGD(" %s, %x, %x, %x, %x\n ", __func__, packet->head, packet->len, packet->func, packet->cmd);

	write(handle->fd , send_buf, packet->len);

	return 0;
}

static int ATS2819Mcu_SendCmd(BYTE func, BYTE cmd, BYTE len, BYTE *data, BYTE mode)
{
	FRAME_PACKET packet;
	ATS2819_PACKET ATS2819;
	UINT16 ck=0;
	BYTE i=0;
	
	memset(&ATS2819, 0, sizeof(ATS2819_PACKET));

	ATS2819.head = ATS2819_SEND_HEAD;
	ATS2819.func = func;
	ATS2819.cmd = cmd;
	

	if(len == 0)
	{
		ck=ATS2819_SEND_HEAD+6+cmd+func;
		ATS2819.len = 6;
		ATS2819.data[0] = 0;
	}
	else
	{
		ck=ATS2819_SEND_HEAD+len+5+cmd+func;
		ATS2819.len = len+5;
		for(i=0; i<len; i++)
		{
			ATS2819.data[i] = data[i];
			ck += data[i];
		}
	}
	ATS2819.chks = ~((BYTE)ck);

	packet.data = (BYTE *)&ATS2819;
	packet.data_len = sizeof(ATS2819);

	if(mode == EXTERNALMCU_PACKET_QUEUE_SEND)
		Fifo_Add_Queue(ATS2819_FIFO_fd, &packet);
	else
		ATS2819Mcu_Send2Uart(&ATS2819);

	return 0;
}



static int Polling_Send_ATS2819Packet(void)
{

	FRAME_PACKET packet;
	ATS2819_PACKET ATS2819;

	memset(&ATS2819, 0, sizeof(ATS2819_PACKET));
	packet.data = (BYTE *)&ATS2819;
	
	if(Fifo_Get_Queue(ATS2819_FIFO_fd, &packet) < 0)
		return -1;

	ATS2819Mcu_Send2Uart((pATS2819_PACKET)packet.data);
	
	return 0;

}

static int Polling_Recv_ATS2819Packet(ATS2819_PACKET packet)
{
	BT_HANDLE *handle = &bt_handle;
	//static BYTE flag = 0;
	//BYTE para = 0;

	LOGD(" %s : %x-%x-%x\n ", __func__, packet.func, packet.cmd, packet.data[0]);

	switch(packet.func)
	{
		case BT_RECV_CMD:
			switch(packet.cmd)
			{
				case BT_RECV_BTADDR:
					LOGD(" BT addr 0x%x-0x%x-0x%x-0x%x-0x%x-0x%x\n ", packet.data[0], packet.data[1], packet.data[2], packet.data[3], packet.data[4], packet.data[5]);
					memcpy(handle->bt_addr, &packet.data[0], 6);

					//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_ADDR_SIG), NULL);
				
					break;
			
				case BT_RECV_POWER_ON:
					break;
					
				case BT_RECV_POWER_OFF:
					break;
					
				case BT_RECV_CONNECTED:
					LOGD(" BT connect\n ");
					QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), NULL);
					handle->link = 1;
					break;
				case BT_RECV_PAIRING:
					LOGD(" BT disconnect\n ");
					QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_READY_SOUNDHINT_SIG), NULL);
					handle->link = 0;
					break;
					
				case BT_RECV_FARAWAY_RECONNECT:
					break;
				case BT_RECV_RECONNECT:
					break;
					
				case BT_RECV_VOL_UP:{
					LOGD(" BT vol up %d\n ", packet.data[0]);
					BTEvt *evt = Q_NEW(BTEvt, BT_VOL_SIG);
					evt->vol = packet.data[0];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break;}
				case BT_RECV_VOL_DN:{
					LOGD(" BT vol dn %d\n ", packet.data[0]);
					BTEvt *evt = Q_NEW(BTEvt, BT_VOL_SIG);
					evt->vol = packet.data[0];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break;}
				case BT_RECV_VOL_SET:{
					LOGD(" BT vol set %d\n ", packet.data[0]);
					BTEvt *evt = Q_NEW(BTEvt, BT_VOL_SIG);
					evt->vol = packet.data[0];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break; }
					
				case BT_RECV_ENTER_REP:
					break;
				case BT_RECV_EXIT_REP:
					break;

				case BT_RECV_AUX:
					if(handle->src != BT_MODE_BT)
						handle->src_timer = 0;
					LOGD(" BT enter aux %d\n ", handle->src);	

					if(handle->bt_upg == 1)
					{
						handle->bt_upg = 0;
						//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_UPG_OK_SIG), NULL);
					}
					break;
				case BT_RECV_BT:
					if(handle->src == BT_MODE_BT)
						handle->src_timer = 0;
					LOGD(" BT enter bt %d\n ", handle->src);	
					break;

				case BT_RECV_PLAY:{
					handle->play = packet.data[0];
					BTEvt *evt = Q_NEW(BTEvt, BT_PLAY_STATUS_SIG);
					evt->vol = packet.data[0];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					LOGD(" BT set play %d\n ", handle->play);	
					break;
				}
				
				case BT_RECV_PERCENT:{
					BTEvt *evt = Q_NEW(BTEvt, BT_UPG_PROCESS_SIG);
					evt->vol = packet.data[0];
					QACTIVE_POST(UserApp_get(), (QEvt*)evt, NULL);
					break;
				}
				default:
					break;
			}
			break;
			
		case BT_RECV_CHECK:
			switch(packet.cmd)
			{
				case BT_RECV_CHK_SRC:
					break;
				case BT_RECV_CHK_VOL:
					break;
				case BT_RECV_CHK_MUTE:
					break;
				case BT_RECV_CHK_PLAY:
					break;
					
				case BT_RECV_CHK_DSP_VER:
					//para = NEON_VER;
					//ATS2819Mcu_SendCmd(BT_CMD_ACK, BT_ACK_DSP_VER, 1, &para, EXTERNALMCU_PACKET_QUEUE_SEND);
					//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, FACTORY_TEST_VER_SIG), NULL);
					break;
					
				case BT_RECV_CHK_KEY:
					//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, FACTORY_TEST_KEY_SIG), NULL);
					break;
					
				case BT_RECV_CHK_VFD:
					/*
					if(flag)
					{
						GPIO_Output_Write(STANDBY_LED, IO_LOW);
						aip1944_Full();
					}
					else
					{
						GPIO_Output_Write(STANDBY_LED, IO_HIGH);
						aip1944_null();
					}
					ATS2819Mcu_SendCmd(BT_CMD_ACK, BT_ACK_VFD, 1, &flag, EXTERNALMCU_PACKET_QUEUE_SEND);
					flag ^= 1;
					QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, FACTORY_TEST_VFD_SIG), NULL);
					*/
					break;
					
				case BT_RECV_CHK_TEST_EXIT:
					//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, FACTORY_TEST_EXIT_SIG), NULL);
					break;
					
				default:
					break;
			}
			break;
			
		case BT_RECV_ACK:
			switch(packet.cmd)
			{
				case BT_RECV_ACK_PLAY:
					break;
				case BT_RECV_ACK_VOL:
					break;
				case BT_RECV_ACK_VERSION:
					LOGD(" BT version %d\n ", packet.data[0]);	
					handle->bt_ver = packet.data[0];
					if(handle->bt_ver != 0)
						handle->bt_ver_cnt = 0;
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


static int ATS2819_MCU_Init(QActive *me)
{
	BT_HANDLE *handle = &bt_handle;

	(void)me;

	packet_buf = malloc(256);
	if(packet_buf == NULL)
	{
		LOGD(" %s BT malloc fail\n ", __func__);
		return -1;
	}

	ATS2819_FIFO_fd = Fifo_Register(&ATS2819_FIFO);
	LOGD(" %s : fifo_fd %d\n ", __func__, ATS2819_FIFO_fd);

	handle->bt_on = 0;
	memset(handle->bt_addr, 0, 6);
	Pinmux_GPIO_Free(BT_POWER_PIN);
	Pinmux_GPIO_Alloc(BT_POWER_PIN,IO_GPIO,IO_RISC);
	GPIO_OE_Set(BT_POWER_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);

	handle->fd = open("C:/uart2", O_RDWR, 0);
	ioctl(handle->fd, UART_NORMAL_MODE|DATA_BITS_8|STOP_BITS_1|PARITY_NONE|FLOW_CONTROL_NONE, BT_MCU_BDR);
	ioctl(handle->fd, UART_CMD_READ_TIMEOUT ,portMAX_DELAY);


	return 0;
}


static int ATS2819_MCU_Read(QActive *me, unsigned char *buf, unsigned int size)
{
	UINT16 i=0;
	UINT32 data;
	static BYTE rx_state = RECV_RX_HD_H;
	static UINT16 data_len = 0;
	static BYTE chksum;
	static ATS2819_PACKET packet;
	(void)me;

	//LOGD(" %s : size %d 0x%x, %d\n ", __func__, size, buf[i], rx_state);

	for(i=0; i<size; i++)
	{
		data = buf[i];	

		switch(rx_state)
		{
			case RECV_RX_HD_H:
			
				if(data == ATS2819_RECV_HEAD)
				{
					//LOGD("ATS2819_HEAD_H: %x\n", data);
					data_len = 0;
					chksum = ATS2819_RECV_HEAD;					
					packet.head = data;
					rx_state = RECV_RX_LEN;
				}
				break;

			case RECV_RX_LEN:
				packet.len = data;
				if(packet.len < 6)
					rx_state = RECV_RX_HD_H;
				else
					rx_state = RECV_RX_FUNC;	
				chksum += data;
				break;
				
			case RECV_RX_FUNC:
				packet.func = data;
				rx_state = RECV_RX_CMD;
				chksum += data;
				break;

			case RECV_RX_CMD:
				packet.cmd = data;
				rx_state = RECV_RX_DAT;
				chksum += data;
				break;

			case RECV_RX_DAT:
				//LOGD("ATS2819_HEAD_DAT: %x\n", data);

				if(packet.cmd == BT_RECV_BTADDR)
				{
					if(data_len < packet.len)
					{
						packet.data[data_len++] = data;
						chksum += data;
						if(data_len == packet.len)
						{
							rx_state = RECV_RX_CHK;
							chksum = ~chksum;
						}
					}
				}
				else
				{
					if(data_len < packet.len-5)
					{
						packet.data[data_len++] = data;
						chksum += data;
						if(data_len == packet.len-5)
						{
							rx_state = RECV_RX_CHK;
							chksum = ~chksum;
						}
					}
				}
				
				break;
				
			case  RECV_RX_CHK:
				//LOGD("ATS2819_HEAD_CHK: %x, %x\n",chksum, data);
				if(chksum == data)
				{
					packet.chks = data;
					Polling_Recv_ATS2819Packet(packet);
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

int ATS2819_MCU_Polling(QActive *me)
{
	BT_HANDLE *handle = &bt_handle;
	(void)me;
	int len=0;

	if(packet_buf != NULL && handle->fd > 0)
	{
		memset(packet_buf, 0, UART_BUF_MAX);
		len = read(handle->fd, packet_buf, UART_BUF_MAX);
		if(len > 0)
		{
			ATS2819_MCU_Read(me, packet_buf, len);
		}
	}

	// check bt module has got the switch cmd 
	if(handle->src_timer > 0)
	{
		handle->src_timer--;
		if(handle->src_timer == 0)
		{
			handle->src_timer = BT_SW_SRC_TIMEOUT;
			//if(handle->src == BT_MODE_BT)
			//	ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_SRC_BT, 0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			//else 
			//	ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_AUX, 0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
		}
	}

	if(handle->disconnect_cnt > 0)
	{
		handle->disconnect_cnt --;
		if(handle->disconnect_cnt == 0)
		{
			if(handle->src != BT_MODE_BT)
				GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
		}
	}


	if(handle->bt_ver == 0)
	{
		if(handle->bt_ver_cnt > 0)
		{
			handle->bt_ver_cnt--;
			if(handle->bt_ver_cnt == 0)
			{
				handle->bt_ver_cnt = 10;
				LOGD(" check bt version\n ");
				//ATS2819Mcu_SendCmd(BT_CMD_CHECK, BT_CHK_VERSION, 0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);	
			}
		}
	}

	static BYTE cnt;

	if(cnt++ == 2)
	{
		cnt = 0;
		Polling_Send_ATS2819Packet();
	}
	
	return 0;
	
}




int ATS2819_MCU_Key(QActive *me, int key, int para)
{
	BT_HANDLE *handle = &bt_handle;
	(void)me;

	//ATS2819_printf1(" %s\n ", __func__);

	switch(key)
	{
		case BT_UPG_SIG:
			LOGD("=============== %s bt upg 0x%x\n ", __func__, para);
			handle->bt_upg = 1;
			break;
	
		case RESET_RELEASE_SIG:
			LOGD("=============== %s reset\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_RESET,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
	
		case POWER_OFF_RELEASE_SIG:
			LOGD("=============== %s power off\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			//GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
			handle->disconnect_cnt = 30;
			break;
	
		
	
		case PAIR_RELEASE_SIG:
			
			LOGD("=============== %s pair\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
			
		case PRE_RELEASE_SIG:
			LOGD("=============== %s prev\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_PREV,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
			
		case NEXT_RELEASE_SIG:
			LOGD("=============== %s next\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_NEXT,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
			
		case PLAY_RELEASE_SIG:
			LOGD("=============== %s play %d\n ", __func__,handle->play);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_PLAY_PAUSE, 0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case VOL_UP_RELEASE_SIG:
			LOGD("=============== %s vol up %d\n ", __func__,para);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_VOL_UP, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case VOL_DOWN_RELEASE_SIG:
			LOGD("=============== %s vol down %d\n ", __func__,para);
			
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_VOL_DN, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case VOL_SET_SIG:
			LOGD("=============== %s vol set %d\n ", __func__,para);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_VOL_SET, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case SOURCE_SW_RELEASE_SIG:
			LOGD("=============== %s src %s, %d\n ", __func__, src_str[para], para);
			handle->src = src_map[para];
			handle->src_timer = BT_SW_SRC_TIMEOUT;

			/*如果是BT模式*/
			if(handle->src == BT_MODE_BT)
			{
				/*打开BT的电源*/
				GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
				handle->disconnect_cnt = 0;
			}
			else 
			{
				/*没有BT连接，将BT的电源关闭*/
				if(handle->link == 0)
					GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
				else
				{
					handle->disconnect_cnt = 30;
					/*SPA300设置ATS2819断开当前蓝牙，进入配对*/
					ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_DISCCONECT,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
				}
			}
			/*
			if(handle->src == BT_MODE_BT)
				ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_SRC_BT, 0, NULL, EXTERNALMCU_PACKET_DIRECT_SEND);
			else 
				ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_AUX, 0, NULL, EXTERNALMCU_PACKET_DIRECT_SEND);
			*/
			handle->link = 0;
			handle->play = 0;
			break;
/*
		case WOOFER_PAIR_SIG:
			LOGD("=============== %s subwoofer pair\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_SUBWOOFER_PAIR,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case HINT_START_SIG:
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_HINT_STATUS, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			LOGD("=============== %s hint start\n ", __func__);
			break;	
			
		case HINT_END_SIG:
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_HINT_STATUS, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			LOGD("===============  %s hint end\n ", __func__);
			break;

		case PD_TEST_SIG:
			handle->bt_ver_cnt = 10;
			LOGD("===============  %s PD TEST\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_CTL, BT_CTL_PD_TEST,0, NULL, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;

		case FACTORY_TEST_KEY_SIG:
			LOGD("===============  %s FACTORY_TEST_KEY_SIG\n ", __func__);
			ATS2819Mcu_SendCmd(BT_CMD_ACK, BT_ACK_KEY, 1, (BYTE *)&para, EXTERNALMCU_PACKET_QUEUE_SEND);
			break;
*/
	}

	return 0;
}


int BT_get_version(void)
{
	return bt_handle.bt_ver;
}

BYTE *BT_get_addr(void)
{
	return bt_handle.bt_addr;
}

UartApp_Func BT_ATS2819 = 
{
	ATS2819_MCU_Init,
	ATS2819_MCU_Read,
	ATS2819_MCU_Polling,
	ATS2819_MCU_Key,
	NULL,
};



