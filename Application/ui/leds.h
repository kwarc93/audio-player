/*
 * leds.h
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */

#ifndef UI_LEDS_H_
#define UI_LEDS_H_

#define LED_STACK_SIZE	TASK_STACK_BYTES(512)

void Led_StartTasks(unsigned portBASE_TYPE uxPriority);

#endif /* UI_LEDS_H_ */
