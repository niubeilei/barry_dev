////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProxyServer.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include <process.h>
#include <Winsock2.h>

#include ".\ProxyServer.h"
#include ".\AppProxy.h"
#include ".\ProxyThread.h"
#include ".\HttpAppProxy.h"
#include ".\FtpAppProxy.h"

CProxyServer::CProxyServer(void)
{
	InitParam();
}

CProxyServer::CProxyServer(EAPP_TYPE eAppType, LPCTSTR sLocalIP, USHORT usLocalPort, BYTE bSSLFlagFront, BYTE bSSLFlagBack )
: m_bStopped(FALSE)
{
	InitParam();
	if( sLocalIP )
	{
		if ( !(m_dwLocalIP = inet_addr( sLocalIP )) )
		{
			AosWinAlarm(eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
				("failed to get ip address of %s", sLocalIP) );
		}
	}
	else
	{
		m_dwLocalIP = 0;
	}

	m_usLocalPort = usLocalPort;
	m_eAppType = eAppType;
	m_bSSLFlagFront = bSSLFlagFront;
	m_bSSLFlagBack = bSSLFlagBack;
}

CProxyServer::CProxyServer(EAPP_TYPE eAppType, DWORD dwLocalIP, USHORT usLocalPort, BYTE  bSSLFlagFront, BYTE bSSLFlagBack)
{
	InitParam();
	m_dwRealIP = dwLocalIP;
	m_usLocalPort = usLocalPort;
	m_eAppType = eAppType;
	m_bSSLFlagFront = bSSLFlagFront;
	m_bSSLFlagBack = bSSLFlagBack;
}

CProxyServer::~CProxyServer(void)
{
}

BOOL CProxyServer::Start(void)
{
	ULONG uRet;
	if ( !m_bStopped ) return TRUE;
	//IncRefCount();
	HoldProxyServer ( this );
	uRet = (ULONG)_beginthread(ProxyServerThread, 0, (PVOID)this);
	if ( uRet != -1 && uRet != 0 )
	{
		m_bStart = TRUE;
		m_bStopped = FALSE;
	}
	else
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("start proxy server thread failed") );
		PutProxyServer ( this );
	}
	return m_bStart;
}

BOOL CProxyServer::Stop(void)
{
    m_bStart = FALSE;
	while( !IsStopped() ) Sleep(50);
	return TRUE;
}

BOOL CProxyServer::IsStopped(void)
{
	return m_bStopped;
}

CAppProxy* CProxyServer::GetAppProxy(EAPP_TYPE eAppType)
{
	CAppProxy *pAppProxy = NULL;
	switch(eAppType)
	{
	case TCP_APP:
	case FTP_DATA_APP:
		pAppProxy = new CAppProxy;
		break;
	case HTTP_APP:
		pAppProxy = new CHttpAppProxy;
		break;
	case FTP_APP:
		pAppProxy = new CFtpAppProxy;
		break;
	default:
		AosWinDebug( ("unsupport app type %d", eAppType) );
		break;
	}
	return pAppProxy;
}

VOID CProxyServer::SetRealServer(LPTSTR sRealIP , USHORT usRealPort)
{	

	if ( !(m_dwRealIP = inet_addr( sRealIP )) )
	{
		AosWinAlarm(eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, 
			("failed to get ip address of %s", sRealIP) );
		return;
	}
	m_usRealPort = usRealPort;
}

VOID CProxyServer::SetRealServer(DWORD dwRealIP, USHORT usLocalPort)
{	

	m_dwRealIP = dwRealIP;
	m_usRealPort = usLocalPort;
}

VOID CProxyServer::Release(void)
{
	return ;
}

VOID CProxyServer::CopyConfig(CProxyServer* pProxyServer)
{
	strncpy( m_sServiceName, pProxyServer->m_sServiceName, sizeof(m_sServiceName) );
	m_dwLocalIP = pProxyServer->m_dwLocalIP;
	m_usLocalPort = pProxyServer->m_usLocalPort;
    m_dwRealIP = pProxyServer->m_dwRealIP;
    m_usRealPort = pProxyServer->m_usRealPort;
	m_eAppType = pProxyServer->m_eAppType;
    m_bStart = pProxyServer->m_bStart;
	m_bSSLFlagFront = pProxyServer->m_bSSLFlagFront;
	m_bSSLFlagBack =pProxyServer->m_bSSLFlagBack;
}


VOID CProxyServer::InitParam(void)
{
	memset( m_sServiceName, 0, sizeof(m_sServiceName) );
	m_dwLocalIP = 0;
	m_usLocalPort = 0;
	m_usRealPort = 0;
	m_dwRealIP = 0;
	m_eAppType = TCP_APP;
	m_bSSLFlagFront = FALSE;
	m_bSSLFlagBack = FALSE;
	m_bStart = FALSE;
	m_bStopped = TRUE;
	m_iRefCounts = 1;
}

/*
int CProxyServer::IncRefCount(void)
{
	CSingleLock singleLock( &m_mutexRefCounts );
	singleLock.Lock();
	if ( singleLock.IsLocked() )
	{
		m_iRefCounts++;
		singleLock.Unlock();
		return m_iRefCounts;
	}
	return -1;
}

int CProxyServer::DecRefCount(void)
{
	CSingleLock singleLock( &m_mutexRefCounts );
	singleLock.Lock();
	if ( singleLock.IsLocked() )
	{
		m_iRefCounts--;
		singleLock.Unlock();
		return m_iRefCounts;
	}
	return -1;
}

int CProxyServer::GetRefCount(void)
{
	return m_iRefCounts;
}
*/
int HoldProxyServer( CProxyServer *pProxyServer )
{
	if ( !pProxyServer ) return -1;
	CSingleLock singleLock( &pProxyServer->m_mutexRefCounts );
	singleLock.Lock();
	if ( singleLock.IsLocked() )
	{
		pProxyServer->m_iRefCounts++;
		singleLock.Unlock();
		return pProxyServer->m_iRefCounts;
	}
	return -1;
}

int PutProxyServer( CProxyServer *pProxyServer )
{
	if ( !pProxyServer ) return -1;
	CSingleLock singleLock( &pProxyServer->m_mutexRefCounts );
	singleLock.Lock();
	if ( singleLock.IsLocked() )
	{
		pProxyServer->m_iRefCounts--;
		singleLock.Unlock();
		if ( pProxyServer->m_iRefCounts == 0 )
		{
			pProxyServer->Release();
			delete pProxyServer;
			return 0;
		}
		return pProxyServer->m_iRefCounts;
	}
	return -1;
}