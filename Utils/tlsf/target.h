#ifndef _TARGET_H_
#define _TARGET_H_

#ifdef USE_FREERTOS
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"

#define TLSF_MLOCK_T            SemaphoreHandle_t
#define TLSF_CREATE_LOCK(l)     *(l) = xSemaphoreCreateMutex()
#define TLSF_DESTROY_LOCK(l)    vSemaphoreDelete(l)
#define TLSF_ACQUIRE_LOCK(l)    xSemaphoreTake(l,0)
#define TLSF_RELEASE_LOCK(l)    xSemaphoreGive(l)
#endif


#endif
