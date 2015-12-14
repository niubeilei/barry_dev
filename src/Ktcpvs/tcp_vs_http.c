////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_http.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/ctype.h>

#include <linux/net.h>
#include <linux/sched.h>
#include <linux/skbuff.h>
#include <linux/inet.h>
#include <net/sock.h>
#include <net/ip.h>
#include <asm/uaccess.h>

#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/tcp_vs_http_parser.h"
#include "KernelPorting/Sock.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"
#include "aos/aosReturnCode.h"
#include "AppProc/http_proc/ForwardTable.h"
#include "AppProc/http_proc/HttpParser.h"
#include "CertVerify/aosAccessManage.h"
#include "PKCS/CertMgr.h"
#include "WebWall/WebWall.h"

#define HTTP_MAX_HOSTNAME 256
#define DNS_SERVER_PORT 12005
#define UTF_SERVER_PORT 12004

MODULE_LICENSE("GPL");
MODULE_AUTHOR("liqin");

enum{
	HTTP_STATE_NEW_REQUEST = 0,
	HTTP_STATE_HEADER_RECEIVE,
	HTTP_STATE_MSGBODY_RECEIVE,
};

static int AosHttp_webwallIsEnable(void);
static int AosHttp_webwallUrlFilter(uint8* uri, int uri_len, uint8* host, int host_len);
static int AosHttp_webwallTimeControl(void);
static char* AosHttp_webwallGetDenyPage(void);
	
struct webwall_api_s gwebwall_api = {
	AosHttp_webwallIsEnable,
	AosHttp_webwallUrlFilter,
	AosHttp_webwallTimeControl,
	AosHttp_webwallGetDenyPage,
};

static int
tcp_vs_http_init_svc(struct tcp_vs_service *svc)
{
	if( svc == NULL )
	/* add by ZQL 08/09/2006
	 * + Only you use this function when a new thread start ,
	 * + you must set 'svc' parameter equal to NULL .
	 * Note : This function used when http server start !!!
	 */
	{
		return 0;
	}
	return 0;
}

static int
tcp_vs_http_done_svc(struct tcp_vs_service *svc)
{
	if( svc == NULL )
	/* add by ZQL 08/09/2006
	 * + Only you use this function when a new thread end ,
	 * + you must set 'svc' parameter equal to NULL .
	 * Note : This function used when http server stop !!!
	 */
	{
		return 0;
	}
	return 0;
}

static int
tcp_vs_http_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

static int AosHttp_webwallIsEnable(void)
{
	return 0;
}

static int AosHttp_webwallUrlFilter(uint8* uri, int uri_len, uint8 *host, int host_len)
{
	return 0;
}

static int AosHttp_webwallTimeControl(void)
{
	return 0;
}

static char* AosHttp_webwallGetDenyPage(void)
{
	return 0;
}

void AosHttp_registerWebWallApi( struct webwall_api_s *webwall_api )
{
	if ( !webwall_api ) return;
	gwebwall_api.is_enable = webwall_api->is_enable;
	gwebwall_api.url_filter = webwall_api->url_filter;
	gwebwall_api.time_control = webwall_api->time_control;
	gwebwall_api.get_deny_page = webwall_api->get_deny_page;
}

void AosHttp_unregisterWebWallApi( void )
{
	gwebwall_api.is_enable = AosHttp_webwallIsEnable;
	gwebwall_api.url_filter = AosHttp_webwallUrlFilter;
	gwebwall_api.time_control = AosHttp_webwallTimeControl;
	gwebwall_api.get_deny_page = AosHttp_webwallGetDenyPage;
}	

static inline tcp_vs_dest_t *
__tcp_vs_http_wlc_schedule(struct list_head *destinations)
{
	register struct list_head *e;
	tcp_vs_dest_t *dest, *least;

	list_for_each(e, destinations) {
		least = list_entry(e, tcp_vs_dest_t, r_list);
		if (least->weight > 0) {
			goto nextstage;
		}
	}
	return NULL;

	/*
	 *    Find the destination with the least load.
	 */
      nextstage:
	for (e = e->next; e != destinations; e = e->next) {
		dest = list_entry(e, tcp_vs_dest_t, r_list);
		if (atomic_read(&least->conns) * dest->weight >
		    atomic_read(&dest->conns) * least->weight) {
			least = dest;
		}
	}

	return least;
}


typedef enum http_methods {
	METHOD_NONE,
	METHOD_GET,
	METHOD_HEAD,
	METHOD_POST,
	METHOD_PUT,
	NR_METHODS
} http_method_t;

typedef enum http_versions {
	NOTKNOWN,
	HTTP_1_0,
	HTTP_1_1
} http_version_t;

#define GOTO_INCOMPLETE							\
do {									\
	TCP_VS_DBG(5, "incomplete at %s:%d.\n", __FILE__, __LINE__);	\
	goto incomplete_message;					\
} while (0)

#define GOTO_ERROR							\
do {									\
	TCP_VS_DBG(5, "parse error at %s:%d.\n", __FILE__, __LINE__);	\
	goto error;							\
} while (0)

#define get_c(ptr,eob)				\
({						\
	if (ptr >= eob)				\
		GOTO_INCOMPLETE;		\
	*((ptr)++);				\
})

#define PARSE_TOKEN(ptr,str,eob)		\
({						\
	int __ret;				\
						\
	if (ptr+sizeof(str)-1 > eob) {		\
		GOTO_INCOMPLETE;		\
	}					\
						\
	if (memcmp(ptr, str, sizeof(str)-1))	\
		__ret = 0;			\
	else {					\
		ptr += sizeof(str)-1;		\
		__ret = 1;			\
	}					\
	__ret;					\
})


/*
 *      Parse HTTP header
 */
int
parse_http_header(const char *buffer, size_t buflen, http_request_t * req)
{
	const char *curr, *eob;
	char c;

	aos_debug_log(eAosMD_TcpProxy, "parsing request");
	aos_debug_log(eAosMD_TcpProxy, "--------------------");
	aos_debug_log(eAosMD_TcpProxy, "%s", buffer);
	aos_debug_log(eAosMD_TcpProxy, "--------------------");

	// parse only the first header if multiple headers are present 
	curr = buffer;
	eob = buffer + buflen;

	req->message = buffer;
	req->message_len = buflen;

	//
	//  RFC 2616, 5.1:
	//       Request-Line = Method SP Request-URI SP HTTP-Version CRLF
	// 
	switch (get_c(curr, eob)) {
	case 'G':
		if (PARSE_TOKEN(curr, "ET ", eob)) {
			req->method = METHOD_GET;
			break;
		}
		GOTO_ERROR;

	case 'H':
		if (PARSE_TOKEN(curr, "EAD ", eob)) {
			req->method = METHOD_HEAD;
			break;
		}
		GOTO_ERROR;

	case 'P':
		if (PARSE_TOKEN(curr, "OST ", eob)) {
			req->method = METHOD_POST;
			break;
		}
		if (PARSE_TOKEN(curr, "UT ", eob)) {
			req->method = METHOD_PUT;
			break;
		}
		GOTO_ERROR;

	default:
		GOTO_ERROR;
	}
	req->method_str = buffer;
	req->method_len = curr - buffer - 1;

	req->uri_str = curr;
	while (1) {
		c = get_c(curr, eob);
		if (isspace(c))
			break;
	}
	req->uri_len = curr - req->uri_str - 1;

	req->version_str = curr;
	if (PARSE_TOKEN(curr, "HTTP/1.", eob)) {
		switch (get_c(curr, eob)) {
		case '0':
			req->version = HTTP_1_0;
			break;
		case '1':
			req->version = HTTP_1_1;
			break;
		default:
			GOTO_ERROR;
		}
	} else
		GOTO_ERROR;

	return PARSE_OK;

      incomplete_message:
	return PARSE_INCOMPLETE;

      error:
	return PARSE_ERROR;
}

static int aos_dns_lookup(uint8_t *hostname, uint32_t *addr)
{
	struct socket *sock;
	struct sockaddr_in sin;
	uint8_t buffer[32];
	int error;
	int len;

	aos_assert1(hostname);

	memset(buffer, 0, 32);
	error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (error < 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error during creation of socket; terminating");
		return -1;
	}

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(DNS_SERVER_PORT);

	error = sock->ops->connect(sock, (struct sockaddr*)&sin,sizeof(sin), 0);
	if (error < 0) 
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error connecting to the dns host");
		sock_release(sock);
		return -1;
	}
	if (tcp_vs_sendbuffer(sock, hostname, strlen(hostname), 0) <= 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error sending request to the dns host");
		sock_release(sock);
		return -1;
	}
	if ((len = tcp_vs_recvbuffer(sock, buffer, 32, 0)) <= 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error receive response from the dns host");
		sock_release(sock);
		return -1;
	}

	*addr = in_aton(buffer);
	sock_release(sock);
	return eAosRc_Success;
}



/*
 *    HTTP content-based scheduling
 *    Parse the http request, select a server according to the
 *    request, and create a socket the server finally.
 */
static int
tcp_vs_http_schedule(struct tcp_vs_conn *conn, struct tcp_vs_service *svc, char* req_client, int req_len)
{
	tcp_vs_dest_t *dest;
	struct socket *dsock = NULL;
	uint8_t hostname[HTTP_MAX_HOSTNAME];
	uint32_t addr_dst;
	uint16_t port_dst = htons(80);
	//int connType;
	int len = 0;

	//aos_min_log(eAosMD_TcpProxy, "Handle http request");
	
	// 
	// Have accepted the connection from a client
	// if set static dest,  it will select dest from static dest list
	// if set dynamic dest, it will select dest according to the first
	// http request received from client.
	// Then it will connect to the selected backend server.
	//

	// static processing
	if (!svc->conf.dynamic_dst)
	{
		dest = tcp_vs_finddst(svc);
		if (!dest)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "No destination found at vs %s", svc->ident.name);
			return -1;
		}
		atomic_inc(&dest->refcnt);
		goto FindDest;
	}
	
	// dynamic processing
	aos_min_log(eAosMD_TcpProxy, "http request len from client is %d", req_len);
	// remove me, it is only for test
	req_client[req_len] = 0;
	//
	//HttpRequestParse2HostName(req_client, req_len, hostname, sizeof(hostname)-1, &len);
	http_get_host( req_client, req_len, hostname, sizeof(hostname)-1, &len );

	if (len <= 0) 
	{
	 	aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "cannot parse http request %d", len);
		return -1;
	}
	aos_assert1(len <= (sizeof(hostname)-1));
	hostname[len] = 0;
	
	aos_trace("hostname %s, svc %s", hostname, svc->ident.name);
	
	/*ret = aos_http_fwdtbl_lookup(svc->ident.name, (char*)hostname, &addr_dst, &port_dst, &connType);
	if (ret != eAosRc_Success)
	{
	 	aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, 
	 		("cannot find backend server in forward table according to http request"));
		return -1;
	}*/

	if (aos_dns_lookup(hostname, &addr_dst) != eAosRc_Success)
	{
	 	aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "cannot resolve hostname %s", hostname);
		return -1;
	}

	//if (!HttpRequestParse2Port(req_client, req_len, &port_dst))
	if ( !http_get_port(req_client, req_len, &port_dst) )
	{
	 	aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "cannot parse port from request %s", req_client);
		return -1;
	}

	//conn->backend_ssl_flags = (connType == eAosConnType_SSL)? 
	//		(eAosSSLFlag_Client|eAosSSLFlag_Backend): 0;
	/*
	if (conn->backend_ssl_flags)
	{
		if ( conn->delay_to_server)
		{
			aos_free(conn->delay_to_server);
			conn->delay_to_server = NULL;
		}
		conn->delay_to_server = aos_malloc(req_len);
		aos_assert1(conn->delay_to_server);
		
		memcpy(conn->delay_to_server, req_client, req_len);
		conn->delay_to_server_len = req_len;
		//aos_trace_hex("delay_to_server", conn->delay_to_server, req_len);
	}
	*/

	dest = aos_malloc(sizeof(tcp_vs_dest_t));
	if (dest == NULL) 
	{
		aos_warn(eAosMD_TcpProxy, eAosAlarm_MemErr, ("dest allocation failed"));
		if ( conn->delay_to_server)
        {
            aos_free(conn->delay_to_server);
            conn->delay_to_server = NULL;
        }
		return -eAosRc_ProgErr;
	}
	memset(dest, 0, sizeof(tcp_vs_dest_t));
	dest->addr = addr_dst;
	dest->port = ntohs(port_dst);
	atomic_set(&dest->conns, 0);
	atomic_set(&dest->refcnt, 1); // dynamic create dest, should be released when the conn is free
	
FindDest:
	aos_eng_log(eAosMD_TcpProxy, "HTTP: %s backend server %d.%d.%d.%d:%d "
	   "conns %d refcnt %d weight %d\n",
	   svc->conf.dynamic_dst?"dynamic":"static",
	   NIPQUAD(dest->addr), ntohs(dest->port),
	   atomic_read(&dest->conns),
	   atomic_read(&dest->refcnt), dest->weight);
	
	dsock = tcp_vs_connect2dest(dest);
	if (!dsock) 
	{
		aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, 
			"The destination for vs %s is not available", svc->ident.name);
		if ( conn->delay_to_server)
        {
            aos_free(conn->delay_to_server);
            conn->delay_to_server = NULL;
        }
		return -1;
	}
	atomic_inc(&dest->conns);
	conn->dest = dest;
	conn->dsock = dsock;

	return 0;
}
static int aos_http_client_to_server(
	char *app_data, 
	unsigned int * app_data_len, 
	struct tcp_vs_conn *conn,
	int (*callback)(struct aos_app_proc_req *req))
{

	struct tcp_vs_service 	*svc = conn->svc;
	struct http_request_s *req = NULL;
	char *deny_page, *uri_str;
	struct sockaddr_in 	src_addr;
	//struct aos_user pmi_user;
	int len, ret;

	aos_assert1(svc);

	/*if (svc->conf.pmi)
    {
        memset(&pmi_user, 0, sizeof(struct aos_user));
		if(conn->front_context && conn->front_context->peer_cert)
        {   
            pmi_user.len = conn->front_context->peer_cert->serial_number.len;
            memcpy(pmi_user.data, conn->front_context->peer_cert->serial_number.data, pmi_user.len);
            aos_trace_hex("pmiuser",pmi_user.data, pmi_user.len); 
        }
        else if(conn->front_context && (conn->front_context->peer_cert_sn.len > 0) )
        {   
            pmi_user.len = conn->front_context->peer_cert_sn.len;
            memcpy(pmi_user.data, conn->front_context->peer_cert_sn.data, pmi_user.len);
            aos_trace_hex("pmiuser",pmi_user.data, pmi_user.len); 
        }
        ret = aos_http_proc(app_data, *app_data_len, &pmi_user, svc->conf.pmi_sysname, conn, efrom_client, callback);
		*app_data_len = 0;
        return (ret>=0)?eAosRc_Success:ret;
    }*/

	if ( !gwebwall_api.is_enable() ) 
	{
		aos_trace("Webwall is disabled");
		return eAosRc_Success;
	}
	// Web Wall time control
	ret = gwebwall_api.time_control();
	if( eWebWall_Ctl_Block ==  ret )
	{
		*app_data_len = 0;
		deny_page = gwebwall_api.get_deny_page();
		if ( deny_page )
		{
			tcp_vs_sendbuffer( conn->csock, deny_page, strlen(deny_page), 0 );
			return 0 ;
		}
		else
		{
			ret = -1;
			goto HTTPCTSEXIT;
		}
	}
	else if ( ret < 0 )
	{
		*app_data_len = 0;
		goto HTTPCTSEXIT;
	}


	// Call http parser
	req = (struct http_request_s*) aos_malloc( sizeof(struct http_request_s) );
	aos_assert1( req );
	
	aos_trace("data size=%d, data: %sEND",*app_data_len,app_data);
	if ( aos_http_header_parser( (uint8*)app_data, *app_data_len, req ) )
	{
		*app_data_len = 0;
		ret = -1;
		goto HTTPCTSEXIT;
	}


	// call log
	conn->csock->ops->getname( conn->csock,(struct sockaddr*)&src_addr,&len,0);
	uri_str = (char*)aos_malloc( req->uri_len + 1 );
	aos_assert1(uri_str);
	memcpy( uri_str, req->uri_str, req->uri_len );
	uri_str[req->uri_len] = 0;
	printk("WebWall Access: %d.%d.%d.%d %s\n", NIPQUAD(src_addr.sin_addr.s_addr), uri_str); 
	aos_free( uri_str );
	
	// Web Wall control
	printk("URI LEN=%d, HOST_LEN=%d",req->uri_len,req->host_len);
	ret = gwebwall_api.url_filter( (uint8*) req->uri_str, req->uri_len, (uint8*)req->host_str, req->host_len );
	if( eWebWall_Ctl_Block ==  ret )
	{
		aos_trace("url filter returns BLOCK");
		*app_data_len = 0;
		deny_page = gwebwall_api.get_deny_page();
		if ( deny_page )
		{
			tcp_vs_sendbuffer( conn->csock, deny_page, strlen(deny_page), 0 );
			return 0 ;
		}
		else
		{
			ret = -1;
			goto HTTPCTSEXIT;
		}
	}
	else if ( ret < 0 )
	{
		aos_trace("url filter returns < 0");
		*app_data_len = 0;
		goto HTTPCTSEXIT;
	}
	aos_trace("url filter ok");

HTTPCTSEXIT:
	if( req ) aos_free(req);
	return ret;
	
}

static struct tcp_vs_scheduler tcp_vs_http_scheduler = {
	{0},							// n_list
	"http",							// name
	THIS_MODULE,					// this module
	tcp_vs_http_init_svc,			// initializer
	tcp_vs_http_done_svc,			// done
	tcp_vs_http_update_svc,			// update
	tcp_vs_http_schedule,			// select a server by http request
	0,								// server to client
	aos_http_client_to_server,		// client to server
	0,								// aos_httpmsg_checking_permit,	// checking permit
	0,								// http_app_proc_req_destructor,	// release app processing request
	0,								// aos_httpmsg_appdata_destructor  // release app private data
};


static __init int tcp_vs_http_init(void)
{
	int ret;
	INIT_LIST_HEAD(&tcp_vs_http_scheduler.n_list);
	ret = register_tcp_vs_scheduler(&tcp_vs_http_scheduler);
	if( ret != 0 )
	{
		aos_min_log( eAosMD_TcpProxy, "tcp_vs_http module load failed" );
	}
	else
	{
		aos_min_log( eAosMD_TcpProxy, "tcp_vs_http module load" );
	}

	return ret;
}

static __exit void tcp_vs_http_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_http_scheduler);
	aos_min_log( eAosMD_TcpProxy, "tcp_vs_http module exit" );
}
module_init( tcp_vs_http_init );
module_exit( tcp_vs_http_cleanup );

EXPORT_SYMBOL( AosHttp_registerWebWallApi );
EXPORT_SYMBOL( AosHttp_unregisterWebWallApi );

	
