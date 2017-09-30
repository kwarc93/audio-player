/*
 * main.c
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */
#include <stdlib.h>
#include "main.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "ui/leds.h"
#include "ui/display.h"
#include "ui/joystick.h"
#include "debug.h"

#ifdef DEBUG
#define DBG_MSG(MSG)	(Debug_Msg("[MAIN] " MSG "\r\n"))
#else
#define DBG_MSG(MSG)	do{}while(0);
#endif

#define mainFLASH_TASK_PRIORITY ( tskIDLE_PRIORITY + 1 )

static void prvConfigureClock(void);

int main(void)
{
	// Clock configuration
	prvConfigureClock();
	// Init debug module
	Debug_Init();
	DBG_MSG("-MP3 PLAYER-");
	// Tasks startup
	DBG_MSG("Starting tasks...");
	Led_StartTasks(mainFLASH_TASK_PRIORITY);
	Display_StartTasks(mainFLASH_TASK_PRIORITY);
	Joystick_StartTasks(mainFLASH_TASK_PRIORITY + 1);
	// Scheduler startup
	DBG_MSG("Starting scheduler...");
	vTaskStartScheduler();
}

static void prvConfigureClock(void)
{
#if defined(USE_MSI_CLOCK)
	/* Config RCC */
	// Select MSI as the clock source of System Clock
	RCC->CR |= RCC_CR_MSION;

	// Wait until MSI ready
	while ((RCC->CR & RCC_CR_MSIRDY) == 0);

	// MSIRANGE can be modified when MSI is off (MSION=0) or when MSI is ready
	RCC->CR &= ~RCC_CR_MSIRANGE;
	RCC->CR |= RCC_CR_MSIRANGE_9;	// Select MSI 24MHz

	// The MSIRGSEL bit in RCC->CR selects which MSIRANGE is used
	// If MSIREGSEL is 0, the MSIRANGE in RCC->CSR is used to select the MSI clock
	// If MSIREGSEL is 1, the MSIRANGE in RCC->CR is used
	RCC->CR |= RCC_CR_MSIRGSEL;

	// Enable MSI and wait until it's ready
	while ((RCC->CR & RCC_CR_MSIRDY) == 0);

#elif defined(USE_HSI_CLOCK)

uint32_t hsi_trim;

// To correctly read data from FLASH memory , the number of wait states (LATECY)
// must bue correctly programmed according to the frequency of the CPU clock (HCLK)
// and the supply voltage of the device.
FLASH->ACR &= ~FLASH_ACR_LATENCY;
FLASH->ACR |=  FLASH_ACR_LATENCY_2WS;

// Enable the internal high speed oscillator (HSI) and wait until ready
RCC->CR |= RCC_CR_HSION;
while(!(RCC->CR & RCC_CR_HSIRDY));

// Adjusts the HSI calibration value
// RC oscillator frequencies are factory calibrated by ST for 1% accuracy at 25*C
// After reset, the factory calibration value is loaded  in HSICAL[7:0] of RCC_ICSR.
hsi_trim = 16;	// user-programmable trimming value that is added to HSICAL[7:0] in ICSCR
RCC->ICSCR &= ~RCC_ICSCR_HSITRIM;
RCC->ICSCR |= hsi_trim << 24;

// Disable PLL
RCC->CR &= ~RCC_CR_PLLON;
while(RCC->CR & RCC_CR_PLLRDY);

// Select clock source to PLL
RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSI;

// Set PLL at 80MHz
// f(VCO clock) = f(PLL clock input) * (PLLN/PLLM) = 16MHz * (20/2) = 160MHz
// f(PLLR) = f(VCO clock) / PLLR = 160MHz / 2 = 80MHz
RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLN) | 20U << 8;
RCC->PLLCFGR = (RCC->PLLCFGR & ~RCC_PLLCFGR_PLLM) | 1U << 4;
RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR;
RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN;

// Enable PLL and wait until ready
RCC->CR |= RCC_CR_PLLON;
while(!(RCC->CR & RCC_CR_PLLRDY));

// Select PLL output as system clock
RCC->CFGR &= ~RCC_CFGR_SW;
RCC->CFGR |= RCC_CFGR_SW_PLL;

// Wait until system clock will be selected (PCLK1 & PCLK2)
while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS);

//@ TODO: Enable USB 48MHz clock
//@ TODO: Enable SAI1 clock

#else
#error Wrong system clock selection!
#endif
}
