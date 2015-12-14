////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpAppProxy.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "appproxy.h"

class CHttpAppProxy :
	public CAppProxy
{
public:
	CHttpAppProxy(void);
	~CHttpAppProxy(void);
	BOOL GetHostFromHttpRequest(CHAR* pcRcvBuffer, int iBufferLen, DWORD& dwIPAddr, USHORT& usPort);
	int SndToServer(CHAR* cSndBuf, int iSndBufLen);
};
