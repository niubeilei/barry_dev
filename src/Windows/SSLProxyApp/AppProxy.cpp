////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\appproxy.h"
#include "..\Common\common.h"

CAppProxy::CAppProxy(void)
: m_sslBack(NULL)
, m_pProxyServer(NULL)
{
	m_socketBack = NULL;
	m_socketFront = NULL;
	m_bSSLFlagBack = FALSE;
	m_dwRealIPAddr = 0;
	m_usRealPort = 0;
}

CAppProxy::~CAppProxy(void)
{
}

int CAppProxy::SndToServer(CHAR* cSndBuf, int iSndBufLen)
{
	int iRet;
	// backend none ssl connection
	if(!m_bSSLFlagBack) 
	{
		if(SOCKET_ERROR == (iRet = send(m_socketBack, cSndBuf, iSndBufLen, 0)))
		{
			// add log here
		}
		return iRet;
	}

	// backend ssl connection
	if(m_sslBack)
	{
		iRet = m_sslBack->EncryptSend(cSndBuf, iSndBufLen);
	}
	return iRet;
}

int CAppProxy::SndToClient(CHAR* cSndBuf, int iSndBufLen)
{
	DWORD dwError = 0;
	int iRet;

	if(SOCKET_ERROR == (iRet = send(m_socketFront, cSndBuf, iSndBufLen, 0)))
	{
		// send should be buffered
		dwError = WSAGetLastError();
		//return eAosWinRc_Failed;
	}

	//AosWinTrace( ("SndToClient %d", iRet ) );
	return iRet;
}

int CAppProxy::RcvFromClient(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen)
{

	iRcvdLen = 0;
	// receive data from client
	iRcvdLen = recv(m_socketFront, cRcvBuf, iRcvBufLen, 0);
	return iRcvdLen;
}

int CAppProxy::RcvFromServer(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen)
{
	iRcvdLen = 0;
	// Backend none ssl connection
	if(!m_bSSLFlagBack)
	{
		iRcvdLen = recv(m_socketBack, cRcvBuf, iRcvBufLen, 0);
		//AosWinTrace( ("RcvFromServer %d", iRcvdLen ) );
		return iRcvdLen;
	}

	// Backend ssl connection
	if(m_sslBack)
	{
		INT iRemain = 0;
        iRcvdLen = m_sslBack->RcvDecrypt(cRcvBuf, iRcvBufLen, iRemain);
		//iRcvdLen = m_sslBack->RcvDecrypt1(cRcvBuf, iRcvBufLen);
		return iRcvdLen;
	}
	return eAosWinRc_Success;
}

BOOL CAppProxy::ConnectToBackend(DWORD dwIPAddr, USHORT usPort)
{
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(usPort);
	memcpy(&sin.sin_addr, &dwIPAddr, 4);
	
	if(!m_bSSLFlagBack)
	{
		if(!m_socketBack)
		{
			m_socketBack = socket(PF_INET, SOCK_STREAM, 0);
			if(m_socketBack == INVALID_SOCKET)
			{
				// add log here printf("**** Error %d creating socket\n", WSAGetLastError());
				return FALSE;
			}
		}
		
		if(SOCKET_ERROR == connect(m_socketBack, (struct sockaddr *)&sin,sizeof(sin)))
		{
			// add log here
			closesocket(m_socketBack);
			m_socketBack = NULL;
			return FALSE;
		}
		return TRUE;
	}

	// ssl is enabled
	if(m_sslBack) free(m_sslBack);
	m_sslBack = new CSSLClient;
	if(!m_sslBack->Init(NULL))
	{
		// add log here printf("Error initializing the security library\n");
		goto CONNECT_TO_BACKEND_CLEANUP;
	}
	if(0 != m_sslBack->ConnectToServer(inet_ntoa(sin.sin_addr), usPort))
	{
		// add log here
		goto CONNECT_TO_BACKEND_CLEANUP;
	}
	m_socketBack = m_sslBack->m_socket;
	
	if(!m_sslBack->Handshake())
	{
		// add log here
		goto CONNECT_TO_BACKEND_CLEANUP;
	}
	return TRUE;
CONNECT_TO_BACKEND_CLEANUP:
	if(m_socketBack) closesocket(m_socketBack);
	if(m_sslBack)
	{
		m_sslBack->UnloadSecurityLibrary();
		free(m_sslBack);
		m_sslBack = NULL;
	}
	return FALSE;
}

VOID CAppProxy::Release(void)
{

	if(m_socketFront) closesocket(m_socketFront);
	if(m_socketBack) closesocket(m_socketBack);
	if(m_sslBack)
	{
		m_sslBack->Release();
		m_sslBack->UnloadSecurityLibrary();
		free(m_sslBack);
	}
	return;
}
