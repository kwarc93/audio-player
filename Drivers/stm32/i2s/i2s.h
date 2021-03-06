/*
 * i2s.h
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */

#ifndef STM32_I2S_I2S_H_
#define STM32_I2S_I2S_H_

#include "main.h"

/* AUDIO FREQUENCY */
#define AUDIO_FREQUENCY_192K          ((uint32_t)192000)
#define AUDIO_FREQUENCY_96K           ((uint32_t)96000)
#define AUDIO_FREQUENCY_48K           ((uint32_t)48000)
#define AUDIO_FREQUENCY_44K           ((uint32_t)44100)
#define AUDIO_FREQUENCY_32K           ((uint32_t)32000)
#define AUDIO_FREQUENCY_22K           ((uint32_t)22050)
#define AUDIO_FREQUENCY_16K           ((uint32_t)16000)
#define AUDIO_FREQUENCY_11K           ((uint32_t)11025)
#define AUDIO_FREQUENCY_8K            ((uint32_t)8000)

void I2S_Init( void );
void I2S_Deinit( void );
void I2S_StartDMA( void *src, uint32_t length );
void I2S_StopDMA( void );
void I2S_PauseDMA( void );
void I2S_ResumeDMA( void );

void I2S_HalfTransferCallback( void );
void I2S_TransferCompleteCallback( void );

#endif /* STM32_I2S_I2S_H_ */
