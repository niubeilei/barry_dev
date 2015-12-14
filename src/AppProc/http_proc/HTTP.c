////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HTTP.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////



#include "AppProc/ReturnCode.h"
#include "AppProc/http_proc/AppProcReq.h"
#include "AppProc/http_proc/HttpParser.h"
#include "AppProc/http_proc/HTTP.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Memory.h"
#include "CertVerify/aosAccessManage.h" 
#include "AppProc/http_proc/url_decode.h"
#include "Ktcpvs/aosProxyConn.h"

#include <net/ip.h>
#include <net/sock.h>
#include <net/tcp.h>

#ifndef _DEBUG
#include "KernelSimu/gfp.h"
#include "aosUtil/Alarm.h"
#include "KernelUtil/KernelMem.h"
#include "CertVerify/aosAccessManage.h" 
#include "KernelSimu/string.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "aos/aosReturnCode.h"
#include "AppProxy/DenyPage.h"
#include "KernelPorting/Sock.h"
#endif


#ifdef _DEBUG
#include "../../Ktcpvs/tcp_vs_def.h"
#endif

AOS_CHECK_PERMIT fun_aos_check_permit;  //pointer to aos_check_permit

#ifdef __KERNEL__
char aos_check_permit_flag = 1;             //indicate that weather to call real aos_check_permit()

extern int tcp_vs_sendbuffer(struct socket *sock, const char *buffer, const size_t length, unsigned long flags);
extern int tcp_vs_recvbuffer(struct socket *sock, char *buffer,  const size_t buflen, unsigned long flags);

#else
char aos_check_permit_flag = 0;
#endif

char sys_name_test[200] = "/dc=公安涉密信息系统/dc=机要业务信息系统/dc=机要业务文电传输系统";

int aos_http_respnose_header(char *header, int header_len,int body_len)
{
	
	snprintf(header, header_len, "HTTP/1.1 200 OK\r\n"
								"Accept-Ranges: bytes\r\n"
								"Content-Length: %d\r\n"
								"Connection: close\r\n"
								"Content-Type: text/html; charset=UTF-8\r\n"
								"\r\n", body_len);

	return strlen(header);
}

//////////////////////////////////////////////////////////////////////////
// This function free an http message. It will release any resources
// held by this message, if any. All http messages should be freed
// by calling this function. 
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_release(struct aos_httpmsg *self)
{
//	aos_debug_log(eAosMD_AppProc, "Delete a msg =%x ", self);
	if (self->buffer)
	{
		aos_free(self->buffer);
		self->buffer = 0;
	}

	if (self->buffer_submit)
	{
		aos_free(self->buffer_submit);
		self->buffer_submit = 0;

	}
	
	aos_free(self);
	return 0;
}


static int aos_httpmsg_hold(struct aos_httpmsg *msg)
{
    aos_assert1(msg);
    atomic_inc(&msg->refcnt);
    return 0;
}

static int aos_httpmsg_put(struct aos_httpmsg *msg)
{
    aos_assert1(msg);
    if (atomic_dec_and_test(&msg->refcnt))
    {
        aos_httpmsg_release(msg);
    }
    return 0;
}

int HttpAppDataCreate(struct tcp_vs_conn * conn)
{
	struct _HttpAppData *http_app_data = NULL;
	
	if (conn->app_data == NULL)
	{
		http_app_data = (struct _HttpAppData*)aos_malloc(sizeof(struct _HttpAppData));
		memset(http_app_data, 0x0, sizeof(struct _HttpAppData));
		
		if (http_app_data == NULL)
		{
			return HAVE_NO_MEM;
		}

		AOS_INIT_MUTEX(http_app_data->aos_httpmsg_lock);
		
		conn->app_data = http_app_data;
		return 0;
		
	}
	else
	{
		return HAVE_APP_DATA;
	}
}

int HttpAppDataDestroy(struct tcp_vs_conn *conn)
{
	struct _HttpAppData *http_app_data = NULL;
	struct aos_httpmsg* http_msg =  NULL;
	struct aos_httpmsg* http_msg_next = NULL;
	if (conn->app_data == NULL)
	{
		return 0;
	}

	http_app_data = (struct _HttpAppData *)conn->app_data;
	aos_lock(http_app_data->aos_httpmsg_lock);

	if (http_app_data->outstanding_msg == NULL)
	{

		aos_free(http_app_data);
		return 0;
	}

	if (http_app_data->outstanding_msg != NULL)     //clear the msg link table
	{
		http_msg = http_app_data->outstanding_msg;
		
		while (http_msg != NULL)		//find the last msg
		{
			http_msg_next = http_msg->next;
			http_msg->next = NULL;
			aos_httpmsg_put(http_msg);
			http_msg = http_msg_next;
		}
	
	}
	aos_unlock(http_app_data->aos_httpmsg_lock);

	aos_free(http_app_data);
	return 0;
	
}

/*
struct aos_httpmsg *HttpAppDataAddMsg(struct tcp_vs_conn *conn)
{
	struct _HttpAppData * app_data = NULL;
	struct aos_httpmsg * msg = NULL;

	app_data = conn->app_data;
	if (app_data == NULL)
	{
		if (HttpAppDataCreate(conn)!=0)
		{
			aos_trace("Can't Create App Data Structure");
			return NULL;
		}
	}

	app_data = conn->app_data;
	if (app_data == NULL)
	{
		aos_trace("Can't Create App Data Structure");
		return NULL;
	}

	if (app_data->outstanding_msg == 0)// This is the first msg
	{
		msg = aos_httpmsg_constructor();//create a new msg struct
		if (!msg)
		{
			aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcMsgError, (""));
			return eAosRc_AppProcMsgError;
		}
		conn->outstanding_msg = msg;
		msg->conn = conn;
	}
	else
	{
		msg = (struct aos_httpmsg*)conn->outstanding_msg;
		
	}

	return msg;
	
	
}
*/

struct aos_httpmsg * HttpAppGetFrontMsg(struct tcp_vs_conn *conn)
{
	struct _HttpAppData * app_data = NULL;
	if (conn->app_data == NULL)
	{
		aos_trace("Error!There is no HttpAppData, but you want it");
		return NULL;
	}

	app_data = (struct _HttpAppData*)conn->app_data;
	if (app_data->outstanding_msg == NULL)
	{
		aos_trace("Error!This is no HttpAppData Msg, but you want it");
		return NULL;
	}

	return app_data->outstanding_msg;
	
 }

/*
 *  Add by lijing 12/12/2005
 *  Url parsing from UTF-8 to GB2312
 *  Judge input argument url wether UTF-8 charset. If yes continue, else return eAosRc_NotUTF8.
*/

static int judge_utf8(uint8_t *url)
{
	int i;
	
	for(i=0;i<strlen(url);i++)
	{
		if(url[i]==(uint8_t)'%')
		{
			return 0;
		}
	}
	return 1;
}

 static int aos_url_utf2gb(uint8_t *url, uint8_t *gb_url)
{
	struct socket *sock;
	struct sockaddr_in sin;
	uint8_t buffer[1024];
	int error;
	int len;
	
	aos_assert1(url);

	

	memset(buffer, 0, 1024);
	error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (error < 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error during creation of socket; terminating");
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(UTF_SERVER_PORT);

	error = sock->ops->connect(sock, (struct sockaddr*)&sin,sizeof(sin), 0);
	if (error < 0) 
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error connecting to the UTF8_to_GB server");
		sock_release(sock);
		return -1;
	}
	if (tcp_vs_sendbuffer(sock, url, strlen(url), 0) <= 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error sending request to the UTF8_to_GB server");
		sock_release(sock);
		return -1;
	}
	if ((len = tcp_vs_recvbuffer(sock, buffer, 1024, 0)) <= 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error receive response from the UTF8_to_GB server");
		sock_release(sock);
		return -1;
	}

	memcpy(gb_url, buffer, len);
	sock_release(sock);
	return eAosRc_Success;
}


/*End add*/

struct aos_httpmsg * HttpAppGetLastMsg(struct tcp_vs_conn *conn)
{
	struct _HttpAppData * app_data = NULL;
	struct aos_httpmsg *msg = NULL;

	if (conn ->app_data == NULL)
	{
		if (HttpAppDataCreate(conn) != 0)
		{
			aos_trace("Error! Can't Create HttpAppData Structure");
			return NULL;
		}

		
	}
	app_data = (struct _HttpAppData*)conn->app_data;
	
	msg = app_data->outstanding_msg;
	if (msg == NULL)
	{
		msg = aos_httpmsg_constructor();//create a new msg struct
		memset(msg, 0x0, sizeof(sizeof(struct aos_httpmsg)));
		
		if (!msg)
		{
			aos_trace("Error! can't create httpmsg");
			return NULL;
		}
		app_data->outstanding_msg= msg;
		msg->conn = conn;
		return msg;
	}


	while (msg->next !=NULL)
	{
		msg = msg->next;
	}
	return msg;
	
}

int HttpAppDataLock(struct tcp_vs_conn *conn)
{
	struct _HttpAppData * app_data = (struct _HttpAppData*)conn->app_data;
	if (app_data == NULL)
	{
		if (HttpAppDataCreate(conn)!=0)
		{
			return CANT_CREATE_APP_DATA;
		}
	}
	app_data = (struct _HttpAppData*)conn->app_data;
	
	aos_lock(app_data->aos_httpmsg_lock);
	return 0;
}

int  HttpAppDataUnlock(struct tcp_vs_conn *conn)
{
	struct _HttpAppData * app_data = (struct _HttpAppData*)conn->app_data;
	aos_unlock(app_data->aos_httpmsg_lock);
	return 0;
	
}

void change_resource_format(char *resource, int len)
{
	int slash_len = 0;
	char *p = resource;
	//
	//	Add by lijing
	//
	char gb_url[1024];
	
	while (p < resource+len)
	{
		if( *p == '/' )
			slash_len++;
		else
			break;
		p++;
	}
/*
	if (slash_len)
	{
		//memcpy(resource, resource+slash_len-1, len-slash_len);
		memcpy(resource, resource+slash_len, len-slash_len);
		resource[len-slash_len] = 0;
	}

	for (i=0; i<len-slash_len; i++)
	{
		if(resource[i] == '/') resource[i] = '\\';
	}
*/

	aos_debug("origin url: %s\n", resource);
	if(judge_utf8((uint8_t*)resource)==0)
	{
		memset(gb_url, 0, 1024);
		if(aos_url_utf2gb((uint8_t*)resource, gb_url)==eAosRc_Success)
		{
			memset(resource, 0, len);
			if(len>=strlen(gb_url))
			{
				memcpy(resource, gb_url, strlen(gb_url));
			}
		}
	}
	aos_debug("convert url: %s \n", resource);

}

//////////////////////////////////////////////////////////////////////////
//
// Interface for Li Qin's code
// check the connection where is checking permission from OCSP
// if it is checking, return CHECKING_PERMIT_BUSY
// if it is not, return CHECKING_PERMIT_READY
//
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_checking_permit(struct tcp_vs_conn *conn)
{
	int msg_count = 0;
	struct aos_httpmsg *msg = NULL;
	struct _HttpAppData *app_data = (struct _HttpAppData*)conn->app_data;
	
	if (app_data == NULL)
	{
		return eCHECKING_PERMIT_READY;
	}
	msg = (struct aos_httpmsg*)app_data->outstanding_msg;
	
	
	while (msg != NULL)
	{
		msg_count++;
		msg = msg->next;
	}
	
	if (msg_count >= HTTP_MAX_MSG)
		return eCHECKING_PERMIT_BUSY;
	else
		return eCHECKING_PERMIT_READY;

}

//////////////////////////////////////////////////////////////////////////
//
//
/////////////////////////////////////////////////////////////////////////
int http_app_proc_req_destructor(struct aos_app_proc_req *req)
{
	aos_assert1(req);
	if (req->submit_buff)
		aos_free(req->submit_buff);
	aos_free(req);
	return eAosRc_ReqDesSuccess;
}

//////////////////////////////////////////////////////////////////////////
//
// If the data comes from Server, switch the data to peer directly
//////////////////////////////////////////////////////////////////////////
int http_app_proc_server_data(char *app_data, 
unsigned int app_data_len,
struct aos_user *user,
struct tcp_vs_conn *conn,
int (*callback)(struct aos_app_proc_req *req))
{
	struct aos_httpmsg *msg;
	struct aos_app_proc_req *req = NULL;
	
	msg = HttpAppGetLastMsg(conn);
	//if there is no HttpAppData, create it
	//if there is no msg link table, create it

	aos_httpmsg_append_data(msg, app_data, app_data_len);
	aos_httpmsg_cut_data(msg, app_data_len);	//switch all data
	
	req = (struct aos_app_proc_req*)aos_app_proc_req_constructor(conn, msg);
	if (!req)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcReqError, (""));
		return eAosRc_AppProcReqError;
	}
	req->pTcpVsConn = conn;
	(callback)(req);
	return eAosRc_ForwardToPeer;	
}

////////////////////////////////////////////////////////////////////////////
// This function allocates a new http message and sets all its
// values to their initial values. All http messages should be
// created by calling this function.
//////////////////////////////////////////////////////////////////////////
struct aos_httpmsg *aos_httpmsg_constructor(void)
{

	struct aos_httpmsg *msg = (struct aos_httpmsg*)aos_malloc(sizeof(struct aos_httpmsg));
	if (!msg)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcPoolMem, ("http msg alloc failed"));
		return 0;
	}
	memset(msg, 0x00, sizeof(struct aos_httpmsg));	
	msg->check_permit_result = eAOS_ACCESS_UNKNOWN;
	atomic_set(&msg->refcnt, 1);
	return msg;
}





int aos_httpmsg_appdata_destructor(struct tcp_vs_conn *conn)
{
/* commented by Liqi at 2005.12.14
    struct aos_httpmsg *msg;
    struct aos_httpmsg *msg_next;

    aos_assert1(conn);
    if (!conn->aos_httpmsg_lock_init)
    {
        AOS_INIT_MUTEX(conn->aos_httpmsg_lock);
        conn->aos_httpmsg_lock_init = 1;
    }
    aos_lock(conn->aos_httpmsg_lock);
    msg = (struct aos_httpmsg *)conn->outstanding_msg;
    while( msg)
    {
        msg_next = msg->next;
        aos_httpmsg_put(msg);
        msg = msg_next;
    }
    aos_unlock(conn->aos_httpmsg_lock);
	return 0;
*/
	if (HttpAppDataDestroy(conn) !=0)
	{
		aos_trace("destroy HttpAppData structure Failed!");
		return -1;
	}
	return 0;
	
}

///////////////////////////////////////////////////////////////////////////
// When finish processing a message, clear all the information in the 
// struct aos_httpmsg. And prepare to receive a new message.
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_reset(struct aos_httpmsg *self)
{
	self->flags = 0;
	self->length_parsed = 0;
	self->msg_length = 0;
	self->port = 0;
	memset(self->domain, 0x00, eAosMaxDomainLen);
	memset(self->resource, 0x00, eAosMaxResourceLen);
	memset(self->sysname, 0, sizeof(self->sysname));
	return 1;
}

// rm the parsed data in msg, this is the front part of data in msg
static int aos_httpmsg_rm_data(struct aos_httpmsg *msg, int rm_len)
{
	char *new_buffer = NULL;

	aos_assert1(msg);

	if (rm_len >= msg->buffer_size)
	{
		aos_free(msg->buffer);
		msg->buffer_size = 0;
		msg->buffer = NULL;
		return 0;
	}

	new_buffer = (char*)aos_malloc(msg->buffer_size - rm_len);
	aos_assert1(new_buffer);

	memcpy(new_buffer, msg->buffer+rm_len, msg->buffer_size-rm_len);
	aos_free(msg->buffer);
	msg->buffer =  new_buffer;
	msg->buffer_size = msg->buffer_size - rm_len;
	return 0;
}

//////////////////////////////////////////////////////////////////////////
//
// This function append the new app_data to msg->buff
// First of all create a bigger buff
// Then copy original data in msg->buff to new buff
// and copy new data to new buff
//
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_append_data(struct aos_httpmsg *msg, char* app_data, unsigned int app_data_len)
{
	unsigned int new_buff_size = msg->buffer_size + app_data_len;
	char* new_buff = (char*)aos_malloc(new_buff_size);
	if (new_buff == NULL)
	{
		aos_trace("Error!Pool Mem!");
		return -1;
	}
	memset(new_buff, 0x00, new_buff_size);
	
	if (new_buff == 0)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcPoolMem, (""));
		aos_debug_log(eAosMD_Platform, "app_data_len =%d", app_data_len);
		return 0;
	}
	memset(new_buff, 0x00, new_buff_size);
	
	memcpy(new_buff, msg->buffer, msg->buffer_size);
	memcpy(new_buff + msg->buffer_size, app_data, app_data_len);

	if (msg->buffer != NULL)
	{
		aos_free(msg->buffer);
	}
	
	msg->buffer = new_buff;
	msg->buffer_size = new_buff_size;
	
	return 1;

}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
//
//	If msg->buff contains some data can be submitted, this function will 
//	cut the the buff into two parts. one is contained in msg->buff_submit
//	the other one is still in msg->buff
//
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_cut_data(struct aos_httpmsg *msg, unsigned int message_size)
{
	char* new_buff_submit = (char*)aos_malloc(message_size + msg->buffer_submit_size);
	char* new_buff_left = NULL;
	
	if (!new_buff_submit)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcPoolMem, ("%d"), msg->buffer_size + message_size);
		return 0;
	}
	memset(new_buff_submit, 0x00, message_size + msg->buffer_submit_size);

	new_buff_left = (char*)aos_malloc(msg->buffer_size - message_size);
	if (!new_buff_left)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_AppProcPoolMem, ("%d"), msg->buffer_size - message_size);
		return 0;
	}
	memset(new_buff_left, 0x00, msg->buffer_size - message_size);

	memcpy(new_buff_submit, msg->buffer_submit, msg->buffer_submit_size);
	memcpy(new_buff_submit + msg->buffer_submit_size, msg->buffer, message_size);
	memcpy(new_buff_left, msg->buffer + message_size, msg->buffer_size - message_size);
	
	aos_free(msg->buffer);
	aos_free(msg->buffer_submit);
	
	msg->buffer = new_buff_left;
	msg->buffer_size = msg->buffer_size - message_size;

	msg->buffer_submit = new_buff_submit;
	msg->buffer_submit_size += message_size;

	return 1;

}
//////////////////////////////////////////////////////////////////////////

int create_deny_page(struct aos_app_proc_req *req, struct tcp_vs_service *svc)
{
	int header_len;
	int deny_page_len;
	int buff_size;
	if (req->submit_buff)
	{
		aos_free(req->submit_buff);
		req->submit_buff = NULL;
		req->submit_buff_len = 0;
	}
	if (svc->deny_page && svc->deny_page->page_contents)
	{
		deny_page_len = strlen(svc->deny_page->page_contents);
		buff_size = HTTP_HEADER_MAXSIZE+deny_page_len+1;
		req->submit_buff = (char*)aos_malloc(buff_size);
		
		if (!req->submit_buff)
		{
			aos_alarm(eAosMD_AppProc, eAosAlarm_MemErr, "malloc memory for submit buffer failed");
			return -1;
		}
		memset(req->submit_buff, 0x00, buff_size);

		// create response header
		header_len = aos_http_respnose_header(req->submit_buff, HTTP_HEADER_MAXSIZE, deny_page_len);
		if (header_len <= 0)
		{
			aos_alarm(eAosMD_AppProc, eAosAlarm_MemErr, "deny page header created error");
			return -1;
		}
		strncpy(req->submit_buff+header_len, svc->deny_page->page_contents, buff_size-header_len);
		req->submit_buff_len = strlen(req->submit_buff);
	}
	
	return eAosRc_Success;
}

//////////////////////////////////////////////////////////////////////////
//
// 	search the conn->outstanding_msg link table. Transfer every msg
//
//////////////////////////////////////////////////////////////////////////
int aos_transfer_msg(struct tcp_vs_conn *conn)
{
	struct aos_httpmsg *front_msg = NULL;
	struct aos_httpmsg *msg = NULL;
	struct aos_app_proc_req* req  = NULL;
	struct _HttpAppData* app_data = NULL;
	
	//find the first message which has not benn parsed yet
	front_msg = HttpAppGetFrontMsg(conn);
	
   	if (front_msg == NULL)
	{
		aos_debug_log(eAosMD_AppProc, "Error! conn->outstanding_msg == NULL");
		
		return -1;
	}
	
	//search  the msg link find the message before this msg to be processed
	//aos_debug_log(eAosMD_AppProc, "(front_msg->flags&eAosHttpmsgFlag_MsgParsed) =%d", (front_msg->flags&eAosHttpmsgFlag_MsgParsed) );
	while (front_msg->flags != eAosHttpmsgFlag_NewMsg)
	{
		if ((front_msg->check_permit_result == eAOS_ACCESS_UNKNOWN)||(front_msg->check_permit_result == eAOS_ACCESS_WAITVERIFY)) //this request has not been verifeid
		{
			return 0;
		}
		// construct the req structure and process it
		req = (struct aos_app_proc_req*)aos_app_proc_req_constructor(front_msg->conn, front_msg);
		if (!req)
		{
			aos_debug_log(eAosMD_AppProc, "http check permit callback, can not alloc aos_app_proc_req");
			return -1;
		}
	
		req->pTcpVsConn = conn;
	
		switch (front_msg->check_permit_result)
		{
			case eAOS_ACCESS_PERMIT:
				//aos_debug_log(eAosMD_AppProc, "http access permit");
				req->rslt = eAosRc_ForwardToPeer;
				break;
			case eAOS_ACCESS_DENY:
				//aos_debug_log(eAosMD_AppProc, "http access denyed");
				req->rslt = eAosRc_ReturnToPeer;
				if(create_deny_page(req, front_msg->conn->svc)!=eAosRc_Success){
					req->rslt=eAosRc_Drop;
				}
				break;
			default:
				//aos_debug_log(eAosMD_AppProc, "http access control failed");
				req->rslt = eAosRc_Drop;
				break;
		}
		//aos_debug_log(eAosMD_AppProc, "get callback from pmi module, call tcp proxy call back, req->conn=0x%x, req->rslt=%d", req->pTcpVsConn, req->rslt);	
		if (front_msg->callback)
			front_msg->callback(req);
		
		if (front_msg->complete == 1)
		{
			//destroy the msg
			//aos_debug_log(eAosMD_AppProc, "Message Complete!");
			//aos_debug_log(eAosMD_AppProc, "Conn= %x", front_msg->conn);
			msg = front_msg;
			app_data = (struct _HttpAppData*)conn->app_data;
			
			app_data->outstanding_msg = front_msg->next;
			front_msg = front_msg->next;
			aos_httpmsg_put(msg);
			if (front_msg == NULL)
			{
				//aos_debug_log(eAosMD_AppProc, "There is no Message left");
				break;
			}
			continue;
		}
		break;		
		

	}
	return 0;
	
}


//////////////////////////////////////////////////////////////////////////
//
// 	CALLBACK function. When call the "check_permit_callback".Pass it to 
//  the function.
//
//////////////////////////////////////////////////////////////////////////
int aos_httpmsg_check_permit_callback(
		struct aos_httpmsg *msg, 
		int permit,  
		struct aos_user *user,
        char *domain,
        char *sysname,
        char *resource_name)
{
	
	struct tcp_vs_conn *conn = NULL;
	

	//aos_debug_log(eAosMD_AppProc, "http check permit callback, msg 0x%x, msg->conn=0x%x, permit is %d", msg, msg->conn, permit);
	if (!msg)
	{
		aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, "http check permit callback, msg is null");
		return -1;
	}

	
	//lock the outstanding_msg link table
	conn = msg->conn;
	HttpAppDataLock(conn);
	
	/*
	commented by liqi at 2005.12.14
	if (!conn->aos_httpmsg_lock_init)
	{
		AOS_INIT_MUTEX(conn->aos_httpmsg_lock);
		conn->aos_httpmsg_lock_init = 1;
	}
	aos_lock(conn->aos_httpmsg_lock);
	*/
	// according the return value of checking permit, set the msg->check_pertmit_result
	switch (permit)
	{
		case eAOS_ACCESS_PERMIT:
			//aos_debug_log(eAosMD_AppProc, "http access permit");
			msg->check_permit_result = eAOS_ACCESS_PERMIT;
			break;
		case eAOS_ACCESS_DENY:
			//aos_debug_log(eAosMD_AppProc, "http access denyed");
			msg->check_permit_result = eAOS_ACCESS_DENY;
			break;
		case eAOS_ACCESS_DROP:
			//aos_debug_log(eAosMD_AppProc, "http access control failed");
			msg->check_permit_result = eAOS_ACCESS_DROP;
			break;
		default:
			//aos_debug_log(eAosMD_AppProc, "http access control failed");
			msg->check_permit_result = eAOS_ACCESS_WAITVERIFY;
			break;
	}

	msg->checking_permit = eCHECKING_PERMIT_READY;	
	
	if (aos_transfer_msg(msg->conn) < 0)
	{
		aos_debug_log(eAosMD_AppProc, "Error in aos_transfer_msg");
	}
	aos_httpmsg_put(msg);
	HttpAppDataUnlock(conn);
	AosAppProxy_connPut(conn);
	return eAosRc_Success;
}

//
// This function parses the data 'app_data' as an http message. 
// The parsed results are set to 'msg'. The caller should have
// allocated the message. This function assumes the http message
// starts at app_data[*cursor]. 'app_data' may not end the message, 
// or it may contain more than one message. If 'app_data' does not
// end the message, '*msg_complete' is set to 0. If 'app_data' 
// contains more than one message, this function will stop at 
// the end of the current message. '*msg_complete' is set to 1, 
// '*cursor' is set to the beginning of the next message. 
//
// 'msg->buffer' is the memory buffer that holds the original 'app_data'.
// If the message needs to be forwarded, it will forward msg->buffer.
// If 'app_data' contains only parts of a message, 'app_data' is appended
// to msg->buffer. The parsing will not begin until the entire message
// has been received. This is to make the program simpler.
//
int aos_httpmsg_parse(char *app_data, 
					  unsigned int app_data_len,
					  unsigned int *cursor,
					  unsigned char *msg_complete,
struct aos_httpmsg *msg)
{
	
	// first of all, judge the state 
	//char permit_state = msg->flags&0xF0;
	char msg_state = msg->flags&0x0F;
	char transfer_coding = 0;
	unsigned int msg_length = 0;
	unsigned int parsed_length = 0;	
	unsigned int return_len = 0;
	char *p = NULL;

	if (msg->flags == eAosHttpmsgFlag_NewMsg) //This is a new Message
	{
		//find its header
		
		unsigned int header_len;
		if (check_valid(msg->buffer, msg->buffer_size) == eAosHttpmsgFlag_InvalidMsg)
		{
//			aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, "Invalid http req message");
			aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_InvalidMsg;
		}
		if (msg->buffer_size > HTTP_MAX_HEADER)
		{
//			aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, "Invalid msg! Too long buffer but no header in it");
			aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_InvalidMsg;
		}

		header_len = find_header(msg->buffer, msg->buffer_size);

		if (header_len > HTTP_MAX_HEADER)
		{
//			aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, "Invalid msg!find header but too long");
			aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_InvalidMsg;
		}

		if (header_len == 0)
		{
			*cursor = app_data_len;
			//aos_alarm(eAosMD_AppProc, eAosAlarm_ProgErr, "no header found in msg");
			//aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_NewMsg; //Can't find its header, it is not a complete message
		}

		//Parse the header
		return_len = 0;
		
		msg->operation = GetMsgType(msg->buffer, header_len);
		HttpRequestParse2HostName(msg->buffer, header_len, msg->domain,(unsigned int)eAosMaxDomainLen, &return_len);
		HttpRequestParse2Port(msg->buffer, header_len, &(msg->port));
		HttpRequestParse2Resource(msg->buffer, header_len, msg->resource, eAosMaxResourceLen, &return_len);
		
		if (strlen(msg->domain) < HTTP_DOMAIN_MIN)
		{
			//aos_trace("msg->domain too short");
			aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_InvalidMsg;
		}	
		if (strlen(msg->resource) < HTTP_RESOURCE_MIN)
		{
			//aos_trace("msg->resource too short");
			aos_httpmsg_rm_data(msg, msg->buffer_size);
			return eAosHttpmsgFlag_InvalidMsg;
		}
		msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_MsgParsed;		
		msg->flags = (msg->flags & 0x0F) + eAosHttpmsgFlag_DoNothing;
		//aos_trace("Url: http://%s:%d%s\n", msg->domain, msg->port, msg->resource);
		
		//determine the transfer type
		//set the type flag
		transfer_coding = 0;
		msg_length = GetTransferTypeFromHeader(msg->buffer, header_len, &transfer_coding);
	
		if ((msg_length > 0)&&(transfer_coding == eAosHttpmsgFlag_LengthKnown)) //it has "Content-Length" in the header
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthKnown;//set the type flag
			msg->msg_length = msg_length; 	
			//assert("Length: %u bytes\n", msg_length); 
		}
		else if ((msg_length == 0)&&(transfer_coding == eAosHttpmsgFlag_LengthIndicator_Chunked))//chunked mode
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthIndicator_Chunked;
		}
		else if ((msg_length == 0)&&(transfer_coding == eAosHttpmsgFlag_LengthIndicator_Multipart))// Multipart mode
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthIndicator_Multipart;
		}
		else if ((msg_length == 0)&&(transfer_coding == 0))
		{
			//unknown transfer type.
			//check it's method
			//if it is "GET" message. It is complete.
			if (GetMsgType(app_data, app_data_len) == HTTP_METHOD_GET)
			{
				msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthKnown;
				msg->msg_length = 0;
				*msg_complete = 1;
			}
				
		}
		*cursor = 0;
		//aos_debug_log(eAosMD_AppProc, "header_len=%d", header_len);
		aos_httpmsg_cut_data(msg, header_len);// cut the header from message
		return eAosHttpmsgFlag_MsgParsed;
	
	}
	else if (msg_state & eAosHttpmsgFlag_MsgParsed) //It's header has parsed. But have not confirmed its transfer type
	{
		//find its transfer type
		//find ending of the message
		transfer_coding = 0;
		parsed_length = 0;
		msg_length = GetTransferTypeFromHeader(app_data, app_data_len, &transfer_coding);
		
		
		if ((msg_length ==0)&&(transfer_coding == (char)eAosHttpmsgFlag_NoLength))//If it has just a header ,and  has no body
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthKnown;
			msg->msg_length = 0;
			*msg_complete = 1;//It indicate the message has completed
			parsed_length = app_data_len;
			
		}
		else if ((msg_length > 0)&&(transfer_coding == eAosHttpmsgFlag_LengthKnown)) //These is "Content-Length"in the header
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthKnown;// set the type flag
			p = (char*)GetContentLength(app_data, app_data_len);
			
			msg->msg_length = msg_length; 
			parsed_length = p - app_data;
			
		}
		else if ((msg_length == 0)&&(transfer_coding == eAosHttpmsgFlag_LengthIndicator_Chunked))//chunked mode
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthIndicator_Chunked;
			parsed_length = find_chunked_end(app_data, app_data_len);
			if (parsed_length == 0)
				parsed_length = app_data_len;
			
		}
		else if ((msg_length == 0)&&(transfer_coding == eAosHttpmsgFlag_LengthIndicator_Multipart))// Multipart mode
		{
			msg->flags = (msg->flags & 0xf0) + eAosHttpmsgFlag_LengthIndicator_Multipart;
			parsed_length = find_multipart_end(app_data, app_data_len);
			if (parsed_length == 0)
				parsed_length = app_data_len;
	
		}
		else if ((msg_length == 0)&&(transfer_coding == 0))
		{
			//unknown transfer tyep
			parsed_length = app_data_len;
		}
		aos_httpmsg_cut_data(msg, parsed_length);//cut the parsed part from message.
		return eAosHttpmsgFlag_MsgParsed;
	}
	else //has find its transfer-type, just find out it's ending of message
	{
		
		unsigned int parsed_length = 0;
		if (msg_state & eAosHttpmsgFlag_LengthIndicator_Chunked)//chunked mode
		{	
			
			parsed_length = find_chunked_end(app_data, app_data_len);
			if (parsed_length == 0)
			{
				parsed_length = app_data_len; 
				*msg_complete = 0;
			}
			else
			{
				*msg_complete = 1;
			}
				
			
		}
		else if (msg_state & eAosHttpmsgFlag_LengthIndicator_Multipart) //Multipart mode
		{
			//find out "-----------xxxxxx--" boundary
			parsed_length = find_multipart_end(app_data, app_data_len);
			if (parsed_length == 0)
			{	
				parsed_length = app_data_len;
				*msg_complete = 0;
			}
			else
			{
				*msg_complete = 1;
			}
				
		}
		else if (msg_state & eAosHttpmsgFlag_LengthKnown)//have known its length we expected.
		{
			
			if (msg->buffer_size >= msg->msg_length)//found ending
			{
				*msg_complete =1;
				parsed_length = msg->msg_length;
				msg->msg_length = 0;
			}
			else	//there is more data
			{
				*msg_complete = 0;
				parsed_length = app_data_len;
				msg->msg_length = msg->msg_length - app_data_len;
				*msg_complete = 0;
			}
			
		}
		*cursor = 0;
		aos_httpmsg_cut_data(msg, parsed_length);
		return eAosHttpmsgFlag_MsgParsed;

	}
	
}


#ifndef __KERNEL__
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

struct thread_param
{
	struct aos_httpmsg* msg;
	int permit;
	struct aos_user *user;
	char* domain;
	char* sysname;
	char* resource_name;
	AMM_CALLBACK callback;
};
void *thread(void *param)
{
	int i;
	struct thread_param*  tparam= (struct thread_param*)param;
	i = 1+(int) (60.0*rand()/(RAND_MAX+1.0));
	aos_trace("Pmi Delay %d s, and then callback", i);
	sleep(i);
	aos_trace("Pmi callback");
	(tparam->callback)(tparam->msg, eAOS_ACCESS_PERMIT, tparam->user, tparam->domain, tparam->sysname, tparam->resource_name);
	aos_free(tparam);
	pthread_exit(NULL);
	return NULL;
	
}

#endif
//////////////////////////////////////////////////////////////////////////
//
//	Here 
//  
//
//////////////////////////////////////////////////////////////////////////
int aos_check_permit_debug(struct aos_httpmsg *msg,
			int operation,
			struct aos_user *user,
			char *domain,
			char *sysname,
			char *resource_name,
			AMM_CALLBACK callback)
{

#ifndef __KERNEL__

	
	struct thread_param * param = (struct thread_param*)aos_malloc(sizeof(struct thread_param));
	pthread_t id;
	int ret;
	
	param->msg = msg;
	param->permit = eAosRc_ForwardToPeer;
	param->user = user;
	param->domain = domain;
	param->sysname = sysname;
	param->resource_name = resource_name;
	param->callback = callback;
	
	ret = pthread_create(&id, NULL, thread, param);
#endif
	return eAosRc_Success;
	
		
}


int call_aos_check_permit(struct aos_httpmsg *msg,
			int operation,
			struct aos_user *user,
			char *domain,
			char *sysname,
			char *resource_name,
			AMM_CALLBACK callback)
{
	if (aos_check_permit_flag == 0)
	{
		//aos_trace("It will call aos_check_permit_debug()");
		return aos_check_permit_debug(msg, operation, user, domain, sysname, resource_name, callback);
	}	
	else
	{
//		aos_trace("It will call aos_check_permit()");
		return aos_check_permit(msg, operation, user, domain, sysname, resource_name, callback);
	}

}
char *gresource="FileSystem\\查询服务\\SndFileQueryDS.aspx";
char *gserial_number="A0200000000748";
//////////////////////////////////////////////////////////////////////////
//
//	Main function.
//  called from tcpvs
//
//////////////////////////////////////////////////////////////////////////
int aos_http_proc(char *app_data, 
				  unsigned int app_data_len, 
				  struct aos_user *user,
				  char *sysname,
				  struct tcp_vs_conn *conn,
				  int fromClient,
	              		  int (*callback)(struct aos_app_proc_req *req))
{
	struct aos_httpmsg *msg;
	struct aos_httpmsg *msg_new;
	unsigned char msg_complete = 0;	//indicates that whether this message is complete.
	unsigned int cursor = 0;
	int ret;
	
	//aos_debug_log(eAosMD_AppProc, "aos_http_proc entry, conn=0x%x, app_data_len=%d, fromClient=%d", conn, app_data_len, fromClient);

//	hardcode for cert number
//	memcpy(user->data, gserial_number, 14);
//	user->len=14;	
	
	if (!fromClient)
	{
		//if the data comes from server ,just switch it to peer.
		//aos_debug_log(eAosMD_AppProc, ("call http_app_proc_server_data"));
		return http_app_proc_server_data(app_data, app_data_len, 
			user, conn, callback);
	}

	/* commented by liqi at 2005.12.14
	//lock the outstanding_msg link table
	if (!conn->aos_httpmsg_lock_init)
	{
		AOS_INIT_MUTEX(conn->aos_httpmsg_lock);
		conn->aos_httpmsg_lock_init = 1;
	}
	aos_lock(conn->aos_httpmsg_lock);

	if (conn->outstanding_msg == 0)// This is the first msg
	{
		msg = aos_httpmsg_constructor();//create a new msg struct
		msg->conn = conn;
		conn->outstanding_msg = msg;
	}
	else
	{
		//find the last node of outstanding_msg link table
		msg = (struct aos_httpmsg*)conn->outstanding_msg;	
		while (msg->next != NULL)
		{
			msg = msg->next;
		}
	}

	*/
	HttpAppDataLock(conn);
	
	msg = HttpAppGetLastMsg(conn);

	
	msg->callback = callback;
	msg->conn = conn;

	//aos_trace("msg:%x", msg);

	strncpy(msg->sysname, sysname, sizeof(msg->sysname));
	memcpy((char*)&(msg->user), (char*)user, sizeof(struct aos_user));

	aos_httpmsg_append_data(msg, app_data, app_data_len);	//append the data newly comes

	while (cursor < msg->buffer_size)  // continue to parse the data in the buffer until it encounters the end of the buffer
	{
		//parse the data
		//aos_debug_log(eAosMD_AppProc, "msg=%x", msg);

		ret = aos_httpmsg_parse(msg->buffer, msg->buffer_size, &cursor, &msg_complete, msg);
		if (ret == eAosHttpmsgFlag_InvalidMsg)
		{
 			//aos_unlock(conn->aos_httpmsg_lock);
 			HttpAppDataUnlock(conn);
			return eAosHttpmsgFlag_InvalidMsg;
		}
		
		//aos_debug_log(eAosMD_AppProc, "cursor=%d", cursor); 
		// check the return value:
		// 1. if the message head has been parsed, call aos_check_permit
		// 2. if the message has completed, check if there is any data else, if true , make a new msg. 
		if (ret == eAosHttpmsgFlag_MsgParsed)
		{
			//aos_trace("msg =%x, check_permit_result=%d", msg, msg->check_permit_result);
			if (msg->check_permit_result == eAOS_ACCESS_UNKNOWN)
			{
				//aos_debug_log(eAosMD_AppProc, "Set NewMsg flag, have not found the whole header yet");

				msg->checking_permit = eCHECKING_PERMIT_BUSY;
	 			url_decode(msg->resource);
				change_resource_format(msg->resource, sizeof(msg->resource));
				
//
// hardcore for pmi resource
// xiaoqing
//memcpy(msg->resource, gresource, strlen(gresource));
				aos_debug_log(eAosMD_AppProc, 
	  				"call aos_check_permit: msg is %x ,pmi_syname: %s, port is %d, resource is %s", 
					msg,
					msg->sysname,
					msg->port, 
					msg->resource);
			
				msg->check_permit_result = eAOS_ACCESS_WAITVERIFY;
				aos_httpmsg_hold(msg);
				AosAppProxy_connHold(msg->conn);

				if(msg->conn && msg->conn->isSecureUser)
					msg->operation = (msg->operation & 0xFF) | CLIENT_USERTYPE_518;
				else
					msg->operation = (msg->operation & 0xFF) | CLIENT_USERTYPE_USB;


				ret = call_aos_check_permit(msg,		//call zhang yun qiao's function
					msg->operation,
					&msg->user,
					msg->domain,
					msg->sysname,
					msg->resource,
					aos_httpmsg_check_permit_callback);

				if (ret != eAosRc_Success)
				{
//					aos_warn(eAosMD_AppProc, eAosAlarm_AppProcMsgError, ("request is not accepted by access control module"));
					aos_httpmsg_put(msg);
					AosAppProxy_connPut(msg->conn);
					//aos_unlock(conn->aos_httpmsg_lock);
					HttpAppDataUnlock(conn);
					
					return eAosRc_Drop; 
				}
				//aos_trace("msg =%x, check_permit_result=%d", msg, msg->check_permit_result);
				//aos_debug_log(eAosMD_AppProc, "aos_http_proc exit");
			}
		}

		if (msg_complete == 1)
		{
			
			// 1. here we have parsed the whole body of the message
			msg->complete = eAosHttpMsg_Complete;			//set the flag
			// 2. there is another a  new message
			if (cursor < msg->buffer_size)//there is another MSG 
			{
				
				msg_new = aos_httpmsg_constructor();
				msg_new->callback = callback;
				msg_new->conn = conn;
				msg->next = msg_new;

				msg_new->buffer = msg->buffer;
				msg_new->buffer_size = msg->buffer_size;

				msg->buffer = NULL;
				msg->buffer_size = 0;
				
				msg = msg->next;
				cursor = 0;
				msg_complete = 0;
				strncpy(msg->sysname, sysname, sizeof(msg->sysname));
				memcpy((char*)&(msg->user), (char*)user, sizeof(struct aos_user));

			}
			
			
			
		}
		
	}
	//aos_trace("aos_http_proc exit");
	if (aos_transfer_msg(conn) < 0)
	{
		aos_debug_log(eAosMD_AppProc, "Error in aos_transfer_msg() ");
	}
	//aos_unlock(conn->aos_httpmsg_lock);
	HttpAppDataUnlock(conn);
	return eAosRc_DoNothing;
}



