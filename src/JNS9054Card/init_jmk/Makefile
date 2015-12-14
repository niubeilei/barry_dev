all:main

main:
	gcc -Wall -c -g init1.c
	gcc -g -Wall -lncurses -o init_jmk_daemon init1.o
clean:
	rm -f *.o
	rm -f init_jmk_daemon
install:
	cp init_jmk_daemon /usr/local/AOS/Bin
uninstall:
	rm -f /usr/local/AOS/Bin/init_jmk_daemon
