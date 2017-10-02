/*
 * debug.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "usart/usart.h"
#include <string.h>
#include <stdio.h>

static char debug_buffer[USART_TX_CB_SIZE];

void Debug_Init(void)
{
	USART_Init();
}

void Debug_Msg(char* msg, ...)
{
	snprintf(debug_buffer,sizeof(debug_buffer), msg);
	USART_TxDMA(debug_buffer, strlen(debug_buffer));
	USART_TxDMA("\n", 1);

}
