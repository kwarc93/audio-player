/*
 * decoder.h
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

#ifndef APP_DECODER_H_
#define APP_DECODER_H_

#define DECODER_STACK_SIZE	(2*1024)

enum audio_format { UNSUPPORTED = 0, WAVE, MP3, FLAC };

struct decoder_if
{
	void (*start)(char* filename);
	void (*stop)(void);
	_Bool (*status)(void);
};

_Bool Decoder_InitInterface(struct decoder_if* interface);

#endif /* APP_DECODER_H_ */
