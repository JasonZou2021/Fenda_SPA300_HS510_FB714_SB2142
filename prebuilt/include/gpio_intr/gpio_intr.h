/*${.::UserApp.h} ..............................................................*/

#ifndef __GPIO_INTR_H__

#define __GPIO_INTR_H__

#define GPIO_INTR_MAX_NUM 46

//enum for external interrupt source selection
enum
{
	eIntr_0 = 0, //remember to sync interrupt_interfaces.h IRQ_PI_GPIO_INT0
	eIntr_1,
	eIntr_2,
	eIntr_3,
	eIntr_num,
    eIntr0_iop,
    eIntr1_iop
};

//enum for interrupt polarity selection
enum
{
	eIntr_polarity_falling = 0,
	eIntr_polarity_raising,
	eIntr_polarity_num,
};

//enum for interrupt type selection
enum
{
	eIntr_type_edge = 0,
	eIntr_type_level,
	eIntr_type_num,
};


typedef void (*CbFunc_t)(void);
typedef void (*DsrCbFunc_t)(uint32_t arg);
void GPIO_INT_Clear_Flag(int intr_sel);

/******************************************************************************************/
/**
* \fn			void GPIO_DeInitIntr(int intr_sel)
*
* \brief		external gpio interrupt de-initial
*
* \param		int intr_sel: select interrupt 0 or 1 or 2 or 3. eIntr_0/ eIntr_1/ eIntr_2/ eIntr_3. \n

* \return		none
*
 ******************************************************************************************/
void GPIO_DeInitIntr(int intr_sel);
void GPIO_INT_ACK(void);

/******************************************************************************************/
/**
*\fn			int GPIO_ExIntr_init(int intr_sel, int polarity, int gpio_num, void const * pCaller, CbFunc_t pfCallbackFunc)
*
*\brief		external GPIO interrupt initial function
*
*\param		int intr_sel: select interrupt 0 or 1 or 2 or 3. eIntr_0/ eIntr_1/ eIntr_2/ eIntr_3. \n
*				int polarity : interrupt polarity : eIntr_polarity_falling/eIntr_polarity_raising \n
*				int type : interrupt type : eIntr_type_edge/eIntr_type_level \n
*				int gpio_num : external gpio number (only specific gpio supported) \n
*				void const * pCaller : caller AO \n
*				CbFunc_t pfCallbackFunc : interrupt callback function \n
*
*\return		-1 : initial fail \n
*				0 : initial success \n
*
******************************************************************************************/
int GPIO_ExIntr_init(int intr_sel, int polarity, int type, void const *pCaller, CbFunc_t pfCallbackFunc);

// for iop intr use
/**
 * @brief register iop intr 0 event with respective callback function and argument
 *
 * @param evt pick from enum intr0_evt
 * @param pfCallbackFunc respective callback function
 * @param arg argument of callback function
 */
void IOP_RegIntr0Callback(uint32_t evt, DsrCbFunc_t pfCallbackFunc, uint32_t arg);

/**
 * @brief register iop intr 1 event with respective callback function and argument
 *
 * @param evt pick from enum intr1_evt
 * @param pfCallbackFunc respective callback function
 * @param arg argument of callback function
 */
void IOP_RegIntr1Callback(uint32_t evt, DsrCbFunc_t pfCallbackFunc, uint32_t arg);

#endif /* __GPIO_INTR_H__ */
