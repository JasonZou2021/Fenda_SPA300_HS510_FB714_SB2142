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
#include "BT_BK8000.h"

#define UART_BUF_MAX	128
static uint8_t *packet_buf;

#define TICK_TIME_3S	(15) // 3000ms / 200ms   
#define TICK_TIME_7S	(35) // 7000ms / 200ms  

#define BT_AUX_SW_PIN	(3)  // H: bt source; L: auxin source
#define BT_POWER_PIN	(54) 

static F6688_BT_HANDLE  F6688_handle;


#define RECONNECT_CNT	(18000)   // 3min
#define POWEROFF_CNT	(40)  // 400ms
#define AUXMUTE_CNT	(500)  // 1.5s
#define BTMUTE_CNT		(80)  // 1.5s

#define POPMUTE_TIMEOUT	(100)

#define BT_HINTSOUND_VOLUME	(30)

#define BT_BK8000_BAUDRATE	 9600

extern QActive *UserApp_get(void);

int BT_BK8000_CheckIsLink(void)
{
	return (F6688_handle.phone_state > F6688_DISCONNECT);
}

int BT_BK8000_CheckIsPlay(void)
{
	return (F6688_handle.phone_state == F6688_PLAY);
}

static int BT_BK8000_SendCmd(BYTE cmd)
{
	write(F6688_handle.fd, bt_cmd[cmd], strlen(bt_cmd[cmd]));

	return 0;

}

/*
static int BT_BK8000_Mute(UserApp *me)
{
	
	//UserAppMute(me, 1); 

	return 0;
}


static int BT_BK8000_Demute(UserApp *me)
{

	//UserAppMute(me, 0); 

	return 0;
}
*/

static int BT_BK8000_CommandHandle(unsigned char *cmd)
{
	BYTE i=0;
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	//static BYTE last_status = 0;
	

	for(i=0; i<BTRESP_MAX; i++)
	{
		if(strncmp(bt_resp[i], cmd, strlen(bt_resp[i])) == 0)
		{		

			LOGD(">>>>>>>>> %s [ %s ]\n ", __func__, bt_resp[i]);
		
			switch(i)
			{
				case BTRESP_CONNECTED:		
				case BTRESP_CONNECTED2:
					if(btHandle->bt_state == F6688_ON && btHandle->phone_state < F6688_CONNECT)
					{
						btHandle->phone_state = F6688_CONNECT;
						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_CONNECT_SOUNDHINT_SIG), NULL);
					}
					break;

				case BTRESP_DISCONNECT:

					//printf(" BT DISCONNECT\n ");
					
					if((btHandle->bt_state == F6688_ON ||btHandle->poweroff_cnt ) && btHandle->phone_state > F6688_DISCONNECT)
					{	
						btHandle->phone_state = F6688_DISCONNECT;

						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_READY_SOUNDHINT_SIG), NULL);
					}
					
					break;
					
				case BTRESP_PLAY:  // MB

					if(btHandle->bt_state == F6688_ON)
					{
						if(btHandle->phone_state < F6688_CONNECT)
							QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_READY_SOUNDHINT_SIG), NULL);


						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_PLAY_STATUS_SIG), NULL);
						
						btHandle->phone_state = F6688_PLAY;

						//if(me->mute == eMute_off)
						{
						//	BT_BK8000_Mute(me);
							btHandle->mute_cnt = 5;  // mute pop 
						}
					
					}
					break;
					
				case BTRESP_PAUSE:  // MA 

					if(btHandle->bt_state == F6688_ON)
					{
						btHandle->phone_state = F6688_PAUSE;

						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_PAUSE_STATUS_SIG), NULL);

						//BT_BK8000_Mute(me); 
					}
					
					break;
					
				case BTRESP_POWER_ON:

					if(btHandle->bt_state == F6688_ON)
					{
						btHandle->phone_state = F6688_PAIR;
						//BT_BK8000_Mute(me);  // will demute when PLAY bt
					}
					else if(btHandle->bt_state == F6688_AUX)
					{
						btHandle->phone_state = F6688_IDLE;
						//btHandle->mute_cnt = AUXMUTE_CNT;
					}
					
					break;

				case BTRESP_STANDBY:
					btHandle->phone_state = F6688_IDLE;
					break;

				case BTRESP_OK: 
					break;

				case BTCMD_MUTE:  // PE

					if(btHandle->bt_state == F6688_ON || btHandle->bt_state == F6688_AUX)
					{
						if(btHandle->bt_mute == 0)
						{
							btHandle->bt_mute = 1;

						//	if(btHandle->phone_state != F6688_PLAY)
						//		BT_BK8000_Mute(me);	
						}

						//if(me->Kara_effect->kok_on_off != 0) 
						//	btHandle->mute_cnt = POPMUTE_TIMEOUT;

						QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_MUTE_ON_SIG), NULL);
						
					}
					
					break;

				case BTCMD_DEMUTE:  // PF
					
					if(btHandle->bt_state == F6688_ON || btHandle->bt_state == F6688_AUX)
					{
						btHandle->bt_mute = 0;
					
						//if(me->mute == eMute_off)
						{
							btHandle->mute_cnt = 0;

						//	BT_BK8000_Demute(me);

							QACTIVE_POST(UserApp_get(),  Q_NEW(QEvt, BT_MUTE_OFF_SIG), NULL);

						}
					}
					
					break;

				case BTRESP_44k:
					//if(btHandle->samp != 0)
					{
						btHandle->samp = 0;
						AudDspService_Set_OutSampleRate(NULL, 44100);
						LOGD(">>>>>>>>> %s [  sample rate 44.1K  ]\n ", __func__);
					}
					break;
					
				case BTRESP_48k:
					//if(btHandle->samp != 1)
					{
						btHandle->samp = 1;
						AudDspService_Set_OutSampleRate(NULL, 48000);
						LOGD(">>>>>>>>> %s [  sample rate 48K  ]\n ", __func__);
					}
					break;

						
					
				default:
					break;
			}
			
			return 1;
		}
	}

	return 0;
}

static int BT_BK8000_PowerOn_BT(void)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;

	//BT_BK8000_Mute(me); 
	
	LOGD(">>>>>>>>> %s\n ", __func__);

	btHandle->bt_state = F6688_ON;
	btHandle->phone_state = F6688_PAIR;
	btHandle->reconnect_cnt = RECONNECT_CNT;
	btHandle->poweroff_cnt = 0;
	//btHandle->mute_cnt = BTMUTE_CNT;

	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
	delay_1ms(10);
	GPIO_Output_Write(BT_AUX_SW_PIN, IO_HIGH);
	GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);


	btHandle->samp = 0xff;
	
	//UserAppDisplay_Blink(me, STR_BT,10,10);

	return 0;
	
}


static int BT_BK8000_PowerOn_AUX(void)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	//UserApp *me = UserApp_getValue();

	LOGD(">>>>>>>>> %s %d\n ", __func__, btHandle->bt_state);

	//BT_BK8000_Mute(me); 
	//me->aux_mute = eMute_on;

	if(btHandle->bt_state == F6688_OFF && btHandle->poweroff_cnt == 0) 
		btHandle->mute_cnt = AUXMUTE_CNT;
	else
		btHandle->mute_cnt = BTMUTE_CNT;	

	btHandle->bt_state = F6688_AUX;
	btHandle->phone_state = F6688_IDLE;
	btHandle->reconnect_cnt = 0;
	btHandle->poweroff_cnt = 0;

	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
	delay_1ms(10);
	GPIO_Output_Write(BT_AUX_SW_PIN, IO_LOW);
	GPIO_Output_Write(BT_POWER_PIN, IO_HIGH);

	btHandle->samp = 0xff;
	
	return 0;
	
}




static int BT_BK8000_PowerOff(void)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	//UserApp *me = UserApp_getValue();

	if(btHandle->phone_state > F6688_DISCONNECT)
	{
		btHandle->bt_state = F6688_WAIT_OFF;
		BT_BK8000_SendCmd(BTCMD_DISCONNECT);
	}
	else
	{
		btHandle->bt_state = F6688_OFF;
		GPIO_Output_Write(BT_POWER_PIN, IO_LOW); //BT_BK8000_SendCmd(BTCMD_POWER_OFF);
	}
	
	btHandle->phone_state = F6688_IDLE;
	btHandle->reconnect_cnt = 0;
	btHandle->mute_cnt = 0;
	btHandle->poweroff_cnt = POWEROFF_CNT;

	//me->aux_mute = eMute_off;

	LOGD(">>>>>>>>> %s\n ", __func__);

	return 0;

}




static int BT_BK8000_Init(QActive *me)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	(void)me;

	packet_buf = malloc(256);

	Pinmux_GPIO_Free(BT_POWER_PIN);
	Pinmux_GPIO_Alloc(BT_POWER_PIN,IO_GPIO,IO_RISC);
	GPIO_OE_Set(BT_POWER_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
	
	Pinmux_GPIO_Free(BT_AUX_SW_PIN);
	Pinmux_GPIO_Alloc(BT_AUX_SW_PIN,IO_GPIO,IO_RISC);
	GPIO_OE_Set(BT_AUX_SW_PIN, IO_OUTPUT);
	GPIO_Output_Write(BT_AUX_SW_PIN, IO_LOW);
	
	btHandle->bt_state = F6688_OFF;
	btHandle->phone_state = F6688_IDLE;
	btHandle->reconnect_cnt = 0;
	btHandle->poweroff_cnt = 0;
	btHandle->mute_cnt = 0;

	btHandle->fd = open("C:/uart2", O_RDWR, 0);
	ioctl(btHandle->fd, UART_NORMAL_MODE|DATA_BITS_8|STOP_BITS_1|PARITY_NONE|FLOW_CONTROL_NONE, BT_BK8000_BAUDRATE);
	ioctl(btHandle->fd, UART_CMD_READ_TIMEOUT ,portMAX_DELAY);

	LOGD(" %s fd %d\n ", __func__, btHandle->fd);
	
	return BT_BK8000_BAUDRATE;
}



static int BT_BK8000_Read(QActive *me, uint8_t *buf, unsigned int size)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	static unsigned char receivedata[F6688_DATA_MAX];
	static BYTE receivecnt = 0;
	BYTE i=0;
	(void)me;

	if(btHandle->bt_state == F6688_OFF && btHandle->poweroff_cnt == 0)
		return -1;

	for(i=0; i<size; i++)
	{	

		if((buf[i] < 'A' || buf[i] > 'Z') && buf[i] != '\n' && buf[i] != '\r' && buf[i] != '4' && buf[i] != '8')
		{
			receivecnt = 0;
			continue;
		}

		receivedata[receivecnt] = buf[i];

		if(receivecnt > 0 && receivedata[receivecnt] == '\n')// && receivedata[receivecnt-1] == '\r')
		{
			BT_BK8000_CommandHandle(receivedata);
			receivecnt = 0;
			continue;
		}
		else
		{
			if(receivecnt >= F6688_DATA_MAX-1)
				receivecnt = 0;
		}

		receivecnt++;
	}

	return 0;
	
}

int BT_BK8000_Polling(QActive *me)
{
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	int len;

	if(packet_buf != NULL && btHandle->fd > 0)
	{
		memset(packet_buf, 0, UART_BUF_MAX);
		len = read(btHandle->fd, packet_buf, UART_BUF_MAX);
		if(len > 0)
		{
			BT_BK8000_Read(me, packet_buf, len);
		}
	}

/*	
	// reconnect
	if(btHandle->reconnect_cnt)
	{
		if(btHandle->bt_state != F6688_ON)
			btHandle->reconnect_cnt = 0;
		else
		{
			btHandle->reconnect_cnt--;
			if(btHandle->reconnect_cnt == 0)
			{
				BT_BK8000_SendCmd(BTCMD_ENTER_PAIR);
				btHandle->phone_state = F6688_PAIR;
			}

			if(btHandle->phone_state > F6688_DISCONNECT)
				btHandle->reconnect_cnt = 0;
		}
	}
*/
	// power off flow
	if(btHandle->poweroff_cnt)
	{
		btHandle->poweroff_cnt--;
		if(btHandle->poweroff_cnt == 0)
		{
			if(btHandle->bt_state == F6688_WAIT_OFF)
			{
				GPIO_Output_Write(BT_POWER_PIN, IO_LOW); //BT_BK8000_SendCmd(BTCMD_POWER_OFF);
				btHandle->bt_state = F6688_OFF;
				btHandle->poweroff_cnt = POWEROFF_CNT;
			}
			else if(btHandle->bt_state == F6688_OFF)
			{
				//GPIO_Output_Write(BT_POWER_PIN, IO_LOW);
				LOGD(">>>>>>>>> %s  POWER off\n ", __func__);
			}
		}
	}

	return 0;
}


int BT_BK8000_Key(QActive *me, int sig, int key) 
{
	static BYTE cnt;
	F6688_BT_HANDLE *btHandle = &F6688_handle;
	(void)me;
	key=key;

	//BK8000_printf1(">>>>>>>>> %s  sig %d\n ", __func__, sig);
	
	switch(sig)
	{

		case POWER_RELEASE_SIG:
			BT_BK8000_PowerOff();
			break;
	
		case PLAY_RELEASE_SIG:
		case SELECT_RELEASE_SIG:
			cnt = 0;
			if(btHandle->bt_state == F6688_ON && btHandle->phone_state > F6688_DISCONNECT)
				BT_BK8000_SendCmd(BTCMD_PALYPAUSE);
			break;

		case NEXT_RELEASE_SIG:
			if(btHandle->bt_state == F6688_ON && btHandle->phone_state > F6688_DISCONNECT)
				BT_BK8000_SendCmd(BTCMD_NEXT);
			break;

		case PRE_RELEASE_SIG:
			if(btHandle->bt_state == F6688_ON && btHandle->phone_state > F6688_DISCONNECT)
				BT_BK8000_SendCmd(BTCMD_PREV);
			break;

		case PAIR_RELEASE_SIG:
			if(btHandle->bt_state == F6688_ON)
			{
				BT_BK8000_SendCmd(BTCMD_ENTER_PAIR);
				btHandle->phone_state = F6688_PAIR;
			}
			
			break;

		case STOP_LONG_PRESS_SIG:
			btHandle->phone_state = F6688_IDLE;
			BT_BK8000_SendCmd(BTCMD_CLEAR);
			if(btHandle->bt_state == F6688_ON)
			{
				//UserAppDisplay_Blink(me, STR_BT,10,10);
			}
			break;

		case UARTAPP_SOURCE_BT_ENTRY_SIG:
			BT_BK8000_PowerOn_BT();
			break;
			
		case UARTAPP_SOURCE_AUX_ENTRY_SIG:
			BT_BK8000_PowerOn_AUX();
			break;

		case UARTAPP_SOURCE_AUX_EXIT_SIG:
			if(btHandle->bt_state != F6688_ON)
				BT_BK8000_PowerOff();
			break;
		
		case UARTAPP_SOURCE_BT_EXIT_SIG:
			if(btHandle->bt_state != F6688_AUX)
				BT_BK8000_PowerOff();
			break;
			
		default:
			break;
	}

	return 0;
}

UartApp_Func BT_BK8000 = 
{
	BT_BK8000_Init,
	BT_BK8000_Read,
	BT_BK8000_Polling,
	BT_BK8000_Key,
	NULL,
};


