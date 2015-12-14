////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FtpAppProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\ftpappproxy.h"
#include "..\Util\AosWinModules.h"
#include "..\Util\AosWinLog.h"
#include ".\ProxyServer.h"

CHAR *gsCmd_list[]=
{
	{"ABOR"},
	{"ACCT"},
	{"APPE"},
	{"ALLO"},
	{"AUTH"},
	{"CWD"},
	{"CDUP"},
	{"DELE"},
	{"EPRT"},
	{"EPSV"},
	{"FEAT"},
	{"HELP"},
	{"LIST"},
	{"MDTM"},
	{"MKD"},
	{"MODE"},
	{"MLFL"},
	{"MAIL"},
	{"MSND"},
	{"MSOM"},
	{"MSAM"},
	{"MRSQ"},
	{"MRCP"},
	{"NLST"},
	{"NOOP"},
	{"OPTS"},
	{"PASV"},
	{"PORT"},	
	{"PWD"},
	{"PASS"},
	{"QUIT"},
	{"REIN"},
	{"RETR"},
	{"RMD"},
	{"REST"},
	{"RNFR"},
	{"RNTO"},
	{"SMNT"},
	{"STRU"},
	{"SITE"},
	{"SYST"},
	{"STAT"},
	{"STOR"},
	{"SIZE"},
	{"STOU"},
	{"TYPE"},
	{"USER"},
	{"XCUP"},
	{"XCWD"},
	{"XMKD"},
	{"XPWD"},
	{"XRMD"},
	{0}
};

CHAR *gsRespList[]=
{
	{"227"},	// Entering Passive Mode
	{0}
};

CFtpAppProxy::CFtpAppProxy(void)
{
}

CFtpAppProxy::~CFtpAppProxy(void)
{
}

int CFtpAppProxy::RcvFromClient(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen)
{
	CHAR sCmd[5];
	BOOL bCmdFind = FALSE;
	int i;
	int ret;

	ret = CAppProxy::RcvFromClient( cRcvBuf, iRcvBufLen, iRcvdLen );
	if ( ret < 0 )
	{
		AosWinMinLog( eAosWinMD_TcpProxy, ("ftp proxy rcv from client failed, %d", ret ) );
		return ret;
	}
	if ( ret == 0 )
	{
		return ret;
	}

	AosWinAssert1( iRcvdLen >= 3 );
	memcpy( sCmd, cRcvBuf, 4);
	sCmd[4]=0;
	if( sCmd[3]==10 || sCmd[3]==13 || sCmd[3]==0x20 ) sCmd[3]=0;

	i=0;
	do
	{
		if( _stricmp( gsCmd_list[i],sCmd ) == 0 ) 
		{
			bCmdFind = TRUE;
			break;
		}
		i++;
	}
	while( gsCmd_list[i] );

	if ( !bCmdFind )
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr,
			("unsupported ftp command %s", sCmd) );
		iRcvdLen = 0;
		return -1;
	}

	AosWinDebug( ("%s", sCmd) );

	if ( _stricmp( "PASV", sCmd ) == 0 )
	{
		return FtpPasvCmd( cRcvBuf, iRcvdLen );
	}
	else if ( _stricmp( "ABOR", sCmd ) == 0 )
	{
		return FtpAborCmd( cRcvBuf, iRcvdLen );
	}
	else if ( _stricmp( "PORT", sCmd ) == 0 )
	{
		return FtpPortCmd( cRcvBuf, iRcvdLen );
	}
	return ret;
}

int CFtpAppProxy::FtpPasvCmd(CHAR* sRcvBuf, int& iRcvLen)
{
	//m_fFtp |= CFtpAppProxy::FTP_FLAG_PASV_RCV;
	return iRcvLen;
}

int CFtpAppProxy::FtpPortCmd(CHAR* sRcvBuf, int& iRcvLen)
{
	return 0;
}

int CFtpAppProxy::FtpAborCmd(CHAR* sRcvBuf, int& iRcvLen)
{
	return iRcvLen;
}


int CFtpAppProxy::SndToClient(CHAR* cSndBuf, int iSndBufLen)
{
	AosWinAssert1( iSndBufLen >= 3 );
	cSndBuf[iSndBufLen] = 0;
	AosWinTrace(("%s", cSndBuf) );
	if ( strnicmp( cSndBuf, "227", 3 ) == 0 )
	{
		return FtpPasvResp(cSndBuf, iSndBufLen);
	}
	
	return CAppProxy::SndToClient( cSndBuf, iSndBufLen );
}

int CFtpAppProxy::FtpPasvResp(CHAR* cSndBuf, int iSndBufLen)
{
	struct sockaddr_in sin;
    DWORD dwLocalDataIP, dwRealDataIP;
	USHORT usLocalDataPort,usRealDataPort;
	UCHAR sAddr[6];
	int iSendBytes;
	int iNameLen;

	AosWinAssert1( iSndBufLen > 27 );

	FtpParseAddress(  &cSndBuf[27], dwRealDataIP, usRealDataPort );
	FindLocalUnusedPort( usLocalDataPort );

	iNameLen = sizeof(sin);
	if ( SOCKET_ERROR == getsockname( m_socketFront, (sockaddr *)&sin, &iNameLen ) )
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
			("Error to get local address, %d", WSAGetLastError() ) );
		return -1;
	}	

	memcpy( &dwLocalDataIP, &sin.sin_addr, 4 );
	AddressToString( htonl(dwLocalDataIP), usLocalDataPort, sAddr );

	// spawn data proxy server
	CProxyServer *pProxyServer = new CProxyServer( FTP_DATA_APP, inet_ntoa(sin.sin_addr), usLocalDataPort, 0, 0 );
	pProxyServer->m_iRefCounts = 0;
	memcpy( &sin.sin_addr, &dwRealDataIP, 4 );
	_snprintf( pProxyServer->m_sServiceName, sizeof(pProxyServer->m_sServiceName), "ftp data" );
	pProxyServer->SetRealServer( inet_ntoa(sin.sin_addr), usRealDataPort );
	AosWinTrace( ("Real ftp data server %s:%d", inet_ntoa(sin.sin_addr), usRealDataPort) );
	pProxyServer->Start();

	sprintf( cSndBuf, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n",
		sAddr[0], sAddr[1], sAddr[2], sAddr[3], sAddr[4], sAddr[5] );
	iSendBytes = (int)strlen( cSndBuf );
	AosWinMinLog( eAosWinMD_TcpProxy, ("rewrite PASV Resp: '%s'",cSndBuf ) );

	return CAppProxy::SndToClient( cSndBuf, iSendBytes );
}

BOOL CFtpAppProxy::FtpParseAddress(CHAR* sBuf, DWORD& dwIP, USHORT& usPort)
{
	CHAR* sTmp;
	UCHAR cAddr[6];
	int	i;

	for( i = 0; i < 6; i++ )
	{
		cAddr[i] = (CHAR)strtol( sBuf, &sTmp, 0 );
		sBuf = sTmp + 1;
	}

	dwIP = htonl(cAddr[3] + (cAddr[2] << 8) + (cAddr[1] << 16) + (cAddr[0] << 24));
	usPort = ((cAddr[4] << 8) + cAddr[5]);
	return TRUE;
}

BOOL CFtpAppProxy::FindLocalUnusedPort(USHORT& usPort)
{
	struct sockaddr_in sin;
	SOCKET sock;
	USHORT usPortTmp;
	int	i;

	usPort = 0;
	memset( &sin, 0, sizeof(sin) );
	sin.sin_family = AF_INET;

	sock = socket(PF_INET,SOCK_STREAM, 0);
	if ( sock == INVALID_SOCKET )
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("FindLocalUnusedPort:Error during creation of socket") );
		return FALSE;
	}

	srand( (unsigned)time( NULL ) );
	for( i = 0; i < 50000; i++ )
	{
		usPortTmp = (SHORT)(10000 + rand()*50000/RAND_MAX+GetCurrentThreadId());
		if ( usPortTmp > 65535 ) continue;
		sin.sin_port = htons(usPortTmp);
		if ( SOCKET_ERROR != bind( sock, (struct sockaddr*)&sin, sizeof(sin) ) )
		{
			usPort = usPortTmp;
			closesocket( sock );
			return TRUE;
		}
		if( WSAGetLastError() != WSAEADDRINUSE )
		{
			AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("error to find unused local port"));
			break;
		}

	}
	return FALSE;
}

VOID CFtpAppProxy::AddressToString(DWORD dwIP, USHORT usPort, UCHAR a[])
{
	a[0] = (UCHAR)((dwIP & 0xFF000000) >> 24);
	a[1] = (UCHAR)((dwIP & 0x00FF0000) >> 16);
	a[2] = (UCHAR)((dwIP & 0x0000FF00) >> 8);
	a[3] = (UCHAR)(dwIP & 0x000000FF);
	a[4] = (UCHAR)((usPort & 0xFF00) >> 8);
	a[5] = (UCHAR)((usPort & 0x00FF));
}

int CFtpAppProxy::RcvFromServer(CHAR* cRcvBuf, int iRcvBufLen , int& iRcvdLen)
{
	return CAppProxy::RcvFromServer( cRcvBuf, iRcvBufLen, iRcvdLen );
}