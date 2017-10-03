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

#define DELAY_US(us) do {\
		asm volatile (	"MOV R0,%[loops]\n\t"\
						"1: \n\t"\
						"SUB R0, #1\n\t"\
						"CMP R0, #0\n\t"\
						"BNE 1b \n\t" : : [loops] "r" (16*us) : "memory"\
		);\
} while(0)

void delay_ms(uint32_t t)
{
	DELAY_US(t*1000UL);
}

/* Determine whether we are in thread mode or handler mode. */
int is_in_handler_mode(void)
{
  return __get_IPSR() != 0;
}

char* mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while ((*dest++ = *src++));
     return --dest;
}


