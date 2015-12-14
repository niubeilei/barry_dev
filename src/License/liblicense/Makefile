CC=g++
CFLAGS=-Wall


all:lib 

lib: rsa.o des.o getHDinfo.o verify.o
	ar rvu liblicense.a rsa.o des.o getHDinfo.o verify.o
	cp -f *.a ../../../obj

clean:
	rm -f *.o *.a ../../../obj/liblicense.a
