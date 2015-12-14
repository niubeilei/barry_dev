#
# Makefile for the WatchDog device drivers.
#

# Only one watchdog can succeed. We probe the hardware watchdog
# drivers first, then the softdog driver.  This means if your hardware
# watchdog dies or is 'borrowed' for some reason the software watchdog
# still gives you some cover.



all:
	make -C liblicense
	make -C getHDinfo 
	make -C verify
clean:
	make -C liblicense clean
	make -C getHDinfo clean
	make -C verify clean
