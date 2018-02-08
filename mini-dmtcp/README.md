# Overview
DMTCP is a widely used software package for checkpoint-restart: http://dmtcp.sourceforge.net. However, you do not need any prior knowledge of DMTCP for this assignment. This software produce is self-contained. I have written a shared library, libckpt.so. Given an arbitrary application in C or C++, you will then run it with this library with the following command-line:

LD_PRELOAD=/path/to/libckpt.so ./myprog
where /path/to/libckpt.so must be an absolute pathname for the libckpt.so.

Makefile is provided for this project

# How to build
 - Run "make clean" to removes all object/binary files
 - Run "make check"
 - If the process doesn't resume automatically after killed
  	- Run "./restart myckpt" to resume manually

# Alternate build
	- Run "make clean"
	- Run "make
rm -rf myckpt
gcc -g -O0  -c -Wall -Werror -fpic -o ckpt.o ckpt.c
gcc -g -O0  -c -Wall -Werror -fpic -o hello.o hello.c
gcc -g -O0  -g -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart myrestart.c
gcc -g -O0  -g -o hello hello.o
gcc -g -O0  -shared -o libckpt.so ckpt.o
(sleep 3 && kill -12 `pgrep -n hello` && sleep 2 && pkill -9 hello) &
LD_PRELOAD=`pwd`/libckpt.so ./hello"


# Limitations
 - Sometimes receives segmentation fault while setting the context during restart
