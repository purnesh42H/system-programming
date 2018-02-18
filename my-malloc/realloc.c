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
		if ((size < THRESHOLD || b->size < THRESHOLD) && b->size >= s) {

			if (mlock(b, s) == 0) {
				if (b->buddy_order > get_buddy_order(s)) { // Same as malloc, split if chunk is bigger than required memory
	 				split_block (b, s);
				}
				b->free = 0;
				munlock(b, s);
			} else {
				errno = ENOMEM; // Error if no more memory can be allocated
 	 	 	 	return(NULL);
			}
		} else {
			if ((size < THRESHOLD || b->size < THRESHOLD) && b->next && b->next->free && (b->size + block_size() + b->next->size) >= s) { // try joining the next free chunk
				if (mlock(b, s) == 0) {
					buddy_join(b);
					if (b->buddy_order > get_buddy_order(s)) {
						split_block (b, s);
					}
					b->free = 0;
					munlock(b, s);
				}
			} else {
					newp = malloc(s); // allocate the memory
					if (!newp) {
						errno = ENOMEM;
			 			return (NULL);
					}
					new = get_block(newp);
					if(mlock(new, s) == 0) {
						copy_block(b, new); // copy data from previous to new block
						munlock(new, s);
					} else {
						errno = ENOMEM; // Error if no more memory can be allocated
		 	 	 	 	return(NULL);
					}
					free(p); // free the old one
					return (newp);
				}
    	}
 			return (p); // return the old one
		}
		errno = ENOMEM;
		return (NULL);
}
