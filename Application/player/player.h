/*
 * player.h
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */

#ifndef APP_PLAYER_H_
#define APP_PLAYER_H_

#define PLAYER_STACK_SIZE	TASK_STACK_BYTES(4 * 1024)

enum player_commands
{
	PLAYER_INIT = 0,
	PLAYER_PLAY,
	PLAYER_STOP,
	PLAYER_PAUSE,
	PLAYER_RESUME,
	PLAYER_NEXT,
	PLAYER_PREV,
	PLAYER_VOLUME,
	PLAYER_MUTE,

	PLAYER_TOP
};

enum player_state
{
	PLAYER_IDLE = 0, PLAYER_PLAYING, PLAYER_PAUSED, PLAYER_STOPPED
};

void Player_StartTasks(unsigned portBASE_TYPE uxPriority);
void Player_SendCommand( enum player_commands command );
enum player_state Player_GetState( void );
void Player_SetSongName( char* name );

void Player_VolumeUp( void );
void Player_VolumeDown( void );
void Player_Mute( _Bool state );

void Player_PlayNext( void );
void Player_PlayPrev( void );

#endif /* APP_PLAYER_H_ */
