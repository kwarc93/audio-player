/*
 * debug.h
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#ifndef DEBUG_H_
#define DEBUG_H_

void Debug_Init( void );
void Debug_Simple( const char* msg );
void Debug_Printf( const char* fmt, ... );

#endif /* DEBUG_H_ */
