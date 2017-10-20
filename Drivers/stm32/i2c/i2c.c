/*
 * i2c.c
 *
 *  Created on: 07.10.2017
 *      Author: Kwarc
 */
#include "gpio/gpio.h"
#include "i2c/i2c.h"

void I2C_Init(void)
{
	/* Enable I2Cx and gpio clocks */
	RCC->APB1ENR1|= RCC_APB1ENR1_I2C1EN;
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN;
	__DSB();
	// Source clock selection for I2Cx - SYSCLK
	RCC->CCIPR |= RCC_CCIPR_I2C1SEL_0;

	GPIO_PinConfig(GPIOB, 6, GPIO_AF4_OD_2MHz_PULL_UP);	// SCL
	GPIO_PinConfig(GPIOB, 7, GPIO_AF4_OD_2MHz_PULL_UP);	// SDA

	// Disable peripheral
	CLEAR_BIT(I2Cx->CR1, I2C_CR1_PE);
	// Enable analog filter and disable digital filter
	MODIFY_REG(I2Cx->CR1, I2C_CR1_ANFOFF | I2C_CR1_DNF, (0 << I2C_CR1_DNF_Pos));
	// Set speed (100kHz for 80MHz I2C PERIPH CLK)
	WRITE_REG(I2Cx->TIMINGR, I2Cx_SPEED_100KHZ);
	// Enable auto-end mode
	SET_BIT(I2Cx->CR2, I2C_CR2_AUTOEND);
}

void I2C_Write(uint8_t address, uint8_t* data, uint32_t length)
{
	// Enable peripheral
	SET_BIT(I2Cx->CR1, I2C_CR1_PE);

	/* Wait until I2C bus is free */
	while(READ_BIT(I2Cx->ISR, I2C_ISR_BUSY));

	/* (1) Initiate a Start condition to the Slave device ***********************/

	/* Master Generate Start condition for a write request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a auto stop condition generation when transmit all bytes */
	MODIFY_REG(I2Cx->CR2, I2C_CR2_SADD | I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_START |
						  I2C_CR2_STOP | I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R,
						  address | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND | I2C_CR2_START);

	/* (2) Loop until end of transfer received (STOP flag raised) ***************/
	/* Loop until STOP flag is raised  */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_STOPF) == (I2C_ISR_STOPF)))
	{
		/* (2.1) Transmit data (TXIS flag raised) *********************************/
		/* Check TXIS flag value in ISR register */
		if((READ_BIT(I2Cx->ISR, I2C_ISR_TXIS) == (I2C_ISR_TXIS)))
		{
			/* Write data in Transmit Data register.
	      TXIS flag is cleared by writing data in TXDR register */
			WRITE_REG(I2Cx->TXDR, *data++);
		}
	}

	/* (3) Clear pending flags, Data consistency are checking into Slave process */
	/* End of I2C_SlaveReceiver_MasterTransmitter Process */
	SET_BIT(I2Cx->ICR, I2C_ICR_STOPCF);

	// Disable peripheral
	CLEAR_BIT(I2Cx->CR1, I2C_CR1_PE);
	__NOP();__NOP();__NOP();
}

void I2C_WriteReg(uint8_t address, uint8_t reg_addr, uint8_t* data, uint8_t length)
{
	// Enable peripheral
	SET_BIT(I2Cx->CR1, I2C_CR1_PE);

	/* Wait until I2C bus is free */
	while(READ_BIT(I2Cx->ISR, I2C_ISR_BUSY));

	/* (1) Initiate a Start condition to the Slave device ***********************/

	/* Master Generate Start condition for a write request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a reload mode condition generation 					  */
	/*    - transmit one byte (memory address)    	 					  */
	MODIFY_REG(I2Cx->CR2, I2C_CR2_SADD | I2C_CR2_ADD10 | I2C_CR2_RD_WRN |I2C_CR2_START |
						  I2C_CR2_STOP | I2C_CR2_RELOAD |I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R,
						  address | 1 << I2C_CR2_NBYTES_Pos | I2C_CR2_RELOAD | I2C_CR2_START);

	/* (2) Request memory write */
	/* Wait until TXIS flag is set */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_TXIS) == (I2C_ISR_TXIS)));
	/* Send memory address */
	WRITE_REG(I2Cx->TXDR, reg_addr);
	/* Wait until TCR flag is set */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_TCR) == (I2C_ISR_TCR)));

	/* (3) Initiate a Start condition to the Slave device ***********************/

	/* Master Generate Start condition for a write request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a auto stop condition generation when transmit all bytes */
	MODIFY_REG(I2Cx->CR2, I2C_CR2_SADD | I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_START |
						  I2C_CR2_STOP | I2C_CR2_RELOAD |I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R,
						  address | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND | I2C_CR2_START);

	/* (4) Loop until end of transfer received (STOP flag raised) ***************/
	/* Loop until STOP flag is raised  */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_STOPF) == (I2C_ISR_STOPF)))
	{
		/* (4.1) Transmit data (TXIS flag raised) *********************************/
		/* Check TXIS flag value in ISR register */
		if((READ_BIT(I2Cx->ISR, I2C_ISR_TXIS) == (I2C_ISR_TXIS)))
		{
			/* Write data in Transmit Data register.
	      TXIS flag is cleared by writing data in TXDR register */
			WRITE_REG(I2Cx->TXDR, *data++);
		}
	}

	/* (5) Clear pending flags, Data consistency are checking into Slave process */
	/* End of I2C_SlaveReceiver_MasterTransmitter Process */
	SET_BIT(I2Cx->ICR, I2C_ICR_STOPCF);

	// Disable peripheral
	CLEAR_BIT(I2Cx->CR1, I2C_CR1_PE);
	__NOP();__NOP();__NOP();
}

void I2C_ReadReg(uint8_t address, uint8_t reg_addr, uint8_t* data, uint8_t length)
{
	// Enable peripheral
	SET_BIT(I2Cx->CR1, I2C_CR1_PE);

	/* Wait until I2C bus is free */
	while(READ_BIT(I2Cx->ISR, I2C_ISR_BUSY));

	/* (1) Initiate a Start condition to the Slave device ***********************/

	/* Master Generate Start condition for a write request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a reload mode condition generation 					  */
	/*    - read one byte (memory address)      	 					  */
	MODIFY_REG(I2Cx->CR2, I2C_CR2_SADD | I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_START | I2C_CR2_STOP | I2C_CR2_RELOAD |
			I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R,
			address | 1 << I2C_CR2_NBYTES_Pos | I2C_CR2_START);

	/* (2) Request memory write */
	/* Wait until TXIS flag is set */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_TXIS) == (I2C_ISR_TXIS)));
	/* Send memory address */
	WRITE_REG(I2Cx->TXDR, reg_addr);
	/* Wait until TC flag is set */
	while(!(READ_BIT(I2Cx->ISR, I2C_ISR_TC) == (I2C_ISR_TC)));

	/* (3) Initiate a Start condition to the Slave device ***********************/

	/* Master Generate Start condition for a read request :              */
	/*    - to the Slave with a 7-Bit SLAVE_OWN_ADDRESS                   */
	/*    - with a auto stop condition generation when read all bytes */
	MODIFY_REG(I2Cx->CR2, I2C_CR2_SADD | I2C_CR2_ADD10 | I2C_CR2_RD_WRN | I2C_CR2_START |
			I2C_CR2_STOP | I2C_CR2_RELOAD | I2C_CR2_NBYTES | I2C_CR2_AUTOEND | I2C_CR2_HEAD10R,
			address | length << I2C_CR2_NBYTES_Pos | I2C_CR2_AUTOEND | (uint32_t)(I2C_CR2_START | I2C_CR2_RD_WRN));

	/* Read all bytes */
	do
	{
		/* Wait until RXNE flag is set */
		while(!READ_BIT(I2Cx->ISR, I2C_ISR_RXNE));
		/* Read data from RXDR */
		*data++ = I2Cx->RXDR;
		length--;

	}while(length > 0);

	/* No need to Check TC flag, with AUTOEND mode the stop is automatically generated */
	/* Wait until STOPF flag is set */
	while(!READ_BIT(I2Cx->ISR, I2C_ISR_STOPF));

	/* Clear STOP Flag */
	SET_BIT(I2Cx->ICR, I2C_ICR_STOPCF);

	// Disable peripheral
	CLEAR_BIT(I2Cx->CR1, I2C_CR1_PE);
	__NOP();__NOP();__NOP();
}
