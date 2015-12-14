////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: interface.h
// Description:
//	define the interface to ssl, appproc etc.   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

 
#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include "AppProc/ReturnCode.h"

//////////////////////////////////////////////////////////////////////////
// defined in CertVerify/aosAccessManage.h
//////////////////////////////////////////////////////////////////////////
struct aos_user;
struct tcp_vs_conn;
enum
{
	efrom_server = 0,
	efrom_client = 1,
	eCHECKING_PERMIT_READY = 0,
	eCHECKING_PERMIT_BUSY = 1,
};

//////////////////////////////////////////////////////////////////////////
//
// request
// rslt£º
//	eAosRc_ForwardToEnd 
//	eAosRc_Drop 
//	eAosRc_BackToSender
//	eAosRc_DoNothing 
// 					
//////////////////////////////////////////////////////////////////////////
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

// call ssl module process
enum{
	eForwardToSender,
	eAppData,
	eDoNothing,
	eDeferred
};

typedef  int (*sslcallback)(int rc, struct tcp_vs_conn *conn, char *rslt_data, int rslt_data_len);

extern int aos_ssl_proc(
		char *data, 
		unsigned int data_len,
		struct tcp_vs_conn *conn, 
		char **rslt_data, 
		unsigned int *rslt_data_len, 
		sslcallback callback);

// release buffer allocated by ssl module		
extern int aos_ssl_release_buf(char *rslt_data);

//////////////////////////////////////////////////////////////////////////
// before call aos_http_proc, call this function to
// check whether app has the ability to process data now
//
// return value:
// eCHECKING_PERMIT_BUSY
// eCHECKING_PERMIT_READY
//
//////////////////////////////////////////////////////////////////////////
extern int aos_httpmsg_checking_permit(struct tcp_vs_conn *conn);

//////////////////////////////////////////////////////////////////////////
// do application data processing
//
// return value: 
//	eAosRc_ForwardToPeer 
//	eAosRc_Drop 
//	eAosRc_ReturnToPeer
//	eAosRc_DoNothing 
//////////////////////////////////////////////////////////////////////////		
extern int aos_http_proc(
		char *app_data, 
		unsigned int app_data_len, 
		struct aos_user *user,
		char *sysname,
		struct tcp_vs_conn *conn,
		int  fromClient,
		int (*callback)(struct aos_app_proc_req *req));
		
//////////////////////////////////////////////////////////////////////////
// after aos_http_proc call back
// the caller should call this function.
//
// return value: 
//	eAosRc_ReqDesSuccess = 0,
//	eAosRc_ReqDesFailed = 1,
// 					
//////////////////////////////////////////////////////////////////////////
extern int http_app_proc_req_destructor(struct aos_app_proc_req *req);
extern int aos_httpmsg_appdata_destructor(struct tcp_vs_conn *conn);
#endif

