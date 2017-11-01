/*
 * wave.h
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */

#ifndef DECODER_WAVE_H_
#define DECODER_WAVE_H_

_Bool WAVE_Init(void);
_Bool WAVE_Decode(struct audio_decoder* decoder);
_Bool WAVE_Deinit(void);

#endif /* DECODER_WAVE_H_ */
