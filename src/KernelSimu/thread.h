#ifndef KERNELSIMU_THREAD_H
#define KERNELSIMU_THREAD_H


#ifdef __KERNEL__
//pid_t kernel_thread(int (*fn)(void *), void *arg, unsigned long flags)

#define aos_thread_create(function,parameter,flags) kernel_thread( function, parameter, flags)

#else
#include <pthread.h>
//int pthread_create(pthread_t * thread, pthread_attr_t * attr, void * (*start_routine)(void *), void * arg);
#define aos_thread_create(function,parameter,flags) { \
	pthread_t tid; \
	pthread_attr_t attr;\
	pthread_attr_init( &attr );\
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_DETACHED );\
	pthread_create(&tid, &attr,  (void*(*)(void*)) function, parameter);\
	\
	}

#endif // __KERNEL__

#endif
