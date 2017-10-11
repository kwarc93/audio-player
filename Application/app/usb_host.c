

/* Includes ------------------------------------------------------------------*/
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

#define TEST_FATFS		0

/* for FS and HS identification */
#define HOST_HS 		0
#define HOST_FS 		1

/* USB Host Core handle declaration */
USBH_HandleTypeDef hUsbHostFS;
static _Bool usb_class_active = false;
static USB_Host_State_t USB_HostState;

static TaskHandle_t xHandleTaskUSB;

static _Bool USBEvent = false;
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id);
static void FatFS_TestApplication(void);

/*
 * Background task
*/
static void USB_HOST_Process(void)
{
  /* USB Host Background task */
    USBH_Process(&hUsbHostFS);
}

/* init function */				        
static void USB_HOST_Init(void)
{
  /* Init Host Library,Add Supported Class and Start the library*/
	USB_HostState = USB_HOST_IDLE;
	USBH_Init(&hUsbHostFS, USBH_UserProcess, HOST_FS);
	USBH_RegisterClass(&hUsbHostFS, USBH_MSC_CLASS);
	USBH_Start(&hUsbHostFS);
}

/*
 * user callback definition
*/ 
static void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
	switch(id)
	{
	case HOST_USER_SELECT_CONFIGURATION:
		USBEvent = true;
		break;

	case HOST_USER_DISCONNECTION:
		USB_HostState = USB_HOST_DISCONNECT;
		USBH_DeInit(&hUsbHostFS);
		USBEvent = true;
		break;

	case HOST_USER_CLASS_ACTIVE:
		USB_HostState = USB_HOST_READY;
		USBEvent = true;
		break;

	case HOST_USER_CONNECTION:
		USB_HostState = USB_HOST_START;
		USBEvent = true;
		USB_HOST_Init();
		break;

	default:
		break;
	}
}

/*
 * internal process
*/
static void USBH_TaskProcess(void)
{
	USB_HOST_Process();

	if(USBEvent)
	{
		USBEvent = false;
		switch(USB_HostState)
		{
		case USB_HOST_IDLE:
			break;

		case USB_HOST_DISCONNECT:
			usb_class_active = false;
			DBG_SIMPLE("USBH disconnection event");
			Display_SendText("USB DISCONNECTED");
			break;

		case USB_HOST_READY:
			usb_class_active = true;
			DBG_SIMPLE("USBH host ready");
			Display_SendText("USB READY");
#if TEST_FATFS == 1
			FatFS_TestApplication();
#endif
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
	vTaskDelete(xHandleTaskUSB);
}

void USB_StartTasks(unsigned portBASE_TYPE uxPriority)
{
	// Init
	USB_HOST_Init();

	// Creating task for USB
	if(xTaskCreate(vTaskUSB, "USB", USB_STACK_SIZE, NULL, uxPriority, &xHandleTaskUSB) == pdPASS)
	{
		DBG_SIMPLE("Task(s) started!");
	}
}

_Bool USB_IsClassActive(void)
{
	return usb_class_active;
}

void OTG_FS_IRQHandler(void)
{
	HAL_HCD_IRQHandler(&hhcd);
}

/**
  * @}
  */

#if TEST_FATFS == 1
static void FatFS_TestApplication(void)
{
	FATFS fs0;
	FIL MyFile;                   								/* File object */
	volatile FRESULT res;                                	 	/* FatFs function common result code */
	uint32_t byteswritten, bytesread;                    	 	/* File write/read counts */
	uint8_t wtext[] = "This is STM32 working with FatFs"; 		/* File write buffer */
	uint8_t rtext[64];                                   		/* File read buffer */

	res = f_mount(&fs0, "0:", 1);

	/* Create and Open a new text file object with write access */
	if(f_open(&MyFile, "0:STM32.TXT", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK)
	{
		/* 'STM32.TXT' file Open for write Error */
		DBG_SIMPLE("FATFS ERROR - file open error");
	}
	else
	{
		/* Write data to the text file */
		res = f_write(&MyFile, wtext, sizeof(wtext), (void *)&byteswritten);

		if((byteswritten == 0) || (res != FR_OK))
		{
			/* 'STM32.TXT' file Write or EOF Error */
			DBG_SIMPLE("FATFS ERROR - file write error");
		}
		else
		{
			/* Close the open text file */
			f_close(&MyFile);

			/* Open the text file object with read access */
			if(f_open(&MyFile, "0:STM32.TXT", FA_READ) != FR_OK)
			{
				/* 'STM32.TXT' file Open for read Error */
				DBG_SIMPLE("FATFS ERROR - file open error");
			}
			else
			{
				/* Read data from the text file */
				res = f_read(&MyFile, rtext, sizeof(rtext), (void *)&bytesread);

				if((bytesread == 0) || (res != FR_OK))
				{
					/* 'STM32.TXT' file Read or EOF Error */
					DBG_SIMPLE("FATFS ERROR - file read error");
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
					}
					else
					{
						/* Success of the demo: no error occurrence */
						DBG_SIMPLE("FATFS OK");

					}
				}
			}
		}
	}
}
#endif
