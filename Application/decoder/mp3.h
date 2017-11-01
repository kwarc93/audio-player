/*
 * mp3.h
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */

#ifndef DECODER_MP3_H_
#define DECODER_MP3_H_

_Bool MP3_Init(void);
_Bool MP3_Decode(struct audio_decoder* decoder);
_Bool MP3_Deinit(void);

#endif /* DECODER_MP3_H_ */
