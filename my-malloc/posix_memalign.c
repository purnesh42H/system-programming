#include "malloc.h"

#define VOID_SIZE (sizeof(void *))

int posix_memalign(void **memptr, size_t alignment, size_t size) {
	if (alignment % 2 != 0 && alignment % VOID_SIZE != 0) {
		return EINVAL;
	}

	block start, last;
	size_t s = alignn(size, alignment);

  // From here it is same as malloc
	if (heap_start) {
		/* First find a block */
		last = heap_start;
		start = find_free_block(heap_start, &last, s);

		if (start) {
			/* can we split */
			if ((start->size - s) >= (block_size() + alignment)) {
			  printf("splitting block %zd, %zd", s, start->size);
 				split_block(start, s);
			}
 			start->free = 0;
		} else {
	 		/* No fitting block , extend the heap */
	 		start = extend_heap(last, s);
			if (!start) {
	 			*memptr = NULL;
				return ENOMEM;
			}
	 	}
	} else {
	 	 /* first time */
		 start = extend_heap(NULL, s);
		 if (!start) {
	 	 	 *memptr = NULL;
			 return ENOMEM;
		 }

		 heap_start = start;
 	}

 	*memptr = start->data;
	return 0;
}
