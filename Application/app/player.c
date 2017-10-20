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

#include "FatFs/ff.h"
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

#define AUDIO_BUFFER_LENGTH			(4096)

static SemaphoreHandle_t shI2SEvent;
static TaskHandle_t xHandleTaskPlayer;
static QueueHandle_t qhPlayerState;
static enum player_states player_state;

static struct audio_file
{
	FIL file;
	FILINFO info;
	FRESULT res;
	UINT bytes_read;

	int16_t buffer[AUDIO_BUFFER_LENGTH];
	int16_t* buffer_ready_part;

	void (*decode)(void* enc_buf, void* dec_buf);

} audio;

void I2S_HalfTransferCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	audio.buffer_ready_part = &audio.buffer[0];
	xSemaphoreGiveFromISR(shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void I2S_TransferCompleteCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	audio.buffer_ready_part = &audio.buffer[AUDIO_BUFFER_LENGTH/2];
	xSemaphoreGiveFromISR(shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void Player_TaskProcess(enum player_states state)
{
	switch(state)
	{
	case PLAYER_IDLE:
		vTaskDelay(100);
		break;
	case PLAYER_WAIT_FOR_DISK:
		if(USB_IsDiskReady())
		{
			player_state = PLAYER_INIT;
		}
		vTaskDelay(100);
		break;
	case PLAYER_INIT:
		memset(audio.buffer, 0, sizeof(audio.buffer));
		if(f_open(&audio.file, "hs_wav.wav", FA_READ) == FR_OK)
		{
			DBG_PRINTF("File: hs_wav.wav");
			Display_SendText("HAPPYSAD - BEZ ZNIECZULENIA");

			I2S_TxDMA(audio.buffer, AUDIO_BUFFER_LENGTH);
			CS43L22_Play(CS43L22_I2C_ADDRESS, 0, 0);

			player_state = PLAYER_PLAY;
		}
		break;
	case PLAYER_PLAY:
		if(xSemaphoreTake(shI2SEvent, portMAX_DELAY) == pdTRUE)
		{
			audio.res = f_read(&audio.file, audio.buffer_ready_part,
						sizeof(audio.buffer)/2, &audio.bytes_read);

			audio.decode(audio.buffer_ready_part, audio.buffer_ready_part);

			if(audio.bytes_read != sizeof(audio.buffer)/2 || audio.res != FR_OK)
			{
				player_state = PLAYER_STOP;
			}
		}
		break;
	case PLAYER_PAUSE:
		I2S_StopDMA();
		CS43L22_Pause(CS43L22_I2C_ADDRESS);
		Display_SendText("STOP");
		player_state = PLAYER_IDLE;
		break;
	case PLAYER_STOP:
		I2S_StopDMA();
		CS43L22_Stop(CS43L22_I2C_ADDRESS, CODEC_PDWN_SW);
		Display_SendText("STOP");
		f_close(&audio.file);
		player_state = PLAYER_IDLE;
		break;
	default:
		player_state = PLAYER_IDLE;
		break;
	}
}
static void vTaskPlayer(void * pvParameters)
{
	// Task's infinite loop
	for(;;)
	{
		Player_TaskProcess(player_state);
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskPlayer);
}

void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	player_state = PLAYER_WAIT_FOR_DISK;

	audio.buffer_ready_part = &audio.buffer[0];
	audio.decode = Decoder_DecodeAudio;

	 // Create queue for player states
	qhPlayerState = xQueueCreate(4, sizeof(enum player_states));

	// Create and take binary semaphore
	vSemaphoreCreateBinary(shI2SEvent);
	xSemaphoreTake(shI2SEvent, 0);

	// Init
	if(!CS43L22_Init(CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, 60, AUDIO_FREQUENCY_44K))
	{
		DBG_PRINTF("CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS));
	}

	// Creating tasks
	if(xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &xHandleTaskPlayer) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

void Player_SetState(enum player_states state)
{
	player_state = state;
}
