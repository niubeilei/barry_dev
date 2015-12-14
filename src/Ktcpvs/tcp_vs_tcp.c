////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2006
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_tcp.c
// Description:
//   
//
// Modification History:
// Created by Pownall.Zhang : 12/08/2006
////////////////////////////////////////////////////////////////////////////

#include <linux/smp_lock.h>
#include <linux/net.h>
#include <linux/in.h>
#include <linux/tcp.h>
#include <net/sock.h>
#include <linux/random.h> // for get_random_bytes();

#include "Ktcpvs/aosTcpVsCntl.h"
#include "Ktcpvs/tcp_vs.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
//#include "ssl/aosSslProc.h"
#include "ssl/SslCommon.h"
#include "aos/aosReturnCode.h"
#include "KernelPorting/Sock.h"
#include "KernelPorting/Daemonize.h"
#include "KernelPorting/Current.h"
#include "Ktcpvs/aosProxyConn.h"
#include "ssl/Ssl.h"

#ifdef CONFIG_AOS_TCPVS_TCP_LOG

#include "Ktcpvs/aosKernelFile.h"
#include "Ktcpvs/aosLogFunc.h"
//extern int aos_ssl_hardware_flag;


#define TCP_BIN_LOGNAME		"/usr/local/keyou/Log/%04d/%02d%02d/tcp.%s.log"

#define write_tcp_bin_data		aos_process_log_data

/*
 * end 2006.08.06
 * add up codes over by ZQL
 */
 
#endif

static int tcp_vs_tcp_init_svc(struct tcp_vs_service *svc)
{	
	return 0;
}


static int tcp_vs_tcp_done_svc(struct tcp_vs_service *svc)
{
	return 0;
}


static int tcp_vs_tcp_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

//
//    general TCP protocol content-based scheduling
//    Parse the ssh request, select a server according to the
//    request, and create a socket the server finally.
 
static int tcp_vs_tcp_schedule(
	struct tcp_vs_conn *conn, 
	struct tcp_vs_service *svc, 
	char*  req_client, 
	int    req_len)
{
	//
	// Have accepted the connection from a client
	// if set static dest,  it will select dest from static dest list
	// if set dynamic dest, it will select dest according to the first
	// telnet request received from client.
	// Then it will connect to the selected backend server.
	//

	// static processing
	conn->dest = tcp_vs_finddst(svc);
	if (!conn->dest)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "No destination found at vs %s", svc->ident.name);
		return -1;
	}
	atomic_inc(&conn->dest->refcnt);
	conn->dsock = tcp_vs_connect2dest(conn->dest);
	if (!conn->dsock)
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
	atomic_inc(&conn->dest->conns);
	
	return 0;
}

static int  tcp_vs_tcp_client_to_server(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;

#ifdef CONFIG_AOS_TCPVS_TCP_LOG
	if( conn->bin_file_ptr == NULL )
		aos_write_log_file_header( conn, TCP_BIN_LOGNAME );
	return write_tcp_bin_data( app_data, *app_data_len, conn, CLIENTTOSERVER );
#else
	return 0;
#endif
}

static int  tcp_vs_tcp_server_to_client(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;
	
#ifdef CONFIG_AOS_TCPVS_TCP_LOG
	if( conn->bin_file_ptr == NULL )
		aos_write_log_file_header( conn, TCP_BIN_LOGNAME );
	return write_tcp_bin_data( app_data, *app_data_len, conn, SERVERTOCLIENT );
#else
	return 0;
#endif
}

//add by Zhang Quanlin, 2006/12/13
static int tcp_vs_tcp_app_data_release(struct tcp_vs_conn *conn)
{
#ifdef CONFIG_AOS_TCPVS_TCP_LOG
	aos_fclose( conn->bin_file_ptr );
	aos_close_daemon_sock( conn );
#endif
	conn->tcp_vs_status = TCP_VS_END;
	return 0;
}

static struct tcp_vs_scheduler tcp_vs_tcp_scheduler = {
	{0},							// n_list 
	"tcp",						// name 
	0,								// this module
	tcp_vs_tcp_init_svc,			// initializer 
	tcp_vs_tcp_done_svc,			// done
	tcp_vs_tcp_update_svc,		// update
	tcp_vs_tcp_schedule,			// select a server by ssh request
	tcp_vs_tcp_server_to_client,				// server to client
	tcp_vs_tcp_client_to_server,				// client to server
	NULL,							// checking permit
	tcp_vs_tcp_app_data_release	// release app processing request
};

static __exit void tcp_vs_tcp_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_tcp_scheduler);
}

static __init int tcp_vs_tcp_init(void)
{
	INIT_LIST_HEAD(&tcp_vs_tcp_scheduler.n_list);
	return register_tcp_vs_scheduler(&tcp_vs_tcp_scheduler);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pownall");
module_init( tcp_vs_tcp_init );
module_exit( tcp_vs_tcp_cleanup );

