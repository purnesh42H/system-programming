#include<stdio.h>
#include<ucontext.h>
#include<signal.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include <sys/types.h>
#include <stdbool.h>


/* Struct to store address range of memory and permissions */
struct section_header {
	char *start_address;
	char *end_address;
	int is_readable;
	int is_writeable;
	int is_executable;
};

/* Read rest of the line of memory map and find the area/path of the current process */
void read_path(int fd, char* name) {
	char chr[1] = " ";
	while(chr[0] != '\n')
	{
		if(read(fd, chr, 1) < 0) {
			printf("Error on reading to the end of line\n");
			exit(1);
		}
		else if(chr[0] == '[') {
			while(chr[0] != ']') {
				read(fd, chr, 1);
				*name = chr[0];
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
	return (strstr(path, "vvar") != NULL || strstr(path, "vdso") != NULL || strstr(path, "vsyscall") != NULL);
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
	copy_permissions(file_descriptor, &sec_header.is_readable, &sec_header.is_writeable, &sec_header.is_executable);

	return sec_header;
}

/* Read the current process memory map */
int read_cur_proc_mem_map(int pid) {
	int file_descriptor = open("/proc/self/maps", O_RDONLY);
	return file_descriptor;
}

/* Signal Handler to trigger the checkpoint
To trigger the checkpoint we just need to call kill -12 PID from command line */
void signal_handler(int signo) {
	char read_character;
	struct section_header sec_header;

	// get the context(stack,register etc.)
	ucontext_t ucp;
	getcontext(&ucp);

	// read file descriptor for the memory layout of the current process
	int cur_proc_map_file_descriptor = open("/proc/self/maps", O_RDONLY);
	// create the output checkpoint file
	int checkpoint_file_descriptor = open("./myckpt", O_CREAT | O_WRONLY | O_APPEND, 0666);

	if(cur_proc_map_file_descriptor < 0) {
		printf("Exiting with error while reading process map %d", cur_proc_map_file_descriptor);
		exit(1);
	} else if (checkpoint_file_descriptor < 0) {
		printf("Exiting with error while creating checkpoint file %d", checkpoint_file_descriptor);
		exit(1);
	}

	// saving the registers to the checkpoint file
	write(checkpoint_file_descriptor, &ucp, sizeof(ucontext_t));

	while (read(cur_proc_map_file_descriptor, &read_character, 1) > 0) {
		lseek(cur_proc_map_file_descriptor, -1, SEEK_CUR);
		sec_header = get_section_header(cur_proc_map_file_descriptor);
		bool is_sys_call = is_sys_call_proc(cur_proc_map_file_descriptor);
		if(sec_header.is_readable == 1 && is_sys_call) {
				write(checkpoint_file_descriptor, &sec_header, sizeof(struct section_header));
		}
	}
	close(checkpoint_file_descriptor);
	close(cur_proc_map_file_descriptor);
}

__attribute__((constructor))
void myconstructor() {
	signal(SIGUSR2, signal_handler);
}
