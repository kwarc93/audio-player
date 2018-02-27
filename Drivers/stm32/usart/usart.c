/*
 * USART.c
 *
 *  Created on: 05.02.2017
 *      Author: Kwarc
 */
#include "usart/usart.h"
#include "stm32l476xx.h"
#include "gpio/gpio.h"
#include "misc.h"

void USART_Init( void )
{
	/* Enable USART2 clock */
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	/* Enable DMA1 clock */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	/* Enable GPIOD clock */
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN;
	__DSB();

	/* USART TX & RX configuration */
	GPIO_PinConfig( GPIOD, 5, GPIO_AF7_PP_2MHz ); 					// TX
	GPIO_PinConfig( GPIOD, 6, GPIO_AF7_PP_2MHz );					// RX
	USARTx->BRR = (uint32_t) (CPU_CLOCK + BAUD_RATE / 2) / BAUD_RATE;		// BaudRate: 115200 Baud
	USART2->CR3 = USART_CR3_DMAR | USART_CR3_DMAT;
	USART2->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

	/* DMA1 Channel 6 configuration - USART RX */
	DMA1_CSELR->CSELR |= (1 << 21);						// Channel 6 mapped on USART2_Rx
	DMA1_Channel6->CPAR = (uint32_t) &(USARTx->RDR);		// Source (byte)
	DMA1_Channel6->CMAR = (uint32_t) &USART_RxCB[0];		// Destination (byte)
	DMA1_Channel6->CNDTR = USART_RX_CB_SIZE;			// Number of transactions
	DMA1_Channel6->CCR = DMA_CCR_CIRC;					// Circular mode
	DMA1_Channel6->CCR |= DMA_CCR_PL_0 | DMA_CCR_PL_1;	// Priority: Very High
	DMA1_Channel6->CCR |= DMA_CCR_MINC;	// periph > mem; pDataSize: 8bit; mDataSize: 8bit(increment)
	DMA1_Channel6->CCR |= DMA_CCR_EN;					// Activate DMA1

	/* DMA1 Channel7 configuration - USART TX */
	DMA1_CSELR->CSELR |= (1 << 25);						// Channel 7 mapped to USART2_Tx
	DMA1_Channel7->CPAR = (uint32_t) &(USARTx->TDR);   	// Destination (byte)
	DMA1_Channel7->CMAR = (uint32_t) &USART_TxCB[0]; 	// Source (byte)
	DMA1_Channel7->CNDTR = USART_TX_CB_SIZE; 			// Number of transactions
	DMA1_Channel7->CCR = DMA_CCR_DIR | DMA_CCR_MINC;// mem > periph; pDataSize: 8bit; mDataSize: 8bit(increment)
	DMA1_Channel7->CCR |= DMA_CCR_TCIE;					// Enable transfer complete interrupt

	// Priority level
	NVIC_SetPriority( DMA1_Channel7_IRQn, NVIC_EncodePriority( NVIC_GetPriorityGrouping(), 5, 0 ) );
	// Enable IRQ
	NVIC_EnableIRQ( DMA1_Channel7_IRQn );

	DMA1C7_TC = true;
}

char USART_Rx( void )
{
	while( !( USARTx->ISR & USART_ISR_RXNE) );
	return USARTx->RDR;
}

void USART_Tx( char data )
{
	while( !( USARTx->ISR & USART_ISR_TXE) );
	USARTx->TDR = data;
}

void USART_TxString( char *s )
{
	register char c;
	while( (c = *s++) ) USART_Tx( c );
}

void USART_TxDMA( void *src, uint8_t length )
{
	// Wait for DMA Transfer Complete
	while( !DMA1C7_TC );
	// Wait for USART TC flag and clear it
	while( !(USARTx->ISR & USART_ISR_TC) );
	USARTx->ICR |= USART_ICR_TCCF;
	/* Disable DMA */
	DMA1_Channel7->CCR &= ~DMA_CCR_EN;

	/* DMA1 Channel 7 configuration - USART TX */
	DMA1_Channel7->CPAR = (uint32_t) &(USARTx->TDR);
	DMA1_Channel7->CMAR = (uint32_t) src;
	DMA1_Channel7->CNDTR = length;
	/* Start transmission */
	DMA1_Channel7->CCR |= DMA_CCR_EN;
}

// --- USART & DMA INTERRUPT HANDLERS --- //

void USART2_IRQHandler( void )
{
	if( USARTx->ISR & USART_ISR_RXNE )
	{
		USARTx->RQR |= USART_RQR_RXFRQ;
		// Interrupt service code:
	}
}

void DMA1_Channel7_IRQHandler( void )
{
	if( DMA1->ISR & DMA_ISR_TCIF7 )
	{
		DMA1->IFCR |= DMA_IFCR_CTCIF7;
		// Interrupt service code:
		DMA1C7_TC = true;					// Indicate that DMA is free
	}

}
