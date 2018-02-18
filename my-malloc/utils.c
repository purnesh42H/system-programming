#include "utils.h"

const int SMALLEST_BLOCK = 64;

int get_two_power(size_t s) {
	int power = 0;
	while (s > 1) {
		s = s / 2;
		power++;
	}
	return power;
}

block join_free_chunks(block b) {
	if (b->next && b->next->free) {
		b->size += b->next->size + block_size();
		b->next = b->next->next;
		if (b->next) {
			b->next->prev = b;
		}
		b->buddy_order += 1;
	}
	return (b);
}

int get_buddy_order(size_t s) {
	size_t lower = SMALLEST_BLOCK;
	int order = 0;
	while (lower < s) {
		lower = lower * 2;
		order++;
	}
	return order;
}

void buddy_split(block b) {
	b->size = b->size / 2;
	block newb = (block)(b->data + b->size);
	newb->size = b->size - block_size();
	newb->next = b->next;
	newb->prev = b;
	b->next = newb;
	b->buddy_order = b->buddy_order - 1;
	newb->buddy_order = b->buddy_order;
	newb->free = 1;
	b->ptr = b->data;
  newb->ptr = newb->data;
}

block buddy_join(block b) {
	while (b->prev && b->prev->free) {
		b = b->prev;
	}
	while(b->next && b->next->free) {
		b = join_free_chunks(b);
	}
	return b;
}



size_t alignn(size_t s, size_t alignment) {
	int power = get_two_power(alignment);
	return (((((s) -1) >> power) << power) + alignment);
}

size_t align8(size_t s) {
	return alignn(s, 8);
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

block insert_block(void *heap_start, block new, size_t s) {
	block start;
	if (heap_start) {
		start = heap_start;
		while(start->next != NULL) {
			start = start->next;
		}
	}
	new->size = s;
	new->next = NULL;
	new->free = 0;
	new->ptr = new->data;
	if (heap_start) {
		start->next = new;
		new->prev = start;
	} else {
		new->prev = NULL;
	}

	return new;
}

block find_free_block(void *heap_start, block *last, size_t size) {
	int order = get_buddy_order(size);
	block start = heap_start;
	while(start && !(start->free && start->buddy_order >= order)) {

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
	size_t sb = sysconf(_SC_PAGESIZE);
	block b = sbrk(0);

	if (sbrk(block_size() + sb) == (void *) -1) {
 		/* sbrk fails , return NULL */
		errno = ENOMEM;
		return (NULL);
	}

	while (sb < size) {
		sb += sysconf(_SC_PAGESIZE);
		if (sbrk(block_size() + sb) == (void *) -1) {
	 		/* sbrk fails , return NULL */
			errno = ENOMEM;
			return (NULL);
		}
	}

  if(mlock(b, sb) == 0) {
		b->size = sb; // size of the new block is page size
		b->buddy_order = get_buddy_order(sb);
		b->next = NULL;
		b->prev = last;
		b->ptr = b->data;
		if (last) {
			last->next = b;
		}
		b->free = 1;
		munlock(b, sb);
	} else {
		/* sbrk fails , return NULL */
		errno = ENOMEM;
		return (NULL);
	}
	return (b);

}

void split_block(block b, size_t s) {
	int required_order = get_buddy_order(s);
	while (b->buddy_order != required_order) {
		buddy_split(b);
	}
}

int valid_address(void *heap_start, void *p) {
	if(heap_start) {
		if(p > heap_start && p < sbrk(0)) { // if the pointer is between the heap start and current break, then it is a valid
			//block b = get_block(p);
			return (p == get_block(p)->ptr); // we have field ptr pointing to the field data, if b->ptr == b->data, then b is probably (very probably) a valid block
		}
	}
	return (0);
}
