
#ifndef Omn_aos_KernelSimu_jiffies_h
#define Omn_aos_KernelSimu_jiffies_h


#ifndef __KERNEL__
/*
#include <time.h>
#include <sys/time.h>
 * Chen Ding, 02/21/2007
 define jiffies ({\
	struct timeval tv;\
	gettimeofday(&tv, 0);\
	tv.tv_sec*1000+tv.tv_usec/1000;\
		})
*/
extern unsigned long volatile jiffies;
#endif


#endif

