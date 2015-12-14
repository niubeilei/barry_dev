////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: HttpAppProxy.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include ".\httpappproxy.h"
#include "..\Common\common.h"

#define HTTP_DEFAULT_PORT 80

CHttpAppProxy::CHttpAppProxy(void)
{
}

CHttpAppProxy::~CHttpAppProxy(void)
{
}

int CHttpAppProxy::SndToServer(CHAR* cSndBuf, int iSndBufLen)
{
	DWORD dwIPAddr;
	USHORT usPort;
    if(!m_socketBack)
	{
		if(!GetHostFromHttpRequest(cSndBuf, iSndBufLen, dwIPAddr, usPort))
		{
			return eAosWinRc_Failed;
		}
		if(!ConnectToBackend(dwIPAddr, usPort))
		{
			// add log here
			return eAosWinRc_Failed;
		}
	}
	if(!m_socketBack)
	{
		// add log here
		return eAosWinRc_Failed;
	}

	// send to server
	return CAppProxy::SndToServer(cSndBuf, iSndBufLen);
}

BOOL CHttpAppProxy::GetHostFromHttpRequest(CHAR* pcRcvBuffer, int iBufferLen, DWORD &dwIPAddr, USHORT& usPort)
{
	// "Host: 192.168.10.191:443\r\n"
	CHAR* strPartten[2]={"GET http://", "POST http://"};
	PCHAR pPos, pcBuffer = NULL;
	CHAR *strPort = NULL, *strIPAddr = NULL;
	int iPortOffset, iIPAddrOffset, iEndOfLineOffset; 
	struct hostent *hp;
	int iPartten = 0;
	BOOL bRet = TRUE;

	usPort = 0;
	dwIPAddr = 0;

	if(!pcRcvBuffer || iBufferLen <= 0) return FALSE;
	pcBuffer = (CHAR*)malloc(iBufferLen+1);
	if(!pcBuffer) return -1;
	memset(pcBuffer, 0, iBufferLen+1);
	memcpy(pcBuffer, pcRcvBuffer, iBufferLen);

	// search IPAddr
	if(!(pPos = strstr(pcBuffer, strPartten[0])))
	{
		if(!(pPos = strstr(pcBuffer, strPartten[++iPartten])))
		{
			bRet = FALSE;
			goto REQUEST_PARSE_CLEANUP;
		}
	}
	iIPAddrOffset = (int)(pPos - pcBuffer + strlen(strPartten[iPartten]));
	if(iIPAddrOffset >= iBufferLen)
	{
		bRet = FALSE;
		goto REQUEST_PARSE_CLEANUP;
	}
	// search "\r\n"
	if(!(pPos = strstr(pPos+strlen(strPartten[iPartten]), "/")))
	{
		bRet = FALSE;
		goto REQUEST_PARSE_CLEANUP;
	}
	iEndOfLineOffset = (int)(pPos - pcBuffer);
	

	// search port
	pPos = strchr(pcBuffer+iIPAddrOffset, ':');

	if(!pPos) // no port
	{

		usPort = HTTP_DEFAULT_PORT;
		strIPAddr = (CHAR*)malloc(iEndOfLineOffset-iIPAddrOffset+1);
		memset(strIPAddr, 0, iEndOfLineOffset-iIPAddrOffset+1);
		strncpy(strIPAddr, pcBuffer+iIPAddrOffset, iEndOfLineOffset-iIPAddrOffset);  
	}
	else
	{
		iPortOffset = (int)(pPos - pcBuffer+1);
		if(iPortOffset >= iEndOfLineOffset)
		{
			usPort = HTTP_DEFAULT_PORT;
			iPortOffset = iEndOfLineOffset+1;
		}
		else
		{
			strPort = (CHAR*)malloc(iEndOfLineOffset-iPortOffset+1);
			memset(strPort, 0, iEndOfLineOffset-iPortOffset+1); 
			strncpy(strPort, pcBuffer+iPortOffset, iEndOfLineOffset-iPortOffset);
			usPort = atoi(strPort);
		}
		strIPAddr = (CHAR*)malloc(iPortOffset-1-iIPAddrOffset+1);
		memset(strIPAddr, 0, iPortOffset-1-iIPAddrOffset+1);
		strncpy(strIPAddr, pcBuffer+iIPAddrOffset, iPortOffset-1-iIPAddrOffset);  
	}
	
	if((hp = gethostbyname(strIPAddr)) == NULL)
    {
		bRet = FALSE;
		goto REQUEST_PARSE_CLEANUP;
    }
    else
    {
        memcpy(&dwIPAddr, hp->h_addr, 4);
    }

REQUEST_PARSE_CLEANUP:
	if(pcBuffer) free(pcBuffer);
	if(strIPAddr) free(strIPAddr);
	if(strPort) free(strPort);

	return bRet;
}


