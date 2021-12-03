/*************************************************************************************************
 * @file mcu_interface.h
 * @brief MCU interface provides the interface to communicate with MCU by UART port
 * @note Copyright (c) 2020 Sunplus Technology Co., Ltd. All rights reserved.
 *
 * @author zy.bai@sunmedia.com.cn
 * @version v0.01
 * @date 2020/12/23
 *************************************************************************************************/

#define MAX_MCUIF_COUNT	(2)
#define MCUIF_DEFAULT_BAUD	(115200)

enum eMCUIF_ConfigType {
	eMCUIF_ConfigType_BaudRate,
	eMCUIF_ConfigType_CustomBaudRate,
	eMCUIF_ConfigType_MAX
};

typedef void (*packet_reader_t)(void *arg);

extern int MCUIF_Init(const char *uart, packet_reader_t packet_reader, void *arg);
extern void MCUIF_Deinit(int id);
extern int MCUIF_Start(int id);
extern void MCUIF_Stop(int id);
extern int MCUIF_Config(int id, enum eMCUIF_ConfigType type, int arg);
extern int MCUIF_Read(int id, char *buf, int len);
extern int MCUIF_Write(int id, char *buf, int len);
int32_t MCUIF_SetTimeout(int32_t id, uint32_t timeout);
