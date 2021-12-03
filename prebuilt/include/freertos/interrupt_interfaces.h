/**************************************************************************/
/**
*
* \file	interrupt_interfaces.h
*
* \brief	This file declare interrupt interfaces and data types\n
* \note  	Copyright (c) 2016 Sunplus Technology Co., Ltd. \n
*		All rights reserved.
*
*
* \author   xh.huang@sunplus.com.cn
* \version	 v0.01
* \date 	 2016/05/16
***************************************************************************/
#ifndef __INTERRUPT_INTERFACES_H__
#define __INTERRUPT_INTERFACES_H__

enum
{
	IRQ_PI_GPIO_INT0													 = 0,
	IRQ_PI_GPIO_INT1													 = 1,
	IRQ_PI_GPIO_INT2													 = 2,
	IRQ_PI_GPIO_INT3													 = 3,
	IRQ_I2CM0_INT														 = 8,
	IRQ_DDC0_INT														 = 10,
	IRQ_CARD_CTL1_INT													 = 16,
	IRQ_SPI_INT															 = 18,
	IRQ_SPI_MASTER_RISC_INT												 = 21,
	IRQ_MSI_UADMA0_INT													 = 32,
	IRQ_AXI_MON_INT														 = 34,
	IRQ_UADBG_INT														 = 35,
	IRQ_CPU0_DIRECT_INT0												 = 36,
	IRQ_CPU0_DIRECT_INT1												 = 37,
	IRQ_CPU0_DIRECT_INT2												 = 38,
	IRQ_CPU0_DIRECT_INT3												 = 39,
	IRQ_CPU0_DIRECT_INT4												 = 40,
	IRQ_CPU0_DIRECT_INT5												 = 41,
	IRQ_CPU0_DIRECT_INT6												 = 42,
	IRQ_CPU0_DIRECT_INT7												 = 43,
	IRQ_CPU1_DIRECT_INT0												 = 44,
	IRQ_CPU1_DIRECT_INT1												 = 45,
	IRQ_CPU1_DIRECT_INT2												 = 46,
	IRQ_CPU1_DIRECT_INT3												 = 47,
	IRQ_CPU1_DIRECT_INT4												 = 48,
	IRQ_CPU1_DIRECT_INT5												 = 49,
	IRQ_CPU1_DIRECT_INT6												 = 50,
	IRQ_CPU1_DIRECT_INT7												 = 51,
	IRQ_CPU0_INT														 = 52,
	IRQ_CPU1_INT														 = 53,
	IRQ_IOP_INT0														 = 62,
	IRQ_IOP_INT1														 = 63,
	IRQ_UA0_INT															 = 65,
	IRQ_UA1_INT															 = 66,
	IRQ_UA2_INT															 = 67,
	IRQ_UADMA0_INT														 = 71,
	IRQ_CBDMA0_INT														 = 74,
	IRQ_RBUS_INTERRUPT													 = 76,
	IRQ_LOSD_INT														 = 78,
	IRQ_AUD_FIFO_INT													 = 79,
	IRQ_SAR_SIG_DET_INT													 = 80,
	IRQ_SAR_DATA_RDY_INT												 = 81,
	IRQ_ICM_INT0														 = 92,
	IRQ_ICM_INT1														 = 93,
	IRQ_ICM_INT2														 = 94,
	IRQ_ICM_INT3														 = 95,
	IRQ_TGEN0_INT_FIELD_START											 = 98,
	IRQ_TGEN0_INT_FIELD_END												 = 99,
	IRQ_TGEN0_INT_USER1													 = 100,
	IRQ_TGEN0_INT_USER2													 = 101,
	IRQ_CEC_INT															 = 110,
	IRQ_IR_INT0															 = 111,
	IRQ_IR_INT1															 = 112,
	IRQ_HDMI_TX_INT														 = 113,
	IRQ_DSP_INTOUT0														 = 126,
	IRQ_DSP_INTOUT1														 = 127,
	IRQ_STC_INTERRUPT_TIMERW											 = 128,
	IRQ_STC_INTERRUPT_TIMER0											 = 129,
	IRQ_STC_INTERRUPT_TIMER1											 = 130,
	IRQ_STC_INTERRUPT_TIMER2B											 = 131,
	IRQ_STC_INTERRUPT_TIMER3B											 = 132,
	IRQ_STC_AV0_INTERRUPT_TIMER0										 = 133,
	IRQ_STC_AV0_INTERRUPT_TIMER1										 = 134,
	IRQ_STC_AV0_INTERRUPT_TIMER2B										 = 135,
	IRQ_STC_AV0_INTERRUPT_TIMER3B										 = 136,
	IRQ_STC_AV1_INTERRUPT_TIMER0										 = 137,
	IRQ_STC_AV1_INTERRUPT_TIMER1										 = 138,
	IRQ_STC_AV1_INTERRUPT_TIMER2B										 = 139,
	IRQ_STC_AV1_INTERRUPT_TIMER3B										 = 140,
	IRQ_STC_AV2_INTERRUPT_TIMER0										 = 142,
	IRQ_STC_AV2_INTERRUPT_TIMER1										 = 143,
	IRQ_STC_AV2_INTERRUPT_TIMER2B										 = 144,
	IRQ_STC_AV2_INTERRUPT_TIMER3B										 = 145,
	IRQ_STC_AV0_INTERRUPT_TIMERW										 = 146,
	IRQ_UPHY0_INT														 = 147,
	IRQ_USBC0_DEVICE_INT												 = 182,
	IRQ_USBC0_EHCI_INT													 = 183,
	IRQ_USBC0_OHCI_INT													 = 184,
	IRQ_MAX,
};

/**
*\brief  Soft1 interrupt and syscall interrupt number
*/
typedef enum
{
	IRQ_SOFT1 = 0 + IRQ_MAX,
	IRQ_SYSCALL = 1 + IRQ_MAX,
	IRQ_TOTAL_EDGE
}OTHER_NUM;

/**
*\brief  IRQ triggering type
*/
#define IRQ_TYPE_EDGE		0
#define IRQ_TYPE_LEVEL		1

/**
*\brief  IRQ polarity type
*/
#define IRQ_POLARITY_LOW	0
#define IRQ_POLARITY_HIGH	1

/**
*\brief  Values for in/out of ISR context
*/
#define OUTOF_INTR_CONTEXT	(0Ul)
#define IN_INTR_CONTEXT	(1Ul)

/**
*\brief  Data type interrupt handler
*/
typedef void (*PINTRHANDLER)(void);
#define NULLHANDLER ((PINTRHANDLER)0)

/******************************************************************************************/
/**
 * \fn		int RegisterInterrupHandler(int intrNum, PINTRHANDLER handler)
 *
 * \brief		Regist interrupt handler whith interrupt number
 *
 * \param		sdIntrNum:(Input)Intrrupt number which define in intedef.h
 * \param		pHandler:(Input)the interrupt handler which you want to regist on the intr number
 *
 * \return	The whether regist handler success
 *
 ******************************************************************************************/
int RegisterInterrupHandler(int sdIntNum, PINTRHANDLER pHandler);

/******************************************************************************************/
/**
 * \fn		int UnRegisterInterrupHandler(int sdIntrNum)
 *
 * \brief		unRegist interrupt handler whith interrupt number
 *
 * \param		sdIntrNum:(Input)Intrrupt number which define in intedef.h
 *
 * \return	The whether unregist handler success
 *
 ******************************************************************************************/
int UnRegisterInterrupHandler(int sdIntNum);

unsigned int EnableInterruptByIRQ(unsigned int vector);
unsigned int DisableInterruptByIRQ(unsigned int vector);
void AckInterruptByIRQ(int vector);
void ConfigInterruptByIRQ(int vector, int type, int polarity);

#endif	//__INTERRUPT_INTERFACES_H__

