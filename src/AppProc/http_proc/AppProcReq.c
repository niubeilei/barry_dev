////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProcReq.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#ifndef _DEBUG
#include "aos/aosKernelAlarm.h"
#include "aosUtil/Alarm.h"
#include "KernelUtil/KernelMem.h"
#include "KernelSimu/string.h"
#include "aosUtil/Tracer.h"
#endif
//#include <string.h> // comment by liqin
#include "AppProc/ReturnCode.h"
#include "AppProc/http_proc/HTTP.h"
#include "AppProc/http_proc/AppProcReq.h"
#include "aosUtil/Memory.h"


// 
// This function allocates a new 'aos_app_proc_req' instance and 
// sets its members to their initial values. All should call this 
// function to allocate a new 'aos_app_proc_req'. 
//



//////////////////////////////////////////////////////////////////////////
//
//	aos_app_proc_req constructor
//
//////////////////////////////////////////////////////////////////////////
struct aos_app_proc_req* aos_app_proc_req_constructor(struct tcp_vs_conn* pTcpVsConn, struct aos_httpmsg *msg)
{
	// 
	// Not implemented yet
	//
	//aosAlarm(eAosAlarm_NotImplementedYet);
	struct aos_app_proc_req* ret = (struct aos_app_proc_req*)aos_malloc(sizeof(struct aos_app_proc_req));
	memset(ret, 0x00, sizeof(struct aos_app_proc_req));

	if (!ret)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcPoolMem, (""));
		return 0;
	}

	memset((char*)ret, 0x00, sizeof(struct aos_app_proc_req));
	ret->pTcpVsConn = pTcpVsConn;
	
	aos_debug_log(eAosMD_AppProc, "msg->buff_submit=0x%x, msg->buffer_submit_size=%d",
					msg->buffer_submit, msg->buffer_submit_size);
	ret->submit_buff =msg->buffer_submit;
	ret->submit_buff_len = msg->buffer_submit_size;
	
	msg->buffer_submit = 0;
	msg->buffer_submit_size = 0x00;
	aos_debug_log(eAosMD_AppProc, "ret->submit_buff=0x%x, ret->submit_buff_len=%d",
					ret->submit_buff, ret->submit_buff_len);
	return ret;
}


// 
// This function frees an 'aos_app_proc_req' instance and releases
// any resources held by this instance. All should call this function
// to free an 'aos_app_proc_req'. 
//
void aos_app_proc_req_destructor(struct aos_app_proc_req* req)
{
	// 
	// Not implemented yet
	//
	//aosAlarm(eAosAlarm_NotImplementedYet);
	aos_free(req->submit_buff);
	aos_free(req);
	return;
}
