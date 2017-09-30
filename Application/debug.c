/*
 * debug.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "usart/usart.h"
#include <string.h>

void Debug_Init(void)
{
	USART_Init();
}

void Debug_Msg(char* msg)
{
	USART_TxDMA(msg, strlen(msg));

}
