/*
 * spi.h
 *
 *  Created on: 10.02.2017
 *      Author: Kwarc
 */

#ifndef SPI_H_
#define SPI_H_

#include "main.h"

#define SPIx					SPI2

#define SPI_GYRO_CS_PIN       	7                			/* PD.07 */
#define SPI_GYRO_CS_PORT   		GPIOD                       /* GPIOD */

#define SPI_ACC_CS_PIN       	0                			/* PE.00 */
#define SPI_ACC_CS_PORT   		GPIOE                       /* GPIOE */

#define SPI_MAG_CS_PIN       	0                			/* PC.00 */
#define SPI_MAG_CS_PORT   		GPIOC                       /* GPIOC */

/* Chip Select macro definition */
#define SPI_GYRO_CS_LOW()       (SPI_GYRO_CS_PORT->BSRR = GPIO_BSRR_BR_7)
#define SPI_GYRO_CS_HIGH()      (SPI_GYRO_CS_PORT->BSRR = GPIO_BSRR_BS_7)

#define SPI_ACC_CS_LOW()       (SPI_ACC_CS_PORT->BSRR = GPIO_BSRR_BR_0)
#define SPI_ACC_CS_HIGH()      (SPI_ACC_CS_PORT->BSRR = GPIO_BSRR_BS_0)

#define SPI_MAG_CS_LOW()       (SPI_MAG_CS_PORT->BSRR = GPIO_BSRR_BR_0)
#define SPI_MAG_CS_HIGH()      (SPI_MAG_CS_PORT->BSRR = GPIO_BSRR_BS_0)

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
