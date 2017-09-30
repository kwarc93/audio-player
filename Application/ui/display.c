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
#include "FreeRTOS/queue.h"
#include "display.h"
#include "debug.h"

#ifdef DEBUG
#define DBG_MSG(MSG)	(Debug_Msg("[DISPLAY] " MSG "\r\n"))
#else
#define DBG_MSG(MSG)	do{}while(0);
#endif

static TaskHandle_t xHandleTaskDisplay;
static QueueHandle_t qhDisplayText;
static QueueHandle_t qhDisplayBar;

static void vTaskDisplay(void * pvParameters)
{
	const uint32_t delay_ms = 300;
	uint8_t bar_lvl = 0;
	char* text = NULL;
	char display_buffer[64] = {0};
	uint32_t text_len = 0;

	// Task's infinite loop
	for(;;)
	{
		if(xQueueReceive(qhDisplayBar, &bar_lvl, 0))
		{
			LCD_DisplayBarLevel(bar_lvl);
		}

		if(xQueueReceive(qhDisplayText, &text, 0))
		{
			LCD_ClearText();
			text_len = strlen(text);

			if(text_len > LCD_DIGIT_MAX_NUMBER)
			{
				// Copy to buffer
				strcpy(display_buffer,text);
				// Add empty space to end of the text
				strcat(display_buffer, "  ");
			}
			else
			{
				LCD_DisplayString((uint8_t*)text);
			}
		}

		if(text_len > LCD_DIGIT_MAX_NUMBER)	// Scroll if string is longer than 6 characters
		{
			//@ TODO: Rewrite scrolling function to be non-blocking
			LCD_ScrollSentence((uint8_t*)display_buffer, 1, delay_ms);
		}
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskDisplay);
}


void Display_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init hardware
	LCD_Init();
	LCD_Contrast(LCD_CONTRASTLEVEL_5);

	 // Create queue for display text
	qhDisplayText = xQueueCreate(1, sizeof(char*));

	 // Create queue for display bar level
	qhDisplayBar = xQueueCreate(1, sizeof(uint8_t));

	// Creating task for LCD
	xTaskCreate(vTaskDisplay, "LCD", LCD_STACK_SIZE, NULL, uxPriority, &xHandleTaskDisplay);

	DBG_MSG("Task(s) started!");
}

void Display_SendText(char* text)
{
	if(!xQueueSend(qhDisplayText, (void*)&text, 0))
	{
		// Error!
		// Failed to send item to queue
	}
}

void Display_SendBarLevel(uint8_t lvl)
{
	if(!xQueueSend(qhDisplayBar, (void*)&lvl, 0))
	{
		// Error!
		// Failed to send item to queue
	}
}


