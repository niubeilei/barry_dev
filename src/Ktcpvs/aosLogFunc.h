///////////////////////////////////////////////////////////////////////
// Author: Zhang QuanLin
// Creation Time: 07/28/2006
// Description:
//
// Change History
// 08/02/2006 aosLogFunc.h created
///////////////////////////////////////////////////////////////////////
#ifndef aos_log_func_h
#define aos_log_func_h

#include <linux/time.h>
#include <linux/sched.h>
#include <linux/in.h>
#include <linux/types.h>
#include <linux/utsname.h>

#include "Ktcpvs/aosKernelFile.h"

//Which direct packet from net ,Which telnet write packet to binary file
#define CLIENTTOSERVER		0
#define SERVERTOCLIENT		1

#define AOS_DAY_SECONDS		86400l
#define AOS_LOG_BUF_SIZE			256
#define DAEMON_LISTEN_PORT		1818

#define AOS_TIMEZONE_SECS	sys_tz.tz_minuteswest * 60
extern struct timezone sys_tz;

//aos time struct
struct aos_tm
{  
    int tm_sec;  
    int tm_min;  
    int tm_hour; 
    int tm_day;  
    int tm_mon;  
    int tm_year;  
};

#define	HAC_DAEMON_CS_MSG_DEFAULT	0x01

//#define AOS_DEBUG_PRINTK(format,x...)	aos_trace(format, ##x)

#ifndef AOS_DEBUG_PRINTK
#define AOS_ZQL_SET_DEBUG
#define AOS_ERROR_LOGNAME	"/usr/local/keyou/Log/syslog.err"
#define AOS_DEBUG_PRINTK(format, x...) 								\
	aos_file_printk(AOS_ERROR_LOGNAME,"%s[%d] : ", __FILE__, __LINE__ );	\
	aos_file_printk(AOS_ERROR_LOGNAME, format, ##x)
#endif

/*
 *	The telnet_key_buf object
 */
 
typedef struct BinFileHeader
{
	u64	sessionID;	// tcp_vs_telnet thread pid
	u32  startTime;	// Session start time
	u32	clientIP;		// client ip address
	u32	destIP;			// server ip address
	u32	hostnameLen;	// host name length	
}BINFILEHEADER;

typedef struct BinDataHeader
{
	u32	direct;		// CLIENTTOSERVER 0 : client to server
					// SERVERTOCLIENT 1 : server to client
	u32	timeSec;	// total seconds from 1970.1.1
	u32	timeMSec;	// differ time	
	u32	dataLen;	// packet data length
}BINDATAHEADER; 

typedef struct v_char
{
	char 	*buf;
	u16		len;
}VCHAR, *LPVCHAR;

#define VCHAR_MALLOC(vchar, nlen) \
	vchar.buf=(char *)aos_malloc(nlen);\
	vchar.len=0;

#define VCHAR_FREE(vchar) \
	if( vchar.buf ){\
		aos_free(vchar.buf);\
		vchar.buf=NULL;\
		vchar.len=0;\
	}

//get host name
#define aos_LogGetHostName() system_utsname.nodename

extern void aos_GetCurrentTime(struct aos_tm *curtm );
extern void aos_LogGetIPStr(struct sockaddr_in *addr, char *buf);
extern char *aos_LogGetSessionID( u64 sid );
extern void aos_LogGetTimeStr(char *buf);
extern char *aos_LogChgTime2Str(u32 seconds);
extern struct file *aos_openvsfile( struct file *filp, char *fname_fmt, u32 *day_secs );
extern KFILE *aos_write_log_file_header( void *Conn, char *fname_fmt );
extern int aos_process_log_data( char *app_data, unsigned int app_data_len, void *Conn, u8 direct );
extern u64 create_session_id(void);
extern int aos_send_log_to_daemon(  char *app_data, unsigned int app_data_len, void *Conn, u8 direct );
extern void aos_close_daemon_sock( void *Conn );

/*
 *  * zlib compression.
 *   */
extern void *zlib_compress_init(void);
extern void zlib_compress_cleanup(void *);
extern void *zlib_decompress_init(void);
extern void zlib_decompress_cleanup(void *);
extern int zlib_compress_block(void *, unsigned char *block, int len,
					unsigned char **outblock, int *outlen);
extern int zlib_decompress_block(void *, unsigned char *block, int len,
					  unsigned char **outblock, int *outlen);

#endif
