/*
 * controller.h
 *
 *  Created on: 19.11.2017
 *      Author: Kwarc
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_


#define CONTROLER_STACK_SIZE	TASK_STACK_BYTES(1 * 1024)

enum user_action
{
	PRESS_OK,
	PRESS_UP,
	PRESS_DOWN,
	PRESS_LEFT,
	PRESS_RIGHT
};

void Controller_StartTasks(unsigned portBASE_TYPE uxPriority);
void Controller_SetUserAction(enum user_action action);

#endif /* CONTROLLER_H_ */
