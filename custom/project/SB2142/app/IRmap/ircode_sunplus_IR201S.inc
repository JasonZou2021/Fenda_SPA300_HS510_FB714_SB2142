//lixuguang   ,  2005-4-29 20:41
const BYTE ir_mapcode[] =
{
	IRC_EJECT,    IRC_3,     	    IRC_6,		        IRC_9,      //0x00, 0x01 ,0x02, 0x03
	IRC_NULL,     IRC_2,          IRC_5,  		    IRC_8,      //0x04, 0x05 ,0x06, 0x07
	IRC_NULL,     IRC_1,          IRC_4, 			    IRC_7,      //0x08, 0x09 ,0x0a, 0x0b

#if defined(DVB_DVD_8202)&&defined(DIRECT_ACCESS_MEDIA)
	IRC_POWER,    IRC_VOLUME_UP,  IRC_VOLUME_DN,      IRC_SW_MEDIA,
#else
	IRC_POWER,    IRC_VOLUME_UP,  IRC_VOLUME_DN,      IRC_MUTE, //0x0c, 0x0d ,0x0e, 0x0f
#endif

	IRC_FORMAT,    IRC_PAUSE,    	IRC_ANGLE,       IRC_PREV,   //0x10, 0x11 ,0x12, 0x13
	IRC_P_SCAN,    IRC_SLOW,      IRC_3D,          IRC_SUBP,         //0x14, 0x15 ,0x16, 0x17
	IRC_GOTO,      IRC_PREVIEW,   IRC_AB,          IRC_NULL,         //0x18, 0x19 ,0x1a, 0x1b
	IRC_SETUP,     IRC_REPEAT,	IRC_ZOOM, 	     IRC_BACKWARD,         //0x1c, 0x1d ,0x1e, 0x1f

	IRC_NULL,  	 IRC_NULL,		IRC_NULL,  		IRC_NULL,    //0x20, 0x21 ,0x22, 0x23
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,    //0x24, 0x25 ,0x26, 0x27
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,    //0x28, 0x29 ,0x2a, 0x2b
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,    //0x2c, 0x2d ,0x2e, 0x2f

	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,     //0x30, 0x31 ,0x32, 0x33
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,     //0x34, 0x35 ,0x36, 0x37
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,     //0x38, 0x39 ,0x3a, 0x3b
	IRC_NULL,      IRC_NULL,		IRC_NULL,  		IRC_NULL,     //0x3c, 0x3d ,0x3e, 0x3f

	IRC_0,  	     IRC_CHANNEL,	    IRC_RIGHT, 		IRC_PROGRAM,    //0x40, 0x41 ,0x42, 0x43
	IRC_P10,   	 IRC_AUD_LANGUAGE, 	IRC_DISPLAY,    IRC_UP,//0x44, 0x45 ,0x46, 0x47
	IRC_TV_RADIO,  IRC_PBC,	        IRC_SELECT,     IRC_DOWN,     //0x48, 0x49 ,0x4a, 0x4b
#ifdef IR_SWITCH_DVD_USB_CARD_AVIN_DVB//Just for test
	IRC_DVD_USB_CARD_AVIN_DVB,   IRC_RETURN,        IRC_LEFT,		IRC_TITLEMENU,     //0x4c, 0x4d ,0x4e, 0x4f
#else
	IRC_DVB_DVD,   IRC_RETURN,        IRC_LEFT,		IRC_TITLEMENU,     //0x4c, 0x4d ,0x4e, 0x4f
#endif

	IRC_NEXT,  	   IRC_NULL,       IRC_NULL,     	IRC_NULL,   //0x50, 0x51 ,0x52, 0x53
	IRC_PLAY_RESUME, IRC_STOP,       IRC_NULL,   	    IRC_NULL,   //0x54, 0x55 ,0x56, 0x57
	IRC_NULL,        IRC_NULL,       IRC_NULL,  		IRC_NULL,     //0x58, 0x59 ,0x5a, 0x5b
	IRC_FORWARD,     IRC_NULL,	   IRC_NULL,  		IRC_NULL, //0x5c, 0x5d ,0x5e, 0x5f

	IRC_NULL,   	IRC_NULL,		    IRC_NULL,  		IRC_NULL,     //0x60, 0x61 ,0x62, 0x63
	IRC_NULL,     IRC_NULL,           IRC_NULL,       IRC_NULL,//0x64, 0x65 ,0x66, 0x67
	IRC_NULL,  	IRC_NULL,      	    IRC_NULL,    	IRC_NULL,     //0x68, 0x69 ,0x6a, 0x6b

};
