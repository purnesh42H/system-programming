
# Overview
 - [Program](read_memory_map.c), that populates an instance of MemoryRegion for each memory region of a given PID (passed in via command line, e.g., ./my_prog 4242) and writes it to a file. If no PID is provided, it should read its own memory maps. The program should also print the total size of all read-only memory regions. Also print the total size of all read-write regions.

- [Program](display_memory_region.c) which reads the file you just created and prints all the information in human readable format. It should also print the total size of read-only and read-write regions. Validate your program by comparing the two outputs.

# How to execute
- Compile: gcc <file.c> -o name
- ./name
- For example:
     - gcc read_memory_map.c -o read_map
     - ./read_map or ./read_map <pid>

