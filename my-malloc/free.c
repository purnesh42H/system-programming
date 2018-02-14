#include "malloc.h"

void free(void *p) {
	block b;

  // We have to check if the address is valid i.e. it is a pointer who has been already allocated the memory
	if (valid_address(heap_start, p)) {
		b = get_block(p); // get the block to free
		mlock(b, sizeof(b));
		b->free = 1;
		/* join previous chunk if possible */
		if (b->prev && b->prev->free) { // if previous block is also free join it with block b
			b = join_free_chunks(b->prev); // When we free a chunk, and its neighbors are also free, we can fusion them in one bigger chunk
		}
		if (b->next) {
			join_free_chunks(b); // When we free a chunk, and its neighbors are also free, we can fusion them in one bigger chunk
		} else {
			/* release memory from end of heap */
			if (b->prev) {
				b->prev->next = NULL;
			} else {
				heap_start = NULL;
			}
			brk(b); // assign the heap break after free operation
		}
		printf("Successfully free'd memory at address %p\n", p);
		munlock(b, sizeof(b));
	}
}
