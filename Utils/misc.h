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

int is_in_handler_mode(void);
void delay_ms(uint32_t t);
char* mystrcat( char* dest, char* src );


#endif /* MISC_H_ */
