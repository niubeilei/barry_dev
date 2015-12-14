
#include "binlog.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>

#include <unistd.h>


int main(int argc, char **argv)
{
	int ret=0;

	if(argc<2)
	{
		puts("Usage: writer string");
		exit(1);

	}

	ret = binlog_write(argv[1],strlen(argv[1]),0);

	printf("ret=%d,err=%s\n",ret,strerror(errno));


	return 0;
}

