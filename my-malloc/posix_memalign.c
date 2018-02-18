#include "malloc.h"

#define VOID_SIZE (sizeof(void *))

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	if (alignment % 2 != 0 && alignment % VOID_SIZE != 0) {
		return EINVAL;
	}

	*memptr = malloc(size);
	if (memptr) {
 		return (0); // returning the starting address of the block
	}
	return ENOMEM;
}