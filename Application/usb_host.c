// +--------------------------------------------------------------------------
// | @ Includes
// +--------------------------------------------------------------------------
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"
#include "FreeRTOS/semphr.h"

#include "usbh_conf.h"
#include "usb_host.h"
#include "usbh_core.h"
#include "usbh_msc.h"

#include "ui/display.h"
#include "FatFs/ffconf.h"
#include "FatFs/ff.h"
#include <stdbool.h>

// +--------------------------------------------------------------------------
// | @ Defines
// +--------------------------------------------------------------------------
#define TEST_FATFS		1
/* for FS and HS identification */
#define HOST_HS 		0
#define HOST_FS 		1

// +--------------------------------------------------------------------------
// | @ Public variables
// +--------------------------------------------------------------------------
/* USB Host Core handle */
USBH_HandleTypeDef hUsbHostFS;

// +--------------------------------------------------------------------------
// | @ Private variables
// +--------------------------------------------------------------------------
static struct context
{
	FATFS fs;
	USB_Host_State_t host_state;
	TaskHandle_t task_handle;

	_Bool event;
	_Bool disk_ready;
}ctx;

// +--------------------------------------------------------------------------
// | @ Private functions
// +--------------------------------------------------------------------------
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
#if TEST_FATFS
static _Bool FatFS_Test(void);
#endif

/* USB Host Background task */
static void USB_HOST_Process(void)
{
    USBH_Process(&hUsbHostFS);
}

/* Init function */
static void USB_HOST_Init(void)
{
  /* Init Host Library,Add Supported Class and Start the library*/
	ctx.host_state = USB_HOST_IDLE;
	USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS);
	USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);
	USBH_Start(&hUsbHostFS);
}

/* User USB callback */
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		ctx.event = true;
		break;

	case HOST_USER_DISCONNECTION:
		ctx.host_state = USB_HOST_DISCONNECT;
		USBH_DeInit(&hUsbHostFS);
		ctx.event = true;
		break;

	case HOST_USER_CLASS_ACTIVE:
		ctx.host_state = USB_HOST_READY;
		ctx.event = true;
		break;

	case HOST_USER_CONNECTION:
		ctx.host_state = USB_HOST_START;
		ctx.event = true;
		USB_HOST_Init();
		break;

	default:
		break;
	}
}

/* Internal process */
static void USBH_TaskProcess(void)
{
	USB_HOST_Process();

	if(ctx.event)
	{
		ctx.event = false;
		switch(ctx.host_state)
		{
		case USB_HOST_IDLE:
			break;

		case USB_HOST_DISCONNECT:
			ctx.disk_ready = false;
			DBG_SIMPLE("USBH disconnection event");
			Display_SendText("USB DISCONNECTED");
			f_mount(0, "", 0);
			break;

		case USB_HOST_READY:
			DBG_SIMPLE("USBH host ready");
			Display_SendText("USB READY");

			if(f_mount(&ctx.fs, "", 1) == FR_OK)
			{
			#if TEST_FATFS == 1
				FatFS_Test();
			#endif
				ctx.disk_ready = true;
			}
			break;

		case USB_HOST_START:
			DBG_SIMPLE("USBH connection event");
			Display_SendText("USB CONNECTED");
			break;

		default:
			break;
		}
	}
}

/* USB task */
static void vTaskUSB(void *pvParameters)
{
	TickType_t xLastFlashTime;
	// Read state of system counter
	xLastFlashTime = xTaskGetTickCount();

	// Task's infinite loop
	for(;;)
	{

		USBH_TaskProcess();

		// Delay 10ms
		vTaskDelayUntil( &xLastFlashTime, 10/portTICK_PERIOD_MS );
	}
	/* Should never go here */
	vTaskDelete(ctx.task_handle);
}

// +--------------------------------------------------------------------------
// | @ Public functions
// +--------------------------------------------------------------------------
void USB_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	ctx.disk_ready = false;
	ctx.event = false;
	USB_HOST_Init();

	// Creating task for USB
	if(xTaskCreate(vTaskUSB, "USB", USB_STACK_SIZE, NULL, uxPriority, &ctx.task_handle) == pdPASS)
	{
		DBG_SIMPLE("Task(s) started!");
	}
}

_Bool USB_IsDiskReady(void)
{
	return ctx.disk_ready;
}

// +--------------------------------------------------------------------------
// | @ Interrupt handlers
// +--------------------------------------------------------------------------
void OTG_FS_IRQHandler(void)
{
	HAL_HCD_IRQHandler(&hhcd);
}

/**
  * @}
  */

#if TEST_FATFS == 1
static _Bool FatFS_Test(void)
{
	FIL MyFile;                   								/* File object */
	FRESULT res;        		                        	 	/* FatFs function common result code */
	uint32_t byteswritten, bytesread;                    	 	/* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; 		/* File write buffer */
	uint8_t rtext[64];                                   		/* File read buffer */
	const char* fname = "STM32.TXT";							/* File name */

	/* Create and Open a new text file object with write access */
	if(f_open(&MyFile, fname, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		/* 'STM32.TXT' file Open for write Error */
		DBG_SIMPLE("FATFS ERROR - f_open error");
		return false;
	}
	else
	{
		/* Write data to the text file */
		res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

		if((byteswritten == 0) || (res != FR_OK))
		{
			/* 'STM32.TXT' file Write or EOF Error */
			DBG_SIMPLE("FATFS ERROR - f_write error");
			return false;
		}
		else
		{
			/* Close the open text file */
			f_close(&MyFile);

			/* Open the text file object with read access */
			if(f_open(&MyFile, fname, FA_READ) != FR_OK)
			{
				/* 'STM32.TXT' file Open for read Error */
				DBG_SIMPLE("FATFS ERROR - f_open error");
				return false;
			}
			else
			{
				/* Read data from the text file */
				res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

				if((bytesread == 0) || (res != FR_OK))
				{
					/* 'STM32.TXT' file Read or EOF Error */
					DBG_SIMPLE("FATFS ERROR - f_read error");
					return false;
				}
				else
				{
					/* Close the open text file */
					f_close(&MyFile);

					/* Compare read data with the expected data */
					if((bytesread != byteswritten))
					{
						/* Read data is different from the expected data */
						DBG_SIMPLE("FATFS ERROR");
						return false;
					}
					else
					{
						/* Delete file */
						if(f_unlink(fname) == FR_OK)
						{
							/* Success of the demo: no error occurrence */
							DBG_SIMPLE("FATFS OK");
							return true;
						}
						else
						{
							DBG_SIMPLE("FATFS ERROR - f_unlink error");
							return false;
						}

					}
				}
			}
		}
	}
}
#endif
