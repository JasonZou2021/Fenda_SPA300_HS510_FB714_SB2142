#ifndef __BK8000_H__
#define __BK8000_H__

#include "qp_pub.h"




#define F6688_DATA_MAX 		10
#define F6688_PACKET_MAX		5

#define BTCMD_LEN	8
#define BTRESP_LEN	5

enum
{
	BTCMD_ENTER_PAIR,
	BTCMD_EXIT_PAIR,
	BTCMD_CONNECT,		// re-connect the last device
	BTCMD_DISCONNECT,
	BTCMD_POWER_OFF,
	BTCMD_CLEAR,
	BTCMD_PALYPAUSE,
	BTCMD_STOP,
	BTCMD_NEXT,
	BTCMD_PREV,
	BTCMD_FORWARD,
	BTCMD_BACKWARD,
	BTCMD_PROMPT_TONE ,
	BTCMD_MAX
};


enum
{
	BTRESP_CONNECTED,
	BTRESP_CONNECTED2,
	BTRESP_DISCONNECT,
	BTRESP_PLAY,
	BTRESP_PAUSE,
	BTRESP_POWER_ON,
	BTRESP_STANDBY,
	BTRESP_OK,
	//BTRESP_PROMPT_TONE_START,
	//BTRESP_PROMPT_TONE_END,

	BTCMD_MUTE,
	BTCMD_DEMUTE,

	BTRESP_44k,
	BTRESP_48k,
	
	BTRESP_MAX
};


typedef enum
{
	F6688_IDLE,
	F6688_PAIR,
	F6688_DISCONNECT,
	F6688_CONNECT,
	F6688_PLAY,
	F6688_PAUSE,
	F6688_STOP,
	F6688_MAX

}F6688_STATE;


enum
{
	F6688_OFF,
	F6688_WAIT_OFF,
	F6688_ON,	
	F6688_AUX,	
};


unsigned char bt_cmd[BTCMD_MAX][BTCMD_LEN] =
{					
	"AT+CA\r\n",   //	BTCMD_ENTER_PAIR
	"AT+CB\r\n",   // 	BTCMD_EXIT_PAIR,
	"AT+CC\r\n",   // 	BTCMD_CONNECT,		
	"AT+CD\r\n",	 //	BTCMD_DISCONNECT,
	"AT+CP\r\n",	 //	BTCMD_POWER_OFF,
	"AT+CZ\r\n",	 // 	BTCMD_CLEAR,
	"AT+MA\r\n",	 // 	BTCMD_PALYPAUSE,
	"AT+MC\r\n",   //	BTCMD_STOP,
	"AT+MD\r\n",	 //	BTCMD_NEXT,
	"AT+ME\r\n",	 // 	BTCMD_PREV,
	"AT+MF\r\n",	 // 	BTCMD_FORWARD,
	"AT+MH\r\n",	 //	BTCMD_BACKWARD,
	"AT+PV\r\n",	 //	BTCMD_PROMPT_TONE ,
};


unsigned char bt_resp[BTRESP_MAX][BTRESP_LEN] = 
{
	"MK", 
	"II",
	"IA",
	"MB",
	"MA",
	"ON",
	"ST",
	"OK",
	//"PS",
	//"PE",

	"PE", // mute
	"PF", // demute

	"SP44",
	"SP48"
};


typedef struct 
{
	int fd;
	BYTE bt_state;
	F6688_STATE phone_state;
	UINT16 reconnect_cnt;
	UINT16 poweroff_cnt;
	UINT16 mute_cnt;

	UINT16 bt_mute;
	BYTE samp;
}F6688_BT_HANDLE;


#endif

