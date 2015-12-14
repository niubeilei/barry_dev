#
# objects and targets
#
AR=				/usr/bin/ar
CC=				/usr/bin/g++
INSTALL=		/usr/bin/install
LN=				/bin/ln

CFLAGS=			-DPOSIX -O   -g
LDFLAGS=


#
# objects and targets
#
OBJECTS = *.o

all: testHbase 

testHbase: testHbase.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o testHbase testHbase.o

testHbase.o: main.cpp
	$(CC) -c -o testHbase.o main.cpp

clean:
	rm -rf testHbase $(OBJECTS)
