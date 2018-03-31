#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

int main(int argc, char *argv[]) {
	unsigned long long malloc_counter = 1;
	size_t allocation_size = 1024;
	size_t last_allocated = 0;
	malloc_counter = 2;
	int failed = 0;
	int add_or_multiply = 0;
	size_t diff = 0;
	void *p = NULL;
	while(1) {
		if (allocation_size > last_allocated) {
			diff = allocation_size - last_allocated;
		} else {
			diff = last_allocated - allocation_size;
		}

		p = mmap(p, diff, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

		if (errno == ENOMEM) {
			printf("Failed to allocate size %zd. Trying with lesser\n", allocation_size);
			malloc_counter = 1024;
			if (add_or_multiply) {
				allocation_size -= 1024;
			} else {
				allocation_size = allocation_size / 2;
			}
			failed += 1;
		} else {
			last_allocated = last_allocated + diff;
			if (failed) {
				allocation_size += malloc_counter;
				malloc_counter += malloc_counter;
			} else {
				allocation_size *= malloc_counter;
				malloc_counter *= 2;
			}
		}
		if (failed > 10) {
			if (add_or_multiply) {
				break;
			}
			failed -= 5;
			add_or_multiply = 1;
		}
	}
	munmap(p, last_allocated);
	printf("Your RAM size %lf\n", (float)((last_allocated/1024)/1024)/1024);
	return 0;
}
