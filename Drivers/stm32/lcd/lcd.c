/**
 ******************************************************************************
 * @file    stm32l476g_discovery_glass_lcd.c
 * @author  MCD Application Team
 * @version V2.0.0
 * @date    07-April-2017
 * @brief   This file provides a set of functions needed to manage the
 *          LCD Glass driver for the STM32L476G-Discovery board.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2016 STMicroelectronics</center></h2>
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
#include "lcd/lcd.h"
#include "gpio/gpio.h"
#include "misc.h"

/** @addtogroup BSP
 * @{
 */

/** @addtogroup STM32L476G_DISCOVERY
 * @{
 */

/** @defgroup STM32L476G_DISCOVERY_GLASS_LCD STM32L476G-DISCOVERY GLASS LCD
 * @brief This file includes the LCD Glass driver for LCD Module of
 *        STM32L476G-DISCOVERY board.
 * @{
 */

/* Private constants ---------------------------------------------------------*/

/** @defgroup STM32L476G_DISCOVERY_GLASS_LCD_Private_Constants Private Constants
 * @{
 */
#define ASCII_CHAR_0                  0x30  /* 0 */
#define ASCII_CHAR_AT_SYMBOL          0x40  /* @ */
#define ASCII_CHAR_LEFT_OPEN_BRACKET  0x5B  /* [ */
#define ASCII_CHAR_APOSTROPHE         0x60  /* ` */
#define ASCII_CHAR_LEFT_OPEN_BRACE    0x7B  /* ( */
/**
 * @}
 */

/* Private variables ---------------------------------------------------------*/

/** @defgroup STM32L476G_DISCOVERY_GLASS_LCD_Private_Variables Private Variables
 * @{
 */

/* this variable can be used for accelerate the scrolling exit when push user button */
__IO uint8_t bLCDGlass_KeyPressed = 0;

/**
  @verbatim
================================================================================
                              GLASS LCD MAPPING
================================================================================
LCD allows to display informations on six 14-segment Digits and 4 bars:

  1       2       3       4       5       6
-----   -----   -----   -----   -----   -----
|\|/| o |\|/| o |\|/| o |\|/| o |\|/|   |\|/|   BAR3
-- --   -- --   -- --   -- --   -- --   -- --   BAR2
|/|\| o |/|\| o |/|\| o |/|\| o |/|\|   |/|\|   BAR1
----- * ----- * ----- * ----- * -----   -----   BAR0

LCD segment mapping:
--------------------
  -----A-----        _
  |\   |   /|   COL |_|
  F H  J  K B
  |  \ | /  |        _
  --G-- --M--   COL |_|
  |  / | \  |
  E Q  P  N C
  |/   |   \|        _
  -----D-----   DP  |_|

 An LCD character coding is based on the following matrix:
COM           0   1   2     3
SEG(n)      { E , D , P ,   N   }
SEG(n+1)    { M , C , COL , DP  }
SEG(23-n-1) { B , A , K ,   J   }
SEG(23-n)   { G , F , Q ,   H   }
with n positive odd number.

 The character 'A' for example is:
  -------------------------------
LSB   { 1 , 0 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
      { 1 , 1 , 0 , 0   }
MSB   { 1 , 1 , 0 , 0   }
      -------------------
  'A' =  F    E   0   0 hexa

  @endverbatim
 */

/* Constant table for cap characters 'A' --> 'Z' */
static const uint16_t CapLetterMap[26]=
{
		/* A      B      C      D      E      F      G      H      I  */
		0xFE00, 0x6714, 0x1D00, 0x4714, 0x9D00, 0x9C00, 0x3F00, 0xFA00, 0x0014,
		/* J      K      L      M      N      O      P      Q      R  */
		0x5300, 0x9841, 0x1900, 0x5A48, 0x5A09, 0x5F00, 0xFC00, 0x5F01, 0xFC01,
		/* S      T      U      V      W      X      Y      Z  */
		0xAF00, 0x0414, 0x5b00, 0x18C0, 0x5A81, 0x00C9, 0x0058, 0x05C0
};

/* Constant table for number '0' --> '9' */
static const uint16_t NumberMap[10]=
{
		/* 0      1      2      3      4      5      6      7      8      9  */
		0x5F00,0x4200,0xF500,0x6700,0xEa00,0xAF00,0xBF00,0x04600,0xFF00,0xEF00
};

static uint32_t Digit[4];     /* Digit frame buffer */

/* LCD BAR status: To save the bar setting after writing in LCD RAM memory */
static uint8_t LCDBar = BARLEVEL_FULL;

/**
 * @}
 */

/** @defgroup STM32L476G_DISCOVERY_LCD_Private_Functions Private Functions
 * @{
 */
static void ConvertChar(uint8_t* ch, _Bool dot, _Bool colon);
static void WriteChar(uint8_t* ch, _Bool dot, _Bool colon, LCD_Digit_t position);

/**
 * @}
 */

/** @addtogroup STM32L476G_DISCOVERY_LCD_Exported_Functions
 * @{
 */

/**
 * @brief  Initialize the LCD GLASS relative GPIO port IOs and LCD peripheral.
 * @retval None
 */
void LCD_Init(void)
{
	/* 1. GPIO INIT */

	// LCD (24 segments, 4 commons, multiplexed 1/4 duty, 1/3 bias) on DIP28 connector
	//   VLCD = PC3
	//   COM0 = PA8     COM1  = PA9      COM2  = PA10    COM3  = PB9
	//   SEG0 = PA7     SEG6  = PD11     SEG12 = PB5     SEG18 = PD8
	//   SEG1 = PC5     SEG7  = PD13     SEG13 = PC8     SEG19 = PB14
	//   SEG2 = PB1     SEG8  = PD15     SEG14 = PC6     SEG20 = PB12
	//   SEG3 = PB13    SEG9  = PC7      SEG15 = PD14    SEG21 = PB0
	//   SEG4 = PB15    SEG10 = PA15     SEG16 = PD12    SEG22 = PC4
	//   SEG5 = PD9     SEG11 = PB4      SEG17 = PD10    SEG23 = PA6

	// Enable GPIO clocks
	RCC->AHB2ENR |=  RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN |
					 RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIODEN;
	// Configure pins
	GPIO_PortConfig(GPIOA, LCD_GPIO_BANKA_PINS, GPIO_AF11_PP_2MHz);
	GPIO_PortConfig(GPIOB, LCD_GPIO_BANKB_PINS, GPIO_AF11_PP_2MHz);
	GPIO_PortConfig(GPIOC, LCD_GPIO_BANKC_PINS, GPIO_AF11_PP_2MHz);
	GPIO_PortConfig(GPIOD, LCD_GPIO_BANKD_PINS, GPIO_AF11_PP_2MHz);

	/* 2. CLOCK INIT */
	// Enable write access to Backup domain
	if ( (RCC->APB1ENR1 & RCC_APB1ENR1_PWREN) == 0)
	{
		RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN;	// Power interface clock enable
	}
	__DSB();  // Delay after an RCC peripheral clock enabling

	// Select LSE as RTC clock soucre
	if ( (PWR->CR1 & PWR_CR1_DBP) == 0)
	{
		PWR->CR1  |= PWR_CR1_DBP;				  			// Enable write access to Backup domain
		while((PWR->CR1 & PWR_CR1_DBP) == 0);  	// Wait for Backup domain Write protection disable
	}

	// Reset LSEON and LSEBYP bits before configuring the LSE
	RCC->BDCR &= ~(RCC_BDCR_LSEON | RCC_BDCR_LSEBYP);

	// RTC Clock selection can be changed only if the Backup Domain is reset
	RCC->BDCR |=  RCC_BDCR_BDRST;
	RCC->BDCR &= ~RCC_BDCR_BDRST;

	// Note from STM32L4 Reference Manual:
	// RTC/LCD Clock:  (1) LSE is in the Backup domain. (2) HSE and LSI are not.
	while((RCC->BDCR & RCC_BDCR_LSERDY) == 0)
	{  // Wait until LSE clock ready
		RCC->BDCR |= RCC_BDCR_LSEON;
	}

	// Select LSE as RTC clock source
	// BDCR = Backup Domain Control Register
	RCC->BDCR	&= ~RCC_BDCR_RTCSEL;	  // RTCSEL[1:0]: 00 = No Clock, 01 = LSE, 10 = LSI, 11 = HSE
	RCC->BDCR	|= RCC_BDCR_RTCSEL_0;   // Select LSE as RTC clock

	RCC->APB1ENR1 &= ~RCC_APB1ENR1_PWREN;	// Power interface clock disable

	// Wait for the external capacitor Cext which is connected to the VLCD pin is charged (approximately 2ms for Cext=1uF)

	// Enable LCD peripheral Clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_LCDEN;

	// Enable SYSCFG
	//	 RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

	/* 3. LCD INIT */
	LCD->CR &= ~LCD_CR_LCDEN;

	LCD->FCR = 0;
	LCD->FCR |= LCD_PRESCALER_1  | LCD_DIVIDER_31 | LCD_BLINKFREQUENCY_DIV32 |
			    LCD_DEADTIME_0 | LCD_PULSEONDURATION_4 | LCD_CONTRASTLEVEL_5;
	while(!(LCD->SR & LCD_SR_FCRSR));

	LCD->CR = 0;
	LCD->CR |= LCD_DUTY_1_4 | LCD_BIAS_1_3;

	/* Enable the peripheral */
	LCD->CR |= LCD_CR_LCDEN;

	/* Wait Until the LCD is enabled */
	while(!(LCD->SR & LCD_SR_ENS));
	/*!< Wait Until the LCD Booster is ready */
	while(!(LCD->SR & LCD_SR_RDY));

	LCD_ClearAll();


}

/**
 * @brief  Configure the LCD Blink mode and Blink frequency.
 * @param  BlinkMode: specifies the LCD blink mode.
 *   This parameter can be one of the following values:
 *     @arg LCD_BLINKMODE_OFF:           Blink disabled
 *     @arg LCD_BLINKMODE_SEG0_COM0:     Blink enabled on SEG[0], COM[0] (1 pixel)
 *     @arg LCD_BLINKMODE_SEG0_ALLCOM:   Blink enabled on SEG[0], all COM (up to 8
 *                                       pixels according to the programmed duty)
 *     @arg LCD_BLINKMODE_ALLSEG_ALLCOM: Blink enabled on all SEG and all COM
 *                                       (all pixels)
 * @param  BlinkFrequency: specifies the LCD blink frequency.
 *     @arg LCD_BLINKFREQUENCY_DIV8:    The Blink frequency = fLcd/8
 *     @arg LCD_BLINKFREQUENCY_DIV16:   The Blink frequency = fLcd/16
 *     @arg LCD_BLINKFREQUENCY_DIV32:   The Blink frequency = fLcd/32
 *     @arg LCD_BLINKFREQUENCY_DIV64:   The Blink frequency = fLcd/64
 *     @arg LCD_BLINKFREQUENCY_DIV128:  The Blink frequency = fLcd/128
 *     @arg LCD_BLINKFREQUENCY_DIV256:  The Blink frequency = fLcd/256
 *     @arg LCD_BLINKFREQUENCY_DIV512:  The Blink frequency = fLcd/512
 *     @arg LCD_BLINKFREQUENCY_DIV1024: The Blink frequency = fLcd/1024
 * @retval None
 */
void LCD_BlinkConfig(uint32_t BlinkMode, uint32_t BlinkFrequency)
{
	MODIFY_REG(LCD->FCR, (LCD_FCR_BLINKF | LCD_FCR_BLINK), ((BlinkMode) | (BlinkFrequency)));
	while(!(LCD->SR & LCD_SR_FCRSR));

}

/**
 * @brief  Configure the LCD contrast.
 * @param  Contrast: specifies the LCD contrast value.
 *   This parameter can be one of the following values:
 *     @arg LCD_CONTRASTLEVEL_0: Maximum Voltage = 2.60V
 *     @arg LCD_CONTRASTLEVEL_1: Maximum Voltage = 2.73V
 *     @arg LCD_CONTRASTLEVEL_2: Maximum Voltage = 2.86V
 *     @arg LCD_CONTRASTLEVEL_3: Maximum Voltage = 2.99V
 *     @arg LCD_CONTRASTLEVEL_4: Maximum Voltage = 3.12V
 *     @arg LCD_CONTRASTLEVEL_5: Maximum Voltage = 3.25V
 *     @arg LCD_CONTRASTLEVEL_6: Maximum Voltage = 3.38V
 *     @arg LCD_CONTRASTLEVEL_7: Maximum Voltage = 3.51V
 * @retval None
 */
void LCD_Contrast(uint32_t Contrast)
{
	MODIFY_REG(LCD->FCR, LCD_FCR_CC, (Contrast));
	while(!(LCD->SR & LCD_SR_FCRSR));

}

/**
 * @brief Display one or several bar in LCD frame buffer.
 * @param BarId: specifies the LCD GLASS Bar to display
 *     This parameter can be one of the following values:
 *     @arg BAR0: LCD GLASS Bar 0
 *     @arg BAR0: LCD GLASS Bar 1
 *     @arg BAR0: LCD GLASS Bar 2
 *     @arg BAR0: LCD GLASS Bar 3
 * @retval None
 */
void LCD_DisplayBar(LCD_BarSeg_t BarId, _Bool state)
{
	uint32_t position = 0;
	// Wait for Update Display Request Bit
	while ((LCD->SR & LCD_SR_UDR) != 0);

	// Bar 0: COM3, LCD_SEG11 -> MCU_LCD_SEG8
	// Bar 1: COM2, LCD_SEG11 -> MCU_LCD_SEG8
	// Bar 2: COM3, LCD_SEG9 -> MCU_LCD_SEG25
	// Bar 3: COM2, LCD_SEG9 -> MCU_LCD_SEG25

	/* Check which bar is selected */
	while ((BarId) >> position)
	{
		/* Check if current bar is selected */
		switch(BarId & (1 << position))
		{
		/* Bar 0 */
		case LCD_BAR_0:
			/* Set BAR0 */
			LCD->RAM[6] = state ? LCD->RAM[6] | (1U << 8) : LCD->RAM[6] & ~(1U << 8);
			break;

			/* Bar 1 */
		case LCD_BAR_1:
			/* Set BAR1 */
			LCD->RAM[4] = state ? LCD->RAM[4] | (1U << 8) : LCD->RAM[4] & ~(1U << 8);
			break;

			/* Bar 2 */
		case LCD_BAR_2:
			/* Set BAR2 */
			LCD->RAM[6] = state ? LCD->RAM[6] | (1U << 25) : LCD->RAM[6] & ~(1U << 25);
			break;

			/* Bar 3 */
		case LCD_BAR_3:
			/* Set BAR3 */
			LCD->RAM[4] = state ? LCD->RAM[4] | (1U << 25) : LCD->RAM[4] & ~(1U << 25);
			break;

		default:
			break;
		}
		position++;
	}

	/* Update the LCD display */
	LCD->SR |= LCD_SR_UDR;
}

/**
 * @brief Configure the bar level on LCD by writing bar value in LCD frame buffer.
 * @param BarLevel: specifies the LCD GLASS Battery Level.
 *     This parameter can be one of the following values:
 *     @arg BATTERYLEVEL_OFF: LCD GLASS Battery Empty
 *     @arg BATTERYLEVEL_1_4: LCD GLASS Battery 1/4 Full
 *     @arg BATTERYLEVEL_1_2: LCD GLASS Battery 1/2 Full
 *     @arg BATTERYLEVEL_3_4: LCD GLASS Battery 3/4 Full
 *     @arg BATTERYLEVEL_FULL: LCD GLASS Battery Full
 * @retval None
 */
void LCD_DisplayBarLevel(LCD_BattLevel_t BarLevel)
{
	switch (BarLevel)
	{
	/* BATTERYLEVEL_OFF */
	case BARLEVEL_OFF:
		LCD_DisplayBar(LCD_BAR_0 | LCD_BAR_1 | LCD_BAR_2 | LCD_BAR_3, false);
		LCDBar = BARLEVEL_OFF;
		break;

		/* BARLEVEL 1/4 */
	case BARLEVEL_1_4:
		LCD_DisplayBar(LCD_BAR_0, true);
		LCD_DisplayBar(LCD_BAR_1 | LCD_BAR_2 | LCD_BAR_3, false);
		LCDBar = BARLEVEL_1_4;
		break;

		/* BARLEVEL 1/2 */
	case BARLEVEL_1_2:
		LCD_DisplayBar(LCD_BAR_0 | LCD_BAR_1, true);
		LCD_DisplayBar(LCD_BAR_2 | LCD_BAR_3, false);
		LCDBar = BARLEVEL_1_2;
		break;

		/* Battery Level 3/4 */
	case BARLEVEL_3_4:
		LCD_DisplayBar(LCD_BAR_0 | LCD_BAR_1 | LCD_BAR_2, true);
		LCD_DisplayBar(LCD_BAR_3, false);
		LCDBar = BARLEVEL_3_4;
		break;

		/* BATTERYLEVEL_FULL */
	case BARLEVEL_FULL:
		LCD_DisplayBar(LCD_BAR_0 | LCD_BAR_1 | LCD_BAR_2 | LCD_BAR_3, true);
		LCDBar = BARLEVEL_FULL;
		break;

	default:
		break;
	}

	/* Update the LCD display */
	LCD->SR |= LCD_SR_UDR;
}

/**
 * @brief  Write a character in the LCD RAM buffer.
 * @param  ch: The character to display.
 * @param  Point: A point to add in front of char.
 *          This parameter can be one of the following values:
 *              @arg POINT_OFF: No point to add in front of char.
 *              @arg POINT_ON: Add a point in front of char.
 * @param  Colon: Flag indicating if a colon character has to be added in front
 *                     of displayed character.
 *          This parameter can be one of the following values:
 *              @arg DOUBLEPOINT_OFF: No colon to add in back of char.
 *              @arg DOUBLEPOINT_ON: Add an colon in back of char.
 * @param  Position: Position in the LCD of the character to write.
 *                   This parameter can be any value in range [1:6].
 * @retval None
 * @note   Required preconditions: The LCD should be cleared before to start the
 *         write operation.
 */
void LCD_DisplayChar(uint8_t* ch, _Bool dot, _Bool colon, LCD_Digit_t position)
{
	WriteChar(ch, dot, colon, position);

	// Update the LCD display
	LCD->SR |= LCD_SR_UDR;
}

/**
 * @brief  Write a character string in the LCD RAM buffer.
 * @param  ptr: Pointer to string to display on the LCD Glass.
 * @retval None
 */
void LCD_DisplayString(uint8_t* ptr)
{
	LCD_Digit_t position = LCD_DIGIT_1;

	/* Send the string character by character on lCD */
	while ((*ptr != 0) & (position <= LCD_DIGIT_6))
	{
		/* Write one character on LCD */
		WriteChar(ptr, false, false, position);

		/* Point on the next character */
		ptr++;

		/* Increment the character counter */
		position++;
	}

	/* Update the LCD display */
	LCD->SR |= LCD_SR_UDR;
}


/**
 * @brief  Clear the whole LCD RAM buffer.
 * @retval None
 */
void LCD_ClearAll(void)
{
	// Wait until LCD ready */
	while ((LCD->SR & LCD_SR_UDR) != 0); // Wait for Update Display Request Bit

	for (uint8_t counter = 0; counter < 16; counter++)
	{
		LCD->RAM[counter] = 0;
	}

	/* Update the LCD display */
	LCD->SR |= LCD_SR_UDR;
}

/**
 * @brief  Clear only text field.
 * @retval None
 */
void LCD_ClearText(void)
{
	LCD_DisplayString("      ");
}

/**
 * @brief  Display a string in scrolling mode
 * @param  ptr: Pointer to string to display on the LCD Glass.
 * @param  nScroll: Specifies how many time the message will be scrolled
 * @param  ScrollSpeed : Specifies the speed of the scroll, low value gives
 *         higher speed
 * @retval None
 * @note   Required preconditions: The LCD should be cleared before to start the
 *         write operation.
 */
void LCD_ScrollSentence(uint8_t* ptr, uint16_t nScroll, uint16_t ScrollSpeed)
{
	uint8_t repetition = 0, nbrchar = 0, sizestr = 0;
	uint8_t* ptr1;
	uint8_t str[6] = "";

	/* Reset interrupt variable in case key was press before entering function */
	bLCDGlass_KeyPressed = 0;

	if(ptr == 0)
	{
		return;
	}

	/* To calculate end of string */
	for(ptr1 = ptr, sizestr = 0; *ptr1 != 0; sizestr++, ptr1++);

	ptr1 = ptr;

	LCD_DisplayString(str);

	/* To shift the string for scrolling display*/
	for (repetition = 0; repetition < nScroll; repetition++)
	{
		for(nbrchar = 0; nbrchar < sizestr; nbrchar++)
		{
			*(str) =* (ptr1+((nbrchar+0)%sizestr));
			*(str+1) =* (ptr1+((nbrchar+1)%sizestr));
			*(str+2) =* (ptr1+((nbrchar+2)%sizestr));
			*(str+3) =* (ptr1+((nbrchar+3)%sizestr));
			*(str+4) =* (ptr1+((nbrchar+4)%sizestr));
			*(str+5) =* (ptr1+((nbrchar+5)%sizestr));
			LCD_DisplayString((uint8_t*)"      ");
			LCD_DisplayString(str);

			/* user button pressed stop the scrolling sentence */
			if(bLCDGlass_KeyPressed)
			{
				bLCDGlass_KeyPressed = 0;
				return;
			}
			delay_ms(ScrollSpeed);
		}
	}
}

/**
 * @}
 */

/** @addtogroup STM32L476G_DISCOVERY_LCD_Private_Functions
 * @{
 */


/**
 * @brief  Convert an ascii char to the a LCD Digit.
 * @param  Char: a char to display.
 * @param  Point: a point to add in front of char
 *         This parameter can be: POINT_OFF or POINT_ON
 * @param  Colon : flag indicating if a colon character has to be added in front
 *         of displayed character.
 *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
 * @retval None
 */
static void ConvertChar(uint8_t* ch, _Bool point, _Bool colon)
{
	uint16_t c = 0 ;
	uint8_t loop = 0, index = 0;

	switch (*ch)
	{
	case ' ' :
		c = 0x00;
		break;

	case '*':
		c = C_STAR;
		break;

	case '(' :
		c = C_OPENPARMAP;
		break;

	case ')' :
		c = C_CLOSEPARMAP;
		break;

	case 'd' :
		c = C_DMAP;
		break;

	case 'm' :
		c = C_MMAP;
		break;

	case 'n' :
		c = C_NMAP;
		break;

	case 'µ' :
		c = C_UMAP;
		break;

	case '-' :
		c = C_MINUS;
		break;

	case '+' :
		c = C_PLUS;
		break;

	case '/' :
		c = C_SLATCH;
		break;

	case '°' :
		c = C_PERCENT_1;
		break;
	case '%' :
		c = C_PERCENT_2;
		break;
	case 255 :
		c = C_FULL;
		break ;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		c = NumberMap[*ch - ASCII_CHAR_0];
		break;

	default:
		/* The character Char is one letter in upper case*/
		if ( (*ch < ASCII_CHAR_LEFT_OPEN_BRACKET) && (*ch > ASCII_CHAR_AT_SYMBOL) )
		{
			c = CapLetterMap[*ch - 'A'];
		}
		/* The character Char is one letter in lower case*/
		if ( (*ch < ASCII_CHAR_LEFT_OPEN_BRACE) && ( *ch > ASCII_CHAR_APOSTROPHE) )
		{
			c = CapLetterMap[*ch - 'a'];
		}
		break;
	}

	/* Set the Digital point can be displayed if the point is on */
	if (point)
	{
		c |= 0x0002;
	}

	/* Set the "COL" segment in the character that can be displayed if the colon is on */
	if (colon)
	{
		c |= 0x0020;
	}

	for (loop = 12,index=0 ;index < 4; loop -= 4,index++)
	{
		Digit[index] = (c >> loop) & 0x0f; /*To isolate the less significant Digit */
	}
}

/**
 * @brief  Write a character in the LCD frame buffer.
 * @param  ch: the character to display.
 * @param  Point: a point to add in front of char
 *         This parameter can be: POINT_OFF or POINT_ON
 * @param  Colon: flag indicating if a colon character has to be added in front
 *         of displayed character.
 *         This parameter can be: DOUBLEPOINT_OFF or DOUBLEPOINT_ON.
 * @param  Position: position in the LCD of the character to write [1:6]
 * @retval None
 */
static void WriteChar(uint8_t* ch, _Bool dot, _Bool colon, LCD_Digit_t position)
{
	// Wait for Update Display Request Bit
	while ((LCD->SR & LCD_SR_UDR));

	/* To convert displayed character in segment in array Digit */
	ConvertChar(ch, dot, colon);

	switch (position)
	{
	/* Position 1 on LCD (Digit1)*/
	case LCD_DIGIT_1:
		LCD->RAM[0] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
		LCD->RAM[2] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
		LCD->RAM[4] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
		LCD->RAM[6] &= ~( 1U << 4 | 1U << 23 | 1U << 22 | 1U << 3 );
		/* 1G 1B 1M 1E */
		LCD->RAM[0] |= ((Digit[0] & 0x1) << 4) | (((Digit[0] & 0x2) >> 1) << 23) | (((Digit[0] & 0x4) >> 2) << 22) | (((Digit[0] & 0x8) >> 3) << 3);
		/* 1F 1A 1C 1D  */
		LCD->RAM[2] |= ((Digit[1] & 0x1) << 4) | (((Digit[1] & 0x2) >> 1) << 23) | (((Digit[1] & 0x4) >> 2) << 22) | (((Digit[1] & 0x8) >> 3) << 3);
		/* 1Q 1K 1Col 1P  */
		LCD->RAM[4] |= ((Digit[2] & 0x1) << 4) | (((Digit[2] & 0x2) >> 1) << 23) | (((Digit[2] & 0x4) >> 2) << 22) | (((Digit[2] & 0x8) >> 3) << 3);
		/* 1H 1J 1DP 1N  */
		LCD->RAM[6] |= ((Digit[3] & 0x1) << 4) | (((Digit[3] & 0x2) >> 1) << 23) | (((Digit[3] & 0x4) >> 2) << 22) | (((Digit[3] & 0x8) >> 3) << 3);

		break;

		/* Position 2 on LCD (Digit2)*/
	case LCD_DIGIT_2:
		LCD->RAM[0] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
		LCD->RAM[2] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
		LCD->RAM[4] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
		LCD->RAM[6] &= ~( 1U << 6 | 1U << 13 | 1U << 12 | 1U << 5 );
		/* 2G 2B 2M 2E */
		LCD->RAM[0] |= ((Digit[0] & 0x1) << 6) | (((Digit[0] & 0x2) >> 1) << 13) | (((Digit[0] & 0x4) >> 2) << 12) | (((Digit[0] & 0x8) >> 3) << 5);
		/* 2F 2A 2C 2D  */
		LCD->RAM[2] |= ((Digit[1] & 0x1) << 6) | (((Digit[1] & 0x2) >> 1) << 13) | (((Digit[1] & 0x4) >> 2) << 12) | (((Digit[1] & 0x8) >> 3) << 5);
		/* 2Q 2K 2Col 2P  */
		LCD->RAM[4] |= ((Digit[2] & 0x1) << 6) | (((Digit[2] & 0x2) >> 1) << 13) | (((Digit[2] & 0x4) >> 2) << 12) | (((Digit[2] & 0x8) >> 3) << 5);
		/* 2H 2J 2DP 2N  */
		LCD->RAM[6] |= ((Digit[3] & 0x1) << 6) | (((Digit[3] & 0x2) >> 1) << 13) | (((Digit[3] & 0x4) >> 2) << 12) | (((Digit[3] & 0x8) >> 3) << 5);

		break;

		/* Position 3 on LCD (Digit3)*/
	case LCD_DIGIT_3:
		LCD->RAM[0] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
		LCD->RAM[2] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
		LCD->RAM[4] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
		LCD->RAM[6] &= ~( 1U << 15 | 1U << 29 | 1U << 28 | 1U << 14 );
		/* 3G 3B 3M 3E */
		LCD->RAM[0] |= ((Digit[0] & 0x1) << 15) | (((Digit[0] & 0x2) >> 1) << 29) | (((Digit[0] & 0x4) >> 2) << 28) | (((Digit[0] & 0x8) >> 3) << 14);
		/* 3F 3A 3C 3D */
		LCD->RAM[2] |= ((Digit[1] & 0x1) << 15) | (((Digit[1] & 0x2) >> 1) << 29) | (((Digit[1] & 0x4) >> 2) << 28) | (((Digit[1] & 0x8) >> 3) << 14);
		/* 3Q 3K 3Col 3P  */
		LCD->RAM[4] |= ((Digit[2] & 0x1) << 15) | (((Digit[2] & 0x2) >> 1) << 29) | (((Digit[2] & 0x4) >> 2) << 28) | (((Digit[2] & 0x8) >> 3) << 14);
		/* 3H 3J 3DP  3N  */
		LCD->RAM[6] |= ((Digit[3] & 0x1) << 15) | (((Digit[3] & 0x2) >> 1) << 29) | (((Digit[3] & 0x4) >> 2) << 28) | (((Digit[3] & 0x8) >> 3) << 14);

		break;

		/* Position 4 on LCD (Digit4)*/
	case LCD_DIGIT_4:
		LCD->RAM[0] &= ~( 1U << 31 | 1U << 30);
		LCD->RAM[1] &= ~( 1U << 1 | 1U << 0 );
		LCD->RAM[2] &= ~( 1U << 31 | 1U << 30);
		LCD->RAM[3] &= ~( 1U << 1 | 1U << 0 );
		LCD->RAM[4] &= ~( 1U << 31 | 1U << 30);
		LCD->RAM[5] &= ~( 1U << 1 | 1U << 0 );
		LCD->RAM[6] &= ~( 1U << 31 | 1U << 30);
		LCD->RAM[7] &= ~( 1U << 1 | 1U << 0 );
		/* 4G 4B 4M 4E */
		LCD->RAM[0] |= ((Digit[0] & 0x1) << 31) | (((Digit[0] & 0x8) >> 3) << 30);
		LCD->RAM[1] |= (((Digit[0] & 0x2) >> 1) << 1) | (((Digit[0] & 0x4) >> 2) << 0);
		/* 4F 4A 4C 4D */
		LCD->RAM[2] |= ((Digit[1] & 0x1) << 31) | (((Digit[1] & 0x8) >> 3) << 30);
		LCD->RAM[3] |= (((Digit[1] & 0x2) >> 1) << 1) | (((Digit[1] & 0x4) >> 2) << 0);
		/* 4Q 4K 4Col 4P  */
		LCD->RAM[4] |= ((Digit[2] & 0x1) << 31) | (((Digit[2] & 0x8) >> 3) << 30);
		LCD->RAM[5] |= (((Digit[2] & 0x2) >> 1) << 1) | (((Digit[2] & 0x4) >> 2) << 0);
		/* 4H 4J 4DP  4N  */
		LCD->RAM[6] |= ((Digit[3] & 0x1) << 31) | (((Digit[3] & 0x8) >> 3) << 30);
		LCD->RAM[7] |= (((Digit[3] & 0x2) >> 1) << 1) | (((Digit[3] & 0x4) >> 2) << 0);
		break;

		/* Position 5 on LCD (Digit5)*/
	case LCD_DIGIT_5:

		LCD->RAM[0] &= ~( 1U << 25 | 1U << 24);
		LCD->RAM[1] &= ~( 1U << 3 | 1U << 2 );
		LCD->RAM[2] &= ~( 1U << 25 | 1U << 24);
		LCD->RAM[3] &= ~( 1U << 3 | 1U << 2 );
		LCD->RAM[4] &= ~( 1U << 24 );
		LCD->RAM[5] &= ~( 1U << 3 | 1U << 2 );
		LCD->RAM[6] &= ~( 1U << 24 );
		LCD->RAM[7] &= ~( 1U << 3 | 1U << 2 );
		/* 5G 5B 5M 5E */
		LCD->RAM[0] |= (((Digit[0] & 0x2) >> 1) << 25) | (((Digit[0] & 0x4) >> 2) << 24);
		LCD->RAM[1] |= ((Digit[0] & 0x1) << 3) | (((Digit[0] & 0x8) >> 3) << 2);
		/* 5F 5A 5C 5D */
		LCD->RAM[2] |= (((Digit[1] & 0x2) >> 1) << 25) | (((Digit[1] & 0x4) >> 2) << 24);
		LCD->RAM[3] |= ((Digit[1] & 0x1) << 3) | (((Digit[1] & 0x8) >> 3) << 2);
		/* 5Q 5K 5Col 5P  */
		LCD->RAM[4] |= (((Digit[2] & 0x2) >> 1) << 25) | (((Digit[2] & 0x4) >> 2) << 24);
		LCD->RAM[5] |= ((Digit[2] & 0x1) << 3) | (((Digit[2] & 0x8) >> 3) << 2);
		/* 5H 5J 5DP  5N  */
		LCD->RAM[6] |= (((Digit[3] & 0x2) >> 1) << 25) | (((Digit[3] & 0x4) >> 2) << 24);
		LCD->RAM[7] |= ((Digit[3] & 0x1) << 3) | (((Digit[3] & 0x8) >> 3) << 2);
		break;

		/* Position 6 on LCD (Digit6)*/
	case LCD_DIGIT_6:
		LCD->RAM[0] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
		LCD->RAM[2] &= ~( 1U << 17 | 1U << 8 | 1U << 9 | 1U << 26 );
		LCD->RAM[4] &= ~( 1U << 17 | 1U << 9 | 1U << 26 );
		LCD->RAM[6] &= ~( 1U << 17 | 1U << 9 | 1U << 26 );
		/* 6G 6B 6M 6E */
		LCD->RAM[0] |= ((Digit[0] & 0x1) << 17) | (((Digit[0] & 0x2) >> 1) << 8) | (((Digit[0] & 0x4) >> 2) << 9) | (((Digit[0] & 0x8) >> 3) << 26);
		/* 6F 6A 6C 6D */
		LCD->RAM[2] |= ((Digit[1] & 0x1) << 17) | (((Digit[1] & 0x2) >> 1) << 8) | (((Digit[1] & 0x4) >> 2) << 9) | (((Digit[1] & 0x8) >> 3) << 26);
		/* 6Q 6K 6Col 6P  */
		LCD->RAM[4] |= ((Digit[2] & 0x1) << 17) | (((Digit[2] & 0x2) >> 1) << 8) | (((Digit[2] & 0x4) >> 2) << 9) | (((Digit[2] & 0x8) >> 3) << 26);
		/* 6H 6J 6DP  6N  */
		LCD->RAM[6] |= ((Digit[3] & 0x1) << 17) | (((Digit[3] & 0x2) >> 1) << 8) | (((Digit[3] & 0x4) >> 2) << 9) | (((Digit[3] & 0x8) >> 3) << 26);
		break;

	default:
		break;
	}
}

