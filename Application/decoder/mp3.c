/*
 * mp3.c
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */



// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "decoder.h"

#include "mp3.h"
#include "mp3dec.h"

#include <stdbool.h>
#include <string.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static HMP3Decoder hMP3Decoder;
static MP3FrameInfo hMP3FrameInfo;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

/* @brief 	Refills decoder's input buffer
 * @param 	Number of unprocessed bytes left in input buffer
 * @retval	Number of bytes filled to input buffer
 */
static uint32_t refill_inbuffer(struct audio_decoder* decoder, uint32_t bytes_left)
{
	UINT bytes_read = 0;
	UINT bytes_to_read = sizeof(decoder->buffers.in) - bytes_left;

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
_Bool MP3_Init(void)
{
	hMP3Decoder = MP3InitDecoder();

	if(!hMP3Decoder)
		return false;

	return true;
}

_Bool MP3_Decode(struct audio_decoder* decoder)
{
	int error = 0;
	int offset = 0;
	_Bool frame_decoded = true;

	do
	{
		if(decoder->buffers.in_bytes_left < 2 * MP3_MAINBUF_SIZE)
		{
			int bytes_filled;

			bytes_filled = refill_inbuffer(decoder, decoder->buffers.in_bytes_left);
			if(!bytes_filled)
			{
				decoder->buffers.in_bytes_left = 0;
				frame_decoded = false;
				break;
			}

			decoder->buffers.in_bytes_left += bytes_filled;
		}

		offset = MP3FindSyncWord(decoder->buffers.in_ptr, decoder->buffers.in_bytes_left);
		if(offset < 0)
		{
			frame_decoded = false;
			break;
		}

		decoder->buffers.in_ptr += offset;
		decoder->buffers.in_bytes_left -= offset;

		error = MP3Decode(hMP3Decoder, &decoder->buffers.in_ptr, (int*)&decoder->buffers.in_bytes_left, decoder->buffers.out_ptr, 0);

		switch(error)
		{
		case ERR_MP3_NONE:
			MP3GetLastFrameInfo(hMP3Decoder, &hMP3FrameInfo);
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

_Bool MP3_Deinit(void)
{
	MP3FreeDecoder(hMP3Decoder);

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
