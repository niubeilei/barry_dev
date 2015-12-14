#
# Makefile for the WatchDog device drivers.
#

# Only one watchdog can succeed. We probe the hardware watchdog
# drivers first, then the softdog driver.  This means if your hardware
# watchdog dies or is 'borrowed' for some reason the software watchdog
# still gives you some cover.


obj-$(CONFIG_JMK) +=  j9054.o lib_9054jmk.o jmk_lib_internal.o
