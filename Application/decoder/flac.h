/*
 * flac.h
 *
 *  Created on: 01.11.2017
 *      Author: Kwarc
 */

#ifndef DECODER_FLAC_H_
#define DECODER_FLAC_H_

_Bool FLAC_Init( struct audio_decoder* main_decoder );
_Bool FLAC_Decode( void );
_Bool FLAC_Deinit( void );

#endif /* DECODER_FLAC_H_ */
