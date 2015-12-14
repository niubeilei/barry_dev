////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemTray.cpp
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
// SystemTray.cpp : file
//

#include "stdafx.h"
#include "SSLProxyApp.h"
#include "SystemTray.h"
#include ".\systemtray.h"

UINT CSystemTray::m_nMaxTooltipLength  = 64;

// CSystemTray

IMPLEMENT_DYNAMIC(CSystemTray, CWnd)
CSystemTray::CSystemTray()
: m_pTargetWnd(NULL)
{
}

CSystemTray::~CSystemTray()
{
}


BEGIN_MESSAGE_MAP(CSystemTray, CWnd)
END_MESSAGE_MAP()



// CSystemTray Message Processor


BOOL CSystemTray::Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, UINT nIconID, UINT nMenuID)
{
	HICON hIcon;

	m_nMaxTooltipLength = sizeof(m_tnd.szTip);

	// Make sure we avoid conflict with other messages
	ASSERT( uCallbackMessage >= WM_APP );

	// Tray only supports tooltip text up to m_nMaxTooltipLength) characters
	ASSERT( AfxIsValidString(szToolTip) );
	ASSERT( _tcslen(szToolTip) <= m_nMaxTooltipLength );

	// Get Icon
	hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(nIconID));  // Icon to use

	// Create an invisible window
	CWnd::CreateEx(0, AfxRegisterWndClass(0), _T(""), WS_POPUP, 0,0,0,0, NULL, 0);

	// load up the NOTIFYICONDATA structure
	m_tnd.cbSize = sizeof(NOTIFYICONDATA);
	m_tnd.hWnd   = pParent->GetSafeHwnd()? pParent->GetSafeHwnd() : m_hWnd;
	m_tnd.uID    = nMenuID;
	m_tnd.hIcon  = hIcon;
	m_tnd.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_tnd.uCallbackMessage = uCallbackMessage;
	_tcsncpy( m_tnd.szTip, szToolTip, m_nMaxTooltipLength-1 );

	return Shell_NotifyIcon(NIM_ADD, &m_tnd);
}

BOOL CSystemTray::SetMenuDefaultItem(UINT nItem, BOOL bByPos)
{
	CMenu menu, *pSubMenu;
	if ( ( m_nDefaultMenuItemID == nItem ) && ( m_bDefaultMenuItemByPos == bByPos ) ) 
	{
		return TRUE;
	}
	m_nDefaultMenuItemID = nItem;
	m_bDefaultMenuItemByPos = bByPos;   

	if ( !menu.LoadMenu( m_tnd.uID ) )
	{
		return FALSE;
	}

	if ( !(pSubMenu = menu.GetSubMenu(0)) )
	{
		return FALSE;
	}

	::SetMenuDefaultItem(pSubMenu->m_hMenu, m_nDefaultMenuItemID, m_bDefaultMenuItemByPos);

	return TRUE;
}

LRESULT CSystemTray::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if ( message == m_tnd.uCallbackMessage )
	{
		return OnTrayNotification(wParam, lParam);
	}
	return CWnd::WindowProc( message, wParam, lParam );
}

LRESULT CSystemTray::OnTrayNotification(WPARAM wParam, LPARAM lParam)
{
	CMenu menu, *pSubMenu;
	CWnd *pTargetWnd = NULL;
	CPoint pos;

	//Return quickly if its not for this tray icon
	if ( wParam != m_tnd.uID )
	{
		return 0L;
	}

	pTargetWnd = GetTargetWnd();
	if ( !pTargetWnd )
	{
		return 0L;
	}
	// Clicking with right button brings up a context menu
	if ( LOWORD(lParam) == WM_RBUTTONUP )
	{    
		if ( !menu.LoadMenu(m_tnd.uID) )
		{
			return 0;
		}
		pSubMenu = menu.GetSubMenu(0);
		if ( !pSubMenu )
		{
			return 0;
		}
		// Make chosen menu item the default (bold font)
		::SetMenuDefaultItem( pSubMenu->m_hMenu, m_nDefaultMenuItemID, m_bDefaultMenuItemByPos );

		// Display and track the popup menu
		GetCursorPos( &pos );
		pTargetWnd->SetForegroundWindow(); 
		::TrackPopupMenu( pSubMenu->m_hMenu, 0, pos.x, pos.y, 0, pTargetWnd->GetSafeHwnd(), NULL);
		// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
		pTargetWnd->PostMessage(WM_NULL, 0, 0);

		menu.DestroyMenu();
	} 
	else if (LOWORD(lParam) == WM_LBUTTONDBLCLK) 
	{
		// double click received, the default action is to execute default menu item
		pTargetWnd->SetForegroundWindow();  

		UINT uItem;
		if ( m_bDefaultMenuItemByPos )
		{
			if (!menu.LoadMenu(m_tnd.uID))
				return 0;

			pSubMenu = menu.GetSubMenu(0);
			if ( !pSubMenu )
			{
				return 0;
			}
			uItem = pSubMenu->GetMenuItemID( m_nDefaultMenuItemID );

			menu.DestroyMenu();
		}
		else
		{
			uItem = m_nDefaultMenuItemID;
		}
		pTargetWnd->PostMessage( WM_COMMAND, uItem, 0 );
	}

	return 1;
}

CWnd* CSystemTray::GetTargetWnd(void)
{
	if ( m_pTargetWnd )
	{
		return m_pTargetWnd;
	}
	else
	{
		return AfxGetMainWnd();
	}
}

void CSystemTray::MinimizeToTray(CWnd* pWnd)
{
	ASSERT( pWnd );
	pWnd->ShowWindow( SW_MINIMIZE );
	pWnd->ShowWindow( SW_HIDE );
}

void CSystemTray::MaximizeFromTray(CWnd* pWnd)
{
	ASSERT( pWnd );
	pWnd->ShowWindow( SW_RESTORE );
}

void CSystemTray::RemoveIcon(void)
{
	m_tnd.uFlags = 0;
	Shell_NotifyIcon( NIM_DELETE, &m_tnd );
}
