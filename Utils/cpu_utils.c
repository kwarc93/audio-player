/*
 * cpu_utils.c
 *
 *  Created on: 20.05.2017
 *      Author: Kwarc
 */

/********************** NOTES **********************************************
To use this module, the following steps should be followed :

1- in the _OS_Config.h file (ex. FreeRTOSConfig.h) enable the following macros :
      - #define configUSE_IDLE_HOOK        1
      - #define configUSE_TICK_HOOK        1

2- in the _OS_Config.h define the following macros :
      - #define traceTASK_SWITCHED_IN()  extern void StartIdleMonitor(void); \
                                         StartIdleMonitor()
      - #define traceTASK_SWITCHED_OUT() extern void EndIdleMonitor(void); \
                                         EndIdleMonitor()
*******************************************************************************/
#include "cpu_utils.h"
#include "stm32l476xx.h"
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/task.h"


TaskHandle_t hIdleTask = NULL;

unsigned int CPU_IdleStartTime = 0;
unsigned int CPU_IdleSpentTime = 0;
unsigned int CPU_TotalIdleTime = 0;

volatile unsigned int CPU_Usage = 0;

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Application Idle Hook
  * @param  None
  * @retval None
  */
void vApplicationIdleHook(void)
{
  /* Idle task */
  if( hIdleTask == NULL )
  {
    /* Store the handle to the idle task. */
    hIdleTask = xTaskGetCurrentTaskHandle();
  }

  /* Wait for interrupt */
  __WFI();
}

/**
  * @brief  Application Idle Hook
  * @param  None
  * @retval None
  */
void vApplicationTickHook (void)
{
  static int tick = 0;

  if(tick ++ > CALCULATION_PERIOD)
  {
    tick = 0;

    if(CPU_TotalIdleTime > 1000)
    {
      CPU_TotalIdleTime = 1000;
    }
    CPU_Usage = (100 - (CPU_TotalIdleTime * 100) / CALCULATION_PERIOD);
    CPU_TotalIdleTime = 0;
  }
}

/**
  * @brief  Start Idle monitor
  * @param  None
  * @retval None
  */
void StartIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == hIdleTask )
  {
    CPU_IdleStartTime = xTaskGetTickCountFromISR();
  }
}

/**
  * @brief  Stop Idle monitor
  * @param  None
  * @retval None
  */
void EndIdleMonitor (void)
{
  if( xTaskGetCurrentTaskHandle() == hIdleTask )
  {
    /* Store the handle to the idle task. */
    CPU_IdleSpentTime = xTaskGetTickCountFromISR() - CPU_IdleStartTime;
    CPU_TotalIdleTime += CPU_IdleSpentTime;
  }
}

/**
  * @brief  Stop Idle monitor
  * @param  None
  * @retval None
  */
unsigned short int Get_CPU_Usage (void)
{
  return (unsigned short int)CPU_Usage;
}


/****END OF FILE****/
