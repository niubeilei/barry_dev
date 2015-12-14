////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.c
// Description:
//	This is a app of reboot the netgate machine.   
//	main#
//	cli_requ#
//	read_all#
//	write_all#
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>



#include "main.h"

int main(int argc, char *argv[]){
	int sockfd;
	struct sockaddr_in servaddr;
	int i;
	int machine_count;
	char buf[256];


	sockfd=socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd<0){
		fprintf(stderr, "Socket error\n");
		exit(1);
	}
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(SERVER_PORT);
	if(inet_aton("127.0.0.1", &servaddr.sin_addr)==0){
		fprintf(stderr, "Inet_aton error \n");
		exit(1);
	}
	
	if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
		fprintf(stderr, "conect error: %s \n", strerror(errno));
		exit(1);
	}

	cli_requ(sockfd, argv[1]);
	cli_rec(sockfd, buf);
	if(buf>0){
		printf("address information is %s\n", buf);
	}
	close(sockfd);

	
	return 0;
}

void cli_requ(int sockfd, char *buf){
	char inbuf[256];
	
	write_all(sockfd, buf, strlen(buf));
	
	return ;
}
void cli_rec(int sockfd, char *buf){
	int bytes;

	bytes=read(sockfd, buf, 256);
	if(bytes==0){
		printf("rec error!\n");
	}

	return ;
}
int write_all(int fd, void *buf, int n){
	int nleft, nbytes;
	char *ptr=buf;
	
	nleft=n;
	ptr=buf;
	for(;nleft>0;){
		nbytes=write(fd, ptr, nleft);
		if(nbytes<=0){
			if(errno==EINTR){
				nbytes=0;
			}else{
				return (-1);
			}
		}
		nleft-=nbytes;
		ptr+=nbytes;
	}
	return (n);
}
	
