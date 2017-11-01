/*
 * flac.h
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */

#ifndef DECODER_FLAC_H_
#define DECODER_FLAC_H_

_Bool FLAC_Init(void);
_Bool FLAC_Decode(struct audio_decoder* decoder);
_Bool FLAC_Deinit(void);


#endif /* DECODER_FLAC_H_ */
