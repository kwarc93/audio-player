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
#include "FatFs/ff.h"
#include "misc.h"

#include <stdbool.h>
#include <stdlib.h>
// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[FLAC] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static FLAC__StreamDecoder* FLACDecoder;
static FLAC__StreamDecoderInitStatus FLACInitStatus;
static FLAC__bool FLACStatus;
static struct audio_decoder* decoder;

static FLAC__uint16 block_size = 0;
static FLAC__uint64 total_samples = 0;
static unsigned sample_rate = 0;
static unsigned channels = 0;
static unsigned bps = 0;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------

/**
 * Read data callback. Called when decoder needs more input data.
 *
 * @param decoder       Decoder instance
 * @param buffer        Buffer to store read data in
 * @param bytes         Pointer to size of buffer
 * @param client_data   Client data set at initilisation
 *
 * @return Read status
 */
static FLAC__StreamDecoderReadStatus read_callback(const FLAC__StreamDecoder* decoder, FLAC__byte buffer[], size_t* bytes, void* client_data)
{
    FIL* file = (FIL*)&((struct audio_decoder*)client_data)->song.file;
    UINT bytes_to_read, bytes_read;
    FRESULT result;

    if (*bytes > 0)
    {
        // read data directly into buffer
        bytes_to_read = *bytes;
        result = f_read(file, buffer, bytes_to_read, &bytes_read);
        if (f_error(file) || result != FR_OK) {
            // read error -> abort
            return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
        }
        else if (bytes_read == 0) {
            // EOF
            return FLAC__STREAM_DECODER_READ_STATUS_END_OF_STREAM;
        }
        else {
            // OK, continue decoding
            return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
        }
    }
    else {
        // decoder called but didn't want ay bytes -> abort
        return FLAC__STREAM_DECODER_READ_STATUS_ABORT;
    }

}

/**
 * Write callback. Called when decoder has decoded a single audio frame.
 *
 * @param decoder       Decoder instance
 * @param frame         Decoded frame
 * @param buffer        Array of pointers to decoded channels of data
 * @param client_data   Client data set at initilisation
 *
 * @return Read status
 */
static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder* decoder, const FLAC__Frame* frame,
    const FLAC__int32* const buffer[], void* client_data)
{
    int16_t* out_buf = ((struct audio_decoder*)client_data)->buffers.out_ptr;
    uint32_t out_buf_size = ((struct audio_decoder*)client_data)->buffers.out_size;

	size_t i;

	(void)decoder;

	if(total_samples == 0) {
		// ERROR: this example only works for FLAC files that have a total_samples count in STREAMINFO
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}
	if(channels != 2 || bps != 16) {
		// ERROR: this example only supports 16bit stereo streams
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}
	/* write decoded PCM samples */
	if(frame->header.blocksize > out_buf_size / (2 * sizeof(*out_buf)))
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	for(i = 0; i < frame->header.blocksize; i++) {
		*(out_buf) = (FLAC__int16)buffer[0][i];		/* left channel */
		*(out_buf + 1) = (FLAC__int16)buffer[1][i];	/* right channel */
		out_buf += 2;
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

/**
 * Seek callback. Called when decoder needs to seek the stream.
 *
 * @param decoder               Decoder instance
 * @param absolute_byte_offset  Offset from beginning of stream to seek to
 * @param client_data           Client data set at initilisation
 *
 * @return Seek status
 */
static FLAC__StreamDecoderSeekStatus seek_callback(const FLAC__StreamDecoder* decoder, FLAC__uint64 absolute_byte_offset, void* client_data)
{
    FIL *file = (FIL*)&((struct audio_decoder*)client_data)->song.file;

    if (f_lseek(file, (FSIZE_t)absolute_byte_offset) < 0) {
        // seek failed
        return FLAC__STREAM_DECODER_SEEK_STATUS_ERROR;
    }
    else {
        return FLAC__STREAM_DECODER_SEEK_STATUS_OK;
    }

}

/**
 * Tell callback. Called when decoder wants to know current position of stream.
 *
 * @param decoder               Decoder instance
 * @param absolute_byte_offset  Offset from beginning of stream to seek to
 * @param client_data           Client data set at initilisation
 *
 * @return Tell status
 */
static FLAC__StreamDecoderTellStatus tell_callback(const FLAC__StreamDecoder* decoder, FLAC__uint64* absolute_byte_offset, void* client_data)
{
	FIL *file = (FIL*)&((struct audio_decoder*)client_data)->song.file;
	FSIZE_t pos;

	if ((pos = f_tell(file)) < 0) {
		// seek failed
		return FLAC__STREAM_DECODER_TELL_STATUS_ERROR;
	}
	else {
		// update offset
		*absolute_byte_offset = (FLAC__uint64)pos;
		return FLAC__STREAM_DECODER_TELL_STATUS_OK;
	}

}

/**
 * Length callback. Called when decoder wants total length of stream.
 *
 * @param decoder        Decoder instance
 * @param stream_length  Total length of stream in bytes
 * @param client_data    Client data set at initilisation
 *
 * @return Length status
 */
static FLAC__StreamDecoderLengthStatus length_callback(const FLAC__StreamDecoder* decoder, FLAC__uint64* stream_length, void* client_data)
{
	FILINFO *filestats = (FILINFO*)&((struct audio_decoder*)client_data)->song.info;

	// pass on length
	*stream_length = (FLAC__uint64)filestats->fsize;
	return FLAC__STREAM_DECODER_LENGTH_STATUS_OK;
}

/**
 * EOF callback. Called when decoder wants to know if end of stream is reached.
 *
 * @param decoder       Decoder instance
 * @param client_data   Client data set at initilisation
 *
 * @return True if end of stream
 */
static FLAC__bool eof_callback(const FLAC__StreamDecoder* decoder, void* client_data)
{
    FIL *file = (FIL*)&((struct audio_decoder*)client_data)->song.file;
    return f_eof(file)? true : false;
}

/**
 * Metadata callback. Called when decoder has decoded metadata.
 *
 * @param decoder       Decoder instance
 * @param metadata      Decoded metadata block
 * @param client_data   Client data set at initilisation
 */
static void metadata_callback(const FLAC__StreamDecoder* decoder, const FLAC__StreamMetadata* metadata, void* client_data)
{

	(void)decoder, (void)client_data;

	/* print some stats */
	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
		/* save for later */
		block_size = metadata->data.stream_info.max_blocksize;
		total_samples = metadata->data.stream_info.total_samples;
		sample_rate = metadata->data.stream_info.sample_rate;
		channels = metadata->data.stream_info.channels;
		bps = metadata->data.stream_info.bits_per_sample;

		DBG_PRINTF("block size     : %u", block_size);
		DBG_PRINTF("sample rate    : %u Hz", sample_rate);
		DBG_PRINTF("channels       : %u", channels);
		DBG_PRINTF("bits per sample: %u", bps);
	}
}

/**
 * Error callback. Called when error occured during decoding.
 *
 * @param decoder       Decoder instance
 * @param status        Error
 * @param client_data   Client data set at initilisation
 */
static void error_callback(const FLAC__StreamDecoder* decoder, FLAC__StreamDecoderErrorStatus status, void* client_data)
{
	(void)decoder, (void)client_data;

	DBG_PRINTF("Got error callback: %s", FLAC__StreamDecoderErrorStatusString[status]);
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool FLAC_Init(struct audio_decoder* main_decoder)
{
	decoder = main_decoder;
	FLACDecoder = FLAC__stream_decoder_new();

	if(!FLACDecoder)
		return false;

	FLACInitStatus = FLAC__stream_decoder_init_stream(
		FLACDecoder,
        read_callback,
        seek_callback,
        tell_callback,
        length_callback,
        eof_callback,
        write_callback,
        metadata_callback,
        error_callback,
        decoder);

	if (FLACInitStatus != FLAC__STREAM_DECODER_INIT_STATUS_OK)
		return false;

	FLAC__stream_decoder_set_md5_checking(FLACDecoder, false);

	// Find metadata to know blocksize of FLAC file and then allocate proper output buffer size
	if(f_open(&decoder->song.file, decoder->song.info.fname, FA_READ) == FR_OK)
	{
		FLAC__stream_decoder_process_until_end_of_metadata(FLACDecoder);
		FLAC__stream_decoder_flush(FLACDecoder);
		f_close(&decoder->song.file);
	}
	else
		return false;

	// Allocate audio buffers
	decoder->buffers.out_size = 4 * block_size * sizeof(*decoder->buffers.out);

	decoder->buffers.out = malloc(decoder->buffers.out_size);

	if(!decoder->buffers.out)
		return false;

	return true;
}

_Bool FLAC_Decode(void)
{
	FLAC__bool result;
	FLAC__StreamDecoderState state;

	result = FLAC__stream_decoder_process_single(FLACDecoder);
	state = FLAC__stream_decoder_get_state(FLACDecoder);

	if(!result)
		return false;

	switch(state)
	{
	case FLAC__STREAM_DECODER_ABORTED:
		FLAC__stream_decoder_flush(FLACDecoder);
		break;
	case FLAC__STREAM_DECODER_END_OF_STREAM:
	case FLAC__STREAM_DECODER_OGG_ERROR:
	case FLAC__STREAM_DECODER_SEEK_ERROR:
	case FLAC__STREAM_DECODER_MEMORY_ALLOCATION_ERROR:
		return false;
	default:
		break;
	}

	return true;
}

_Bool FLAC_Deinit(void)
{
	DBG_PRINTF("Exited with status: %s", FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(FLACDecoder)]);

	FLAC__stream_decoder_finish(FLACDecoder);
	FLAC__stream_decoder_delete(FLACDecoder);
	free(decoder->buffers.out);

	return true;
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
