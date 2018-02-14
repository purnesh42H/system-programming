#include "utils.h"
size_t alignn(size_t s, size_t n) {
	size_t alignment = n;
	int power = 0;
	while (alignment > 0) {
		alignment = alignment / 2;
		power++;
	}

	return (((((s) -1) >> power) << power) + n);
}


size_t align8(size_t s) {
	return (((((s) -1) >> 3) << 3) + 8);
}

size_t block_size() {
	// Since its a 64 bit machine
	return 40;
}

/* Get the block from and addr */
block get_block (void *p) {
	char *tmp;
	tmp = p;
	return (p = tmp -= block_size());
}

block find_free_block(void *heap_start, block *last, size_t size) {
	block start = heap_start;
	while(start && !(start->free && start->size >= size)) {
		*last = start;
		start = start->next;
	}
	return start;
}

void copy_block(block src, block dest) {
	int *src_data, *dest_data;
	size_t i;
	src_data = src->ptr;
	dest_data = dest->ptr;

	for(i = 0; i * 8 > src->size && i * 8 < dest->size; i++) {
		dest_data[i] = src_data[i];
	}
}

block extend_heap(block last, size_t size) {
	unsigned int sb;
	block b = sbrk(0);
	sb = (uintptr_t)(sbrk(sysconf(_SC_PAGESIZE)));

	if (sb < 0) {
 		/* sbrk fails , return NULL */
		errno = ENOMEM;
		return (NULL);
	}
  mlock(b, sb);
	b->size = size;
	b->next = NULL;
	b->prev = last;
	b->ptr = b->data;
	if (last) {
		last->next = b;
	}
	b->free = 0;
	munlock(b, sb);
	return (b);
}

void split_block(block b, size_t s) {
	block new;
	new = (block)(b->data + s);
	new->size = b->size - s - block_size() ;
	new->next = b->next;
	new->prev = b;
	new->free = 1;
	new->ptr = new->data;
	b->size = s;
	b->next = new;
	if (new->next) {
		new->next->prev = new;
	}
}

int valid_address(void *heap_start, void *p) {
	if(heap_start) {
		if(p > heap_start && p < sbrk(0)) { // if the pointer is between the heap start and current break, then it is a valid address
			return (p == get_block(p)->ptr); // we have field ptr pointing to the field data, if b->ptr == b->data, then b is probably (very probably) a valid block
		}
	}
	return (0);
}

block join_free_chunks(block b) {
	if (b->next && b->next->free) {
		b->size += b->next->size + block_size();
		b->next = b->next->next;
		if (b->next) {
			b->next->prev = b;
		}
	}
	return (b);
}
