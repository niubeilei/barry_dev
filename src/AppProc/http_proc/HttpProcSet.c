////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpProcSet.c
// Description:
//             This file is used to set HttpProc   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include "aos/aosKernelApi.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "KernelSimu/kernel.h"
#include "KernelUtil/KernelMem.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/StringUtil.h"
#include "aos/aosKernelApi.h"


#include "AppProc/http_proc/HTTP.h"
#include "AppProc/http_proc/AppProcReq.h"
#include "AppProc/http_proc/HttpProcSet.h"

#ifndef AOS_USERLAND
#include <asm/delay.h>
#include <linux/delay.h>
#include <linux/random.h>
#endif

#include <linux/string.h> 
////////////////////////////////////////////////////////////////////
//
//	This function set 
//
////////////////////////////////////////////////////////////////////
char* sys_name_test1="aabbccddeeffgghhiijjkkllmmnnooppqq";

#define MAX_BUFF_SIZE 5000
#define READ_MAX 100
char *HTTP_DATA = NULL;
unsigned int HTTP_DATA_SIZE=0;

unsigned int THREAD_NUM = 1; 
unsigned int THREAD_COUNT = 1;
THREADTABLE *thread_table = NULL;
int end_flag = 0;


int tcp_vs_callback_multi_thread_debug(struct aos_app_proc_req *req)
{
	int ret;
    unsigned int index = 0;
    for(index = 0; index< THREAD_NUM; index++)
    {
        if (thread_table[index].conn == req->pTcpVsConn)
            break;
    }
    ret = memcmp(thread_table[index].recv_pos, req->submit_buff, req->submit_buff_len);
    thread_table[index].recv_pos += req->submit_buff_len;
    if (thread_table[index].recv_pos == (HTTP_DATA + strlen(HTTP_DATA)))
        thread_table[index].recv_pos = HTTP_DATA;
    if (ret != 0)
    {
        end_flag = 1;
        aos_trace("Parse Failed: \n%s\n", req->submit_buff);
        return ret;

    }
	else
    {
        aos_trace("********************************************************************************\n");
    }
    //OmnTest(ret == 0) << endtc;

    aos_app_proc_req_destructor(req);


    return 0;
}




int  MultiThreadInit(void)
{
	unsigned int i = 0;

	thread_table = (THREADTABLE*)aos_malloc(sizeof(THREADTABLE)*(THREAD_NUM+1));
	if (thread_table == NULL)
	{
		aos_trace("Pool Memeory!");
		return -1;
	}
	
	memset(thread_table, 0x00, sizeof(THREADTABLE)*(THREAD_NUM+1));	
	for (i=1; i<=THREAD_NUM; i++)
	{
		thread_table[i].conn = (struct tcp_vs_conn *)aos_malloc(sizeof(struct tcp_vs_conn));
		memset(thread_table[i].conn, 0x00, sizeof(struct tcp_vs_conn));
		if (!thread_table[i].conn)
		{
			aos_trace("Can't create conn");
		}
		thread_table[i].send_pos = HTTP_DATA;
		thread_table[i].recv_pos = HTTP_DATA;
		thread_table[i].count = 0;
	}
	return 0;
}

void MultiThreadDestroy(void)
{
	unsigned int i = 0;

	for (i = 1;i<=THREAD_NUM; i++)
	{
		aos_free(thread_table[i].conn);
	}
	aos_free(thread_table);
}




int aos_cert_module_set(char* data, 
			unsigned int *length, 
			struct aosKernelApiParms *parms, 
			char* errmsg, 
			const int errlen)
{
	char* buff = parms->mStrings[0];
	unsigned int flag = 0;
	aos_trace("Flag is %s\n", buff);

	*length = 0;

	flag = memcmp(buff, "on", 2);

	if (flag == 0)
	{
		aos_trace("it will call CertVerify Module");
		aos_check_permit_flag = 1;
	}
	else
	{
		aos_trace("it will not call CertVerify Module");
		aos_check_permit_flag = 0;
	}	
		
	return 0;
}


int aos_httpproc_test_run(char* data,
			unsigned int *length,
			struct aosKernelApiParms *parms,
			char* errmsg,
			const int errlen)
{
	unsigned int end_num = 0;
	unsigned int j = 0; 
	unsigned int index = 0;
	int reach_end = 0;
	int app_data_len;

	struct aos_user *user;	
	unsigned int file_len;
	*length = 0;
	
	HTTP_DATA = parms->mStrings[1];
	end_flag = 0;
	aos_trace("start");
	
	if (MultiThreadInit())
	{
		aos_trace("MultiThreadInit Failed!");
		return 0;
	}	
	aos_trace("Init ok!");
	
	if (HTTP_DATA == NULL)
	{
		aos_trace("HTTP_DATA == NULL");
		return 0;
	}
	file_len = strlen(HTTP_DATA);
	aos_trace("file_len:%d", file_len);
	
	user = (struct aos_user *)aos_malloc(sizeof(struct aos_user));
	if (user == NULL)
	{
		aos_trace("Can't create user struct");
		return 0;
	}
	memset(user, 0x00, sizeof(struct aos_user));
	
	while (1)
	{
#ifndef AOS_USERLAND
		mdelay(500);
#endif
			
		end_num = 0;
		for (j =1; j<= THREAD_NUM; j++)
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

			
		//index = 1+(int) (((float)THREAD_NUM)*rand()/(READ_MAX+1.0));
		index = 1;
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
			aos_trace("Parse Error!\n");
			break;
		}
		reach_end = 0;
		//app_data_len = 1+(int) (100.0*rand()/(READ_MAX+1.0));
		app_data_len = 50;

		if ((thread_table[index].send_pos + app_data_len)>(HTTP_DATA + file_len))
		{
			app_data_len = HTTP_DATA + file_len - thread_table[index].send_pos;

			thread_table[index].count ++;
			
			reach_end = 0;
		}
		if (eAosHttpmsgFlag_InvalidMsg == aos_http_proc(thread_table[index].send_pos, app_data_len, user, sys_name_test1, thread_table[index].conn, 1, tcp_vs_callback_multi_thread_debug))
		{
			end_flag = 1;
			break;
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
		
		return 0;
	}
	else
	{
		aos_trace("Tester Failed");
		return 0;
	}
	return 0;
}

int aos_httpproc_test_data_set(char* data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char* errmsg,
            const int errlen)
{
    char *contents;
    unsigned int contents_length = 0;
   
	 contents = parms->mStrings[0];
	*length = 0;

	contents_length = strlen(contents);
	
	if (contents_length > MAX_BUFF_SIZE)
	{
		aos_trace("Buffer size is too big");
		return 0;
	}

	if (HTTP_DATA != NULL)
	{
		aos_trace("Clean old buffer");
		aos_free(HTTP_DATA);
	}
	HTTP_DATA =(char*)aos_malloc(contents_length);
	HTTP_DATA_SIZE = contents_length;
	aos_trace("New Buffer set");
	return 0;
}
