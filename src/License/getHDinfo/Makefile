CC=g++
CFLAGS=-Wall

BIN=getHDinfo
LIBDIR = ../objlicense
INCLUDE=../objlicense

all:main

main:
	$(CC) -c -I$(INCLUDE) main.c
	$(CC) -o $(BIN) main.o -L$(LIBDIR) -llicense
	strip $(BIN)
clean:
	rm -f $(BIN)
	rm -f *.o
