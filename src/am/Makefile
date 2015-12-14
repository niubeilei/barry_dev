
CC=gcc
CFLAGS=-Wall -g -I..
LDFLAGS=-lpthread -ldl -lsqlite3 -lssl 
#libCommandMan.a

LIBDIR = ../../obj

modules=user ipdeny
all: am 

am: am.o user.o ipdeny.o sha.o do_str.o author.o dbauthor.o web_authen.o 

clean:
	rm -rf *.o am *.so
