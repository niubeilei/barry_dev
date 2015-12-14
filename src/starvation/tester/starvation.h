#ifndef __BINLOG_H
#define __BINLOG_H

#include <errno.h>

#define __NR_ros_register_starvation 294
#define __NR_ros_unregister_starvation 295

/*
 * user-visible error numbers are in the range -1 - -128: see
 * <asm-i386/errno.h>
 */
#define __syscall_return(type, res) \
do { \
	if ((unsigned long)(res) >= (unsigned long)(-(128 + 1))) { \
		errno = -(res); \
		res = -1; \
	} \
	return (type) (res); \
} while (0)

#define _syscall1(type,name,type1,arg1) \
type name(type1 arg1) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)) : "memory"); \
__syscall_return(type,__res); \
}


#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3) \
type name(type1 arg1,type2 arg2,type3 arg3) \
{ \
long __res; \
__asm__ volatile ("int $0x80" \
	: "=a" (__res) \
	: "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
		  "d" ((long)(arg3)) : "memory"); \
__syscall_return(type,__res); \
}

_syscall1(int, ros_register_starvation, int, pid);
_syscall1(int, ros_unregister_starvation, int, pid);

#endif

