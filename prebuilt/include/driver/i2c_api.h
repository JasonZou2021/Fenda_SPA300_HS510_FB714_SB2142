#ifndef __I2C_API_H__
#define __I2C_API_H__
#include "types.h"

// for work mode setting
#define POLLING_MODE        0
#define INTERRUPT_MODE      1

#define REPEATSTARTCMD_NUM  2

/**
 *\brief     The i2c driver request code
 */
#define SETPARAMETERS       (1) /** set i2c parameters */
#define SETREPEATSTART      (2) /** 1/0-set repeat start to enable or disable*/
#define SETREPEATSTARTCMD0  (3) /** set repeat start cmd data address */
#define SETREPEATSTARTCMD1  (4) /** set repeat start cmd data len */
#define SETADDRTYPE         (5) /** set address type :7 bit or 10 bit*/
#define SETPERIOD           (6) /** set i2c stretch perio*/
#define SETTRYTIME          (7) /** set i2c stretch trytime*/
#define SETWORKMODE         (8) /** set i2c work mode 0:polling !0:handle of the i2c callback function*/
#define SETABORT            (9) /** abort current i2c operation*/
#define SETFREQ             (10)/** set i2c frequency*/
#define SETFREQ_ACK         (11)/** set i2c frequency for ack*/
#define SETTIMEOUT          (12)/** set i2c interrupt notify timeout*/

typedef struct {
    uint8_t  *buf;     // i2c write: data buf to write, i2c read: data buf to store read data
    uint32_t cmd;      // command or register
    uint32_t dataLen;  // r/w data length
    uint16_t slaveAddr;// i2c slave addr
    uint8_t  cmdLen;   // command or register length
    uint8_t  showMsg;  // 0: no msg, 1: show i2c r/w data msg
} I2cData_t;

/**
 * @brief i2c read
 *
 * @param pIdStr i2c device id
 * @param pData pointer to I2cData_t
 * @return int32_t TRUE: success, FALSE: fail
 */
int32_t I2C_Read(uint8_t *pIdStr, I2cData_t *pData);

/**
 * @brief i2c write
 *
 * @param pIdStr i2c device id
 * @param pData pointer to I2cData_t
 * @return int32_t TRUE: success, FALSE: fail
 */
int32_t I2C_Write(uint8_t *pIdStr, I2cData_t *pData);

/**
 * @brief set hw i2c work mode, POLLING_MODE or INTERRUPT_MODE
 *
 * @param pIdStr i2c device id, should be hw i2c id
 * @param mode POLLING_MODE or INTERRUPT_MODE
 * @return int32_t 0: success, -1: open fail
 */
int32_t I2C_SetWorkMode(uint8_t *pIdStr, uint32_t mode);// only for hw i2c

/**
 * @brief set i2c bus freq, only for hw i2c
 *
 * @param pIdStr i2c device id, should be hw i2c id
 * @param freq i2c bus frequency, unit is kHz
 * @return int32_t 0: success, -1: open fail
 */
int32_t I2C_SetFreq(uint8_t *pIdStr, uint32_t freq);//freq unit = kHz, only for hw i2c

int32_t I2C_SetFreq_Ack(uint8_t *pIdStr, uint32_t freq);

/**
 * @brief set interrupt notify timeout when i2c read/write, only for hw i2c
 *
 * @param pIdStr should be hw i2c id
 * @param timeout unit is ms
 * @return int32_t 0: success, -1: open fail
 */
int32_t I2C_SetTimeout(uint8_t *pIdStr, uint32_t timeout);

#endif
