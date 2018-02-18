#include "malloc.h"

// Start of the heap
void *heap_start = NULL;
void *mmap_start = NULL;

const int THRESHOLD = 2048;


block mmap_malloc(size_t s);

void *malloc(size_t size) {
	block start, last;
	size_t s;
	s = align8(size); // 8-byte alignment for every size

	if (s > THRESHOLD) {
		start = mmap_malloc(s);
	} else if (heap_start) {
		last = heap_start;
		start = find_free_block(heap_start, &last, s); // finding the next free block
		if (start) {
			if (mlock(start, s) == 0) {
				if (start->buddy_order > get_buddy_order(s)) {
				  printf("splitting block %zd to %zd\n", s, start->size);
	 				split_block(start, s); // splitting the block if the requested memory can be allocated in a smaller block
				}
	 			start->free = 0; // marking the memory region occupied
				munlock(start, s);
			} else {
				errno = ENOMEM; // Error if no more memory can be allocated
 	 	 	 	return(NULL);
			}
		} else {
	 		start = extend_heap(last, s); // No fitting block , extend the heap
			printf("No fitting block , extend the heap\n");
			if (!start) {
				errno = ENOMEM; //Error if no more memory can be allocated
	 			return(NULL);
			} else {
				if (start->buddy_order == get_buddy_order(s)) {
					start->free = 0;
				} else {
					printf("splitting block %zd to %zd\n", s, start->size);
	 				split_block(start, s); // splitting the block if the requested memory can be allocated in a smaller block
					start->free = 0;
				}
			}
	 	}
	} else {
		 start = extend_heap(NULL, s); // extending the heap for the first time
		 if (!start) {
			 errno = ENOMEM; // Error if no more memory can be allocated
	 	 	 return(NULL);
		 } else {
			 if (start->buddy_order == get_buddy_order(s)) {
				 start->free = 0;
			 } else {
				 printf("splitting block %zd-%zd\n", s, start->size);
				 split_block(start, s); // splitting the block if the requested memory can be allocated in a smaller block
				 start->free = 0;
			 }
		 }
		 heap_start = start; // after allocation current block will become the heap_start as heap break will increase
 	}

 	return(start->data); // returning the starting address of the block
}

block mmap_malloc(size_t s) {
	block new;
	void *addr = mmap(0, s, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_LOCKED, -1, 0);
	if (addr == MAP_FAILED) {
		errno = ENOMEM; //Error if no more memory can be allocated
		return(NULL);
	}

	new = insert_block(mmap_start, (block)addr, s);
	if (!mmap_start) {
		mmap_start = new;
	}

	return (new);
}
