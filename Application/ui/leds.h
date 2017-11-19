/*
 * leds.h
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */

#ifndef UI_LEDS_H_
#define UI_LEDS_H_

#define LED_STACK_SIZE	TASK_STACK_BYTES(512)

void LED_ToggleGreen(void);
void LED_ToggleRed(void);
void LED_SetGreen(_Bool state);
void LED_SetRed(_Bool state);
void Led_StartTasks(unsigned portBASE_TYPE uxPriority);

#endif /* UI_LEDS_H_ */
