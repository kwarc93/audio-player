/*
 * joystick.c
 *
 *  Created on: 30.09.2017
 *      Author: Kwarc
 */

#include "keyboard/keyb.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/timers.h"
#include "FreeRTOS/semphr.h"
#include "joystick.h"
#include "ui/display.h"

#ifdef DEBUG
#include "debug.h"
#define DBG_MSG(...)	(Debug_Msg("[Joystick] " __VA_ARGS__))
#else
#define DBG_MSG(...)
#endif

static TaskHandle_t xHandleTaskJoystick;

// Keyboard timer & binary semaphore handle
static TimerHandle_t thJoystickTim;
static SemaphoreHandle_t shJoystickEvent;

static void vTimerCallback(TimerHandle_t xTimer)
{
	KeybProc();
	xSemaphoreGive(shJoystickEvent);
}

static void vTaskJoystick(void *pvParameters)
{
	uint8_t bar_lvl = 0;
	// Task's infinite loop
	for(;;)
	{
		if(xSemaphoreTake(shJoystickEvent, portMAX_DELAY))
		{
			switch(GetKeys())
			{
			case KEY_OK:
				ClrKeyb( KBD_LOCK );
				Display_SendText("PLAY");
				DBG_MSG("Key OK");
				break;
			case KEY_UP:
				ClrKeyb( KBD_LOCK );
				if(bar_lvl < 4) Display_SendBarLevel(++bar_lvl);
				Display_SendText("VOL+");
				DBG_MSG("Key UP");
				break;
			case KEY_DOWN:
				ClrKeyb( KBD_LOCK );
				if(bar_lvl > 0) Display_SendBarLevel(--bar_lvl);
				Display_SendText("VOL-");
				DBG_MSG("Key DOWN");
				break;
			case KEY_LEFT:
				ClrKeyb( KBD_LOCK );
				Display_SendText("PREV");
				DBG_MSG("Key LEFT");
				break;
			case KEY_RIGHT:
				ClrKeyb( KBD_LOCK );
				Display_SendText("NEXT");
				DBG_MSG("Key RIGHT");
				break;
			default: break;

			}
		}
	}
	/* Should never go here */
	vTaskDelete(xHandleTaskJoystick);
}

void Joystick_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Hardware init
	KeybInit();

	// Create and take binary semaphore
	vSemaphoreCreateBinary(shJoystickEvent);
	xSemaphoreTake(shJoystickEvent, 0);

	// Create timer for reading keyboard port (10ms period)
	thJoystickTim = xTimerCreate("TIMER", KEY_CHECK_PERIOD_MS/portTICK_PERIOD_MS, pdTRUE, (void*)0, vTimerCallback);
	xTimerStart(thJoystickTim, 0);

	// Create task for reading input button
	xTaskCreate(vTaskJoystick, "JOYSTICK", JOYSTICK_STACK_SIZE, NULL, uxPriority, &xHandleTaskJoystick);

	DBG_MSG("Task(s) started!");
}
