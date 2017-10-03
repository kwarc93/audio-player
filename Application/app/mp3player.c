/*
 * mp3player.c
 *
 *  Created on: 02.10.2017
 *      Author: Kwarc
 */

#include "gpio/gpio.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "mp3player.h"
#include "usb_host.h"


#ifdef DEBUG
#include "debug.h"
#define DBG_MSG(...)	(Debug_Msg("[PLAYER] " __VA_ARGS__))
#else
#define DBG_MSG(...)
#endif

static TaskHandle_t xHandleTaskPlayer;

static void vTaskMp3Player(void * pvParameters)
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for(;;)
	{
		// Delay
		vTaskDelayUntil( &xLastFlashTime, 20/portTICK_PERIOD_MS );

		USB_HOST_Process();
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskPlayer);
}

void Mp3Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	USB_HOST_Init();
	// Creating task for PLAYER
	xTaskCreate(vTaskMp3Player, "PLAYER", MP3PLAYER_STACK_SIZE, NULL, uxPriority, &xHandleTaskPlayer);

	DBG_MSG("Task(s) started!");
}
