/*
 * debug.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "usart/usart.h"
#include "misc.h"
#ifdef USE_FREERTOS
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"
#endif

#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static char debug_buffer[USART_TX_CB_SIZE];

#ifdef USE_FREERTOS
static SemaphoreHandle_t shMutexUSART;
#endif

void Debug_Init( void )
{
	USART_Init();
#ifdef USE_FREERTOS
	// Create Mutex for USART interface
	shMutexUSART = xSemaphoreCreateMutex();
	xSemaphoreGive(shMutexUSART);
#endif
}

void Debug_Simple( const char* msg )
{
	USART_TxDMA( (void*) msg, strlen( msg ) );
	USART_TxDMA( "\r\n", 2 );
}

void Debug_Printf( const char* fmt, ... )
{
	va_list args;
#ifdef USE_FREERTOS
	if(xSemaphoreTake(shMutexUSART, 10))
	{
#else
	enter_critical();
#endif

	va_start( args, fmt );
	vsnprintf( debug_buffer, sizeof(debug_buffer), fmt, args );
	va_end( args );

	USART_TxDMA( debug_buffer, strlen( debug_buffer ) );
	USART_TxDMA( "\r\n", 2 );

#ifdef USE_FREERTOS
	xSemaphoreGive(shMutexUSART);
	}
#else
	exit_critical();
#endif
}
