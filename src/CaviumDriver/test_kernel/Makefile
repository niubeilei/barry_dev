ifneq ($(KERNELRELEASE),)
	obj-m := test.o 
else 	
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build 	
	PWD := $(shell pwd) 
default: 	
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules 
endif
clean:	
	rm -f *.o	rm -f *.ko	rm -f *.mod*
