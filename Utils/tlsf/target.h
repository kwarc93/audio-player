#ifndef _TARGET_H_
#define _TARGET_H_

#ifdef USE_FREERTOS
#include "FreeRTOS/FreeRTOS.h"
#include "FreeRTOS/semphr.h"

#define TLSF_LOCK_TIMEOUT		1000

#define TLSF_MLOCK_T            SemaphoreHandle_t
#define TLSF_CREATE_LOCK(l)     do{l = xSemaphoreCreateMutex(); if(!l){asm volatile("BKPT 0");}}while(0)
#define TLSF_DESTROY_LOCK(l)    vSemaphoreDelete(l)
#define TLSF_ACQUIRE_LOCK(l)    if(xSemaphoreTake(l,TLSF_LOCK_TIMEOUT)!=pdTRUE){asm volatile("BKPT 0");}
#define TLSF_RELEASE_LOCK(l)    xSemaphoreGive(l)
#endif

#endif
