/*
 * spi.c
 *
 *  Created on: 10.02.2017
 *      Author: Kwarc
 */
#include "spi/spi.h"
#include "gpio/gpio.h"

void SPI_Init(void)
{
	// SPI initialization

	/* Enable clock for SPI2 and GPIOs */
	RCC->APB1ENR1 |= RCC_APB1ENR1_SPI2EN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIODEN | RCC_AHB2ENR_GPIOEEN;
	__DSB();
	/* Configure SPI2 pins */
	GPIO_PinConfig(SPI_GYRO_CS_PORT,SPI_GYRO_CS_PIN,GPIO_OUT_PP_25MHz);	/* GYRO CS */
	GPIO_PinConfig(SPI_ACC_CS_PORT,SPI_ACC_CS_PIN,GPIO_OUT_PP_25MHz);		/* ACC CS */
	GPIO_PinConfig(SPI_MAG_CS_PORT,SPI_MAG_CS_PIN,GPIO_OUT_PP_25MHz);		/* MAG CS */
	GPIO_PinConfig(GPIOD, 1, GPIO_AF5_PP_25MHz); 							/* SCK */
	GPIO_PinConfig(GPIOD, 3, GPIO_AF5_PP_25MHz); 							/* MISO */
	GPIO_PinConfig(GPIOD, 4, GPIO_AF5_PP_25MHz); 							/* MOSI */

	/* Deselect: Chip Select high */
	SPI_GYRO_CS_HIGH();
	SPI_ACC_CS_HIGH();
	SPI_MAG_CS_HIGH();

	/* Setup SPI peripheral */
	SPIx->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_MSTR;		// Full-duplex master
	SPIx->CR1|= SPI_CR1_BR_0;									// fPCLK/4
	SPIx->CR2 = SPI_CR2_DS_0 | SPI_CR2_DS_1 | SPI_CR2_DS_2;		// 8-bit mode
	SPIx->CR2|= SPI_CR2_FRXTH;									// FIFO 8-bit threshold

}

/* Function for SPI full-duplex communication */
uint8_t SPIx_ReadWrite(uint8_t data)
{
	__SPI_ENABLE(SPIx);
	// Tx
	while( !(SPIx->SR & SPI_SR_TXE) );
	*(volatile uint8_t*)&SPIx->DR = data;
	// Rx
	while( !(SPIx->SR & SPI_SR_RXNE) );
	data = *(volatile uint8_t*)&SPIx->DR;
	// Wait for BSY flag reset
	while( (SPIx->SR & SPI_SR_FTLVL) );
	while( (SPIx->SR & SPI_SR_BSY) );
	__SPI_DISABLE(SPIx);

	return data;
}
/* Function for SPI half-duplex communication */
void SPIx_Write(uint8_t data)
{
	__SPI_ENABLE(SPIx);
	// Check TXE flag
	while( !(SPIx->SR & SPI_SR_TXE) );
	// Tx
	*(volatile uint8_t*)&SPIx->DR = data;
	// Wait for BSY flag reset
	while( (SPIx->SR & SPI_SR_BSY) );
	__SPI_DISABLE(SPIx);

}
/* Function for SPI half-duplex communication */
uint8_t SPIx_Read(void)
{
	uint8_t data;

	// Generate clock & receive 8-bit value
	__SPI_ENABLE(SPIx);
	__DSB();
	__DSB();
	__DSB();
	__DSB();
	__DSB();
	__DSB();
	__DSB();
	__DSB();
	__SPI_DISABLE(SPIx);

	// Rx
	while( !(SPIx->SR & SPI_SR_RXNE) );
	data = *(volatile uint8_t*)&SPIx->DR;

	// Wait for the BSY flag reset
	while( (SPIx->SR & SPI_SR_BSY) );

	return data;
}
