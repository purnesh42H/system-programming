#Makefile for Ram & Cache charaterstics
CC=gcc
CFLAGS=-g -O0 -fPIC -Werror -Wall

TESTS=calc_ram cache_charac
# build an executable named myprog from myprog.c
all: calc_ram_size.c
	gcc -g -Wall -o calc_ram calc_ram_size.c
	gcc -g -Wall -o cache_charac cache_charac.c

clean:
	$(RM) calc_ram cache_charac

check:	${TESTS}
	./calc_ram

check1:	${TESTS}
	./cache_charac
