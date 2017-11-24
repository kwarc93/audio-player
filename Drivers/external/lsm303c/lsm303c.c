/**
  ******************************************************************************
  * @file    lsm303c.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    24-June-2015
  * @brief   This file provides a set of functions needed to manage the LSM303C
  *          MEMS accelerometer.
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
#include "lsm303c/lsm303c.h"

#define SPI_ACC_CS_PIN       	0                			/* PE.00 */
#define SPI_ACC_CS_PORT   		GPIOE                       /* GPIOE */
#define SPI_ACC_CS_LOW()        (SPI_ACC_CS_PORT->BSRR = GPIO_BSRR_BR_0)
#define SPI_ACC_CS_HIGH()       (SPI_ACC_CS_PORT->BSRR = GPIO_BSRR_BS_0)

#define SPI_MAG_CS_PIN       	0                			/* PC.00 */
#define SPI_MAG_CS_PORT   		GPIOC                       /* GPIOC */
#define SPI_MAG_CS_LOW()        (SPI_MAG_CS_PORT->BSRR = GPIO_BSRR_BR_0)
#define SPI_MAG_CS_HIGH()       (SPI_MAG_CS_PORT->BSRR = GPIO_BSRR_BS_0)

/**
  * @brief  Initialize the accelrometer.
  * @retval STATUS_OK or STATUS_ERROR
  */
status_t ACC_Init(void)
{
  status_t ret = STATUS_OK;
  uint16_t ctrl = 0x0000;

  /* ACC CS */
  GPIO_PinConfig(SPI_ACC_CS_PORT,SPI_ACC_CS_PIN,GPIO_OUT_PP_25MHz);
  SPI_ACC_CS_HIGH();

  if(LSM303C_AccReadID() != LSM303C_ACC_ID)
  {
    ret = STATUS_ERROR;
  }
  else
  {
    /* MEMS configuration ------------------------------------------------------*/

	  /* Configure MEMS: data rate, power mode, full scale and axes */
    ctrl = (uint16_t)(LSM303C_ACC_HR_ENABLE| LSM303C_ACC_ODR_100_HZ | LSM303C_ACC_AXES_ENABLE | LSM303C_ACC_BDU_CONTINUOUS);
    ctrl |= (uint16_t)((LSM303C_ACC_FULLSCALE_2G | LSM303C_ACC_SPI_MODE) << 8);
    LSM303C_AccInit(ctrl);

    /* Configure MEMS: mode, cutoff frequency, Filter status, Click, AOI1 and AOI2 */
    ctrl = (uint8_t) (LSM303C_ACC_HPM_NORMAL_MODE | LSM303C_ACC_DFC1_ODRDIV9);
    LSM303C_AccFilterConfig(ctrl);
  }
  return ret;
}

/**
  * @brief  Set LSM303C Accelerometer Initialization.
  * @param  InitStruct: Init parameters
  * @retval None
  */
void LSM303C_AccInit(uint16_t InitStruct)
{
  uint8_t ctrl = 0x00;

  /* Write value to ACC MEMS CTRL_REG1 register */
  ctrl = (uint8_t) InitStruct;
  ACCELERO_IO_Write(LSM303C_CTRL_REG1_A, ctrl);

  /* Write value to ACC MEMS CTRL_REG4 register */
  ctrl = ((uint8_t) (InitStruct >> 8));
  ACCELERO_IO_Write(LSM303C_CTRL_REG4_A, ctrl);
}

/**
  * @brief  LSM303C Accelerometer De-initialization.
  * @param  None
  * @retval None
  */
void LSM303C_AccDeInit(void)
{
}

/**
  * @brief  Read LSM303C ID.
  * @param  None
  * @retval ID
  */
uint8_t LSM303C_AccReadID(void)
{
  uint8_t ctrl = 0x00;

  /* Enabled SPI/I2C read communication */
  ACCELERO_IO_Write(LSM303C_CTRL_REG4_A, 0x5);

  /* Read value at Who am I register address */
  ctrl = ACCELERO_IO_Read(LSM303C_WHO_AM_I_ADDR);

  return ctrl;
}

/**
  * @brief  Put Accelerometer in power down mode.
  * @param  None
  * @retval None
  */
void LSM303C_AccLowPower(void)
{
  uint8_t ctrl = 0x00;

  /* Read control register 1 value */
  ctrl = ACCELERO_IO_Read(LSM303C_CTRL_REG1_A);

  /* Clear ODR bits */
  ctrl &= ~(LSM303C_ACC_ODR_BITPOSITION);

  /* Set Power down */
  ctrl |= LSM303C_ACC_ODR_OFF;

  /* write back control register */
  ACCELERO_IO_Write(LSM303C_CTRL_REG1_A, ctrl);
}

/**
  * @brief  Set High Pass Filter Modality
  * @param  FilterStruct: contains data for filter config
  * @retval None
  */
void LSM303C_AccFilterConfig(uint8_t FilterStruct)
{
  uint8_t tmpreg;

//  /* Read CTRL_REG2 register */
//  tmpreg = ACCELERO_IO_Read(LSM303C_CTRL_REG2_A);
//
//  tmpreg &= 0x0C;
  tmpreg = FilterStruct;

  /* Write value to ACC MEMS CTRL_REG2 register */
  ACCELERO_IO_Write(LSM303C_CTRL_REG2_A, tmpreg);
}

/**
  * @brief  Read X, Y & Z Acceleration values
  * @param  pData: Data out pointer
  * @retval None
  */
void LSM303C_AccReadXYZ(int16_t* pData)
{
  int16_t pnRawData[3];
  uint8_t ctrlx[2]={0,0};
  uint8_t buffer[6];
  uint8_t i = 0;
  uint8_t sensitivity = LSM303C_ACC_SENSITIVITY_2G;

  /* Read the acceleration control register content */
  ctrlx[0] = ACCELERO_IO_Read(LSM303C_CTRL_REG4_A);
  ctrlx[1] = ACCELERO_IO_Read(LSM303C_CTRL_REG5_A);

  /* Read output register X, Y & Z acceleration */
  buffer[0] = ACCELERO_IO_Read(LSM303C_OUT_X_L_A);
  buffer[1] = ACCELERO_IO_Read(LSM303C_OUT_X_H_A);
  buffer[2] = ACCELERO_IO_Read(LSM303C_OUT_Y_L_A);
  buffer[3] = ACCELERO_IO_Read(LSM303C_OUT_Y_H_A);
  buffer[4] = ACCELERO_IO_Read(LSM303C_OUT_Z_L_A);
  buffer[5] = ACCELERO_IO_Read(LSM303C_OUT_Z_H_A);

  for(i=0; i<3; i++)
  {
    pnRawData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) | buffer[2*i]);
  }

  /* Normal mode */
  /* Switch the sensitivity value set in the CRTL4 */
  switch(ctrlx[0] & LSM303C_ACC_FULLSCALE_8G)
  {
  case LSM303C_ACC_FULLSCALE_2G:
    sensitivity = LSM303C_ACC_SENSITIVITY_2G;
    break;
  case LSM303C_ACC_FULLSCALE_4G:
    sensitivity = LSM303C_ACC_SENSITIVITY_4G;
    break;
  case LSM303C_ACC_FULLSCALE_8G:
    sensitivity = LSM303C_ACC_SENSITIVITY_8G;
    break;
  }

  /* Obtain the mg value for the three axis */
  for(i=0; i<3; i++)
  {
    pData[i]=(pnRawData[i] * sensitivity);
  }
}
/**
* @brief  Calculate the LSM303C ACC offset. (must be called from loop)
* @param  pfOffset: Offset out pointer
* @retval true if offset is ready
*/
_Bool LSM303C_AccCalcOffset(int32_t *pOffset, const uint8_t avg_n)
{
	int16_t data[3];
	_Bool status = false;
	static uint8_t cnt = 0;


	LSM303C_AccReadXYZ(data);

	pOffset[0] += data[0];
	pOffset[1] += data[1];
	pOffset[2] += data[2];
	cnt++;

	if(cnt==avg_n)
	{
		pOffset[0] /= (int16_t)avg_n;
		pOffset[1] /= (int16_t)avg_n;
		pOffset[2] /= (int16_t)avg_n;

		status = true;
	}

	return status;
}

/**
  * @}
  */
/***********************************************************************************************
  Magnetometer driver
***********************************************************************************************/
/**
  * @brief  Initialize the magnetometer.
  * @retval STATUS_OK or STATUS_ERROR
  */
status_t MAGN_Init(void)
{
  status_t ret = STATUS_OK;

  /* MAGN CS */
  GPIO_PinConfig(SPI_MAG_CS_PORT,SPI_MAG_CS_PIN,GPIO_OUT_PP_25MHz);
  SPI_MAG_CS_HIGH();

  if(LSM303C_MagReadID() != LSM303C_MAG_ID)
  {
    ret = STATUS_ERROR;
  }
  else
  {
    /* MEMS configuration ------------------------------------------------------*/
	/* Configure the magnetometer main parameters */
    MAGNETO_IO_Write(LSM303C_CTRL_REG1_M, LSM303C_MAG_TEMPSENSOR_DISABLE | LSM303C_MAG_OM_XY_ULTRAHIGH | LSM303C_MAG_ODR_80_HZ);
    MAGNETO_IO_Write(LSM303C_CTRL_REG2_M, LSM303C_MAG_FS_DEFAULT | LSM303C_MAG_REBOOT_DEFAULT | LSM303C_MAG_SOFT_RESET_DEFAULT);
    MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, LSM303C_MAG_SPI_MODE | LSM303C_MAG_CONFIG_NORMAL_MODE | LSM303C_MAG_CONTINUOUS_MODE);
    MAGNETO_IO_Write(LSM303C_CTRL_REG4_M, LSM303C_MAG_OM_Z_ULTRAHIGH | LSM303C_MAG_BLE_LSB);
    MAGNETO_IO_Write(LSM303C_CTRL_REG5_M, LSM303C_MAG_BDU_CONTINUOUS);

  }
  return ret;
}

/**
  * @brief  LSM303C Magnetometer De-initialization.
  * @param  None
  * @retval None
  */
void LSM303C_MagDeInit(void)
{
}

/**
  * @brief  Read LSM303C ID.
  * @param  None
  * @retval ID
  */
uint8_t LSM303C_MagReadID(void)
{

  /* Enabled the SPI/I2C read operation */
  MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, 0x84);

  /* Read value at Who am I register address */
  return MAGNETO_IO_Read(LSM303C_WHO_AM_I_ADDR);
}

/**
  * @brief  Put Magnetometer in power down mode.
  * @param  None
  * @retval None
  */
void LSM303C_MagLowPower(void)
{
  uint8_t ctrl = 0x00;

  /* Read control register 1 value */
  ctrl = MAGNETO_IO_Read(LSM303C_CTRL_REG3_M);

  /* Clear ODR bits */
  ctrl &= ~(LSM303C_MAG_SELECTION_MODE);

  /* Set Power down */
  ctrl |= LSM303C_MAG_POWERDOWN2_MODE;

  /* write back control register */
  MAGNETO_IO_Write(LSM303C_CTRL_REG3_M, ctrl);
}

/**
  * @brief  Get status for Mag LSM303C data
  * @param  None
  * @retval Data status in a LSM303C Data register
  */
uint8_t LSM303C_MagGetDataStatus(void)
{
  /* Read Mag STATUS register */
  return MAGNETO_IO_Read(LSM303C_STATUS_REG_M);
}

/**
  * @brief  Read X, Y & Z Magnetometer values
  * @param  pData: Data out pointer
  * @retval None
  */
void LSM303C_MagReadXYZ(int16_t* pData)
{
  uint8_t ctrlx;
  uint8_t buffer[6];
  uint8_t i=0;

  /* Read the magnetometer control register content */
  ctrlx = MAGNETO_IO_Read(LSM303C_CTRL_REG4_M);

  /* Read output register X, Y & Z magnetometer */
  buffer[0] = MAGNETO_IO_Read(LSM303C_OUT_X_L_M);
  buffer[1] = MAGNETO_IO_Read(LSM303C_OUT_X_H_M);
  buffer[2] = MAGNETO_IO_Read(LSM303C_OUT_Y_L_M);
  buffer[3] = MAGNETO_IO_Read(LSM303C_OUT_Y_H_M);
  buffer[4] = MAGNETO_IO_Read(LSM303C_OUT_Z_L_M);
  buffer[5] = MAGNETO_IO_Read(LSM303C_OUT_Z_H_M);

  /* Check in the control register4 the data alignment*/
  if((ctrlx & LSM303C_MAG_BLE_MSB))
  {
    for(i=0; i<3; i++)
    {
      pData[i]=((int16_t)((uint16_t)buffer[2*i] << 8) | buffer[2*i+1]);
    }
  }
  else
  {
    for(i=0; i<3; i++)
    {
      pData[i]=((int16_t)((uint16_t)buffer[2*i+1] << 8) | buffer[2*i]);
    }
  }
}


/*################################### LOW LEVEL IO FUNCTIONS ########################################## */
/**
  * @}
  */
/**
  * @brief  Writes one byte to the COMPASS / ACCELEROMETER.
  * @param  RegisterAddr specifies the COMPASS / ACCELEROMETER register to be written.
  * @param  Value : Data to be written
  * @retval   None
 */
void ACCELERO_IO_Write(uint8_t RegisterAddr, uint8_t Value)
{
  SPI_ACC_CS_LOW();
  __SPI_DIRECTION_1LINE_TX(SPIx);
  /* call SPI Read data bus function */
  SPIx_Write(RegisterAddr);
  SPIx_Write(Value);
  SPI_ACC_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the COMPASS / ACCELEROMETER.
  * @param  RegisterAddr : specifies the COMPASS / ACCELEROMETER internal address register to read from
  * @retval ACCELEROMETER register value
  */
uint8_t ACCELERO_IO_Read(uint8_t RegisterAddr)
{
  RegisterAddr = RegisterAddr | ((uint8_t)0x80);
  SPI_ACC_CS_LOW();
  __SPI_DIRECTION_1LINE_TX(SPIx);
  SPIx_Write(RegisterAddr);
  __SPI_DIRECTION_1LINE_RX(SPIx);
  uint8_t val = SPIx_Read();
  SPI_ACC_CS_HIGH();
  return val;
}
/**
  * @}
  */
/**
  * @brief  Writes one byte to the COMPASS/MAGNETO.
  * @param  RegisterAddr specifies the COMPASS/MAGNETO register to be written.
  * @param  Value : Data to be written
  * @retval   None
 */
void MAGNETO_IO_Write(uint8_t RegisterAddr, uint8_t Value)
{
  SPI_MAG_CS_LOW();
  __SPI_DIRECTION_1LINE_TX(SPIx);
  /* call SPI Read data bus function */
  SPIx_Write(RegisterAddr);
  SPIx_Write(Value);
  SPI_MAG_CS_HIGH();
}

/**
  * @brief  Reads a block of data from the COMPASS/MAGNETO.
  * @param  RegisterAddr : specifies the COMPASS/MAGNETO internal address register to read from
  * @retval ACCELEROMETER register value
  */
uint8_t MAGNETO_IO_Read(uint8_t RegisterAddr)
{
  RegisterAddr = RegisterAddr | ((uint8_t)0x80);
  SPI_MAG_CS_LOW();
  __SPI_DIRECTION_1LINE_TX(SPIx);
  SPIx_Write(RegisterAddr);
  __SPI_DIRECTION_1LINE_RX(SPIx);
  uint8_t val = SPIx_Read();
  SPI_MAG_CS_HIGH();
  return val;
}
/*################################## LOW LEVEL IO FUNCTIONS END ######################################## */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
