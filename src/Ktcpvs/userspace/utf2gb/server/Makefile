all:main

main:
	gcc -c -g -Wall main.c
	gcc -g -o -Wall utf82gb main.o

clean:
	rm -f *.o
	rm -f utf82gb
install:
	cp utf82gb /usr/local/AOS/Bin/utf82gb
uninstall:
	rm -f /usr/local/AOS/Bin/utf82gb
