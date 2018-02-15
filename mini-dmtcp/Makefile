#Sample Makefile

all:	check

default: check

clean:
	rm -rf tempckpt myckpt context_ckpt restart hello.o hello ckpt ckpt.o libckpt.so

ckpt.o: ckpt.c
	gcc -c -fno-stack-protector -Wall -fpic -o ckpt.o ckpt.c

libckpt.so: ckpt.o
	gcc -shared -fno-stack-protector -o libckpt.so ckpt.o

hello.o: hello.c
	gcc -c -fno-stack-protector -Wall -Werror -fpic -o hello.o hello.c

hello:	hello.o
	gcc -g -fno-stack-protector -o hello hello.o

prog:	hello

restart: myrestart.c
	gcc -g -fno-stack-protector -static -Wl,-Ttext-segment=5000000 -Wl,-Tdata=5100000 -Wl,-Tbss=5200000 -o restart myrestart.c

res: 	restart
	./restart myckpt

gdb:
	gdb --args ./restart myckpt

check:	libckpt.so prog restart
	(sleep 3 && kill -12 `pgrep -n hello` && sleep 10 && pkill -9 hello) &
	LD_PRELOAD=`pwd`/libckpt.so ./hello
	(sleep 2 &&  pkill -9 restart) &
	make res

dist:
	dir=`basename $$PWD`; cd ..; tar cvf $$dir.tar ./$$dir; gzip $$dir.tar
