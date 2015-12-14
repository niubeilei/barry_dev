all:main

main:
	gcc -c -Wall -g main.c
	gcc -g -Wall -o dnslookup main.o

clean:
	rm -f *.o
	rm -f dnslookup
install:
	cp dnslookup /usr/local/AOS/Bin
uninstall:
	rm -f /usr/local/AOS/Bin/dnslookup
	
	
