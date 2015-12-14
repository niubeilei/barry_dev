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
//   
//
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
#include <iconv.h>

#include "main.h"


int main(int argc, char *argv[]){
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	pid_t pid;

	daemon(0,0);
	//if((pid=fork())==0){
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
			fprintf(stderr, "Listen error!\n");
			exit(1);
		}
		for(;;){
			connfd=accept(listenfd, NULL, NULL);
			if(connfd<0){
				fprintf(stderr, "Accept error\n");
				//exit(1);
				continue;
			}
			serv_respon(connfd);
			close(connfd);
		}
		close(listenfd);
/*
}else{
		if(pid<0){
			exit(1);
		}else{
			return 0;
		}
	}
	*/
	return 0;
}

void serv_respon(int sockfd){
	int nbytes;
	char buf[MAX_LENGTH];
	
	for(;;){
		memset(buf, 0, MAX_LENGTH);	
		nbytes=read_requ(sockfd, buf, MAX_LENGTH);
		printf("will send buf %s\n", buf);
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
		memset(buf, 0, MAX_LENGTH);
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
        return (n);
}

int read_requ(int sockfd, char *buf, int size){
	char *inbuf;
	int n=0;
	int i;
	char ch;
	int ret;
	char *out_buf;
	int out_len;
	char *result;
	int res_len;
	
	inbuf=(char*)malloc(MAX_LENGTH);
	memset(inbuf, 0, MAX_LENGTH);
	if(inbuf==NULL){
		printf("alloc memory error!\n");
		return 1;
	}
	memset(inbuf, 0, MAX_LENGTH);
	i=read(sockfd, inbuf, MAX_LENGTH);
	if(i<=0){
		printf("error! no data received!\n");
		perror("read error:");
		return -1;
	}
	printf("read %d bytes\n", i);
	out_buf=(char*)malloc(MAX_LENGTH);
	if(out_buf==NULL){
		printf("error! alloc memory error!\n");
		free(inbuf);
		return -1;
	}
	memset(out_buf, 0, MAX_LENGTH);
	ret=parse_url(inbuf, i, out_buf, &out_len);
	printf("pase_url return out length is %d\n", out_len);
	if(ret){
		printf("error! parse_url fail!\n");
		free(out_buf);
		free(inbuf);
		return -1;
	}
	result=(char *)malloc(MAX_LENGTH);
	memset(result, 0, MAX_LENGTH);
	if(result==NULL){
		free(inbuf);
		free(out_buf);
		printf("alloc memory error!\n");
		return -1;
	}
	ret=charset_convertion("UTF8", "GB2312", &out_buf, out_len, &result, &res_len);
	if(ret){
		printf("charset convert error!\n");
		free(inbuf);
		free(out_buf);
		free(result);
		return -1;
	}
	printf("result %s \n", result);	
	memcpy(buf, result, res_len);
	free(inbuf);
	free(out_buf);
	free(result);
	printf("res_len is %d\n", res_len);
	return res_len;
}
				
int charset_convertion(const char *from, const char *to, char **inbuf, int length, char **outbuf, int *outlen)            
{                                                                                                                 
	char *sin, *sout;                                                                                               
	int lenin, lenout, ret;                                                                                   
	iconv_t c_pt;                                                                                                   
                                                                                                                  
                                                                                                                  
	if ((c_pt = iconv_open(to, from)) == (iconv_t)-1) {                                                             
		printf("iconv_open false: %s ==> %s\n", from, to);                                                            
		return -1;                                                                                                    
	}                                                                                                               
                                                                                                                  
//	iconv(c_pt, NULL, NULL, NULL, NULL);                                                                            
                                                                                                                  
	lenin  = length;                                                                                            
	lenout = MAX_LENGTH;                                                                                                
	sin    = (*inbuf);                                                                                            
	sout   = (*outbuf);                                                                                           
	ret = iconv(c_pt, &sin, &lenin, &sout, &lenout);
	printf("%s ==> %s: ret:%d, outbuffer length is %d\n", 
			from, to, ret, MAX_LENGTH-lenout);
	if (ret == -1) {                                                                                              
		printf("stop at: %s\n", sin);                                                                               
	}                                                                                                             
	iconv_close(c_pt);                             
	*outlen=MAX_LENGTH-lenout;
	return 0;                                                                                                       
}

int parse_url(char *in_buf, int in_length, char *out_buf, int *out_length){

	int temp32;
	int str_len;
	int i;
	char *buf;

	buf=(char *)malloc(in_length);
	if(buf==NULL){
		printf("error! alloc memeory error!\n");
		return 1;
	}
	memset(buf, 0, in_length);
	memcpy(buf, in_buf, in_length);
	str_len=in_length;

	for(i=0;i<str_len;i++){
		if(buf[i]=='%'){
			sscanf(buf+1+i, "%02x", &temp32);
			buf[i]=(unsigned char)temp32;
			memcpy(buf+i+1, buf+i+3, str_len-i-3);
			str_len-=2;
		}
	}
	memcpy(out_buf, buf, strlen(buf));
	*out_length=str_len;
	free(buf);

	return 0;
}
