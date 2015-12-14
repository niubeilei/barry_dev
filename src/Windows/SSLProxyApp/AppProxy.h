////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AppProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "..\SSL\SSLClient.h"

class CProxyServer;

class CAppProxy
{
public:

	SOCKET m_socketFront;
	SOCKET m_socketBack;
	BYTE m_bSSLFlagBack;
	CSSLClient* m_sslBack;
	DWORD m_dwRealIPAddr;
	USHORT m_usRealPort;
	CProxyServer* m_pProxyServer;

	CAppProxy(void);
	~CAppProxy(void);
	virtual int SndToServer(CHAR* cSndBuf, int iSndBufLen);
	virtual int SndToClient(CHAR* cSndBuf, int iSndBufLen);
	virtual int RcvFromClient(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen);
	virtual int RcvFromServer(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen);
	virtual BOOL ConnectToBackend(DWORD dwIPAddr, USHORT usPort);
	virtual VOID Release(void);
};
