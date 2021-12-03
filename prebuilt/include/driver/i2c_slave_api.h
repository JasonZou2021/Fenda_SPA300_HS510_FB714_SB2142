#ifndef __I2CS_API_H__
#define __I2CS_API_H__
#include "types.h"
#include "ring_buf.h"

#define SETDEVICEADDR    1// set i2c slave address
#define GETDEVICEADDR    2// get i2c slave address
#define SETSRAMINDEX     3// set i2c sram index
#define SETCALLBACK      4// set i2c slave callback function
#define GETWRITEDATABUF  5// get the ring buffer that stored i2c master write data
typedef struct {
    uint8_t  *buf;     // i2c write: data buf to write, i2c read: data buf to store read data
    uint32_t cmd;      // command or register
    uint32_t dataLen;  // r/w data length
} I2csData_t;

enum {
    I2CS_EVENT_READ, // master has read data from i2c slave
    I2CS_EVENT_WRITE,// master has write data to i2c slave
};

typedef void (*i2cs_event_handler)(uint32_t event); // call back function type

/**
 * @brief read slave i2c reg to pData
 *
 * @param pData pointer to I2csData_t
 * @return int32_t TRUE: success, FALSE: fail
 */
int32_t I2CS_ReadData(I2csData_t *pData);

/**
 * @brief write pData to slave i2c reg
 *
 * @param pData pointer to I2csData_t
 * @return int32_t TRUE: success, FALSE: fail
 */
int32_t I2CS_WriteData(I2csData_t *pData);

/**
 * @brief set i2cs call back function
 *
 * @param cb cb function is i2cs_event_handler type
 * @return int32_t 0: success, -1: open fail
 */
int32_t I2CS_SetCallback(i2cs_event_handler cb);

/**
 * @brief set slave i2c addr
 *
 * @param addr slave i2c addr
 * @return int32_t 0: open device success, -1: open device fail
 */
int32_t I2CS_SetAddr(uint8_t addr);

/**
 * @brief get slave i2c addr
 *
 * @return int32_t > 0: addr, -1: open device fail
 */
int32_t I2CS_GetAddr(void);

/**
 * @brief get the ring buffer that stored i2c master write data
 *        data is [data1 byte][data1 addr byte][data2 byte][data2 addr byte]...
 * @return RingBuf_t* pointer to ring buffer
 */
RingBuf_t *I2CS_GetWriteDataBuf(void);
#endif
