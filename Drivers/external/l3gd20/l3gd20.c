/**
 ******************************************************************************
 * @file    l3gd20.c
 * @author  MCD Application Team
 * @version V2.0.0
 * @date    26-June-2015
 * @brief   This file provides a set of functions needed to manage the L3GD20,
 *          ST MEMS motion sensor, 3-axis digital output gyroscope.  
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "l3gd20/l3gd20.h"

#define SPI_GYRO_CS_PIN       	7                			/* PD.07 */
#define SPI_GYRO_CS_PORT   		GPIOD                       /* GPIOD */
#define SPI_GYRO_CS_LOW()       (SPI_GYRO_CS_PORT->BSRR = GPIO_BSRR_BR_7)
#define SPI_GYRO_CS_HIGH()      (SPI_GYRO_CS_PORT->BSRR = GPIO_BSRR_BS_7)

/**
 * @brief  Set Gyroscope Initialization.
 * @retval STATUS_OK if no problem during initialization
 */
uint8_t GYRO_Init( void )
{
	status_t ret = STATUS_OK;
	uint16_t ctrl = 0x0000;

	/* GYRO CS */
	GPIO_PinConfig( SPI_GYRO_CS_PORT, SPI_GYRO_CS_PIN, GPIO_OUT_PP_25MHz );
	SPI_GYRO_CS_HIGH();

	if( (L3GD20_ReadID() == I_AM_L3GD20) || (L3GD20_ReadID() == I_AM_L3GD20_TR) )
	{
		/* MEMS configuration ----------------------------------------------------*/

		/* Configure MEMS: data rate, power mode, full scale and axes */
		ctrl = (uint16_t) (L3GD20_MODE_ACTIVE | L3GD20_OUTPUT_DATARATE_1 | L3GD20_AXES_ENABLE
				| L3GD20_BANDWIDTH_1);
		ctrl |= (uint16_t) ((L3GD20_BDU_CONTINOUS | L3GD20_BLE_LSB | L3GD20_FULLSCALE_2000) << 8);
		L3GD20_Init( ctrl );

		/* Configure MEMS: high-pass filter */
		ctrl = (uint8_t) (L3GD20_HPM_NORMAL_MODE_RES | L3GD20_HPFCF_9);
		L3GD20_FilterConfig( ctrl );
		L3GD20_FilterCmd( L3GD20_HIGHPASSFILTER_ENABLE );

		ret = STATUS_OK;
	}
	else
		ret = STATUS_ERROR;

	return ret;
}
/**
 * @brief  Writes one byte to the Gyroscope.
 * @param  pBuffer: Pointer to the buffer containing the data to be written to the Gyroscope.
 * @param  WriteAddr: Gyroscope's internal address to write to.
 * @param  NumByteToWrite: Number of bytes to write.
 */
void L3GD20_IO_Write( uint8_t* pBuffer, uint8_t WriteAddr, uint16_t NumByteToWrite )
{
	/* Configure the MS bit:
	 - When 0, the address will remain unchanged in multiple read/write commands.
	 - When 1, the address will be auto incremented in multiple read/write commands.
	 */
	if( NumByteToWrite > 0x01 )
	{
		WriteAddr |= (uint8_t) MULTIPLEBYTE_CMD;
	}

	// Set SPI in full-duplex
	__SPI_DIRECTION_2LINES( SPIx );

	/* Set chip select Low at the start of the transmission */
	SPI_GYRO_CS_LOW();

	/* Send the Address of the indexed register */
	SPIx_ReadWrite( WriteAddr );

	/* Send the data that will be written into the device (MSB First) */
	while( NumByteToWrite >= 0x01 )
	{
		SPIx_ReadWrite( *pBuffer );
		NumByteToWrite--;
		pBuffer++;
	}

	/* Set chip select High at the end of the transmission */
	SPI_GYRO_CS_HIGH();
}

/**
 * @brief  Reads a block of data from the Gyroscope.
 * @param  pBuffer: Pointer to the buffer that receives the data read from the Gyroscope.
 * @param  ReadAddr: Gyroscope's internal address to read from.
 * @param  NumByteToRead: Number of bytes to read from the Gyroscope.
 */
void L3GD20_IO_Read( uint8_t* pBuffer, uint8_t ReadAddr, uint16_t NumByteToRead )
{
	if( NumByteToRead > 0x01 )
	{
		ReadAddr |= (uint8_t) (READWRITE_CMD | MULTIPLEBYTE_CMD);
	}
	else
	{
		ReadAddr |= (uint8_t) READWRITE_CMD;
	}

	// Set SPI in full-duplex
	__SPI_DIRECTION_2LINES( SPIx );
	/* Set chip select Low at the start of the transmission */
	SPI_GYRO_CS_LOW();

	/* Send the Address of the indexed register */
	SPIx_ReadWrite( ReadAddr );

	/* Receive the data that will be read from the device (MSB First) */
	while( NumByteToRead > 0x00 )
	{
		/* Send dummy byte (0x00) to generate the SPI clock to Gyroscope (Slave device) */
		*pBuffer = SPIx_ReadWrite( DUMMY_BYTE );
		NumByteToRead--;
		pBuffer++;
	}

	/* Set chip select High at the end of the transmission */
	SPI_GYRO_CS_HIGH();
}
/**
 * @brief  Set L3GD20 Initialization.
 * @param  L3GD20_InitStruct: pointer to a L3GD20_InitTypeDef structure 
 *         that contains the configuration setting for the L3GD20.
 * @retval None
 */
void L3GD20_Init( uint16_t InitStruct )
{
	uint8_t ctrl = 0x00;

	/* Write value to MEMS CTRL_REG1 register */
	ctrl = (uint8_t) InitStruct;
	L3GD20_IO_Write( &ctrl, L3GD20_CTRL_REG1_ADDR, 1 );

	/* Write value to MEMS CTRL_REG4 register */
	ctrl = (uint8_t) (InitStruct >> 8);
	L3GD20_IO_Write( &ctrl, L3GD20_CTRL_REG4_ADDR, 1 );
}

/**
 * @brief L3GD20 De-initialization
 * @param  None
 * @retval None
 */
void L3GD20_DeInit( void )
{
}

/**
 * @brief  Read ID address of L3GD20
 * @param  None
 * @retval ID name
 */
uint8_t L3GD20_ReadID( void )
{
	uint8_t tmp;

	/* Read WHO I AM register */
	L3GD20_IO_Read( &tmp, L3GD20_WHO_AM_I_ADDR, 1 );

	/* Return the ID */
	return (uint8_t) tmp;
}

/**
 * @brief  Reboot memory content of L3GD20
 * @param  None
 * @retval None
 */
void L3GD20_RebootCmd( void )
{
	uint8_t tmpreg;

	/* Read CTRL_REG5 register */
	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG5_ADDR, 1 );

	/* Enable or Disable the reboot memory */
	tmpreg |= L3GD20_BOOT_REBOOTMEMORY;

	/* Write value to MEMS CTRL_REG5 register */
	L3GD20_IO_Write( &tmpreg, L3GD20_CTRL_REG5_ADDR, 1 );
}

/**
 * @brief Set L3GD20 in low-power mode
 * @param 
 * @retval  None
 */
void L3GD20_LowPower( uint16_t InitStruct )
{
	uint8_t ctrl = 0x00;

	/* Write value to MEMS CTRL_REG1 register */
	ctrl = (uint8_t) InitStruct;
	L3GD20_IO_Write( &ctrl, L3GD20_CTRL_REG1_ADDR, 1 );
}

/**
 * @brief  Set L3GD20 Interrupt INT1 configuration
 * @param  Int1Config: the configuration setting for the L3GD20 Interrupt.
 * @retval None
 */
void L3GD20_INT1InterruptConfig( uint16_t Int1Config )
{
	uint8_t ctrl_cfr = 0x00, ctrl3 = 0x00;

	/* Read INT1_CFG register */
	L3GD20_IO_Read( &ctrl_cfr, L3GD20_INT1_CFG_ADDR, 1 );

	/* Read CTRL_REG3 register */
	L3GD20_IO_Read( &ctrl3, L3GD20_CTRL_REG3_ADDR, 1 );

	ctrl_cfr &= 0x80;
	ctrl_cfr |= ((uint8_t) Int1Config >> 8);

	ctrl3 &= 0xDF;
	ctrl3 |= ((uint8_t) Int1Config);

	/* Write value to MEMS INT1_CFG register */
	L3GD20_IO_Write( &ctrl_cfr, L3GD20_INT1_CFG_ADDR, 1 );

	/* Write value to MEMS CTRL_REG3 register */
	L3GD20_IO_Write( &ctrl3, L3GD20_CTRL_REG3_ADDR, 1 );
}

/**
 * @brief  Enable INT1 or INT2 interrupt
 * @param  IntSel: choice of INT1 or INT2 
 *      This parameter can be: 
 *        @arg L3GD20_INT1
 *        @arg L3GD20_INT2   
 * @retval None
 */
void L3GD20_EnableIT( uint8_t IntSel )
{
	uint8_t tmpreg;

	/* Read CTRL_REG3 register */
	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG3_ADDR, 1 );

	if( IntSel == L3GD20_INT1 )
	{
		tmpreg &= 0x7F;
		tmpreg |= L3GD20_INT1INTERRUPT_ENABLE;
	}
	else if( IntSel == L3GD20_INT2 )
	{
		tmpreg &= 0xF7;
		tmpreg |= L3GD20_INT2INTERRUPT_ENABLE;
	}

	/* Write value to MEMS CTRL_REG3 register */
	L3GD20_IO_Write( &tmpreg, L3GD20_CTRL_REG3_ADDR, 1 );
}

/**
 * @brief  Disable  INT1 or INT2 interrupt
 * @param  IntSel: choice of INT1 or INT2 
 *      This parameter can be: 
 *        @arg L3GD20_INT1
 *        @arg L3GD20_INT2   
 * @retval None
 */
void L3GD20_DisableIT( uint8_t IntSel )
{
	uint8_t tmpreg;

	/* Read CTRL_REG3 register */
	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG3_ADDR, 1 );

	if( IntSel == L3GD20_INT1 )
	{
		tmpreg &= 0x7F;
		tmpreg |= L3GD20_INT1INTERRUPT_DISABLE;
	}
	else if( IntSel == L3GD20_INT2 )
	{
		tmpreg &= 0xF7;
		tmpreg |= L3GD20_INT2INTERRUPT_DISABLE;
	}

	/* Write value to MEMS CTRL_REG3 register */
	L3GD20_IO_Write( &tmpreg, L3GD20_CTRL_REG3_ADDR, 1 );
}

/**
 * @brief  Set High Pass Filter Modality
 * @param  FilterStruct: contains the configuration setting for the L3GD20.        
 * @retval None
 */
void L3GD20_FilterConfig( uint8_t FilterStruct )
{
	uint8_t tmpreg;

	/* Read CTRL_REG2 register */
	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG2_ADDR, 1 );

	tmpreg &= 0xC0;

	/* Configure MEMS: mode and cutoff frequency */
	tmpreg |= FilterStruct;

	/* Write value to MEMS CTRL_REG2 register */
	L3GD20_IO_Write( &tmpreg, L3GD20_CTRL_REG2_ADDR, 1 );
}

/**
 * @brief  Enable or Disable High Pass Filter
 * @param  HighPassFilterState: new state of the High Pass Filter feature.
 *      This parameter can be: 
 *         @arg: L3GD20_HIGHPASSFILTER_DISABLE 
 *         @arg: L3GD20_HIGHPASSFILTER_ENABLE          
 * @retval None
 */
void L3GD20_FilterCmd( uint8_t HighPassFilterState )
{
	uint8_t tmpreg;

	/* Read CTRL_REG5 register */
	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG5_ADDR, 1 );

	tmpreg &= 0xEF;

	tmpreg |= HighPassFilterState;

	/* Write value to MEMS CTRL_REG5 register */
	L3GD20_IO_Write( &tmpreg, L3GD20_CTRL_REG5_ADDR, 1 );
}

/**
 * @brief  Get status for L3GD20 data
 * @param  None         
 * @retval Data status in a L3GD20 Data
 */
uint8_t L3GD20_GetDataStatus( void )
{
	uint8_t tmpreg;

	/* Read STATUS_REG register */
	L3GD20_IO_Read( &tmpreg, L3GD20_STATUS_REG_ADDR, 1 );

	return tmpreg;
}

/**
 * @brief  Calculate the L3GD20 angular data [deg/s].
 * @param  pfData: Data out pointer
 * @retval None
 */
void L3GD20_ReadXYZRawData( int16_t *data )
{
	uint8_t tmpbuffer[6] =
	{ 0 };
	uint8_t tmpreg = 0;
	uint8_t i;

	L3GD20_IO_Read( &tmpreg, L3GD20_CTRL_REG4_ADDR, 1 );

	L3GD20_IO_Read( tmpbuffer, L3GD20_OUT_X_L_ADDR, 6 );

	/* check in the control register 4 the data alignment (Big Endian or Little Endian)*/
	if( !(tmpreg & L3GD20_BLE_MSB) )
	{
		for( i = 0; i < 3; i++ )
		{
			data[i] = (int16_t) (((uint16_t) tmpbuffer[2 * i + 1] << 8) | tmpbuffer[2 * i]);
		}
	}
	else
	{
		for( i = 0; i < 3; i++ )
		{
			data[i] = (int16_t) (((uint16_t) tmpbuffer[2 * i] << 8) | tmpbuffer[2 * i + 1]);
		}
	}

}

/**
 * @brief  Calculate the L3GD20 offset. (must be called from loop)
 * @param  pfOffset: Offset out pointer
 * @retval true if offset is ready
 */
_Bool L3GD20_CalcOffset( int32_t *pOffset, const uint8_t avg_n )
{
	int16_t data[3];
	_Bool status = false;
	static uint8_t cnt = 0;

	L3GD20_ReadXYZRawData( data );

	pOffset[0] += data[0];
	pOffset[1] += data[1];
	pOffset[2] += data[2];
	cnt++;

	if( cnt == avg_n )
	{
		pOffset[0] /= (int16_t) avg_n;
		pOffset[1] /= (int16_t) avg_n;
		pOffset[2] /= (int16_t) avg_n;

		status = true;
	}

	return status;
}

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
