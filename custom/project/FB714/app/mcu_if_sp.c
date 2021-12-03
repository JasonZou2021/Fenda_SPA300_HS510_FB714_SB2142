/*************************************************************************************************
 * @file mcu_if_sp.c
 * @brief To communicate with MCU by Sunplus packet protocol
 * @note Copyright (c) 2020 Sunplus Technology Co., Ltd. All rights reserved.
 *
 * @author zy.bai@sunmedia.com.cn
 * @version v0.01
 * @date 2020/12/25
 *************************************************************************************************/
#include <string.h>
#include "qpc.h"
#define LOG_MODULE_ID ID_APUSR
#define LOG_EXTRA_STR "[MCUIF]"
#include "log_utils.h"
#include "custom_sig.h"
#include "datarammap.h"
#include "upgradeservice.h"
#include "mcu_interface.h"
#include "mcu_if_sp.h"
#include "fsi_api.h"
#include "exstd_interfaces.h"
#include "upgradeservice.h"
#include "apupgrade.h"
#include "systemservice.h"
#include "userapp.h"

Q_DEFINE_THIS_FILE

static QState MCUIF_SP_Active(stMCUIF_SP_t *const me, QEvt const *const e);
static QState MCUIF_SP_WaitAck(stMCUIF_SP_t *const me, QEvt const *const e);

#ifdef MCU_MASTER
static QState MCUIF_SP_UPGRD_SEND(stMCUIF_SP_t *const me, QEvt const *const e);
#else
static QState MCUIF_SP_UPGRD(stMCUIF_SP_t *const me, QEvt const *const e);
#endif

#define PKT_SFLAG                    (0x5a)
#define PKT_OFFSET_SFLAG             (0)               /* offset of SFLAG */
#define PKT_OFFSET_DEVID             (1)               /* offset of DEV ID */
#define PKT_OFFSET_CMD               (2)               /* offset of CMD ID */
#define PKT_OFFSET_SUBCMD            (3)               /* offset of CMD ID */
#define PKT_OFFSET_LENGTH            (4)               /* offset of payload length */
#define PKT_OFFSET_PAYLOAD           (6)               /* offset of payload */
#define PKT_HEADER_SIZE              PKT_OFFSET_PAYLOAD/* header size */
#define VOL_PAYLOAD_LEN              (2)               /* payload length of CMD VOL */
#define SRC_PAYLOAD_LEN              (1)               /* payload length of CMD SRC */
#define BAUD_PAYLOAD_LEN              (1)               /* payload length of CMD BAUD */
#define SEND_DATA_START_PAYLOAD_LEN  (0)               /* payload length of CMD SEND_DATA_START */
#define SEND_DATA_END_PAYLOAD_LEN    (0)               /* payload length of CMD SEND_DATA_END */
#define START_UPGRD_PAYLOAD_LEN      (0)               /* payload length of CMD START_UPGRD */
#define UPGRD_SEND_DATA_LEN          (4096)

#define STATE_INIT                   0
#define STATE_GET_SYNC_BYTE          1
#define STATE_GET_PACKET             2
#define STATE_PACKET_DONE            3

#define BUFFER_NUM                   2
#define PAYLOAD_ALIGN                8
#define MAX_PACKET_LENGTH            (4096 + 1 + PKT_HEADER_SIZE + PAYLOAD_ALIGN - 1) // header + payload + chksum + align offset
#define UART_FIFO_SIZE               128
#define PACKET_READ_TIMEOUT          10 // ms

typedef void (*MCUCmdFunc_t)(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload);

typedef struct
{
	uint8_t cmd;
	MCUCmdFunc_t CmdFunc;
} stMCUCmdHandler;

static stMCUIF_SP_t *pstMCU_IF;

static UINT8 calc_checksum(UINT8 *data, UINT16 len)
{
	UINT16 i;
	UINT8 checksum = 0;
	for (i = 0; i < len; i++) {
		checksum += data[i];
	}
	return checksum;
}

static void send_packet(uint32_t id, PacketHeader_t *header, uint8_t *payload)
{
    uint8_t checksum;
    uint16_t payloadLen;

    if(header == NULL) {
        LOGE("header is NULL\n");
    }

    header->sflag = PKT_SFLAG;
    // header->devId = 0x00;
    payloadLen = header->payloadLen;

	checksum = calc_checksum((uint8_t *)header, PKT_HEADER_SIZE);
    if (payload) {
        checksum += calc_checksum(payload, payloadLen);
    }

	MCUIF_Write(id, (uint8_t *)header, PKT_HEADER_SIZE);
    if (payload) {
        MCUIF_Write(id, payload, payloadLen);
    }
	MCUIF_Write(id, &checksum, 1);

	return;
}

static void send_ack(int id, uint8_t ack_cmd, uint8_t *data, uint16_t data_len)
{
	uint8_t *payload;
    PacketHeader_t header;

    header.cmd = CMD_ACK;
	header.subCmd = ack_cmd;
	if (data != NULL) {
		payload = (UINT8 *)malloc(data_len);
		if (payload == NULL) {
			LOGE("%s failed to allocate buffer\n", __FUNCTION__);
			return;
		}
		memcpy(payload, data, data_len);
        header.payloadLen = data_len;
        send_packet(id, &header, payload);
		free(payload);
	} else {
        header.payloadLen = 0;
		send_packet(id, &header, NULL);
	}
	LOGDT("send ack 0x%x\n", ack_cmd);
}

static void send_nack(int id, UINT8 ack_cmd, INT8 error_code, UINT8 *data, UINT16 data_len)
{
	UINT8 *payload;
    PacketHeader_t header;

    header.cmd = CMD_NACK;
	header.subCmd = ack_cmd;
	if (data != NULL) {
		payload = (UINT8 *)malloc(data_len + 1);
		if (payload == NULL) {
			LOGE("%s failed to allocate buffer\n", __FUNCTION__);
			return;
		}
		payload[0] = error_code;
		memcpy(payload + 1, data, data_len);
        header.payloadLen = data_len + 1;
		send_packet(id, &header, payload);
		free(payload);
	} else {
        header.payloadLen = 1;
		send_packet(id, &header, &error_code);
	}
	LOGDT("send nack 0x%x\n", ack_cmd);
}

#ifdef MCU_MASTER
static void CbFreePacket(void *e)
{
    MCUIF_SP_Evt_t *evt = (MCUIF_SP_Evt_t *)e;
	free(evt->header);
	free(evt->payload);
}

static INT32 SeekBin(INT32 sdfd, INT32 offset)
{
	INT32 sdRet = -1;

	sdRet = lseek(sdfd, offset, SEEK_SET);
	if (sdRet < 0)
	{
		LOGE("fat read fail ret = %d\n", sdRet);
	}
	return sdRet;
}
#endif

static void packet_reader(void *arg)
{
    stMCUIF_SP_t *me = arg;
    MCUIF_SP_Evt_t *evt;
    volatile PacketHeader_t *header[BUFFER_NUM];
	volatile uint8_t *payload[BUFFER_NUM];
    volatile uint8_t *packet[BUFFER_NUM];
	uint32_t readed, state, idx, ret, offset;
	int32_t payload_len;
	uint8_t checksum;

    idx = 0;
    readed = 0;
    payload_len = -1;
    state = STATE_INIT;
    while (1) {
        switch (state) {
        case STATE_INIT:
            offset = ((PKT_HEADER_SIZE + (PAYLOAD_ALIGN - 1)) & (~(PAYLOAD_ALIGN - 1))) - PKT_HEADER_SIZE; // for 8 aligned
            for (idx = 0; idx < BUFFER_NUM; idx++) {
                packet[idx] = (uint8_t *)malloc(MAX_PACKET_LENGTH);
                if (packet[idx] == NULL) {
                    LOGE("failed to allocate packet buffer\n");
                    break;
                }
                packet[idx] += offset;
                header[idx] = (PacketHeader_t *)packet[idx];
                payload[idx] = packet[idx] + PKT_HEADER_SIZE;// payload + checksum
            }
            idx = 0;
            state = STATE_GET_SYNC_BYTE;
            // LOGX("to sync\n");
        case STATE_GET_SYNC_BYTE:
            readed = MCUIF_Read(me->mcuif_id, (char *)packet[idx], 1);
            if (readed && header[idx]->sflag == PKT_SFLAG) {
                MCUIF_SetTimeout(me->mcuif_id, PACKET_READ_TIMEOUT);
                payload_len = -1;
                state = STATE_GET_PACKET;
                // LOGX("to packet\n");
            } else {
                break;
            }
        case STATE_GET_PACKET:
            ret = MCUIF_Read(me->mcuif_id, (char *)packet[idx] + readed, UART_FIFO_SIZE);
            if (ret == 0) {
                LOGW("timeout\n");
                state = STATE_GET_SYNC_BYTE;
                break;
            } else {
                readed += ret;
            }

            if (payload_len < 0) {
                if (readed >= PKT_HEADER_SIZE) {
                    payload_len = header[idx]->payloadLen;
                } else {
                    break;
                }
            }

            if (readed >= (uint32_t)(PKT_HEADER_SIZE + payload_len + 1)) {
                state = STATE_PACKET_DONE;
            } else {
                break;
            }
        case STATE_PACKET_DONE:
            // LOGX("done\n");
    #ifndef MCU_MASTER
            LOGDT("readed = %d\n", readed);
            LOGDT("payload[0] = 0x%02X, payload[1] = 0x%02X", (payload[idx])[0], (payload[idx])[1]);
			if(payload_len > 0)
				LOGXDT(", payload[%d] = 0x%02X", payload_len - 1, (payload[idx])[payload_len - 1]);
			LOGXDT(", chksum = 0x%02X\n", (payload[idx])[payload_len]);
    #endif
            if (readed != (uint32_t)(PKT_HEADER_SIZE + payload_len + 1)) {
                LOGE("failed to receive packet, readed = %d, len = %d\n", readed, payload_len);
    #ifndef MCU_MASTER
                LOGE("received len = %d\n", readed);
                LOGE("cmd = 0x%02X, payload_len = %d, payload = ", header[idx]->cmd, payload_len);
                uint32_t i;
                for (i = 0; i < readed; ++i) {
                    LOGX("0x%02X ", (payload[idx])[i]);
                    if ((i + 1) % 20 == 0) {
                        LOGX("\n");
                    }
                }
                LOGX("\n");
    #endif
                state = STATE_GET_SYNC_BYTE;
                // LOGX("to sync\n");
                break;
            }
            checksum = calc_checksum((UINT8 *)header[idx], PKT_HEADER_SIZE);
            checksum += calc_checksum((UINT8 *)payload[idx], (UINT16)payload_len);
            if (checksum != (payload[idx])[payload_len]) {
                LOGI("packet checksum error 0x%02X 0x%02X\n", checksum, (payload[idx])[payload_len]);
                int32_t i;
                for(i = 0; i <= payload_len; i++) {
                    LOGX("0x%02X ", (payload[idx])[i]);
                    if((i + 1) % 20 == 0) {
                        LOGX("\n");
                    }
                }
                state = STATE_GET_SYNC_BYTE;
                // LOGX("to sync\n");
                break;
            }
            evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_RCV_PKT_SIG);
            evt->header = (PacketHeader_t *)header[idx];
            evt->payload = (uint8_t *)payload[idx];
            evt->cb = NULL;
            QACTIVE_POST(me->superAO, (QEvt *)evt, (void *)0);
            idx ^= 1;
            MCUIF_SetTimeout(me->mcuif_id, 0xffffffff);
            state = STATE_GET_SYNC_BYTE;
            // LOGX("to init\n");
            break;
        default:
            break;
        }
    }
}

#ifdef MCU_MASTER
/**
 * @brief The handler to process the ready signal
 *
 * @param me: orthogonal component MCUIF_SP
 * @param header header of received packet
 * @param payload data of received packet
 */
static void handler_ready(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload)
{
    (void)header;
    (void)payload;
	LOGI("%s in\n", __FUNCTION__);
	send_ack(me->mcuif_id, CMD_SP_READY, NULL, 0);
}
#endif

/**
 * @brief The handler processes the vol signal and sets the device to a specific volume
 *
 * @param me: orthogonal component MCUIF_SP
 * @param header header of received packet
 * @param payload data of received packet
 */
static void handler_vol(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload)
{
	(void)header;
	LOGI("%s in\n", __FUNCTION__);
	stMCU_IF_Vol_t *vol_data = (stMCU_IF_Vol_t *)payload;
	send_ack(me->mcuif_id, CMD_VOL, NULL, 0);
	MCUIF_Vol_Evt_t *e = Q_NEW(MCUIF_Vol_Evt_t, MCU_IF_USER_VOLSET_SIG);
	memcpy(&e->vol_data, vol_data, header->payloadLen);
	QACTIVE_POST(me->superAO, (QEvt *)e, (void *)0);
}

/**
 * @brief The handler processes the src signal and sets the device to a specific source
 *
 * @param me: orthogonal component MCUIF_SP
 * @param header header of received packet
 * @param payload data of received packet
 */
static void handler_src(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload)
{
	(void)header;
	LOGI("%s in\n", __FUNCTION__);
	send_ack(me->mcuif_id, CMD_SRC, NULL, 0);
	stMCU_IF_Src_t *src_data = (stMCU_IF_Src_t *)payload;
	MCUIF_Src_Evt_t *e = Q_NEW(MCUIF_Src_Evt_t, MCU_IF_USER_SWITCHSRC_SIG);
	memcpy(&e->src_data, src_data, header->payloadLen);
	QACTIVE_POST(me->superAO, (QEvt *)e, (void *)0);
}

/**
 * @brief The handler processes the baud rate setting signal and sets the device to a specific speed
 *
 * @param me: orthogonal component MCUIF_SP
 * @param header header of received packet
 * @param payload data of received packet
 */
static void handler_baud(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload)
{
	(void)header;
	LOGI("%s in\n", __FUNCTION__);
	send_ack(me->mcuif_id, CMD_BAUD, NULL, 0);
	FullEnvDelayMs(1);
	int user_baud = payload[0];
	me->baud = user_baud;
	MCUIF_Config(me->mcuif_id, eMCUIF_ConfigType_BaudRate, me->baud);
}

/**
 * @brief Do this callback function after upgrade service return the size and offset of the bin file
 *
 * @param pstMe The AO of Userapp
 * @param pParam Get the parameers from upgrade service
 */
void cbk_ack(QActive * const pstMe, void* pParam)
{
	(void)pstMe;
	/* stResponse_UpgrdSrv_t is an alternative structure of UPGRADE_CBK_Param_t under upgradeservice.h */
	stResponse_UpgrdSrv_t *ack_params = (stResponse_UpgrdSrv_t *)pParam;
	if(ack_params->error_code == 0)
	{
		LOGDT("Read Size = %d\n", ack_params->ack_payload.len);
		LOGDT("Read Offset = %d\n", ack_params->ack_payload.offset);
		send_ack(pstMCU_IF->mcuif_id, CMD_SYS_UPGRD, (uint8_t *)&ack_params->ack_payload, sizeof(ack_params->ack_payload));
	}
	else
	{
		LOGE("Error code = %d, reject the upgrade request!\n", ack_params->error_code);
		send_nack(pstMCU_IF->mcuif_id, CMD_SYS_UPGRD, 0, NULL, 0);
	}
}

/**
 * @brief The handler to process the upgrade signal and start upgrade flow
 *
 * @param me: orthogonal component MCUIF_SP
 * @param header header of received packet
 * @param payload data of received packet
 */
static void handler_sys_upgrade(stMCUIF_SP_t *me, PacketHeader_t *header, uint8_t *payload)
{
	// LOGI("%s in\n", __FUNCTION__);
	uint16_t payloadLen = header->payloadLen;
	switch (header->subCmd)
	{
		case CMD_SEND_DATA_START:
		{
			LOGDT("--> Get SEND_DATA_START\n");

			MCUIF_SP_Evt_t *evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SYS_UPGRD_SIG);
			QACTIVE_POST(me->superAO, (QEvt *)evt, (void *)0);

			UpgradeService_Start(me->superAO, cbk_ack);
			break;
		}
		case CMD_SEND_DATA:
		{
			LOGDT("--> Get SEND_DATA\n");
			if(payloadLen > 0)
				UpgradeService_Send_Data(me->superAO, payload, payloadLen, cbk_ack);
			break;
		}
		default:
		{
			send_nack(me->mcuif_id, CMD_SYS_UPGRD, 0, NULL, 0);
			break;
		}
	}
}

static void isp_upgrade_start(stMCUIF_SP_t *me)
{
	LOGI("%s in\n", __FUNCTION__);

	QEvt *e = Q_NEW(QEvt, MCU_IF_USER_UPGRADE_START_SIG);

	volatile int *psdiISP_Magic = (int * )SRAM_ADDR_ISP_UPGRADE;
	volatile int *psdiISP_BR_Magic = (int * )SRAM_ADDR_UART1_BR;

	*psdiISP_Magic = ISP_UPGRADE_UART1;
	// *psdiISP_BR_Magic = GET_SDK_CFG(CFG_MCUIF_T)->baudRate;
	*psdiISP_BR_Magic = me->baud;

	QACTIVE_POST(me->superAO, e, (void *)0);
}

static void handler_isp_upgrade(stMCUIF_SP_t *me, PacketHeader_t *header,__UNUSED uint8_t *payload)
{
	LOGI("%s in\n", __FUNCTION__);
	switch (header->subCmd)
	{
		case CMD_SEND_DATA_START:
		{
			LOGD("%s: subCmd = SEND_DATA_START\n", __FUNCTION__);
			isp_upgrade_start(me);
			break;
		}
		default:
		{
			send_nack(me->mcuif_id, CMD_ISP_UPGRD, 0, NULL, 0);
			break;
		}
	}
}

static stMCUCmdHandler MCUCmdFuncTbl[] =
{
#ifdef MCU_MASTER
	{CMD_SP_READY, handler_ready},
#endif
	{CMD_VOL, handler_vol},
	{CMD_SRC, handler_src},
	{CMD_BAUD, handler_baud},
	{CMD_SYS_UPGRD, handler_sys_upgrade},
	{CMD_ISP_UPGRD, handler_isp_upgrade},
};

static QState MCUIF_SP_WaitAck(stMCUIF_SP_t *const me, QEvt const *const e)
{
	QState status = Q_HANDLED();
	MCUIF_SP_Evt_t const *evt;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("%s ENTRY\n", __FUNCTION__);
			QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC, 0);
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("%s EXIT\n", __FUNCTION__);
			QTimeEvt_disarm(&me->waitAck);
			if (me->CmdSent.payload != NULL) {
				free(me->CmdSent.payload);
				me->CmdSent.payload = NULL;
			}
			break;
		}
		case MCUIF_RCV_PKT_SIG:
		{
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);
			if ((evt->header->cmd != CMD_ACK) && (evt->header->cmd != CMD_NACK)) {
				LOGI("waiting for ack, ignore other packets\n");
			} else if (evt->header->cmd == CMD_ACK) {
				LOGI("get ACK\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			} else if(evt->header->cmd == CMD_NACK) {
				LOGI("get NACK\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			} else if (me->CmdSent.retries-- > 0) { /* N-ACK, retry the command */
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
			} else {
				status = Q_TRAN(&MCUIF_SP_Active);
			}
            if (evt->cb) {
                evt->cb((void *)e);
            }
			break;
		}
		case MCUIF_SND_PKT_SIG:
		{
			LOGI("waiting for ack, ignore send packet request\n");
			break;
		}
		case MCUIF_TIMEEVT_SIG:
		{
			LOGE("waiting for ack timeout\n");
			if (me->CmdSent.retries-- > 0) { /* retry the command */
				LOGD("resend CMD 0x%02X\n", me->CmdSent.header.cmd);
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC, 0);
			} else {
				status = Q_TRAN(&MCUIF_SP_Active);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}

#ifndef MCU_MASTER
static QState MCUIF_SP_UPGRD(stMCUIF_SP_t *const me, QEvt const *const e)
{
	QState status;
	MCUIF_SP_Evt_t const *evt;
	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("%s ENTRY\n", __FUNCTION__);
			QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC * 3, 0);
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("%s EXIT\n", __FUNCTION__);
			QTimeEvt_disarm(&me->waitAck);
			status = Q_HANDLED();
			break;
		}
		case MCUIF_RCV_PKT_SIG:
		{
			QTimeEvt_disarm(&me->waitAck);
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);
			LOGDT("%s RCV PKT 0x%02X\n", __FUNCTION__, evt->header->cmd);
			if (evt->header->cmd == CMD_SYS_UPGRD)
			{
				handler_sys_upgrade(me, evt->header, evt->payload);
			}
			else
			{
				send_nack(me->mcuif_id, evt->header->cmd, 0, NULL, 0);
			}
            if (evt->cb) {
                evt->cb((void *)e);
            }
			QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC * 3, 0);
			status = Q_HANDLED();
			break;
		}
		case MCUIF_TIMEEVT_SIG:
		{
			LOGE("waiting for cmd timeout\n");
			status = Q_TRAN(&MCUIF_SP_Active);
			break;
		}
		case MCUIF_UPGRD_END_SIG:
		{
			QTimeEvt_disarm(&me->waitAck);
			status = Q_TRAN(&MCUIF_SP_Active);
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}

#else

/**
 * @fn static QState MCUIF_SP_UPGRD_SEND(stMCUIF_SP_t *const me, QEvt const *const e)
 *
 * @brief The state machine handles the data sending of the upgrade process.
 *
 * @param me: pointer to the orthogonal component MCUIF_SP
 * @param e : the first event dispatch from Userapp
 * @return QState: the value of state machine function return.
 */
static QState MCUIF_SP_UPGRD_SEND(stMCUIF_SP_t *const me, QEvt const *const e)
{
	QState status;
	MCUIF_SP_Evt_t const *evt;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("%s ENTRY\n", __FUNCTION__);
			me->CmdSent.header.cmd = CMD_SYS_UPGRD;
            me->CmdSent.payload = NULL; // payload len = 0
            me->CmdSent.retries = 3; /* max retry 3 times */
            me->CmdSent.header.subCmd = CMD_SEND_DATA_START;
			me->CmdSent.header.payloadLen = SEND_DATA_START_PAYLOAD_LEN;
			send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
			QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC, 0);
			LOGD("send CMD_SEND_DATA_START --> slave\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("%s EXIT\n", __FUNCTION__);
			QTimeEvt_disarm(&me->waitAck);

			free(me->CmdSent.payload);
			me->CmdSent.payload = NULL;

			free(me->asbpucUSBBuffer);
			me->asbpucUSBBuffer = NULL;

			close(me->sdfd);

			status = Q_HANDLED();
			break;
		}
		case MCUIF_RCV_PKT_SIG:
		{
			QTimeEvt_disarm(&me->waitAck);
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);
			if ((evt->header->cmd != CMD_ACK) && (evt->header->cmd != CMD_NACK))
			{
				LOGI("waiting for ack, ignore other packets\n");
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else if (evt->header->cmd == CMD_ACK)
			{
				LOGI("get ACK\n");
				free(me->CmdSent.payload);
				stUpgrade_Ack_t *payload = (stUpgrade_Ack_t *)evt->payload;

				if(payload->len == 0)
				{
					LOGD("Bin file transfer finished!\n");
					status = Q_TRAN(&MCUIF_SP_Active);
					break;
				}

				me->read_len = payload->len;
				me->read_offset = payload->offset;

				LOGD("me->read_len = %d, me->read_offset = %d\n", me->read_len, me->read_offset);

				MCUIF_SP_Evt_t *next_evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SEND_DATA_SIG);
				QACTIVE_POST(me->superAO, (QEvt *)next_evt, (void *)0);

				status = Q_HANDLED();
			}
			else if (me->CmdSent.retries-- > 0)
			{ /* N-ACK, retry the command */
				LOGI("get NACK\n");
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else
			{
				LOGI("Reach the max retry times\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			}
			if (evt->cb) {
            	evt->cb((void *)e);
			}
			break;
		}
		case MCUIF_SEND_DATA_SIG:
		{
			if (me->read_len > 0)
			{
				LOGD("%s MCUIF_SEND_DATA_SIG\n", __FUNCTION__);
				SeekBin(me->sdfd, me->read_offset);
				INT32 sdRet = read(me->sdfd, me->asbpucUSBBuffer, me->read_len);
				if (sdRet < 0)
				{
					LOGE("fat read fail ret = %d\n", sdRet);
					status = Q_TRAN(&MCUIF_SP_Active);
					break;
				}

				me->CmdSent.payload = malloc(me->read_len);
				if (me->CmdSent.payload == NULL)
				{
					LOGE("%s: failed to allocate buffer\n", __FUNCTION__);
					status = Q_TRAN(&MCUIF_SP_Active);
					break;
				}

				me->CmdSent.header.cmd = CMD_SYS_UPGRD;
				me->CmdSent.header.subCmd = CMD_SEND_DATA;
				memcpy(me->CmdSent.payload, me->asbpucUSBBuffer, me->read_len);
				me->CmdSent.header.payloadLen = me->read_len;
				me->CmdSent.retries = 3; /* max retry 3 times */
				LOGD("payload[0] = 0x%02X, payload[1] = 0x%02X, payload[%d] = 0x%02X\n", me->CmdSent.payload[0], me->CmdSent.payload[1], me->read_len - 1, me->CmdSent.payload[me->read_len - 1]);
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				LOGD("send CMD_SEND_DATA --> slave\n");
			}
			status = Q_HANDLED();
			break;
		}
		case MCUIF_TIMEEVT_SIG:
		{
			LOGE("waiting for ack timeout\n");
			if (me->CmdSent.retries-- > 0) { /* retry the command */
				LOGD("resend CMD 0x%02X\n", me->CmdSent.header.cmd);
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else
			{
				LOGE("Reach the max retry times\n");

				LOGD("cmd = 0x%02X, payload =\n", me->CmdSent.header.cmd);
				int i;
				for(i = 0; i < me->CmdSent.header.payloadLen; ++i)
				{
					LOGX("0x%02X ", me->CmdSent.payload[i]);
					if((i + 1) % 20 == 0) {
						LOGX("\n");
                    }
				}
				LOGX("\n");

				LOGE("Reach the max retry times\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}

static QState MCUIF_SP_UPGRD_ISP_START(stMCUIF_SP_t *const me, QEvt const *const e)
{
	QState status;
	MCUIF_SP_Evt_t const *evt;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("%s ENTRY\n", __FUNCTION__);
			me->CmdSent.header.cmd = CMD_ISP_UPGRD;
            me->CmdSent.payload = NULL; // payload len = 0
            me->CmdSent.retries = 30; /* max retry 3 times */
            me->CmdSent.header.subCmd = CMD_SEND_DATA_START;
			me->CmdSent.header.payloadLen = SEND_DATA_START_PAYLOAD_LEN;
			send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
			QTimeEvt_armX(&me->waitAck, TICKS_PER_SEC, 0);
			LOGD("send CMD_SEND_DATA_START --> slave\n");
			status = Q_HANDLED();
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("%s EXIT\n", __FUNCTION__);
			QTimeEvt_disarm(&me->waitAck);

			free(me->CmdSent.payload);
			me->CmdSent.payload = NULL;

			free(me->asbpucUSBBuffer);
			me->asbpucUSBBuffer = NULL;

			close(me->sdfd);

			status = Q_HANDLED();
			break;
		}
		case MCUIF_RCV_PKT_SIG:
		{
			QTimeEvt_disarm(&me->waitAck);
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);
			if ((evt->header->cmd != CMD_ACK) && (evt->header->cmd != CMD_NACK))
			{
				LOGI("waiting for ack, ignore other packets\n");
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else if (evt->header->cmd == CMD_ACK)
			{
				LOGI("get ACK\n");
				free(me->CmdSent.payload);
				stUpgrade_Ack_t *payload = (stUpgrade_Ack_t *)evt->payload;

				if(payload->len == 0)
				{
					LOGD("Bin file transfer finished!\n");
					status = Q_TRAN(&MCUIF_SP_Active);
					break;
				}

				me->read_len = payload->len;
				me->read_offset = payload->offset;

				LOGD("me->read_len = %d, me->read_offset = %d\n", me->read_len, me->read_offset);

				status = Q_HANDLED();
				evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SEND_DATA_SIG);
				QACTIVE_POST(me->superAO, (QEvt *)evt, (void *)0);
			}
			else if (me->CmdSent.retries-- > 0)
			{ /* N-ACK, retry the command */
				LOGI("get NACK\n");
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else
			{
				LOGI("Reach the max retry times\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			}
			if (evt->cb) {
            	evt->cb((void *)e);
			}
			break;
		}
		case MCUIF_SEND_DATA_SIG:
		{
			LOGD("%s MCUIF_SEND_DATA_SIG\n", __FUNCTION__);
			SeekBin(me->sdfd, me->read_offset);
			INT32 sdRet = read(me->sdfd, me->asbpucUSBBuffer, me->read_len);
			if (sdRet < 0)
			{
				LOGE("fat read fail ret = %d\n", sdRet);
				status = Q_TRAN(&MCUIF_SP_Active);
				break;
			}

			me->CmdSent.payload = malloc(me->read_len);
			if (me->CmdSent.payload == NULL)
			{
				LOGE("%s: failed to allocate buffer\n", __FUNCTION__);
				status = Q_TRAN(&MCUIF_SP_Active);
				break;
			}

			me->CmdSent.header.cmd = CMD_ISP_UPGRD;
			me->CmdSent.header.subCmd = CMD_SEND_DATA;
			memcpy(me->CmdSent.payload, me->asbpucUSBBuffer, me->read_len);
			me->CmdSent.header.payloadLen = me->read_len;
			me->CmdSent.retries = 20; /* max retry 3 times */
			LOGD("payload[0] = 0x%02X, payload[1] = 0x%02X, payload[%d] = 0x%02X\n", me->CmdSent.payload[0], me->CmdSent.payload[1], me->read_len - 1, me->CmdSent.payload[me->read_len - 1]);
			send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
			QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
			LOGD("send CMD_SEND_DATA --> slave\n");
			status = Q_HANDLED();
			break;
		}
		case MCUIF_TIMEEVT_SIG:
		{
			LOGE("waiting for ack timeout\n");
			if (me->CmdSent.retries-- > 0) { /* retry the command */
				LOGD("resend CMD 0x%02X\n", me->CmdSent.header.cmd);
				send_packet(me->mcuif_id, &(me->CmdSent.header), me->CmdSent.payload);
				QTimeEvt_rearm(&me->waitAck, TICKS_PER_SEC);
				status = Q_HANDLED();
			}
			else
			{
				LOGE("Reach the max retry times\n");

				LOGD("cmd = 0x%02X, payload =\n", me->CmdSent.header.cmd);
				int i;
				for(i = 0; i < me->CmdSent.header.payloadLen; ++i)
				{
					LOGX("0x%02X ", me->CmdSent.payload[i]);
					if((i + 1) % 20 == 0) {
						LOGX("\n");
                    }
				}
				LOGX("\n");

				LOGE("Reach the max retry times\n");
				status = Q_TRAN(&MCUIF_SP_Active);
			}
			break;
		}
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}
#endif

static QState MCUIF_SP_Active(stMCUIF_SP_t *const me, QEvt const *const e)
{
	QState status = Q_HANDLED();
	MCUIF_SP_Evt_t const *evt;
	int cmd_tbl_size = sizeof(MCUCmdFuncTbl) / sizeof(MCUCmdFuncTbl[0]);
	int i;

	switch (e->sig)
	{
		case Q_ENTRY_SIG:
		{
			LOGD("%s ENTRY\n", __FUNCTION__);
			break;
		}
		case Q_EXIT_SIG:
		{
			LOGD("%s EXIT\n", __FUNCTION__);
			break;
		}
		case Q_INIT_SIG:
		{
			LOGD("%s INIT\n", __FUNCTION__);
			break;
		}
		case MCUIF_RCV_PKT_SIG:
		{
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);
			LOGD("%s RCV PKT 0x%02X\n", __FUNCTION__, evt->header->cmd);
			for (i = 0 ; i < cmd_tbl_size; i++) {
				if (evt->header->cmd == MCUCmdFuncTbl[i].cmd) {
					MCUCmdFuncTbl[i].CmdFunc(me, evt->header, evt->payload);
					break;
				}
			}
			/* by default, ack the command if no corresponding handler found */
			if (i == cmd_tbl_size && evt->header->cmd != CMD_ACK && evt->header->cmd != CMD_NACK) {
				send_nack(me->mcuif_id, evt->header->cmd, 0, NULL, 0);
			}
            if (evt->cb) {
                evt->cb((void *)e);
            }
			break;
		}
		case MCUIF_SND_PKT_SIG:
		{
            uint16_t payloadLen;

			LOGD("%s MCUIF_SND_PKT_SIG\n", __FUNCTION__);
			evt = Q_EVT_CAST(MCUIF_SP_Evt_t);

            payloadLen = evt->header->payloadLen;
			if(payloadLen != 0)
			{
				me->CmdSent.payload = malloc(payloadLen);
				if (me->CmdSent.payload == NULL) {
					LOGE("%s: failed to allocate buffer\n", __FUNCTION__);
					break;
				} else {
					memcpy(me->CmdSent.payload, evt->payload, payloadLen);
					me->CmdSent.retries = 3; /* max retry 3 times */
				}
			}
			me->CmdSent.header = *(evt->header);
			send_packet(me->mcuif_id, evt->header, evt->payload);
			status = Q_TRAN(&MCUIF_SP_WaitAck);
			break;
		}

		case MCUIF_SYS_UPGRD_SIG:
		{
			LOGD("%s MCUIF_SYS_UPGRD_SIG\n", __FUNCTION__);
#ifdef MCU_MASTER
			status = Q_TRAN(&MCUIF_SP_UPGRD_SEND);
#else
			status = Q_TRAN(&MCUIF_SP_UPGRD);
#endif
			break;
		}
#ifdef MCU_MASTER
		case MCUIF_ISP_UPGRD_SIG:
		{
			LOGD("%s MCUIF_ISP_UPGRD_SIG\n", __FUNCTION__);
			status = Q_TRAN(&MCUIF_SP_UPGRD_ISP_START);
			break;
		}
#endif
		default:
		{
			status = Q_SUPER(&QHsm_top);
			break;
		}
	}
	return status;
}

static QState MCUIF_SP_initial(stMCUIF_SP_t *const me, QEvt const *const e)
{
	(void)e; /* avoid compiler warning about unused parameter */
	me->mcuif_id = MCUIF_Init("uart1", packet_reader, me);
	if (me->mcuif_id < 0) {
		return (QState)Q_RET_NULL;
	}
	me->baud = GET_SDK_CFG(CFG_MCUIF_T)->baudRate;
	switch (me->baud)
	{
		case eUART_BR_115200 ... eUART_BR_3686400:
		{
			MCUIF_Config(me->mcuif_id, eMCUIF_ConfigType_BaudRate, me->baud);
			break;
		}
		case eUART_BR_CUSTOM:
		{
			MCUIF_Config(me->mcuif_id, eMCUIF_ConfigType_CustomBaudRate, 460800);
			break;
		}
		default:
		{
			LOGE("The default baud is set to an invalid value!\n");
			break;
		}
	}
	MCUIF_Start(me->mcuif_id);
#ifndef MCU_MASTER
    PacketHeader_t header;
    header.cmd = CMD_SP_READY;
    header.payloadLen = 0;
	send_packet(me->mcuif_id, &header, NULL);
#endif
	return Q_TRAN(&MCUIF_SP_Active);
}

/******************************************************************************************/
/**
 * @fn void MCUIF_SP_ctor(stMCUIF_SP_t *const me, QActive *superAO)
 * @brief constructor
 * @param me: the object to be constructed
 * @param superAO: the UserApp active object
 ******************************************************************************************/
void MCUIF_SP_ctor(stMCUIF_SP_t *const me, QActive *superAO)
{
	LOGD("%s entered\n", __FUNCTION__);
	QHsm_ctor(&me->super, Q_STATE_CAST(&MCUIF_SP_initial));
	me->superAO = superAO;
	pstMCU_IF = me;
	QTimeEvt_ctorX(&me->waitAck, superAO, MCUIF_TIMEEVT_SIG, 0U);
}

/******************************************************************************************/
/**
 * @fn void set_baud(stMCUIF_SP_t *me, UINT8 USER_BAUD)
 * @brief set baud rate to the specific speed
 * @param me: the mcu_if_sp orthogonal component
 * @param USER_BAUD: (Input) enum eUART_BR_baudrate
 ******************************************************************************************/
void MCUIF_SP_set_baud(stMCUIF_SP_t *me, UINT8 USER_BAUD)
{
#ifdef MCU_MASTER
	MCUIF_SP_Evt_t *evt;
	evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SND_PKT_SIG);

    evt->header = (PacketHeader_t *)malloc(sizeof(PacketHeader_t));
    evt->payload = (uint8_t *)malloc(BAUD_PAYLOAD_LEN);

    evt->header->cmd = CMD_BAUD;
	evt->payload[0] = USER_BAUD;
	evt->header->payloadLen = BAUD_PAYLOAD_LEN;
    evt->cb = CbFreePacket;
	QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
	FullEnvDelayMs(1);
#endif
	me->baud = USER_BAUD;
	MCUIF_Config(me->mcuif_id, eMCUIF_ConfigType_BaudRate, me->baud);
}

#ifdef MCU_MASTER
/******************************************************************************************/
/**
 * @fn void set_vol(stMCUIF_SP_t *me, UINT8 vol_int)
 * @brief set volume to the specific level
 * @param me: the mcu_if_sp orthogonal component
 * @param vol_int: the level of the volume
 ******************************************************************************************/
void MCUIF_SP_set_vol(stMCUIF_SP_t *me, UINT8 vol_int)
{
	MCUIF_SP_Evt_t *evt;
	evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SND_PKT_SIG);

    evt->header = (PacketHeader_t *)malloc(sizeof(PacketHeader_t));
    evt->payload = (uint8_t *)malloc(VOL_PAYLOAD_LEN);

	stMCU_IF_Vol_t vol_data;
	vol_data.channel_ID = 0;
	vol_data.volume = vol_int;

    evt->header->cmd = CMD_VOL;
	memcpy(evt->payload, &vol_data, VOL_PAYLOAD_LEN);
	evt->header->payloadLen = VOL_PAYLOAD_LEN;
    evt->cb = CbFreePacket;
	QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
}

/******************************************************************************************/
/**
 * @fn void set_src(stMCUIF_SP_t *me, UINT8 USER_SRC)
 * @brief set user source to the specific source
 * @param me: the mcu_if_sp orthogonal component
 * @param USER_SRC: (Input) enum UserAppSrc
 ******************************************************************************************/
void MCUIF_SP_set_src(stMCUIF_SP_t *me, UINT8 USER_SRC)
{
	MCUIF_SP_Evt_t *evt;
	evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SND_PKT_SIG);

    evt->header = (PacketHeader_t *)malloc(sizeof(PacketHeader_t));
    evt->payload = (uint8_t *)malloc(SRC_PAYLOAD_LEN);

	evt->header->cmd = CMD_SRC;
	evt->payload[0] = USER_SRC;
	evt->header->payloadLen = SRC_PAYLOAD_LEN;
    evt->cb = CbFreePacket;
    QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
}

/******************************************************************************************/
/**
 * @fn int MCUIF_SP_upgrade(stMCUIF_SP_t *me, char *pbUpgFilePath)
 * @brief Upgrade service API, communacate through MCU interface (UART 1)
 * @param me: the mcu_if_sp orthogonal component
 * @param pbUpgFilePath: (Input) the path of the upgrade bin file
 ******************************************************************************************/
int MCUIF_SP_upgrade(stMCUIF_SP_t *me, char *pbUpgFilePath)
{
	if (pbUpgFilePath == NULL)
	{
		LOGE("ERR pbUpgFilePath == NULL\n");
		return -1;
	}

	me->pbUpgFilePath = pbUpgFilePath;
	LOGD("open bin file: %s\n", pbUpgFilePath);
	me->sdfd = open(pbUpgFilePath, O_RDONLY, 0);
	if (me->sdfd < 0)
	{
		LOGE("fat open fail ret = %d\n", me->sdfd);
		return -1;
	}
	me->asbpucUSBBuffer = malloc(UPGRD_SEND_DATA_LEN);
	if (me->asbpucUSBBuffer == NULL)
	{
		LOGE("malloc fail, return app main\n");
		return -1;
	}

	MCUIF_SP_Evt_t *evt;
	evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_SYS_UPGRD_SIG);
	evt->cb = NULL;
	QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
	return 0;
}

int MCUIF_SP_upgrade_isp(stMCUIF_SP_t *me, char *pbUpgFilePath)
{
	if (pbUpgFilePath == NULL)
	{
		LOGE("ERR pbUpgFilePath == NULL\n");
		return -1;
	}

	me->pbUpgFilePath = pbUpgFilePath;
	LOGD("open bin file: %s\n", pbUpgFilePath);
	me->sdfd = open(pbUpgFilePath, O_RDONLY, 0);
	if (me->sdfd < 0)
	{
		LOGE("fat open fail ret = %d\n", me->sdfd);
		return -1;
	}
	me->asbpucUSBBuffer = malloc(UPGRD_SEND_DATA_LEN);
	if (me->asbpucUSBBuffer == NULL)
	{
		LOGE("malloc fail, return app main\n");
		return -1;
	}

	MCUIF_SP_Evt_t *evt;
	evt = Q_NEW(MCUIF_SP_Evt_t, MCUIF_ISP_UPGRD_SIG);
	evt->cb = NULL;
	QMSM_DISPATCH(&me->super, &evt->super);
	QF_gc(&evt->super);
	return 0;
}

#endif
