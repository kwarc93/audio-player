/*
 * debug.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "usart/usart.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"
#include "misc.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static char debug_buffer[USART_TX_CB_SIZE];

// USART Mutex handle
static SemaphoreHandle_t shMutexUSART;

void Debug_Init(void)
{
	USART_Init();
#ifdef USE_FREERTOS
	// Create Mutex for USART interface
	shMutexUSART = xSemaphoreCreateMutex();
	xSemaphoreGive(shMutexUSART);
#endif
}

void Debug_Simple(const char* msg)
{
#ifdef USE_FREERTOS
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(xSemaphoreTakeFromISR(shMutexUSART, &xHigherPriorityTaskWoken))
	{
#endif
		USART_TxDMA((void*)msg, strlen(msg));
		USART_TxDMA("\n", 1);
#ifdef USE_FREERTOS
		xSemaphoreGiveFromISR(shMutexUSART, &xHigherPriorityTaskWoken);
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
#endif
}

void Debug_Printf(const char* fmt, ...)
{
	va_list args;
#ifdef USE_FREERTOS
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	if(xSemaphoreTakeFromISR(shMutexUSART, &xHigherPriorityTaskWoken))
	{
#endif
		va_start(args, fmt);
		vsnprintf(debug_buffer, sizeof(debug_buffer), fmt, args);
		va_end(args);
		USART_TxDMA(debug_buffer, strlen(debug_buffer));
		USART_TxDMA("\n", 1);
#ifdef USE_FREERTOS
		xSemaphoreGiveFromISR(shMutexUSART, &xHigherPriorityTaskWoken);
	}
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
#endif
}
