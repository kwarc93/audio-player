/*
 * misc.c
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */

#include "misc.h"

#ifdef USE_FREERTOS
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#endif

#include <stdbool.h>

#define DELAY_US_ASM(us) do {\
		asm volatile (	"MOV R0,%[loops]\n\t"\
						"1: \n\t"\
						"SUB R0, #1\n\t"\
						"CMP R0, #0\n\t"\
						"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
					 );\
} while(0)

#if 0
#pragma GCC push_options
#pragma GCC optimize ("O3")
static inline void delayUS_DWT(uint32_t us) {
	volatile uint32_t cycles = (CPU_CLOCK/1000000L)*us;
	volatile uint32_t start = DWT->CYCCNT;
	do  {
	} while(DWT->CYCCNT - start < cycles);
}
#pragma GCC pop_options
#endif

static uint32_t primask;

void delay_us(uint32_t t)
{
	DELAY_US_ASM(t);
}

void delay_ms(uint32_t t)
{
	DELAY_US_ASM(t*1000UL);
}

_Bool is_in_handler_mode(void)
{
  return __get_IPSR() != 0;
}

void disable_interrupts(void)
{
	__disable_irq();
}

void enable_interrupts(void)
{
	__enable_irq();
}

void enter_critical(void)
{
#ifdef USE_FREERTOS
	portENTER_CRITICAL();
#else
	primask = __get_PRIMASK();
	disable_interrupts();
#endif
}

void exit_critical(void)
{
#ifdef USE_FREERTOS
	portEXIT_CRITICAL();
#else
	if(!primask)
		enable_interrupts();
#endif
}

__attribute__((always_inline)) inline void nop(void)
{
	asm volatile ("MOV R0, R0");
}

void sleep_deep(void)
{
	// Sleep deep
	PWR->CR1 |= PWR_CR1_LPMS_SHUTDOWN;
	SCB->SCR |= (1 << SCB_SCR_SLEEPDEEP_Pos);
}

char* mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while ((*dest++ = *src++));
     return --dest;
}



