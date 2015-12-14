////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: main.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#include "common.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define SERVPORT 80
#define BACKLOG 10
#define SERVER_ADDR "192.168.1.6"
#define MAXDATASIZE 5000



#include "HTTP.h"
#include "AppProcReq.h"

#ifdef _DEBUG
#include "Ktcpvs/tcp_vs_def.h"
#include "CertVerify/aosAccessManage.h"
#else
#include "Ktcpvs/tcp_vs_def.h"
//#include "CertVerify/aosAccessManage.h"
#endif

#define BUFF_SIZE 50

struct aos_user
{
	int uid;
};
/*
void TestApp()
{
	char buff[BUFF_SIZE];
	FILE *pFile;
	size_t read;
	int i = 0;
	struct aos_user user;
	struct tcp_vs_conn conn;
	int fromClient = 1;
	void* callback;

	int size_array[100]={50,250,250,250,250, 250, 250, 250, 250, 250, 250};
	
	i=0;
	pFile = fopen("./app_data.txt", "r");
	do{
		memset(buff, 0x00, BUFF_SIZE);
		read = fread(buff, size_array[i], 1, pFile);
		
		aos_http_proc(buff, size_array[i], &user, &conn, fromClient, (int(*)(aos_app_proc_req*))callback);
		i++;
		
	}while (i < 10000);
	
	
}
*/

int tcp_vs_callback(struct aos_app_proc_req *req)
{
	aos_app_proc_req_destructor(req);
	return 0;
}
/*
int aos_httpmsg_check_permit_callback(struct aos_app_proc_req* req, int action)
{
	return 0;
}
*/
int aos_check_permit(struct aos_user *user, 
					 char* domain, 
					 unsigned short port, 
					 char* resource, 
					 struct aos_app_proc_req *req, 
					 int (*callback)(struct aos_app_proc_req *req, int permit))
{
	(callback)(req, eAosRc_ForwardToPeer);
	return 0;
}




int main(int argc, char *argv[])
{
	
	
//If data comes from data files
/*
	TestApp();
	return 0;
*/	

//	FILE *file;
	int sockfd,client_fd,redirect_fd, maxfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	struct sockaddr_in server_addr;
	fd_set rfds;
	struct timeval tv;
	int retval;
//	unsigned short nPort;
	socklen_t sin_size = 0;

	int recvbytes;
	int sendbytes;
	char buf[MAXDATASIZE];
//	char outbuf[2048];
//	int ReturnLength;
//	struct hostent *host;

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	
	
	
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0))== -1)
	{
		perror("socket create error!");
		exit(1);
	}
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(SERVPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero),8);
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind error!");
		exit(1);
	}

	printf("%s\n",inet_ntoa(my_addr.sin_addr));

	if (listen(sockfd,BACKLOG) == -1)
	{
		perror("listen error!");
		exit(1);
	}

	sin_size = sizeof(struct sockaddr_in);

	while (1)
	{
		if ((client_fd =(int) accept(sockfd, (struct sockaddr *)&remote_addr, &sin_size)) == -1)
		{
			perror("accept error!");
			continue;
		}
		break;
	}

	printf ("receive a connection from %s \n", inet_ntoa(remote_addr.sin_addr));
	
	if ((redirect_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("redirect socket create error!");
		exit(1);
	}
    
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(80);
	server_addr.sin_addr.s_addr  = inet_addr(SERVER_ADDR);

	//server_addr.sin_addr = *((struct addr_in *)host->h_addr);
	//bcopy(&(host->h_addr), &(server_addr.sin_addr), host->h_length);

	bzero(&(server_addr.sin_zero),8);
	if (connect(redirect_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("connect to redirect server error!");
		exit(1);
	}
	struct aos_user *user;
	struct tcp_vs_conn *conn = (struct tcp_vs_conn *)malloc(sizeof(struct tcp_vs_conn));
	
	
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(client_fd, &rfds);
		FD_SET(redirect_fd, &rfds);
		maxfd = (client_fd>redirect_fd)?(client_fd+1):(redirect_fd+1);
		retval = select(maxfd, &rfds, NULL, NULL, &tv);
		memset(buf, 0x00, MAXDATASIZE);

		if (FD_ISSET(client_fd, &rfds) == 1)
		{
			if ((recvbytes = recv(client_fd, buf, MAXDATASIZE, 0)) == -1)
			{
				perror("recv error!");
			}
			//if (recvbytes > 10)
			//{
			//	printf("Clinet->Server: %d bytes\n", recvbytes);
			//	printf("----------------------------------\n");
				aos_http_proc(buf, recvbytes, user, "name" ,conn,1,  tcp_vs_callback);
			//	printf("----------------------------------\n");
			//}
			
			
			
			
			if ((sendbytes = send(redirect_fd, buf, recvbytes, 0)) == -1)
			{
				perror("send error!");
			}
			continue;
		}

		if (FD_ISSET(redirect_fd, &rfds) == 1)
		{
			if ((recvbytes = recv(redirect_fd, buf, MAXDATASIZE, 0)) == -1)
			{
				perror("recv error!");
			}
			
			if ((sendbytes = send(client_fd, buf, recvbytes, 0)) == -1)
			{
				perror("send error!");
				exit(1);
			}
			continue;
		}
		continue;
	}  	

	return EXIT_SUCCESS;
}
