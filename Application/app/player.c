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

#include "mp3dec.h"
#include "FatFs/ff.h"
#include "usb_host.h"
#include "ui/display.h"
#include "misc.h"

#include <stdbool.h>

#ifdef DEBUG
#include "debug.h"
#define DBG_PRINTF(...)	(Debug_Printf("[PLAYER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

#define AUDIO_BUFFER_LENGTH			(4096)
#define AUDIO_HALF_BUFFER_LENGTH	(AUDIO_BUFFER_LENGTH/2)

static SemaphoreHandle_t shI2SEvent;
static TaskHandle_t xHandleTaskPlayer;
static HMP3Decoder hMP3Decoder;

static int16_t audio_buffer[AUDIO_BUFFER_LENGTH];
static int16_t* audio_buffer_ready_part;

void I2S_HalfTransferCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	audio_buffer_ready_part = &audio_buffer[0];
	xSemaphoreGiveFromISR(shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void I2S_TransferCompleteCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	audio_buffer_ready_part = &audio_buffer[AUDIO_HALF_BUFFER_LENGTH];
	xSemaphoreGiveFromISR(shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

static void vTaskPlayer(void * pvParameters)
{
	FIL audio_file;
	FATFS fs0;
	FRESULT res;
	UINT bytes_read;
	TickType_t xLastFlashTime;

	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for(;;)
	{
		if(!USB_IsClassActive())
		{
			while(!USB_IsClassActive()) {vTaskDelay(100);};

			f_mount(&fs0, "0:", 1);
			if(f_open(&audio_file, "0:hs_wav.wav", FA_READ) == FR_OK)
			{
				DBG_PRINTF("Playing audio file: hs_wav.wav");
				Display_SendText("HAPPYSAD - BEZ ZNIECZULENIA");
			}
			I2S_TxDMA(audio_buffer, AUDIO_BUFFER_LENGTH);
			CS43L22_Play(CS43L22_I2C_ADDRESS, 0, 0);
		}

		if(xSemaphoreTake(shI2SEvent, portMAX_DELAY) == pdTRUE)
		{
			res = f_read(&audio_file, audio_buffer_ready_part, AUDIO_BUFFER_LENGTH, &bytes_read);
			if(bytes_read == 0 || res != FR_OK)
			{
				CS43L22_Stop(CS43L22_I2C_ADDRESS, CODEC_PDWN_SW);
				Display_SendText("STOP");
			}
		}
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskPlayer);
}

void Player_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Create and take binary semaphore
	vSemaphoreCreateBinary(shI2SEvent);
	xSemaphoreTake(shI2SEvent, 0);

	// Init
	if(!CS43L22_Init(CS43L22_I2C_ADDRESS, CS43L22_OUTPUT_HEADPHONE, 60, AUDIO_FREQUENCY_44K))
	{
		DBG_PRINTF("CS43L22 initialized, chip ID: %d", CS43L22_ReadID(CS43L22_I2C_ADDRESS));
	}

	hMP3Decoder = MP3InitDecoder();
	if(hMP3Decoder != NULL)
	{
		DBG_PRINTF("Helix MP3 decoder initialized");
	}

	// Creating tasks
	if(xTaskCreate(vTaskPlayer, "PLAYER", PLAYER_STACK_SIZE, NULL, uxPriority, &xHandleTaskPlayer) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
}

