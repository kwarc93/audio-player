/*
 * debug.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "usart/usart.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"
#include <string.h>
#include <stdio.h>

static char debug_buffer[USART_TX_CB_SIZE];
// USART Mutex handle
static SemaphoreHandle_t shMutexUSART;

void Debug_Init(void)
{
	USART_Init();

	// Create Mutex for USART interface
	shMutexUSART = xSemaphoreCreateMutex();
	xSemaphoreGive(shMutexUSART);
}

void Debug_Msg(char* msg, ...)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	snprintf(debug_buffer,sizeof(debug_buffer), msg);

	if(xSemaphoreTakeFromISR(shMutexUSART, &xHigherPriorityTaskWoken))
	{
		USART_TxDMA(debug_buffer, strlen(debug_buffer));
		USART_TxDMA("\n", 1);

		xSemaphoreGiveFromISR(shMutexUSART, &xHigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}
