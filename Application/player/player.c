/*
 * player.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */

// +--------------------------------------------------------------------------
// | @ Includes
// +------------------------------------------------------------------------
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"
#include "FreeRTOS/timers.h"

#include "cs43l22/cs43l22.h"
#include "player.h"

#include "usb_host.h"
#include "ui/display.h"
#include "decoder/decoder.h"
#include "filebrowser/file_browser.h"
#include "cpu_utils.h"
#include "misc.h"

#include <stdbool.h>
#include <string.h>

// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[PLAYER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

#define DEBUG_PLAYER_STATS		1

#define PLAYER_MAX_VOLUME		100
#define PLAYER_MIN_VOLUME		0

#define PLAYER_TIMER_PERIOD_MS		1000

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static struct player_context
{
	enum player_commands command;
	enum player_state state;

	char song_name[64];
	struct decoder_if decoder;
	uint8_t volume;
	_Bool mute;

	TaskHandle_t task;
	QueueHandle_t queue;
	TimerHandle_t timer;
} player;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

static void vTimerCallback( TimerHandle_t xTimer )
{
#if DEBUG_PLAYER_STATS
	extern uint8_t appHeap[];
	extern size_t get_used_size( void *mem_pool );

	DBG_PRINTF( "CPU Load: %u%%", Get_CPU_Usage() );
	DBG_PRINTF( "RTOS heap used: %uB", configTOTAL_HEAP_SIZE - xPortGetFreeHeapSize() );
	DBG_PRINTF( "TLSF heap used: %uB", get_used_size( appHeap ) );
#endif
}

static void TaskProcess( void )
{
	if( player.decoder.is_working() == false && player.state == PLAYER_PLAYING )
	{
		Player_SendCommand( PLAYER_STOP );
	}

	if( xQueueReceive( player.queue, &player.command, 0 ) )
	{
		switch( player.command )
		{
		case PLAYER_INIT:
			player.state = PLAYER_IDLE;
			break;
		case PLAYER_PLAY:
			if( USB_IsDiskReady() )
			{
				player.decoder.start( player.song_name );
				CS43L22_Play( CS43L22_I2C_ADDRESS, 0, 0 );
				player.state = PLAYER_PLAYING;
			}
			else
			{
				player.state = PLAYER_IDLE;
			}
			break;
		case PLAYER_PAUSE:
			CS43L22_Pause( CS43L22_I2C_ADDRESS );
			player.decoder.pause();
			player.state = PLAYER_PAUSED;
			break;
		case PLAYER_RESUME:
			CS43L22_Resume( CS43L22_I2C_ADDRESS );
			player.decoder.resume();
			player.state = PLAYER_PLAYING;
			break;
		case PLAYER_STOP:
			CS43L22_Stop( CS43L22_I2C_ADDRESS, CODEC_PDWN_SW );
			player.decoder.stop();
			player.state = PLAYER_STOPPED;
			// Autoplay feature
			Player_PlayNext();
			break;
		case PLAYER_NEXT:
		{
			if(FB_FindNextItem(FB_GetCurrentPath(), player.song_name, player.song_name))
			{
				Player_SendCommand( PLAYER_PLAY );
			}
		}
			break;
		case PLAYER_PREV:
			break;
		case PLAYER_VOLUME:
			CS43L22_SetVolume( CS43L22_I2C_ADDRESS, player.volume );
			break;
		case PLAYER_MUTE:
			CS43L22_SetMute( CS43L22_I2C_ADDRESS, player.mute );
			break;
		default:
			break;
		}
	}

}

static void vTaskPlayer( void * pvParameters )
{
	// Task's infinite loop
	for( ;; )
	{
		TaskProcess();
		vTaskDelay( 100 );
	}
	/* Should never go there */
	vTaskDelete( player.task );
}

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void Player_StartTasks( unsigned portBASE_TYPE uxPriority )
{
	// Init
	memset( &player, 0, sizeof(player) );
	player.volume = 50;

	Decoder_InitInterface( &player.decoder );

	if( !CS43L22_Init( CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, player.volume,
	AUDIO_FREQUENCY_44K ) )
	{
		DBG_PRINTF( "CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS) );
	}

	// Create queue for player states
	player.queue = xQueueCreate( 8, sizeof(enum player_commands) );

	// Create timer for printing CPU load
	player.timer = xTimerCreate( "TIMER", PLAYER_TIMER_PERIOD_MS / portTICK_PERIOD_MS, pdTRUE,
			(void*) 0, vTimerCallback );
	xTimerStart( player.timer, 1000 );

	Player_SendCommand( PLAYER_INIT );

	// Creating tasks
	if( xTaskCreate( vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority,
			&player.task ) == pdPASS )
	{
		DBG_PRINTF( "Task(s) started!" );
	}
}

void Player_SendCommand( enum player_commands command )
{
	if( !xQueueSend( player.queue, &command, 0 ) )
	{
		// Error!
		// Failed to send item to queue
	}
}

enum player_state Player_GetState( void )
{
	return player.state;
}

void Player_VolumeUp( void )
{
	if( player.volume < PLAYER_MAX_VOLUME )
		player.volume += 2;
	Player_SendCommand( PLAYER_VOLUME );
}

void Player_VolumeDown( void )
{
	if( player.volume > PLAYER_MIN_VOLUME )
		player.volume -= 2;
	Player_SendCommand( PLAYER_VOLUME );
}

void Player_Mute( _Bool state )
{
	player.mute = state;
	Player_SendCommand( PLAYER_MUTE );
}

void Player_SetSongName( char* name )
{
	strncpy( player.song_name, name, sizeof(player.song_name) );
}
void Player_PlayNext( void )
{
	Player_SendCommand( PLAYER_NEXT );
}

void Player_PlayPrev( void )
{
	/* Not implemented yet */
}

void Player_PlayPause( void )
{
	if( Player_GetState() == PLAYER_PAUSED )
		Player_SendCommand( PLAYER_RESUME );
	else if( Player_GetState() == PLAYER_PLAYING )
		Player_SendCommand( PLAYER_PAUSE );
}
