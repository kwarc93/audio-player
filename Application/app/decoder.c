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
#include "misc.h"
#include "file_browser.h"
#include "FatFs/ff.h"
#include "i2s/i2s.h"

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

#define DECODER_MP3_FRAME_LEN			(MP3_MAX_NGRAN * MP3_MAX_NCHAN * MP3_MAX_NSAMP)

#define DECODER_OUT_BUFFER_LEN			(2 * DECODER_MP3_FRAME_LEN)
#define DECODER_IN_BUFFER_LEN			(2 * MP3_MAINBUF_SIZE)
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
struct audio_file
{
	FIL file;
	FILINFO info;
	FRESULT result;
};

struct audio_buffers
{
	uint8_t* in_ptr;
	uint8_t  in[DECODER_IN_BUFFER_LEN];
	uint32_t in_bytes_left;

	int16_t* out_ptr;
	int16_t  out[DECODER_OUT_BUFFER_LEN];
	uint32_t out_bytes_left;
};

static struct decoder_context
{
	_Bool initialized;
	_Bool working;
	enum audio_format format;

	struct audio_file song;
	struct audio_buffers buffers;

	HMP3Decoder MP3Decoder;
	MP3FrameInfo MP3FrameInfo;

	SemaphoreHandle_t shI2SEvent;
	TaskHandle_t xHandleTaskDecoder;
} decoder;

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
	else
		decoder.format = UNSUPPORTED;
}

/* @brief 	Refills decoder's input buffer
 * @param 	Number of unprocessed bytes left in input buffer
 * @retval	Number of bytes filled to input buffer
 */
static uint32_t refill_inbuffer(uint32_t bytes_left)
{
	UINT bytes_read = 0;
	UINT bytes_to_read = sizeof(decoder.buffers.in) - bytes_left;

	// Move unprocessed bytes to beginning of input buffer
	decoder.buffers.in_ptr = memmove(decoder.buffers.in, decoder.buffers.in_ptr, bytes_left);

	decoder.song.result = f_read(&decoder.song.file, decoder.buffers.in + bytes_left, bytes_to_read, &bytes_read);

	if(decoder.song.result != FR_OK || !bytes_read)
		return 0;

	// Zero-pad last old bytes
	if (bytes_read < bytes_to_read)
		memset(decoder.buffers.in + bytes_left + bytes_read, 0, bytes_to_read - bytes_read);

	return bytes_read;
}

// Decoding functions
// ---------------------------------------------------------------------------
static _Bool decode_wave(void)
{
	UINT bytes_read;

	decoder.song.result = f_read(&decoder.song.file, decoder.buffers.out_ptr,
						   sizeof(decoder.buffers.out)/2, &bytes_read);

	if(bytes_read != sizeof(decoder.buffers.out)/2 || decoder.song.result != FR_OK)
	{
		return false;
	}

	return true;
}

static _Bool decode_mp3(void)
{
	int error = 0;
	int offset = 0;
	_Bool frame_decoded = true;

	do
	{
		if(decoder.buffers.in_bytes_left < 2 * MP3_MAINBUF_SIZE)
		{
			int bytes_filled;

			bytes_filled = refill_inbuffer(decoder.buffers.in_bytes_left);
			if(!bytes_filled)
			{
				decoder.buffers.in_bytes_left = 0;
				frame_decoded = false;
				break;
			}

			decoder.buffers.in_bytes_left += bytes_filled;
		}

		offset = MP3FindSyncWord(decoder.buffers.in_ptr, decoder.buffers.in_bytes_left);
		if(offset < 0)
		{
			frame_decoded = false;
			break;
		}

		decoder.buffers.in_ptr += offset;
		decoder.buffers.in_bytes_left -= offset;

		error = MP3Decode(decoder.MP3Decoder, &decoder.buffers.in_ptr, (int*)&decoder.buffers.in_bytes_left, decoder.buffers.out_ptr, 0);

		switch(error)
		{
		case ERR_MP3_NONE:
			MP3GetLastFrameInfo(decoder.MP3Decoder, &decoder.MP3FrameInfo);
			frame_decoded = true;
			break;
		case ERR_MP3_MAINDATA_UNDERFLOW:
			// Do nothing - next call to decode will provide more maindata
			break;
		case ERR_MP3_INDATA_UNDERFLOW:
		case ERR_MP3_FREE_BITRATE_SYNC:
		default:
			frame_decoded = false;
			return frame_decoded;
		}

	}
	while(!frame_decoded);

	return frame_decoded;
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
// Init functions
// ---------------------------------------------------------------------------
static _Bool init_wave(void)
{
	return true;
}

static _Bool init_mp3(void)
{
	decoder.MP3Decoder = MP3InitDecoder();
	if(!decoder.MP3Decoder)
	{
		DBG_PRINTF("MP3 decoder init ERROR");
		return false;
	}

	return true;
}

static _Bool init_flac(void)
{
	return true;
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

// Decoder task
// ---------------------------------------------------------------------------
static void vTaskDecoder(void * pvParameters)
{
	f_open(&decoder.song.file, (const TCHAR*)&decoder.song.info.fname, FA_READ);

	I2S_StartDMA(decoder.buffers.out, DECODER_OUT_BUFFER_LEN);
	decoder.working = true;

	_Bool result = true;

	for(;;)
	{
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

// Interface functions
// ---------------------------------------------------------------------------
static void init(char* filename)
{
	if(decoder.initialized)
	{
		deinit();
	}

	// Clear structure
	memset(&decoder, 0, sizeof(decoder));
	decoder.buffers.in_ptr = decoder.buffers.in;
	decoder.buffers.out_ptr = decoder.buffers.out;

	// Get file information to structure
	if(f_stat(filename, &decoder.song.info) != FR_OK)
		return;

	// Set audio format based on file extension
	set_audio_format(FB_GetFileExtension(filename));

	// Init proper decoder based on audio format
	switch(decoder.format)
	{
	case WAVE:
		init_wave();
		break;
	case MP3:
		init_mp3();
		break;
	case FLAC:
		init_flac();
		break;
	default:
		DBG_PRINTF("Unsupported format!");
		return;
	}

	// Init decoder task
	if(!init_task())
		return;

	// Decoder initialized
	DBG_PRINTF("Decoding file: %s", filename);
	decoder.initialized = true;

}

static void deinit(void)
{
	if(!decoder.initialized)
		return;

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
		break;

	case MP3:
		MP3FreeDecoder(decoder.MP3Decoder);
		break;

	case FLAC:
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

	decoder.buffers.out_ptr = &decoder.buffers.out[DECODER_OUT_BUFFER_LEN/2];
	xSemaphoreGiveFromISR(decoder.shI2SEvent, &xHigherPriorityTaskWoken);
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
