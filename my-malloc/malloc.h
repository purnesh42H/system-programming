#ifndef MALLOC_H_
#define MALLOC_H_
#include "utils.h"

/* The function returns a fitting chunk, or NULL if none where found. After the execution, the
   argument last points to the last visited chunk. */
void *malloc(size_t size);
void *calloc(size_t count, size_t size);
void free(void *ptr);
void *realloc(void *p, size_t size);
int posix_memalign(void **memptr, size_t alignment, size_t size);
void *memalign(size_t alignment, size_t size);

extern void *heap_start;

#endif
