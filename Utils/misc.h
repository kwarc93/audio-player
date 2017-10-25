/*
 * misc.h
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */

#ifndef MISC_H_
#define MISC_H_

#include "main.h"

#define ARRAY_LEN(a)	(sizeof(a)/sizeof(a[0]))
#define AT_CCMRAM	__attribute__((section (".ccmram")))
#define AT_SDRAM	__attribute__((section (".sdram")))

_Bool is_in_handler_mode(void);
void disable_interrupts(void);
void enable_interrupts(void);
void enter_critical(void);
void exit_critical(void);

void delay_us(uint32_t t);
void delay_ms(uint32_t t);

char* mystrcat( char* dest, char* src );


#endif /* MISC_H_ */
