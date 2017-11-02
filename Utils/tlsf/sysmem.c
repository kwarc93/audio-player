/*
 * Link-time interposition of malloc and free using the static
 * linker's (ld) "--wrap symbol" flag.
 *
 * Compile the executable using "-Wl,--wrap,malloc -Wl,--wrap,free -Wl,--wrap,calloc -Wl,--wrap,realloc".
 * This tells the linker to resolve references to malloc as __wrap_malloc, free as __wrap_free,
 *  calloc as __wrap_calloc and realloc as __wrap_realloc
 */
#include <stdio.h>
#include "tlsf/tlsf.h"

void* __real_malloc(size_t size);
void  __real_free(void *ptr);
void* __real_calloc(size_t nelem, size_t elem_size);
void* __real_realloc(void *ptr, size_t size);

/*
 * __wrap_malloc - malloc wrapper function
 */
void *__wrap_malloc(size_t size)
{
	return tlsf_malloc(size);
}

/*
 * __wrap_free - free wrapper function
 */
void __wrap_free(void *ptr)
{
    tlsf_free(ptr);
}

/*
 * __wrap_calloc - calloc wrapper function
 */
void *__wrap_calloc(size_t nelem, size_t elem_size)
{
    return tlsf_calloc(nelem, elem_size);
}

/*
 * __wrap_relloc - realloc wrapper function
 */
void *__wrap_realloc(void *ptr, size_t size)
{
	return tlsf_realloc(ptr, size);
}
