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

#define PLAYER_MAX_VOLUME			100
#define PLAYER_MIN_VOLUME			0

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

	struct decoder_if decoder;
	uint8_t volume;
	_Bool mute;

	TaskHandle_t xHandleTaskPlayer;
	QueueHandle_t qhPlayerState;
	TimerHandle_t thPlayerTimer;
}player;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

static void vTimerCallback(TimerHandle_t xTimer)
{
	DBG_PRINTF("CPU Load: %u%%", Get_CPU_Usage());
	DBG_PRINTF("RTOS heap bytes left: %u", xPortGetFreeHeapSize());
}

static void Player_TaskProcess(void)
{
	if(!player.decoder.is_working() && player.state == PLAYER_PLAYING)
	{
		Player_SendCommand(PLAYER_STOP);
		// @ TODO: Play next song in folder
	}

	if(xQueueReceive(player.qhPlayerState, &player.command, 0))
	{
		switch(player.command)
		{
		case PLAYER_INIT:
			player.state = PLAYER_IDLE;
			break;
		case PLAYER_PLAY:
			if(USB_IsDiskReady())
			{
				player.decoder.start("aac_ex.aac");
				CS43L22_Play(CS43L22_I2C_ADDRESS, 0, 0);
				Display_SendText("PLAYING");
				player.state = PLAYER_PLAYING;
			}
			else
			{
				player.state = PLAYER_IDLE;
			}
			break;
		case PLAYER_PAUSE:
			CS43L22_Pause(CS43L22_I2C_ADDRESS);
			player.decoder.pause();
			Display_SendText("PAUSE");
			player.state = PLAYER_PAUSED;
			break;
		case PLAYER_RESUME:
			CS43L22_Resume(CS43L22_I2C_ADDRESS);
			player.decoder.resume();
			Display_SendText("PLAYING");
			player.state = PLAYER_PLAYING;
			break;
		case PLAYER_STOP:
			CS43L22_Stop(CS43L22_I2C_ADDRESS, CODEC_PDWN_SW);
			player.decoder.stop();
			Display_SendText("STOP");
			player.state = PLAYER_STOPPED;
			break;
		case PLAYER_NEXT:
			break;
		case PLAYER_PREV:
			break;
		case PLAYER_VOLUME:
			CS43L22_SetVolume(CS43L22_I2C_ADDRESS, player.volume);
			break;
		case PLAYER_MUTE:
			CS43L22_SetMute(CS43L22_I2C_ADDRESS, player.mute);
			break;
		default:
			break;
		}
	}

}

static void vTaskPlayer(void * pvParameters)
{
	// Task's infinite loop
	for(;;)
	{
		Player_TaskProcess();
		vTaskDelay(100);
	}
	/* Should never go there */
	vTaskDelete(player.xHandleTaskPlayer);
}

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	memset(&player, 0, sizeof(player));
	player.volume = 50;

	Decoder_InitInterface(&player.decoder);

	if(!CS43L22_Init(CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, player.volume, AUDIO_FREQUENCY_44K))
	{
		DBG_PRINTF("CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS));
	}

	 // Create queue for player states
	player.qhPlayerState = xQueueCreate(8, sizeof(enum player_commands));

	// Create timer for printing CPU load
	player.thPlayerTimer = xTimerCreate("TIMER", PLAYER_TIMER_PERIOD_MS/portTICK_PERIOD_MS, pdTRUE, (void*)0, vTimerCallback);
	xTimerStart(player.thPlayerTimer, 1000);

	Player_SendCommand(PLAYER_INIT);

	// Creating tasks
	if(xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &player.xHandleTaskPlayer) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

void Player_SendCommand(enum player_commands command)
{
	if(!xQueueSend(player.qhPlayerState, &command, 0))
	{
		// Error!
		// Failed to send item to queue
	}
}

enum player_state Player_GetState(void)
{
	return player.state;
}

void Player_VolumeUp(void)
{
	if(player.volume < PLAYER_MAX_VOLUME)
		player.volume += 5;
	Player_SendCommand(PLAYER_VOLUME);
}

void Player_VolumeDown(void)
{
	if(player.volume > PLAYER_MIN_VOLUME)
		player.volume -= 5;
	Player_SendCommand(PLAYER_VOLUME);
}

void Player_Mute(_Bool state)
{
	player.mute = state;
	Player_SendCommand(PLAYER_MUTE);
}
