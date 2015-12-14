////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: FtpAppProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "appproxy.h"

class CFtpAppProxy :
	public CAppProxy
{
public:
	CFtpAppProxy(void);
	~CFtpAppProxy(void);
	int RcvFromClient(CHAR* cRcvBuf, int iRcvBufLen, int& iRcvdLen);
	int FtpPasvCmd(CHAR* sRcvBuf, int& iRcvLen);
	int FtpPortCmd(CHAR* sRcvBuf, int& iRcvLen);
	int FtpAborCmd(CHAR* sRcvBuf, int& iRcvLen);
	int SndToClient(CHAR* cSndBuf, int iSndBufLen);
	int FtpPasvResp(CHAR* cSndBuf, int iSndBufLen);
// Data member
protected:
public:
	BOOL FtpParseAddress(CHAR* sBuf, DWORD& dwIP, USHORT& usPort);
	BOOL FindLocalUnusedPort(USHORT& usPort);
	VOID AddressToString(DWORD dwIP, USHORT usPort, UCHAR a[]);
	int RcvFromServer(CHAR* cRcvBuf, int iRcvBufLen , int& iRcvdLen);
};
