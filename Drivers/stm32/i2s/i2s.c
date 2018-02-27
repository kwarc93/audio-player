/*
 * i2s.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */
#include "i2s/i2s.h"
#include "gpio/gpio.h"

#include <stdbool.h>

/* SCK(kHz) = SAI_CK_x/(SAIClockDivider*2*256) */
#define SAIClockDivider(__FREQUENCY__) \
        (__FREQUENCY__ == AUDIO_FREQUENCY_8K)  ? 12 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_11K) ? 2 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_16K) ? 6 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_22K) ? 1 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_32K) ? 3 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_44K) ? 0 \
      : (__FREQUENCY__ == AUDIO_FREQUENCY_48K) ? 2 : 1  \

void I2S_Init( void )
{
	/* Enable SAI1 clock */
	RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;
	__DSB();
	/* Enable GPIOE clock */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN;
	__DSB();
	/* Enable DMA2 clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	__DSB();

	GPIO_PinConfig( GPIOE, 6, GPIO_AF13_PP_100MHz );	// SD
	GPIO_PinConfig( GPIOE, 5, GPIO_AF13_PP_100MHz );	// SCK
	GPIO_PinConfig( GPIOE, 4, GPIO_AF13_PP_100MHz );	// FS
	GPIO_PinConfig( GPIOE, 2, GPIO_AF13_PP_100MHz );	// MCLK

	/* Configure SAI_Block_x */
	SAI1_Block_A->CR1 &= ~SAI_xCR1_SAIEN;			// SAI disable

	SAI1_Block_A->CR1 = 0;
	SAI1_Block_A->CR1 |= SAI_xCR1_OUTDRIV | SAI_xCR1_DMAEN; 	// Output drive enable, DMA enable
	SAI1_Block_A->CR1 |= (SAIClockDivider(AUDIO_FREQUENCY_44K) << SAI_xCR1_MCKDIV_Pos);	// MCLK divider
	SAI1_Block_A->CR1 |= SAI_xCR1_DS_2 | SAI_xCR1_CKSTR;// 16Bit data size, falling clockstrobing edge
	SAI1_Block_A->CR2 = 0;
	SAI1_Block_A->CR2 |= SAI_xCR2_FFLUSH | SAI_xCR2_FTH_1;		// Flush FIFO, FIFO threshold 1/2

	/* Configure SAI_Block_x Frame */
	SAI1_Block_A->FRCR = 0;
	SAI1_Block_A->FRCR |= ((64 - 1) << SAI_xFRCR_FRL_Pos);		// Frame length: 64
	SAI1_Block_A->FRCR |= ((16 - 1) << SAI_xFRCR_FSALL_Pos);	// Frame active Length: 16
	SAI1_Block_A->FRCR |= SAI_xFRCR_FSDEF;// FS Definition: Start frame + Channel Side identification
	SAI1_Block_A->FRCR |= SAI_xFRCR_FSOFF;// FS Offset: FS asserted one bit before the first bit of slot 0

	/* Configure SAI Block_x Slot */
	SAI1_Block_A->SLOTR = 0;
	SAI1_Block_A->SLOTR |= SAI_xSLOTR_NBSLOT_1;					// Slot number: 2
	SAI1_Block_A->SLOTR |= (3 << SAI_xSLOTR_SLOTEN_Pos);		// Enable slot 0,1
	SAI1_Block_A->CR1 |= SAI_xCR1_SAIEN;						// SAI enable

	/* DMA2 Channel6 configuration - SAI1 TX */
	DMA2_CSELR->CSELR |= (1 << 20);								// Channel 6 mapped on SAI1_A

	// Data flow: mem > periph; pDataSize: 16bit; mDataSize: 16bit(increment), circular mode
	DMA2_Channel6->CCR = DMA_CCR_DIR | DMA_CCR_MINC | DMA_CCR_PSIZE_0 | DMA_CCR_MSIZE_0 |
	DMA_CCR_CIRC;

	DMA2_Channel6->CCR |= DMA_CCR_HTIE | DMA_CCR_TCIE;// Enable half-transfer & transfer complete interrupts

	// Priority level
	NVIC_SetPriority( DMA2_Channel6_IRQn, NVIC_EncodePriority( NVIC_GetPriorityGrouping(), 5, 0 ) );
	// Enable IRQ
	NVIC_EnableIRQ( DMA2_Channel6_IRQn );

}

void I2S_Deinit( void )
{
	/* Disable SAI1 clock */
	RCC->APB2ENR &= ~RCC_APB2ENR_SAI1EN;
	__DSB();
	/* Disable DMA2 clock */
	RCC->AHB1ENR &= ~RCC_AHB1ENR_DMA2EN;
	__DSB();

	/* Clear DMA control register and status flags */
	DMA2_Channel6->CCR = 0;
	DMA2->IFCR = DMA_ISR_GIF6;

	/* Disable DMA2 interrupts */
	NVIC_DisableIRQ( DMA2_Channel6_IRQn );
}

void I2S_StartDMA( void *src, uint32_t length )
{
	/* Disable DMA */
	DMA2_Channel6->CCR &= ~DMA_CCR_EN;

	/* Enable the SAI DMA requests */
	SAI1_Block_A->CR1 |= SAI_xCR1_DMAEN;

	/* DMA2 Channel 6 configuration - SAI TX */
	DMA2_Channel6->CPAR = (uint32_t) &(SAI1_Block_A->DR);
	DMA2_Channel6->CMAR = (uint32_t) src;
	DMA2_Channel6->CNDTR = length;

	/* Start transmission */
	DMA2_Channel6->CCR |= DMA_CCR_EN;
}

void I2S_StopDMA( void )
{
	// Pause the audio file playing by disabling the SAI DMA requests
	SAI1_Block_A->CR1 &= ~SAI_xCR1_DMAEN;
	// Disable DMA
	DMA2_Channel6->CCR &= ~DMA_CCR_EN;
}

void I2S_PauseDMA( void )
{
	// Pause the audio file playing by disabling the SAI DMA requests
	SAI1_Block_A->CR1 &= ~SAI_xCR1_DMAEN;
}

void I2S_ResumeDMA( void )
{
	// Resume the audio file playing by enabling the SAI DMA requests
	SAI1_Block_A->CR1 |= SAI_xCR1_DMAEN;
}

__attribute__((weak)) void I2S_HalfTransferCallback( void )
{
	/* NOTE : This function Should not be modified, when the callback is needed */
}

__attribute__((weak)) void I2S_TransferCompleteCallback( void )
{
	/* NOTE : This function Should not be modified, when the callback is needed */
}

// --- SAI & DMA INTERRUPT HANDLERS --- //

void SAI1_IRQHandler( void )
{
	// Interrupt service code:
}

void DMA2_Channel6_IRQHandler( void )
{
	/* Transfer Complete */
	if( DMA2->ISR & DMA_ISR_TCIF6 )
	{
		DMA2->IFCR |= DMA_IFCR_CTCIF6;

		// Interrupt service code:
		I2S_TransferCompleteCallback();

	}

	/* Half-Transfer Complete */
	if( DMA2->ISR & DMA_ISR_HTIF6 )
	{
		DMA2->IFCR |= DMA_IFCR_CHTIF6;

		// Interrupt service code:
		I2S_HalfTransferCallback();
	}
}

