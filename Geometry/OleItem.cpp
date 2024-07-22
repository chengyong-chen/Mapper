#include "stdafx.h"
#include "Geom.h"
#include "OleObj.h"
#include "OleItem.h"

#include "../Dataview/Global.h"
#include "../Dataview/Datainfo.h"
#include "../Dataview/Viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(COleItem, COleClientItem, 0)

COleItem::COleItem(COleDocument* pContainer, COleObj* pGeom)
	: COleClientItem(pContainer)
{
	m_pOleObj = pGeom;
}

COleItem::~COleItem()
{
	if(m_pOleObj!=nullptr)
		m_pOleObj->m_pOleItem = nullptr;
}

void COleItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleClientItem::OnChange(nCode, dwParam);

	const CDocument* pDoc = (CDocument*)GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	if(pos==nullptr)
		return;

	CWnd* pWnd = pDoc->GetNextView(pos);
	CViewinfo* pViewinfo = (CViewinfo*)pWnd->SendMessage(WM_GETVIEWINFO, 0, 0);
	CViewinfo& viewinfo(*pViewinfo);
	switch(nCode)
	{
	case OLE_CHANGED_STATE:
	case OLE_CHANGED_ASPECT:
		m_pOleObj->Invalidate(pWnd, viewinfo, 1);
		break;
	case OLE_CHANGED:
		UpdateExtent(pWnd); // extent may have changed
		m_pOleObj->Invalidate(pWnd, viewinfo, 1);
		break;
	default: 
		break;
	}
}

BOOL COleItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	CWnd* pWnd = GetActiveView();
	CViewinfo* pViewinfo = (CViewinfo*)pWnd->SendMessage(WM_GETVIEWINFO, 0, 0);
	CViewinfo& viewinfo(*pViewinfo);
	const Gdiplus::Rect cliRect = viewinfo.DocToClient <Gdiplus::Rect>(rectPos);
	const CRect docRect = CRect(cliRect.GetLeft(), cliRect.GetTop(), cliRect.GetRight(), cliRect.GetBottom());
	if(docRect!=m_pOleObj->m_Rect)
	{
		m_pOleObj->Invalidate(pWnd, viewinfo, 1);
		m_pOleObj->m_Rect = docRect;
		GetExtent(&m_pOleObj->m_extent);
		m_pOleObj->Invalidate(pWnd, viewinfo, 1);
	}

	return COleClientItem::OnChangeItemPosition(rectPos);
}

void COleItem::OnGetItemPosition(CRect& rect)
{
	ASSERT_VALID(this);

	CWnd* pWnd = GetActiveView();
	if(m_pOleObj->m_Rect.IsRectEmpty())
		UpdateExtent(pWnd);

	CViewinfo* pViewinfo = (CViewinfo*)pWnd->SendMessage(WM_GETVIEWINFO, 0, 0);
	CViewinfo& viewinfo(*pViewinfo);
	rect = viewinfo.ClientToDoc(m_pOleObj->m_Rect);
}

BOOL COleItem::UpdateExtent(CWnd* pWnd)
{
	CSize size;
	if(GetExtent(&size)==FALSE)
		return FALSE;
	if(size==m_pOleObj->m_extent)
		return FALSE;

	CViewinfo* pViewinfo = (CViewinfo*)pWnd->SendMessage(WM_GETVIEWINFO, 0, 0);
	CViewinfo& viewinfo(*pViewinfo);
	if(m_pOleObj->m_extent==CSize(0, 0))
	{
		m_pOleObj->m_Rect.right = m_pOleObj->m_Rect.left+MulDiv(size.cx, 72.f, 2540)*viewinfo.m_datainfo.m_zoomPointToDoc;
		m_pOleObj->m_Rect.top = m_pOleObj->m_Rect.bottom-MulDiv(size.cx, 72.f, 2540)*viewinfo.m_datainfo.m_zoomPointToDoc;
	}
	else if(!IsInPlaceActive()&&size!=m_pOleObj->m_extent)
	{
		m_pOleObj->m_Rect.right = m_pOleObj->m_Rect.left+MulDiv(m_pOleObj->m_Rect.Width(), size.cx, m_pOleObj->m_extent.cx);
		m_pOleObj->m_Rect.top = m_pOleObj->m_Rect.bottom+MulDiv(m_pOleObj->m_Rect.Height(), size.cy, m_pOleObj->m_extent.cy);
	}

	m_pOleObj->m_extent = size;
	const CDocument* pDoc = GetDocument();
	POSITION pos = pDoc->GetFirstViewPosition();
	if(pos!=nullptr)
	{
		CWnd* pWnd1 = pDoc->GetNextView(pos);
		m_pOleObj->Invalidate(pWnd1, viewinfo, 1); // redraw to the new size/position
	}
	return TRUE;
}

void COleItem::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	COleClientItem::Serialize(ar);

	if(ar.IsStoring())
	{
	}
	else
	{
	}
}

void COleItem::OnActivate()
{
	// Allow only one inplace activate item per frame
	CWnd* pWnd = GetActiveView();
	COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pWnd);
	if(pItem!=nullptr&&pItem!=this)
	{
		pItem->Close();
	}

	COleClientItem::OnActivate();
}

void COleItem::OnDeactivateUI(BOOL bUndoable)
{
	COleClientItem::OnDeactivateUI(bUndoable);

	// Hide the object if it is not an outside-in object
	DWORD dwMisc = 0;
	m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
	if(dwMisc&OLEMISC_INSIDEOUT)
		DoVerb(OLEIVERB_HIDE, nullptr);
}

#ifdef _DEBUG
void COleItem::AssertValid() const
{
	COleClientItem::AssertValid();
}

void COleItem::Dump(CDumpContext& dc) const
{
	COleClientItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
