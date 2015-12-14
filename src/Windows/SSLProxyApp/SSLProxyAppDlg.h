////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLProxyAppDlg.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// SSLProxyAppDlg.h : 

#pragma once
#include "systemtray.h"
#include "ProxyServer.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "splitterctrl.h"
#include "ResizingDialog.h"
#include <afxcoll.h>
#include <afxmt.h>
#include "resource.h"

#define WM_LOG_INFO WM_USER+100

// CSSLProxyAppDlg
class CSSLProxyAppDlg : public CResizingDialog
{
// constructor
public:
	CSSLProxyAppDlg(CWnd* pParent = NULL);	
	//~CSSLProxyAppDlg(void);
// data member
	enum { IDD = IDD_SSLPROXYAPP_DIALOG };
	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV supports


// implementation
protected:
	HICON m_hIcon;

	// message
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	VOID BuildTree(void);
	VOID DoSplitterReSize(int iDelta);
	BOOL AddProxyServerToTree(CProxyServer* pProxyServer);
	BOOL CheckProxyServer(CProxyServer* pProxyServer);
	VOID LogInfo(LPCTSTR strInfo);
	CProxyServer* FindProxyServerByName(LPCTSTR sServiceName);

	afx_msg void OnClose();
	afx_msg void OnTraymenuExit();
	afx_msg void OnTraymenuShowService();
	afx_msg void OnTraymenuAbout();
	afx_msg void OnBnClickedButtonAddService();
	afx_msg void OnBnClickedButtonExit();
	afx_msg void OnBnClickedButtonRemoveService();
	afx_msg LRESULT OnMessageLogInfo(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedButtonControlService();

protected:
	virtual void OnCancel();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

protected:
	// control
	CTreeCtrl m_tree;
	CImageList m_imageList;
	CStatic m_wndManager;
	CStatic m_wndLog;
	CSplitterCtrl m_ctrlSplit;
	CListBox m_listLog;
	// private data
	enum { eMaxProxyServiceCount = 50 };

	CSystemTray m_systemTray;
	CPtrList* m_plistProxy;
	CMutex* m_pMutexList;
	HTREEITEM m_treeitemAppType[MAX_APP_TYPE];
	int m_iServiceCounts;
public:
	BOOL m_bExit;
	afx_msg void OnTraymenuAddService();
	afx_msg void OnTraymenuControlService();
	afx_msg void OnTraymenuRemoveService();
};

VOID DlgLogInfo( LPCTSTR sInfo );
VOID DlgLogInfo( char *fmt, ...);