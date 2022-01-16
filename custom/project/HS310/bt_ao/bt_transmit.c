#ifndef __BT_TRANSMIT_C__
#define __BT_TRANSMIT_C__
#include "log_utils.h"
#include "bt_ao.h"
#include "bt_mw_core.h"
#include "bt_ao_service_api.h"

//#define TRANSPORT_DEBUG

#define TRANSFER_PAYLOAD_SIZE       (2)
#define RECEIVER_PAYLOAD_SIZE       (2)



#define GROUP_MAX_PAYLOAD  492 // 120  /*ONE  SPP  PACKET MAX LENGTH*/
#define SPP_MAX_PAYLOAD  512 // 128  /*ONE  SPP  PACKET MAX LENGTH*/
#define SPP_MAX_PAYLOAD_NUM 4//16 /*MAX SPP  PACKET NUM*/


typedef enum
{
    ERROR_CHECK_NULL,
	ERROR_CHECK_CRC,
	ERROR_CHECK_CHESUM,
	ERROR_CHECK_MAX,
}error_check_mode;


#if 0
typedef enum _BLUETOOTH_SPP_MSG
{
	SPP_APSEND_DATA,
	SPP_APSEND_DATA_OK,
	SPP_RECEIVE_DATA_TO_AP,
	SPP_RECEIVE_DATA,
	SPP_RECEIVE_DATA_PARSER,
}BT_SPP_MSG_E;

#endif

typedef struct _Spp_playload {
	UINT8 payload[SPP_MAX_PAYLOAD_NUM][SPP_MAX_PAYLOAD];/*SPP BUF FOR APP APk*/
}Spp_playload;


typedef struct _transport_data_header {
	UINT16 	transport_mark;// aabb
	UINT8	trans_package_id:4;// 传输包ID
	UINT8 	Check_mode:2;// 00表示不需要检测 01表示CRC，10表示CHECK_SUM，11保留
	UINT8 	Reserved2:2;//
	UINT16 	trans_package_len;
	UINT32 	error_value;
}Transport_data_Header;


typedef struct _retransport_header {
	UINT16 	retransport_mark;// EEFF
	UINT8	retrans_package_id:4;// 重传包ID
} Retransport_Header;




typedef struct _GROUPS_HEADER {
	UINT16  payload_type:2;//封装的数据类型10 DATA,01 CMD
	UINT16 	all_data_send:1;// 1..表示所有数据发送完
	UINT16 	Reserved1:1;
	UINT16 	mulit:1;//1表示该分组数据有多个包
	UINT16 	group_end:1;// 1..表示分组数据最后一个包
	UINT16 	Reserved2:2;
	UINT16 	value:8;// 表示该数据功能类型；
}GROUPS_HEADER;



static  Spp_playload *pSppTransBuf = NULL;
static  Spp_playload *pSppRecvBuf = NULL;
static UINT8 transfer_package_id=0;
static int receiver_ok_package_id=-1;
Spp_receiver_Data_If re_pdata;
Transport_data_Header tr_pdata;

#define TRANSPORT_MARK_ID 0xaabb
#define RETRANSPORT_MARK_ID 0xeeff

static void ApSendData(UINT8 *flag,UINT8 *buf, UINT32 len);


static void SendTransDataFinishMsg2Ao(BtAddr_t BtAddr)
{
	QActive * const me = BtAoGet();
	(void)me;

	BtCBKEvt *msg = Q_NEW(BtCBKEvt, BT_TRANSMIT_SIG);
	msg->pdata = BtMalloc(sizeof(BtAddr_t));
	memcpy(msg->pdata,&BtAddr,sizeof(BtAddr_t));
	msg->type = BT_TRANS_DATA_FINISH_SIG;
	QACTIVE_POST(me, (QEvt *)msg, me);

}


static void SendTransDataInfo2Ao(Spp_receiver_Data_If *pTransData,int len)
{
	QActive * const me = BtAoGet();
	Spp_receiver_Data_If * pData = NULL;
	(void)me;

	BtCBKEvt *msg = Q_NEW(BtCBKEvt, BT_TRANSMIT_SIG);
	pData = BtMalloc(len);
	memcpy(pData,pTransData,len);
	pData->pdatabuf = BtMalloc(pTransData->payload_len);//not include group
	memcpy(pData,pTransData->pdatabuf,pTransData->payload_len);

	msg->type = BT_RECV_TRANS_DATA_SIG;
	QACTIVE_POST(me, (QEvt *)msg, me);

}




#ifdef	TRANSPORT_DEBUG
 void PrintBlock(UINT8* pBuffStar, UINT32 uiBuffLen)
 {
	 UINT32 uiIter;
	 bt_ao_printf("@@ start:0x%x, uiBuffLen:%d\n", pBuffStar, uiBuffLen);
	 for (uiIter = 0; uiIter < uiBuffLen; uiIter++)
	 {
		 if(((uiIter % 16 == 0) || (uiIter % 512 == 0)) && (uiIter != 0))
		 {
			 bt_ao_printf("\n");
		 }

		 bt_ao_printf(" %02x", pBuffStar[uiIter]);
	 }
	 bt_ao_printf("\n");
 }
#else
void PrintBlock(UINT8* pBuffStar, UINT32 uiBuffLen)
{
	 pBuffStar = pBuffStar;
	 uiBuffLen = uiBuffLen;
}
#endif


void TransferBufInit(void)
{
	UINT32 *transfer_buf_start = NULL;
	UINT32 *receiver_buf_start = NULL;

	if(pSppTransBuf == NULL)
	{
		transfer_buf_start = (UINT32 *)BtZoreMalloc(TRANSFER_PAYLOAD_SIZE<<10);
		pSppTransBuf = (Spp_playload *)transfer_buf_start;//all Spp_playload max:8k,here just 2k (4 trans nums)
	}

	if(pSppRecvBuf == NULL)
	{
		receiver_buf_start = (UINT32 *)BtZoreMalloc(RECEIVER_PAYLOAD_SIZE<<10);
		pSppRecvBuf = (Spp_playload *)receiver_buf_start;
	}
}


void TransferBufDeinit(void)
{
	if(pSppTransBuf != NULL)
	{
		BtFree(pSppTransBuf);
	}

	if(pSppRecvBuf != NULL)
	{
		BtFree(pSppRecvBuf);
	}
}


UINT32 CheckDataCrc()
{
	return 0;
}

UINT32 CheckSum(UINT8 *buf,UINT32 dLength)
{
	UINT32 sum_value=0;

	while(dLength--)
	{
		sum_value+=(*(buf++));
	}

	bt_ao_printf("<TRANSPORT DBG>sum_value=%d~~~\n",sum_value);

	return sum_value;
}

UINT32 SetCheckValue(UINT8 error_check,UINT8*buf,UINT32 dLength)
{
	UINT32 Check_Value=0;

	if(error_check==ERROR_CHECK_CHESUM)
		Check_Value=CheckSum(buf, dLength);
	else if(error_check==ERROR_CHECK_CRC)
	{
		Check_Value=CheckDataCrc();
	}

	return Check_Value;
}


void SetTransferDataHeader(UINT8 error_check, UINT8*buf, UINT16 len)
{
	tr_pdata.transport_mark=0xaabb;
	tr_pdata.trans_package_id = transfer_package_id;
	tr_pdata.trans_package_len=len+sizeof(Transport_data_Header);
	tr_pdata.Check_mode=error_check;
	tr_pdata.error_value=SetCheckValue(error_check,buf,len);
}

//package group data to transport data
void SendGroupData2TransLayer(BtAddr_t *pBtAddr,UINT8 error_check,UINT8 *buf,UINT16 dLength)
{
#ifdef TRANSPORT_DEBUG
	bt_ao_printf("<TRANSPORT DBG>send data:%lld[%lldms]\n", stc0 - stc1, (stc0-stc1)/90);
	bt_ao_printf("<TRANSPORT DBG>send_group_data_to_transport_layer error_check=%d\n",error_check);
	PrintBlock(buf, dLength);
#endif
	bt_ao_printf("\n %s: dLength = %d\n",__FUNCTION__,dLength);
	SetTransferDataHeader(error_check,buf,dLength);

#ifdef TRANSPORT_DEBUG
	bt_ao_printf("~tr_hander->transport_mark 0x%x\n",tr_pdata.transport_mark);
	bt_ao_printf("~tr_hander->trans_package_id 0x%x\n",tr_pdata.trans_package_id);
	bt_ao_printf("~tr_hander->trans_package_len 0x%x\n",tr_pdata.trans_package_len);
	bt_ao_printf("~tr_hander->Check_mode 0x%x\n",tr_pdata.Check_mode);
	bt_ao_printf("~tr_hander->error_value 0x%x\n",tr_pdata.error_value);
	bt_ao_printf("~transfer_package_id 0x%x\n",transfer_package_id);
#endif
	bt_ao_printf("~transfer_package_id 0x%x\n",transfer_package_id);

	memcpy(pSppTransBuf->payload[transfer_package_id],&tr_pdata, sizeof(Transport_data_Header));
	memcpy(pSppTransBuf->payload[transfer_package_id]+ sizeof(Transport_data_Header), buf, dLength);//group data

	(void)pBtAddr;
	//BtServiceSendData(pBtAddr,pSppTransBuf->payload[transfer_package_id],tr_pdata.trans_package_len);

#ifdef TRANSPORT_DEBUG
	bt_ao_printf("before send_group_data_to_transport_layer()...trans_package_len:%d\n",tr_pdata.trans_package_len);
	PrintBlock(pSppTransBuf->payload[transfer_package_id],(UINT32)tr_pdata.trans_package_len);
#endif

	transfer_package_id++;
	if(transfer_package_id==SPP_MAX_PAYLOAD_NUM)
		transfer_package_id=0;
}


//pakage  user data to group data
void  SendData2TransLayer(BtAddr_t BtAddr,char *buf)
{
	Spp_Send_Data_If *data_info = (Spp_Send_Data_If *)buf;
	GROUPS_HEADER data_group_header;
	UINT32 total_datalen = 0;
	UINT8 error_check = 0;
	UINT32 send_data_len = 0;
	UINT16 group_package_Payload_len = 0;
	UINT8 group_Payload[GROUP_MAX_PAYLOAD];
	UINT32 dPayload_package_num = 0;
	UINT8  break_flag = 0;

	total_datalen = data_info->payload_len;
	error_check = data_info->Type_Flag.Error_check;
	data_group_header.all_data_send = 0;
	data_group_header.value = data_info->Type_Flag.value;
	data_group_header.payload_type = data_info->Type_Flag.func;

#ifdef TRANSPORT_DEBUG
	bt_ao_printf("<TRANSPORT DBG>error_check 0x%x\n",error_check);
	bt_ao_printf("<TRANSPORT DBG>spp_group->all_data_send 0x%x\n",data_group_header.all_data_send);
	bt_ao_printf("<TRANSPORT DBG>pp_group->value 0x%x\n",data_group_header.value);
	bt_ao_printf("<TRANSPORT DBG>spp_group->payload_type 0x%x\n",data_group_header.payload_type);
	bt_ao_printf("<TRANSPORT DBG>total_datalen 0x%x\n",total_datalen);
#endif
	//total_datalen :from ap data len   send_data_len: had sent data len
	while((send_data_len <= total_datalen) && (break_flag == 0))
	{
		memset(group_Payload,0,GROUP_MAX_PAYLOAD);
		//if data len > GROUP_MAX_PAYLOAD framage to send
		if((total_datalen - send_data_len) > (GROUP_MAX_PAYLOAD - sizeof(GROUPS_HEADER)))
		{
			group_package_Payload_len = (GROUP_MAX_PAYLOAD - sizeof(GROUPS_HEADER));
			data_group_header.mulit = 1;
			data_group_header.group_end = 0;
			dPayload_package_num++;
		}
		else
		{
			group_package_Payload_len = (total_datalen - send_data_len);
			data_group_header.group_end = 1;
			dPayload_package_num = 0;

			if(data_info->Type_Flag.all_data_send == 1)
				data_group_header.all_data_send = 1;
		}



		memcpy(group_Payload, &data_group_header, sizeof(GROUPS_HEADER));
		memcpy(group_Payload + sizeof(GROUPS_HEADER), data_info->pdatabuf+send_data_len, group_package_Payload_len);


		SendGroupData2TransLayer(&BtAddr,error_check,group_Payload,(group_package_Payload_len+sizeof(GROUPS_HEADER)));
		send_data_len += group_package_Payload_len;

		if((total_datalen == 0)||(send_data_len == total_datalen))//for cmd msg
		{
			if(data_group_header.payload_type==PAYLOAD_DATA)
				SendTransDataFinishMsg2Ao(BtAddr);//transfer_drv_msg_up(SPP_APSEND_DATA_OK,NULL, 0);//HEWEI ADD TO TELL AP ALL DATA  SEND OK]
			break;
		}

		if(dPayload_package_num == 2)
		{
			bt_ao_printf("~~need_break\n");

			break_flag = 1;
			AppSendData_Type_Flag pdata;

			memcpy(&pdata, &(data_info->Type_Flag), sizeof(AppSendData_Type_Flag));
			ApSendData((UINT8 *)&pdata, data_info->pdatabuf + send_data_len, total_datalen-send_data_len);//continue to send
		}

	}

	if(data_info->pdatabuf)
		BtFree(data_info->pdatabuf); //daijun add 20150105
}

void SendRetransMsg(BtAddr_t *pBtAddr,UINT8 id)
{
	Retransport_Header re_header;
	re_header.retransport_mark=0xeeff;
	re_header.retrans_package_id=id;
	(void)pBtAddr;
	//BtServiceSendData(pBtAddr,(UINT8 *)&re_header,sizeof(Retransport_Header));
}



int CheckDataError(BtAddr_t *pBtAddr,UINT8 *buf,UINT32 dLength)
{
	Transport_data_Header*data_header=(Transport_data_Header*)buf;

	switch(data_header->Check_mode)
	{
		case ERROR_CHECK_CRC:
			if(CheckDataCrc())
			{
				SendRetransMsg(pBtAddr,data_header->trans_package_id);
				return -1;
			}
			break;

		case ERROR_CHECK_CHESUM:
			if(data_header->error_value!=CheckSum(buf+sizeof(Transport_data_Header),(dLength-sizeof(Transport_data_Header))))
			{
				SendRetransMsg(pBtAddr,data_header->trans_package_id);
				return -1;
			}
			break;

		default:
			break;
	}

	bt_ao_printf("~ERROR_CHECK_CHESUM ok~\n");
	return 0;
}



int CheckDataHeader(BtAddr_t *pBtAddr,UINT8* buf)
{
	Retransport_Header*re_header=(Retransport_Header*)buf;
	int ret=-1;
	UINT8 retrans_id=0;

	if(re_header->retransport_mark == TRANSPORT_MARK_ID)//trans mark
		ret=1;

	if(re_header->retransport_mark == RETRANSPORT_MARK_ID)//retrans mark for transfer
	{
		retrans_id=re_header->retrans_package_id;

		bt_ao_printf("<TRANSPORT DBG>retrans_id=%x---transfer_package_id=%d---len=%d\n",retrans_id,transfer_package_id,pSppTransBuf->payload[retrans_id][(UINT8)&((Transport_data_Header*)0)->trans_package_len]);
		//transfer_package_id means pakage nums
		while(retrans_id!=transfer_package_id)
		{

			bt_ao_printf("~retrans_id 0x%x\n",retrans_id);
			//receve retrans req resend retrans_id data
			(void)pBtAddr;
			//BtServiceSendData(pBtAddr,pSppTransBuf->payload[retrans_id],pSppTransBuf->payload[retrans_id][(UINT8)&((Transport_data_Header*)0)->trans_package_len]);
			retrans_id++;

			if(retrans_id==SPP_MAX_PAYLOAD_NUM)
				retrans_id=0;
		}

		ret=-1;
	}

	return ret;
}

int CheckDataId(BtAddr_t *pBtAddr,UINT8 * buf)
{
	static int wailt_retrans_package=0;
	Transport_data_Header*data_header=(Transport_data_Header*)buf;

	bt_ao_printf("<TRANSPORT DBG>receiver_package_id=%d~~id=%d\n",receiver_ok_package_id,data_header->trans_package_id);
#ifdef TRANSPORT_DEBUG
	bt_ao_printf("before check_data_id():\n");
	PrintBlock(buf,data_header->trans_package_len);
#endif

	if(receiver_ok_package_id == -1)
		receiver_ok_package_id = data_header->trans_package_id;

	if(receiver_ok_package_id == data_header->trans_package_id)
	{
		wailt_retrans_package=1;

		return 0;
	}
	else if(wailt_retrans_package == 1)
	{
		wailt_retrans_package=2;
		SendRetransMsg(pBtAddr,receiver_ok_package_id);
	}

	return -1;
}

//pakage data for ap
void SetReceiverDataHeader( UINT8 *buf, UINT16 len)
{
	bt_ao_printf("<TRANSPORT DBG>set_receiver_data_header() in\n");
	GROUPS_HEADER *re_group_header = (GROUPS_HEADER *)buf;

	re_pdata.Type_Flag.func = re_group_header->payload_type;
	re_pdata.Type_Flag.all_data_send = re_group_header->all_data_send;
	re_pdata.Type_Flag.data_end = re_group_header->group_end;
	re_pdata.Type_Flag.value = re_group_header->value;

	re_pdata.pdatabuf = buf + sizeof(GROUPS_HEADER);
	re_pdata.payload_len = (UINT32)len - sizeof(GROUPS_HEADER);
}


void ParserGroupData2Ap( UINT8*buf, UINT16 len)
{
	SetReceiverDataHeader(buf,len);

#ifdef TRANSPORT_DEBUG
	bt_ao_printf("~re_pdata.Type_Flag.all_data_send %x\n",re_pdata.Type_Flag.all_data_send);
	bt_ao_printf("~re_pdata.Type_Flag.func %x\n",re_pdata.Type_Flag.func);
	bt_ao_printf("~re_pdata.Type_Flag.data_end %x\n",re_pdata.Type_Flag.data_end);
	bt_ao_printf("~re_pdata.Type_Flag.value %x\n",re_pdata.Type_Flag.value);
	bt_ao_printf("~re_pdata.payload_len %x\n",re_pdata.payload_len);
	bt_ao_printf("~~~receiver_ok_package_id=%d~\n",receiver_ok_package_id);
	bt_ao_printf("~sizeof(Spp_receiver_Data_If) = %d ~\n",sizeof(Spp_receiver_Data_If));
#endif

	SendTransDataInfo2Ao(&re_pdata,sizeof(Spp_receiver_Data_If));
}




Spp_Send_Data_If ap_pdata;
void SentApDataFlag(UINT8 *flag,UINT8 *buf, UINT32 len)
{
	memcpy(&ap_pdata.Type_Flag,flag,sizeof(AppSendData_Type_Flag));
	ap_pdata.pdatabuf = buf;
	ap_pdata.payload_len=len;
}

//may be first msg data
static void ApSendData(UINT8 *flag,UINT8 *buf, UINT32 len)
{
	SentApDataFlag(flag,buf,len);
	//transfer_drv_msg_up(SPP_APSEND_DATA,&ap_pdata, sizeof(Spp_Send_Data_If));
}



//get data from bt_service(this buf:trans header + group header + real data)
void TransportLayerReceiverDataAnalys(BtAddr_t BtAddr,char *buf,UINT16 dLength)
{
	//printf_w("<TRANSPORT DBG>transport_layer_receiverdata_parser dLength = %d ~\n",dLength);

	if(CheckDataHeader(&BtAddr,buf)<0)
		return;

	if(CheckDataId(&BtAddr,buf)<0)
		return;

	if(CheckDataError(&BtAddr,buf,dLength)<0)
		return;

	memcpy(pSppRecvBuf->payload[receiver_ok_package_id],buf+sizeof(Transport_data_Header), dLength-sizeof(Transport_data_Header));
	ParserGroupData2Ap(pSppRecvBuf->payload[receiver_ok_package_id],dLength-sizeof(Transport_data_Header));

	receiver_ok_package_id++;
	if(receiver_ok_package_id==SPP_MAX_PAYLOAD_NUM)
		receiver_ok_package_id=0;
}






#endif //__BT_TRANSMIT_C__

