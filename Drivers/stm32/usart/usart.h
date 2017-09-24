/*
 * USART.h
 *
 *  Created on: 05.02.2017
 *      Author: Kwarc
 */

#ifndef USART_H_
#define USART_H_

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

#define USARTx USART2
#define BAUD_RATE 115200UL

/* Rx circular buffer */
#define USART_RX_CB_SIZE 128					// Length of USART circular buffer
#define USART_RX_CB_MASK (USART_RX_CB_SIZE-1) 	// Mask for USART circular buffer
char USART_RxCB[USART_RX_CB_SIZE];				// USART rx circular buffer
uint8_t USART_RxHead;
uint8_t USART_RxTail;

/* Tx circular buffer */
#define USART_TX_CB_SIZE 128					// Length of USART circular buffer
char USART_TxCB[USART_TX_CB_SIZE];				// USART tx circular buffer

/* Message decoder */
char msg_id;									// Message id (c - command, d - data)
int32_t msg_data;								// Message data/command in decimal
_Bool msg_ready;
enum  commands {RESET = 0, TOGGLE_LED };
_Bool DMA1C7_TC;

void USART_Init(void);
void USART_Tx(char data);
void USART_TxDMA(void *src, uint8_t length);
char USART_Rx(void);
char USART_RxBufferRead(void);
void USART_TxString(char *s);

void USART_Parse(char c);
void USART_DecodeMessage(char id, int32_t data);
void USART_ProcessCommand(uint32_t cmd);

#endif /* USART_H_ */