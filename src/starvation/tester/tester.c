
#include "starvation.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LEN 1024

int main()
{

	int ret;
	ret = ros_register_starvation( 99 );

	printf("ret=%d\n errmsg=%s\n",ret,strerror(errno));
	return 0;
}

