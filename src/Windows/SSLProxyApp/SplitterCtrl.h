////////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 2005
// Packet Engineering, Inc. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification is not permitted unless authorized in writing by a duly
// appointed officer of Packet Engineering, Inc. or its derivatives
//
// File Name: SplitterCtrl.h
// Description:
//   
//
// Modification History:
// 
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
//	CSplitterCtrl.h
//////////////////////////////////////////////////////////////////////////////////
#if !defined(AFX_SplitterCtrl_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_)
#define AFX_SplitterCtrl_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplitterCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSplitterCtrl window
#define SPN_SIZED WM_USER + 1
#define CW_LEFTALIGN 1
#define CW_RIGHTALIGN 2
#define CW_TOPALIGN 3
#define CW_BOTTOMALIGN 4
#define SPS_VERTICAL 1
#define SPS_HORIZONTAL 2
typedef struct SPC_NMHDR
{
	NMHDR hdr;
	int delta;
} SPC_NMHDR;

class CSplitterCtrl : public CStatic
{
// Construction
public:
	CSplitterCtrl();

// Attributes
public:
protected:
	BOOL		m_bIsPressed;
	int			m_nType;
	int			m_nX, m_nY;
	int			m_nMin, m_nMax;
	int			m_nSavePos;		// Save point on the lbutton down 
								// message
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	static void ChangePos(CWnd* pWnd, int dx, int dy);
	static void ChangeWidth(CWnd* pWnd, int dx, DWORD dwFlag = CW_LEFTALIGN);
	static void ChangeHeight(CWnd* pWnd, int dy, DWORD dwFlag = CW_TOPALIGN);
public:
	void		SetRange(int nMin, int nMax);
	void		SetRange(int nSubtraction, int nAddition, int nRoot);

	int			GetStyle();
	int			SetStyle(int nStyle = SPS_VERTICAL);
	void		Create(DWORD dwStyle, const CRect& rect, CWnd* pParent, UINT nID);
	virtual		~CSplitterCtrl();

	// Generated message map functions
protected:
	virtual void	DrawLine(CDC* pDC, int x, int y);
	void			MoveWindowTo(CPoint pt);
	//{{AFX_MSG(CSplitterCtrl)
	afx_msg void	OnPaint();
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL	OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SplitterCtrl_H__FEBBA242_B2C9_4403_B68D_E519D645CB15__INCLUDED_)
