/*
 * aac.h
 *
 *  Created on: 02.11.2017
 *      Author: Kwarc
 */

#ifndef DECODER_AAC_H_
#define DECODER_AAC_H_

_Bool AAC_Init(struct audio_decoder* main_decoder);
_Bool AAC_Decode(void);
_Bool AAC_Deinit(void);

#endif /* DECODER_AAC_H_ */
