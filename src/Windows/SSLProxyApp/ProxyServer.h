////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ProxyServer.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "afxsock.h"
#include "AppProxy.h"
#include "Proxy.h"
#include <afxmt.h>

class CProxyServer
{
public:
	//CHAR m_sIPAddr[16];
	CHAR m_sServiceName[128];
	DWORD m_dwLocalIP;
	USHORT m_usLocalPort;
	DWORD m_dwRealIP;
	USHORT m_usRealPort;
	EAPP_TYPE m_eAppType;
	BOOL m_bStart;
	BYTE m_bSSLFlagFront;
	BYTE m_bSSLFlagBack;
	int	 m_iRefCounts;
	CMutex m_mutexRefCounts;
	
	CProxyServer(EAPP_TYPE eAppType, LPCTSTR sLoalIP, USHORT usLocalPort, BYTE bSSLFlagFront, BYTE bSSLFlagBack);
	CProxyServer(EAPP_TYPE eAppType, DWORD dwLocalIP, USHORT usLocalPort, BYTE bSSLFlagFront, BYTE bSSLFlagBack);
	~CProxyServer(void);
	
	BOOL Start(void);
	BOOL Stop(void);
	BOOL IsStopped(void);
	CAppProxy* GetAppProxy(EAPP_TYPE eAppType);
	VOID SetRealServer(LPTSTR sRealIP, USHORT usRealPort);
	VOID SetRealServer(DWORD dwRealIP, USHORT usRealPort);
	VOID Release(void);
	CProxyServer(void);

	VOID CopyConfig(CProxyServer* pProxyServer);
	BOOL m_bStopped;
	VOID InitParam(void);
	//int IncRefCount(void);
	//int DecRefCount(void);
	//int GetRefCount(void);
};

extern int HoldProxyServer( CProxyServer *pProxyServer );
extern int PutProxyServer( CProxyServer *pProxyServer );