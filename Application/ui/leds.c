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

static TaskHandle_t xHandleTaskLEDG, xHandleTaskLEDR;

static inline void vhToggleLEDG(void)
{
	GPIOE->ODR ^= GPIO_ODR_ODR_8;
}

static inline void vhToggleLEDR(void)
{
	GPIOB->ODR ^= GPIO_ODR_ODR_2;
}

void vTaskLEDG(void * pvParameters)
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for(;;)
	{
		// Delay 250ms
		vTaskDelayUntil( &xLastFlashTime, 250/portTICK_PERIOD_MS );
		// Change LED state
		vhToggleLEDG();
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskLEDG);
}

void vTaskLEDR(void * pvParameters)
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();
	// Task's infinite loop
	for(;;)
	{
		// Delay 250ms
		vTaskDelayUntil( &xLastFlashTime, 500/portTICK_PERIOD_MS );
		// Change LED state
		vhToggleLEDR();
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskLEDR);
}

void vStartLEDTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init hardware
	RCC->AHB2ENR |=  RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOEEN;
	__DSB();
	GPIO_PinConfig(GPIOB,2,GPIO_OUT_PP_2MHz);
	GPIO_PinConfig(GPIOE,8,GPIO_OUT_PP_2MHz);

	// Creating task for LED blinking
	xTaskCreate(vTaskLEDG, "LEDG", LED_STACK_SIZE, NULL, uxPriority, &xHandleTaskLEDG);
	xTaskCreate(vTaskLEDR, "LEDR", LED_STACK_SIZE, NULL, uxPriority, &xHandleTaskLEDR);

}