/*
 * aac.c
 *
 *  Created on: 02.11.2017
 *      Author: Kwarc
 */



// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "decoder.h"

#include "aac.h"
#include "aacdec.h"
#include "FatFs/ff.h"

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[AAC] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

#define DECODER_AAC_FRAME_LEN			(AAC_MAX_NCHANS * AAC_MAX_NSAMPS)
#define DECODER_OUT_BUFFER_LEN			(2 * DECODER_AAC_FRAME_LEN)
#define DECODER_IN_BUFFER_LEN			(4 * AAC_MAINBUF_SIZE)
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static HAACDecoder hAACDecoder;
static AACFrameInfo hAACFrameInfo;
static struct audio_decoder* decoder;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
/* @brief 	Refills decoder's input buffer
 * @param 	Number of unprocessed bytes left in input buffer
 * @retval	Number of bytes filled to input buffer
 */
static uint32_t refill_inbuffer(uint32_t bytes_left)
{
	UINT bytes_read = 0;
	UINT bytes_to_read = decoder->buffers.in_size - bytes_left;

	// Move unprocessed bytes to beginning of input buffer
	decoder->buffers.in_ptr = memmove(decoder->buffers.in, decoder->buffers.in_ptr, bytes_left);

	decoder->song.result = f_read(&decoder->song.file, decoder->buffers.in + bytes_left, bytes_to_read, &bytes_read);

	if(decoder->song.result != FR_OK || !bytes_read)
		return 0;

	// Zero-pad last old bytes
	if (bytes_read < bytes_to_read)
		memset(decoder->buffers.in + bytes_left + bytes_read, 0, bytes_to_read - bytes_read);

	return bytes_read;
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool AAC_Init(struct audio_decoder* main_decoder)
{
	decoder = main_decoder;
	hAACDecoder = AACInitDecoder();

	if(!hAACDecoder)
		return false;

	// Allocate audio buffers
	decoder->buffers.in_size = DECODER_IN_BUFFER_LEN * sizeof(*decoder->buffers.in);
	decoder->buffers.out_size = DECODER_OUT_BUFFER_LEN * sizeof(*decoder->buffers.out);

	decoder->buffers.in = malloc(decoder->buffers.in_size);
	decoder->buffers.out = malloc(decoder->buffers.out_size);

	if(!decoder->buffers.in || !decoder->buffers.out)
		return false;

//	memset(&hAACFrameInfo, 0, sizeof(AACFrameInfo));
//	hAACFrameInfo.nChans = 2;
//	hAACFrameInfo.sampRateCore = 44100;
//	hAACFrameInfo.profile = AAC_PROFILE_LC;
//
//	if(AACSetRawBlockParams(hAACDecoder, 0, &hAACFrameInfo) < 0)
//		return false;

	return true;
}

_Bool AAC_Decode(void)
{
	int error = 0;
	int offset = 0;
	_Bool frame_decoded = false;

	do
	{
		if(decoder->buffers.in_bytes_left < 2 * AAC_MAINBUF_SIZE)
		{
			int bytes_filled;

			bytes_filled = refill_inbuffer(decoder->buffers.in_bytes_left);
			if(!bytes_filled)
			{
				decoder->buffers.in_bytes_left = 0;
				frame_decoded = false;
				break;
			}

			decoder->buffers.in_bytes_left += bytes_filled;
		}

		offset = AACFindSyncWord(decoder->buffers.in_ptr, decoder->buffers.in_bytes_left);
		if(offset < 0)
		{
			decoder->buffers.in_bytes_left = 0;
			continue;
		}

		decoder->buffers.in_ptr += offset;
		decoder->buffers.in_bytes_left -= offset;

		error = AACDecode(hAACDecoder, &decoder->buffers.in_ptr, (int*)&decoder->buffers.in_bytes_left, decoder->buffers.out_ptr);

		switch(error)
		{
		case ERR_AAC_NONE:
			AACGetLastFrameInfo(hAACDecoder, &hAACFrameInfo);
			frame_decoded = true;
			break;
		case ERR_AAC_INDATA_UNDERFLOW:
			// Do nothing - next call to decode will provide more maindata
			break;
		default:
			frame_decoded = false;
			return frame_decoded;
		}

	}
	while(!frame_decoded);

	return frame_decoded;
}

_Bool AAC_Deinit(void)
{
	AACFreeDecoder(hAACDecoder);
	free(decoder->buffers.in);
	free(decoder->buffers.out);

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
