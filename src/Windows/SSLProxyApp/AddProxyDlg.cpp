////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: AddProxyDlg.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// AddProxyDlg.cpp : file
//

#include "stdafx.h"
#include "SSLProxyApp.h"
#include "AddProxyDlg.h"
#include ".\addproxydlg.h"


// CAddProxyDlg dialog

IMPLEMENT_DYNAMIC(CAddProxyDlg, CDialog)
CAddProxyDlg::CAddProxyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddProxyDlg::IDD, pParent)
	, m_bStart(TRUE)
	, m_bPlain(FALSE)
	, m_iLocalPort(0)
	, m_iServerPort(0)
	, m_strServiceName(_T(""))
{
}

CAddProxyDlg::~CAddProxyDlg()
{
}

void CAddProxyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_comboBox);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ctrlLocalIP);
	DDX_Control(pDX, IDC_IPADDRESS2, m_ctrlServerIP);
	DDX_Radio(pDX, IDC_RADIO1, m_bPlain);
	DDX_Check(pDX, IDC_CHECK1, m_bStart);
	DDX_Text(pDX, IDC_EDIT2, m_iLocalPort);
	DDV_MinMaxInt(pDX, m_iLocalPort, 0, 65535);
	DDX_Text(pDX, IDC_EDIT1, m_iServerPort);
	DDX_Text(pDX, IDC_EDIT3, m_strServiceName);
	DDV_MinMaxInt(pDX, m_iServerPort, 0, 65535);

}


BEGIN_MESSAGE_MAP(CAddProxyDlg, CDialog)
ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CAddProxyDlg message processor

BOOL CAddProxyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_comboBox.AddString( "TCP" );
	m_comboBox.AddString( "HTTP" );
	m_comboBox.AddString( "FTP" );
	m_comboBox.SetCurSel( 0 );
	//m_strServiceName = "Input service name here";
	m_ctrlServerIP.SetAddress( 0 );
	m_ctrlLocalIP.SetAddress( 0 );
	UpdateData( FALSE );
	return TRUE; 
}

void CAddProxyDlg::OnBnClickedOk()
{
	// Check local ip and port
	UpdateData( TRUE );
	if ( m_strServiceName == "" )
	{
		MessageBox("Service Name should not be null\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}
	if ( m_strServiceName.GetLength() > 127 )
	{
		MessageBox("Service name length should between 1 and 127 bytes\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}
	strncpy( m_proxyServer.m_sServiceName, m_strServiceName, sizeof(m_proxyServer.m_sServiceName) );
	m_ctrlLocalIP.GetAddress( m_proxyServer.m_dwLocalIP );
	m_ctrlServerIP.GetAddress( m_proxyServer.m_dwRealIP );
	m_proxyServer.m_dwLocalIP = htonl( m_proxyServer.m_dwLocalIP );
	m_proxyServer.m_dwRealIP = htonl( m_proxyServer.m_dwRealIP );
	if ( !m_proxyServer.m_dwRealIP )
	{
		MessageBox("Server IP should not be zero\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}

	if ( m_iLocalPort <= 0 )
	{
		MessageBox("Local port input error\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}

	if ( !IsLocalAddressBindOK( m_proxyServer.m_dwLocalIP, m_proxyServer.m_usLocalPort ) )
	{
		MessageBox("Local ip and port input error or this adress is already used\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}
	if ( m_iServerPort <= 0 )
	{
		MessageBox("Server port input error\n", "input error", MB_OK|MB_ICONERROR );
		return;
	}

	
	if ( MessageBox("Are you sure to add this proxy, click yes to add, no to cancel\n", 
		            "Add proxy",
					MB_YESNO | MB_ICONQUESTION) != IDYES )
	{
		return;
	}
	m_proxyServer.m_eAppType = (EAPP_TYPE)(m_comboBox.GetCurSel());
	m_proxyServer.m_usLocalPort = m_iLocalPort;
	m_proxyServer.m_usRealPort = m_iServerPort;
	m_proxyServer.m_bSSLFlagBack = !m_bPlain;
	m_proxyServer.m_bStart = m_bStart;
	OnOK();
}

BOOL CAddProxyDlg::IsLocalAddressBindOK(DWORD dwLocalIP, USHORT usLocalPort)
{
	struct sockaddr_in sin;
	SOCKET sock;

	memset( &sin, 0, sizeof(sin) );
	sin.sin_family = AF_INET;
	memcpy( &sin.sin_addr, &dwLocalIP, 4 );

	sock = socket(PF_INET,SOCK_STREAM, 0);
	if ( sock == INVALID_SOCKET )
	{
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("Failed to check input local address") );
		return FALSE;
	}

	sin.sin_port = htons( usLocalPort );
	if ( SOCKET_ERROR != bind( sock, (struct sockaddr*)&sin, sizeof(sin) ) )
	{
		closesocket( sock );
		return TRUE;
	}
	
	return FALSE;
}
