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
	enum user_action action;

	TaskHandle_t task;
	QueueHandle_t queue;

}controller;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void press_ok_handle(void)
{
//	switch(Player_GetState())
//	{
//	case PLAYER_IDLE:
//	case PLAYER_STOPPED:
//		Player_SendCommand(PLAYER_PLAY);
//		break;
//	case PLAYER_PAUSED:
//		Player_SendCommand(PLAYER_RESUME);
//		break;
//	case PLAYER_PLAYING:
//		Player_SendCommand(PLAYER_PAUSE);
//		break;
//	default:
//		break;
//	}

	Menu_Click();
}

static void press_up_handle(void)
{
	Player_VolumeUp();
}

static void press_down_handle(void)
{
	Player_VolumeDown();
}

static void press_left_handle(void)
{
//	Player_PlayPrev();
	Menu_SelectPrev();
}

static void press_right_handle(void)
{
//	Player_PlayNext();
	Menu_SelectNext();
}

static void TaskProcess(void)
{
	if(xQueueReceive(controller.queue, &controller.action, 0))
	{
		switch(controller.action)
		{
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
}

static void vTaskController(void * pvParameters)
{
	// Task's infinite loop
	for(;;)
	{
		TaskProcess();
		vTaskDelay(50);
	}
	/* Should never go there */
	vTaskDelete(controller.task);
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void Controller_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	memset(&controller, 0, sizeof(controller));

	 // Create input queue for user actions
	controller.queue = xQueueCreate(8, sizeof(enum user_action));

	// Creating tasks
	if(xTaskCreate(vTaskController, "CONTROLLER", CONTROLER_STACK_SIZE, NULL, uxPriority, &controller.task) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

void Controller_SetUserAction(enum user_action action)
{
	if(!xQueueSend(controller.queue, (void*)&action, 0))
	{
		// Error!
		// Failed to send item to queue
	}
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
