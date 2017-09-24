/*
 * display.c
 *
 *  Created on: 24.09.2017
 *      Author: Kwarc
 */

#include <string.h>
#include "lcd/lcd.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "display.h"

static TaskHandle_t xHandleTaskLCD;

void vTaskLCD(void * pvParameters)
{
	const char* text = "MP3 PLAYER  ";
	const uint32_t delay_ms = 300;
	uint8_t bar_lvl = 0;
	// Task's infinite loop
	for(;;)
	{
		if(strlen(text) > LCD_DIGIT_MAX_NUMBER)	// Scroll if string is longer than 6 characters
		{
			LCD_ScrollSentence((uint8_t*)text, 1, delay_ms);
		}
		else
		{
			LCD_DisplayString((uint8_t*)text);
			vTaskDelay(delay_ms);
		}

		bar_lvl++;
		if(bar_lvl > BARLEVEL_FULL) bar_lvl = 0;
		LCD_DisplayBarLevel(bar_lvl);

	}
	/* Should never go there */
	vTaskDelete(xHandleTaskLCD);
}


void vStartLCDTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init hardware
	LCD_Init();
	LCD_Contrast(LCD_CONTRASTLEVEL_5);

	// Creating task for LCD
	xTaskCreate(vTaskLCD, "LCD", LCD_STACK_SIZE, NULL, uxPriority, &xHandleTaskLCD);

}



