/*
 * decoder.c
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "decoder.h"
#include "wave.h"
#include "mp3.h"
#include "flac.h"
#include "aac.h"

#include "filebrowser/file_browser.h"
#include "FatFs/ff.h"
#include "i2s/i2s.h"
#include "misc.h"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[DECODER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------

static struct audio_decoder decoder;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void init(char* filename);
static void deinit(void);
static void vTaskDecoder(void * pvParameters);

// Helper functions
// ---------------------------------------------------------------------------
/* @brief 	Sets audio format for decoder
 * @param 	File extension
 * @retval	None
 */
void set_audio_format(const char* file_ext)
{
	if(!strcmp(file_ext, "wav"))
		decoder.format = WAVE;
	else if(!strcmp(file_ext, "mp3"))
		decoder.format = MP3;
	else if(!strcmp(file_ext, "flac"))
		decoder.format = FLAC;
	else if(!strcmp(file_ext, "aac") || !strcmp(file_ext, "mp4") || !strcmp(file_ext, "m4a"))
		decoder.format = AAC;
	else
		decoder.format = UNSUPPORTED;
}

// Decoding functions
// --------------------------------------------------------------------------
static _Bool decode(void)
{
	_Bool result = false;

	if(!decoder.initialized)
		return false;

	switch(decoder.format)
	{
	case WAVE:
		result = WAVE_Decode();
		break;

	case MP3:
		result = MP3_Decode();
		break;

	case FLAC:
		result = FLAC_Decode();
		break;

	case AAC:
		result = AAC_Decode();
		break;

	default:
		result = false;
		break;
	}

	return result;
}

// Decoder task
// ---------------------------------------------------------------------------
static void vTaskDecoder(void * pvParameters)
{
	f_open(&decoder.song.file, (const TCHAR*)&decoder.song.info.fname, FA_READ);

	I2S_StartDMA(decoder.buffers.out, decoder.buffers.out_size / sizeof(int16_t));
	decoder.working = true;

	_Bool result = true;

	for(;;)
	{
		// Wait for decoder to be initialized
		while(!decoder.initialized)	vTaskDelay(100);

		if(xSemaphoreTake(decoder.shI2SEvent, portMAX_DELAY) == pdTRUE)
		{
			if(!result)
				break;
			result = decode();
		}
	}


	decoder.working = false;
	I2S_StopDMA();
	f_close(&decoder.song.file);
	vSemaphoreDelete(decoder.shI2SEvent);
	vTaskDelete(decoder.xHandleTaskDecoder);
}

static _Bool init_task(void)
{
	// Create and take binary semaphore
	vSemaphoreCreateBinary(decoder.shI2SEvent);
	xSemaphoreTake(decoder.shI2SEvent, 0);

	// Creating tasks
	if(xTaskCreate(vTaskDecoder, "DECODER", DECODER_STACK_SIZE, NULL, mainFLASH_TASK_PRIORITY + 2, &decoder.xHandleTaskDecoder) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
		return true;
	}

	return false;
}

// Interface functions
// ---------------------------------------------------------------------------
static void init(char* filename)
{
	_Bool result;

	if(decoder.initialized)
		deinit();

	// Clear structure
	memset(&decoder, 0, sizeof(decoder));

	// Get file information to structure
	if(f_stat(filename, &decoder.song.info) != FR_OK)
		return;

	// Set audio format based on file extension
	set_audio_format(FB_GetFileExtension(filename));

	// Init proper decoder based on audio format
	switch(decoder.format)
	{
	case WAVE:
		result = WAVE_Init(&decoder);
		break;
	case MP3:
		result = MP3_Init(&decoder);
		break;
	case FLAC:
		result = FLAC_Init(&decoder);
		break;
	case AAC:
		result = AAC_Init(&decoder);
		break;
	default:
		DBG_PRINTF("Unsupported format!");
		result = false;
		return;
	}

	if(!result)
		return;

	if(!init_task())
		return;

	decoder.buffers.in_ptr = decoder.buffers.in;
	decoder.buffers.out_ptr = decoder.buffers.out;

	// Decoder initialized
	DBG_PRINTF("Decoding file: %s", filename);
	decoder.initialized = true;

}

static void deinit(void)
{
	// Cleanup
	if(decoder.working)
	{
		I2S_StopDMA();
		f_close(&decoder.song.file);
		vSemaphoreDelete(decoder.shI2SEvent);
		vTaskDelete(decoder.xHandleTaskDecoder);
	}

	// Deinit actual decoder
	switch(decoder.format)
	{
	case WAVE:
		WAVE_Deinit();
		break;

	case MP3:
		MP3_Deinit();
		break;

	case FLAC:
		FLAC_Deinit();
		break;

	case AAC:
		AAC_Deinit();
		break;

	default:
		return;
	}

	// Clear structure
	memset(&decoder, 0, sizeof(decoder));

	// Decoder deinitialized
	DBG_PRINTF("Decoder stopped");
	decoder.initialized = false;
}

static void pause(void)
{
	if(!decoder.initialized)
		return;

	I2S_PauseDMA();
}

static void resume(void)
{
	if(!decoder.initialized)
		return;

	I2S_ResumeDMA();
}

static _Bool is_working(void)
{
	return decoder.working;
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool Decoder_InitInterface(struct decoder_if* interface)
{
	if(!interface)
		return false;

	interface->start = init;
	interface->stop = deinit;
	interface->pause = pause;
	interface->resume = resume;
	interface->is_working = is_working;

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
void I2S_HalfTransferCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	decoder.buffers.out_ptr = &decoder.buffers.out[0];
	xSemaphoreGiveFromISR(decoder.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void I2S_TransferCompleteCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	decoder.buffers.out_ptr = &decoder.buffers.out[decoder.buffers.out_size / (sizeof(int16_t)*2)];
	xSemaphoreGiveFromISR(decoder.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
