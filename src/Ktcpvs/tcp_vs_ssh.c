///////////////////////////////////////////////////////////////////////
// Author: Powall Zhang
// Creation Time: 10/30/2006
// Description:
//
// Change History
// 10/30/2006 File created
///////////////////////////////////////////////////////////////////////

#include <linux/net.h>
#include <net/sock.h>

#include "Ktcpvs/tcp_vs.h"
#include "aosUtil/Modules.h"
#include "aosUtil/Tracer.h"
#include "aosUtil/Alarm.h"
#include "aosUtil/Memory.h"
#include "ssl/aosSslProc.h"
#include "aos/aosReturnCode.h"

#ifdef CONFIG_AOS_TCPVS_SSH_LOG
#include "Ktcpvs/aosKernelFile.h"
#include "Ktcpvs/aosLogFunc.h"

#define SSH_MAX_HOSTNAME 256
#define SSH_DEBUG_INFO	 1
/*
 * start 2006.10.06
 * add under some codes by ZQL
 */

#define SSH_BIN_LOGNAME		"/usr/local/keyou/Log/%04d/%02d%02d/ssh.%s.log"
#define write_ssh_bin_data		aos_process_log_data

#endif

#define SSH_CLIENT_VER		0x10
#define SSH_CLIENT_ID		0x305E4E1B
#define SSH_AUTH_PASS		0xff
#define SSH_AUTH_DENY		0xf0

typedef struct SSH_Proxy_Header
{
	u32	version;
	u32	message_id;
	u32 	length;
}SSHDR,*LPSSHDR;

typedef struct SSH_Proxy_context
{
	u32	direct;		//last message data direct
	u32	length;		//last message need data length to superaddition
}SSHCONTEXT,*LPSSHCONTEXT;

static struct SSH_Proxy_Header ssh_prefix={
	SSH_CLIENT_VER,
	SSH_CLIENT_ID,
	SSH_AUTH_DENY
};
/*
 * end 2006.10.06
 * add up codes over by ZQL
 */

static int tcp_vs_ssh_init_svc(struct tcp_vs_service *svc)
{	
	return 0;
}

static int tcp_vs_ssh_done_svc(struct tcp_vs_service *svc)
{	
	return 0;
}

static int tcp_vs_ssh_update_svc(struct tcp_vs_service *svc)
{
	return 0;
}

/*
//
//      Parse SSH protocol packet
//
static int parse_ssh_header(const char *buffer, size_t buflen, uint32_t *ipaddr, uint16_t *port)
{

	//aos_trace_hex("parsing SSH proto first packet", buffer, buflen);
	memcpy((uint8*)ipaddr, buffer, 4);
	memcpy((uint8*)port, buffer+4, 2); 

	return eAosRc_Success;
}
*/

static int ssh_connect( struct tcp_vs_conn * conn
		)
{
	tcp_vs_dest_t *dest;
	struct socket *dsock = NULL;
	struct tcp_vs_service * svc = conn->svc;
	// 
	// Have accepted the connection from a client
	// if set static dest,  it will select dest from static dest list
	// if set dynamic dest, it will select dest according to the first
	// ssh request received from client.
	// Then it will connect to the selected backend server.
	//

	// static processing
		dest = tcp_vs_finddst(svc);
		if (!dest)
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr, "No destination found at vs %s", svc->ident.name);
			return -1;
		}
		atomic_inc(&dest->refcnt);
/*
	aos_min_log(eAosMD_TcpProxy, "SSH: %s backend server %d.%d.%d.%d:%d "
	   "conns %d refcnt %d weight %d\n",
	   svc->conf.dynamic_dst?"dynamic":"static",
	   NIPQUAD(dest->addr), ntohs(dest->port),
	   atomic_read(&dest->conns),
	   atomic_read(&dest->refcnt), dest->weight);
*/	
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
/*
#ifdef CONFIG_AOS_TCPVS_SSH_LOG
//add by Zhang Quanlin, 2006/10/06
void  write_ssh_bin_data(char *app_data, unsigned int app_data_len,\
				struct tcp_vs_conn *conn, u8 direct )
{
	int	len;
	u8	*dedata=NULL;
	BINDATAHEADER dataHeader;
	struct sockaddr_in client_addr;
	KFILE	*binFilp=get_ssh_bin_filp();
	//LPSSHCONTEXT context = (LPSSHCONTEXT)conn->log_buffer.buf;
	
	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&len,1);
	//conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);
	
	dataHeader.direct       = direct;
	dataHeader.timeSec      = get_seconds();
	dataHeader.sessionId    = conn->session_id;
	dataHeader.clientIP     = ntohl(client_addr.sin_addr.s_addr);
	dataHeader.destIP       = ntohl(conn->dest_addr); //ntohl(server_addr.sin_addr.s_addr);
	dataHeader.hostnameLen  = strlen( system_utsname.nodename );
	
	write_lock_bh( &ssh_bin_filp_lock );
	if( get_seconds() - ssh_filename_seconds >= AOS_DAY_SECONDS ) 
	{
		//get_ssh_txt_filp() = aos_openvsfile( get_ssh_txt_filp(), SSH_TXT_LOGNAME, NULL );
		binFilp = get_ssh_bin_filp() = aos_openvsfile( binFilp, SSH_BIN_LOGNAME, &ssh_filename_seconds );
	}
	//write dataHeader、hostname and packet data in turns
	//zlib_decompress_block(context->decompress, app_data, app_data_len, &dedata, &len);
	if( dedata ){
#ifdef SSH_DEBUG_INFO
		aos_printk(AOS_ERROR_LOGNAME, "ssh get %s log data length=%ld/%ld.\n",\
				(direct&0x1) ? "Incoming" : "Outgoing",\
				app_data_len, len );
#endif
		app_data = dedata;
		app_data_len = len;
	}
#ifdef SSH_DEBUG_INFO
	else {
		aos_printk(AOS_ERROR_LOGNAME, "ssh get %s log data length=%ld.\n",\
				(direct&0x1) ? "Incoming" : "Outgoing",\
				app_data_len );
	}
#endif
	dataHeader.dataLen = (u32)app_data_len; 

	aos_fwrite( (char *)&dataHeader, sizeof(BINDATAHEADER), binFilp );
	aos_fwrite( system_utsname.nodename, dataHeader.hostnameLen, binFilp );
	aos_fwrite( app_data, app_data_len, binFilp );
	if( dedata )aos_free( dedata );

	write_unlock_bh( &ssh_bin_filp_lock );
}
#endif
*/

//
//    SSH protocol content-based scheduling
//    Parse the ssh request, select a server according to the
//    request, and create a socket the server finally.
 
static int tcp_vs_ssh_schedule(
	struct tcp_vs_conn *conn, 
	struct tcp_vs_service *svc, 
	char*  req_client, 
	int    req_len)
{

#ifdef CONFIG_AOS_TCPVS_SSH_AUTH
	//aos_min_log(eAosMD_TcpProxy, "Handle ssh request");
	
	switch(conn->tcp_vs_status)
	{
		case TCP_VS_START:
			//conn->tcp_vs_status=TCP_VS_NORMAL;
			//break;
			conn->tcp_vs_status=TCP_VS_LOGIN;
			break;
		default:
			aos_trace("ssh status error");
			return -1;
	}
#else
	conn->tcp_vs_status=TCP_VS_NORMAL;
	ssh_connect(conn);
#endif
	return 0;
}

/*
static char * triprn(char * s)
{
	aos_trace("triprn:%s",s);
	int i;
	int len=strlen(s);
	for( i=0; i<len; i++ )
	{
		if( s[i]=='\r' || s[i]=='\n' )
		{
			s[i]=0;
			break;
		}
	}
	aos_trace("triprn1:%s",s);
	return s;
}


static int ssh_auth_check( struct tcp_vs_conn *conn, int pass )
{
	char	data[32];
	LPSSHDR	header;

	header = (LPSSHDR)data;
	header->version = SSH_CLIENT_VER;
	header->message_id   = SSH_CLIENT_ID;
	data[sizeof(SSHDR)]=pass?SSH_AUTH_PASS:SSH_AUTH_DENY;
	tcp_vs_sendbuffer( conn->csock, data,sizeof(SSHDR)+1,0);
	if(pass){
		conn->tcp_vs_status = TCP_VS_PASS;
		//ssh_connect( conn );
	}
	return pass;
}
*/

static void covert_hash_to_string( unsigned char *hash, char *hstr )
{
	register int i,j;
	for( i=0,j=0; i<SHA_HASH_LENGTH; i++,j+=2 )
		sprintf(hstr+j,"%02X",hash[i]);
	hstr[SHA_HASH_LENGTH*2]='\0';
}

static int ssh_auth_check( unsigned char *app_data, unsigned int app_data_len, struct tcp_vs_conn *conn )
{
	u64	*session_id;
	int	ret,msgLen=0;
        char    data[64];
	struct  sockaddr_in client_addr;
	//struct  tcp_vs_dest *dest=NULL;
        LPSSHDR header = (LPSSHDR)app_data;
	//LPSSHCONTEXT context = (LPSSHCONTEXT)conn->log_buffer.buf;

	if( !conn->session_id )
		conn->session_id = create_session_id();

	if( app_data_len < (sizeof(SSHDR)+header->length) ||
	    header->version != SSH_CLIENT_VER ||
            header->message_id   != SSH_CLIENT_ID )
	{
		conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,&ret,1);
        	aos_LogGetIPStr( &client_addr, data );
		aos_trace("ssh client (%s) is invaild client !.", data );
		return -1;
	}

	//if this thread just used to store the log of ssh
	//then set status to TCP_VS_LOG.
	header = (LPSSHDR)(app_data+sizeof(SSHDR));
	if( header->version == SSH_CLIENT_ID )	//double CLIENT ID
	{
                conn->tcp_vs_status = TCP_VS_LOG;	//set status to LOG
                atomic_dec(&conn->svc->conns);		//cheat server to don't show this connection
		//context->decompress = zlib_decompress_init();
		//real proxy thread conn var piont
		conn->session_id = (((u64)header->message_id)<<32) | header->length;
		conn->dest = tcp_vs_finddst(conn->svc);
		AOS_DEBUG_PRINTK("ssh thread be seted to store log data(%d%05d/%08X)!\n",\
			(u32)(conn->session_id>>32), (u32) conn->session_id, conn->dest->addr );
#ifdef CONFIG_AOS_TCPVS_SSH_LOG
		conn->cs_msg_to_daemon = HAC_DAEMON_CS_MSG_DEFAULT;
		aos_write_log_file_header( conn, SSH_BIN_LOGNAME );
#endif
		conn->dest = NULL;
		VCHAR_MALLOC( conn->log_buffer, sizeof(SSHCONTEXT)+AOS_LOG_BUF_SIZE )
		return 0;
	}

	covert_hash_to_string( app_data+sizeof(SSHDR), conn->proxy_username );
	covert_hash_to_string( app_data+sizeof(SSHDR)+SHA_HASH_LENGTH, conn->proxy_password );

	AOS_DEBUG_PRINTK("ssh username=%s,passowrd=%s.",conn->proxy_username, conn->proxy_password );

#ifndef AOS_ZQL_SET_DEBUG
	conn->error_info_prefix.buf = (char *)&ssh_prefix;
	conn->error_info_prefix.len = sizeof( ssh_prefix );
	//AOS_DEBUG_PRINTK("ssh prefix length [%d]\n", conn->error_info_prefix.len );
	ret = tcp_vs_authentication(conn);
#else
        ret = TRUE;
#endif
	if( ret )
	{
       	header = (LPSSHDR)data;
        	header->version = SSH_CLIENT_VER;
       	header->message_id   = SSH_CLIENT_ID;
		header->length = sizeof(char);
		msgLen += sizeof(SSHDR);

		//add allow or deny to message
        	data[msgLen]=ret?SSH_AUTH_PASS:SSH_AUTH_DENY;
		msgLen += sizeof(char);

		//add conn session ID to message
        	session_id = (u64 *)(data+msgLen);
		*session_id = conn->session_id;
		msgLen += sizeof(u64);
		tcp_vs_sendbuffer( conn->csock, data, msgLen, 0 );
	}
	else {
		if( tcp_vs_sendbuffer( conn->csock, conn->auth_fail_infor.buf, conn->auth_fail_infor.len, 0 ) == 0 )
			return -1;
		VCHAR_FREE(conn->auth_fail_infor);
	}

        if(ret){	//good, we have passed auth process
                conn->tcp_vs_status = TCP_VS_PASSWORD;
        }

        return ret;
}

#define SSH_NORMAL_DATA		0xff

#ifdef CONFIG_AOS_TCPVS_SSH_LOG
static u8 ssh_log_data_check( unsigned char *app_data, unsigned int *app_data_len, struct tcp_vs_conn *conn )
{
        LPSSHDR header = (LPSSHDR)app_data;
	LPSSHCONTEXT context = (LPSSHCONTEXT)conn->log_buffer.buf;
	u32	 msgID = header->message_id ^ header->length;
	int	msgLen = header->length&0xffff;
	u8	*newPos, Direct = header->version&0x1;
	int	newLen = 0;

	if( conn->log_buffer.len > 0 )
	{
		Direct = context->direct;
		msgLen = conn->log_buffer.len;
		if( msgLen > *app_data_len )msgLen = *app_data_len;
#ifdef AOS_ZQL_SET_DEBUG
		AOS_DEBUG_PRINTK("sshX get %s log data length=%ld.\n",\
				(Direct&0x1) ? "Incoming" : "Outgoing",\
				msgLen );
#endif
                write_ssh_bin_data( app_data, msgLen, conn, Direct );
		*app_data_len -= msgLen;
		if( *app_data_len == 0 ){
			conn->log_buffer.len -= msgLen;
			return SSH_NORMAL_DATA;
		}
		newPos = app_data+newLen;
		newLen = *app_data_len;
		while(newLen--)*app_data++=*newPos++;
		conn->log_buffer.len = 0;
		return Direct;
	}

        if ( msgID == SSH_CLIENT_ID )
	{
		if( (msgLen+sizeof(SSHDR)) > *app_data_len )
		{
			conn->log_buffer.len = msgLen+sizeof(SSHDR)-*app_data_len;
			context->direct = Direct;
			msgLen = *app_data_len - sizeof(SSHDR);
		}

		write_ssh_bin_data( app_data+sizeof(SSHDR), msgLen, conn, Direct );

		if( (msgLen+sizeof(SSHDR)) < *app_data_len )
		{
			*app_data_len -= msgLen+sizeof(SSHDR);
			newPos = app_data+msgLen+sizeof(SSHDR);
			newLen = *app_data_len;
			while(newLen--)*app_data++=*newPos++;
		}
		else {
			*app_data_len = 0;
			return SSH_NORMAL_DATA;
		}

		//aos_trace("ssh log packet, direct is \"%s\".", Direct?"RtoC":"CtoS" );
        	return Direct;	/*mean log data direct*/
	}
#ifdef AOS_ZQL_SET_DEBUG
	else AOS_DEBUG_PRINTK( "ssh get ID '%08X' data length=(%d/%ld).\n",\
				msgID, sizeof(SSHDR), *app_data_len );
#endif


	return SSH_NORMAL_DATA;
}
#endif

static int  tcp_vs_ssh_client_to_server(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
	app_data[*app_data_len]=0;
	switch(conn->tcp_vs_status)
	{
		case TCP_VS_LOGIN:
			if( ssh_auth_check( app_data, *app_data_len, conn ) < 0 )
			{
				*app_data_len=0;
				return -1;
			}
			*app_data_len = 0;
			break;
		case TCP_VS_PASSWORD:
			if( !strncmp(app_data,"OK",2) )
			{
				conn->session_id = 0;		//if auth success, set id equal 0, else two thread ids are same.
				conn->tcp_vs_status = TCP_VS_NORMAL;
                		if( ssh_connect( conn ) < 0 ||session_time_control_init( conn ) < 0 )
					return -1;
			}
			*app_data_len = 0;
			break;
		case TCP_VS_NORMAL:
			break;
		case TCP_VS_LOG:
#ifdef CONFIG_AOS_TCPVS_SSH_LOG
			while( ssh_log_data_check( app_data, app_data_len, conn ) != SSH_NORMAL_DATA );
#endif
			*app_data_len = 0;
			break;
		default:
			aos_trace("ssh status error!");
			return -1;
	}

	//aos_eng_log(eAosMD_TcpProxy, "client_to_server: '%s'",cmd);
	//aos_eng_log(eAosMD_TcpProxy,"client sending rubbish");

	return 0;
}

static int  tcp_vs_ssh_server_to_client(unsigned char *app_data, unsigned int * app_data_len, struct tcp_vs_conn *conn, int (*callback)(struct aos_app_proc_req *req))
{
#ifdef CONFIG_AOS_TCPVS_SSH_LOG
	//add by Zhang Quanlin, 2006/11/06
	switch( conn->tcp_vs_status )
	{
		case TCP_VS_LOG:
			*app_data_len = 0;
			break;
		default:
			break;
	}
#endif
	app_data[*app_data_len]=0;
	return 0;
}

//add by Zhang Quanlin, 2006/11/13
static int tcp_vs_ssh_app_data_release(struct tcp_vs_conn *conn)
{
#ifdef CONFIG_AOS_TCPVS_SSH_LOG
	//LPSSHCONTEXT context = (LPSSHCONTEXT)conn->log_buffer.buf;
	switch( conn->tcp_vs_status )
	{
		case TCP_VS_LOG:
			//AOS_DEBUG_PRINTK("ssh tcp_vs_ssh_app_data_release\n" );
			//zlib_decompress_cleanup(context->decompress);
			atomic_inc( &conn->svc->conns );
			break;
		default:
			break;
	}
	aos_fclose( conn->bin_file_ptr );
	VCHAR_FREE( conn->log_buffer );
	aos_close_daemon_sock( conn );
#endif
	session_time_control_destroy( conn );
	conn->tcp_vs_status = TCP_VS_END;
	return 0;
}

static struct tcp_vs_scheduler tcp_vs_ssh_scheduler = {
	{0},					// n_list 
	"ssh",					// name 
	THIS_MODULE,				// this module
	tcp_vs_ssh_init_svc,			// initializer 
	tcp_vs_ssh_done_svc,			// done
	tcp_vs_ssh_update_svc,			// update
	tcp_vs_ssh_schedule,			// select a server by ssh request
	tcp_vs_ssh_server_to_client,
	tcp_vs_ssh_client_to_server,		// client to server
	NULL,					// checking permit
	NULL,					// release the application processing request
	tcp_vs_ssh_app_data_release		// release application private data
};

static __init int tcp_vs_ssh_init(void)
{
	INIT_LIST_HEAD(&tcp_vs_ssh_scheduler.n_list);
	return register_tcp_vs_scheduler(&tcp_vs_ssh_scheduler);
}

static __exit void tcp_vs_ssh_cleanup(void)
{
	unregister_tcp_vs_scheduler(&tcp_vs_ssh_scheduler);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pownall.Zhang");
module_init( tcp_vs_ssh_init );
module_exit( tcp_vs_ssh_cleanup );

