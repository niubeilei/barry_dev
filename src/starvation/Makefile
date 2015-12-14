
ifneq ($(KERNELRELEASE),)
	obj-m := starvation.o
else
	KERNELVERSION := $(shell uname -r)
	KERNELDIR := /lib/modules/$(KERNELVERSION)/build
	PWD := $(shell pwd)
modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) LDDINC=$(PWD) modules
endif

clean :
	   rm -rf *.o *.mod.c *.ko .*cmd .*versions *.symvers
