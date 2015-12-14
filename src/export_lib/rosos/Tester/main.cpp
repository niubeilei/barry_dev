#include "rosos.h"
#include <stdio.h>


void print_current()
{
	int sz;
	aos_get_core_size(&sz);
	printf("cur: %d\n", sz);
}


int main(void)
{
	
	print_current();

	aos_set_core_size(90000);
	print_current();


	aos_set_core_size(80000);
	print_current();

	aos_set_pid_core_size(5540, 20100);
	int sz;
	aos_get_pid_core_size(5540, &sz);
	printf("%d\n", sz);
	return 0;
}
