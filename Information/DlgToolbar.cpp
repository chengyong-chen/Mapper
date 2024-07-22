// ToolBar1.cpp : implementation file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1998 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

#include "stdafx.h"
#include "Resource.h"
#include "DlgToolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgToolbar

CDlgToolbar::CDlgToolbar() : m_pTBButtons(nullptr)
{

}

CDlgToolbar::~CDlgToolbar()
{
	delete [] m_pTBButtons;
}


BEGIN_MESSAGE_MAP(CDlgToolbar, CToolBarCtrl)
	//{{AFX_MSG_MAP(CDlgToolbar)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTA, ID_GRID_DBCOMBO, ID_GRID_GEOM, OnNeedTextA)
	ON_NOTIFY_RANGE(TTN_NEEDTEXTW, ID_GRID_DBCOMBO, ID_GRID_GEOM, OnNeedTextW)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgToolbar message handlers

// MFC routes the notifications sent to the parent of the control to
// the control before the parent can process the notification.
// The control object can handle the notification or ignore it.
// If the notification is handled then return TRUE. Otherwise MFC
// will route it to the parent of the control.



// Helper function for tooltips

CString CDlgToolbar::NeedText(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult)
{
	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct ;
	ASSERT(nId == lpTTT->hdr.idFrom);

	CString toolTipText;
	toolTipText.LoadString(nId);

	// szText length is 80
	int nLength = (toolTipText.GetLength() > 79) ? 79 : toolTipText.GetLength();

	toolTipText = toolTipText.Left(nLength);

	return toolTipText;
}


void CDlgToolbar::OnNeedTextW(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult)
{
	CString toolTipText = NeedText(nId, pNotifyStruct, lResult);

	LPTOOLTIPTEXTW lpTTT = (LPTOOLTIPTEXTW)pNotifyStruct;

#ifndef _UNICODE
	mbstowcs(lpTTT->szText,(LPCSTR)toolTipText, toolTipText.GetLength() + 1);
#else
	_tcsncpy(lpTTT->szText, toolTipText, toolTipText.GetLength() + 1);
#endif
}

void CDlgToolbar::OnNeedTextA(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult)
{
	CString toolTipText = NeedText(nId, pNotifyStruct, lResult);

	LPTOOLTIPTEXT lpTTT = (LPTOOLTIPTEXT)pNotifyStruct;

	_tcscpy(lpTTT->szText,(LPCTSTR)toolTipText);
}


