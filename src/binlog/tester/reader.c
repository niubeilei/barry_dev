
#include "binlog.h"

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define LEN 1024

int readlog(void)
{
	char buf[LEN]="BUF";
	int ret=0;

	ret = binlog_read(buf,LEN,0);

	printf("ret=%d,err=%s\n",ret,strerror(errno));

	if(ret>0)
	{
		buf[ret]=0;
		printf("buf=%s\n",buf);
	}

	return ret;
}
int main()
{

	while(readlog()>=0)
	{
	}
	return 0;
}

