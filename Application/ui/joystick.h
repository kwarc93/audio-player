/*
 * joystick.h
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#ifndef UI_JOYSTICK_H_
#define UI_JOYSTICK_H_

#define JOYSTICK_STACK_SIZE	TASK_STACK_BYTES(1024)

void Joystick_StartTasks(unsigned portBASE_TYPE uxPriority);

#endif /* UI_JOYSTICK_H_ */
