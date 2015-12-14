///////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: tcp_vs_ftp.c
// Description:
//
//
// Modification History:
//
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
#include "PKCS/CertMgr.h"
#include "PKCS/CertChain.h"
#include "ssl/Ssl.h"

#ifdef CONFIG_AOS_TCPVS_FTP_LOG

#include "Ktcpvs/aosKernelFile.h"
#include "Ktcpvs/aosLogFunc.h"
//extern int aos_ssl_hardware_flag;

/*
 * start 2006.12.12
 * add under some codes by ZQL
 */

#define FTP_BIN_LOGNAME		"/usr/local/keyou/Log/%04d/%02d%02d/ftp.%s.log"
#define write_ftp_bin_data		aos_process_log_data

static char ftp_prefix[]="500 ";
/*
 * end 2006.12.12
 * add up codes over by ZQL
 */

#endif


static long
strtol(char *string, char **endptr, int radix)
{
	char *s;
	long value;
	long new_value;
	int sign;
	int increment;

	value = 0;
	sign = 1;
	s = string;

	if ((radix == 1) || (radix > 36) || (radix < 0)) {
		goto done;
	}

	/* skip whitespace */
	while ((*s == ' ') || (*s == '\t') || (*s == '\n') || (*s == '\r')) {
		s++;
	}

	if (*s == '-') {
		sign = -1;
		s++;
	} else if (*s == '+') {
		s++;
	}

	if (radix == 0) {
		if (*s == '0') {
			s++;
			if ((*s == 'x') || (*s == 'X')) {
				s++;
				radix = 16;
			} else
				radix = 8;
		} else
			radix = 10;
	}

	/* read number */
	while (1) {
		if ((*s >= '0') && (*s <= '9'))
			increment = *s - '0';
		else if ((*s >= 'a') && (*s <= 'z'))
			increment = *s - 'a' + 10;
		else if ((*s >= 'A') && (*s <= 'Z'))
			increment = *s - 'A' + 10;
		else
			break;

		if (increment >= radix)
			break;

		new_value = value * radix + increment;
		/* detect overflow */
		if ((new_value - increment) / radix != value) {
			s = string;
			value = -1 >> 1;
			if (sign < 0)
				value += 1;

			goto done;
		}

		value = new_value;
		s++;
	}

      done:
	if (endptr)
		*endptr = s;

	return value * sign;
}


static int tcp_vs_ftp_init_svc(struct tcp_vs_service *svc)
{
	return 0;
}


static int tcp_vs_ftp_done_svc(struct tcp_vs_service *svc)
{
	return 0;
}


static int tcp_vs_ftp_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

//
//    JNS protocol content-based scheduling
//    Parse the ftp request, select a server according to the
//    request, and create a socket the server finally.


static int tcp_vs_ftp_schedule(
	struct tcp_vs_conn *conn,
	struct tcp_vs_service *svc,
	char*  req_client,
	int    req_len)
{
	tcp_vs_dest_t *dest = NULL;
	struct socket *dsock = NULL;

	//aos_min_log(eAosMD_TcpProxy, "Handle ftp request ...");

	dest = tcp_vs_finddst(svc);
	if (!dest)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "No destination found at vs %s", svc->ident.name);
		return -1;
	}
	atomic_inc(&dest->refcnt);
/*
	aos_min_log(eAosMD_TcpProxy, "FTP: %s backend server %d.%d.%d.%d:%d "
	   "conns %d refcnt %d weight %d\n",
	   svc->conf.dynamic_dst?"dynamic":"static",
	   NIPQUAD(dest->addr), ntohs(dest->port),
	   atomic_read(&dest->conns),
	   atomic_read(&dest->refcnt), dest->weight);
*/
	dsock = tcp_vs_connect2dest(dest);
	if (!dsock)
	{
		aos_warn(eAosMD_TcpProxy, eAosAlarm_ProgErr, "The destination for vs %s is not available", svc->ident.name);
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


static unsigned short find_unused_port(struct sockaddr_in * addr)
{
	//this way is not efficient, we need to change it later. use tcp_v4_get_port()
	struct socket	*sock;
	unsigned short	port;
	int	rand;
	int	i;
	int	error;

	error=sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&sock);
	if(error<0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error during creation of socket");
		return -1;
	}

	for(i=0;i<50000;i++)
	{
		get_random_bytes(&rand,sizeof(int));
		port=10000+(i+rand)%50000;
		addr->sin_port=htons(port);
		error = sock->ops->bind(sock,(struct sockaddr*)addr,sizeof(*addr));
		if(!error)
			break;
		if(error != -EADDRINUSE)
		{
			port=0;
			break;
		}

	}
	sock_release(sock);
	return port;
}
static void com2n(struct sockaddr_in *addr, unsigned char * a)
{
	addr->sin_addr.s_addr = htonl(a[3] + (a[2] << 8) + (a[1] << 16) + (a[0] << 24));
	addr->sin_port = htons((a[4] << 8) + a[5]);
	addr->sin_family = AF_INET;
}
static void extract_address(struct sockaddr_in * addr, char * arg)
{
	char * tmp;
	unsigned char n[6];
	int	i;

	for(i=0;i<6;i++)
	{
		n[i]=strtol(arg,&tmp,0);
		arg=tmp+1;
	}

	com2n(addr,n);
}

static void n2com(struct sockaddr_in *addr, unsigned char * a)
{
	struct sockaddr_in	addr1;

	addr1.sin_addr.s_addr = ntohl(addr->sin_addr.s_addr);
	addr1.sin_port = ntohs(addr->sin_port);
	a[0] = (addr1.sin_addr.s_addr & 0xFF000000) >> 24;
	a[1] = (addr1.sin_addr.s_addr & 0x00FF0000) >> 16;
	a[2] = (addr1.sin_addr.s_addr & 0x0000FF00) >> 8;
	a[3] = (addr1.sin_addr.s_addr & 0x000000FF);
	a[4] = (addr1.sin_port & 0xFF00) >> 8;
	a[5] = (addr1.sin_port & 0x00FF);
}

static struct socket *
start_listening(struct sockaddr_in * addr)
{
	struct socket *sock;
	int error;

	EnterFunction(3);

	/* First create a socket */
	error = sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock);
	if (error < 0) {
		TCP_VS_ERR
		    ("Error during creation of socket; terminating\n");
		return 0;
	}

	/* set the option to reuse the address. */
	aosSock_Reuse(sock->sk) = 1;

	/* Now bind the socket */
	error = sock->ops->bind(sock, (struct sockaddr *) addr, sizeof(*addr));
	if (error < 0) {
		TCP_VS_ERR("Error binding socket. This means that some "
			   "other daemon is (or was a short time ago) "
			   "using %u.%u.%u.%u:%d.\n",
			   NIPQUAD(addr->sin_addr.s_addr), ntohs(addr->sin_port));
		return 0;
	}

	/* Now, start listening on the socket */
	error = sock->ops->listen(sock, sysctl_ktcpvs_max_backlog);
	if (error != 0) {
		TCP_VS_ERR("ktcpvs: Error listening on socket \n");
		return 0;
	}

	LeaveFunction(3);
	return sock;
}


struct ftp_data
{
	struct sockaddr_in		listen_addr;
	struct sockaddr_in		rs_addr;
	__u32	front_ssl_flags;
	__u32	back_ssl_flags;
	struct tcp_vs_conn	*main_conn;
};

/*
static struct socket * connect_to_address(struct sockaddr_in * addr)
{
	struct socket * socket=0;
	int	error;


	error=sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&socket);
	if(error<0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error during creation of socket; terminating");
		return 0;
	}
	error=socket->ops->connect(socket,(struct sockaddr*)addr,sizeof(*addr),0);
	if (error < 0)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "Error connecting to the remote host");
		sock_release(socket);
		return 0;
	}

	return socket;
}
*/






/*
static int tcp_vs_ftp_data_transfer_data(struct socket*client_socket, struct socket* server_socket)
{
	unsigned long lastupdated;
	size_t	data_len;
	char	* data=0;

	DECLARE_WAITQUEUE(wait1, current);
	DECLARE_WAITQUEUE(wait2, current);

	data = aos_malloc(eAosSSL_RecordMaxLen);
    	aos_assert1(data);

	lastupdated=jiffies;

	aos_min_log(eAosMD_TcpProxy,  "tcp_vs_ftp_data_transfer_data():begin transfer");
	while ((jiffies - lastupdated) < sysctl_ktcpvs_read_timeout * HZ)
	{
		if  (!skb_queue_empty(&(aosSock_ReceiveQueue(client_socket->sk))))
		{
			data_len = tcp_vs_recvbuffer(client_socket, data, eAosSSL_RecordMaxLen, 0);
			aos_min_log(eAosMD_TcpProxy,  "tcp_vs_ftp_data_transfer_data():data received , data_len %d", data_len);
			if(data_len<=0)
			{
				aos_min_log(eAosMD_TcpProxy,  "break while");
				break;
			}
			tcp_vs_sendbuffer(server_socket,data,data_len,0);

			lastupdated = jiffies;
		}
		if (server_socket && !skb_queue_empty(&(aosSock_ReceiveQueue(server_socket->sk))) )
		{
			data_len = eAosSSL_RecordMaxLen;
    			data_len = tcp_vs_recvbuffer(server_socket, data, data_len, 0);
			aos_min_log(eAosMD_TcpProxy,  "tcp_vs_ftp_data_transfer_data():data received , data_len %d", data_len);
			if(data_len<=0)
			{
				aos_min_log(eAosMD_TcpProxy,  "break while");
				break;
			}
			tcp_vs_sendbuffer(client_socket,data,data_len,0);

			lastupdated = jiffies;
		}
		if (skb_queue_empty(&(aosSock_ReceiveQueue(client_socket->sk))) &&
		    	skb_queue_empty(&(aosSock_ReceiveQueue(server_socket->sk))))
		{
			if (aosSock_State(client_socket->sk) == TCP_CLOSE_WAIT ||
			    aosSock_State(server_socket->sk) == TCP_CLOSE_WAIT)
			{
				aos_min_log(eAosMD_TcpProxy,  "server or client connection closed,break while");
				break;
			}
			add_wait_queue(aosSock_Sleep(client_socket->sk), &wait1);
			add_wait_queue(aosSock_Sleep(server_socket->sk), &wait2);
			__set_current_state(TASK_INTERRUPTIBLE);
			schedule_timeout(HZ);
			__set_current_state(TASK_RUNNING);
			remove_wait_queue(aosSock_Sleep(client_socket->sk), &wait1);
			remove_wait_queue(aosSock_Sleep(server_socket->sk), &wait2);

		}
	}//while ((jiffies - lastupdated) < sysctl_ktcpvs_read_timeout * HZ)

	aos_free(data);
	return 0;
}
*/

/*

static int tcp_vs_ftp_data_thread(void * __ftp_data)
{

	struct socket * listen_socket;
	struct socket * client_socket;
	struct socket * server_socket;
	struct ftp_data	* info=(struct ftp_data*)__ftp_data;
	int	ret;
	DECLARE_WAITQUEUE(wait, current);

	lock_kernel();
	aosDaemonize("k-ftp-d");

	spin_lock_irq(&aosCurrent_Siglock(current));
	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGSTOP));
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
	aos_min_log(eAosMD_TcpProxy, "ftp data thread listen on : %d.%d.%d.%d:%d",
			NIPQUAD(info->listen_addr.sin_addr.s_addr),ntohs(info->listen_addr.sin_port)
			);

	aos_min_log(eAosMD_TcpProxy, "the real server is : %d.%d.%d.%d:%d",
			NIPQUAD(info->rs_addr.sin_addr.s_addr),ntohs(info->rs_addr.sin_port)
			);

	listen_socket = start_listening(&info->listen_addr);
	if(!listen_socket)
	{
		// error process
		aos_min_log(eAosMD_TcpProxy, "ftp data thread listen error");
		return 0;
	}

	aos_min_log(eAosMD_TcpProxy, "ftp data thread listen success, waiting for connection");

	while (aosSock_TcpOpt(listen_socket->sk).accept_queue == NULL)
	{
		add_wait_queue(aosSock_Sleep(listen_socket->sk), &wait);
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		__set_current_state(TASK_RUNNING);
		remove_wait_queue(aosSock_Sleep(listen_socket->sk), &wait);
	}

	ret=sock_create(PF_INET,SOCK_STREAM,IPPROTO_TCP,&client_socket);
	if (ret < 0)
	{
		aos_min_log(eAosMD_TcpProxy, "sock_create client_socket error");
		goto listen_socket_release;
	}

	ret = listen_socket->ops->accept(listen_socket, client_socket, O_NONBLOCK);
	if (ret < 0)
	{
		aos_min_log(eAosMD_TcpProxy, "ftp data thread accept error");
		goto client_socket_release;
	}

	if (aosSock_State(client_socket->sk) != TCP_ESTABLISHED)
	{
		aos_min_log(eAosMD_TcpProxy, "csock accept error %d",aosSock_State(client_socket->sk));
	}

	aos_min_log(eAosMD_TcpProxy, "ftp data thread accept success");


	server_socket=connect_to_address(&info->rs_addr);
	if(!server_socket)
	{
		aos_min_log(eAosMD_TcpProxy, "ftp data thread connect to real server error");
		goto client_socket_release;
	}

	aos_min_log(eAosMD_TcpProxy, "ftp data thread connect to real server success");


	aos_min_log(eAosMD_TcpProxy, "csock status %d",aosSock_State(client_socket->sk));

	tcp_vs_ftp_data_transfer_data(client_socket,server_socket);

	sock_release(server_socket);
client_socket_release:
	sock_release(client_socket);
listen_socket_release:
	sock_release(listen_socket);
	return 0;
}

*/



static int tcp_vs_ftp_data_thread(void * __ftp_data)
{

	struct socket * listen_socket;

	struct tcp_vs_service 	*svc;
	struct tcp_vs_conn	*conn;
	struct tcp_vs_scheduler	*schd;

	struct ftp_data	* info=(struct ftp_data*)__ftp_data;
	int	ret;
	DECLARE_WAITQUEUE(wait, current);

	lock_kernel();
	aosDaemonize("k-ftp-d");

	spin_lock_irq(&aosCurrent_Siglock(current));
	siginitsetinv(&current->blocked,
		      sigmask(SIGKILL) | sigmask(SIGSTOP));
	aos_recalc_sigpending(current);
	spin_unlock_irq(&aosCurrent_Siglock(current));
	/*
	aos_min_log(eAosMD_TcpProxy, "ftp data thread listen on : %d.%d.%d.%d:%d",
			NIPQUAD(info->listen_addr.sin_addr.s_addr),ntohs(info->listen_addr.sin_port)
			);
	aos_min_log(eAosMD_TcpProxy, "the real server is : %d.%d.%d.%d:%d",
			NIPQUAD(info->rs_addr.sin_addr.s_addr),ntohs(info->rs_addr.sin_port)
			);
	*/
	listen_socket = start_listening(&info->listen_addr);
	if(!listen_socket)
	{
		// error process
		aos_min_log(eAosMD_TcpProxy, "ftp data thread listen error");
		return 0;
	}

	//aos_min_log(eAosMD_TcpProxy, "ftp data thread listen success, waiting for connection");

	// refer to inet_csk_accept()
	//while (aosSock_TcpOpt(listen_socket->sk).accept_queue == NULL)
	while(reqsk_queue_empty(&(inet_csk(listen_socket->sk)->icsk_accept_queue)))
	{
		add_wait_queue(aosSock_Sleep(listen_socket->sk), &wait);
		__set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
		__set_current_state(TASK_RUNNING);
		remove_wait_queue(aosSock_Sleep(listen_socket->sk), &wait);
	}

	svc = aos_malloc(sizeof(*svc));
	if(!svc)
	{
		aos_min_log(eAosMD_TcpProxy, "no available memory");
		goto listen_socket_release;
	}
	memset(svc,0,sizeof(*svc));
	svc->lock = RW_LOCK_UNLOCKED;
	INIT_LIST_HEAD(&svc->destinations);
	tcp_vs_add_dest(svc,info->rs_addr.sin_addr.s_addr,info->rs_addr.sin_port,0,0);
	svc->conf.front_ssl_flags=info->front_ssl_flags;
	svc->conf.back_ssl_flags=info->back_ssl_flags;
	aos_min_log(eAosMD_TcpProxy, "malloc svc ok");

	schd=aos_malloc(sizeof(*schd));
	if(!schd)
	{
		aos_min_log(eAosMD_TcpProxy, "no available memory");
		goto svc_release;
	}
	memset(schd,0,sizeof(*schd));
	schd->schedule=tcp_vs_ftp_schedule;
	svc->scheduler=schd;
	conn = (struct tcp_vs_conn*)AosAppProxy_connCreate(listen_socket);
	if (!conn)
	{
		goto svc_release;
	}
	conn->svc = svc;
	conn->front_ssl_flags = svc->conf.front_ssl_flags;
	conn->backend_ssl_flags = svc->conf.back_ssl_flags;
	//save main session id and bin file ptr vars
	conn->session_id = info->main_conn->session_id;
	conn->bin_file_ptr = info->main_conn->bin_file_ptr;
	//accessorial thread flags 0xFF
	conn->log_buffer.len = 0xff;

	aos_trace("data front_ssl_flags=%d",conn->front_ssl_flags);
	aos_trace("data backend_ssl_flags=%d",conn->backend_ssl_flags);

	ret = listen_socket->ops->accept(listen_socket,conn->csock, O_NONBLOCK);
	if (ret < 0)
	{
		aos_min_log(eAosMD_TcpProxy, "ftp data thread accept error");
		goto conn_release;
	}

	if (aosSock_State(conn->csock->sk) != TCP_ESTABLISHED)
	{
		aos_min_log(eAosMD_TcpProxy, "csock accept error");
	}
	aos_min_log(eAosMD_TcpProxy, "ftp data thread accept success");
	aos_min_log(eAosMD_TcpProxy, "ready for ssl");

	// start ssl server state machine
	if (conn->front_ssl_flags)
	{
#ifdef CONFIG_AOS_SSL
		conn->front_cert = AosCertMgr_getSystemCert();
		if (conn->front_cert)
		{
			AosCert_hold(conn->front_cert);
		}
		else
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system cert");
			goto conn_release;
		}
		conn->front_prikey = AosCertMgr_getSystemPrivKey();
		//if (!aos_ssl_hardware_flag && !conn->front_prikey)
		if ( (gAosSslAcceleratorType != eAosSslAcceleratorType_517PM) && !conn->front_prikey)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no system pri key");
			goto conn_release;
		}
		conn->front_authorities = AosCertChain_getByName("system");
		if (!conn->front_authorities)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed, no authorities certs");
			goto conn_release;
		}
/*		if(AosSsl_Start(conn, eAosSSLFlag_Server) < 0)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm, "ssl start failed");
			goto conn_release;
		}*/
#endif
	}

	aos_min_log(eAosMD_TcpProxy, "ready for conn_handle()");
	tcp_vs_conn_handle(conn,svc);
	aos_min_log(eAosMD_TcpProxy, "conn_handle() end");

conn_release:
	aos_free(info);
	AosAppProxy_connRelease(conn);
svc_release:
	aos_free(svc);
listen_socket_release:
	sock_release(listen_socket);
	return 0;
}

/*****
static void session_time_control(unsigned long data)
{
	char *buf="500 Time out/r/n";
	struct tcp_vs_conn *conn=(struct tcp_vs_conn *)(data);
        tcp_vs_sendbuffer(conn->csock,buf,strlen(buf),0);
	aos_free( conn->session_timer );
//	sock_release(conn->dsock);
//	sock_release(conn->csock);
        conn->session_time_out_flags=1;
}
*****/
extern void session_time_control(unsigned long data);

static int tcp_vs_ftp_pass_parse(struct tcp_vs_conn * conn, char *app_data, unsigned int * app_data_len)
{
	int colon,i;

	//aos_min_log(eAosMD_TcpProxy, "ftp parsing PASS");

	colon=0;
	while( colon<*app_data_len && app_data[colon]!=':' )
	{
		colon++;
	}

	if( colon == *app_data_len )
	{
		aos_trace("FTP: Please use the passowrd PROXY_PASSWORD:FTP_PASSWORD");
		return -1;
	}

	i=5;
	while( i<colon && i-5<PROXY_PASSWORD_LEN-1 )
	{
		conn->proxy_password[i-5]=app_data[i];
		i++;
	}

	for( i=colon+1; i<=*app_data_len; i++)
	{
		app_data[i-colon+4] = app_data[i];
	}

	conn->proxy_password[PROXY_USERNAME_LEN-1]=0;

#ifndef AOS_ZQL_SET_DEBUG
	conn->error_info_prefix.buf = ftp_prefix;
	conn->error_info_prefix.len = strlen( ftp_prefix );
	conn->cs_msg_to_daemon = HAC_DAEMON_CS_MSG_DEFAULT;
	//AOS_DEBUG_PRINTK("%s[%d]\n", conn->error_info_prefix.buf, conn->error_info_prefix.len );
	if( !tcp_vs_authentication(conn) )
	{
		aos_min_log(eAosMD_TcpProxy, "ftp authentication failed, username=%s,password=%s.",
				conn->proxy_username,
				conn->proxy_password);
		proxy_send_string( conn->auth_fail_infor.buf, conn->auth_fail_infor.len, conn );
		VCHAR_FREE(conn->auth_fail_infor);
		return -1;
	}
#endif
	//open daemon sock !
	aos_send_log_to_daemon( "", 0, conn, SERVERTOCLIENT );

	conn->proxy_logintime = get_seconds();

	*app_data_len -= colon-4;

	if( session_time_control_init( conn ) < 0 )return -1;

	conn->tcp_vs_status = TCP_VS_NORMAL;
	
	aos_trace("FTP proxy password:%s. new pass command:%s.",conn->proxy_password,app_data);
	return 0;
}


static int tcp_vs_ftp_user_parse(struct tcp_vs_conn * conn, char *app_data, unsigned int * app_data_len)
{
	int colon,i;

	//aos_min_log(eAosMD_TcpProxy, "ftp parsing USER");

	colon=0;
	while( colon<*app_data_len && app_data[colon]!=':' )
	{
		colon++;
	}

	if( colon == *app_data_len )
	{
		aos_trace("FTP: Please use the username PROXY_USER:FTP_USER");
		return -1;
	}

	i=5;
	while( i<colon && i-5<PROXY_USERNAME_LEN-1 )
	{
		conn->proxy_username[i-5]=app_data[i];
		i++;
	}

	for( i=colon+1; i<=*app_data_len; i++)
	{
		app_data[i-colon+4] = app_data[i];
	}
	conn->proxy_username[PROXY_USERNAME_LEN-1]=0;

	*app_data_len -= colon-4;

	aos_trace("FTP proxy username:%s. new user command:%s.",conn->proxy_username,app_data);
	return 0;
}

static int tcp_vs_ftp_port_parse(struct tcp_vs_conn * conn, char *app_data, unsigned int * app_data_len)
{
	int	len;
	unsigned char	a[6];
	u_int32_t	s1;
	struct ftp_data *info;

	//aos_min_log(eAosMD_TcpProxy, "parsing PORT");

	info=aos_malloc(sizeof(struct ftp_data));
	if(!info)
	{
		aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "aos_malloc struct ftp_data error,out of memory");
		return 0;
	}
	memset(info,0,sizeof(struct ftp_data));
	conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&info->listen_addr,&len,0);
	find_unused_port(&info->listen_addr);

	conn->dsock->ops->getname(conn->csock,(struct sockaddr*)&info->rs_addr,&len,1);
	s1=info->rs_addr.sin_addr.s_addr;
	extract_address(&info->rs_addr,&app_data[5]);
	info->rs_addr.sin_addr.s_addr=s1;
	//aos_min_log(eAosMD_TcpProxy,"start ftp data thread");
	if (conn->svc->conf.back_ssl_flags)
	{
#ifdef CONFIG_AOS_SSL
		info->front_ssl_flags=(conn->svc->conf.back_ssl_flags&(~eAosSSLFlag_Backend))|eAosSSLFlag_Front;
		//if(aos_ssl_hardware_flag)
		if( gAosSslAcceleratorType == eAosSslAcceleratorType_517PM )
			info->front_ssl_flags |= eAosSSLFlag_ClientAuth;
#endif
	}
	if (conn->svc->conf.front_ssl_flags)
	{
		info->back_ssl_flags=(conn->svc->conf.front_ssl_flags&~(eAosSSLFlag_Front|eAosSSLFlag_ClientAuth))|eAosSSLFlag_Backend;
	}
	//back up main thread conn
	info->main_conn = conn;

	aos_trace("control front_ssl_flags=%d",conn->front_ssl_flags);
	aos_trace("control backend_ssl_flags=%d",conn->backend_ssl_flags);

	kernel_thread(tcp_vs_ftp_data_thread,info,CLONE_VM|CLONE_FS|CLONE_FILES);


	//wait until the ftpdata started

	n2com(&info->listen_addr,a);
	sprintf(app_data,"PORT %d,%d,%d,%d,%d,%d\r\n",a[0],a[1],a[2],a[3],a[4],a[5] );
	*app_data_len=strlen(app_data);
	//aos_min_log(eAosMD_TcpProxy, "parse PORT: rewrite PORT '%s'",app_data);

	return 0;
}

static int tcp_vs_ftp_pasv_parse(struct tcp_vs_conn * conn, char *app_data, unsigned int * app_data_len)
{


	return 0;

}
static int tcp_vs_ftp_abor_parse(struct tcp_vs_conn * conn, char *app_data, unsigned int * app_data_len)
{


	return 0;

}

int	strcasencmp(char * s1, char * s2, int n)
{
	int	i;
	for(i=0;i<n;i++)
	{
		if( (s1[i]-s2[i]) % ('a'-'A') )
			return 1;
	}
	return 0;
}
/*
#ifdef CONFIG_AOS_TCPVS_FTP_LOG

//add by Zhang Quanlin, 2006/08/23
void  write_ftp_bin_data(char *app_data, unsigned int app_data_len,\
				struct tcp_vs_conn *conn, u8 direct )
{
	int	len;
	BINDATAHEADER dataHeader;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	KFILE	*binFilp=get_ftp_bin_filp();

	if( !conn->session_id )
		conn->session_id = ((u64)get_seconds()<<32) | current->tgid;

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&len,1);
	conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);

	dataHeader.direct       = direct;
	dataHeader.timeSec      = get_seconds()-AOS_TIMEZONE_SECS;
	dataHeader.sessionId    = conn->session_id;
	dataHeader.clientIP     = ntohl(client_addr.sin_addr.s_addr);
	dataHeader.destIP       = ntohl(server_addr.sin_addr.s_addr);
	dataHeader.hostnameLen  = strlen( system_utsname.nodename );
 	dataHeader.dataLen      = (u32)app_data_len;

	write_lock_bh( &ftp_bin_filp_lock );
	if( get_seconds() - ftp_filename_seconds >= AOS_DAY_SECONDS )
	{
		get_ftp_txt_filp() = aos_openvsfile( get_ftp_txt_filp(), FTP_TXT_LOGNAME, NULL );
		binFilp = get_ftp_bin_filp() = aos_openvsfile( binFilp, FTP_BIN_LOGNAME, &ftp_filename_seconds );
	}
	//write dataHeader、hostname and packet data in turns
	aos_fwrite( (char *)&dataHeader, sizeof(BINDATAHEADER), binFilp );
	aos_fwrite( system_utsname.nodename, dataHeader.hostnameLen, binFilp );
	aos_fwrite( app_data, app_data_len, binFilp );

	write_unlock_bh( &ftp_bin_filp_lock );
}

#ifdef AOS_TCPVS_FTP_TXT_LOG

inline void  write_ftp_txt_data(char *app_data, unsigned int app_data_len,\
		                 struct tcp_vs_conn *conn, u8 direct )
{
	int	   len;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	char   printStr[256];

	if( !conn->session_id )
		conn->session_id = ((u64)get_seconds()<<32) | current->tgid;

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&len,1);
	conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);

	aos_LogGetTimeStr( printStr );
	strcat( printStr, aos_LogGetHostName() );
	len=strlen( printStr );
	sprintf( printStr+len,\
			(direct==CLIENTTOSERVER)?FTP_CS_LOGFMTSTR:FTP_SC_LOGFMTSTR,\
			(u32)(conn->session_id>>32),\
			(u32) conn->session_id );
	len=strlen( printStr );
	aos_LogGetIPStr( &client_addr, printStr+len );
	strcat( printStr,"->" );
	len=strlen( printStr );
	aos_LogGetIPStr( &server_addr, printStr+len );
	strcat( printStr," " );

	len=0;
	if( app_data[ app_data_len - 1 ] != 0x0D ){ //if the end of app_data char isn't ENTER, then add it
		app_data[ app_data_len ]=0x0D;
		len = 1;
	}
	write_lock_bh(&ftp_txt_filp_lock);
	//if( ( get_seconds() - ftp_filename_seconds ) >= AOS_DAY_SECONDS )
	//	get_ftp_txt_filp() = aos_openvsfile( get_ftp_txt_filp(), FTP_TXT_LOGNAME, &ftp_filename_seconds );

	aos_fwrite( printStr, strlen(printStr), get_ftp_txt_filp() );
	aos_fwrite( app_data, app_data_len+len, get_ftp_txt_filp() );
	write_unlock_bh(&ftp_txt_filp_lock);
	if( len )app_data[app_data_len]=0;
}
#endif  //just for txt log mode

#endif
*/
static int  tcp_vs_ftp_client_to_server(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	char	cmd[5];
	int		i;

	struct
	{
		char	*cmd;
		int 	(*func)(struct tcp_vs_conn*,char*,unsigned int*);
	}
	cmd_list[]=
	{
		{"ABOR", tcp_vs_ftp_abor_parse},
		{"ACCT", 0},
		{"APPE", 0},
		{"ALLO", 0},
		{"AUTH", 0},
		{"CWD", 0},
		{"CDUP", 0},
		{"DELE", 0},
		{"EPRT", 0},
		{"EPSV", 0},
		{"FEAT", 0},
		{"HELP", 0},
		{"LIST", 0},
		{"MDTM", 0},
		{"MKD", 0},
		{"MODE", 0},
		{"MLFL", 0},
		{"MAIL", 0},
		{"MSND", 0},
		{"MSOM", 0},
		{"MSAM", 0},
		{"MRSQ", 0},
		{"MRCP", 0},
		{"NLST", 0},
		{"NOOP", 0},
		{"OPTS", 0},
		{"PASV", tcp_vs_ftp_pasv_parse},
		{"PORT", tcp_vs_ftp_port_parse},
		{"PWD", 0},
		{"PASS", tcp_vs_ftp_pass_parse},
		{"QUIT", 0},
		{"REIN", 0},
		{"RETR", 0},
		{"RMD", 0},
		{"REST", 0},
		{"RNFR", 0},
		{"RNTO", 0},
		{"SMNT", 0},
		{"STRU", 0},
		{"SITE", 0},
		{"SYST", 0},
		{"STAT", 0},
		{"STOR", 0},
		{"SIZE", 0},
		{"STOU", 0},
		{"TYPE", 0},
		{"USER", tcp_vs_ftp_user_parse},
		{"XCUP", 0},
		{"XCWD", 0},
		{"XMKD", 0},
		{"XPWD", 0},
		{"XRMD", 0},
		{0, 0}

	};

	if(*app_data_len>=eAosSSL_RecordMaxLen-500)
		*app_data_len=eAosSSL_RecordMaxLen-500-1;

	app_data[*app_data_len]=0;

#ifdef CONFIG_AOS_TCPVS_FTP_LOG
	if( conn->bin_file_ptr == NULL )
	{
		aos_write_log_file_header( conn, FTP_BIN_LOGNAME );
		//main thread flags 0x0F
		conn->log_buffer.len = 0x0F;
	}
	if( write_ftp_bin_data( app_data, *app_data_len, conn, CLIENTTOSERVER ) < 0 )
		return -1;
#endif
	if(*app_data_len<3)
		return -1;

	for(i=0;i<4;i++)
		cmd[i]=app_data[i];
	cmd[4]=0;
	if(cmd[3]==10 || cmd[3]==13)
		cmd[3]=0;

	aos_eng_log(eAosMD_TcpProxy, "client_to_server: '%s'",cmd);

	i=0;
	do
	{
		if( strcasencmp(cmd_list[i].cmd,cmd,4)==0 )
		{
			if(cmd_list[i].func)
			{
				if ( cmd_list[i].func(conn,app_data,app_data_len) < 0 )
				{
					return -1;
				}
			}

			return 0;
		}
		i++;
	}
	while(cmd_list[i].cmd);

	aos_eng_log(eAosMD_TcpProxy,"client sending rubbish");
	*app_data_len=-1;

	return -1;
}

static int  tcp_vs_ftp_server_to_client(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;

#ifdef CONFIG_AOS_TCPVS_FTP_LOG
	//zql
	if( conn->bin_file_ptr == NULL )
	{
		aos_write_log_file_header( conn, FTP_BIN_LOGNAME );
		//main thread flags 0x0F
		conn->log_buffer.len = 0x0F;
	}
	if( write_ftp_bin_data( app_data, *app_data_len, conn, SERVERTOCLIENT ) < 0 )
		return -1;
#endif

	return 0;
}

//add by Zhang Quanlin, 2006/12/13
static int tcp_vs_ftp_app_data_release(struct tcp_vs_conn *conn)
{
	if( conn->log_buffer.len == 0x0F ) //main thread
	{
#ifdef CONFIG_AOS_TCPVS_FTP_LOG
		aos_fclose( conn->bin_file_ptr );
		aos_close_daemon_sock( conn );
#endif
		session_time_control_destroy( conn );
	}
	else {
	}
	conn->tcp_vs_status = TCP_VS_END;
	return 0;
}

static struct tcp_vs_scheduler tcp_vs_ftp_scheduler = {
	{0},							// n_list
	"ftp",							// name
	0,					// this module
	tcp_vs_ftp_init_svc,			// initializer
	tcp_vs_ftp_done_svc,			// done
	tcp_vs_ftp_update_svc,			// update
	tcp_vs_ftp_schedule,			// select a server by ftp request
	tcp_vs_ftp_server_to_client,								// server to client
	tcp_vs_ftp_client_to_server,				// client to server
	NULL,							// checking permit
	0,
	tcp_vs_ftp_app_data_release		// release app processing request
};

static __exit void tcp_vs_ftp_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_ftp_scheduler);
}

static __init int tcp_vs_ftp_init(void)
{
	INIT_LIST_HEAD(&tcp_vs_ftp_scheduler.n_list);
	return register_tcp_vs_scheduler(&tcp_vs_ftp_scheduler);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("zzh");
module_init( tcp_vs_ftp_init );
module_exit( tcp_vs_ftp_cleanup );

