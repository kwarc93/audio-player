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
#include "main.h"
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
#define DECODER_HEADER_ID			0x52494646
// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static struct audio_decoder* decoder;

struct riff_header
{
  uint32_t chunk_id;
  uint32_t chunk_size;
  uint32_t format;
  uint32_t subchunk1_id;
  uint32_t subchunk1_size;
  uint16_t audio_format;
  uint16_t num_channels;
  uint32_t sample_rate;
  uint32_t byte_rate;
  uint16_t block_align;
  uint16_t bits_per_sample;
  uint32_t subchunk2_id;
  uint32_t subchunk2_size;
}__attribute__((packed));

static struct riff_header header;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
// Reverse byte order in big-endian fields
static void format_header(void)
{
  header.chunk_id 		= __REV(header.chunk_id);
  header.format 		= __REV(header.format);
  header.subchunk1_id 		= __REV(header.subchunk1_id);
  header.subchunk2_id 		= __REV(header.subchunk2_id);
}
// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
_Bool WAVE_Init(struct audio_decoder* main_decoder)
{
  UINT bytes_read;

  decoder = main_decoder;

  // Find, read and skip RIFF header
  uint32_t chunk;
  for(uint8_t bytes = 0; bytes < sizeof(header); bytes+=sizeof(chunk))
    {
      decoder->song.result = f_read(&decoder->song.file, &chunk, sizeof(chunk), &bytes_read);
      if(decoder->song.result != FR_OK)
	return false;

      if(chunk == __REV(DECODER_HEADER_ID))
	{
	  header.chunk_id = chunk;
	  decoder->song.result = f_read(&decoder->song.file, &header.chunk_size, sizeof(header) - sizeof(chunk), &bytes_read);
	  format_header();
	  break;
	}
    }

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
