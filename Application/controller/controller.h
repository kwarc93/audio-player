/*
 * controller.h
 *
 *  Created on: 19.11.2017
 *      Author: Kwarc
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#define CONTROLER_STACK_SIZE	TASK_STACK_BYTES(2 * 1024)

enum user_action
{
	// User actions
	PRESS_OK,
	PRESS_UP,
	PRESS_DOWN,
	PRESS_LEFT,
	PRESS_RIGHT
};

enum menu_action
{
	// Menu actions
	SELECT_THIS,
	SELECT_PREV,
	SELECT_NEXT
};

void Controller_StartTasks(unsigned portBASE_TYPE uxPriority);
void Controller_SetUserAction( enum user_action action );
void Controller_SetMenuAction( enum menu_action action, char* txt );

#endif /* CONTROLLER_H_ */
