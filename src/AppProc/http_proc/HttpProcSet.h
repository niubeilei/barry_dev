////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpProcSet.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

/*
typedef int (*AMM_CALLBACK) (struct aos_httpmsg *msg,
                          	int permit,
				struct aos_user *user,
				char *domain,
				char *sysname,
				char *resource_name);
				
*/
#include "AppProc/http_proc/HTTP.h"


extern AOS_CHECK_PERMIT fun_aos_check_permit;	//pointer to aos_check_permit
extern char aos_check_permit_flag;				//indicate that weather to call real aos_check_permit()

typedef struct _THREAD_TABLE
{
	struct tcp_vs_conn *conn;
	char *send_pos;
	char *recv_pos;
	unsigned int  count;
	
}THREADTABLE;

int aos_httpproc_test_run(char* data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char* errmsg,
            const int errlen);
int aos_cert_module_set(char* data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char* errmsg,
            const int errlen);
int aos_httpproc_test_data_set(char* data,
            unsigned int *length,
            struct aosKernelApiParms *parms,
            char* errmsg,
            const int errlen);


