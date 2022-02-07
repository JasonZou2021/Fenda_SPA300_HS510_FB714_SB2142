
int UserApp_SetBass(UserApp *const me, BYTE vol);
int UserApp_SetTreble(UserApp *const me, BYTE vol);
QActive *TouchApp_get(void);
int UserAppSurroundSet(QActive *me, int State);
int UserAppDisplayOnce(UserApp *const me, BYTE *str, BYTE time);
void UserApp_ScrollBackUp_ShowSource(UserApp * const me, int show_id);
int IT6622_SendVendorCmd(stUserCECEvt_t *cmd);

#ifdef SUPPORT_RUKOTV
enum
{
	ROKU_AnnouncePresence = 0X10,  	// tv send
	ROKU_InitSession = 0X11,			// soundbar send	
	ROKU_AcceptSession = 0X12,		// tv send

	ROKU_UnsupportVersion = 0x14, // soundbar send	v2.0
	ROKU_ResetSession = 0x15,  // tv send v2.0

	ROKU_SetState = 0X30,  			// tv send 
	ROKU_ReportState = 0X31, 		// soundbar send

	ROKU_Reset = 0X33, 		// tv send , soundbar need send ROKU_ReportState
	ROKU_ReportFirmwareVersion = 0x34,  // soundbar send, soudnbar ver
};


#define ROKU_PRODUCT_ID	150
#define ROKU_FEATURE_ID		87



#define RUKO_MAJOR_VER  3
#define RUKO_MINJOR_VER  0

#define RUKO_VER  (RUKO_MAJOR_VER<<3|RUKO_MINJOR_VER)
#define RUKO_2p0_VER  (2<<3)



void UserApp_ROKUCmd_Send(UserApp * const me, BYTE cmd, BYTE *data, BYTE len)
{
	BYTE i=0;

	stUserCECEvt_t * RequestCEC = Q_NEW(stUserCECEvt_t,HDMI_SEND_VENDOR_CMD_SIG);
	RequestCEC->Header = 0x50; //26; //;
	RequestCEC->Opcode = 0xA0;
	RequestCEC->MsgLen = 2+4+len;
	RequestCEC->Operand[0] = 0x8a;
	RequestCEC->Operand[1] = 0xc7;
	RequestCEC->Operand[2] = 0x2e;
	RequestCEC->Operand[3] = cmd;
	for(i=0; i<len; i++)
		RequestCEC->Operand[4+i] =data[i];
	
	ap_printf(" %s \n ", __func__);
	
	QACTIVE_POST(TouchApp_get(),(QEvt *)RequestCEC,me);
}


void UserApp_ROKU_Setting_Send(UserApp * const me)
{
	BYTE cmd_buf[3] = {0};
	cmd_buf[0] = ROKU_FEATURE_ID;  // Feature set ID, allocated by Roku
	cmd_buf[1] = (me->EQMode<<4)|me->Surround_3D;
	if(me->Bass_vol < 5)
	{
		if(me->Treble_vol < 5)
			cmd_buf[2] = ((6+me->Bass_vol)<<4)|(6+me->Treble_vol);
		else
			cmd_buf[2] = ((6+me->Bass_vol)<<4)|(me->Treble_vol-5);
	}
	else
	{
		if(me->Treble_vol < 5)
			cmd_buf[2] = ((me->Bass_vol-5)<<4)|(6+me->Treble_vol);
		else
			cmd_buf[2] = ((me->Bass_vol-5)<<4)|(me->Treble_vol-5);
		
	}
	UserApp_ROKUCmd_Send(me, ROKU_ReportState, cmd_buf, 3);
}


void UserApp_ROKU_Standby_Send(BYTE cmd, BYTE *data, BYTE len)
{
	BYTE i=0;

	stUserCECEvt_t RequestCEC;
	RequestCEC.Header = 0x50; 
	RequestCEC.Opcode = 0xA0;
	RequestCEC.MsgLen =  4+2+len;// need +2
	RequestCEC.Operand[0] = 0xA8;
	RequestCEC.Operand[1] = 0x82;
	RequestCEC.Operand[2] = 0x00;
	RequestCEC.Operand[3] = cmd;  //OK
	for(i=0; i<len; i++)
		RequestCEC.Operand[4+i] =data[i];

	LOGD(" %s \n ", __func__);
	
	IT6622_SendVendorCmd(&RequestCEC);
}


void UserApp_RukoReset(UserApp * const me)
{
	me->Bass_vol = 5;
	UserSetting_SaveBass(me->Bass_vol);
	UserApp_SetBass(me, me->Bass_vol);

	me->Treble_vol = 5;
	UserSetting_SaveTreble(me->Treble_vol);
	UserApp_SetTreble(me, me->Treble_vol);

	me->EQMode = 0;
	UserSetting_SaveEQ(me->EQMode);
	AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER0, MODE1+me->EQMode);

	me->Surround_3D = 1;
	UserSetting_SaveSurround(me->Surround_3D);
	UserAppSurroundSet((QActive *)me, me->Surround_3D);


	UserApp_ROKU_Setting_Send(me);
	
}

void UserApp_ROKU_Standby_Handle(UserApp * const me, stUserCECEvt_t *eCECRX)
{
	BYTE cmd_buf[8];
	BYTE ver = AND_VER;

	switch(eCECRX->Operand[3])
	{
		case ROKU_AnnouncePresence:
			
			ap_printf("ROKU AnnouncePresence\n");
			if(eCECRX->Operand[4] >= RUKO_VER) // if TV ver greater then our ver, beacause our certified 3.0, so need send InitSession
			{
				cmd_buf[0] = RUKO_VER; 
				cmd_buf[1] = ROKU_PRODUCT_ID; // product ID assigned by Roku
				memcpy(&cmd_buf[2],me->device_id, 6); // Unique Device Identification (it is bt address)
				UserApp_ROKU_Standby_Send(ROKU_InitSession, cmd_buf, 8);
			}
			else if(eCECRX->Operand[4] == RUKO_2p0_VER) // if TV send 2.0, send unspport
			{
				cmd_buf[0] = RUKO_VER; 
				cmd_buf[1] = ROKU_PRODUCT_ID; // product ID assigned by Roku
				memcpy(&cmd_buf[2],me->device_id, 6); // Unique Device Identification (it is bt address)
				UserApp_ROKU_Standby_Send(ROKU_UnsupportVersion, cmd_buf, 8);
			}

			break;

		case ROKU_ResetSession:
			ap_printf("ROKU ResetSession\n");
			break;

		case ROKU_AcceptSession:

			ap_printf("ROKU AcceptSession\n");
			cmd_buf[0] = ver/10+'0';
			cmd_buf[1] = '.';
			cmd_buf[2] = ver%10+'0';
			UserApp_ROKU_Standby_Send(ROKU_ReportFirmwareVersion, cmd_buf, 3);
			break;

		case ROKU_SetState:
			
			cmd_buf[0] = ROKU_FEATURE_ID; // Feature set ID, allocated by Roku

			ap_printf(" ROKU SetState len %d\n ", eCECRX->MsgLen);
			
			if(eCECRX->MsgLen == 6)  // <empty> cmd
			{
				cmd_buf[1] = (me->EQMode<<4)|me->Surround_3D;
				if(me->Bass_vol < 5)
				{
					if(me->Treble_vol < 5)
						cmd_buf[2] = ((6+me->Bass_vol)<<4)|(6+me->Treble_vol);
					else
						cmd_buf[2] = ((6+me->Bass_vol)<<4)|(me->Treble_vol-5);
				}
				else
				{
					if(me->Treble_vol < 5)
						cmd_buf[2] = ((me->Bass_vol-5)<<4)|(6+me->Treble_vol);
					else
						cmd_buf[2] = ((me->Bass_vol-5)<<4)|(me->Treble_vol-5);
					
				}
				ap_printf("ROKU SetState <empty>\n");
			}	
			
			UserApp_ROKU_Standby_Send(ROKU_ReportState, cmd_buf, 3);
			break;


		default:
			break;
	}

}


void UserApp_ROKU_Handle(UserApp * const me, stUserCECEvt_t *eCECRX)
{
	BYTE cmd_buf[8];
	BYTE ver = AND_VER;
	static BYTE strBuf[10];

	switch(eCECRX->Operand[3])
	{
		case ROKU_AnnouncePresence:
			
			ap_printf("ROKU AnnouncePresence\n");
			if(eCECRX->Operand[4] >= RUKO_VER) // if TV ver greater then our ver, beacause our certified 2.0, so need send unsupprt verion
			{
				cmd_buf[0] = RUKO_VER; 
				cmd_buf[1] = ROKU_PRODUCT_ID; // product ID assigned by Roku
				memcpy(&cmd_buf[2],me->device_id, 6); // Unique Device Identification (it is bt address)
				UserApp_ROKUCmd_Send(me, ROKU_InitSession, cmd_buf, 8);
			}
			else if(eCECRX->Operand[4] >= RUKO_2p0_VER) // if TV ver = our ver, send initsession
			{
				cmd_buf[0] = RUKO_VER; 
				cmd_buf[1] = ROKU_PRODUCT_ID; // product ID assigned by Roku
				memcpy(&cmd_buf[2],me->device_id, 6); // Unique Device Identification (it is bt address)
				UserApp_ROKUCmd_Send(me, ROKU_UnsupportVersion, cmd_buf, 8);
			}

			break;

		case ROKU_ResetSession:
			ap_printf("ROKU ResetSession\n");
			break;

		case ROKU_AcceptSession:

			ap_printf("ROKU AcceptSession\n");
			cmd_buf[0] = ver/10+'0';
			cmd_buf[1] = '.';
			cmd_buf[2] = ver%10+'0';
			UserApp_ROKUCmd_Send(me, ROKU_ReportFirmwareVersion, cmd_buf, 3);
			break;

		case ROKU_SetState:
			
			cmd_buf[0] = ROKU_FEATURE_ID; // Feature set ID, allocated by Roku

			ap_printf(" ROKU SetState len %d\n ", eCECRX->MsgLen);
			
			if(eCECRX->MsgLen == 6)  // <empty> cmd
			{
				cmd_buf[1] = (me->EQMode<<4)|me->Surround_3D;
				if(me->Bass_vol < 5)
				{
					if(me->Treble_vol < 5)
						cmd_buf[2] = ((6+me->Bass_vol)<<4)|(6+me->Treble_vol);
					else
						cmd_buf[2] = ((6+me->Bass_vol)<<4)|(me->Treble_vol-5);
				}
				else
				{
					if(me->Treble_vol < 5)
						cmd_buf[2] = ((me->Bass_vol-5)<<4)|(6+me->Treble_vol);
					else
						cmd_buf[2] = ((me->Bass_vol-5)<<4)|(me->Treble_vol-5);
					
				}

				ap_printf("ROKU SetState <empty>\n");
			}
			else
			{
				ap_printf("ROKU SetState 0x%x, 0x%x\n", eCECRX->Operand[5], eCECRX->Operand[6]);

				if(eCECRX->Operand[4] == ROKU_FEATURE_ID)
				{
					if((eCECRX->Operand[5] & 0xf0) != 0xf0)
					{

						me->EQMode = eCECRX->Operand[5]>>4;
						UserSetting_SaveEQ(me->EQMode);

						UserAppDisplayOnce(me, vfd_str[STR_EQ0 + me->EQMode],3);
						AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER0, MODE1+me->EQMode);
			
					}
					
					if((eCECRX->Operand[5] & 0x0f) != 0x0f)
					{
						me->Surround_3D = eCECRX->Operand[5]&0x0f;
						if(me->Surround_3D < 2)
						{
					
							UserApp_ScrollBackUp_ShowSource(me, STR_SU0 + me->Surround_3D);

							UserAppSurroundSet((QActive *)me, me->Surround_3D);
							UserSetting_SaveSurround(me->Surround_3D);	
					
					
						}
						
					}
					
					if((eCECRX->Operand[6] & 0xf0) != 0xf0)
					{
						me->Bass_vol = eCECRX->Operand[6]>>4;

						ap_printf(" ROKU set bass %d\n ", me->Bass_vol);

						UserSetting_SaveBass(me->Bass_vol);
						UserApp_SetBass(me, me->Bass_vol);
						
						if(me->Bass_vol > 5)
							sprintf(strBuf, "BAS+%d", me->Bass_vol-5);
						else if(me->Bass_vol < 6)
							sprintf(strBuf, "BAS-%d", 5-me->Bass_vol);
						else	
							sprintf(strBuf, "BAS %d", 5-me->Bass_vol);
						
						UserAppDisplayOnce(me, strBuf, 3);
					}

					if((eCECRX->Operand[6] & 0x0f) != 0x0f)
					{
						me->Treble_vol = eCECRX->Operand[6]&0x0f;
						
						UserSetting_SaveTreble(me->Treble_vol);

						UserApp_SetTreble(me, me->Treble_vol);
						
						if(me->Treble_vol > 5)
							sprintf(strBuf, "TRE+%d", me->Treble_vol-5);
						else if(me->Treble_vol == 5)
							sprintf(strBuf, "TRE %d", 5-me->Treble_vol);
						else
							sprintf(strBuf, "TRE-%d", 5-me->Treble_vol);
					
						UserAppDisplayOnce(me, strBuf, 3);
					}

					cmd_buf[1] = (me->EQMode<<4)|me->Surround_3D;
					if(me->Bass_vol < 5)
					{
						if(me->Treble_vol < 5)
							cmd_buf[2] = ((6+me->Bass_vol)<<4)|(6+me->Treble_vol);
						else
							cmd_buf[2] = ((6+me->Bass_vol)<<4)|(me->Treble_vol-5);
					}
					else
					{
						if(me->Treble_vol < 5)
							cmd_buf[2] = ((me->Bass_vol-5)<<4)|(6+me->Treble_vol);
						else
							cmd_buf[2] = ((me->Bass_vol-5)<<4)|(me->Treble_vol-5);
						
					}

				}
				else
				{
					cmd_buf[1] = (me->EQMode<<4)|me->Surround_3D;
					if(me->Bass_vol < 5)
					{
						if(me->Treble_vol < 5)
							cmd_buf[2] = ((6+me->Bass_vol)<<4)|(6+me->Treble_vol);
						else
							cmd_buf[2] = ((6+me->Bass_vol)<<4)|(me->Treble_vol-5);
					}
					else
					{
						if(me->Treble_vol < 5)
							cmd_buf[2] = ((me->Bass_vol-5)<<4)|(6+me->Treble_vol);
						else
							cmd_buf[2] = ((me->Bass_vol-5)<<4)|(me->Treble_vol-5);
						
					}

				}
			}
			
			UserApp_ROKUCmd_Send(me, ROKU_ReportState, cmd_buf, 3);
			break;

		case ROKU_Reset:

			UserApp_RukoReset(me);
			
			break;

		default:
			break;
	}

	
}

#endif


enum
{
	Give_Support_Features = 0x90,
	Report_Support_Features,
	Give_TVSymphony_Support,
	Report_TVSymphony_Support,
	Give_DeviceID,
	Report_DeviceID,

	Give_All_Params = 0xa0,
	Report_All_Params,
	Set_Item_Value,
	Get_Item_Value,
	Report_Item_Value,
	Update_Item_Value,
	Reset_All_Params,
	Set_TVSymphony,
	Give_TVSymphony,
	Report_TVSymphony,
};

enum
{
	EQ_SUPPORT = (1<<0),
	SURROUND_SUPPORT = (1<<1),
	BASS_SUPPORT = (1<<2),
	TREBLE_SUPPORT = (1<<3),
	DIMMER_SUPPORT = (1<<4),	
};

enum
{
	HISENSE_EQ = 1,
	HISENSE_SURROUND ,
	HISENSE_BASS,
	HISENSE_TREBLE,
	HISENSE_DIMMER,	
};


void Hisese_SmartTv_VendorIDCmd_Send(UserApp * const me, BYTE cmd, BYTE *data, BYTE len)
{
	BYTE i=0;

	stUserCECEvt_t * RequestCEC = Q_NEW(stUserCECEvt_t,HDMI_SEND_VENDOR_CMD_SIG);
	RequestCEC->Header = 0x50; 
	RequestCEC->Opcode = 0xA0;
	RequestCEC->MsgLen = 2+4+len;
	RequestCEC->Operand[0] = 0xA8;
	RequestCEC->Operand[1] = 0x82;
	RequestCEC->Operand[2] = 0x00;
	RequestCEC->Operand[3] = cmd;
	for(i=0; i<len; i++)
		RequestCEC->Operand[4+i] =data[i];
	
	LOGD(" %s \n ", __func__);
	
	QACTIVE_POST(TouchApp_get(),(QEvt *)RequestCEC,me);
}

void Hisese_SmartTv_VendorIDCmd_Standby_Send(BYTE cmd, BYTE *data, BYTE len)
{
	BYTE i=0;

	stUserCECEvt_t RequestCEC;
	RequestCEC.Header = 0x50; 
	RequestCEC.Opcode = 0xA0;
	RequestCEC.MsgLen =  4+2+len;// need +2
	RequestCEC.Operand[0] = 0xA8;
	RequestCEC.Operand[1] = 0x82;
	RequestCEC.Operand[2] = 0x00;
	RequestCEC.Operand[3] = cmd;  //OK
	for(i=0; i<len; i++)
		RequestCEC.Operand[4+i] =data[i];

	LOGD(" %s \n ", __func__);
	
	IT6622_SendVendorCmd(&RequestCEC);
}


void 	Hisese_SmartTv_SendCmd(UserApp * const me)
{
	BYTE cmd_buf[5];

	cmd_buf[0] = me->EQMode;
	cmd_buf[1] = me->Surround_3D;
	cmd_buf[2] = me->Bass_vol+0x7B;	
	cmd_buf[3] = me->Treble_vol+0x7B;
	cmd_buf[4] = 0xff;
	Hisese_SmartTv_VendorIDCmd_Send(me, Report_All_Params, cmd_buf, 5);
}



void Hisese_SmartTv_Reset(UserApp * const me)
{

	me->Bass_vol = 5;
	UserSetting_SaveBass(me->Bass_vol);
	UserApp_SetBass(me, me->Bass_vol);

	me->Treble_vol = 5;
	UserSetting_SaveTreble(me->Treble_vol);
	UserApp_SetTreble(me, me->Treble_vol);

	me->EQMode = 0;
	UserSetting_SaveEQ(me->EQMode);
	AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER0, MODE1+me->EQMode);

	me->Surround_3D = 1;
	UserSetting_SaveSurround(me->Surround_3D);
	UserAppSurroundSet((QActive *)me, me->Surround_3D);

	LOGD(" %s \n ", __func__);

	Hisese_SmartTv_SendCmd(me);
}


void Hisese_SmartTv_Handle(UserApp * const me, stUserCECEvt_t *eCECRX)
{
	BYTE cmd_buf[8] = {0};
	static BYTE strBuf[10];

	switch(eCECRX->Operand[3])
	{
		case Give_Support_Features:

			LOGD(" Give_Support_Features \n");
			
			cmd_buf[0] = EQ_SUPPORT|SURROUND_SUPPORT|BASS_SUPPORT|TREBLE_SUPPORT;
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_Support_Features, cmd_buf, 1);
			
			break;
			
		case Give_TVSymphony_Support:

			LOGD(" Give_TVSymphony_Support \n");
			
			cmd_buf[0] = me->TV_Symphony;
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_TVSymphony_Support, cmd_buf, 1);
			
			break;

		case Give_DeviceID:

			LOGD(" Give_DeviceID \n");
			
			cmd_buf[0] = me->device_id[5]; 
			cmd_buf[1] = me->device_id[4];
			cmd_buf[2] = me->device_id[3];
			cmd_buf[3] = me->device_id[2];
			cmd_buf[4] = me->device_id[1];
			cmd_buf[5] = me->device_id[0];
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_DeviceID, cmd_buf, 6);
			
			break;

		case Give_All_Params:

			LOGD(" Give_All_Params \n");
			
			cmd_buf[0] = me->EQMode;;
			cmd_buf[1] = me->Surround_3D;
			cmd_buf[2] = me->Bass_vol+0x7B;
			cmd_buf[3] = me->Treble_vol+0x7B;
			cmd_buf[4] = 0xff;
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_All_Params, cmd_buf, 5);
			
			break;
			
		case Set_Item_Value:

			LOGD(" Set_Item_Value %d-%d \n", eCECRX->Operand[4], eCECRX->Operand[5]);

			cmd_buf[0] = eCECRX->Operand[4];
			cmd_buf[1] = eCECRX->Operand[5];
			
			switch(eCECRX->Operand[4])
			{
				case HISENSE_EQ:
					
					me->EQMode = eCECRX->Operand[5];
					
					UserSetting_SaveEQ(me->EQMode);

					UserAppDisplayOnce(me, vfd_str[STR_EQ0 + me->EQMode],3);
					AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER0, MODE1+me->EQMode);
					
					break;
					
				case HISENSE_SURROUND:
					me->Surround_3D = eCECRX->Operand[5];

					if(me->Surround_3D < 2)
					{
						UserApp_ScrollBackUp_ShowSource(me, STR_SU0 + me->Surround_3D);

						UserAppSurroundSet((QActive *)me, me->Surround_3D);
						UserSetting_SaveSurround(me->Surround_3D);	
					
					}
					break;
					
				case HISENSE_BASS:

					me->Bass_vol = eCECRX->Operand[5] - 0x7B;

					UserSetting_SaveBass(me->Bass_vol);
					UserApp_SetBass(me, me->Bass_vol);
					
					if(me->Bass_vol > 5)
						sprintf(strBuf, "BAS+%d", me->Bass_vol-5);
					else if(me->Bass_vol < 6)
						sprintf(strBuf, "BAS-%d", 5-me->Bass_vol);
					else	
						sprintf(strBuf, "BAS %d", 5-me->Bass_vol);
					
					UserAppDisplayOnce(me, strBuf, 3);
			
					break;
					
				case HISENSE_TREBLE:

					me->Treble_vol = eCECRX->Operand[5] - 0x7B;
					UserSetting_SaveTreble(me->Treble_vol);

					UserApp_SetTreble(me, me->Treble_vol);
					
					if(me->Treble_vol > 5)
						sprintf(strBuf, "TRE+%d", me->Treble_vol-5);
					else if(me->Treble_vol == 5)
						sprintf(strBuf, "TRE %d", 5-me->Treble_vol);
					else
						sprintf(strBuf, "TRE-%d", 5-me->Treble_vol);
				
					UserAppDisplayOnce(me, strBuf, 3);
					break;
					
				case HISENSE_DIMMER:
					
					break;
				
			}
			
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_Item_Value, cmd_buf, 2);
			
			break;
			
		case Get_Item_Value:

			LOGD(" Get_Item_Value\n");

			cmd_buf[0] = eCECRX->Operand[4];
			switch(eCECRX->Operand[4])
			{
				case HISENSE_EQ:
					cmd_buf[1] = me->EQMode;
					break;
					
				case HISENSE_SURROUND:
					cmd_buf[1] = me->Surround_3D;					
					break;
					
				case HISENSE_BASS:
					cmd_buf[1] = me->Bass_vol + 0x7B;
					break;
					
				case HISENSE_TREBLE:
					cmd_buf[1] = me->Treble_vol + 0x7B;
					break;
					
				case HISENSE_DIMMER:
					cmd_buf[1] = 0xff;
					break;
				
			}
			
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_Item_Value, cmd_buf, 2);
			break;
			
		case Reset_All_Params:
			LOGD(" Reset_All_Params\n");
			Hisese_SmartTv_Reset(me);
			break;
			
		case Set_TVSymphony:
			LOGD(" Set_TVSymphony\n");
			if(me->TV_Symphony)
			{
				me->TV_Symphony_flag = eCECRX->Operand[4];
				cmd_buf[0] = me->TV_Symphony_flag;
				
				Hisese_SmartTv_VendorIDCmd_Send(me, Report_TVSymphony, cmd_buf, 1);

				if(me->TV_Symphony_flag == 1)
				{
					UserApp_ScrollBackUp_ShowSource(me, STR_TV_SYMPHONY_ON);

					//UserAppMute(me, 1);

					me->EQMode = 0;
					UserSetting_SaveEQ(me->EQMode);
					AudDspService_Set_AppMode(&me->super, SET_EQ, GRP_FILTER0, MODE1+me->EQMode);

					me->Surround_3D = 0;
					UserSetting_SaveSurround(me->Surround_3D);
					UserAppSurroundSet((QActive *)me, me->Surround_3D);
					
				}
				else
					UserApp_ScrollBackUp_ShowSource(me, STR_TV_SYMPHONY_OFF);
			}
			break;
			
		case Give_TVSymphony:
			LOGD(" Give_TVSymphony\n");
			cmd_buf[0] = me->TV_Symphony_flag;
			Hisese_SmartTv_VendorIDCmd_Send(me, Report_TVSymphony, cmd_buf, 1);
			break;

		default:
			break;

	}

}

void Hisese_SmartTv_Standby_Handle(UserApp * const me, stUserCECEvt_t *eCECRX)
{
	BYTE cmd_buf[8];
	
	switch(eCECRX->Operand[3])
	{
		case Give_Support_Features:
	
			cmd_buf[0] = EQ_SUPPORT|SURROUND_SUPPORT|BASS_SUPPORT;

			Hisese_SmartTv_VendorIDCmd_Standby_Send(Report_Support_Features, cmd_buf, 1);
			break;
			
		case Give_TVSymphony_Support:
			cmd_buf[0] = me->TV_Symphony;
			Hisese_SmartTv_VendorIDCmd_Standby_Send(Report_TVSymphony_Support, cmd_buf, 1);
			break;

		case Give_DeviceID:
			cmd_buf[0] = me->device_id[5]; 
			cmd_buf[1] = me->device_id[4];
			cmd_buf[2] = me->device_id[3];
			cmd_buf[3] = me->device_id[2];
			cmd_buf[4] = me->device_id[1];
			cmd_buf[5] = me->device_id[0];
			Hisese_SmartTv_VendorIDCmd_Standby_Send(Report_DeviceID, cmd_buf, 6);
			break;

		case Give_All_Params:
			cmd_buf[0] = me->EQMode;
			cmd_buf[1] = me->Surround_3D;
			cmd_buf[2] = me->Bass_vol+0x7D;
			cmd_buf[3] = 0xff;
			cmd_buf[4] = 0xff;
				
			Hisese_SmartTv_VendorIDCmd_Standby_Send(Report_All_Params, cmd_buf, 5);
			break;
	}

}


