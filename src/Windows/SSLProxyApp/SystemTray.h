////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SystemTray.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
#pragma once


// CSystemTray

class CSystemTray : public CWnd
{
	DECLARE_DYNAMIC(CSystemTray)

public:
	CSystemTray();
	virtual ~CSystemTray();

protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL CSystemTray::Create(CWnd* pParent, UINT uCallbackMessage, LPCTSTR szToolTip, UINT nIconID, UINT nMenuID);
protected:
	NOTIFYICONDATA m_tnd;
	UINT m_nDefaultMenuItemID;
	BOOL m_bDefaultMenuItemByPos;   

// static member 
	static UINT m_nMaxTooltipLength;
public:
	BOOL SetMenuDefaultItem(UINT nItem, BOOL bByPos);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	virtual LRESULT OnTrayNotification(WPARAM wParam, LPARAM lParam);
private:
	CWnd* m_pTargetWnd;
public:
	CWnd* GetTargetWnd(void);
	void MinimizeToTray(CWnd* pWnd);
	void MaximizeFromTray(CWnd* pWnd);
	void RemoveIcon(void);
};


