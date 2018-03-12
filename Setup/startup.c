/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------
// This module contains the startup code for a portable embedded
// C/C++ application, built with newlib.
//
// Control reaches here from the reset handler via jump or call.
//
// The actual steps performed by _start are:
// - copy the initialised data region(s)
// - clear the BSS region(s)
// - initialise the system
// - run the preinit/init array (for the C++ static constructors)
// - initialise the arc/argv
// - branch to main()
// - run the fini array (for the C++ static destructors)
// - call _exit(), directly or via exit()
//
// If OS_INCLUDE_STARTUP_INIT_MULTIPLE_RAM_SECTIONS is defined, the
// code is capable of initialising multiple regions.
//
// The normal configuration is standalone, with all support
// functions implemented locally.
//
// ----------------------------------------------------------------------------
#include <stdint.h>
#include <sys/types.h>

// ----------------------------------------------------------------------------

// Begin address for the initialisation values of the .data section.
// defined in linker script
extern unsigned int __data_init_start;
// Begin address for the .data section; defined in linker script
extern unsigned int __data_start;
// End address for the .data section; defined in linker script
extern unsigned int __data_end;

// Begin address for the .bss section; defined in linker script
extern unsigned int __bss_start;
// End address for the .bss section; defined in linker script
extern unsigned int __bss_end;

// main() is the entry point for newlib based applications.
// By default, there are no arguments, but this can be customised
// by redefining __initialize_args(), which is done when the
// semihosting configurations are used.
extern int
main( void );

// The implementation for the exit routine; for embedded
// applications, a system reset will be performed.
extern void
__attribute__((noreturn))
_exit( int );

// ----------------------------------------------------------------------------

// Forward declarations

void
Reset_Handler( void );

void
__initialize_hardware_early( void );

void
__initialize_hardware( void );

void
__initialize_data( unsigned int* from, unsigned int* region_begin, unsigned int* region_end );

void
__initialize_bss( unsigned int* region_begin, unsigned int* region_end );

extern void
__libc_init_array( void );

extern void
__libc_fini_array( void );

void
__initialize_hardware_early( void );

void
__initialize_hardware( void );

// ----------------------------------------------------------------------------

inline void
__attribute__((always_inline))
__initialize_data( unsigned int* from, unsigned int* region_begin, unsigned int* region_end )
{
	// Iterate and copy word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while( p < region_end )
		*p++ = *from++;
}

inline void
__attribute__((always_inline))
__initialize_bss( unsigned int* region_begin, unsigned int* region_end )
{
	// Iterate and clear word by word.
	// It is assumed that the pointers are word aligned.
	unsigned int *p = region_begin;
	while( p < region_end )
		*p++ = 0;
}


// This is the second hardware initialisation routine, it can be
// redefined in the application for more complex cases that
// require custom inits (before constructors), otherwise these can
// be done in main().
//
// Called from Reset_Handler(), right after data & bss init, before
// constructors.

void
__attribute__((weak))
__initialize_hardware( void )
{

}

// This is the early hardware initialisation routine, it can be
// redefined in the application for more complex cases that
// require early inits (before BSS init).
//
// Called early from Reset_Handler(), right before data & bss init.
//
// After Reset the Cortex-M processor is in Thread mode,
// priority is Privileged, and the Stack is set to Main.

void
__attribute__((weak))
__initialize_hardware_early( void )
{

}

// This is the place where Cortex-M core will go immediately after reset,
// via a call or jump from the Reset_Handler.
//
// For the call to work, and for the call to __initialize_hardware_early()
// to work, the reset stack must point to a valid internal RAM area.

void Reset_Handler( void )
{

	// Initialise hardware right after reset, to switch clock to higher
	// frequency and have the rest of the initialisations run faster.
	//
	// Mandatory on platforms like Kinetis, which start with the watch dog
	// enabled and require an early sequence to disable it.
	//
	// Also useful on platform with external RAM, that need to be
	// initialised before filling the BSS section.

	__initialize_hardware_early();

	// Copy the DATA segment from Flash to RAM (inlined).
	__initialize_data( &__data_init_start, &__data_start, &__data_end );

	// Zero fill the BSS section (inlined).
	__initialize_bss( &__bss_start, &__bss_end );

	// Hook to continue the initialisations. Usually compute and store the
	// clock frequency in the global CMSIS variable, cleared above.
	__initialize_hardware();

	// Call the standard library initialisation (mandatory for C++ to
	// execute the constructors for the static objects).
	__libc_init_array();

	// Call the main entry point, and save the exit code.
	int code = main();

	// Run the C++ static destructors.
	__libc_fini_array();

	_exit( code );

	// Should never reach this, _exit() should have already
	// performed a reset.
	while(1);
}

// ----------------------------------------------------------------------------
