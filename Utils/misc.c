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

void delay_ms(uint32_t t)
{
#ifdef USE_FREERTOS
	vTaskDelay(t);
#else
	t = t * CPU_CLOCK / 1000;
	while(t--)
	{
		__NOP();
	}
#endif
}

char* mystrcat( char* dest, char* src )
{
     while (*dest) dest++;
     while ((*dest++ = *src++));
     return --dest;
}


