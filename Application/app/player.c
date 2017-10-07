/*
 * player.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "player.h"

#include "mp3dec.h"

#ifdef DEBUG
#include "debug.h"
#define DBG_PRINTF(...)	(Debug_Printf("[PLAYER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

static TaskHandle_t xHandleTaskPlayer;
static HMP3Decoder hMP3Decoder;

static void vTaskPlayer(void * pvParameters)
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for(;;)
	{
		// Delay
		vTaskDelayUntil( &xLastFlashTime, 1000/portTICK_PERIOD_MS );
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskPlayer);
}

void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	hMP3Decoder = MP3InitDecoder();
	if(hMP3Decoder != NULL)
	{
		DBG_PRINTF("Helix MP3 decoder initialized");
	}

	// Creating tasks
	xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &xHandleTaskPlayer);

	DBG_PRINTF("Task(s) started!");
}

