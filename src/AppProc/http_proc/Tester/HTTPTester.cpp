////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HTTPTester.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "AppProc/http_proc/Tester/HTTPTester.h"

#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Tracer.h"
#include "KernelSimu/in.h"
#include "Debug/Debug.h"
#include "KernelInterface/CliProc.h"
#include "Tester/Test.h"
#include "Tester/TestPkg.h"
#include "Tester/TestMgr.h"
#include "Util/OmnNew.h"
#include "Util/Random.h"
#include "Util/IpAddr.h"
#include "UtilComm/TcpClient.h"
#include "UtilComm/ConnBuff.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "AppProc/http_proc/HttpParser.h"
#include "AppProc/http_proc/HTTP.h"
#include "AppProc/http_proc/AppProcReq.h"
#include <stdlib.h>

extern char sys_name_test[200];
#define MAX_BUFF_SIZE 500000
extern char *HTTP_DATA;
extern unsigned int HTTP_DATA_SIZE;


char end_flag;
char *receive_pos;

int receive_count = 0;



char RESULT_BUFF[MAX_BUFF_SIZE];
char file_lock_init;
aos_mutex_t	file_lock;


typedef struct _THREAD_TABLE
{
	struct tcp_vs_conn *conn;
	char *send_pos;
	char *recv_pos;
	unsigned int  count;
	
}THREADTABLE;

extern unsigned THREAD_NUM; 
extern unsigned THREAD_COUNT;		//how many cycles it will test

THREADTABLE *thread_table;

int tcp_vs_callback_debug(struct aos_app_proc_req *req);
int tcp_vs_callback_multi_thread_debug(struct aos_app_proc_req *req);

bool HTTPTester::start()
{
	// 
	// Test default constructor
	//
	
	file_lock_init = 0;
	OmnTest(basicTest() == true) << "Single Thread failed" << endtc;
	//aos_trace("Thread=%u", THREAD_NUM);
	//OmnTest(MultiThreadTest() == true) << "Multi Thread failed" << endtc;
	return true;
}


void HTTPTester::MultiThreadInit()
{
	thread_table = (THREADTABLE*)aos_malloc(sizeof(THREADTABLE)*(THREAD_NUM+1));
	
	for (unsigned int i=1; i<=THREAD_NUM; i++)
	{
		thread_table[i].conn = (struct tcp_vs_conn *)malloc(sizeof(struct tcp_vs_conn));
		if (!thread_table[i].conn)
		{
			aos_trace("Can't create conn");
		}
		thread_table[i].send_pos = HTTP_DATA;
		thread_table[i].recv_pos = HTTP_DATA;
		thread_table[i].count = 0;
	}

	time_t stime;
	srand(time(&stime));
	
}

void HTTPTester::MultiThreadDestroy()
{
	for (unsigned int i = 1;i<=THREAD_NUM; i++)
	{
		aos_free(thread_table[i].conn);
	}
	aos_free(thread_table);
}
//////////////////////////////////////////////////////////////////////////////////////////////////
//
//	This Test simulates multi-connection 
//
////////////////////////////////////////////////////////////////////////////////////////////////////
bool HTTPTester::MultiThreadTest()
{	
	
	MultiThreadInit();

	end_flag = 0;

	
	unsigned int file_len = strlen(HTTP_DATA);
	
	struct aos_user * user = (struct aos_user *)aos_malloc(sizeof(struct aos_user));

	while (true)
	{
		usleep(500);	
		unsigned int end_num = 0;
		for (unsigned int j =1; j<= THREAD_NUM; j++)
		{
			if (thread_table[j].count == THREAD_COUNT)
			{
				end_num ++;
			}
		}
		if (end_num == THREAD_NUM)
		{
			aos_trace("Misson accomplished!");
			break;
		}

			
		unsigned int index = 1+(int) (((float)THREAD_NUM)*rand()/(RAND_MAX+1.0));
		if (THREAD_NUM == 1)
		{
			index = 1;
		}
		if (index > THREAD_NUM)
			continue;

		
		if (thread_table[index].count == THREAD_COUNT)
		{
			continue;
		}
		if (end_flag == 1)
		{
			printf("Parse Error!\n");
			break;
		}
		bool reach_end = false;
		int app_data_len = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
		if ((thread_table[index].send_pos + app_data_len)>(HTTP_DATA + file_len))
		{
			app_data_len = HTTP_DATA + file_len - thread_table[index].send_pos;

			thread_table[index].count ++;
			
			reach_end = true;
		}
		aos_trace("conn:%x, read from:%d, bytes: %d", thread_table[index].conn, thread_table[index].send_pos-HTTP_DATA, app_data_len);
		if (eAosHttpmsgFlag_InvalidMsg == aos_http_proc(thread_table[index].send_pos, app_data_len, user, sys_name_test, thread_table[index].conn, 1, tcp_vs_callback_multi_thread_debug))
		{
			return false;
		}
		if (reach_end)
		{
			thread_table[index].send_pos = HTTP_DATA;
		}
		thread_table[index].send_pos += app_data_len;
				
			
	}

	MultiThreadDestroy();
	if (end_flag == 0)
	{	
		aos_trace("Tester Success");
		
		return true;
	}
	else
	{
		aos_trace("Tester Failed");
		return false;
	}
	return true;
}	







////////////////////////////////////////////////////////////////////////////////////
//
//	This function simulates the Ktcpvs And test HTTP.c
//  Read HTTP data from file "Http_Get"
//  And Read a random size large buffer from file 
//
////////////////////////////////////////////////////////////////////////////////////
bool HTTPTester::basicTest()
{	
	
	end_flag = 0;
	receive_pos = HTTP_DATA;
	
	unsigned int file_len = strlen(HTTP_DATA);
	char* cursor = HTTP_DATA;
	
	struct aos_user * user = (struct aos_user *)aos_malloc(sizeof(struct aos_user));
	struct tcp_vs_conn * conn = (struct tcp_vs_conn *)aos_malloc(sizeof(struct tcp_vs_conn));
	srand(rand());
	
	while (true)
	{ 
		if (aos_httpmsg_checking_permit(conn) == eCHECKING_PERMIT_BUSY)
		{
			aos_trace("http proc is busy!");
			sleep(5);
			continue;
		}
		
		if (end_flag == 1)
		{
			printf("Parse Error!\n");
			return false;
		}
		bool reach_end = false;
		int app_data_len = 1+(int) (100.0*rand()/(RAND_MAX+1.0));
		if ((cursor + app_data_len)>(HTTP_DATA + file_len))
		{
			app_data_len = HTTP_DATA + file_len - cursor;
			reach_end = true;
		}
		aos_debug_log(eAosMD_AppProc, "from:%d, bytes: %d", cursor-HTTP_DATA, app_data_len);
		aos_http_proc(cursor, app_data_len, user, sys_name_test, conn, 1, tcp_vs_callback_debug);
		if (reach_end)
		{
			while (receive_pos < HTTP_DATA + file_len) 
			{
				sleep(1);
			}
			aos_http_proc(cursor, 0, user, sys_name_test, conn, 1, tcp_vs_callback_debug);
			aos_debug_log(eAosMD_AppProc, "reach the end");
			return true;
		}
		cursor += app_data_len;
		usleep(100);		
			
	}
	return true;
}	


int tcp_vs_callback_multi_thread_debug(struct aos_app_proc_req *req)
{	
	unsigned int index = 0;	
	for(index = 0; index< THREAD_NUM; index++)
	{
		if (thread_table[index].conn == req->pTcpVsConn)
			break;	
	}
	int ret = memcmp(thread_table[index].recv_pos, req->submit_buff, req->submit_buff_len);
	thread_table[index].recv_pos += req->submit_buff_len;
	if (thread_table[index].recv_pos == (HTTP_DATA + strlen(HTTP_DATA)))
		thread_table[index].recv_pos = HTTP_DATA;
	if (ret != 0)
	{
		end_flag = 1;
		printf("Parse Failed: \n%s\n", req->submit_buff);
		exit(1);

	}else
	{
		printf("********************************************************************************\n");	
	}
	//OmnTest(ret == 0) << endtc;
	
	aos_app_proc_req_destructor(req);

	
	return 0;
}

int tcp_vs_callback_debug(struct aos_app_proc_req *req)
{	

	if (file_lock_init == 0)
	{
		AOS_INIT_MUTEX(file_lock);
		file_lock_init = 1;
	}


	FILE *File;
	
	
	File = fopen("./Http_result", "a+");
	aos_lock(file_lock);
	if (!File)
	{
		aos_debug_log(eAosMD_AppProc, "Open Http_result file error!\n");
		exit(1);
	}
	if(fseek(File, 0, SEEK_END)<0)
	{
		aos_debug_log(eAosMD_AppProc, "Open Http_result file error!\n");
        	exit(1);
	}

	
	if(fwrite((char*)req->submit_buff, 1, req->submit_buff_len, File)<0)
	{
		aos_debug_log(eAosMD_AppProc, "Open Http_result file error!\n");
        	exit(1);
	}
	fclose(File);
	int ret = memcmp(receive_pos, req->submit_buff, req->submit_buff_len);
	
	
	if (ret != 0)
	{
		end_flag = 1;
		printf("Parse Failed: \n%s\n", req->submit_buff);
		exit(1);

	}else
	{
		aos_trace("receive from %d,  len %d, count %d", receive_pos - HTTP_DATA, req->submit_buff_len, ++receive_count);
		receive_pos += req->submit_buff_len;

	}
	//OmnTest(ret == 0) << "aa" << endtc;

	aos_app_proc_req_destructor(req);
	aos_unlock(file_lock);

	
	return 0;
}


#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef int (*AMM_CALLBACK) (struct aos_httpmsg *msg, int permit, struct aos_user *user, char *domain, char *sysname, char* resource);
struct check_permit_thread_args
{
	struct aos_httpmsg *msg;
	int operation;
	struct aos_user *user;
	char *domain;
	char *sysname;
	char *resource;
	AMM_CALLBACK callback;

};


////////////////////////////////////////////////////////////////////////////
//
// This is the thread function which  will callback 	
//
////////////////////////////////////////////////////////////////////////////
void* aos_check_permit_thread(void* args)
{
	
//	sleep(1+(int) (10.0*rand()/(RAND_MAX+1.0)));
	struct check_permit_thread_args *thread_args = (struct check_permit_thread_args*)args;
	AMM_CALLBACK callback = (AMM_CALLBACK)thread_args->callback;
	callback(thread_args->msg, eAOS_ACCESS_PERMIT, thread_args->user, thread_args->domain, thread_args->sysname, thread_args->resource);
	
	
	aos_free(args);	
    return NULL;	
}

////////////////////////////////////////////////////////////////////////////
//
//	This function simulates the aos_check_permit of CertVerify Module
//
////////////////////////////////////////////////////////////////////////////
int aos_check_permit(struct aos_httpmsg *msg, int operation, struct aos_user *user, char *domain, char* sysname, char* resource_name, int (*callback)(struct aos_httpmsg *msg, int permit, struct aos_user *user, char *domain, char* sysname, char* resource_name))
{
	//create a new thread and sleep for a while then callback HTTP_PROC
	pthread_t id;
	int ret;
	struct check_permit_thread_args *args = (struct check_permit_thread_args*)aos_malloc(sizeof(struct check_permit_thread_args));
	args->msg = msg;
	args->operation = operation;
	args->user = user;
	args->domain = domain;
	args->resource = resource_name;
	args->callback= (AMM_CALLBACK)callback;
	aos_debug_log(eAosMD_AppProc, "aos_check_permit: msg=%x, domain=%s, resource=%s", msg, domain, resource_name);
	ret=pthread_create(&id, NULL, aos_check_permit_thread, args);
	
	return 0;
}

#define TIMEOUT_SEC 5
#define SERVER_ADDR "219.239.95.131"
#define SERVPORT = 8080
#define MAXDATASIZE 5000
#define BACKLOG 10
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


/*
void HTTPTester::AutoTest()
{
	int sockfd, client_fd, redirect_fd, maxfd;
	struct sockaddr_in my_addr;
	struct sockaddr_in remote_addr;
	struct sockaddr_in server_addr;
	fd_set rfds;
	struct timeval tv;
	int retval;
	
	socklen_t sin_size = 0;
	
	int recvbytes;
	int sendbytes;
	char buf[MAXDATASIZE];

	//set timeout value
	tv.tv_sec = TIMEOUT_SEC;
	tv.tv_usec = 0;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket create error!");
		return;
	}
	my_addr.sin_family = AF_INET;
	//my_addr.sin_port = htons(SERVPORT);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(my_addr.sin_zero), 8);
	if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind error!");
		return;
	}

	printf("%s\n", inet_ntoa(my_addr.sin_addr));
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
				aos_http_proc(buf, recvbytes, user, "name" ,conn,1,  tcp_vs_callback_debug);
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

	return;


}
*/


