////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_jns.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////


#include <linux/net.h>
#include <net/sock.h>

#include "Ktcpvs/tcp_vs.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"
#include "aos/aosReturnCode.h"

#define HTTP_MAX_HOSTNAME 256

static int tcp_vs_jns_init_svc(struct tcp_vs_service *svc)
{
	if( svc == NULL )
	/* add by ZQL 08/09/2006
	 * + Only you use this function when a new thread start ,
	 * + you must set 'svc' parameter equal to NULL .
	 * Note : This function used when jns server start !!!
	 */
	{
		return 0;
	}
	return 0;
}

static int tcp_vs_jns_done_svc(struct tcp_vs_service *svc)
{
	if( svc == NULL )
	/* add by ZQL 08/09/2006
	 * + Only you use this function when a new thread end ,
	 * + you must set 'svc' parameter equal to NULL .
	 * Note : This function used when jns server stop !!!
	 */
	{
		return 0;
	}
	return 0;
}

static int tcp_vs_jns_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

//
//      Parse JNS protocol packet
//
static int parse_jns_header(const char *buffer, size_t buflen, uint32_t *ipaddr, uint16_t *port)
{

	//aos_trace_hex("parsing JNS proto first packet", buffer, buflen);
	memcpy((uint8*)ipaddr, buffer, 4);
	memcpy((uint8*)port, buffer+4, 2); 

	return eAosRc_Success;
}

//
//    JNS protocol content-based scheduling
//    Parse the jns request, select a server according to the
//    request, and create a socket the server finally.
 
static int tcp_vs_jns_schedule(
	struct tcp_vs_conn *conn, 
	struct tcp_vs_service *svc, 
	char*  req_client, 
	int    req_len)
{
	tcp_vs_dest_t *dest;
	struct socket *dsock = NULL;
	uint32_t addr_dst;
	uint16_t port_dst;

	aos_min_log(eAosMD_TcpProxy, "Handle jns request");
	
	// 
	// Have accepted the connection from a client
	// if set static dest,  it will select dest from static dest list
	// if set dynamic dest, it will select dest according to the first
	// jns request received from client.
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
	if( eAosRc_Success != parse_jns_header(req_client, req_len, &addr_dst, &port_dst))
	{
	 	aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "cannot parse the first request of jns proto");
		return -1;
	}
	
	conn->backend_ssl_flags = eAosSSLFlag_Client|eAosSSLFlag_Backend;
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
	dest = aos_malloc(sizeof(tcp_vs_dest_t));
	if (dest == NULL) 
	{
		aos_warn(eAosMD_TcpProxy, eAosAlarm_MemErr, ("jns dest allocation failed"));
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
	if(ntohs(5604)==dest->port)
		dest->port=htons(5605);
	atomic_set(&dest->conns, 0);
	atomic_set(&dest->refcnt, 1); // dynamic create dest, should be released when the conn is free
	
FindDest:
	aos_min_log(eAosMD_TcpProxy, "HTTP: %s backend server %d.%d.%d.%d:%d "
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

static struct tcp_vs_scheduler tcp_vs_jns_scheduler = {
	{0},							// n_list 
	"jns",							// name 
	THIS_MODULE,					// this module
	tcp_vs_jns_init_svc,			// initializer 
	tcp_vs_jns_done_svc,			// done
	tcp_vs_jns_update_svc,			// update
	tcp_vs_jns_schedule,			// select a server by jns request
	0,								// server to client
	NULL,							// client to server
	NULL,							// checking permit
	NULL							// release app processing request
};

int tcp_vs_jns_init(void)
{
	INIT_LIST_HEAD(&tcp_vs_jns_scheduler.n_list);
	return register_tcp_vs_scheduler(&tcp_vs_jns_scheduler);
}

void tcp_vs_jns_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_jns_scheduler);
}
