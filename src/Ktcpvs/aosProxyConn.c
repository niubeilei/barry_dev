////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: aosProxyConn.c
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////

#include <linux/net.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <net/sock.h>
#include <linux/module.h>

#include "aosUtil/Tracer.h"
#include "aosUtil/Memory.h"
#include "aosUtil/Alarm.h"

#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/tcp_vs_def.h"
#include "PKCS/x509.h"
#include "ssl/SslMisc.h"
#include "ssl/SslStatemachine.h"

// sock_create_lite(int family, int type, int protocol, struct socket **res);

struct tcp_vs_conn * AosAppProxy_connCreate(struct socket *sock)
{
	struct tcp_vs_conn *conn = NULL;

	conn = aos_zmalloc(sizeof(*conn));
	if (!conn) 
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_MemErr, "tcp vs conn no memory available");
		return NULL;
	}
	
	memset(conn, 0, sizeof(struct tcp_vs_conn));

	//aos_trace("AosAppProxy_connCreate %p", conn);
	// clone the socket
	
	//conn->csock = sock_alloc();
	sock_create_lite(PF_INET,SOCK_STREAM,IPPROTO_TCP,&conn->csock);
	if (!conn->csock) 
	{
		aos_free(conn);
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_MemErr, "tcp vs conn csock allocation failed");
		return NULL;
	}
	conn->csock->ops = sock->ops;
	conn->csock->type = sock->type;

    conn->client_buff = (char*)aos_malloc(eAosSSL_RecordMaxLen-500);
	if (!conn->client_buff) 
	{
		aos_free(conn);
		sock_release(conn->csock);
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_MemErr, "proxy client buffer allocation failed");
		return NULL;
	}
    conn->cbuff_len = eAosSSL_RecordMaxLen-500;

	conn->server_buff = (char*)aos_malloc(eAosSSL_RecordMaxLen-500);
    if (!conn->server_buff)
    {
        aos_free(conn);
        sock_release(conn->csock);
		aos_free(conn->client_buff);
        aos_alarm(eAosMD_TcpProxy, eAosAlarm_MemErr, "proxy server buffer allocation failed");
        return NULL;
    }
    conn->sbuff_len = eAosSSL_RecordMaxLen-500;

	conn->csock->type = sock->type;
	conn->csock->ops = sock->ops;
	conn->client_delay = 1;
	conn->server_delay = 1;
	atomic_set(&conn->refcnt, 1);

	return conn;
}

// 
// Chen Ding, 07/18/2005
// The buffers used to be released in tcp_vs_child(...). We moved it into this
// function. 
//
// Not sure whether we should call kfree(conn->csock) since conn->csock is allocated
// by us. 
//
int AosAppProxy_connRelease(struct tcp_vs_conn *conn)
{
	//aos_trace("AosAppProxy_connRelease %p", conn);
	// close the socket to the destination
    if (conn->dsock)
    {
        sock_release(conn->dsock);
        conn->dsock = NULL;
    }
	// release the cloned socket
	if (conn->csock)
	{
		sock_release(conn->csock);
		conn->csock = NULL;
	}

	if (conn->client_buff)
		aos_free(conn->client_buff);

	if (conn->server_buff)
        aos_free(conn->server_buff);
	
	if (conn->delay_to_server)
		aos_free(conn->delay_to_server);

	if (conn->dest)
	{
		atomic_dec(&conn->dest->conns);
		if (atomic_dec_and_test(&conn->dest->refcnt))
		{
			// this means the dest is dynamic created
			// so release here
			aos_free(conn->dest);
			conn->dest = NULL;
		}
	}

	if (conn->svc && conn->svc->scheduler && conn->svc->scheduler->app_data_release)
		conn->svc->scheduler->app_data_release(conn);

#ifdef CONFIG_AOS_SSL
	if (conn->front_context)
		AosSslContext_put(conn->front_context);
	if (conn->backend_context)
		AosSslContext_put(conn->backend_context);
	if (conn->front_cert)
		AosCert_put(conn->front_cert);
	if (conn->backend_cert)
		AosCert_put(conn->backend_cert);

#endif

	aos_free(conn);

	return 0;
}

EXPORT_SYMBOL( AosAppProxy_connRelease );
