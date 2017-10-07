/*
 * i2c.h
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */

#ifndef STM32_I2C_I2C_H_
#define STM32_I2C_I2C_H_

#include "main.h"

#define I2Cx					I2C1

/* Values are correct for i2c peripheral clk = 80MHz */
#define I2Cx_SPEED_100KHZ		0x10909CEC
#define I2Cx_SPEED_400KHZ		0x00F02B86

#define I2Cx_USE_TIMEOUT		0
#define I2Cx_TIMEOUT_MS			10

void I2C_Init(void);
void I2C_Write(uint8_t address, uint8_t* data, uint32_t length);
void I2C_WriteToAddr(uint8_t address, uint8_t mem_address, uint8_t* data, uint32_t length);

#endif /* STM32_I2C_I2C_H_ */
