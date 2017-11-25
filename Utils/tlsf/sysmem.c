
#include <stdio.h>
#include <malloc.h>
#include "tlsf/tlsf.h"

/*
 * malloc wrapper function
 */
void *malloc(size_t size)
{
	return tlsf_malloc(size);
}

/*
 * free wrapper function
 */
void free(void *ptr)
{
    tlsf_free(ptr);
}

/*
 * calloc wrapper function
 */
void *calloc(size_t nelem, size_t elem_size)
{
    return tlsf_calloc(nelem, elem_size);
}

/*
 * realloc wrapper function
 */
void *realloc(void *ptr, size_t size)
{
	return tlsf_realloc(ptr, size);
}
