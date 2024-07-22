// CellList.cpp : implementation file
//

#include "stdafx.h"
#include "CellList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCellList

CCellList::CCellList()
{
	m_bLButtonDown = FALSE;
}

CCellList::~CCellList()
{
}

BEGIN_MESSAGE_MAP(CCellList, CListBox)
	//{{AFX_MSG_MAP(CCellList)
	ON_WM_KILLFOCUS()
	ON_WM_KEYUP()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCellList message handlers

void CCellList::OnKillFocus(CWnd* pNewWnd)
{
	CListBox::OnKillFocus(pNewWnd);

	DestroyWindow();
	delete this;
}

// Need to keep a lookout for Tabs, Esc and Returns. These send a 
// "KeyUp" message, but no "KeyDown". That's why I didn't put their
// code in OnKeyDown. (I will never understand windows...)
void CCellList::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_ESCAPE)
		GetParent()->SetFocus(); // This will destroy this window
	if(nChar==VK_RETURN)
	{
		EndSelect();
		GetParent()->SetFocus(); // This will destroy this window
	}
	else
		CListBox::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CCellList::EndSelect() const
{
	const int nItem = GetCurSel();

	if(nItem!=LB_ERR)
	{
		CString csItemText;
		GetText(nItem, csItemText);

		// Send Notification to parent (this was originally designed for listviews,
		// so we use listview structures and messages)

		LV_DISPINFO dispinfo;
		dispinfo.hdr.hwndFrom = GetParent()->GetParent()->GetSafeHwnd();
		dispinfo.hdr.idFrom = GetDlgCtrlID();
		dispinfo.hdr.code = LVN_ENDLABELEDIT;

		dispinfo.item.mask = LVIF_TEXT;
		dispinfo.item.iItem = m_nRow;
		dispinfo.item.iSubItem = m_nCol;
		dispinfo.item.pszText = LPTSTR((LPCTSTR)csItemText);
		dispinfo.item.cchTextMax = csItemText.GetLength();

		// Send a message to the parent of this edit's parent, telling the parent's parent
		// that the parent of this edit ctrl has recieved a LVN_ENDLABELEDIT message. 
		// Makes perfect sense, no? :)
		GetParent()->GetParent()->GetParent()->SendMessage(WM_NOTIFY, GetParent()->GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);
	}
}

void CCellList::OnLButtonUp(UINT nFlag, CPoint pt)
{
	CListBox::OnLButtonUp(nFlag, pt);

	if(m_bLButtonDown==TRUE)
	{
		m_bLButtonDown = FALSE;

		EndSelect();
		GetParent()->SetFocus();
	}
	else
		m_bLButtonDown = FALSE;
}

void CCellList::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bLButtonDown = TRUE;
	CListBox::OnLButtonDown(nFlags, point);
}
