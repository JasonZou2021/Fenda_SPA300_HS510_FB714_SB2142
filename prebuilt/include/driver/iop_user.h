#ifndef _IOP_USER_H_
#define _IOP_USER_H_

#define ROTARY_IDX1  1
#define ROTARY_IDX2  2
#define ROTARY_IDX3  3
#define ROTARY_IDX4  4

typedef enum {
    SRC_HPD = 1,
    SRC_CEC,
    SRC_IR,
    SRC_PWRKEY,
    SRC_RTC,
    SRC_GPIO25,
    SRC_GPIO26,
    SRC_GPIO27,
    SRC_GPIO28,
    SRC_GPIO29,
    SRC_GPIO30
} src_type;

typedef enum {
    INTR0_EVT_GPIO25_TRIG = 1,
    INTR0_EVT_IR_TRIG,
    INTR0_EVT_MAX
} intr0_evt;

typedef enum {
    INTR1_EVT_ROT1_UPDATED = 1,
    INTR1_EVT_ROT2_UPDATED,
    INTR1_EVT_ROT3_UPDATED,
    INTR1_EVT_ROT4_UPDATED,
    INTR1_EVT_MAX
} intr1_evt;

/**
 * @brief start polling repeater intr pin
 *
 */
void IOP_RepeaterIntrPollingStart(void);

/**
 * @brief stop polling repeater intr pin
 *
 */
void IOP_RepeaterIntrPollingStop(void);

/**
 * @brief send serial data to repective pin
 *
 * @param data serial data
 * @param len data length
 */
void IOP_SendSerialData(uint8_t *data, uint32_t len);

/**
 * @brief get the direction and speed counter of respective rotary encoder
 *
 * @param idx ROTARY_IDX1 to ROTARY_IDX4
 * @param pDirection pointer to value 1: clockwise, 0: counter clockwise
 * @param pSpeedCntr pointer to speed counter, sample rate is 500us
 */
void IOP_GetRotary(uint32_t idx, uint32_t *pDirection, uint32_t *pSpeedCntr);

/**
 * \brief enable rotary encoder
 *
 * \param idx rotary encoder idx
 */
void IOP_EnableRotary(uint32_t idx);

#ifdef WD_TICK_ENABLE
int32_t iop_wd_enable(void);
int32_t iop_wd_disable(void);
int32_t iop_wd_tick(void);
int32_t iop_wd_set_timeout(uint8_t msec);
#endif

#endif
