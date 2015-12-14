
all:
	gcc -Wall -o mysudo mysudo.c
	gcc -Wall -o setxmlconfig setxmlconfig.c
	gcc -Wall -o netconfig netconfig.c
	
clean:
	rm -f *.o
	rm -f mysudo setxmlconfig

install:
	cp -f mysudo /usr/local/AOS/Bin
	cp -f setxmlconfig /usr/local/AOS/Bin

uninstall:
	rm -f mysudo /usr/local/AOS/Bin
	rm -f setxmlconfig /usr/local/AOS/Bin

