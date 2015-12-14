////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: init.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
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
#include <errno.h>
#include <curses.h>




#include "init.h"

int main(int argc, char *argv[]){
	struct struct_ioctl *ioctl_buf;
	int fd;
	FILE *fp;
	int ret;
	
	ioctl_buf=(struct struct_ioctl*)malloc(sizeof(struct struct_ioctl));

	if((fd=open_jmk())<0){
		printf("open jmk error!\n");
		exit(1);
	}

	if((fp=fopen("/usr/local/AOS/Data/ca", "r"))!=NULL){
		ioctl_buf->send_direct=SEND_CA_TO_JMK;
		fread(&ioctl_buf->buf, sizeof(struct io_buf), 1, fp);
		ioctl_buf->cmd=INIT_MULTIJMK;
		ret=init_jmk(fd, ioctl_buf);
		if(ret<0){
			printf("ioctl error!\n");
			exit(1);
		}
		fclose(fp);
		close(fd);
		exit(0);
	}else{
		ioctl_buf->send_direct=RECEIVE_CA_FROM_JMK;
	}

	while((ret=init_jmk(fd, ioctl_buf))<0);

	printf("init_jmk return %d\n", ret);
	fp=fopen("/usr/local/AOS/Data/ca", "w");
	if(fp==NULL){
		printf("create file error!\n");
		exit(1);
	}
	if(ioctl_buf->send_direct==RECEIVE_CA_FROM_JMK){
		fwrite(&ioctl_buf->buf, sizeof(struct io_buf), 1, fp);
		printf("write file success\n");
		printf("ca0 len:%d\n", ioctl_buf->buf.ca0.len);
		printf("ca1 len:%d\n", ioctl_buf->buf.ca1.len);
		printf("ca2 len:%d\n", ioctl_buf->buf.ca2.len);
		printf("server_cert len:%d\n", ioctl_buf->buf.server_cert.len);
		printf("pub2 len:%d\n", ioctl_buf->buf.pub2.len);
		printf("ocsp len:%d\n", ioctl_buf->buf.ocsp.len);
		printf("pri2 len:%d\n", ioctl_buf->buf.pri2.len);
		
	}else{
		printf("error!\n");
		exit(1);
	}
	
	return 0;
}

int open_jmk(void){
	int fd;


	if ((fd=open("/dev/j9054", O_RDWR)) == -1){
		return -1;
	}
	printf("fd=%d\n", fd);
	return fd;
}

int init_jmk(int fd, struct struct_ioctl *ioctl_buf){
	int ret;

	ioctl_buf->cmd=INIT_MULTIJMK;

	printf("%08x\n", (int)ioctl_buf);

	ret=ioctl(fd, INIT_MULTIJMK, ioctl_buf);
	if(ret==-1){
		perror("ioctl error");
	}		
	printf("ret=%d\n", ret);
	return ret;
}
