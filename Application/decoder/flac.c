/*
 * flac.c
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */



// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "decoder.h"

#include "flac.h"
#include "stream_decoder.h"

#include <stdbool.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static FLAC__StreamDecoder* FLACDecoder;
static FLAC__StreamDecoderInitStatus FLACInitStatus;
static FLAC__bool FLACStatus;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool FLAC_Init(void)
{
	FLACDecoder = FLAC__stream_decoder_new();

	if(!FLACDecoder)
		return false;

#if TODO
	FLACInitStatus = FLAC__stream_decoder_init_stream(
		decoder.FLACDecoder,
        read_callback,
        seek_callback,
        tell_callback,
        length_callback,
        eof_callback,
        write_callback,
        metadata_callback,
        error_callback,
        NULL);
#endif

	if (FLACInitStatus != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return false;

	return true;
}

_Bool FLAC_Decode(struct audio_decoder* decoder)
{
	return true;
}

_Bool FLAC_Deinit(void)
{
	FLAC__stream_decoder_delete(FLACDecoder);
	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
