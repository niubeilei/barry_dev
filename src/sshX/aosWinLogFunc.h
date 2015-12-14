///////////////////////////////////////////////////////////////////////
// Author: Pownall.Zhang
// Creation Time: 10/27/2006
// Description:
// * File only for windows sshX program test using.
//
// Change History
// 10/27/2006 aosWinLogFunc.h created
///////////////////////////////////////////////////////////////////////
#ifndef AOS_WIN_LOG_FUNC_H
#define AOS_WIN_LOG_FUNC_H

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

#ifdef _WIN32
#else
#include <stdint.h>
#endif

#define LINEMAXLENTH		1024

#define KFILE FILE

//Which direct packet from net ,Which telnet write packet to binary file
#define CLIENTTOSERVER		0
#define SERVERTOCLIENT		1

#define AOS_DAY_SECONDS		86400l
#define SSH_BIN_LOGNAME		"log\\ssh.%Y%m%d.bin.log"

typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned long		u32;

#ifdef _WIN32
typedef __int64				u64;
#else
typedef uint64_t			u64;
#endif

#define SSH_CLIENT_ID		0x305E4E1B

typedef struct BinDataHeader
{
	u32	direct;		// CLIENTTOSERVER 0 : client to server
					// SERVERTOCLIENT 1 : server to client
	u32	timeSec;	// total seconds from 1970.1.1
	u64	sessionId;	// tcp_vs_telnet thread pid
	u32	clientIP;	// client ip address
	u32	destIP;		// server ip address
	u32	hostnameLen;	// host name length	
	u32	dataLen;	// packet data length
}BINDATAHEADER;


extern u64 get_seconds();
extern void  write_ssh_bin_data(u8 *app_data, u32 app_data_len, u8 direct );
extern KFILE *aos_openvsfile( KFILE *filp, char *fname_fmt );

#endif
