/*
 * player.h
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */

#ifndef APP_PLAYER_H_
#define APP_PLAYER_H_

#define PLAYER_STACK_SIZE	(1*1024)

enum player_states
{
	PLAYER_IDLE = 0,
	PLAYER_WAIT_FOR_DISK,
	PLAYER_PLAY,
	PLAYER_STOP,
	PLAYER_PAUSE,
	PLAYER_NEXT,
	PLAYER_PREV,

	PLAYER_TOP
};

void Player_StartTasks(unsigned portBASE_TYPE uxPriority);
void Player_SetState(enum player_states state);

#endif /* APP_PLAYER_H_ */
