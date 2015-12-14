#!/bin/bash
PWD = $(AOSHOME)/src/CaviumDriver/init_cavium
all:main

main:
	gcc -c -g -Wall cavium_init.c
	gcc -o init_cavium cavium_init.o
clean:
	rm -f *.o init_cavium

install:
	rm -rf /usr/local/cavium
	cp -f ./init_cavium /usr/sbin
	chmod +x /usr/sbin/init_cavium
	chmod +x ./caviumd
	ln -s -f $(PWD)/caviumd /etc/rc.d/rc3.d/S99caviumd 
	mkdir /usr/local/cavium
	ln -s -f $(PWD)/main.out /usr/local/cavium/main.out
	ln -s -f $(PWD)/boot.out /usr/local/cavium/boot.out
	
	
	 

	
