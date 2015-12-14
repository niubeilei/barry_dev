# File: makefile
#
TARGETEXE1 = hac_ftpc
TARGETEXE2 = hac_telnetc

CC = gcc

all:
		$(CC) -g hac_ftpc.c -o $(TARGETEXE1)
		$(CC) -g hac_telnetc.c -o $(TARGETEXE2)
		#$(CC) -g hac_telnetc.c -o $(TARGETEXE2)
		$(CC) -o readhaclog ReadHacLog.c
clean:
		make clean

