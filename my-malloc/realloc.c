#include "malloc.h"

void *realloc(void *p, size_t size) {
	size_t s;
	block b, new;
	void *newp = NULL;
	if (!p) // if pointer is null just allocate the memory to current break
		return (malloc(size));
	if (valid_address (heap_start, p)) { // Reallocate only if its a valid address, as we need to free the old address
		s = align8(size);
 		b = get_block(p);
 		if (b->size >= s) {
			if (b->size - s >= ( block_size() + 8)) { // Same as malloc, split if chunk is bigger than required memory
 				split_block (b, s);
			}
		} else {

			if (b->next && b->next->free && (b->size + block_size() + b->next->size) >= s) { // try joining the next free chunk
				join_free_chunks(b);
				if (b->size - s >= ( block_size() + 8)) {
					split_block (b, s);
				}
			} else {
					newp = malloc(s); // allocate the memory
					if (!newp) {
						errno = ENOENT;
			 			return (NULL);
					}
					new = get_block(newp);

					copy_block(b, new); // copy data from previous to new block
					free(p); // free the old one
					printf("Successfully re-allocated %zd bytes to address %p\n", s, newp);
					return (newp);
				}
    	}
			printf("Successfully re-allocated %zd bytes to address %p\n", s, newp);
 			return (p); // return the old one
		}
		errno = ENOENT;
		return (NULL);
}
