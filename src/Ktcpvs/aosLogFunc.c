///////////////////////////////////////////////////////////////////////
// Author: Zhang QuanLin
// Creation Time: 08/02/2006
// Description:
//
// Change History
// 08/02/2006 aosLogFunc.c created
///////////////////////////////////////////////////////////////////////
#include <linux/net.h>
#include <net/sock.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <asm/string.h>
#include <linux/delay.h>

#include "Ktcpvs/tcp_vs.h"
#include "Ktcpvs/aosLogFunc.h"
#include "Ktcpvs/aosKernelFile.h"


//extern struct timezone sys_tz;
//
// This function used to get current time
// The caller must allocate more than 20 bytes buffer space for this function.
//
//void inline aos_LogGetTimeStr(char *buf)
//void inline aos_LogGetTimeStr(char *buf)

void aos_GetCurrentTime( struct aos_tm *curtm )
{
	unsigned long days;
	int leap_y_count;
	int months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	unsigned long seconds = get_seconds()-AOS_TIMEZONE_SECS;

   	curtm->tm_sec  = seconds%60;	//get second
	seconds /= 60;
	curtm->tm_min  = seconds%60;  //get minute
	seconds /= 60;
	curtm->tm_hour = seconds%24;	//get hour +17 ????
	days     = seconds/24;	//get tatol days
	//how many leap year(4 years for one leap year)
	leap_y_count = (days+365)/1461;

	if( ((days+366)%1461) == 0 ) //the last of leap year
	{	
		curtm->tm_year = (days/366)+1970;//get year
		curtm->tm_mon  = 12;              //modfiy month
		curtm->tm_day  = 31;
		return;
	}	
	days -= leap_y_count;
	curtm->tm_year = (days/365)+1970;   //get year
	days %= 365;                //the day of this year
	days = 1 + days;
	if( (curtm->tm_year%4) == 0 )
	{
		if(days > 60)--days;    //modfiy leap year
		else {
			if(days == 60)
			{
				curtm->tm_mon = 2;
				curtm->tm_day = 29;
				return;
			}
		}
	}
	for( curtm->tm_mon = 0; months[ curtm->tm_mon ] < days; curtm->tm_mon++)
	{
		days -= months[ curtm->tm_mon ];
	}
	++curtm->tm_mon;              //get month
	curtm->tm_day = days;         //get day
	
}

void aos_LogGetTimeStr(char *buf)
{
	//static char buf[64];
	struct aos_tm curtm;
	
	aos_GetCurrentTime( &curtm );
	
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d ",\
			curtm.tm_year, curtm.tm_mon, curtm.tm_day,\
			curtm.tm_hour, curtm.tm_min, curtm.tm_sec );
	//return buf;
}



char *aos_LogChgTime2Str(u32 seconds)
{
	static char buf[64];
	struct aos_tm curtm;
	unsigned long days;
	int leap_y_count;
	int months[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
	
	seconds -= AOS_TIMEZONE_SECS;
   	curtm.tm_sec  = seconds%60;	//get second
	seconds /= 60;
	curtm.tm_min  = seconds%60;  //get minute
	seconds /= 60;
	curtm.tm_hour = seconds%24;	//get hour +17 ????
	days     = seconds/24;	//get tatol days
	//how many leap year(4 years for one leap year)
	leap_y_count = (days+365)/1461;

	if( ((days+366)%1461) == 0 ) //the last of leap year
	{	
		curtm.tm_year = (days/366)+1970;//get year
		curtm.tm_mon  = 12;              //modfiy month
		curtm.tm_day  = 31;
		goto date_end;
	}	
	days -= leap_y_count;
	curtm.tm_year = (days/365)+1970;   //get year
	days %= 365;                //the day of this year
	days = 1 + days;
	if( (curtm.tm_year%4) == 0 )
	{
		if(days > 60)--days;    //modfiy leap year
		else {
			if(days == 60)
			{
				curtm.tm_mon = 2;
				curtm.tm_day = 29;
				goto date_end;
			}
		}
	}
	for( curtm.tm_mon = 0; months[ curtm.tm_mon ] < days; curtm.tm_mon++)
	{
		days -= months[ curtm.tm_mon ];
	}
	++curtm.tm_mon;              //get month
	curtm.tm_day = days;         //get day
date_end:
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",\
			curtm.tm_year, curtm.tm_mon, curtm.tm_day,\
			curtm.tm_hour, curtm.tm_min, curtm.tm_sec );

	return buf;
}


//
// This function used to convert IP to string
// The caller must allocate more than 16 bytes buffer space for this function.
//

void aos_LogGetIPStr(struct sockaddr_in *addr, char *buf)
{
	struct sockaddr_in addr1;
	//static char buf[64];

	addr1.sin_addr.s_addr = ntohl(addr->sin_addr.s_addr);
	sprintf( buf, "%d.%d.%d.%d", \
		(unsigned char)((addr1.sin_addr.s_addr>>24)&0xFF),\
		(unsigned char)((addr1.sin_addr.s_addr>>16)&0xFF),\
		(unsigned char)((addr1.sin_addr.s_addr>> 8)&0xFF),\
		(unsigned char)((addr1.sin_addr.s_addr    )&0xFF));
		
	//return buf;
}

//
// This function used to local machine host name
// The caller must allocate more than 128 bytes buffer space for this function.

char *aos_LogGetSessionID( u64 sid )
{
	static char buf[96];

	sprintf( buf, "%08X%08X", (u32)(sid>>32), (u32)sid );
	
	return buf;
}


KFILE *aos_openvsfile( KFILE *filp, char *fname_fmt, u32 *day_secs )
{
	char		filename[512];
	struct aos_tm	curtm;
	
	if( filp != NULL )aos_fclose( filp );
	
	// build file name according fname_fmt and current date time.
	aos_GetCurrentTime( &curtm );
	sprintf( filename, fname_fmt, curtm.tm_year, curtm.tm_mon, curtm.tm_day );
	
	if (( filp = aos_fopen( filename, "a" )) == NULL )
	{ 
		AOS_DEBUG_PRINTK("%s : Aos_openvsfile \"%s\" error !\n",\
				aos_LogChgTime2Str(get_seconds()),filename );
	}
	else AOS_DEBUG_PRINTK("%s (%lu,%lu): Aos_openvsfile \"%s\" success !\n",\
				aos_LogChgTime2Str(get_seconds()),get_seconds(),\
				(day_secs!=NULL)?*day_secs:0,\
				filename );
	// mktime to seconds from 01/01/1970 00:00:00
	if( day_secs ) //7*3600=25200
	{
		if (0 >= (int) (curtm.tm_mon -= 2) ) 
		{	/* 1..12 -> 11,12,1..10 */
			curtm.tm_mon += 12;		/* Puts Feb last since it has leap day */
			curtm.tm_year -= 1;
		}
		*day_secs = (unsigned long) ((curtm.tm_year/4 - curtm.tm_year/100 +\
				curtm.tm_year/400 + 367*curtm.tm_mon/12 + curtm.tm_day)\
				+curtm.tm_year*365 - 719499) *86400+AOS_TIMEZONE_SECS; /* finally seconds */
		//*day_secs = mktime( curtm.tm_year, curtm.tm_mon, curtm.tm_day, 0, 0, 0 )+25200l;
	}
	return filp;
}

u64 create_session_id()
{
	u64 session_id = 0;
	int i = 0;

	for( i=31; i>=0; i++ )
		if( (current->tgid>>i) & 0x01 )break;
	session_id = (((0xFFFFFFFF>>i)<<16)^0xFFFFFFFF)&xtime.tv_nsec;
	session_id |= current->tgid<<16;
	session_id |= ((u64)xtime.tv_sec)<<32;
	
	return session_id;
}

KFILE *aos_write_log_file_header( void *Conn, char *fname_fmt )
{
	int	len;
	char	filename[512];
	struct aos_tm   curtm;
	BINFILEHEADER bin_fmt;
	struct sockaddr_in client_addr;
	struct tcp_vs_conn *conn = (struct tcp_vs_conn *)Conn;
	
	if( !conn->session_id )
	{
		conn->session_id = create_session_id();
		//if procotol is 'ftp' or 'telnet', cs_msg_to_daemon is setted in here 
		conn->cs_msg_to_daemon = HAC_DAEMON_CS_MSG_DEFAULT;
		//AOS_DEBUG_PRINTK("set  cs_msg_to_daemon to [%d]\n", conn->cs_msg_to_daemon );
	}

	if( conn->bin_file_ptr != NULL )
		return conn->bin_file_ptr;
	// build file name according fname_fmt and current date time.
	aos_GetCurrentTime( &curtm );
	sprintf( filename, fname_fmt, curtm.tm_year, curtm.tm_mon, curtm.tm_day,\
		aos_LogGetSessionID(conn->session_id) );
	
	if (( conn->bin_file_ptr= aos_fopen( filename, "w" )) == NULL )
	{ 
		AOS_DEBUG_PRINTK("%s : Open file \"%s\" error !\n",\
				aos_LogChgTime2Str(get_seconds()),filename );
		return NULL;
	}
#ifdef AOS_ZQL_SET_DEBUG	//just debug mode to use
	else AOS_DEBUG_PRINTK("%s : Open file \"%s\" success !\n",\
				aos_LogChgTime2Str(get_seconds()), filename );
#endif

	conn->csock->ops->getname(conn->csock,(struct sockaddr*)&client_addr,  &len,1);
	//conn->dsock->ops->getname(conn->dsock,(struct sockaddr*)&server_addr,&len,0);
	
	bin_fmt.sessionID 		= conn->session_id;
	bin_fmt.clientIP     		= ntohl(client_addr.sin_addr.s_addr);
	bin_fmt.destIP       		= conn->dest->addr;
	bin_fmt.hostnameLen  	= strlen( system_utsname.nodename );
	
	aos_fwrite( (char *)&bin_fmt, sizeof( BINFILEHEADER ), conn->bin_file_ptr );
	aos_fwrite( system_utsname.nodename, bin_fmt.hostnameLen, conn->bin_file_ptr );
	
	return conn->bin_file_ptr;
}

int aos_process_log_data( char *app_data, unsigned int app_data_len, void *Conn, u8 direct )
{
	BINDATAHEADER	logHeader;
	struct tcp_vs_conn *conn = (struct tcp_vs_conn *)Conn;
	KFILE	*binFp = conn->bin_file_ptr;

	if( binFp == NULL )return -1;
	
	logHeader.direct = direct;
	logHeader.timeSec = xtime.tv_sec-AOS_TIMEZONE_SECS;
	logHeader.timeMSec = xtime.tv_nsec&0xffffffff;
	logHeader.dataLen = app_data_len;

	aos_fwrite( (char *)&logHeader, sizeof(BINDATAHEADER), binFp );
	aos_fwrite( app_data, app_data_len, binFp );

	if( conn->tcp_vs_status == TCP_VS_NORMAL ||\
	     conn->tcp_vs_status == TCP_VS_LOG )
		return aos_send_log_to_daemon( app_data, app_data_len, Conn, direct);
	return 1;
}

int aos_send_log_to_daemon(  char *app_data, unsigned int app_data_len, void *Conn, u8 direct )
{
	struct tcp_vs_conn *conn = (struct tcp_vs_conn *)Conn;
	char *log_data = app_data;
	u32	log_data_len = app_data_len;

	//aos_file_printk("/usr/local/keyou/Log/syslog.err","cs_msg_to_daemon = %d\n", conn->cs_msg_to_daemon );
	//some proxy server don't need send msg to daemon
	//like normal tcp proxy server
	if( !conn->cs_msg_to_daemon &&\
	     !conn->sc_msg_to_daemon )return 0;
	
	if( !conn->daemon_sock )
	{
		tcp_vs_dest_t  dest;

		dest.addr= htonl(INADDR_LOOPBACK);
		dest.port = htons(DAEMON_LISTEN_PORT);
		conn->daemon_sock = tcp_vs_connect2dest( &dest );
		if( !conn->daemon_sock )
		{
			aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr,  "Error : Connect to the hac Daemon failure!\n");
			return -1;
		}
		//add send session_id and procotol style msg to daemon, cod...
		{
			char msg[128];
			int msgLen = 0;

			msgLen = snprintf( msg, 128, "%s %s ", aos_LogGetSessionID(conn->session_id),\
				conn->svc->scheduler->name );

			//the first msg to daemon, for get session id and procotol name
			//format : "id name"
			if( tcp_vs_sendbuffer( conn->daemon_sock, msg, msgLen, 0 ) < msgLen )
			{
				aos_alarm(eAosMD_TcpProxy, eAosAlarm_ProgErr,  "Error : Send init data to the hac Daemon failure!\n");
				return -1;
			}
			//mdelay( 500 ); //about 15 msec
			//return 1;
		}
		
	}

	switch( direct )
	{
		case CLIENTTOSERVER:
			if( !conn->cs_msg_to_daemon )return 0;
			{
				char log_prefix[]={0xaf, 0xbf, 0xcf, 0xdf, 0xef};
				log_data_len = sizeof(log_prefix)/sizeof(char);
				log_data = aos_malloc( app_data_len+log_data_len );
				if( log_data == NULL ) //malloc memory failure, stored error info
				{
					char *err_info = "Error : Malloc memory failure!\n";
					AOS_DEBUG_PRINTK(err_info );
					return -1;
				}
				memcpy( log_data, log_prefix, log_data_len);
				memcpy( &log_data[log_data_len], app_data, app_data_len );
				log_data_len += app_data_len;
			}
			break;
		case SERVERTOCLIENT:
			if( !conn->sc_msg_to_daemon )return 0;
			break;
		default:
			return 0;
	}
	
	/*
	aos_file_printk("/usr/local/keyou/Log/syslog.err","{%s} send [direct:%d]-%d bytes log to daemon!\n",\
		conn->svc->scheduler->name,direct, log_data_len );
	*/

	if( tcp_vs_sendbuffer( conn->daemon_sock, log_data, log_data_len, 0 )\
		< app_data_len ){
		char *err_info = "Error : Send data to the hac Daemon failure!\n";
		AOS_DEBUG_PRINTK(err_info );
		if( log_data_len != app_data_len ) aos_free( log_data );
		return -1;
	}
#ifdef AOS_ZQL_SET_DEBUG	//just debug mode to use
	else AOS_DEBUG_PRINTK( "{%s} send [direct:%d]-%d bytes log to daemon!\n", conn->svc->scheduler->name,direct, log_data_len );
#endif
	if( log_data_len != app_data_len ) 
		aos_free( log_data );
	return 1;
}

void aos_close_daemon_sock( void *Conn )
{
	struct tcp_vs_conn *conn = (struct tcp_vs_conn *)Conn;
	if( conn->daemon_sock )
		sock_release( conn->daemon_sock );
}
