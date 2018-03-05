#include "malloc.h"

void free(void *p) {
	block b;
  // We have to check if the address is valid i.e. it is a pointer who has been already allocated the memory
	if (valid_address(heap_start, p)) {
		b = get_block(p); // get the block to free
		if (b->size > THRESHOLD) {
			if (b->prev) {
				b->prev->next = b->next;
			} else {
				mmap_start = NULL;
			}
			b->free = 1;
			munmap(b, b->size);
		} else {
			if(mlock(b, b->size) == 0) {
				b->free = 1;
				/* join previous chunk if possible */
				if ((b->prev && b->prev->free)) { // if previous block is also free or next block join it with block b
					b = buddy_join(b->prev);
					munlock(b, b->size);		  // When we free a chunk, and its neighbors are also free, we can fusion them in one bigger chunk
				}
				if (b->next) {
					b = buddy_join(b); // We have to fusion with next chunk
					munlock(b, b->size);
				} else {

					/* release memory from end of heap */
					if (b->prev) {
						b->prev->next = NULL;
					} else {
						heap_start = NULL;
					}
					munlock(b, b->size);
					brk(b); // assign the heap break after free operation
				}
			}
		}
	}
}
