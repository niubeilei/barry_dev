////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AddProxyDlg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "proxy.h"
#include "proxyserver.h"


// CAddProxyDlg 对话框

class CAddProxyDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddProxyDlg)

public:
	CAddProxyDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CAddProxyDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_ADD_PROXY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboBox;
	CIPAddressCtrl m_ctrlLocalIP;
	CIPAddressCtrl m_ctrlServerIP;
	BOOL m_bPlain;
	BOOL m_bStart;
	int m_iLocalPort;
	int m_iServerPort;
	virtual BOOL OnInitDialog();
	CString m_strServiceName;
	afx_msg void OnBnClickedOk();
	CProxyServer m_proxyServer;
	BOOL IsLocalAddressBindOK(DWORD dwLocalIP, USHORT usLocalPort);
};
