/*
 * decoder.h
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

#ifndef APP_DECODER_H_
#define APP_DECODER_H_

enum audio_format { WAVE = 0, MP3, FLAC };

struct decoder_if
{
	void (*init)(enum audio_format format);
	void (*deinit)(void);
	void (*decode)(void* enc_buf, void* dec_buf);
};

_Bool Decoder_InitInterface(struct decoder_if* interface);

#endif /* APP_DECODER_H_ */
