/*
 * display.c
 *
 *  Created on: 24.09.2017
 *      Author: Kwarc
 */

#include <string.h>
#include "misc.h"
#include "lcd/lcd.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"
#include "display.h"

#ifdef DEBUG
#include "debug.h"
#define DBG_PRINTF(...)	(Debug_Printf("[DISPLAY] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

static TaskHandle_t xHandleTaskDisplay;
static QueueHandle_t qhDisplayText;
static QueueHandle_t qhDisplayBar;

static void vTaskDisplay(void * pvParameters)
{
	const uint32_t delay_ms = 50;
	const uint32_t scroll_delay = 4*delay_ms;
	uint32_t task_tick = 0;
	uint8_t q_bar_lvl = 0;
	char* q_text = NULL;
	char display_buffer[64] = "";
	uint32_t text_len = 0;
	_Bool is_new_text = 0;

	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();

	// Task's infinite loop
	for(;;)
	{
		/* Receive and display bar level */
		if(xQueueReceive(qhDisplayBar, &q_bar_lvl, 0))
		{
			LCD_DisplayBarLevel(q_bar_lvl);
		}

		/* Receive and display text */
		if(xQueueReceive(qhDisplayText, &q_text, 0))
		{
			is_new_text = true;
			text_len = strlen(q_text);

			LCD_ClearText();

			if(text_len > LCD_DIGIT_MAX_NUMBER && text_len < ARRAY_LEN(display_buffer))
			{
				// Copy to buffer
				strcpy(display_buffer,q_text);
				// Add empty space to end of the text
				strcat(display_buffer, "  ");
			}
			else
			{
				LCD_DisplayString((uint8_t*)q_text);
			}
		}

		if(text_len > LCD_DIGIT_MAX_NUMBER)	// Scroll if string is longer than 6 characters
		{
			if(task_tick > scroll_delay)
			{
				LCD_ScrollSentence((uint8_t*)display_buffer, is_new_text);
				task_tick = 0;
				is_new_text = false;
			}
		}

		task_tick += delay_ms;

		// Delay
		vTaskDelayUntil( &xLastFlashTime, delay_ms/portTICK_PERIOD_MS );
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

	// Display init text
	Display_SendText("STM32L476 MP3 PLAYER");

	// Creating task for LCD
	if(xTaskCreate(vTaskDisplay, "LCD", LCD_STACK_SIZE, NULL, uxPriority, &xHandleTaskDisplay) == pdPASS)
	{
		DBG_PRINTF("Task(s) started!");
	}
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


