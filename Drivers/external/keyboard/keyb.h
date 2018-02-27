//***********************************************************************
// Plik: keyb.h 
//
// Zaawansowana obsługa przycisków i klawiatur 
// Wersja:    1.0
// Licencja:  GPL v2
// Autor:     Deucalion  
// Email:     deucalion#wp.pl
// Szczegóły: http://mikrokontrolery.blogspot.com/2011/04/jezyk-c-biblioteka-obsluga-klawiatury.html
//
//***********************************************************************

#ifndef _KEYB_H_
#define _KEYB_H_

#include "main.h"

#define KEY_CHECK_PERIOD_MS	(10)

#define KEY_PERIOD_1S      (1000/KEY_CHECK_PERIOD_MS)
#define KEY_PERIOD_750MS   (750/KEY_CHECK_PERIOD_MS)
#define KEY_PERIOD_500MS   (500/KEY_CHECK_PERIOD_MS)
#define KEY_PERIOD_100MS   (100/KEY_CHECK_PERIOD_MS)
#define KEY_PERIOD_30MS    (30/KEY_CHECK_PERIOD_MS)

#define KEY_PORT		(GPIOA)
#define KEY_1			(0)
#define KEY_2			(1)
#define KEY_3			(5)
#define KEY_4			(2)
#define KEY_5			(3)

#define KEY_OK			(1<<KEY_1)
#define KEY_LEFT		(1<<KEY_2)
#define KEY_DOWN		(1<<KEY_3)
#define KEY_RIGHT		(1<<KEY_4)
#define KEY_UP			(1<<KEY_5)

#define ANYKEY			(KEY_OK | KEY_LEFT | KEY_DOWN | KEY_RIGHT | KEY_UP)
#define KEY_MASK		(KEY_OK | KEY_LEFT | KEY_DOWN | KEY_RIGHT | KEY_UP)

#define KBD_LOCK		1
#define KBD_NOLOCK		0

#define KBD_DEFAULT_ART	((void *)0)

void
KeybProc( void );

void
ClrKeyb( int lock );

unsigned int
GetKeys( void );

unsigned int
KeysTime( void );

unsigned int
IsKeyPressed( unsigned int mask );

unsigned int
IsKey( unsigned int mask );

void
KeybLock( void );

void
KeybSetAutoRepeatTimes( unsigned short * AutoRepeatTab );

void
KeybInit( void );

#endif
