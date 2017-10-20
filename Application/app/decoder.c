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

#include "mp3dec.h"

#include <stddef.h>
#include <string.h>
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
static struct decoder_context
{
	_Bool initialized;
	enum audio_format format;
	HMP3Decoder MP3Decoder;
} ctx;
// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void decode_wave(void* enc_buf, void* dec_buf)
{
	enc_buf = dec_buf;
}

static void decode_mp3(void* enc_buf, void* dec_buf)
{

}

static void decode_flac(void* enc_buf, void* dec_buf)
{

}

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void Decoder_Init(enum audio_format format)
{
	if(ctx.initialized)
	{
		Decoder_Deinit();
	}

	memset(&ctx, 0, sizeof(ctx));

	switch(format)
	{
	case WAVE:
		ctx.format = format;
		break;

	case MP3:
		ctx.format = format;
		ctx.MP3Decoder = MP3InitDecoder();
		if(ctx.MP3Decoder == NULL)
		{
			// @ TODO: handle error
		}
		break;

	case FLAC:
		ctx.format = format;
		break;

	default:
		ctx.initialized = false;
		return;
	}

	ctx.initialized = true;
}

void Decoder_Deinit(void)
{
	if(!ctx.initialized)
		return;

	switch(ctx.format)
	{
	case WAVE:
		break;

	case MP3:
		MP3FreeDecoder(ctx.MP3Decoder);
		break;

	case FLAC:
		break;

	default:
		return;
	}

	memset(&ctx, 0, sizeof(ctx));
	ctx.initialized = false;
}
void Decoder_DecodeAudio(void* enc_buf, void* dec_buf)
{
	if(!ctx.initialized)
		return;
	if(!enc_buf || !dec_buf)
		return;

	switch(ctx.format)
	{
	case WAVE:
		decode_wave(enc_buf, dec_buf);
		break;

	case MP3:
		decode_mp3(enc_buf, dec_buf);
		break;

	case FLAC:
		decode_flac(enc_buf, dec_buf);
		break;

	default:
		break;
	}
}
// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
