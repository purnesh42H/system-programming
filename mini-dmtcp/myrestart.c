#include <stdio.h>
#include <ucontext.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <sys/mman.h>

// Constants to map in some new memory for a new stack
#define RARE_USED_ADDR 0x5300000
#define SIZE 0x1000

/* Struct to store address range of memory and permissions */
struct section_header {
	char *start_address;
	char *end_address;
	int is_readable;
	int is_writeable;
	int is_executable;
};

ucontext_t g_ucp_ref; // pre-allocated memory for reading registers from checkpoint image
struct section_header sec_header;
char chkpt_img[1000]; // global variable where checkpoint image file is copied
int file_descriptor;
char chr;

void print_error(){
	printf("Encountered Error....Try again later");
	exit(1);
}

void restore_memory() {
	file_descriptor = open(chkpt_img, O_RDONLY);

	if (file_descriptor < 0) {
		print_error();
	}

	// Copy the registers from the file header of your checkpoint image file into some pre-allocated memory in your data segment.
	// The memory of your myrestart process should no longer conflict with the memory to be restored from the checkpoint image.
  memset(&g_ucp_ref, 0, sizeof(g_ucp_ref));
	int rec = read(file_descriptor, &g_ucp_ref, sizeof(ucontext_t));
	//printf("%d", rec);

	while (read(file_descriptor, &chr, 1) > 0) {
		lseek(file_descriptor, -1, SEEK_CUR);
		read(file_descriptor, &sec_header, sizeof(struct section_header));

		unsigned long int length = sec_header.end_address - sec_header.start_address;
		//printf("%lu\n", length);
		//fflush(stdout);

		munmap((void *) sec_header.start_address, length);
		// Call mmap to map into memory a corresponding memory section for your new process
		char *p = mmap((void*) sec_header.start_address, length, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, -1, 0);

		int rwxp = 0;

		if (sec_header.is_readable == 1) {
			rwxp |= PROT_READ;
		}
		if (sec_header.is_writeable == 1) {
			rwxp |= PROT_WRITE;
		}
		if (sec_header.is_executable == 1) {
			rwxp |= PROT_EXEC;
		}

		mprotect(sec_header.start_address, length, rwxp); // setting the same permissions as the restored process for the memory region
	}

	// Now, you need to jump into the old program and restore the old registers.
	// In a previous step, you had copied the old registers into your data segment.
	// Now use setcontext() to restore those as the working registers.
	printf("hey\n");
	fflush(stdout);
	setcontext(&g_ucp_ref);
	//printf("hi");
	close(file_descriptor);
}

int main(int argc, char * argv[]) {
	char *file = argv[1];
	sprintf(chkpt_img, file); // copying the checkpoint image to the global string variable
	char *new_papping_pointer = mmap((void *)RARE_USED_ADDR, SIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED, -1, 0); // creating new memory
	/*
	 Use the inline assembly syntax of gcc to include code that will switch the stack pointer (syntax: $sp or %sp)
	 from the initial stack to the new stack. Then immediately make a function call to a new function.
	 The new function will then use a call frame in the new stack.
	*/
	asm volatile ("mov %0, %%sp;" : : "g" (new_papping_pointer) : "memory");
	restore_memory();
	return 0;
}
