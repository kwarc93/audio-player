/*
 * decoder.h
 *
 *  Created on: 20.10.2017
 *      Author: Kwarc
 */

#ifndef APP_DECODER_H_
#define APP_DECODER_H_

enum audio_format { WAVE = 0, MP3, FLAC };

void Decoder_Init(enum audio_format format);
void Decoder_Deinit(void);
void Decoder_DecodeAudio(void* enc_buf, void* dec_buf);

#endif /* APP_DECODER_H_ */
