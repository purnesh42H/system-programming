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

/* Struct to store address range of memory and permissions */
struct section_header {
	char *start_address;
	char *end_address;
	size_t length;
	int is_readable;
	int is_writeable;
	int is_executable;
};

ucontext_t g_ucp_ref; // pre-allocated memory for reading registers from checkpoint image
char chkpt_img[1000]; // global variable where checkpoint image file is copied
int file_descriptor;
char chr;
struct section_header restart_header;

/* Read rest of the line of memory map and find the area/path of the current process */
void read_path(int fd, char* name) {
	char chrt[1] = " ";
	while(chrt[0] != '\n')
	{
		if(read(fd, chrt, 1) < 0) {
			printf("Error on reading to the end of line\n");
			exit(1);
		}
		else if(chrt[0] == '[') {
			while(chrt[0] != ']') {
				read(fd, chrt, 1);
				*name = chrt[0];
				name++;
			}

		}
	}
}

/* Returns true iff the region is virtual dynamic shared object
its used by system calls to switch to kernel mode */

bool is_sys_call_proc(int file_descriptor) {
	char path[100];
	read_path(file_descriptor, path);
	return (strstr(path, "stack") != NULL);
}

/* Read hexa decimal number and assign it to the passed value
Taken from mtcp_readhex() (found in: src/mtcp/mtcp_util.ic ,
or online a: http://www.ccs.neu.edu/course/cs5600f15/dmtcp/mtcp__util_8ic.html ) */
void copy_address(int file_descriptor,char ** value) {
	char chr;
	unsigned long int val;
	val = 0;
	while(1) {
		read(file_descriptor, &chr, 1);
		if((chr >= '0') && (chr <= '9')) chr -= '0';
		else if ((chr >= 'a') && (chr <= 'f')) chr -= 'a' - 10;
		else if ((chr >= 'A') && (chr <= 'F')) chr -= 'A' - 10;
		else break;
		val = val * 16 + chr;
	}
	*value = (char *)val;
}

/* Read permissions(rwxp) of the current process and assign it the passed values */
void copy_permissions(int file_descriptor, int *read_perm, int *write_perm, int *exec_perm) {
	char rwxp[4];
	read(file_descriptor, &rwxp, 4);
	if (rwxp[0] == 'r') {
		*read_perm = 1;
	} else {
		*read_perm = 0;
	}

	if (rwxp[0] == 'w') {
		*write_perm = 1;
	} else {
		*write_perm = 0;
	}

	if (rwxp[0] == 'x') {
		*exec_perm = 1;
	} else {
		*exec_perm = 0;
	}
}

/* Populate and returns the section header for the memory reagion of a process */
struct section_header get_section_header(int file_descriptor) {
	struct section_header sec_header;

	copy_address(file_descriptor, &sec_header.start_address);
	copy_address(file_descriptor, &sec_header.end_address);
	sec_header.length = sec_header.end_address - sec_header.start_address;
	copy_permissions(file_descriptor, &sec_header.is_readable, &sec_header.is_writeable, &sec_header.is_executable);

	return sec_header;
}

void print_error() {
	printf("Encountered Error....Try again later");
	exit(1);
}

void* create_new_stack() {

	char *new_address = (char*)0x5300000;
	size_t len = 0x1000;  // 0x5300000 - 0x5301000

	void *start_address = mmap(new_address, len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (start_address == MAP_FAILED) {
		printf("Unable to create a MAP in the specified region\n");
		exit(1);
	}

	return new_address + len;
}

struct section_header unmap_stack_address() {
	int fd = open("/proc/self/maps", O_RDONLY);
	while (read(fd, &chr, 1) > 0) {
		lseek(fd, -1, SEEK_CUR);
		struct section_header header = get_section_header(fd);
		bool is_sys_call = is_sys_call_proc(fd);
		if (is_sys_call) {
		 	return header;
		}
	}
}

void restore_memory() {
	file_descriptor = open(chkpt_img, O_RDONLY);

	if (file_descriptor < 0) {
		print_error();
	}

	int ret = munmap(restart_header.start_address, (size_t)(restart_header.end_address - restart_header.start_address));
		if (ret == -1) {
			printf("Failed to unmap the current stack memory\n");
  }

	// Copy the registers from the file header of your checkpoint image file into some pre-allocated memory in your data segment.
	// The memory of your myrestart process should no longer conflict with the memory to be restored from the checkpoint image.
	memset(&g_ucp_ref, 0, sizeof(g_ucp_ref));
	int rec = read(file_descriptor, &g_ucp_ref, sizeof(ucontext_t));

	while (read(file_descriptor, &chr, 1) > 0) {
		lseek(file_descriptor, -1, SEEK_CUR);
		struct section_header sec_header;
		read(file_descriptor, &sec_header, sizeof(struct section_header));

		int rwxp = PROT_READ | MAP_PRIVATE;
		if (sec_header.is_readable == 1) {
			rwxp |= PROT_READ;
		}
		if (sec_header.is_writeable == 1) {
			rwxp |= PROT_WRITE;
		}
		if (sec_header.is_executable == 1) {
			rwxp |= PROT_EXEC;
		}

		char *p=mmap((void*) sec_header.start_address, sec_header.length, rwxp, MAP_ANONYMOUS | MAP_FIXED | MAP_PRIVATE, -1, 0);

		printf("%zd %p-%p %d %d %d\n", sec_header.length,
			sec_header.start_address, sec_header.end_address, sec_header.is_readable, sec_header.is_writeable, sec_header.is_executable);
		fflush(stdout);

		char *data = (char *)malloc(sec_header.length);
		if (read(file_descriptor, data, sec_header.length) <= 0) {
			printf("Failed to read the data from file...\n");
			exit(1);
		}

		memcpy(sec_header.start_address, data, sec_header.length);
	}

	// Now, you need to jump into the old program and restore the old registers.
	// In a previous step, you had copied the old registers into your data segment.
	// Now use setcontext() to restore those as the working registers
	setcontext(&g_ucp_ref);

	close(file_descriptor);
}

int main(int argc, char * argv[]) {

	if (argc != 2) {
		printf("Invalid number of arguments ...\n");
	}

	// Mapping memory for new stack
	void* new_stack_ptr = create_new_stack();

	// Copy the checkpoint file location
	strcpy(chkpt_img, argv[1]);
	restart_header = unmap_stack_address();

	// Moving sp to new stack address location

	asm volatile("mov %0, %%rsp" : : "g" (new_stack_ptr): "memory");

	restore_memory();
}
