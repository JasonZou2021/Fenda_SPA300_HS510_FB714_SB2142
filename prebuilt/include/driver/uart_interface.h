#ifndef __UART_INTERFACE_H__
#define __UART_INTERFACE_H__

#define UART_RW_MODE_ISR            0
#define UART_RW_MODE_POLLING        1
#define UART_DISABLE				0
#define UART_ENABLE					1

#define UART_CBK_RX_NOT_EMPTY       1 /* < rx fifo have data */
#define UART_CBK_TX_EMPTY           2 /* < tx fifo is empty */
#define UART_CBK_ERROR      		3 /* < uart transform error */

/* < don't do too much work in the callback function */
typedef int (*uart_callback)(int event, int param, void* context);

/****************************** UART driver return value ******************************/
#define UART_DRV_SUCCESS            0
#define UART_DRV_FAIL               -1
#define UART_DRV_INVALID_PARAM      -2
#define UART_DRV_BUSY               -3
#define UART_DRV_TIMEOUT            -4

/*---------------- UART Mode Parameters: Data Bits -----*/
#define DATA_BITS_5               (1UL << 8) /* < 5 Data bits */
#define DATA_BITS_6               (2UL << 8) /* < 6 Data bit */
#define DATA_BITS_7               (3UL << 8) /* < 7 Data bits */
#define DATA_BITS_8               (0UL << 8) /* < 8 Data bits (default) */

/*---------------- UART Mode Parameters: loopback test -----*/
#define LOOPBACK_DISABLE          (0UL << 11) /* < disable loopback (default) */
#define LOOPBACK_ENABLE           (1UL << 11) /* < enable loopback */

/*---------------- UART Mode Parameters: Parity -----*/
#define PARITY_NONE               (0UL << 12) /* < No Parity (default) */
#define PARITY_EVEN               (1UL << 12) /* < Even Parity */
#define PARITY_ODD                (2UL << 12) /* < Odd Parity */

/*---------------- UART Mode Parameters: Stop Bits -----*/
#define STOP_BITS_1               (0UL << 14) /* < 1 Stop bit (default) */
#define STOP_BITS_1P5_2           (1UL << 14) /* < word len = 5b, 1.5 stop bits, >5b 2 Stop bits */

/*---------------- UART Mode Parameters: Flow Control -----*/
#define FLOW_CONTROL_NONE         (0UL << 16) /* < No Flow Control (default) */
#define FLOW_CONTROL_RTS          (1UL << 16) /* < RTS Flow Control */
#define FLOW_CONTROL_CTS          (2UL << 16) /* < CTS Flow Control */
#define FLOW_CONTROL_RTS_CTS      (3UL << 16) /* < RTS/CTS Flow Control */

/*---------------------------IOCTL UART COMMAND------------------------------ -----*/
#define UART_NORMAL_MODE       		0x0A 	  /* < set uart nomal mode, arg = baud default(115200)*/
#define UART_UADMA_MODE     		0x0B 	  /* < set uart receive dma mode, arg = baud default(115200)*/


#define UART_CMD_DMA_BUF_SIZE  		0x0D 	  /* < set uart receive dma ring buffer size, default(1024)*/
#define UART_CMD_READ_TIMEOUT       0x0C      /* < set receive or send wait data timeout, arg = (ms) defaul(forever)*/
#define UART_POLLING_OR_IRQ         0x12      /* < UART receive or send mode, 1 polling mode or 0 irq mode*/
#define UART_RECV_BUF_EN  		 	0x13      /* < enable uart receive buffer mode, arg = receive buffer size*/
#define UART_SEND_BUF_EN  		 	0x14      /* < enable uart send buffer mode, arg = send buffer size */
#define UART_RECV_ABORT             0x15      /* < abort receive, clear the receive buffer and FIFO data*/
#define UART_SEND_ABORT             0x16      /* < abort send, clear the send buffer and FIFO data*/
#define UART_GET_RECV_BUF_CNT       0x17      /* < get the receive buffer data count byte*/
#define UART_GET_SEND_BUF_CNT       0x18      /* < get the send buffer data count byte*/
#define UART_REGISTER_CALLBACK      0x19      /* < register the callback function, arg = uart_callback function pointor*/
#define UART_SET_CALLBACK_CONTEXT   0x1a      /* < set the callback function context, default NULL*/



void uart0_init(void);

#endif
