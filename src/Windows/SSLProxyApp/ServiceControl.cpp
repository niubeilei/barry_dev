////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: ServiceControl.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// ServiceControl.cpp : 实现文件
//

#include "stdafx.h"
#include "SSLProxyApp.h"
#include "ServiceControl.h"


// CServiceControl 对话框

IMPLEMENT_DYNAMIC(CServiceControl, CDialog)
CServiceControl::CServiceControl(CWnd* pParent /*=NULL*/)
	: CDialog(CServiceControl::IDD, pParent)
	, m_strServiceName(_T(""))
	, m_bStart(FALSE)
{
}

CServiceControl::~CServiceControl()
{
}

void CServiceControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strServiceName);
	DDX_Radio(pDX, IDC_RADIO1, m_bStart);
}


BEGIN_MESSAGE_MAP(CServiceControl, CDialog)
END_MESSAGE_MAP()


// CServiceControl 消息处理程序
