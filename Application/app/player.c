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

struct audio_file
{
	FIL file;
	FILINFO info;
	FRESULT result;
	UINT bytes_read;

	int16_t buffer[AUDIO_BUFFER_LENGTH];
	int16_t* buffer_ready_part;

	struct decoder_if decoder;
};

static struct player_context
{
	enum player_states player_state;

	SemaphoreHandle_t shI2SEvent;
	TaskHandle_t xHandleTaskPlayer;
	QueueHandle_t qhPlayerState;

	struct audio_file song;
} ctx;

void I2S_HalfTransferCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	ctx.song.buffer_ready_part = &ctx.song.buffer[0];
	xSemaphoreGiveFromISR(ctx.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void I2S_TransferCompleteCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	ctx.song.buffer_ready_part = &ctx.song.buffer[AUDIO_BUFFER_LENGTH/2];
	xSemaphoreGiveFromISR(ctx.shI2SEvent, &xHigherPriorityTaskWoken);
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
			ctx.player_state = PLAYER_INIT;
		}
		vTaskDelay(100);
		break;
	case PLAYER_INIT:
		memset(ctx.song.buffer, 0, sizeof(ctx.song.buffer));
		ctx.song.decoder.init(WAVE);
		if(f_open(&ctx.song.file, "hss_wav.wav", FA_READ) == FR_OK)
		{
			DBG_PRINTF("File: hs_wav.wav");
			Display_SendText("HAPPYSAD - BEZ ZNIECZULENIA");

			I2S_TxDMA(ctx.song.buffer, AUDIO_BUFFER_LENGTH);
			CS43L22_Play(CS43L22_I2C_ADDRESS, 0, 0);
			CS43L22_SetVolume(CS43L22_I2C_ADDRESS, 60);

			ctx.player_state = PLAYER_PLAY;
		}
		break;
	case PLAYER_PLAY:
		if(xSemaphoreTake(ctx.shI2SEvent, portMAX_DELAY) == pdTRUE)
		{
			ctx.song.result = f_read(&ctx.song.file, ctx.song.buffer_ready_part,
						sizeof(ctx.song.buffer)/2, &ctx.song.bytes_read);

			ctx.song.decoder.decode(ctx.song.buffer_ready_part, ctx.song.buffer_ready_part);

			if(ctx.song.bytes_read != sizeof(ctx.song.buffer)/2 || ctx.song.result != FR_OK)
			{
				ctx.player_state = PLAYER_STOP;
			}
		}
		break;
	case PLAYER_PAUSE:
		I2S_StopDMA();
		CS43L22_Pause(CS43L22_I2C_ADDRESS);
		Display_SendText("STOP");
		ctx.player_state = PLAYER_IDLE;
		break;
	case PLAYER_STOP:
		I2S_StopDMA();
		CS43L22_Stop(CS43L22_I2C_ADDRESS, CODEC_PDWN_SW);
		Display_SendText("STOP");
		f_close(&ctx.song.file);
		ctx.player_state = PLAYER_IDLE;
		break;
	default:
		ctx.player_state = PLAYER_IDLE;
		break;
	}
}
static void vTaskPlayer(void * pvParameters)
{
	// Task's infinite loop
	for(;;)
	{
		Player_TaskProcess(ctx.player_state);
	}
	/* Should never go there */
	vTaskDelete(ctx.xHandleTaskPlayer);
}

void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	ctx.player_state = PLAYER_WAIT_FOR_DISK;

	ctx.song.buffer_ready_part = &ctx.song.buffer[0];
	Decoder_InitInterface(&ctx.song.decoder);

	 // Create queue for player states
	ctx.qhPlayerState = xQueueCreate(4, sizeof(enum player_states));

	// Create and take binary semaphore
	vSemaphoreCreateBinary(ctx.shI2SEvent);
	xSemaphoreTake(ctx.shI2SEvent, 0);

	if(!CS43L22_Init(CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, 60, AUDIO_FREQUENCY_44K))
	{
		DBG_PRINTF("CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS));
	}

	// Creating tasks
	if(xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &ctx.xHandleTaskPlayer) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

void Player_SetState(enum player_states state)
{
	ctx.player_state = state;
}
