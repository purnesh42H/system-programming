# Overview
 A malloc library that provides the following dynamic memory allocation routines (as defined in man 3 malloc):
 
- void *malloc(size_t size);
- void free(void *ptr);
- void *calloc(size_t nmemb, size_t size);
- void *realloc(void *ptr, size_t size);

- int posix_memalign(void **memptr, size_t alignment, size_t size);
- void *memalign(size_t alignment, size_t size);

# How to run
- Existing tests
  - There are two tests
      - [Simple test](test1.c). run "make check"
      - [Benchmark test](t-test1.c). run "make check1"
      
- Create your test
  - Include the my malloc header file (# include "malloc.h")
  - Use the way you use standard malloc
  - Run the "make" to create the shared library i.e. (.so) file
  - run your test program using LD_PRELOAD=`pwd`/libmalloc.so <your output file>. For eg: LD_PRELOAD=`pwd`/libmalloc.so ./test1
  
# Design
- Code Strucure
	- There are two header files utils.h and malloc.h
	- Utils.h contains all the helper functions like aligning the size, splitting blocks etc.
	- malloc.h contains the functions of malloc library malloc, calloc, realloc, free, memalign, posix_memalign
	- Each of the malloc functions are in separate .c files
	
- Struct to hold the block information i.e. meta information about the allocated memory
```c
struct memory_block {
	size_t size; 	// size of the block or the requested memory(8-byte aligned)
	block next;  	// Address of the next block. It is important because we want to 
              	     	// traverse from one block to another to find the free block and also join the 
            		// free blocks if they are adjacent to each other to decrease fregmentation. I jump from meta to meta.
	block prev; 	// Address of the previous block. This is very important during free to combine adjacent free blocks
	int free; 	// To check if the block is free i.e. we can allocate the new memory here as it is not mapped to any.
	void *ptr; 	// This pointer stores the address of the memory block's data i.e. metadata. 
           		// It is useful to validate the block's address as it tells us that the 
			// block is alloced through this malloc library. Its helpful during free and realloc
	char data [1]; 	// Meta data for each block. The pointer to this mark the end of block as it at the end in struct.
};
```
- Malloc Logic
	- First align the requested size
	- If heap_start is initialized:
		– Search for a free chunk wide enough;
	– If new chunk is found:
		- Try to split the block using Buddy
		- Mark the chunk as used (b->free=0;)
		– Otherwise: we extend the heap.
	
	Note while finding the new block I put the pointer to the last visited chunk in
	last, so I can access it during the extension without traversing the whole list
	again. Otherwise I extend the heap (which is empty at that point.)
	Note that the function extend heap works here with last=NULL.

- Important Design Decisions
	- I am making sure that my block is always aligned so that while allocating memory I have to only allign the requested size.
	- While traversing the linked list to find the free block I am keeping track of last visited node so the malloc function can easily extends the end of the heap if no fitting chunk found
	- the split_block function in [utils.c](utils.c) cut the block passed in argument to make data block of the wanted size.
	- I have kept the "char data[1]" field in my struct which marks the end of block. This helps us know that from here on the memory is allocated till size specified in size attribute of struct

