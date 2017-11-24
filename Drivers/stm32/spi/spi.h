/*
 * spi.h
 *
 *  Created on: 10.02.2017
 *      Author: Kwarc
 */

#ifndef SPI_H_
#define SPI_H_

#include "main.h"
#include "gpio/gpio.h"

#define SPIx					SPI2

/* LL definition */
#define __SPI_DIRECTION_2LINES(__HANDLE__)   do{\
                                             CLEAR_BIT((__HANDLE__)->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);\
                                             }while(0);

#define __SPI_DIRECTION_2LINES_RXONLY(__HANDLE__)   do{\
                                                   CLEAR_BIT((__HANDLE__)->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);\
                                                   SET_BIT((__HANDLE__)->CR1, SPI_CR1_RXONLY);\
                                                   }while(0);

#define __SPI_DIRECTION_1LINE_TX(__HANDLE__) do{\
                                             CLEAR_BIT((__HANDLE__)->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);\
                                             SET_BIT((__HANDLE__)->CR1, SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);\
                                             }while(0);

#define __SPI_DIRECTION_1LINE_RX(__HANDLE__) do {\
                                             CLEAR_BIT((__HANDLE__)->CR1, SPI_CR1_RXONLY | SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE);\
                                             SET_BIT((__HANDLE__)->CR1, SPI_CR1_BIDIMODE);\
                                             } while(0);

#define __SPI_ENABLE(__HANDLE__)	SET_BIT((__HANDLE__)->CR1,SPI_CR1_SPE)
#define __SPI_DISABLE(__HANDLE__)	CLEAR_BIT((__HANDLE__)->CR1,SPI_CR1_SPE)

void	SPI_Init(void);
uint8_t SPIx_ReadWrite(uint8_t);
void SPIx_Write(uint8_t data);
uint8_t SPIx_Read(void);

#endif /* SPI_H_ */
