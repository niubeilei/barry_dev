///////////////////////////////////////////////////////////////////////
// Author: Li Qin
// Creation Time: 10/09/2005
// Description:
//
// Change History
// 10/09/2005 File created
///////////////////////////////////////////////////////////////////////

#include <linux/net.h>
#include <net/sock.h>

#include "Ktcpvs/tcp_vs.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Timer.h"
#include "aosUtil/Memory.h"
#include "ssl/aosSslProc.h"
#include "aos/aosReturnCode.h"

#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
#include "Ktcpvs/aosKernelFile.h"
#include "Ktcpvs/aosLogFunc.h"

#define TCP_VS_MAX_HOSTNAME 256

/*
* date : 2006.12.12
* add under some codes by ZQL
*/

#define TCP_VS_BIN_LOGNAME	"/usr/local/keyou/Log/%04d/%02d%02d/telnet.%s.log"
#define write_telnet_bin_data		aos_process_log_data
/*
* end 2006.12.12
* add up codes over by ZQL
*/
#endif

static int tcp_vs_telnet_init_svc(struct tcp_vs_service *svc)
{
	return 0;
}

static int tcp_vs_telnet_done_svc(struct tcp_vs_service *svc)
{
	return 0;
}

static int tcp_vs_telnet_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

/*
//
//      Parse TELNET protocol packet
//
static int parse_telnet_header(const char *buffer, size_t buflen, uint32_t *ipaddr, uint16_t *port)
{

	//aos_trace_hex("parsing TELNET proto first packet", buffer, buflen);
	memcpy((uint8*)ipaddr, buffer, 4);
	memcpy((uint8*)port, buffer+4, 2);

	 return eAosRc_Success;
	 }
*/

static int telnet_connect( struct tcp_vs_conn * conn
						  )
{
	//tcp_vs_dest_t *dest;
	//struct socket *dsock = NULL;
	struct tcp_vs_service * svc = conn->svc;
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
	/*
	aos_min_log(eAosMD_TcpProxy, "TELNET: %s backend server %d.%d.%d.%d:%d "
	   "conns %d refcnt %d weight %d\n",
	   svc->conf.dynamic_dst?"dynamic":"static",
	   NIPQUAD(dest->addr), ntohs(dest->port),
	   atomic_read(&dest->conns),
	   atomic_read(&dest->refcnt), dest->weight);
	*/
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
	//conn->dest = dest;
	//conn->dsock = dsock;

	return 0;

}
/*
#ifdef LOG_NULL
//CONFIG_AOS_TCPVS_TELNET_LOG
//add by Zhang Quanlin, 2006/08/06
void  write_telnet_bin_data(char *app_data, unsigned int app_data_len,\
							struct tcp_vs_conn *conn, u8 direct )
{
	int	len;
	BINDATAHEADER dataHeader;
	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;
	KFILE	*binFilp=get_telnet_bin_filp();

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
	if( direct == CLIENTTOSERVER )
		dataHeader.dataLen = (u32)app_data_len + conn->log_buffer.len;
	else dataHeader.dataLen = (u32)app_data_len;

	write_lock_bh( &telnet_bin_filp_lock );
	if( get_seconds() - telnet_filename_seconds >= AOS_DAY_SECONDS )
	{
		get_telnet_txt_filp() = aos_openvsfile( get_telnet_txt_filp(), TCP_VS_TXT_LOGNAME, NULL );
		binFilp = get_telnet_bin_filp() = aos_openvsfile( binFilp, TCP_VS_BIN_LOGNAME, &telnet_filename_seconds );
	}
	//write dataHeader,hostname and packet data in turns
	aos_fwrite( (char *)&dataHeader, sizeof(BINDATAHEADER), binFilp );
	aos_fwrite( system_utsname.nodename, dataHeader.hostnameLen, binFilp );
	//08/13/2006
	//only sigle character mode use this code when packet send to server from client .
	if( conn->log_buffer.len && direct == CLIENTTOSERVER )
	{
		aos_fwrite( conn->log_buffer.buf, conn->log_buffer.len, binFilp );
		//Note : If you don't write telnet txt log,
		//       You must set conn->log_buffer.len zero here !!!
		//conn->log_buffer.len = 0;
	}
	aos_fwrite( app_data, app_data_len, binFilp );
	write_unlock_bh( &telnet_bin_filp_lock );
}

//add by Zhang Quanlin, 2006/08/26
void  write_telnet_txt_data(char *app_data, unsigned int app_data_len,\
							struct tcp_vs_conn *conn )
{
	int		len;
	struct	sockaddr_in client_addr;
	struct	sockaddr_in server_addr;
	char	printStr[256];
	KFILE	*txtFilp=get_telnet_txt_filp();

	if( !conn->session_id )
		conn->session_id = ((u64)get_seconds()<<32) | current->tgid;

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&len,1);
	conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);

	aos_LogGetTimeStr( printStr );
	strcat( printStr, aos_LogGetHostName() );
	len=strlen( printStr );
	sprintf( printStr+len, TCP_VS_CS_LOGFMTSTR,\
			(u32)(conn->session_id>>32),\
			(u32) conn->session_id );
	len=strlen( printStr );
	aos_LogGetIPStr( &client_addr, printStr+len );
	strcat( printStr,"->" );
	len=strlen( printStr );
	aos_LogGetIPStr( &server_addr, printStr+len );
	strcat( printStr," " );

	write_lock_bh( &telnet_txt_filp_lock );
	aos_fwrite( printStr, strlen(printStr), txtFilp );
	// 08/13/2006
	// Control key send to server in 3 characters, when sigle character mode .
	// So it can jump over other characters those were inputed a while ago.
	if( conn->log_buffer.len )
	{
		aos_fwrite( conn->log_buffer.buf, conn->log_buffer.len, txtFilp );
		conn->log_buffer.len = 0;
	}
	//app_data[ app_data_len ]= '\n';
	aos_fwrite( app_data, app_data_len, txtFilp );
	aos_fwrite( "\n", 1, txtFilp );
	write_unlock_bh( &telnet_txt_filp_lock );
	//app_data[ app_data_len ]= 0x0;
}

#endif
*/

//
//    TELNET protocol content-based scheduling
//    Parse the telnet request, select a server according to the
//    request, and create a socket the server finally.

static int tcp_vs_telnet_schedule(
								  struct tcp_vs_conn *conn,
								  struct tcp_vs_service *svc,
								  char*  req_client,
								  int    req_len)
{

	//#ifdef CONFIG_AOS_TCPVS_TELNET_AUTH
	/*char control_code[]={ 0xff,0xfb,0x01};
	int len;
	//aos_min_log(eAosMD_TcpProxy, "Handle telnet request");

	 switch(conn->tcp_vs_status)
	 {
	 case TCP_VS_START:
	 len=tcp_vs_sendbuffer(conn->csock,control_code,3,0);
	 if(len<3)return -1;
	 break;
	 default:
	 aos_trace("telnet status error");
	 return -1;
	 }
	*/
	//#else
	//conn->tcp_vs_status=TCP_VS_NORMAL;
#ifdef CONFIG_AOS_TCPVS_TELNET_AUTH
	conn->tcp_vs_status=TCP_VS_START;
#else
	conn->tcp_vs_status=TCP_VS_NORMAL;
#endif
	if( telnet_connect(conn) < 0 )return -1;
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
	aos_write_log_file_header( conn, TCP_VS_BIN_LOGNAME );
	VCHAR_MALLOC( conn->log_buffer, AOS_LOG_BUF_SIZE );
#endif
	return 0;
}

#ifdef CONFIG_AOS_TCPVS_TELNET_AUTH

static int triprn(char * s)
{
	int ilen;
	int len=strlen(s);
	aos_trace("triprn:%s",s);
	for( ilen=0; ilen<len; ilen++ )
	{
		if( s[ilen]=='\r' || s[ilen]=='\n' )
		{
			s[ilen]=0;
			break;
		}
	}
	aos_trace("triprn1:%s",s);
	return ilen;
}

/*********
//Add by zql
static void session_time_control(unsigned long data)
{
	char *buf="\n\rThe time you are allowed to access is expired\r\n";
	struct tcp_vs_conn *conn=(struct tcp_vs_conn *)(data);
	tcp_vs_sendbuffer(conn->csock,buf,strlen(buf),0);
	aos_free( conn->session_timer );
//	sock_release(conn->csock);
//	sock_release(conn->dsock);
        conn->session_time_out_flags=1;
}
//END
*********/
extern void session_time_control(unsigned long data);

static int input_is_enter( u8 *app_data, u16 app_data_len )
{
	register u16 ilen = 0;
	if( app_data_len > 3 )ilen = app_data_len - 3;
	for( ; ilen < app_data_len; ilen++ )
	{
		if( app_data[ilen] == '\r' || app_data[ilen] == '\n' )return 1;
	}
	return 0;
}

inline int proxy_send_string( char *str, int strlen, struct tcp_vs_conn *conn )
{
	if( tcp_vs_sendbuffer( conn->csock, str, strlen, 0 ) < strlen )return -1;
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
	write_telnet_bin_data( str, strlen, conn, SERVERTOCLIENT );
#endif
	return 1;
}

#define PROXYAUTHFAIL	-1
#define PROXYAUTHSTART	 1

inline int init_proxy_auth( struct tcp_vs_conn *conn, int flags )
{
	char *login_str = "Proxy Login: ";
	if( flags == PROXYAUTHFAIL )
	{
		char *result_str = "\n\rProxy Login incorrect !\n\r\n\r";
		//If try to login times more than three times, then exit
		if( conn->proxy_logintime >= 3 )return -1;
		if( proxy_send_string( result_str, strlen(result_str), conn ) < 0 )
			return -1;
		if( proxy_send_string( conn->auth_fail_infor.buf, conn->auth_fail_infor.len, conn ) < 0 )
			return -1;
		VCHAR_FREE(conn->auth_fail_infor);
	}
	if( proxy_send_string( login_str, strlen(login_str), conn ) < 0 )
		return -1;
	conn->proxy_username[ 0 ] = '\0';
	conn->proxy_password[ 0 ] = '\0';
	conn->tcp_vs_status = TCP_VS_LOGIN;
	//Conn->proxy_logintime is used for login times temporarily
	conn->proxy_logintime++;
	return 0;
}

//static slowtimer_t mytimer;
#define PROXY_AUTH_WRITE_LOG( proxystr, slen ) \
	int blen = conn->log_buffer.len;\
	conn->log_buffer.len = 0;\
	write_telnet_bin_data( proxystr, slen, conn, CLIENTTOSERVER );\
	conn->log_buffer.len = blen;

/*
 * Process_proxy_auth function add by Zhang Quanlin, 2006/08/26
 */
int process_proxy_auth(u8 *app_data, unsigned int *app_data_len,\
					   struct tcp_vs_conn *conn, u8 direct )
{
	switch( conn->tcp_vs_status )
	{
	case TCP_VS_START:
		if( direct == CLIENTTOSERVER ||\
			0xFF   == app_data[0] ||\
		  ( 0x20   == app_data[0] &&\
		    0xFF   == app_data[1] ) )return 0;
		{
			char *result_str = "\n\rProxy Login Success !\n\r";
			//Conn->tcp_vs_inputbuf is used for saving server's login packet temporarily
			if( !conn->log_buffer.len )
			{
				strcpy(conn->log_buffer.buf, result_str );
				conn->log_buffer.len = strlen(result_str);
			}
			strncat( conn->log_buffer.buf, app_data, AOS_LOG_BUF_SIZE );
			conn->log_buffer.buf[AOS_LOG_BUF_SIZE-1] = '\0';
			conn->log_buffer.len += *app_data_len;
			if( init_proxy_auth( conn, PROXYAUTHSTART ) < 0 )return -1;
			*app_data_len=0;
		}
		break;
	case TCP_VS_LOGIN:
		strncat( conn->proxy_username, app_data, PROXY_USERNAME_LEN );
		conn->proxy_username[ PROXY_USERNAME_LEN - 1 ] = '\0';
		//aos_fprintf( get_telnet_txt_filp(), "telnet proxy login(%2d): %s\n",*app_data_len, app_data);
		if( input_is_enter( app_data, *app_data_len ) )
		{
			char *pswd_str="\n\rPassword: ";
			int slen = triprn(conn->proxy_username);
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
			PROXY_AUTH_WRITE_LOG( conn->proxy_username, slen )
#endif
			if( !slen )
			{
				if( init_proxy_auth( conn, PROXYAUTHFAIL ) < 0 )
					return -1;
			}
			else {
				if( proxy_send_string( pswd_str, strlen(pswd_str), conn ) < 0 )
					return -1;
				conn->tcp_vs_status = TCP_VS_PASSWORD;
				conn->proxy_password[0] = 0;
			}
		}
		else {
			if( proxy_send_string( app_data, *app_data_len, conn ) < 0 )
				return -1;
		}
		*app_data_len=0;
		break;
	case TCP_VS_PASSWORD:
		strncat( conn->proxy_password, app_data, PROXY_PASSWORD_LEN );
		conn->proxy_password[ PROXY_PASSWORD_LEN-1 ] = '\0';
		//aos_fprintf( get_telnet_txt_filp(), "telnet proxy login(%2d): %s\n",*app_data_len, app_data);
		if( input_is_enter( app_data, *app_data_len ) )
		{
			int ret, slen = triprn( conn->proxy_password );
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
			PROXY_AUTH_WRITE_LOG( conn->proxy_password, slen )
#endif
			aos_trace( "telnet username=%s,passowrd=%s.", conn->proxy_username, conn->proxy_password );

#ifndef AOS_ZQL_SET_DEBUG
			//set error information prefix in access control and authrization
			conn->error_info_prefix.buf = NULL;
			conn->error_info_prefix.len = 0;
			conn->cs_msg_to_daemon = HAC_DAEMON_CS_MSG_DEFAULT;
	 		ret = tcp_vs_authentication(conn);
#else
       		ret = TRUE;
#endif
			if( ret )
			{
				//we need send some context whose server has sended to client before.
				aos_send_log_to_daemon( "", 0, conn, SERVERTOCLIENT );
				if( proxy_send_string( conn->log_buffer.buf,\
						conn->log_buffer.len, conn ) < 0 )
					return -1;
				conn->tcp_vs_status   = TCP_VS_NORMAL;
				conn->proxy_logintime = get_seconds();
				conn->log_buffer.len = 0;
				if( session_time_control_init( conn ) < 0 )
						return -1;
			}
			else
			{
				//OK, I will give you new chances !
				if( init_proxy_auth( conn, PROXYAUTHFAIL ) < 0 )
					return -1;
			}
		}
		*app_data_len=0;
		break;
	case TCP_VS_NORMAL:
		break;
	default:
		aos_trace("telnet proxy status error!");
		return -1;
	}
	return 0;
}

#endif

static int  tcp_vs_telnet_client_to_server(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;
	if( conn->tcp_vs_status == TCP_VS_NORMAL )
	{
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
		if( write_telnet_bin_data( app_data, *app_data_len, conn, CLIENTTOSERVER ) < 0 )
			return -1;
#endif
	}
#ifdef CONFIG_AOS_TCPVS_TELNET_AUTH
	else {
		if( process_proxy_auth( app_data, app_data_len, conn, CLIENTTOSERVER ) < 0 )
			return -1;
	}
#endif
	return 0;
}


static int  tcp_vs_telnet_server_to_client(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;

	//add by Zhang Quanlin, 2006/08/26
	switch( conn->tcp_vs_status )
	{
#ifdef CONFIG_AOS_TCPVS_TELNET_AUTH
	case TCP_VS_START:
		if( process_proxy_auth( app_data, app_data_len, conn, SERVERTOCLIENT ) < 0 )
			return -1;
		break;
	case TCP_VS_LOGIN:
	case TCP_VS_PASSWORD:
		strncat( conn->log_buffer.buf, app_data, AOS_LOG_BUF_SIZE );
		conn->log_buffer.buf[AOS_LOG_BUF_SIZE-1] = '\0';
		conn->log_buffer.len += *app_data_len;
		*app_data_len=0;
		break;
#endif
	case TCP_VS_NORMAL:
	default:
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
		//add by Zhang Quanlin, 2006/08/06
		return write_telnet_bin_data( app_data, *app_data_len, conn, SERVERTOCLIENT );
#else
		break;
#endif
	}

	return 0;
}

//add by Zhang Quanlin, 2006/12/13
static int tcp_vs_telnet_app_data_release(struct tcp_vs_conn *conn)
{
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
	switch( conn->tcp_vs_status )
	{
		case TCP_VS_NORMAL:
			//AOS_DEBUG_PRINTK( "ssh tcp_vs_telnet_app_data_release\n" );
			break;
		default:
			break;
	}
	VCHAR_FREE( conn->log_buffer );
	aos_fclose( conn->bin_file_ptr );
	aos_close_daemon_sock( conn );
#endif
	//AOS_DEBUG_PRINTK("begin free session_timer !\n");
	session_time_control_destroy( conn );
	//AOS_DEBUG_PRINTK("end free session_timer !\n");
	conn->tcp_vs_status = TCP_VS_END;
	return 0;
}

static struct tcp_vs_scheduler tcp_vs_telnet_scheduler = {
	{0},							// n_list
		"telnet",							// name
		THIS_MODULE,					// this module
		tcp_vs_telnet_init_svc,			// initializer
		tcp_vs_telnet_done_svc,			// done
		tcp_vs_telnet_update_svc,			// update
		tcp_vs_telnet_schedule,			// select a server by telnet request
#ifdef CONFIG_AOS_TCPVS_TELNET_LOG
		tcp_vs_telnet_server_to_client,
#else
		0,
#endif
		tcp_vs_telnet_client_to_server,							// client to server
		NULL,							// checking permit
		0,
		tcp_vs_telnet_app_data_release	// release app processing request
};

static __init int tcp_vs_telnet_init(void)
{
	INIT_LIST_HEAD(&tcp_vs_telnet_scheduler.n_list);
	return register_tcp_vs_scheduler(&tcp_vs_telnet_scheduler);
}

static __exit void tcp_vs_telnet_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_telnet_scheduler);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yangtao");
module_init( tcp_vs_telnet_init );
module_exit( tcp_vs_telnet_cleanup );

