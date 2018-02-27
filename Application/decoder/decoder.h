/*
 * decoder.h
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

#ifndef APP_DECODER_H_
#define APP_DECODER_H_

#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"

#include "FatFs/ff.h"

#define DECODER_STACK_SIZE	TASK_STACK_BYTES(8 * 1024)

enum audio_format
{
	UNSUPPORTED = 0, WAVE, MP3, FLAC, AAC, M4A, MP4
};

struct audio_file
{
	FIL file;
	FILINFO info;
	FRESULT result;
};

struct audio_buffers
{
	uint8_t* in_ptr;
	uint8_t* in;
	uint32_t in_bytes_left;
	uint32_t in_size;

	int16_t* out_ptr;
	int16_t* out;
	uint32_t out_bytes_left;
	uint32_t out_size;
};

struct audio_decoder
{
	_Bool initialized;
	_Bool working;
	enum audio_format format;

	struct audio_file song;
	struct audio_buffers buffers;

	SemaphoreHandle_t shI2SEvent;
	TaskHandle_t xHandleTaskDecoder;
};

struct decoder_if
{
	void (*start)( char* filename );
	void (*stop)( void );
	void (*pause)( void );
	void (*resume)( void );
	_Bool (*is_working)( void );
};

_Bool Decoder_InitInterface( struct decoder_if* interface );

#endif /* APP_DECODER_H_ */
