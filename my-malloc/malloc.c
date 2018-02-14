#include "malloc.h"

// Start of the heap
void *heap_start = NULL;

void *malloc(size_t size) {
	block start, last;
	size_t s;
	s = align8(size); // 8-byte alignment for every size

	if (heap_start) {
		last = heap_start;
		start = find_free_block(heap_start, &last, s); // finding the next free block
		if (start) {
			mlock(start, s); //locking the address as we are going to allocate the memory

			if ((start->size - s) >= (block_size() + 8)) {
			  printf("splitting block %zd, %zd", s, start->size);
 				split_block(start, s); // splitting the block if the requested memory can be allocated in a smaller block
			}
 			start->free = 0; // marking the memory region occupied
			munlock(start, s);
		} else {

	 		start = extend_heap(last, s); // No fitting block , extend the heap
			if (!start) {
				errno = ENOENT; //Error if no more memory can be allocated
	 			return(NULL);
			}
	 	}
	} else {
		 start = extend_heap(NULL, s); // extending the heap for the first time
		 if (!start) {
			 errno = ENOENT; // Error if no more memory can be allocated
	 	 	 return(NULL);
		 }
		 heap_start = start; // after allocation current block will become the heap_start as heap break will increase
 	}

	printf("Successfully allocated %zd bytes to address %p\n", s, start);
 	return(start->data); // returning the starting address of the block
}
