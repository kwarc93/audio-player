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

/* Determine whether we are in thread mode or handler mode. */
int is_in_handler_mode(void)
{
  return __get_IPSR() != 0;
}

void delay_ms(uint32_t t)
{
	t = t * CPU_CLOCK / 1000;
	while(t--)
	{
		__NOP();
	}
}

char* mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while ((*dest++ = *src++));
     return --dest;
}


