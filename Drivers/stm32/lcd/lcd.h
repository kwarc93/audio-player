/*
 * lcd.h
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */
#ifndef STM32_LCD_LCD_H_
#define STM32_LCD_LCD_H_

#include "main.h"
#include <stdbool.h>

/**
  * @brief LCD Bar location
  */
#define LCD_BAR0_2_COM            LCD_COM3
#define LCD_BAR1_3_COM            LCD_COM2
#define LCD_BAR0_SEG              LCD_SEG11
#define LCD_BAR1_SEG              LCD_SEG11
#define LCD_BAR2_SEG              LCD_SEG9
#define LCD_BAR3_SEG              LCD_SEG9
#define LCD_BAR0_2_SEG_MASK       ~(LCD_BAR0_SEG | LCD_BAR2_SEG)
#define LCD_BAR1_3_SEG_MASK       ~(LCD_BAR1_SEG | LCD_BAR3_SEG)

/**
  * @brief LCD Pins definition.
  */
#if defined (USE_STM32L476G_DISCO_REVC) || defined (USE_STM32L476G_DISCO_REVB)
#define LCD_GPIO_BANKA_PINS  (GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |    \
                              GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_15)
#define LCD_GPIO_BANKB_PINS  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 |    \
                              GPIO_PIN_5 | GPIO_PIN_9 | GPIO_PIN_12 |   \
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#define LCD_GPIO_BANKC_PINS  (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |    \
                              GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8)
#define LCD_GPIO_BANKD_PINS  (GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 |   \
                              GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | \
                              GPIO_PIN_14 | GPIO_PIN_15)
#elif defined (USE_STM32L476G_DISCO_REVA)
#define LCD_GPIO_BANKA_PINS  (GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_15)
#define LCD_GPIO_BANKB_PINS  (GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 |    \
                              GPIO_PIN_5 | GPIO_PIN_12 | GPIO_PIN_13 |  \
                              GPIO_PIN_14 | GPIO_PIN_15)
#define LCD_GPIO_BANKC_PINS  (GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |    \
                              GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 |    \
                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12)
#define LCD_GPIO_BANKD_PINS  (GPIO_PIN_2 | GPIO_PIN_8 | GPIO_PIN_9 |    \
                              GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | \
                              GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15)
#endif

/** @defgroup LCD_Prescaler LCD Prescaler
  * @{
  */
#define LCD_PRESCALER_1        ((uint32_t)0x00000000)  /*!< CLKPS = LCDCLK        */
#define LCD_PRESCALER_2        ((uint32_t)0x00400000)  /*!< CLKPS = LCDCLK/2      */
#define LCD_PRESCALER_4        ((uint32_t)0x00800000)  /*!< CLKPS = LCDCLK/4      */
#define LCD_PRESCALER_8        ((uint32_t)0x00C00000)  /*!< CLKPS = LCDCLK/8      */
#define LCD_PRESCALER_16       ((uint32_t)0x01000000)  /*!< CLKPS = LCDCLK/16     */
#define LCD_PRESCALER_32       ((uint32_t)0x01400000)  /*!< CLKPS = LCDCLK/32     */
#define LCD_PRESCALER_64       ((uint32_t)0x01800000)  /*!< CLKPS = LCDCLK/64     */
#define LCD_PRESCALER_128      ((uint32_t)0x01C00000)  /*!< CLKPS = LCDCLK/128    */
#define LCD_PRESCALER_256      ((uint32_t)0x02000000)  /*!< CLKPS = LCDCLK/256    */
#define LCD_PRESCALER_512      ((uint32_t)0x02400000)  /*!< CLKPS = LCDCLK/512    */
#define LCD_PRESCALER_1024     ((uint32_t)0x02800000)  /*!< CLKPS = LCDCLK/1024   */
#define LCD_PRESCALER_2048     ((uint32_t)0x02C00000)  /*!< CLKPS = LCDCLK/2048   */
#define LCD_PRESCALER_4096     ((uint32_t)0x03000000)  /*!< CLKPS = LCDCLK/4096   */
#define LCD_PRESCALER_8192     ((uint32_t)0x03400000)  /*!< CLKPS = LCDCLK/8192   */
#define LCD_PRESCALER_16384    ((uint32_t)0x03800000)  /*!< CLKPS = LCDCLK/16384  */
#define LCD_PRESCALER_32768    ((uint32_t)0x03C00000)  /*!< CLKPS = LCDCLK/32768  */
/**
  * @}
  */

/** @defgroup LCD_Divider LCD Divider
  * @{
  */
#define LCD_DIVIDER_16    ((uint32_t)0x00000000)  /*!< LCD frequency = CLKPS/16 */
#define LCD_DIVIDER_17    ((uint32_t)0x00040000)  /*!< LCD frequency = CLKPS/17 */
#define LCD_DIVIDER_18    ((uint32_t)0x00080000)  /*!< LCD frequency = CLKPS/18 */
#define LCD_DIVIDER_19    ((uint32_t)0x000C0000)  /*!< LCD frequency = CLKPS/19 */
#define LCD_DIVIDER_20    ((uint32_t)0x00100000)  /*!< LCD frequency = CLKPS/20 */
#define LCD_DIVIDER_21    ((uint32_t)0x00140000)  /*!< LCD frequency = CLKPS/21 */
#define LCD_DIVIDER_22    ((uint32_t)0x00180000)  /*!< LCD frequency = CLKPS/22 */
#define LCD_DIVIDER_23    ((uint32_t)0x001C0000)  /*!< LCD frequency = CLKPS/23 */
#define LCD_DIVIDER_24    ((uint32_t)0x00200000)  /*!< LCD frequency = CLKPS/24 */
#define LCD_DIVIDER_25    ((uint32_t)0x00240000)  /*!< LCD frequency = CLKPS/25 */
#define LCD_DIVIDER_26    ((uint32_t)0x00280000)  /*!< LCD frequency = CLKPS/26 */
#define LCD_DIVIDER_27    ((uint32_t)0x002C0000)  /*!< LCD frequency = CLKPS/27 */
#define LCD_DIVIDER_28    ((uint32_t)0x00300000)  /*!< LCD frequency = CLKPS/28 */
#define LCD_DIVIDER_29    ((uint32_t)0x00340000)  /*!< LCD frequency = CLKPS/29 */
#define LCD_DIVIDER_30    ((uint32_t)0x00380000)  /*!< LCD frequency = CLKPS/30 */
#define LCD_DIVIDER_31    ((uint32_t)0x003C0000)  /*!< LCD frequency = CLKPS/31 */
/**
  * @}
  */


/** @defgroup LCD_Duty LCD Duty
  * @{
  */
#define LCD_DUTY_STATIC                 ((uint32_t)0x00000000)            /*!< Static duty */
#define LCD_DUTY_1_2                    (LCD_CR_DUTY_0)                   /*!< 1/2 duty    */
#define LCD_DUTY_1_3                    (LCD_CR_DUTY_1)                   /*!< 1/3 duty    */
#define LCD_DUTY_1_4                    ((LCD_CR_DUTY_1 | LCD_CR_DUTY_0)) /*!< 1/4 duty    */
#define LCD_DUTY_1_8                    (LCD_CR_DUTY_2)                   /*!< 1/8 duty    */
/**
  * @}
  */


/** @defgroup LCD_Bias LCD Bias
  * @{
  */
#define LCD_BIAS_1_4                    ((uint32_t)0x00000000)  /*!< 1/4 Bias */
#define LCD_BIAS_1_2                    LCD_CR_BIAS_0           /*!< 1/2 Bias */
#define LCD_BIAS_1_3                    LCD_CR_BIAS_1           /*!< 1/3 Bias */
/**
  * @}
  */

/** @defgroup LCD_PulseOnDuration LCD Pulse On Duration
  * @{
  */
#define LCD_PULSEONDURATION_0           ((uint32_t)0x00000000)          /*!< Pulse ON duration = 0 pulse   */
#define LCD_PULSEONDURATION_1           (LCD_FCR_PON_0)                 /*!< Pulse ON duration = 1/CK_PS  */
#define LCD_PULSEONDURATION_2           (LCD_FCR_PON_1)                 /*!< Pulse ON duration = 2/CK_PS  */
#define LCD_PULSEONDURATION_3           (LCD_FCR_PON_1 | LCD_FCR_PON_0) /*!< Pulse ON duration = 3/CK_PS  */
#define LCD_PULSEONDURATION_4           (LCD_FCR_PON_2)                 /*!< Pulse ON duration = 4/CK_PS  */
#define LCD_PULSEONDURATION_5           (LCD_FCR_PON_2 | LCD_FCR_PON_0) /*!< Pulse ON duration = 5/CK_PS  */
#define LCD_PULSEONDURATION_6           (LCD_FCR_PON_2 | LCD_FCR_PON_1) /*!< Pulse ON duration = 6/CK_PS  */
#define LCD_PULSEONDURATION_7           (LCD_FCR_PON)                   /*!< Pulse ON duration = 7/CK_PS  */
/**
  * @}
  */


/** @defgroup LCD_DeadTime LCD Dead Time
  * @{
  */
#define LCD_DEADTIME_0                  ((uint32_t)0x00000000)            /*!< No dead Time  */
#define LCD_DEADTIME_1                  (LCD_FCR_DEAD_0)                  /*!< One Phase between different couple of Frame   */
#define LCD_DEADTIME_2                  (LCD_FCR_DEAD_1)                  /*!< Two Phase between different couple of Frame   */
#define LCD_DEADTIME_3                  (LCD_FCR_DEAD_1 | LCD_FCR_DEAD_0) /*!< Three Phase between different couple of Frame */
#define LCD_DEADTIME_4                  (LCD_FCR_DEAD_2)                  /*!< Four Phase between different couple of Frame  */
#define LCD_DEADTIME_5                  (LCD_FCR_DEAD_2 | LCD_FCR_DEAD_0) /*!< Five Phase between different couple of Frame  */
#define LCD_DEADTIME_6                  (LCD_FCR_DEAD_2 | LCD_FCR_DEAD_1) /*!< Six Phase between different couple of Frame   */
#define LCD_DEADTIME_7                  (LCD_FCR_DEAD)                    /*!< Seven Phase between different couple of Frame */
/**
  * @}
  */

/** @defgroup LCD_BlinkFrequency LCD Blink Frequency
  * @{
  */
#define LCD_BLINKFREQUENCY_DIV8         ((uint32_t)0x00000000)                /*!< The Blink frequency = fLCD/8    */
#define LCD_BLINKFREQUENCY_DIV16        (LCD_FCR_BLINKF_0)                    /*!< The Blink frequency = fLCD/16   */
#define LCD_BLINKFREQUENCY_DIV32        (LCD_FCR_BLINKF_1)                    /*!< The Blink frequency = fLCD/32   */
#define LCD_BLINKFREQUENCY_DIV64        (LCD_FCR_BLINKF_1 | LCD_FCR_BLINKF_0) /*!< The Blink frequency = fLCD/64   */
#define LCD_BLINKFREQUENCY_DIV128       (LCD_FCR_BLINKF_2)                    /*!< The Blink frequency = fLCD/128  */
#define LCD_BLINKFREQUENCY_DIV256       (LCD_FCR_BLINKF_2 |LCD_FCR_BLINKF_0)  /*!< The Blink frequency = fLCD/256  */
#define LCD_BLINKFREQUENCY_DIV512       (LCD_FCR_BLINKF_2 |LCD_FCR_BLINKF_1)  /*!< The Blink frequency = fLCD/512  */
#define LCD_BLINKFREQUENCY_DIV1024      (LCD_FCR_BLINKF)                      /*!< The Blink frequency = fLCD/1024 */
/**
  * @}
  */

/** @defgroup LCD_Contrast LCD Contrast
  * @{
  */
#define LCD_CONTRASTLEVEL_0               ((uint32_t)0x00000000)        /*!< Maximum Voltage = 2.60V    */
#define LCD_CONTRASTLEVEL_1               (LCD_FCR_CC_0)                /*!< Maximum Voltage = 2.73V    */
#define LCD_CONTRASTLEVEL_2               (LCD_FCR_CC_1)                /*!< Maximum Voltage = 2.86V    */
#define LCD_CONTRASTLEVEL_3               (LCD_FCR_CC_1 | LCD_FCR_CC_0) /*!< Maximum Voltage = 2.99V    */
#define LCD_CONTRASTLEVEL_4               (LCD_FCR_CC_2)                /*!< Maximum Voltage = 3.12V    */
#define LCD_CONTRASTLEVEL_5               (LCD_FCR_CC_2 | LCD_FCR_CC_0) /*!< Maximum Voltage = 3.26V    */
#define LCD_CONTRASTLEVEL_6               (LCD_FCR_CC_2 | LCD_FCR_CC_1) /*!< Maximum Voltage = 3.40V    */
#define LCD_CONTRASTLEVEL_7               (LCD_FCR_CC)                  /*!< Maximum Voltage = 3.55V    */
/**
  * @}
  */

/* code for '(' character */
#define C_OPENPARMAP          ((uint16_t) 0x0041)

/* code for ')' character */
#define C_CLOSEPARMAP         ((uint16_t) 0x0088)

/* code for 'd' character */
#define C_DMAP                ((uint16_t) 0xf300)

/* code for 'm' character */
#define C_MMAP                ((uint16_t) 0xb210)

/* code for 'n' character */
#define C_NMAP                ((uint16_t) 0x2210)

/* code for 'µ' character */
#define C_UMAP                ((uint16_t) 0x6084)

/* constant code for '*' character */
#define C_STAR                ((uint16_t) 0xA0DD)

/* constant code for '-' character */
#define C_MINUS               ((uint16_t) 0xA000)

/* constant code for '+' character */
#define C_PLUS                ((uint16_t) 0xA014)

/* constant code for '/' */
#define C_SLATCH              ((uint16_t) 0x00c0)

/* constant code for ° */
#define C_PERCENT_1           ((uint16_t) 0xec00)

/* constant code for small o */
#define C_PERCENT_2           ((uint16_t) 0xb300)

#define C_FULL                ((uint16_t) 0xffdd)

/**
  * @brief LCD digit position
  */
typedef enum
{
  LCD_DIGIT_1 = 0,
  LCD_DIGIT_2 = 1,
  LCD_DIGIT_3 = 2,
  LCD_DIGIT_4 = 3,
  LCD_DIGIT_5 = 4,
  LCD_DIGIT_6 = 5,
  LCD_DIGIT_MAX_NUMBER = 6,
}LCD_Digit_t;

/**
  * @brief LCD Glass Battery Level
  * element values correspond to different LCD Glass battery levels
  */
typedef enum
{
  BARLEVEL_OFF = 0,
  BARLEVEL_1_4 = 1,
  BARLEVEL_1_2 = 2,
  BARLEVEL_3_4 = 3,
  BARLEVEL_FULL = 4
}LCD_BattLevel_t;

/**
  * @brief LCD Glass Bar Id
  */
typedef enum
{
  LCD_BAR_NONE  = 0,
  LCD_BAR_0     = (1 << 0),
  LCD_BAR_1     = (1 << 1),
  LCD_BAR_2     = (1 << 2),
  LCD_BAR_3     = (1 << 3)
}LCD_BarSeg_t;

/* Exported functions --------------------------------------------------------*/

/** @defgroup STM32L476G_DISCOVERY_LCD_Exported_Functions Exported Functions
  * @{
  */
void LCD_Init(void);
void LCD_BlinkConfig(uint32_t BlinkMode, uint32_t BlinkFrequency);
void LCD_Contrast(uint32_t Contrast);
void LCD_DisplayChar(uint8_t* ch, _Bool dot, _Bool colon, LCD_Digit_t Position);
void LCD_DisplayString(uint8_t* ptr);
void LCD_ScrollSentence(uint8_t* ptr, _Bool reset);
void LCD_DisplayBar(LCD_BarSeg_t BarId, _Bool state);
void LCD_DisplayBarLevel(LCD_BattLevel_t BarLevel);
void LCD_ClearAll(void);
void LCD_ClearText(void);
/**
  * @}
  */


#endif /* STM32_LCD_LCD_H_ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
