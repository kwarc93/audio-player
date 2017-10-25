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
#define DECODER_IN_BUFFER_LEN			(2 * MAINBUF_SIZE)
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
	_Bool working;
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
static void vTaskDecoder(void * pvParameters);

// Decoding functions
// ---------------------------------------------------------------------------
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
	int error;
	int offset;
	MP3FrameInfo mp3FrameInfo;
	static unsigned char* in_buffer_ptr = decoder.in_buffer;
	static int bytes_left = 0;
	_Bool frame_decoded = false;

	do
	{
	if(bytes_left < DECODER_IN_BUFFER_LEN)
	{
		// Copy left bytes to beginning of input buffer
		memmove(decoder.in_buffer, in_buffer_ptr, bytes_left);

		in_buffer_ptr = decoder.in_buffer;

		decoder.song.fresult = f_read(&decoder.song.ffile, in_buffer_ptr + bytes_left,
				sizeof(decoder.in_buffer) - bytes_left, &decoder.song.fbr);

		if(decoder.song.fbr != (sizeof(decoder.in_buffer) - bytes_left) || decoder.song.fresult != FR_OK)
		{
			// Reset all static variables
			bytes_left = 0;
			in_buffer_ptr = decoder.in_buffer;
			return false;
		}

		// zero-pad to avoid finding false sync word after last frame (from old data in readBuf)
		if (decoder.song.fbr < sizeof(decoder.in_buffer) - bytes_left)
			memset(in_buffer_ptr+bytes_left+decoder.song.fbr, 0, sizeof(decoder.in_buffer)-bytes_left-decoder.song.fbr);
		// Update bytes left value
		bytes_left += decoder.song.fbr;
	}

	offset = MP3FindSyncWord(in_buffer_ptr, bytes_left);
	if(offset == -1)
	{
		in_buffer_ptr = decoder.in_buffer;
		bytes_left = 0;
		continue;
	}
	in_buffer_ptr += offset;
	bytes_left -= offset;

	error = MP3GetNextFrameInfo(decoder.MP3Decoder, &mp3FrameInfo, in_buffer_ptr);
	if(error)
	{
		in_buffer_ptr += 1;		//header not valid, try next one
		bytes_left -= 1;
		continue;
	}

	error = MP3Decode(decoder.MP3Decoder, &in_buffer_ptr, &bytes_left, decoder.out_buffer_ready_part, 0);
	if(error == -6)
	{
		in_buffer_ptr += 1;
		bytes_left -= 1;
		continue;
	}
	if (error) {
		/* error occurred */
		switch (error) {
		case ERR_MP3_INDATA_UNDERFLOW:
			return false;
			break;
		case ERR_MP3_MAINDATA_UNDERFLOW:
			/* do nothing - next call to decode will provide more mainData */
			break;
		case ERR_MP3_FREE_BITRATE_SYNC:
		default:
			return false;
			break;
		}
	} else {
		/* no error */
		frame_decoded = true;
	}
	}
	while(!frame_decoded);

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

// Other functions
// ---------------------------------------------------------------------------
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
// Decoder task
// ---------------------------------------------------------------------------
static void vTaskDecoder(void * pvParameters)
{
	f_open(&decoder.song.ffile, (const TCHAR*)&decoder.song.finfo.fname, FA_READ);
	I2S_StartDMA(decoder.out_buffer, DECODER_OUT_BUFFER_LEN);
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
	f_close(&decoder.song.ffile);
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

	// Get file information to structure
	decoder.song.fresult = f_stat(filename, &decoder.song.finfo);

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
		f_close(&decoder.song.ffile);
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
