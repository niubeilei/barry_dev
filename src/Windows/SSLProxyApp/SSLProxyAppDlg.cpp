////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SSLProxyAppDlg.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// SSLProxyAppDlg.cpp : file
//

#include "stdafx.h"
#include "SSLProxyApp.h"
#include "SSLProxyAppDlg.h"
#include "ProxyServer.h"
#include ".\sslproxyappdlg.h"
#include "..\Util\AosWinLog.h"
#include "..\Util\AosWinAlarm.h"
#include "..\Util\WinReturnCode.h"
#include ".\AddProxyDlg.h"
#include "ServiceControl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define	WM_ICON_NOTIFY WM_APP+10

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSSLProxyAppDlg dialog



CSSLProxyAppDlg::CSSLProxyAppDlg(CWnd* pParent /*=NULL*/)
	: CResizingDialog(CSSLProxyAppDlg::IDD, pParent)
	, m_iServiceCounts(0)
	, m_plistProxy(NULL)
	, m_pMutexList(NULL)
	, m_bExit(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	SetControlInfo( IDC_TREE1, ANCHORE_TOP| RESIZE_HOR );
	SetControlInfo( IDC_LIST1, RESIZE_HOR | RESIZE_VER );
	SetControlInfo( IDC_STATIC_PROXY_SERVICES, ANCHORE_TOP| RESIZE_HOR );
	SetControlInfo( IDC_STATIC_LOG_INFO, RESIZE_HOR | RESIZE_VER  );
	SetControlInfo( IDC_STATIC_SPLIT_BAR, ANCHORE_TOP| RESIZE_HOR );
}

void CSSLProxyAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_STATIC_PROXY_SERVICES, m_wndManager);
	DDX_Control(pDX, IDC_STATIC_LOG_INFO, m_wndLog);
	DDX_Control(pDX, IDC_LIST1, m_listLog);
}

BEGIN_MESSAGE_MAP(CSSLProxyAppDlg, CResizingDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CLOSE()
	ON_COMMAND(ID_TRAYMENU_EXIT, OnTraymenuExit)
	ON_COMMAND(ID_TRAYMENU_SHOW_SERVICE, OnTraymenuShowService)
	ON_COMMAND(ID_TRAYMENU_ABOUT, OnTraymenuAbout)
	ON_BN_CLICKED(IDC_BUTTON_ADD_PROXY, OnBnClickedButtonAddService)
	ON_BN_CLICKED(IDC_BUTTON_EXIT, OnBnClickedButtonExit)
	ON_BN_CLICKED(IDC_BUTTON_LOG_LEVEL, OnBnClickedButtonRemoveService)
	ON_MESSAGE(WM_LOG_INFO, OnMessageLogInfo)
	ON_BN_CLICKED(IDC_BUTTON_CONTROL_SERVICE, OnBnClickedButtonControlService)
	ON_COMMAND(ID_TRAYMENU_ADD_SERVICE, OnTraymenuAddService)
	ON_COMMAND(ID_TRAYMENU_CONTROL_SERVICE, OnTraymenuControlService)
	ON_COMMAND(ID_TRAYMENU_REMOVE_SERVICE, OnTraymenuRemoveService)
END_MESSAGE_MAP()


// CSSLProxyAppDlg message processing

BOOL CSSLProxyAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AosWinMinLog( eAosWinMD_TcpProxy, ("SSLProxyApp start") );

	// IDM_ABOUTBOX should be in the field of system command
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// set big icon
	SetIcon(m_hIcon, FALSE);		// set small icon

	m_systemTray.Create(NULL, WM_ICON_NOTIFY, _T("AOS Proxy Client"), IDR_MAINFRAME, IDR_MENU_TRAY );
	m_systemTray.SetMenuDefaultItem( 0, TRUE );
	
	BuildTree();

	// Init Splitter bar
	CRect rc;
	CWnd *pWnd; 
	pWnd = GetDlgItem( IDC_STATIC_SPLIT_BAR );
	pWnd->GetWindowRect( &rc );
	ScreenToClient( &rc );
	m_ctrlSplit.Create( WS_CHILD | WS_VISIBLE, rc, this, IDC_STATIC_SPLIT_BAR );
	m_ctrlSplit.SetRange( 50, 150, -1 );

	// Init Proxy Server List

	AosWinTrace( ("Init proxy services list") );
	m_plistProxy = new CPtrList( eMaxProxyServiceCount );

	// Create Proxy Service List Access Mutex
	m_pMutexList = new CMutex( TRUE ); // synchronize access proxy server list

	return TRUE;  
}

void CSSLProxyAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

void CSSLProxyAppDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // drawing device context

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// make cursor in the center of the work rect
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// draw icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CResizingDialog::OnPaint();
	}
}

// when user drag minimize window, system call this
// function to fetch cursor display
HCURSOR CSSLProxyAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CSSLProxyAppDlg::OnClose()
{
	m_systemTray.MinimizeToTray( this );
	//CDialog::OnClose();
}

void CSSLProxyAppDlg::OnTraymenuExit()
{
	m_systemTray.RemoveIcon();
	OnBnClickedButtonExit();
	//DestroyWindow();
}

void CSSLProxyAppDlg::OnTraymenuShowService()
{
	m_systemTray.MaximizeFromTray( this );
}

void CSSLProxyAppDlg::OnTraymenuAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

void CSSLProxyAppDlg::OnCancel()
{
	m_systemTray.RemoveIcon();

	CDialog::OnCancel();
}


VOID CSSLProxyAppDlg::BuildTree(void)
{

	CBitmap bm;
	m_imageList.Create( 16, 16, ILC_COLOR24, 0, 4 );

	bm.LoadBitmap(IDB_BITMAP2);
	m_imageList.Add( &bm, RGB(0, 0, 0) );

	//m_imageList.Create( IDB_BITMAP1, 32, 2, RGB(0,0,0) );
	m_tree.SetImageList( &m_imageList, TVSIL_NORMAL );
	m_treeitemAppType[TCP_APP] = m_tree.InsertItem( "Tcp Service", 0, 0 );
	m_treeitemAppType[HTTP_APP] = m_tree.InsertItem( "Http Service", 0, 0 );
	m_treeitemAppType[FTP_APP] = m_tree.InsertItem( "Ftp Service", 0, 0 );
}

void CSSLProxyAppDlg::OnBnClickedButtonAddService()
{
	CAddProxyDlg dlg;
	dlg.m_strServiceName.Format( "service%d", m_iServiceCounts+1 );
	if ( dlg.DoModal() != IDOK )
	{
		return;
	}

	// Get Data
	CProxyServer* pProxyServer = new CProxyServer;
	pProxyServer->CopyConfig( &dlg.m_proxyServer );

	// check whether the max count of proxy server is reach
	// or whether the proxy server is exist
	
	if ( !CheckProxyServer( pProxyServer ) )
	{
	
		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("proxy server failed to pass verification") );
		delete pProxyServer;
		LogInfo( "proxy server to be added failed to pass checking\n" );
		return;
	}
	m_iServiceCounts++;
}

LRESULT CSSLProxyAppDlg::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == WM_NOTIFY )
	{
		if ( wParam == IDC_STATIC_SPLIT_BAR )
		{	
			SPC_NMHDR* pHdr = (SPC_NMHDR*) lParam;
			DoSplitterReSize( pHdr->delta );
		}
	}
	return CDialog::DefWindowProc( message, wParam, lParam );
}

VOID CSSLProxyAppDlg::DoSplitterReSize(int iDelta)
{
	CSplitterCtrl::ChangeHeight( &m_tree, iDelta );
	CSplitterCtrl::ChangeHeight( &m_wndManager, iDelta );
	CSplitterCtrl::ChangeHeight( &m_listLog, -iDelta, CW_BOTTOMALIGN );
	CSplitterCtrl::ChangeHeight( &m_wndLog, -iDelta, CW_BOTTOMALIGN );

	Invalidate();
	UpdateWindow();
}

void CSSLProxyAppDlg::OnBnClickedButtonExit()
{
	if ( MessageBox( "Are you sure to exit 'SSLProxyApp', press yes to make sure or no to cancel", 
			"Exit Application",
			MB_YESNO|MB_ICONQUESTION ) == IDNO )
	{
		return;
	}
	m_bExit = TRUE;
	if ( m_plistProxy )
	{
		while ( !m_plistProxy->IsEmpty() )
		{
			CProxyServer* pProxyServer = (CProxyServer*) m_plistProxy->RemoveHead();
			if( !pProxyServer ) continue;
			pProxyServer->Stop();
			while( !pProxyServer->IsStopped() ) Sleep(50);
			PutProxyServer( pProxyServer );
		}
		delete m_plistProxy;
	}

	if ( m_pMutexList )
	{
		delete m_pMutexList;
	}
	OnCancel();
}

BOOL CSSLProxyAppDlg::AddProxyServerToTree(CProxyServer* pProxyServer)
{
	HTREEITEM hService;
	CString strName;
	CString strInfo;

	strName.Format("%s (%s:%d<--%s-->%s:%d)", pProxyServer->m_sServiceName,
		inet_ltoa(pProxyServer->m_dwLocalIP), pProxyServer->m_usLocalPort, 
		pProxyServer->m_bSSLFlagBack?"S":"P",
		inet_ltoa(pProxyServer->m_dwRealIP), pProxyServer->m_usRealPort );
	
	hService = m_tree.InsertItem( TVIF_TEXT|TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_PARAM, 
								  strName, 
								  pProxyServer->m_bStart? 1:2, 
								  pProxyServer->m_bStart? 1:2,
								  0, 0, 
								  (LPARAM)pProxyServer, 
								  m_treeitemAppType[pProxyServer->m_eAppType], 
								  TVI_LAST );
	
	m_tree.Expand( m_treeitemAppType[pProxyServer->m_eAppType], TVE_COLLAPSE );
	m_tree.SelectItem( hService );
	m_tree.EnsureVisible( hService );
	
	strInfo.Format( "add '%s'", strName );
	AosWinMinLog( eAosWinMD_TcpProxy, ("%s", strInfo.GetBuffer(strInfo.GetLength())) );
	LogInfo( strInfo );
	
	return TRUE;
}

VOID CSSLProxyAppDlg::LogInfo(LPCTSTR strInfo)
{
	CString str;
	CHAR sLocalTime[64];
	int index;
	
	_strtime( sLocalTime );
	str.Format( "<%s>: %s", sLocalTime, strInfo );
	index = m_listLog.AddString( str );
	m_listLog.SetCurSel( index );
	m_listLog.SetCurSel( -1 );
}

BOOL CSSLProxyAppDlg::CheckProxyServer(CProxyServer* pProxyServer)
{
	CProxyServer* pServer;
	CString strError;
	DWORD dwLocalIP;
	BOOL bRet = TRUE;
	CSingleLock singleLock( m_pMutexList );

	singleLock.Lock();

	if ( !singleLock.IsLocked() )
	{

		AosWinAlarm( eAosWinMD_TcpProxy, eAosWinAlarm_ProgErr, ("Failed to lock proxy server list mutext") );
		return FALSE;
	}

	POSITION hPos = m_plistProxy->GetHeadPosition();
	while( hPos && (pServer = (CProxyServer*)m_plistProxy->GetNext( hPos )) )
	{
		if ( strcmp( pServer->m_sServiceName, pProxyServer->m_sServiceName ) == 0 )
		{
			strError.Format( "'%s' already exists", pProxyServer->m_sServiceName );
			MessageBox( strError, "Add proxy service error", MB_OK|MB_ICONERROR );
			bRet = FALSE;
			break;
		}

		if ( pServer->m_dwLocalIP == pProxyServer->m_dwLocalIP &&
			 pServer->m_usLocalPort == pProxyServer->m_usLocalPort )
		{
			dwLocalIP = htonl( pProxyServer->m_dwLocalIP );
			strError.Format( "'%s' already listened on %d.%d.%d.%d:%d", 
				pProxyServer->m_sServiceName,NIPQUAD(dwLocalIP), pProxyServer->m_usLocalPort );
			MessageBox( strError, "Add proxy service error", MB_OK|MB_ICONERROR );
			bRet = FALSE;
			break;
		}

	}

	m_plistProxy->AddTail( (VOID*) pProxyServer );

	AddProxyServerToTree( pProxyServer );

	if ( pProxyServer->m_bStart )
	{
		pProxyServer->Start();
	}

	singleLock.Unlock();
	return bRet;
}

void CSSLProxyAppDlg::OnBnClickedButtonRemoveService()
{
	HTREEITEM hItem;
	POSITION pos;
	CProxyServer *pProxyServer;
	DWORD dwLocalIP, dwRealIP;
	CString strName, strInfo;

	hItem = m_tree.GetSelectedItem();
	if ( !hItem )
	{
		MessageBox("please select serivce you want to remove\n", "remove service", MB_OK|MB_ICONWARNING);
		return;
	}

	pProxyServer = (CProxyServer*)m_tree.GetItemData( hItem );
	if ( !pProxyServer )
	{
		MessageBox("please select serivce you want to remove\n", "remove service", MB_OK|MB_ICONWARNING);
		return;
	}

	pos = m_plistProxy->Find( (VOID*)pProxyServer );

	if ( pos )
	{

		dwLocalIP = htonl( pProxyServer->m_dwLocalIP );
		dwRealIP = htonl( pProxyServer->m_dwRealIP );
		strName.Format( "'%s' %d.%d.%d.%d.%d<-->%d.%d.%d.%d:%d", pProxyServer->m_sServiceName,
			NIPQUAD(dwLocalIP), pProxyServer->m_usLocalPort,
			NIPQUAD(dwRealIP), pProxyServer->m_usRealPort);

		strInfo.Format( "Do you want to remove %s,\n press yes to remove, no to cancel", strName );
		if ( MessageBox( strInfo, "remove service", MB_YESNO|MB_ICONQUESTION ) == IDNO )
		{
			return;
		}
		m_tree.DeleteItem( hItem );
		m_iServiceCounts--;
	}
	else
	{
		m_tree.DeleteItem( hItem );
		m_iServiceCounts--;
		return;
	}
	

	
	m_plistProxy->RemoveAt( pos );

	strInfo.Format( "remove service %s", strName );
	AosWinMinLog( eAosWinMD_TcpProxy, ( strInfo.GetBuffer( strInfo.GetLength() ) ) );

	LogInfo( strInfo );

	pProxyServer->Stop();
	PutProxyServer( pProxyServer );
	/*
	while( !pProxyServer->IsStopped() )
	{
		Sleep( 50 );
	}
	delete pProxyServer;
	*/
}

LRESULT CSSLProxyAppDlg::OnMessageLogInfo( WPARAM wParam, LPARAM lParam )
{
	CHAR* sNews = (CHAR*) wParam;
	if ( !sNews ) return 0;
	LogInfo( sNews );
	free(sNews);
	return 0;
}

VOID DlgLogInfo( LPCTSTR sInfo )
{
	CHAR* sNews;
	int iLen;

	if( ((CSSLProxyAppDlg*)theApp.m_pMainWnd)->m_bExit ) return;
	iLen = (int)strlen( sInfo );
	sNews = (CHAR*)malloc(iLen+1);
	strncpy( sNews, sInfo, iLen+1 );
	::PostMessage( theApp.m_pMainWnd->m_hWnd, WM_LOG_INFO, (WPARAM)sNews, 0 );
}

VOID DlgLogInfo( char *fmt, ...)
{
	CHAR* sBuffer;
	va_list args;
	int n;

	if( ((CSSLProxyAppDlg*)theApp.m_pMainWnd)->m_bExit ) return;
	
	sBuffer = (CHAR*) malloc( eAosWinLogLocalBufSize );
	va_start( args, fmt );
	n = vsprintf( sBuffer, fmt, args );
	va_end(args);

	::PostMessage( theApp.m_pMainWnd->m_hWnd, WM_LOG_INFO, (WPARAM)sBuffer, 0 );
}
void CSSLProxyAppDlg::OnBnClickedButtonControlService()
{
	HTREEITEM hItem = m_tree.GetSelectedItem();
	CProxyServer *pProxyServer;
	CString str;
	if ( hItem )
	{
		pProxyServer = (CProxyServer*) m_tree.GetItemData( hItem );

		if ( pProxyServer )
		{
			str.Format("Are you sure to %s %s, press yes to make sure, no to cancel\n",
				pProxyServer->m_bStart?"stop":"start", pProxyServer->m_sServiceName );
			if ( MessageBox(str, "Service control", MB_YESNO|MB_ICONQUESTION) == IDYES )
			{
				if ( pProxyServer->m_bStart )
				{
					pProxyServer->Stop();
					m_tree.SetItem( hItem, TVIF_STATE, NULL, 0, 0, 2, TVIS_STATEIMAGEMASK , 0 );
				}
				else
				{
					pProxyServer->Start();
					m_tree.SetItem( hItem, TVIF_STATE, NULL, 0, 0, 1, TVIS_STATEIMAGEMASK , 0 );
				}
			}
			return;
		}
	}

	CServiceControl dlg;
	dlg.DoModal();
	
	pProxyServer = FindProxyServerByName( dlg.m_strServiceName );
	if ( !pProxyServer ) return;
	if ( dlg.m_bStart )
	{
		pProxyServer->Start();
	}
	else
	{
		pProxyServer->Stop();
	}

}

CProxyServer* CSSLProxyAppDlg::FindProxyServerByName(LPCTSTR sServiceName)
{
	CProxyServer *pServer = NULL;
	POSITION hPos = m_plistProxy->GetHeadPosition();
	while( hPos && (pServer = (CProxyServer*)m_plistProxy->GetNext( hPos )) )
	{
		if ( strcmp( pServer->m_sServiceName, sServiceName ) == 0 )
		{
			return pServer;
		}
	}
	return NULL;
}

void CSSLProxyAppDlg::OnTraymenuAddService()
{
	OnTraymenuShowService();
	OnBnClickedButtonAddService();
}

void CSSLProxyAppDlg::OnTraymenuControlService()
{
	OnTraymenuShowService();
	OnBnClickedButtonControlService();
}

void CSSLProxyAppDlg::OnTraymenuRemoveService()
{
	OnTraymenuShowService();
	OnBnClickedButtonRemoveService();
}
