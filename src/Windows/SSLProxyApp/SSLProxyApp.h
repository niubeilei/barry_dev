////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLProxyApp.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// SSLProxyApp.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error 在包含用于 PCH 的此文件之前包含“stdafx.h”
#endif

#include "resource.h"		// 主符号


// CSSLProxyAppApp:
// 有关此类的实现，请参阅 SSLProxyApp.cpp
//

class CSSLProxyAppApp : public CWinApp
{
public:
	CSSLProxyAppApp();

// 重写
	public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CSSLProxyAppApp theApp;
