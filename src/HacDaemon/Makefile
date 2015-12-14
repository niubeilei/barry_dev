
CC=gcc
CFLAGS=-Wall -g -I..
LDFLAGS=-lpthread -ldl -lsqlite3 -lssl 
#libCommandMan.a

LIBDIR = ../../obj

modules=do_str sha
all: daemon 

daemon: daemon.o share.o sha.o do_str.o pro_ftp.o pro_telnet.o pro_ssh.o

clean:
	rm -rf *.o daemon *.so
