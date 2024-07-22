// CellEdit.cpp : implementation file
//
// Written by Chris Maunder (Chris.Maunder@cbr.clw.csiro.au)
// Copyright (c) 1998.
//
// The code contained in this file is based on the original
// CCellEdit from http://www.codeguru.com/listview/edit_subitems.shtml
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CellEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCellEdit

CCellEdit::CCellEdit()
{
}

CCellEdit::CCellEdit(int iItem, int iSubItem, CString sInitText)
{
	m_iItem = iItem;
	m_iSubItem = iSubItem;
	m_bESC = FALSE;

	m_sInitText = sInitText;
}

CCellEdit::~CCellEdit()
{
}

BEGIN_MESSAGE_MAP(CCellEdit, CEdit)
	//{{AFX_MSG_MAP(CCellEdit)
	ON_WM_KILLFOCUS()
	ON_WM_NCDESTROY()
	ON_WM_CHAR()
	ON_WM_CREATE()
	ON_WM_KEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////////////////
// CCellEdit message handlers

int CCellEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if(CEdit::OnCreate(lpCreateStruct)==-1)
		return -1;

	// Set the proper font
	CFont* font = GetParent()->GetFont();
	SetFont(font);

	SetWindowText(m_sInitText);
	SetFocus();
	SetSel(0, -1);
	return 0;
}

// Need to keep a lookout for Tabs, Esc and Returns. These send a 
// "KeyUp" message, but no "KeyDown". That's why I didn't put their
// code in OnKeyDown. (I will never understand windows...)
void CCellEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_TAB||nChar==VK_RETURN)
		GetParent()->SetFocus(); // This will destroy this window
	else if(nChar==VK_ESCAPE)
	{
		SetWindowText(m_sInitText); // restore previous text
		GetParent()->SetFocus();
	}
	else
		CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

BOOL CCellEdit::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message==WM_KEYDOWN)
	{
		if(pMsg->wParam==VK_RETURN||pMsg->wParam==VK_DELETE||pMsg->wParam==VK_ESCAPE||GetKeyState(VK_CONTROL))
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE; // DO NOT process further
		}
	}

	return CEdit::PreTranslateMessage(pMsg);
}

// As soon as this edit loses focus, kill it.
void CCellEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	CString str;
	GetWindowText(str);

	// Send Notification to parent of ListView ctrl
	LV_DISPINFO dispinfo;
	dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
	dispinfo.hdr.idFrom = GetDlgCtrlID();
	dispinfo.hdr.code = LVN_ENDLABELEDIT;

	dispinfo.item.mask = LVIF_TEXT;
	dispinfo.item.iItem = m_iItem;
	dispinfo.item.iSubItem = m_iSubItem;
	dispinfo.item.pszText = m_bESC ? nullptr : LPTSTR((LPCTSTR)str);
	dispinfo.item.cchTextMax = str.GetLength();

	GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

	DestroyWindow();
}

void CCellEdit::OnNcDestroy()
{
	CEdit::OnNcDestroy();
	delete this;
}

void CCellEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE||nChar==VK_RETURN)
	{
		if(nChar==VK_ESCAPE)
			m_bESC = TRUE;
		GetParent()->SetFocus();
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);

	// Resize edit control if needed

	// Get text extent
	CString str;
	GetWindowText(str);

	CWindowDC dc(this);
	CFont* pFontDC = dc.SelectObject(GetFont());
	CSize size = dc.GetTextExtent(str);
	dc.SelectObject(pFontDC);

	size.cx += 5; // add some extra buffer

	// Get client rect
	CRect rect, parentrect;
	GetClientRect(&rect);
	GetParent()->GetClientRect(&parentrect);

	// Transform rect to parent coordinates
	ClientToScreen(&rect);
	GetParent()->ScreenToClient(&rect);

	// Check whether control needs to be resized
	// and whether there is space to grow
	if(size.cx>rect.Width())
	{
		if(size.cx+rect.left<parentrect.right)
			rect.right = rect.left+size.cx;
		else
			rect.right = parentrect.right;

		MoveWindow(&rect);
	}
}
