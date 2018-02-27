/*
 * leds.c
 *
 *  Created on: 23.09.2017
 *      Author: Kwarc
 */

#include "gpio/gpio.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "leds.h"

#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[LED] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

static TaskHandle_t xHandleTaskLEDG, xHandleTaskLEDR;

#define LEDG_PIN		(8)
#define LEDR_PIN		(2)
#define LEDG_PORT		(GPIOE)
#define LEDR_PORT		(GPIOB)

inline void LED_ToggleGreen( void )
{
	LEDG_PORT->ODR ^= (1 << LEDG_PIN);
}

inline void LED_ToggleRed( void )
{
	LEDR_PORT->ODR ^= (1 << LEDR_PIN);
}

inline void LED_SetGreen( _Bool state )
{
	LEDG_PORT->BSRR = state ? (1 << LEDG_PIN) : (1 << (LEDG_PIN + 16));
}

inline void LED_SetRed( _Bool state )
{
	LEDR_PORT->BSRR = state ? (1 << LEDR_PIN) : (1 << (LEDR_PIN + 16));
}

static void vTaskLEDR( void * pvParameters )
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for( ;; )
	{
		// Delay ms
		vTaskDelayUntil( &xLastFlashTime, 1000 / portTICK_PERIOD_MS );
		// Change LED state
		LED_ToggleRed();
	}
	/* Should never go there */
	vTaskDelete( xHandleTaskLEDR );
}

void Led_StartTasks( unsigned portBASE_TYPE uxPriority )
{
	BaseType_t result;
	// Init
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	__DSB();
	GPIO_PinConfig( LEDG_PORT, LEDG_PIN, GPIO_OUT_PP_2MHz );
	GPIO_PinConfig( LEDR_PORT, LEDR_PIN, GPIO_OUT_PP_2MHz );

	// Creating task for LED blinking
	result = xTaskCreate( vTaskLEDR, "LEDR", LED_STACK_SIZE, NULL, uxPriority, &xHandleTaskLEDR );

	if( result == pdPASS )
	{
		DBG_PRINTF( "Task(s) started!" );
	}
}
