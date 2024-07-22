// ListCtrlEx.cpp : implementation file
//

#include "stdafx.h"
#include "ListCtrlEx.h"
#include "CellEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx

CListCtrlEx::CListCtrlEx()
{
	m_bEditble = false;
}

CListCtrlEx::~CListCtrlEx()
{
}

BEGIN_MESSAGE_MAP(CListCtrlEx, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlEx message handlers

void CListCtrlEx::PreSubclassWindow()
{
	CListCtrl::PreSubclassWindow();

	EnableToolTips(TRUE);
}

int CListCtrlEx::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	int col;
	RECT cellrect;
	const int row = CellRectFromPoint(point, &cellrect, &col);
	if(row==-1)
		return -1;

	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	pTI->rect = cellrect;

	return pTI->uId;
}

// CellRectFromPoint	- Determine the row, col and bounding rect of a cell
// Returns		- row index on success, -1 otherwise
// point		- point to be tested.
// cellrect		- to hold the bounding rect
// col			- to hold the column index
int CListCtrlEx::CellRectFromPoint(const CPoint& point, RECT* cellrect, int* col) const
{
	// Make sure that the ListView is in LVS_REPORT
	if((GetWindowLong(m_hWnd, GWL_STYLE)&LVS_TYPEMASK)!=LVS_REPORT)
		return -1;

	// Get the top and bottom row visible
	int row = GetTopIndex();
	int bottom = row+GetCountPerPage();
	if(bottom>GetItemCount())
		bottom = GetItemCount();

	// Get the number of columns
	const CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	const int nColumnCount = pHeader->GetItemCount();
	for(; row<=bottom; row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;
		GetItemRect(row, &rect, LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			for(int colnum = 0; colnum<nColumnCount; colnum++)
			{
				const int colindex = pHeader->OrderToIndex(colnum);
				const int colwidth = GetColumnWidth(colindex);
				if(point.x>=rect.left&&point.x<=(rect.left+colwidth))
				{
					RECT rectClient;
					GetClientRect(&rectClient);
					if(col)
						*col = colindex;
					rect.right = rect.left+colwidth;

					// Make sure that the right extent does not exceed
					// the client area
					if(rect.right>rectClient.right)
						rect.right = rectClient.right;
					*cellrect = rect;
					return row;
				}
				rect.left += colwidth;
			}
		}
	}
	return -1;
}

BOOL CListCtrlEx::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	const UINT nId = pNMHDR->idFrom;
	if(nId==0) // Notification in NT from automatically
		return FALSE; // created tooltip
	const int row = ((nId-1)>>10)&0x3fffff;
	const int col = (nId-1)&0x3ff;
	const CString strTagText = GetItemText(row, col);

#ifndef _UNICODE
	if(pNMHDR->code==TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTagText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTagText, 80);
#else
	if(pNMHDR->code==TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTagText, 80);
	else
		lstrcpyn(pTTTW->szText, strTagText, 80);
#endif
	* pResult = 0;

	return TRUE; // message was handled
}

void CListCtrlEx::OnPaint()
{
	const MSG* msg = GetCurrentMessage();
	DefWindowProc(msg->message, msg->wParam, msg->lParam);
	const DWORD dwStyle = GetStyle();
	if((dwStyle&LVS_TYPEMASK)!=LVS_REPORT)
		return;

	if(GetItemCount()<1)
		return;
	const int topItem = GetTopIndex();
	const int bottomItem = topItem+GetCountPerPage()>GetItemCount() ? GetItemCount() : topItem+GetCountPerPage();

	const CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	const long nColumnCount = pHeader->GetItemCount();

	CRect rcBounds;
	if(!GetItemRect(0, &rcBounds, LVIR_BOUNDS))
		return;
	const long height = rcBounds.bottom-rcBounds.top;

	CRect rcClient;
	pHeader->GetClientRect(&rcClient);
	rcClient.top = rcClient.bottom;
	rcClient.bottom = rcClient.top+(bottomItem-topItem)*height;

	CClientDC dc(this);
	CPen pen(PS_SOLID, 1, RGB(200, 200, 200));
	CPen* oldpen = (CPen*)dc.SelectObject(&pen);

	long borderx = 0-GetScrollPos(SB_HORZ);
	for(long i = 0; i<nColumnCount; i++)
	{
		borderx += GetColumnWidth(i);

		if(borderx>=rcClient.right)
			break;

		dc.MoveTo(borderx-1, rcClient.top);
		dc.LineTo(borderx-1, rcClient.bottom);
	}

	for(long i = topItem; i<bottomItem; i++)
	{
		dc.MoveTo(0, rcClient.top+height*(i+1-topItem));
		dc.LineTo(rcClient.right, rcClient.top+height*(i+1-topItem));
	}
	dc.SelectObject(oldpen);
	pen.DeleteObject();
}

void CListCtrlEx::OnLButtonDown(UINT nFlags, CPoint point)
{
	int colnum;
	const int row = HitTestEx(point, &colnum);
	if(row!=-1)
	{
		if((GetItemState(row, LVIS_FOCUSED)&LVIS_FOCUSED)==LVIS_FOCUSED&&colnum>0)
		{
			if(m_bEditble==true)//if(GetWindowLong(m_hWnd, GWL_STYLE) & LVS_EDITLABELS )// Add check for LVS_EDITLABELS
			{
				// Send Notification to parent (this was originally designed for listviews,
				// so we use listview structures and messages)

				LV_DISPINFO dispinfo;
				dispinfo.hdr.hwndFrom = GetParent()->GetSafeHwnd();
				dispinfo.hdr.idFrom = GetDlgCtrlID();
				dispinfo.hdr.code = LVN_BEGINLABELEDIT;

				dispinfo.item.mask = LVIF_TEXT;
				dispinfo.item.iItem = row;
				dispinfo.item.iSubItem = colnum;

				const CWnd* pParent1 = GetParent();
				const CWnd* pParent2 = pParent1->GetParent();
				CWnd* pParent3 = pParent2->GetParent();
				const DWORD result = pParent2->SendMessage(WM_NOTIFY, pParent1->GetDlgCtrlID(), (LPARAM)&dispinfo);
				if(result==FALSE)
				{
					this->EditSubLabel(row, colnum);
				}
				return;
			}
		}
		else
		{
			SetItemState(row, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		}
	}

	CListCtrl::OnLButtonDown(nFlags, point);
}

void CListCtrlEx::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_DISPINFO* plvDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* plvItem = &plvDispInfo->item;

	if(plvItem->pszText!=nullptr)
	{
		SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
	}

	*pResult = FALSE;
}

void CListCtrlEx::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(GetFocus()!=this)
		SetFocus();

	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CListCtrlEx::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(GetFocus()!=this)
		SetFocus();

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}

int CListCtrlEx::HitTestEx(const CPoint& point, int* col) const
{
	if(col!=nullptr)
		*col = 0;

	// Make sure that the ListView is in LVS_REPORT
	if((GetWindowLong(m_hWnd, GWL_STYLE)&LVS_TYPEMASK)!=LVS_REPORT)
	{
		return HitTest(point, nullptr);
	}

	// Get the top and bottom row visible
	const int topRow = GetTopIndex();
	int bottomRow = topRow+GetCountPerPage();
	if(bottomRow>GetItemCount())
		bottomRow = GetItemCount();

	// Get the number of columns

	// Loop through the visible rows
	for(int row = topRow; row<=bottomRow; row++)
	{
		CRect rect;// Get bounding rect of item and check whether point falls in it.
		GetItemRect(row, &rect, LVIR_BOUNDS);
		if(rect.PtInRect(point))
		{
			// Now find the column
			const CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
			const int nColumnCount = pHeader->GetItemCount();
			for(int colnum = 0; colnum<nColumnCount; colnum++)
			{
				const int colwidth = GetColumnWidth(colnum);
				if(point.x>=rect.left&&point.x<=(rect.left+colwidth))
				{
					if(col!=nullptr)
					{
						*col = colnum;
					}

					return row;
				}
				rect.left += colwidth;
			}
		}
	}

	return -1;
}

CEdit* CListCtrlEx::EditSubLabel(int nItem, int nCol)
{
	if(EnsureVisible(nItem, TRUE)==FALSE)
		return nullptr;

	const CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
	const int nColumnCount = pHeader->GetItemCount();
	if(nCol>=nColumnCount||GetColumnWidth(nCol)<5)
		return nullptr;

	// Get the column offset
	int offset = 0;
	for(int i = 0; i<nCol; i++)
	{
		offset += GetColumnWidth(i);
	}

	CRect rect;
	GetItemRect(nItem, &rect, LVIR_BOUNDS);

	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset+rect.left < 0||offset+rect.left > rcClient.right)
	{
		CSize size;
		size.cx = offset+rect.left;
		size.cy = 0;
		Scroll(size);
		rect.left -= size.cx;
	}

	// Get Column alignment
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);
	DWORD dwStyle;
	if((lvcol.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else dwStyle = ES_CENTER;

	rect.left += offset+4;
	rect.right = rect.left+GetColumnWidth(nCol)-3;
	if(rect.right>rcClient.right)
		rect.right = rcClient.right;

	dwStyle |= WS_BORDER|WS_CHILD|WS_VISIBLE|ES_AUTOHSCROLL;

	CEdit* pEdit = new CCellEdit(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create(dwStyle, rect, this, IDC_IPEDIT);
	return pEdit;
}
