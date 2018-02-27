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
#include "ui/menu_manager.h"

#include <string.h>
#include <stdbool.h>

#include "debug.h"
#if DEBUG
#define DBG_PRINTF(...)	(Debug_Printf("[DISPLAY] " __VA_ARGS__))
#else
#define DBG_PRINTF(...)
#endif

extern const uint8_t* const system8_array[];
extern const uint8_t image_data_play[];

static TaskHandle_t xHandleTaskDisplay;
static QueueHandle_t qhDisplayText;

static void vTaskDisplay( void * pvParameters )
{
	const uint32_t task_delay_ms = 50;
	uint32_t task_tick = 0;
	char* q_text = NULL;

	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();

	// Display init screen
	SSD1306_Clear( false );
	SSD1306_DrawBitmap( 0, 0, image_data_play, false );
	SSD1306_CpyDirtyPages();

	vTaskDelay( 1500 );
	Menu_Show();

	// Task's infinite loop
	for( ;; )
	{
		/* Receive and display text */
		if( xQueueReceive( qhDisplayText, &q_text, 0 ) )
		{
			SSD1306_Clear( 0 );
			SSD1306_SetText( 0, 0, q_text, system8_array, 0 );
			SSD1306_CpyDirtyPages();
		}

		task_tick += task_delay_ms;

		// Delay
		vTaskDelayUntil( &xLastFlashTime, task_delay_ms / portTICK_PERIOD_MS );
	}
	/* Should never go there */
	vTaskDelete( xHandleTaskDisplay );
}

void Display_StartTasks( unsigned portBASE_TYPE uxPriority )
{
	// Init hardware
	SSD1306_Init();

	MenuManager_CreateMenu();

	// Create queue for display text
	qhDisplayText = xQueueCreate( 1, sizeof(char*) );

	// Creating task for LCD
	if( xTaskCreate( vTaskDisplay, "LCD", LCD_STACK_SIZE, NULL, uxPriority,
			&xHandleTaskDisplay ) == pdPASS )
	{
		DBG_PRINTF( "Task(s) started!" );
	}
}

void Display_SendText( char* text )
{
	if( !xQueueSend( qhDisplayText, (void* )&text, 0 ) )
	{
		// Error!
		// Failed to send item to queue
	}
}
