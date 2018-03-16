/*
 * controller.c
 *
 *  Created on: 19.11.2017
 *      Author: Kwarc
 */

// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"
#include "FreeRTOS/timers.h"
#include "main.h"

#include "controller.h"

#include "player/player.h"
#include "Menu/menu.h"

#include <string.h>
#include <stdbool.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[CONTROLLER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static struct controller_context
{
	enum user_action u_action;
	enum menu_action m_action;

	TaskHandle_t task;
	QueueHandle_t user_queue;
	QueueHandle_t menu_queue;

} controller;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void press_ok_handle( void )
{
	Menu_Click();
}

static void press_up_handle( void )
{
	Menu_SelectPrev();
}

static void press_down_handle( void )
{
	Menu_SelectNext();
}

static void press_left_handle( void )
{
	Player_VolumeDown();
}

static void press_right_handle( void )
{
	Player_VolumeUp();
	Player_PlayNext();
}

static void TaskProcess( void )
{
	if( xQueueReceive( controller.user_queue, &controller.u_action, 0 ) )
	{
		switch( controller.u_action )
		{
		// User actions
		case PRESS_OK:
			press_ok_handle();
			break;
		case PRESS_UP:
			press_up_handle();
			break;
		case PRESS_DOWN:
			press_down_handle();
			break;
		case PRESS_LEFT:
			press_left_handle();
			break;
		case PRESS_RIGHT:
			press_right_handle();
			break;
		default:
			break;
		}
	}
	if( xQueueReceive( controller.menu_queue, &controller.m_action, 0 ) )
	{
		switch( controller.m_action )
		{
		// Menu actions
		case SELECT_THIS:
		{
			Player_SendCommand( PLAYER_PLAY );
			break;
		}
		case SELECT_PREV:
			break;
		case SELECT_NEXT:
			break;

		default:
			break;
		}
	}
}

static void vTaskController( void * pvParameters )
{
	// Task's infinite loop
	for( ;; )
	{
		TaskProcess();
		vTaskDelay( 50 );
	}
	/* Should never go there */
	vTaskDelete( controller.task );
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void Controller_StartTasks( unsigned portBASE_TYPE uxPriority )
{
	memset( &controller, 0, sizeof(controller) );

	// Create input queue for user actions
	controller.user_queue = xQueueCreate( 2, sizeof(enum user_action) );
	// Create input queue for menu actions
	controller.menu_queue = xQueueCreate( 1, sizeof(enum menu_action) );

	// Creating tasks
	if( xTaskCreate( vTaskController, "CONTROLLER", CONTROLER_STACK_SIZE, NULL, uxPriority,
			&controller.task ) == pdPASS )
	{
		DBG_PRINTF( "Task(s) started!" );
	}
}

void Controller_SetUserAction( enum user_action action )
{
	if( !xQueueSend( controller.user_queue, (void* )&action, 0 ) )
	{
		// Error!
		// Failed to send item to queue
	}
}

void Controller_SetMenuAction( enum menu_action action, char* txt )
{
	Player_SetSongName( txt );

	if( !xQueueSend( controller.menu_queue, (void* )&action, 0 ) )
	{
		// Error!
		// Failed to send item to queue
	}
}

// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
