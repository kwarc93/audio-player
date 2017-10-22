/*
 * decoder.c
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"

#include "decoder.h"
#include "mp3dec.h"
#include "file_browser.h"
#include "FatFs/ff.h"
#include "i2s/i2s.h"

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#ifdef DEBUG
#include "debug.h"
#define DBG_PRINTF(...)	(Debug_Printf("[DECODER] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

#define DECODER_MP3_FRAME_LEN			(MAX_NGRAN * MAX_NCHAN * MAX_NSAMP)

#define DECODER_OUT_BUFFER_LEN			(2 * DECODER_MP3_FRAME_LEN)
#define DECODER_IN_BUFFER_LEN			(2 * MAINBUF_SIZE + 216)
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
struct audio_file
{
	FIL ffile;
	FILINFO finfo;
	FRESULT fresult;
	UINT fbr;
};

static struct decoder_context
{
	_Bool initialized;
	enum audio_format format;

	uint8_t in_buffer[DECODER_IN_BUFFER_LEN] __attribute__((aligned(4)));
	int16_t out_buffer[DECODER_OUT_BUFFER_LEN] __attribute__((aligned(4)));
	int16_t* out_buffer_ready_part;
	uint32_t bytes_read;

	struct audio_file song;

	HMP3Decoder MP3Decoder;

	SemaphoreHandle_t shI2SEvent;
	TaskHandle_t xHandleTaskDecoder;
} decoder;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void init(char* filename);
static void deinit(void);

static _Bool decode_wave(void)
{
	decoder.song.fresult = f_read(&decoder.song.ffile, decoder.out_buffer_ready_part,
						   sizeof(decoder.out_buffer)/2, &decoder.song.fbr);

	if(decoder.song.fbr != sizeof(decoder.out_buffer)/2 || decoder.song.fresult != FR_OK)
	{
		return false;
	}

	return true;
}

static _Bool decode_mp3(void)
{
	return true;
}

static _Bool decode_flac(void)
{
	return true;
}

static _Bool decode(void)
{
	_Bool result = false;

	if(!decoder.initialized)
		return false;

	switch(decoder.format)
	{
	case WAVE:
		result = decode_wave();
		break;

	case MP3:
		result = decode_mp3();
		break;

	case FLAC:
		result = decode_flac();
		break;

	default:
		result = false;
		break;
	}

	return result;
}


static void vTaskDecoder(void * pvParameters)
{
	f_open(&decoder.song.ffile, (const TCHAR*)&decoder.song.finfo.fname, FA_READ);

	I2S_TxDMA(decoder.out_buffer, DECODER_OUT_BUFFER_LEN);

	// Task's infinite loop
	for(;;)
	{
		if(xSemaphoreTake(decoder.shI2SEvent, portMAX_DELAY) == pdTRUE)
		{
			if(!decode())
				break;
		}
	}

	f_close(&decoder.song.ffile);
	I2S_StopDMA();
	vTaskDelete(decoder.xHandleTaskDecoder);
}

static void init(char* filename)
{
	if(decoder.initialized)
	{
		deinit();
	}

	memset(&decoder, 0, sizeof(decoder));
	decoder.initialized = false;

	const char* file_ext = FB_GetFileExtension(filename);

	if(!strcmp(file_ext, "wav"))
	{
		decoder.format = WAVE;
	}
	else if(!strcmp(file_ext, "mp3"))
	{
		decoder.format = MP3;
		decoder.MP3Decoder = MP3InitDecoder();

		if(decoder.MP3Decoder)
		{
			DBG_PRINTF("MP3 decoder initialized");
		}
	}
	else if(!strcmp(file_ext, "flac"))
	{
		decoder.format = FLAC;
	}
	else
	{
		decoder.format = UNSUPPORTED;
		DBG_PRINTF("Unsupported format!");
		return;
	}

	// Get file information to structure
	decoder.song.fresult = f_stat(filename, &decoder.song.finfo);

	// Create and take binary semaphore
	vSemaphoreCreateBinary(decoder.shI2SEvent);
	xSemaphoreTake(decoder.shI2SEvent, 0);

	// Creating tasks
	if(xTaskCreate(vTaskDecoder, "DECODER", DECODER_STACK_SIZE, NULL, mainFLASH_TASK_PRIORITY + 2, &decoder.xHandleTaskDecoder) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
		decoder.initialized = true;
	}

}

static void deinit(void)
{
	if(!decoder.initialized)
		return;

	switch(decoder.format)
	{
	case WAVE:
		break;

	case MP3:
		MP3FreeDecoder(decoder.MP3Decoder);
		break;

	case FLAC:
		break;

	default:
		return;
	}

	memset(&decoder, 0, sizeof(decoder));
	decoder.initialized = false;
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

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
void I2S_HalfTransferCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	decoder.out_buffer_ready_part = &decoder.out_buffer[0];
	xSemaphoreGiveFromISR(decoder.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

void I2S_TransferCompleteCallback(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	decoder.out_buffer_ready_part = &decoder.out_buffer[DECODER_OUT_BUFFER_LEN/2];
	xSemaphoreGiveFromISR(decoder.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
