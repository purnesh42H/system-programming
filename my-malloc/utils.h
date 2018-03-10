#ifndef UTILS_H_
#define UTILS_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

typedef struct memory_block *block;

struct memory_block {
	size_t size;
	block next;
	block prev;
	int free;
	int buddy_order;
	void *ptr;
	char data [1];
};

/* The function returns a fitting chunk, or NULL if none where found. After the execution, the
   argument last points to the last visited chunk. */
int is_two_power(size_t s);
int get_two_power(size_t s);
block join_free_chunks(block b);
int get_buddy_order(size_t s);
void buddy_split(block b);
block buddy_join(block b);
size_t alignn(size_t s, size_t alignment);
size_t align8(size_t s);
size_t block_size();
block get_block (void *p);
block insert_block(void *heap_start, size_t s);
block find_free_block(void *heap_start, block *last, size_t size);
void copy_block(block src, block dest);
block extend_heap(block last, size_t size);
void split_block(block b, size_t s);
int valid_address(void *heap_start, void *mmap_start, void *p);
int some(int a, int b);

extern const int SMALLEST_BLOCK;

#endif
