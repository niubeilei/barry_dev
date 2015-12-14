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
//      This is a app of look up hostname or ip address.   
//      main#
//      cli_requ#
//      read_all#
//      write_all#
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>

#include "main.h"

#define MAX_BUFFER_SIZE 256

int main(int argc, char *argv[]){
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	pid_t pid;
	
	daemon(0,0);

	if((pid=fork())==0){
		listenfd=socket(AF_INET, SOCK_STREAM, 0);
		if(listenfd<0){
			fprintf(stderr, "Socket error\n");
			exit(1);
		}

		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family=AF_INET;
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
		servaddr.sin_port=htons(SERVER_PORT);
		if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))<0){
			fprintf(stderr, "Bind error\n");
			exit(1);
		}
		if(listen(listenfd, BACKLOG)<0){
			fprintf(stderr, "Listen s!\n");
			exit(1);
		}
		for(;;){
			connfd=accept(listenfd, NULL, NULL);
			if(connfd<0){
				fprintf(stderr, "Accept error\n");
				exit(1);
			}
			serv_respon(connfd);
			close(connfd);
		}
		close(listenfd);
	}else{
		if(pid<0){
			exit(1);
		}else{
			return 0;
		}
	}
	return 0;
}

void serv_respon(int sockfd){
	int nbytes;
	char buf[MAX_BUFFER_SIZE];
	
	for(;;){
		memset(buf, 0, MAX_BUFFER_SIZE);
		nbytes=read_requ(sockfd, buf, MAX_BUFFER_SIZE);
		if(nbytes==0){
			return ;
		}else{
			if(nbytes<0){
				fprintf(stderr, "read error: %s \n", strerror(errno));
				return ;
			}
		}
		if(write_all(sockfd, buf, nbytes+1)<0){
			fprintf(stderr, "write error: %s\n", strerror(errno));
		}
	}
	return;
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
		printf("write %d bytes!\n", n);
        return (n);
}

int read_requ(int sockfd, char *buf, int size){
	char inbuf[MAX_BUFFER_SIZE];
	int read_bytes;
	struct hostent *host_info;
	struct sockaddr_in addr;
	int flag=-1;
	char temp[MAX_BUFFER_SIZE]={0};
	int bytes;

	read_bytes=read(sockfd, inbuf, MAX_BUFFER_SIZE-1);
	if(read_bytes!=0 && read_bytes<=MAX_BUFFER_SIZE-1){
		inbuf[read_bytes] = 0;
		if(inet_aton(inbuf, &addr.sin_addr)!=0){
			//host_info=gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET);
			flag=0;
			printf("address information of ip %s\n", inbuf);
		}else{
			host_info=gethostbyname(inbuf);
			flag=1;
			printf("address information of hostname is %s\n", inbuf);
		}
		if(host_info==NULL && flag==1){
			fprintf(stderr, "no address information of %s\n", inbuf);
			return -1;
		}
		memset(buf, 0, MAX_BUFFER_SIZE);
		if(flag==1){
			bytes=strlen(inet_ntoa(*(struct in_addr*)(*host_info->h_addr_list)));
			memcpy(buf, inet_ntoa(*(struct in_addr*)(*host_info->h_addr_list)), bytes);
		}
		if(flag==0){
			//strcpy(buf, host_info->h_name);
			//bytes=strlen(host_info->h_name);
			memcpy(buf, inbuf, read_bytes);
			bytes=read_bytes;
		}
		if(flag==-1){
			printf("this is some errors!\n");
			bytes=0;
		}
	}else{
		bytes=0;
	}

	return bytes;
}
				
