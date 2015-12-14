////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: init1.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <sys/msg.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <linux/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sched.h>
#include <signal.h>
#include <sys/ipc.h>
//#include <sys/sem.h>
#include <pthread.h>
#include <errno.h>
#include <curses.h>
//#include <openssl/md5.h>
#include <syslog.h>
#include <sys/ioctl.h>


#include "init.h"

int main(int argc, char *argv[]){
	struct struct_ioctl *ioctl_buf;
	int fd;
	FILE *fp;
	int ret;
	
	ioctl_buf=(struct struct_ioctl*)malloc(sizeof(struct struct_ioctl));

	fd=open("/dev/j9054", O_RDWR);
	if(fd<0)
	{
		perror("open device file error");
		return -1;
	}
	ioctl_buf->send_direct=RECEIVE_CA_FROM_JMK;
	ioctl_buf->cmd=INIT_MULTIJMK;
	ret=ioctl(fd, INIT_MULTIJMK, ioctl_buf);
	if(ret<0)
	{
		perror("init device error");
		return -1;
	}
	close(fd);

	fp=fopen("/usr/local/AOS/Data/ca", "w");
	if(fp==NULL)
	{
		perror("create file error");
		syslog(LOG_ERR,"create file error");
		return -1;
	}
	fwrite(&ioctl_buf->buf, sizeof(struct io_buf), 1, fp);
	fclose(fp);
	printf("init jmk ok\n");

	return 0;
}

