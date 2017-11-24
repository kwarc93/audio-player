/*
 * main.h
 *
 *  Created on: 16.02.2017
 *      Author: Kwarc
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "stm32l476xx.h"

#define USE_HSI_CLOCK
#define USE_ART_ACCELERATOR		(0)

#define mainFLASH_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )
#define TASK_STACK_BYTES(bytes)	(((bytes) < configMINIMAL_STACK_SIZE * 4) ? configMINIMAL_STACK_SIZE : ((bytes) / 4))

#define assert_param(expr) 		((void)0U)

/* Macros for dummies ;) */

#define SET_BIT(REG, BIT)     	((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)		((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)		((REG) & (BIT))

#define CLEAR_REG(REG)			((REG) = (0x0))

#define WRITE_REG(REG, VAL)   	((REG) = (VAL))

#define READ_REG(REG)			((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))

#define POSITION_VAL(VAL)		(__CLZ(__RBIT(VAL)))

#define max(a,b) (a>=b?a:b)
#define min(a,b) (a<=b?a:b)

/**
 * @}
 */
typedef enum
{
	STATUS_OK = 0,
	STATUS_ERROR = 1,
	STATUS_TIMEOUT = 2
}
status_t;
/**
 * @}
 */
#endif /* MAIN_H_ */
