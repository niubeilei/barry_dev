/////////////////////////////////////////////////////
// draft by pownall.zhang
// modified by Ricky Chen
// 2007/02/07
//
//////////////////////////////////////////////////////
// ReadHacLog.cpp : Defines the entry point for the console application.
//
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <string.h>

#ifdef _WIN32
#else
#include <stdint.h>
#endif

//Which direct packet from net ,Which telnet write packet to binary file
#define CLIENTTOSERVER		0
#define SERVERTOCLIENT		1

#define READ_FTP_FILE		0
#define READ_TELNET_FILE	1
#define READ_SSH_FILE		2

typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned long	u32;

#ifdef _WIN32
typedef __int64			u64;
#else
typedef uint64_t		u64;
#endif

typedef struct BinFileHeader
{
	u64	sessionID;	// tcp_vs_telnet thread pid
	u32  startTime;	// Session start time
	u32	clientIP;		// client ip address
	u32	destIP;			// server ip address
	//u32	protocolLen;	// host name length	
	//u32	hostnameLen;	// host name length	
	u32	extLen;	// ext length	
}BINFILEHEADER;

typedef struct BinDataHeader
{
	u32	direct;		// CLIENTTOSERVER 0 : client to server
	// SERVERTOCLIENT 1 : server to client
	u32	timeSec;	// total seconds from 1970.1.1
	u32	timeMSec;	// differ time	
	u32	dataLen;	// packet data length
}BINDATAHEADER;

FILE *m_logFp=NULL;

char m_logSysname[256];
BINFILEHEADER	m_logHeader;

char *aos_LogGetTimeStr(u32 secs)
{
	static char buf[64];
	time_t ltime = (time_t)secs;
	struct tm *today;

	today = gmtime( &ltime );
	//sprintf(buf,"%s", asctime( today ) );
	sprintf(buf,"%d-%02d-%02d %02d:%02d:%02d",\
			today->tm_year+1900, today->tm_mon+1, today->tm_mday,\
			today->tm_hour, today->tm_min, today->tm_sec );
	return buf;
}

char *aos_LogGetIPStr( u32 addr, char *buf )
{
	sprintf( buf, "%d.%d.%d.%d", \
			(unsigned char)((addr>>24)&0xFF),\
			(unsigned char)((addr>>16)&0xFF),\
			(unsigned char)((addr>> 8)&0xFF),\
			(unsigned char)((addr    )&0xFF));
	return buf;
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

int tmpLen = 0;

int aos_read_log_file_header( char *fname )
{
	u32	rlen;
	u32 protocolLen;

	if( m_logFp )
		fclose( m_logFp );

	m_logFp = fopen( fname, "rb" );

	if( m_logFp == NULL )return 0;

	rlen = fread( &m_logHeader, 1, sizeof( BINFILEHEADER ), m_logFp );
	if( rlen != sizeof( BINFILEHEADER ) )return 0;

	if( strstr( fname, ".hlog" ) )
	    for( tmpLen =0;fname[tmpLen];tmpLen++)if( fname[tmpLen]=='.' )break;
	protocolLen = (m_logHeader.extLen>>16)&0xff;

	rlen = fread( m_logSysname, 1, protocolLen, m_logFp );
	m_logSysname[ protocolLen ] = '\0';
	printf("Protocol[%d] : %s\n",protocolLen, m_logSysname);
	
	m_logHeader.extLen &= 0xff;
	m_logHeader.extLen += tmpLen;

	rlen = fread( m_logSysname, 1, m_logHeader.extLen, m_logFp );
	if( rlen != m_logHeader.extLen )return 0;
	m_logSysname[ m_logHeader.extLen ] = '\0';
	printf("Hostname[%d] : %s\n",m_logHeader.extLen, m_logSysname);

	
	return 1;
}

int aos_process_log_data( BINDATAHEADER *dataHeader, char *app_data )
{
	u32	rlen;
	if( m_logFp == NULL )return -1;

	rlen = fread( dataHeader, 1, sizeof(BINDATAHEADER), m_logFp );
	if( rlen != sizeof( BINDATAHEADER ) )return -1;
	rlen = fread( app_data, 1, dataHeader->dataLen, m_logFp );
	if( rlen != dataHeader->dataLen )return -1;

	app_data[ dataHeader->dataLen ] = '\0';

	return rlen;
}

int main(int argc, char* argv[])
{
	BINDATAHEADER dataHeader;
	char logData[65536];
	char *cmd1[2]={"-CMD","-RSP"};
	char clientIPBuf[64], serverIPBuf[64];
	int	 FindNum=0;
	char *strpoint=NULL	;	
	int index=1;
	if (argc<2)
	{   printf("miss parameter,please input more parameters\n ");
		printf("\n");
		printf("usage: ./readhaclog -l [logfile_path] -s [string_find]\n ");
		printf("\n");
		printf("example: ./readhaclog -l ftp.123456789.log -s 'findstring'\n ");
		printf("\n");
		printf("help: ./readhaclog -h\n");
		exit(0);
	}
	while (index < argc)
	{  
		if (strcmp(argv[index], "-l") == 0)
		{   
			if(!aos_read_log_file_header( argv[index+1] )) 
			{
				printf("Open file \'%s\' to read error !\n",argv[index+1]);
				exit(0);
			}
			else 
			{
				index += 2;
				continue;
			}
		}
		if (strcmp(argv[index], "-s") == 0)
		{  
			strpoint=argv[index+1];
			printf("\n");
			index += 2;
			continue;
		}
		if (strcmp(argv[index], "-h") == 0)
		{
			printf("\n");
			printf("usage: ./readhaclog -l [logfile_path] -s [string_find]\n ");
			printf("\n");
			printf("example: ./readhaclog -l ftp.123456789.log -s 'findstring'\n ");
			printf("\n");
			printf("help: ./readhaclog -h\n");
			exit(0);
		}
		index++;
	}

	aos_LogGetIPStr( m_logHeader.clientIP, clientIPBuf );
	aos_LogGetIPStr( m_logHeader.destIP, serverIPBuf );
	while( aos_process_log_data( &dataHeader, logData ) != -1 )
	{
		if( dataHeader.direct == CLIENTTOSERVER )
		{
			if( strstr( logData, strpoint ) != NULL )
			{
				FindNum++;
			}
		}
	}

	fclose( m_logFp );
	printf("the appearance counts of the searched string is: %d\n",FindNum);
	return 0;
}
