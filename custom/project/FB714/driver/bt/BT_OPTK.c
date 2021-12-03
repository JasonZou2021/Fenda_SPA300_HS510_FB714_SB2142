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

#define BT_POWER_KEY	 54
#define BT_POWER_PIN	 55

#define BT_TX_PIN	 49
#define BT_RX_PIN	 48

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
	BYTE wait_sample_cnt;
	BYTE wait_hint_cnt;;

}BT_HANDLE;


#define BT_SW_SRC_TIMEOUT		70
#define BT_RECONNECT_TIME		100

QActive *UserApp_get(void);

#define BT_MCU_BDR	 	115200
#define BT_UPG_BDR	 	115200*4


int BT_FIFO_fd;
PACKET_FIFO BT_FIFO;
BT_HANDLE bt_handle = {.fd = -1, .src = BT_MODE_AUX, .bt_upg = 0};

/*
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

	GPIO_Output_Write(BT_POWER_KEY, IO_LOW);
	
}
*/

static void BT_PIN_SetLow(void)
{
	Pinmux_Module_Free(I2S_IN_2CH);
	Pinmux_GPIO_Free(56);
	Pinmux_GPIO_Alloc(56, IO_GPIO, IO_RISC);
	GPIO_OE_Set(56, IO_INPUT);

	Pinmux_GPIO_Free(57);
	Pinmux_GPIO_Alloc(57, IO_GPIO, IO_RISC);
	GPIO_OE_Set(57, IO_INPUT);

	Pinmux_GPIO_Free(58);
	Pinmux_GPIO_Alloc(58, IO_GPIO, IO_RISC);
	GPIO_OE_Set(58, IO_INPUT);

	Pinmux_GPIO_Free(59);
	Pinmux_GPIO_Alloc(59, IO_GPIO, IO_RISC);
	GPIO_OE_Set(59, IO_INPUT);


	Pinmux_Module_Free(BT);

	Pinmux_GPIO_Free(BT_RX_PIN);
	Pinmux_GPIO_Alloc(BT_RX_PIN, IO_GPIO, IO_RISC);
	GPIO_OE_Set(BT_RX_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_RX_PIN, IO_LOW);

	Pinmux_GPIO_Free(BT_TX_PIN);
	Pinmux_GPIO_Alloc(BT_TX_PIN, IO_GPIO, IO_RISC);
	GPIO_OE_Set(BT_TX_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_TX_PIN, IO_LOW);

	GPIO_Output_Write(BT_POWER_KEY, IO_LOW);
	
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
					if(packet.data[1] == 2)
					{
						ap_printf(" BT connect\n ");
						handle->wait_sample_cnt = 50; // wait sample rate set
						//QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), NULL);
						handle->link = 1;
						AudDspService_Set_SourceGain(NULL,-99.0);
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
					BTEvt *evt = Q_NEW(BTEvt, BT_VOL_SIG);
					evt->vol = packet.data[1];
					QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
					break;
				}

				case BT_RECV_SAMPRATE:
					LOGD(" BT sample %d\n ", packet.data[1]);	

					if(handle->bt_on ==  1)
					{
					if(packet.data[1] == 0)
						AudDspService_Set_OutSampleRate(NULL, 44100);
					else
						AudDspService_Set_OutSampleRate(NULL, 48000);
						
					}
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

	BT_PIN_SetLow();

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

	if(handle->wait_sample_cnt > 0)
	{
		handle->wait_sample_cnt--;
		if(handle->wait_sample_cnt == 0 && handle->bt_on == 1)
		{
			if(handle->link == 1)
			{
				handle->wait_hint_cnt = 100;
				QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), NULL);
			}
		}
	}

	if(handle->wait_hint_cnt > 0)
	{
		handle->wait_hint_cnt--;
		if(handle->wait_hint_cnt == 0 && handle->bt_on == 1)
		{
			AudDspService_Set_SourceGain(NULL, 0);	
		}
	}

	// check bt module has got the switch cmd 
	if(handle->src_timer > 0)
	{
		handle->src_timer--;
	/*
		if(handle->src_timer == 20)
		{
			if(handle->src == BT_MODE_BT && handle->bt_on == 1)
			{
				GPIO_Output_Write(BT_POWER_KEY, IO_HIGH);
			}
		}
		else 
	*/
		if(handle->src_timer == 15) 
		{
			if(handle->src == BT_MODE_BT && handle->bt_on == 1)
			{
				GPIO_Output_Write(BT_POWER_KEY, IO_HIGH);
				GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
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


static void BT_MCU_UPG_SetPin(void)
{
	// I2SIN PIN set low
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

	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
	GPIO_Output_Write(BT_POWER_KEY, IO_LOW);

	delay_1ms(1300);

	// need set UART high
	GPIO_OE_Set(BT_TX_PIN, IO_INPUT); 
	GPIO_Output_Write(BT_RX_PIN, IO_HIGH);

	delay_1ms(200);

	GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);
	delay_1ms(50);
	GPIO_Output_Write(BT_POWER_KEY, IO_HIGH);
}

static void BT_MCU_UPG(void)
{
	BT_HANDLE *handle = &bt_handle;
	close(handle->fd);

	handle->bt_upg = 1;

	QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_UPG_SIG), NULL);

	BT_MCU_UPG_SetPin();

	Pinmux_Module_Alloc(BT);
	handle->fd = open("C:/uart2", O_RDWR, 0);
	ioctl(handle->fd, UART_NORMAL_MODE|DATA_BITS_8|STOP_BITS_1|PARITY_NONE|FLOW_CONTROL_NONE, BT_UPG_BDR);
	
}

static void BT_PC_UPG(void)
{
	BT_HANDLE *handle = &bt_handle;
	close(handle->fd);

	handle->bt_upg = 1;

	QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_UPG_SIG), NULL);
	
	BT_MCU_UPG_SetPin();

}

#ifdef SUPPPORT_BT_UPG


/************************************************/


/************************************************/

typedef struct 
{
	UINT32 rx_header_start;
	UINT32 rx_copy_addr;
	UINT32 rx_run_addr; 
	UINT32 rx_total_bytes;
	UINT32 rx_bootloader_flag;
	UINT32 rx_sflash_size;
	UINT32 rx_checksum;
	UINT32 rx_dummy_data_size;
}BOOT_HEADER;



#define  HEADER_START 0x44332211
#define  BAUDRATE_DET_DATA 0xFFAAFF55
#define  BAUDRATE_DET_DATA_LEN 0xB00
#define  HEADER_LEN 0x20
#define  MAC_ADDR_OFFSET 0x68000

//const char dft_bdaddr[6] = {0x00,0x00,0x00,0x3F,0x9f,0x94};

//const char macid_sub[3] = {0x00,0x00,0x00};
//0x9e8b00~0x9e8b3f useless macid sub

//const char macid_company[3] = {0x3F,0x9f,0x94};
enum {
    READ_CPU_TICKS_ERR,
    WRONG_FILE_FORMAT,
    CHECKSUM_ERROR,
    MACID_SUB_USELESS,
    FILE_CHCK_SUCCESSFUL
};

#define MACID_USELESS -1

#if 0 //def  MAC_ID_AND_CODE_CHECKSUM
/**********************************************/
int fileCheck_fix_MACID(char *fData,char *dataLen,const char macid_sub[3],const char macid_company[3])
{
	int i;
	int ret = FILE_CHCK_SUCCESSFUL;
    char *p = fData+MAC_ADDR_OFFSET+BAUDRATE_DET_DATA_LEN+HEADER_LEN;
    UINT32 *p1 = (UINT32 *)fData;
	unsigned char baaddr[6];

	if ((macid_sub[0]==0x9e)&&(macid_sub[1]==0x8b)&&((macid_sub[2]>=0x0)&&(macid_sub[2]<=0x3f)))
	{
		ret = MACID_SUB_USELESS;
	}
	
	memcpy(baaddr,macid_sub,sizeof(macid_sub));
	memcpy(baaddr+(sizeof(macid_sub)),macid_company,sizeof(macid_company));
	
	//memset(baaddr,0,sizeof(baaddr));
	//app_lcd_device.display_char ("                ", 0, 0);
	//app_lcd_device.display_char ("                ", 1, 0);
	
    /***********det data check*********************/
    for (i=0;i<BAUDRATE_DET_DATA_LEN/4;i++)
    {
        if (*p1++ != BAUDRATE_DET_DATA)
        {
            ret = WRONG_FILE_FORMAT;
			//app_lcd_device.display_char("wrong file",0,0);
            break;
        }
    }

    BOOT_HEADER *p_header = (BOOT_HEADER *)(fData+BAUDRATE_DET_DATA_LEN);
    /***********header check*********************/
    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        if (p_header->rx_header_start != HEADER_START)
        {
            ret = WRONG_FILE_FORMAT;
			//app_lcd_device.display_char("wrong file",0,0);
        }
        else if (p_header->rx_total_bytes != (dataLen - BAUDRATE_DET_DATA_LEN - sizeof(BOOT_HEADER)))
        {
            ret = WRONG_FILE_FORMAT;
			//app_lcd_device.display_char("wrong file",0,0);
        }
    }

    /***********checksum*********************/
    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        int checksum = 0;
        p1 = (UINT32*)(fData + BAUDRATE_DET_DATA_LEN + sizeof(BOOT_HEADER));
        for(i=0;i<(p_header->rx_total_bytes-4)/4;i++)
        {
            checksum += *p1++;
        }

        if ((checksum != *p1)||(checksum != p_header->rx_checksum))
        {
            ret = CHECKSUM_ERROR;
			//app_lcd_device.display_char("check err",0,0);
        }
    }


//BOOL QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
	//LARGE_INTEGER ticks;
#if 0//
		baaddr[0] = (unsigned char)(macid_0)&0x000000ff;
		baaddr[1] = (unsigned char)(macid_1)&0x000000ff;
		baaddr[2] = (unsigned char)(macid_2)&0x000000ff;

		baaddr[3] = (unsigned char)(macid_3)&0x000000ff;
		baaddr[4] = (unsigned char)(macid_4)&0x000000ff;
		baaddr[5] = (unsigned char)(macid_5)&0x000000ff;
//#else
	U32 U32_ticks = read_ccount();
	if (U32_ticks)
	{
		//char szMessage[70];
		baaddr[0] = (unsigned char)(U32_ticks&0x000000ff);
		baaddr[1] = (unsigned char)(U32_ticks>>8)&0x000000ff;
		baaddr[2] = (unsigned char)(U32_ticks>>16)&0x000000ff;

		//wsprintf(szMessage, "bt bdaddr:0x%02x%02x%02x%02x%02x%02x\r\n",baaddr[5],baaddr[4],baaddr[3],baaddr[2],baaddr[1],baaddr[0]);
		//UpdateStatus(szMessage);
	}
	else
	{
		ret = READ_CPU_TICKS_ERR;
		app_lcd_device.display_char("read clk err",0,0);
	}
#endif
//    "Optek Bt\0\0"
    *p++ = 'S';
    *p++ = 'Q';
    *p++ = '2';
    *p++ = '0';
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
//    *p++ = 0;

//u32 wr_cn;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

// bt mac id
/*    *p++ = 0x11;
    *p++ = 0x12;
    *p++ = 0x13;
    *p++ = 0x14;
    *p++ = 0x15;
    *p++ = 0x16;*/
	memcpy(p,baaddr,sizeof(baaddr));
	p+=6;

	
    p = fData+MAC_ADDR_OFFSET+0X1000+0xb20;

//    "Optek Bt\0\0"
     *p++ = 'S';
    *p++ = 'Q';
    *p++ = '2';
    *p++ = '0';
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
 //   *p++ = 0;

//u32 wr_cn;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
// bt mac id
/*    *p++ = 0x11;
    *p++ = 0x12;
    *p++ = 0x13;
    *p++ = 0x14;
    *p++ = 0x15;
    *p++ = 0x16;*/
	memcpy(p,baaddr,sizeof(baaddr));
	p+=6;

    //a-a5
/*
    buf = buf+0xfc00c;
    buf[0] = a;
    buf[1] = a1;
    buf[2] = a2;
    buf[3] = a3;
    buf[4] = a4;
    buf[5] = a5;
    buf = buf-0xfc00c;
  */

    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        int checksum = 0;
        p1 = (UINT32*)(fData + BAUDRATE_DET_DATA_LEN + sizeof(BOOT_HEADER));
        for(i=0;i<(p_header->rx_total_bytes-4)/4;i++)
        {
            checksum += *p1++;
        }

        *p1 = checksum;
        p_header->rx_checksum = checksum;
		//app_lcd_device.display_char("check ok",0,0);
    }
	return ret;
}
#endif


/*******************************************/
const char dft_bdaddr[6] = {0x00,0x00,0x00,0x3F,0x9f,0x94};
int fileCheck(char *fData, UINT32 dataLen) //char *dataLen)
{
    UINT32 i;
    int ret = FILE_CHCK_SUCCESSFUL;
    char *p = fData+MAC_ADDR_OFFSET+BAUDRATE_DET_DATA_LEN+HEADER_LEN;
    UINT32 *p1 = (UINT32 *)fData;
    unsigned char baaddr[6];

    memcpy(baaddr,dft_bdaddr,sizeof(baaddr));
    LOGD ("fileCheck\n");
    
    /***********det data check*********************/
    for (i=0;i<BAUDRATE_DET_DATA_LEN/4;i++)
    {
        if (*p1++ != BAUDRATE_DET_DATA)
        {
            ret = WRONG_FILE_FORMAT;
            LOGD("11 wrong file\n");
            break;
        }
    }

    BOOT_HEADER *p_header = (BOOT_HEADER *)(fData+BAUDRATE_DET_DATA_LEN);
    /***********header check*********************/
    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        if (p_header->rx_header_start != HEADER_START)
        {
            ret = WRONG_FILE_FORMAT;
            LOGD("22 wrong file\n");
        }
        else if (p_header->rx_total_bytes != (dataLen - BAUDRATE_DET_DATA_LEN - sizeof(BOOT_HEADER)))
        {
            ret = WRONG_FILE_FORMAT;
           LOGD("33 wrong file\n");
        }
    }

    /***********checksum*********************/
    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        UINT32 checksum = 0;
        p1 = (UINT32*)(fData + BAUDRATE_DET_DATA_LEN + sizeof(BOOT_HEADER));
        for(i=0;i<((p_header->rx_total_bytes-4)/4);i++)
        {
            checksum += *p1++;
        }

        if ((checksum != *p1)||(checksum != p_header->rx_checksum))
        {
            ret = CHECKSUM_ERROR;
            LOGD("check err\n");
        }
    }

//BOOL QueryPerformanceCounter(LARGE_INTEGER *lpPerformanceCount)
    //LARGE_INTEGER ticks;
    UINT32 U32_ticks = GetSTC();
    if (U32_ticks)
    {

	LOGD("tick 0x%x\n", U32_ticks);
	
        //char szMessage[70];
        baaddr[0] = (unsigned char)(U32_ticks&0x000000ff);
        baaddr[1] = (unsigned char)(U32_ticks>>8)&0x000000ff;
        baaddr[2] = (unsigned char)(U32_ticks>>16)&0x000000ff;

        //wsprintf(szMessage, "bt bdaddr:0x%02x%02x%02x%02x%02x%02x\r\n",baaddr[5],baaddr[4],baaddr[3],baaddr[2],baaddr[1],baaddr[0]);
        //UpdateStatus(szMessage);

    }
    else
    {
        ret = READ_CPU_TICKS_ERR;
        LOGD("read clk err\n");
    }

    if ((baaddr[0]==0x9e)&&(baaddr[1]==0x8b) &&((baaddr[2]<=0x3f))) //(baaddr[2]>=0x0)&&
    {
        ret = MACID_USELESS;
    }

//    "Optek Bt\0\0"
    *p++ = 'O';
    *p++ = 'p';
    *p++ = 't';
    *p++ = 'e';
    *p++ = 'k';
    *p++ = ' ';
    *p++ = 'B';
    *p++ = 't';
//    *p++ = 0;

//u32 wr_cn;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

// bt mac id
/*    *p++ = 0x11;
    *p++ = 0x12;
    *p++ = 0x13;
    *p++ = 0x14;
    *p++ = 0x15;
    *p++ = 0x16;*/
    memcpy(p,baaddr,sizeof(baaddr));
    p+=6;

    
    p = fData+MAC_ADDR_OFFSET+0X1000+0xb20;

//    "Optek Bt\0\0"
    *p++ = 'O';
    *p++ = 'p';
    *p++ = 't';
    *p++ = 'e';
    *p++ = 'k';
    *p++ = ' ';
    *p++ = 'B';
    *p++ = 't';
 //   *p++ = 0;

//u32 wr_cn;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;

    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
    *p++ = 0;
// bt mac id
/*    *p++ = 0x11;
    *p++ = 0x12;
    *p++ = 0x13;
    *p++ = 0x14;
    *p++ = 0x15;
    *p++ = 0x16;*/
    memcpy(p,baaddr,sizeof(baaddr));
    p+=6;
    
    //a-a5
/*
    buf = buf+0xfc00c;
    buf[0] = a;
    buf[1] = a1;
    buf[2] = a2;
    buf[3] = a3;
    buf[4] = a4;
    buf[5] = a5;
    buf = buf-0xfc00c;
  */

    if (ret == FILE_CHCK_SUCCESSFUL)
    {
        int checksum = 0;
        p1 = (UINT32*)(fData + BAUDRATE_DET_DATA_LEN + sizeof(BOOT_HEADER));
        for(i=0;i<(p_header->rx_total_bytes-4)/4;i++)
        {
            checksum += *p1++;
        }

        *p1 = checksum;
        p_header->rx_checksum = checksum;

		
        LOGD("check ok, 0x%x, 0x%x, 0x%x\n", baaddr[0], baaddr[1], baaddr[2]);
    }
    return ret;
}

int BT_MCU_UPGING(int fd, BYTE *bin_buf, UINT32 file_size)
{
	#define BT_UPG_UART_BUF_MAX 128

	BT_HANDLE *handle = &bt_handle;
	int check_file_count = 0;
	int fileCheck_status;
	UINT32 i=0;
	#define CHECK_FILE_COUNT_MAX 20
	static BYTE last_vol = 0xff;
	BYTE cur_vol=0;
	BYTE write_len = 0;
	//int len = 0;

	fd = fd;

rf_test_fileCheck_strat:            
    fileCheck_status = fileCheck(bin_buf, file_size); //(BYTE *)file_size);
    LOGD("-0-fileCheck_status:%d;len:%d\n",fileCheck_status,file_size);

    if (fileCheck_status != FILE_CHCK_SUCCESSFUL)
    {
	 if(fileCheck_status == WRONG_FILE_FORMAT)
		return -1;
	
        if (check_file_count< CHECK_FILE_COUNT_MAX-1)
            check_file_count++;
        else
            check_file_count = 0;
        //fill_buf_with_digital_num_uart_upgrade((const U8 *)check_file_count_disp_buf,check_file_count);
        //LOGD("check_file_count : %x", check_file_count);
        delay_1ms(100);
        goto rf_test_fileCheck_strat;
    }
				
	for(i=0; i<file_size; i+=write_len)
   	{
   		write_len = write(handle->fd, bin_buf+i, BT_UPG_UART_BUF_MAX);
		
		LOGD("i %d\n", write_len);

		cur_vol = (i*100)/file_size;
		
		if(cur_vol != last_vol)
		{
			last_vol = cur_vol;
		
			BTEvt *evt = Q_NEW(BTEvt, BT_UPG_PROCESS_SIG);
			evt->vol = (i*100)/file_size;
			QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);
		}

		//memset(packet_buf, 0, UART_BUF_MAX);
		//len = read(handle->fd, packet_buf, UART_BUF_MAX);

		//if(len )
		//	LOGD(" upg len %d, [%s]\n ", len, packet_buf);
	}

	if(i > file_size)
	{
		i = i-BT_UPG_UART_BUF_MAX;
		write(handle->fd, bin_buf+i, file_size-i);
		LOGD(">i %d\n", file_size-i);
		
		BTEvt *evt = Q_NEW(BTEvt, BT_UPG_PROCESS_SIG);
		evt->vol = 100;
		QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);

		//memset(packet_buf, 0, UART_BUF_MAX);
		//len = read(handle->fd, packet_buf, UART_BUF_MAX);

		//if(len )
		//	LOGD(" upg len %d, [%s]\n ", len, packet_buf);
	}

	return 0;
}
/**********************************************/


static int get_file_size(const char* file) 
{
    struct _sTat tbuf;
    stat(file, &tbuf);
    return tbuf.st_size;
}


static int BT_UPG_PROCESS()
{
	BT_HANDLE *handle = &bt_handle;
	BYTE upg_path[] = {"USB:SQ20_BT.bin"};
	UINT32 file_size = 0;
	int upgrade_fd = -1;
	BYTE *upgrade_buf = NULL;
	int ret = -1;
	BTEvt *evt;

	BT_MCU_UPG();

	delay_1ms(100);

//	mount("USB", "USB:", "fsfat");

 	upgrade_fd = open(upg_path,  O_RDONLY, 0);
	
        if (upgrade_fd < 0)
        {  
        	LOGD(" open bt bin fail\n ");
		ret = -1;
		goto UPG_EXIT;
	}
	else
        {
        	file_size = get_file_size(upg_path);
		LOGD(" open bt bin ok, file_size %d\n ", file_size);
        }
		

	upgrade_buf = (BYTE *)malloc(file_size);
	if(upgrade_buf != NULL)
	{
		file_size = read(upgrade_fd, upgrade_buf, file_size);
		delay_1ms(100);
		if(BT_MCU_UPGING(upgrade_fd,upgrade_buf, file_size) == 0)
		{
			LOGD("bt upg OK\n ");
			ret = 0;
		}
		else
		{
			LOGD("bt upg ERR\n ");
			ret = -1;
		}

		free(upgrade_buf);
		
	}
	else
	{
		LOGD(" malloc bt upg buf error\n ");
		ret = -1;
	}

	close(upgrade_fd);

	delay_1ms(1500);

UPG_EXIT:	

	evt = Q_NEW(BTEvt, BT_UPG_PROCESS_SIG);
	if(ret == 0)
		evt->vol = BT_UPG_OK;
	else
		evt->vol = BT_UPG_NG;
	QACTIVE_POST(UserApp_get(),  (QEvt*)evt, NULL);

	handle->bt_upg = 0;
	
	return ret;

}

#endif


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

					handle->bt_ver_cnt = 200;
					handle->src_timer = BT_SW_SRC_TIMEOUT;
					handle->bt_on = 1;
					BT_PIN_SetLow();
					GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
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
					handle->src = BT_MODE_AUX;
					BT_PIN_SetLow();
					GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
				}
			}
	
			handle->play = 0;
			
			break;
		}
		case BT_UPG_SIG:
			LOGD("=============== %s\n ", __func__);
			BT_UPG_PROCESS(); //
			break;

		case BT_PC_UPG_SIG:
			LOGD("BT_PC_UPG_SIG %s\n ", __func__);
			BT_PC_UPG();
			break;
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


