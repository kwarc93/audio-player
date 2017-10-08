/*
 * i2s.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */
#include "i2s/i2s.h"
#include "gpio/gpio.h"

/* SCK(kHz) = SAI_CK_x/(SAIClockDivider*2*256) */
#define SAIClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 12 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 1 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 3 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 2 : 1  \

void I2S_Init(void)
{

	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
	__DSB();
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	__DSB();

	SAI1_Block_A->CR1 &= ~SAI_xCR1_SAIEN;			// SAI disable

	GPIO_PinConfig(GPIOE, 6, GPIO_AF13_PP_100MHz);	// SD
	GPIO_PinConfig(GPIOE, 5, GPIO_AF13_PP_100MHz);	// SCK
	GPIO_PinConfig(GPIOE, 4, GPIO_AF13_PP_100MHz);	// FS
	GPIO_PinConfig(GPIOE, 2, GPIO_AF13_PP_100MHz);	// MCLK

	/* Configure SAI_Block_x */
	SAI1_Block_A->CR1 |= SAI_xCR1_OUTDRIV;	// Output drive enable
	SAI1_Block_A->CR2 |= SAI_xCR2_FTH_0;	// FIFO threshold
	SAI1_Block_A->CR1 |= (SAIClockDivider(AUDIO_FREQUENCY_44K) << SAI_xCR1_MCKDIV_Pos);	// MCLK divider
	SAI1_Block_A->CR1 |= SAI_xCR1_DS_2 | SAI_xCR1_CKSTR;	// 16Bit data size, falling clockstrobing edge

	/* Configure SAI_Block_x Frame */
	SAI1_Block_A->FRCR |= ((32 - 1) << SAI_xFRCR_FRL_Pos);		// Frame length: 32
	SAI1_Block_A->FRCR |= ((16 - 1) << SAI_xFRCR_FSALL_Pos);	// Frame active Length: 16
	SAI1_Block_A->FRCR |= SAI_xFRCR_FSDEF;	// FS Definition: Start frame + Channel Side identification
	SAI1_Block_A->FRCR |= SAI_xFRCR_FSOFF;	// FS Offset: FS asserted one bit before the first bit of slot 0

	/* Configure SAI Block_x Slot */
	SAI1_Block_A->SLOTR |= ((2 - 1) << SAI_xSLOTR_NBSLOT_Pos);	// Slot number: 2
	SAI1_Block_A->SLOTR |= (3 << SAI_xSLOTR_SLOTEN_Pos);		// Enable slot 0 & 1

	SAI1_Block_A->CR1 |= SAI_xCR1_SAIEN;						// SAI enable

}

