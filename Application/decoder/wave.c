/*
 * wave.c
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */


// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "decoder.h"

#include "FatFs/ff.h"

#include <stdbool.h>
#include <stdlib.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[WAV] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

#define DECODER_OUT_BUFFER_LEN			(8 * 1024)
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static struct audio_decoder* decoder;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool WAVE_Init(struct audio_decoder* main_decoder)
{
	decoder = main_decoder;

	// Allocate audio buffers
	decoder->buffers.out_size = DECODER_OUT_BUFFER_LEN * sizeof(*decoder->buffers.out);

	decoder->buffers.out = malloc(decoder->buffers.out_size);

	if(!decoder->buffers.out)
		return false;

	return true;
}

_Bool WAVE_Decode(void)
{
	UINT bytes_read;

	decoder->song.result = f_read(&decoder->song.file, decoder->buffers.out_ptr,
							decoder->buffers.out_size/2, &bytes_read);

	if(bytes_read != decoder->buffers.out_size/2 || decoder->song.result != FR_OK)
	{
		return false;
	}

	return true;
}

_Bool WAVE_Deinit(void)
{
	free(decoder->buffers.out);

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
