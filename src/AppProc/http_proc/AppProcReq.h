////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProcReq.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

//#include "../Ktcpvs/tcp_vs_def.h"


//#include "common.h"

#ifndef _DEBUG
#include "aos/aosKernelAlarm.h"
#endif

struct aos_httpmsg;

struct aos_app_proc_req
{
	struct tcp_vs_conn* pTcpVsConn;
	char *submit_buff;
	unsigned int submit_buff_len;
	int from_client;
	int flags;
	int rslt;
	int error;	
};

//struct aos_app_proc_req* aos_app_proc_req_constructor(struct tcp_vs_conn *pTcpVsConn, struct aos_httpmsg *msg);
//void aos_app_proc_req_destructor(struct aos_app_proc_req* req);
struct aos_app_proc_req* aos_app_proc_req_constructor(struct tcp_vs_conn* pTcpVsConn, struct aos_httpmsg *msg);
void aos_app_proc_req_destructor(struct aos_app_proc_req* req);

