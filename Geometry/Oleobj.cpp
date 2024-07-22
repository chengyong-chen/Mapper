#include "stdafx.h"

#include "Geom.h"
#include "OleObj.h"
#include "OleItem.h"
#include "../Public/Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(COleObj, CGeom, 0)

COleObj::COleObj()
	: CGeom()
{
	m_extent = CSize(0, 0);
	m_pOleItem = nullptr;

	m_bType = 6;
}

COleObj::COleObj(const CRect& rect)
	: CGeom(rect, nullptr, nullptr)
{
	m_extent = rect.Size();
	m_pOleItem = nullptr;

	m_bType = 6;
}

COleObj::~COleObj()
{
	if(m_pOleItem!=nullptr)
	{
		m_pOleItem->Release();
		m_pOleItem = nullptr;
	}
}
void COleObj::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CGeom::Sha1(sha1);

	sha1.process_bytes(&m_Rect.left, sizeof(int));
	sha1.process_bytes(&m_Rect.top, sizeof(int));
	sha1.process_bytes(&m_Rect.right, sizeof(int));
	sha1.process_bytes(&m_Rect.bottom, sizeof(int));
	if(m_pOleItem!=nullptr)
	{
		//	m_pOleItem->Sha1(sha1);
	}
}
void COleObj::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	LONG  left = m_Rect.left-offset.cx;
	LONG  top = m_Rect.top-offset.cy;
	LONG  rgiht = m_Rect.right-offset.cx;
	LONG  bottom = m_Rect.bottom-offset.cy;

	sha1.process_bytes(&left, sizeof(int));
	sha1.process_bytes(&top, sizeof(int));
	sha1.process_bytes(&right, sizeof(int));
	sha1.process_bytes(&bottom, sizeof(int));
	if(m_pOleItem!=nullptr)
	{
		//	m_pOleItem->Sha1(sha1);
	}
}
BOOL COleObj::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(COleObj))==FALSE)
		return FALSE;

	return FALSE;
}

void COleObj::Open(CWnd* pWnd) const
{
	if(pWnd->IsKindOf(RUNTIME_CLASS(CView))==TRUE)
	{
		AfxGetApp()->BeginWaitCursor();

		CView* pView = (CView*)pWnd;
		m_pOleItem->DoVerb(GetKeyState(VK_CONTROL)<0 ? OLEIVERB_OPEN : OLEIVERB_PRIMARY, pView);

		AfxGetApp()->EndWaitCursor();
	}
}

void COleObj::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_extent;
		ar<<m_pOleItem;
	}
	else
	{
		ar>>m_extent;
		ar>>m_pOleItem;
		m_pOleItem->m_pOleObj = this;
	}
}

void COleObj::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(COleObj))==TRUE)
	{
		COleObj* pOleObj = (COleObj*)pGeom;

		COleItem* pItem = nullptr;
		try
		{
			COleItem* pItem = new COleItem(nullptr, pOleObj);
			if(!pItem->CreateCloneFrom(m_pOleItem))
				AfxThrowMemoryException();
			pOleObj->m_pOleItem = pItem;
			ASSERT_VALID(pOleObj);
		}
		catch(CException*ex)
		{
			//			OutputDebugString(ex->GetErrorMessage());
			pItem->Delete();
			pOleObj->m_pOleItem = nullptr;
			THROW_LAST();
		}
	}
}

void COleObj::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(COleObj))==TRUE)
	{
		COleObj* pOleObj = (COleObj*)pGeom;

		Swapclass<COleItem*>(pOleObj->m_pOleItem, m_pOleItem);
	}

	CGeom::Swap(pGeom);
}

void COleObj::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	/*commented out after deleted the decimal
	HDC hDC = g.GetHDC();

	CDC* pDC = nullptr;
	UINT oldTextAlign = pDC->SetTextAlign(TA_TOP);
	if(m_pOleItem != nullptr)
	{
		CPoint lefttop = CPoint(m_Rect.left,m_Rect.bottom);

		pDC->SaveDC();
		CPoint Org = pDC->GetWindowOrg();
		Org.Offset(-lefttop);
		Org.y = -Org.y;
		pDC->SetWindowOrg(Org);
		pDC->SetWindowExt(72*nDecimal,72*nDecimal);

		CRect rect = m_Rect;
		rect.OffsetRect(-rect.TopLeft());
		rect.NormalizeRect();
		m_pOleItem->Draw(pDC, rect);
		pDC->RestoreDC(-1);

//		CRect rect = m_Rect;
//		int  x = rect.bottom;
//		rect.bottom = rect.top;
//		rect.top = rect.bottom;
//		m_pOleItem->Draw(pDC, rect);

		// don't draw tracker in print preview or on printer
		if(pDC->IsPrinting() == FALSE)
		{
			CRectTracker tracker;
			tracker.m_rect = m_Rect;
			CWnd* pWnd = pDC->GetWindow();
			if(pWnd != nullptr)
			{
				Rect rect = viewinfo.DocToClient(tracker.m_rect);
				tracker.m_rect = CRect(rect.GetLeft(),rect.GetTop(),rect.GetRight(),rect.GetBottom());
				if(m_pOleItem->Gettype() == OT_LINK)
					tracker.m_nStyle |= CRectTracker::dottedLine;
				else
					tracker.m_nStyle |= CRectTracker::solidLine;
				if(m_pOleItem->GetItemState() == COleClientItem::openState || m_pOleItem->GetItemState() == COleClientItem::activeUIState)
					tracker.m_nStyle |= CRectTracker::hatchInside;
				tracker.Draw(pDC);
			}
		}
	}

	g.ReleaseHDC(hDC);

	*/
}

void COleObj::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	return COleObj::Draw(g, viewinfo, 1);
}
void COleObj::Move(const int& dx, const int& dy)
{
	ASSERT_VALID(this);

	if(dx == 0 && dy == 0)
		return;

	// call base class to update position
	CGeom::Move(dx, dy);

	// update position of in-place editing session on position change
	if(m_pOleItem->IsInPlaceActive())
		m_pOleItem->SetItemRects();
}
void COleObj::Move(const CSize& Δ)
{
	ASSERT_VALID(this);

	if(Δ==CSize(0, 0))
		return;

	// call base class to update position
	CGeom::Move(Δ);

	// update position of in-place editing session on position change
	if(m_pOleItem->IsInPlaceActive())
		m_pOleItem->SetItemRects();
}
