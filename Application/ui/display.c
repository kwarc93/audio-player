/*
 * display.c
 *
 *  Created on: 24.09.2017
 *      Author: Kwarc
 */

#include "misc.h"
#include "ssd1306/ssd1306.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/queue.h"
#include "display.h"

#include <string.h>
#include <stdbool.h>

#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[DISPLAY] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

static TaskHandle_t xHandleTaskDisplay;
static QueueHandle_t qhDisplayText;
static QueueHandle_t qhDisplayBar;

static void vTaskDisplay(void * pvParameters)
{
	const uint32_t task_delay_ms = 50;
	uint32_t task_tick = 0;
	uint8_t q_bar_lvl = 0;
	char* q_text = NULL;

	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();

	// Task's infinite loop
	for(;;)
	{
		/* Receive and display bar level */
		if(xQueueReceive(qhDisplayBar, &q_bar_lvl, 0))
		{
//			LCD_DisplayBarLevel(q_bar_lvl);
		}

		/* Receive and display text */
		if(xQueueReceive(qhDisplayText, &q_text, 0))
		{
			clr_buff();
			write_string(0,0,q_text);
			show_buff();
		}

		task_tick += task_delay_ms;

		// Delay
		vTaskDelayUntil( &xLastFlashTime, task_delay_ms/portTICK_PERIOD_MS );
	}
	/* Should never go there */
	vTaskDelete(xHandleTaskDisplay);
}


void Display_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init hardware
	SSD1306_Init();

	 // Create queue for display text
	qhDisplayText = xQueueCreate(1, sizeof(char*));

	 // Create queue for display bar level
	qhDisplayBar = xQueueCreate(1, sizeof(uint8_t));

	// Display init text
	Display_SendText("AUDIO PLAYER");

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


