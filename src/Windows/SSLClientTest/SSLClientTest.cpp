////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLClientTest.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// CSSLClientTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "SSLClientTest.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "..\SSL\SSLClient.h"

// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	int nRetCode = 0;

	// 初始化 MFC 并在失败时显示错误
	if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("致命错误: MFC 初始化失败\n"));
		nRetCode = 1;
	}
	else
	{
		// TODO: 在此处为应用程序的行为编写代码。
		CSSLClient client;
		CHAR* sHttpRequest = "GET /mainpage.html HTTP/1.1\r\n"
							 "Accept: image/gif, image/x-xbitmap, image/jpeg, image/pjpeg, application/x-shockwave-flash,.application/vnd.ms-excel, application/vnd.ms-powerpoint, application/msword, */*\r\n"
							 "Accept-Language: zh-cn\r\n"
							 "Accept-Encoding: gzip, deflate\r\n"
							 "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1;  NET CLR 1.1.4322)\r\n"
							 "Host: 192.168.10.191:443\r\n"
							 "Connection: Keep-Alive\r\n\r\n";
		CHAR cBuffer[4096];
		int iRcved;
		int iRemain = 0;

		memset(cBuffer, 0, sizeof(cBuffer));

		if(!client.Init(NULL))
		{
			printf("Error initializing the security library\n");
			return nRetCode;
		}
		client.ConnectToServer("192.168.10.191", 443);
		client.Handshake();
		client.EncryptSend(sHttpRequest, (int)(strlen(sHttpRequest)));
		iRcved = client.RcvDecrypt(cBuffer, sizeof(cBuffer), iRemain);
		printf("Recv %d bytes:\n", iRcved);
		printf("%s", cBuffer);
	}

	return nRetCode;
}
