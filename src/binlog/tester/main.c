#include <stdio.h>
#include <errno.h>


struct meminfo {
	unsigned long totalram;
	unsigned long sharedram;
	unsigned long freeram;
	unsigned long bufferram;
	unsigned long totalhigh;
	unsigned long freehigh;
	unsigned long mem_unit;
};


#define __NR_memstat 	223

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

_syscall1(int, memstat, struct meminfo*, n);

int main()
{
	int	ret;
	struct meminfo m ;

	memstat(&m);
	printf("totalram:\t %d\n", m.totalram);
	printf("sharedram:\t %d\n", m.sharedram);
	printf("freeram:\t %d\n", m.freeram);
	printf("bufferram:\t %d\n", m.bufferram);
	printf("totalhigh:\t %d\n", m.totalhigh);
	printf("freehigh:\t %d\n", m.freehigh);
	printf("mem_unit:\t %d\n", m.mem_unit);
	
	return 0;
}

