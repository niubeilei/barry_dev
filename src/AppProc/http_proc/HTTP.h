////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HTTP.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#ifndef __HTTP_H__
#define __HTTP_H__
#ifdef _DEBUG
#include "../../Ktcpvs/tcp_vs_def.h"
#else
#include "Ktcpvs/tcp_vs_def.h"
#endif

#include "aos/aosReturnCode.h"
#include "CertVerify/aosAccessManage.h"

#define HTTP_HEADER_MAXSIZE 1024
//Added by lijing
#define UTF_SERVER_PORT 12004

typedef int (*AOS_CHECK_PERMIT)(struct aos_httpmsg *msg,
			int operation,
			struct aos_user *user,
			char *domain,
			char *sysname,
			char *resource_name,
			AMM_CALLBACK callback);

struct aos_app_proc_req;
enum
{
	eAosMaxDomainLen = 50,
	eAosMaxResourceLen = 1024,
	eAosHttpMsg_Complete= 1,
};

enum
{
	eAosHttpmsgFlag_NewMsg = 0x00, //new msg
	eAosHttpmsgFlag_MsgParsed = 0x01, //header parsed, but unknown transfer type
	eAosHttpmsgFlag_LengthIndicator_Chunked = 0x02,  //chunked mode
	eAosHttpmsgFlag_LengthIndicator_Multipart  =0x04,  //multi-part mode
	eAosHttpmsgFlag_LengthKnown = 0x08,  //content-length known
	eAosHttpmsgFlag_DoNothing = 0x10, //have not got the permission from OCSP
	eAosHttpmsgFlag_Drop = 0x20,  //drop the message
	eAosHttpmsgFlag_ReturnMsg = 0x40, 
	eAosHttpmsgFlag_ForwardToPeer = 0x80, 
	eAosHttpmsgFlag_NoLength = 0xff,
};

enum
{
	eAosHttpmsgFlag_InvalidMsg = -1,
	eAosHttpmsgFlag_ValidMsg = 0,
};	


#define HTTP_MAX_HEADER 65535	//max length of a Http Message Header
#define HTTP_DOMAIN_MIN 3
#define HTTP_RESOURCE_MIN 1
#define HTTP_MAX_MSG 5

extern unsigned int HTTP_BODY_MAX;

struct aos_httpmsg
{
	char*			buffer;
	unsigned int	buffer_size;
	char*			buffer_submit;
	unsigned int	buffer_submit_size;
	
	int 			checking_permit;
	char 			check_permit_result;//after check permit, according the return value ,set the variable

	unsigned int	msg_length;		// Message expected length
	unsigned int	length_parsed;	// The length being parsed so far

	struct tcp_vs_conn *conn;
	int (*callback)(struct aos_app_proc_req *req);
	char			flags;
	int 			operation;		// Get or Post
	char			domain[eAosMaxDomainLen];
	unsigned short  port;
	char 			sysname[eAosMaxResourceLen];
	char			resource[eAosMaxResourceLen];
	struct aos_user user;
	
	char 			complete; //if the message whole body has completed , complee = 1
	struct aos_httpmsg *next;
	atomic_t		refcnt;	// added by liqin	
};


struct _HttpAppData
{
	struct aos_httpmsg* outstanding_msg;
	char                    aos_httpmsg_lock_init; //indicate that the aos_httpmsg_lock has been inited
    aos_mutex_t             aos_httpmsg_lock;   //lock the outstanding_msg link table

};


enum ReturnCode
{
	HAVE_APP_DATA,
	HAVE_NO_MEM,
	CANT_CREATE_APP_DATA,
};


enum _HTTP_METHOD
{
	HTTP_METHOD_UNKNOWN=0,
	HTTP_METHOD_GET=1,
	HTTP_METHOD_POST=2,
	
};

enum
{
	EnCoding_Mime=1,
	EnCoding_Chunked=2,
	EnCoding_Length = 3,
};
	
enum
{
	eCHECKING_PERMIT_READY = 0,
	eCHECKING_PERMIT_BUSY = 1,
	
};

enum
{
	
	eAOS_ACCESS_PERMIT =  0,
	eAOS_ACCESS_DENY = 1,
	eAOS_ACCESS_DROP = -1,
	eAOS_ACCESS_UNKNOWN = -2,
	eAOS_ACCESS_WAITVERIFY = -3,
};
	
struct aos_httpmsg *aos_httpmsg_constructor(void);
int aos_httpmsg_destructor(struct aos_httpmsg *self);

int aos_httpmsg_append_data(struct aos_httpmsg *msg, char* app_data, unsigned int app_data_len);
int aos_httpmsg_cut_data(struct aos_httpmsg *msg, unsigned int message_size);

//////////////////////////////////////////////////////////////////////////
//
// Interface for Li Qin's code
// check the connection where is checking permission from OCSP
// if it is checking, return CHECKING_PERMIT_BUSY
// if it is not, return CHECKING_PERMIT_READY
//
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_checking_permit(struct tcp_vs_conn *conn);

//int (*aos_httpmsg_check_permit_callback)(struct aos_httpmsg* msg, int permit))struct aos_user;
//////////////////////////////////////////////////////////////////////////
//
// Interface for Li Qin's code
// Tcpvs will call this function.
//
// return value: 
//	eAosRc_ForwardToPeer 
//	eAosRc_Drop 
//	eAosRc_ReturnToPeer
//	eAosRc_DoNothing 
//////////////////////////////////////////////////////////////////////////
int aos_http_proc(char *app_data, 
		unsigned int app_data_len, 
		struct aos_user *user,
		char *sysname,
		struct tcp_vs_conn *conn,
	    int fromClient,
	    int (*callback)(struct aos_app_proc_req *req));
	
//////////////////////////////////////////////////////////////////////////
//
// Interface for Li Qin's code
// Tcpvs will call this function.
//
// return value: 
//	eAosRc_ReqDesSuccess = 0,
//	eAosRc_ReqDesFailed = 1,
// 					
//////////////////////////////////////////////////////////////////////////
int http_app_proc_req_destructor(struct aos_app_proc_req *req);

int aos_httpmsg_appdata_destructor(struct tcp_vs_conn *conn);
#endif

