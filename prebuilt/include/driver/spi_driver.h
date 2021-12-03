#ifndef __SPI_DRIVER_H
#define __SPI_DRIVER_H
#include "types.h"

/**
 * @brief SPI_IOCTL defines the supported commands for ioctl API.
 */
enum SPI_IOCTL {
	/**
	 * @brief set the divisor of sys_clk.
	 * the 'data' is the divisor, the output spi_clk would be calculated
	 * as the formula: ' spi_clk = sys_clk/(2*data+1) '.
	 */
	SPI_IOCTL_SET_CLKDIV,

	/**
	 * @brief get the divisor of sys_clk.
	 * the 'data' is a pointer of uint32_t, which is for the return value.
	 */
	SPI_IOCTL_GET_CLKDIV,

	/**
	 * @brief set the spi mode.
	 * refer to SPI_MODE for valid value.
	 */
	SPI_IOCTL_SET_MODE,

	/**
	 * @brief get the spi mode.
	 * the 'data' is a pointer of uint32_t, which is for the return value.
	 */
	SPI_IOCTL_GET_MODE,

	/**
	 * @brief set delay cycles for sampling bits when receive data.
	 */
	SPI_IOCTL_SET_RDLY,

	/**
	 * @brief set delay cycles for sampling bits when receive data.
	 * the 'data' is a pointer of uint32_t, which is for the return value.
	 */
	SPI_IOCTL_GET_RDLY,

	/**
	 * @brief set the bit order of words to be sent/received.
	 * refer to SPI_BITORDER for valid value.
	 */
	SPI_IOCTL_SET_BITORDER,

	/**
	 * @brief set the bit order of words to be sent/received.
	 * the 'data' is a pointer of uint32_t, which is for the return value.
	 */
	SPI_IOCTL_GET_BITORDER,

	/**
	 * @brief set the tranfer mode.
	 * refer to SPI_TRSMODE for valid value.
	 */
	SPI_IOCTL_SET_TRSMODE,

	/**
	 * @brief set the tranfer mode.
	 * the 'data' is a pointer of uint32_t, which is for the return value.
	 */
	SPI_IOCTL_GET_TRSMODE,

	/**
	 * @brief transfer data in full-duplex mode.
	 * the 'data' should be a pointer of struct iovec.
	 */
	SPI_IOCTL_TRANSFER,
};

enum SPI_MODE {
	SPI_MODE_CPOL0_CPHA0 = 0,
	SPI_MODE_CPOL0_CPHA1,
	SPI_MODE_CPOL1_CPHA0,
	SPI_MODE_CPOL1_CPHA1,
	SPI_MODE_UNKNOWN,
};

enum SPI_BITORDER {
	SPI_BITORDER_LSB = 0,
	SPI_BITORDER_MSB,
	SPI_BITORDER_UNKNOWN,
};

enum SPI_TRSMODE {
	SPI_TRSMODE_DMA_IRQ = 0,
	SPI_TRSMODE_DMA_POLLING,
	SPI_TRSMODE_BURST_IRQ,
	SPI_TRSMODE_BURST_POLLING,
	SPI_TRSMODE_UNKNOWN,
};

typedef struct _trs_buf {
	void  *addr;
	UINT32 len;
} trs_buf;


#endif // __SPI_DRIVER_H

