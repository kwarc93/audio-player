/*
 * player.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"
#include "cs43l22/cs43l22.h"
#include "player.h"

#include "usb_host.h"
#include "ui/display.h"
#include "decoder.h"
#include "misc.h"

#include <stdbool.h>
#include <string.h>

#ifdef DEBUG
#include "debug.h"
#define DBG_PRINTF(...)	(Debug_Printf("[PLAYER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

static struct player_context
{
	enum player_states state;
	struct decoder_if decoder;

	TaskHandle_t xHandleTaskPlayer;
	QueueHandle_t qhPlayerState;
}player;

static void Player_TaskProcess(enum player_states state)
{
	switch(state)
	{
	case PLAYER_IDLE:
		break;
	case PLAYER_WAIT_FOR_DISK:
		if(USB_IsDiskReady())
		{
			player.state = PLAYER_PLAY;
		}
		break;
	case PLAYER_PLAY:
		player.decoder.start("hs_wav.wav");
		CS43L22_Play(CS43L22_I2C_ADDRESS, 0, 0);
		Display_SendText("PLAYING");
		player.state = PLAYER_IDLE;
		break;
	case PLAYER_PAUSE:
		I2S_StopDMA();
		CS43L22_Pause(CS43L22_I2C_ADDRESS);
		Display_SendText("STOP");
		player.state = PLAYER_IDLE;
		break;
	case PLAYER_STOP:
		I2S_StopDMA();
		CS43L22_Stop(CS43L22_I2C_ADDRESS, CODEC_PDWN_SW);
		player.decoder.stop();
		Display_SendText("STOP");
		player.state = PLAYER_IDLE;
		break;
	default:
		player.state = PLAYER_IDLE;
		break;
	}

}
static void vTaskPlayer(void * pvParameters)
{
	// Task's infinite loop
	for(;;)
	{
		Player_TaskProcess(player.state);
		vTaskDelay(100);
	}
	/* Should never go there */
	vTaskDelete(player.xHandleTaskPlayer);
}

void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	player.state = PLAYER_WAIT_FOR_DISK;
	Decoder_InitInterface(&player.decoder);
	if(!CS43L22_Init(CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, 40, AUDIO_FREQUENCY_44K))
	{
		DBG_PRINTF("CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS));
	}

	 // Create queue for player states
	player.qhPlayerState = xQueueCreate(4, sizeof(enum player_states));
	// Creating tasks
	if(xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &player.xHandleTaskPlayer) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

void Player_SetState(enum player_states state)
{
	player.state = state;
}
