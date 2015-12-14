////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProxyThread.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "..\Common\common.h"
#include ".\ProxyThread.h"
#include ".\AppProxy.h"
#include ".\ProxyServer.h"
#include <process.h>
#include "..\Util\AosWinModules.h"
#include "..\Util\AosWinAlarm.h"
#include "..\Util\AosWinLog.h"
#include "..\Util\WinReturnCode.h"
#include ".\FtpAppProxy.h"
#include "SSLProxyAppDlg.h"

#define TIME_INTERVAL_SECOND_LISTEN 1	// 1 second
#define TIME_INTERVAL_USECOND_READ 500	// 0.5 second

VOID ProxyServerThread(PVOID lpParameter)
{
	CProxyServer *pProxyServer = (CProxyServer*)lpParameter;
	SOCKET server;
	struct sockaddr_in sin;
	struct fd_set fdAccept;
	DWORD dwIPAddr;
	struct timeval waitTime;
	ULONG uRet;
	int iRet;

	sin.sin_family = AF_INET;
	sin.sin_port = ntohs( pProxyServer->m_usLocalPort );
	dwIPAddr = pProxyServer->m_dwLocalIP;

	memcpy(&sin.sin_addr, &dwIPAddr, 4);

	server = socket(AF_INET, SOCK_STREAM, 0);
	if(!server)
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
			( "socket create failed, %d.%d.%d.%d:%d",NIPQUAD(dwIPAddr), pProxyServer->m_usLocalPort ) );
		PutProxyServer ( pProxyServer );
		return;
	}

	if(SOCKET_ERROR == bind(server, (struct sockaddr *)&sin, sizeof(sin)))
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
			( "failed to bind,  %d.%d.%d.%d:%d",NIPQUAD(dwIPAddr), pProxyServer->m_usLocalPort ) );
		PutProxyServer ( pProxyServer );
		return;
	}

	if(SOCKET_ERROR == listen(server,5))
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
			( "failed to listen,  %d.%d.%d.%d:%d",NIPQUAD(dwIPAddr), pProxyServer->m_usLocalPort ) );
		PutProxyServer ( pProxyServer );
		return;
	}

	AosWinMinLog(eAosWinMD_TcpProxy, ("Service '%s' listening on %d.%d.%d.%d:%d", 
		pProxyServer->m_sServiceName, NIPQUAD(dwIPAddr), pProxyServer->m_usLocalPort) );
	
	DlgLogInfo( "%s start", pProxyServer->m_sServiceName );

	waitTime.tv_sec = TIME_INTERVAL_SECOND_LISTEN;
	waitTime.tv_usec = 0;

	while( pProxyServer->m_bStart )
	{
		FD_ZERO(&fdAccept);
		FD_SET(server, &fdAccept);
		iRet = select(0, &fdAccept, NULL, NULL, &waitTime);
		if ( iRet == 0 )
		{
			// time interval
			continue;
		}
		if(SOCKET_ERROR == iRet )
		{
			break;
		}
		if(FD_ISSET(server, &fdAccept))
		{
			SOCKET ConnectedSocket = NULL;
			if(!(ConnectedSocket = accept(server, NULL, NULL)))
			{
				return;
			}
			CAppProxy *pAppProxy;
			pAppProxy = pProxyServer->GetAppProxy(pProxyServer->m_eAppType);
			if(!pAppProxy)
			{
				AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ( "failed to get app proxy") );
				return;
			}
            pAppProxy->m_bSSLFlagBack = pProxyServer->m_bSSLFlagBack; 
			pAppProxy->m_socketFront = ConnectedSocket;
			pAppProxy->m_dwRealIPAddr = pProxyServer->m_dwRealIP;
			pAppProxy->m_usRealPort = pProxyServer->m_usRealPort;
			pAppProxy->m_pProxyServer = pProxyServer;

			HoldProxyServer ( pProxyServer );

			uRet = (ULONG)_beginthread(ProxyThreadProc, 0, (PVOID)pAppProxy);
			if ( uRet == -1 || uRet == 0 )
			{
				AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("start thread failed") );
				PutProxyServer( pProxyServer );
			}

			if ( pProxyServer->m_eAppType == FTP_DATA_APP ) 
			{
				break;
			}
		}
	}

	closesocket( server );
	pProxyServer->m_bStopped = TRUE;

	//pProxyServer->m_bStart = FALSE;
	if ( pProxyServer->m_eAppType != FTP_DATA_APP )
	{
		CHAR sInfo[256];
		_snprintf( sInfo, sizeof(sInfo),"'%s' %d.%d.%d.%d:%d is stopped", 
			pProxyServer->m_sServiceName, NIPQUAD(dwIPAddr), pProxyServer->m_usLocalPort );
		AosWinMinLog( eAosWinMD_TcpProxy, ("%s", sInfo) ); 
		DlgLogInfo( sInfo );
	}
	PutProxyServer ( pProxyServer );
}

VOID ProxyThreadProc(PVOID lpParameter)
{
	CAppProxy *pAppProxy = (CAppProxy*)lpParameter;
	BOOL bBackendConnected = FALSE;
	CHAR cRcvBuf[MAX_BUFFER_SIZE-1]; 
	int iRcvBufLen = MAX_BUFFER_SIZE-1, iRcvdLen;
	struct fd_set fd_read;
	struct timeval waitTime;

	if(!pAppProxy)
	{
		return;
	}

	if ( pAppProxy->m_dwRealIPAddr )
	{
		if ( !pAppProxy->ConnectToBackend(pAppProxy->m_dwRealIPAddr, pAppProxy->m_usRealPort) )
		{
			AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr,
				("failed to connect to %s:%d", inet_ltoa(pAppProxy->m_dwRealIPAddr), pAppProxy->m_usRealPort) );
			DlgLogInfo( "failed to connect to %s:%d", inet_ltoa(pAppProxy->m_dwRealIPAddr), pAppProxy->m_usRealPort );
			goto CLEAN_UP;
		}
		AosWinTrace( ("succeed to connect to %s:%d", inet_ltoa(pAppProxy->m_dwRealIPAddr), pAppProxy->m_usRealPort) );

	}

	waitTime.tv_sec = TIME_INTERVAL_USECOND_READ;
	waitTime.tv_usec = 0;

	while( pAppProxy->m_pProxyServer->m_bStart )
	{
		FD_ZERO(&fd_read);
		FD_SET(pAppProxy->m_socketFront, &fd_read);
		if(pAppProxy->m_socketBack)
		{
			FD_SET(pAppProxy->m_socketBack, &fd_read);
		}

		if( SOCKET_ERROR == select(0, &fd_read, NULL, NULL, &waitTime))
		{
			goto CLEAN_UP;
		}
		if(FD_ISSET(pAppProxy->m_socketFront, &fd_read))
		{
			if(0 >= pAppProxy->RcvFromClient(cRcvBuf, iRcvBufLen, iRcvdLen))
			{
				goto CLEAN_UP;
			}
			if(0 > pAppProxy->SndToServer(cRcvBuf, iRcvdLen))
			{
				goto CLEAN_UP;
			}
		}
		if(FD_ISSET(pAppProxy->m_socketBack, &fd_read))
		{
			if(0 >= pAppProxy->RcvFromServer(cRcvBuf, iRcvBufLen, iRcvdLen))
			{
				goto CLEAN_UP;
			}
			if(0 > pAppProxy->SndToClient(cRcvBuf, iRcvdLen))
			{
				goto CLEAN_UP;
			}
		}
		
	}

CLEAN_UP:
	if(pAppProxy)
	{
		PutProxyServer ( pAppProxy->m_pProxyServer );
		pAppProxy->Release();
		delete pAppProxy;
	}
	return;
}

