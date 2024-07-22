#include "stdafx.h"
#include "EditGeom.h"
#include "Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/TextPoly.h"
#include "../Layer/Layer.h"

#include "../Mapper/Global.h"
#include "../Action/Remove.h"
#include "../Action/Modify.h"
#include "../Action/Document/EditGeom.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Poly.h"

#include "../Dataview/viewinfo.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Bezier.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CEditGeom::CEditGeom(const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
	: m_Viewinfo(viewinfo), m_actionstack(actionstack)
{
	m_pLayer = nullptr;
	m_pGeom = nullptr;

	m_Handle.first = 0;
	m_Handle.second = 0;
}

void CEditGeom::AddAnchor(CWnd* pWnd, const CPoint& point, bool bTail)
{
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;
	else if(m_pGeom->IsKindOf(RUNTIME_CLASS(CPoly))==FALSE)
		return;

	this->UnFocusAnchors(pWnd);
	CPoly* pPoly = (CPoly*)m_pGeom;
	if(pPoly->AddAnchor(point, bTail)==true)
	{
		const std::pair<CPoint, bool> anchor = std::pair<CPoint, bool>(point, bTail);
		Undoredo::CAppend<CWnd*, CPoly*, std::pair<CPoint, bool>>* pAppend = new Undoredo::CAppend<CWnd*, CPoly*, std::pair<CPoint, bool>>(pWnd, pPoly, anchor);
		pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Append_Anchor;
		pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Append_Anchor;
		m_actionstack.Record(pAppend);
	}
}

void CEditGeom::AddAnchor(CWnd* pWnd, const CPoint points[], bool bTail)
{
	if(m_pLayer==nullptr)
		return;
	else if(m_pGeom==nullptr)
		return;
	else if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==FALSE)
		return;

	this->UnFocusAnchors(pWnd);
	CBezier* pBezier = (CBezier*)m_pGeom;

	if(pBezier->AddAnchor(points, bTail)==true)
	{
		std::list<CPoint> pointlist;
		pointlist.push_back(points[0]);
		pointlist.push_back(points[1]);
		pointlist.push_back(points[2]);
		const std::pair<std::list<CPoint>, bool> anchor = std::pair<std::list<CPoint>, bool>(pointlist, bTail);
		Undoredo::CAppend<CWnd*, CBezier*, std::pair<std::list<CPoint>, bool>>* pAppend = new Undoredo::CAppend<CWnd*, CBezier*, std::pair<std::list<CPoint>, bool>>(pWnd, pBezier, anchor);
		pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Undo_Append_Anchor;
		pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Redo_Append_Anchor;
		m_actionstack.Record(pAppend);
	}
}
void CEditGeom::MoveAnchor(CWnd* pWnd, const CSize& Δ, bool bMatch)
{
	if(m_Anchors.size()==0)
		return;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->MoveAnchor(&dc, m_Viewinfo, nAnchor, Δ, bMatch);
	}
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CEditGeom::MoveContrl(CDC* pDC, const CSize& Δ) const
{
	if(m_pGeom!=nullptr)
		m_pGeom->MoveContrl(pDC, m_Viewinfo, m_Handle, Δ);
}

void CEditGeom::ChangeAnchor(CWnd* pWnd, CDC* pDC, const CSize& Δ, bool bMatch)
{
	if(Δ==CSize(0, 0))
		return;
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;
	if(m_Anchors.size()==0)
		return;

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
	}

	m_actionstack.Start();

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->ChangeAnchor(nAnchor, Δ, bMatch, &m_actionstack);
	}

	if(m_pGeom->m_bClosed==false)
	{
		for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
		{
			unsigned int nAnchor = *it;
			if(nAnchor==1||nAnchor==m_pGeom->GetAnchors())
			{
				const CSize tolerence = m_Viewinfo.ClientToDoc(Gdiplus::SizeF(2, 2));
				if(m_pGeom->Close(false, tolerence.cx, &m_actionstack)==true)
				{
					m_Anchors.clear();
					m_pLayer->Invalidate(pWnd, m_Viewinfo, m_pGeom);
					m_pLayer = nullptr;
					m_pGeom = nullptr;
					break;
				}
			}
			m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
		}
	}
	else
	{
		m_pLayer->Invalidate(pWnd, m_Viewinfo, m_pGeom);
		//		UnFocusAll(pWnd);
	}

	m_actionstack.Stop();

	pWnd->SendMessage(WM_DOCMODIFIED, 0, true);
}
void CEditGeom::ChangeContrl(CWnd* pWnd, CDC* pDC, const CSize& Δ)
{
	if(Δ==CSize(0, 0))
		return;
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;

	m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, m_Handle.first);
	m_pGeom->ChangeContrl(m_Handle, Δ);

	Undoredo::CModify<CWnd*, std::pair<CGeom*, std::pair<unsigned int, BYTE>>, CSize>* pModify = new Undoredo::CModify<CWnd*, std::pair<CGeom*, std::pair<unsigned int, BYTE>>, CSize>(nullptr, std::pair<CGeom*, std::pair<unsigned int, BYTE>>(m_pGeom, m_Handle), -Δ, Δ);
	pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Undo_Modify_Control;
	pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Redo_Modify_Control;
	m_actionstack.Record(pModify);

	m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, m_Handle.first);

	pWnd->SendMessage(WM_DOCMODIFIED, 0, true);
}

void CEditGeom::FocusAnchor(CWnd* pWnd, unsigned int nAnchor, bool bAdd)
{
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	if(!bAdd)
	{
		SwitchAnchorsNormal(&dc);
		m_Anchors.clear();
	}

	m_Anchors.insert(nAnchor);
	m_Handle.first = nAnchor;

	m_pGeom->DrawAnchor(&dc, m_Viewinfo, nAnchor);
	m_pGeom->DrawPentip(&dc, m_Viewinfo, nAnchor);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CEditGeom::UnFocusAll(CWnd* pWnd)
{
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	SwitchAnchorsNormal(&dc);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);

	m_Anchors.clear();

	m_pGeom = nullptr;
	m_Mode = Mode::Nothing;
}

void CEditGeom::UnFocusAnchors(CWnd* pWnd)
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	SwitchAnchorsNormal(&dc);
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
	m_Anchors.clear();
}

void CEditGeom::SwitchAnchorsFocused(CDC* pDC) const
{
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;
	else if(m_Anchors.size()<1)
		return;
	const CPoint oldOrg = pDC->SetViewportOrg(0, 0);
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->DrawAnchor(pDC, m_Viewinfo, nAnchor);
		m_pGeom->DrawPentip(pDC, m_Viewinfo, nAnchor);
	}

	pDC->SetViewportOrg(oldOrg);
	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CEditGeom::SwitchAnchorsNormal(CDC* pDC) const
{
	if(m_pLayer==nullptr)
		return;
	if(m_pGeom==nullptr)
		return;
	if(m_Anchors.size()<1)
		return;

	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->DrawPentip(pDC, m_Viewinfo, nAnchor);
		m_pGeom->DrawAnchor(pDC, m_Viewinfo, nAnchor);
	}

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

bool CEditGeom::Clear(CWnd* pWnd)
{
	if(m_pLayer==nullptr)
		return false;
	if(m_pGeom==nullptr)
		return false;
	if(m_Anchors.size()<1)
		return false;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	SwitchAnchorsNormal(&dc);
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
	if(m_pGeom->GetAnchors()-m_Anchors.size()<2)
	{
		pWnd->SendMessage(WM_REMOVEGEOM, 0, (DWORD)m_pGeom);
	}
	else
	{
		for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
		{
			unsigned int nAnchor = *it;
			m_pGeom->DrawAnchor(&dc, m_Viewinfo, nAnchor);
			m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
		}

		std::map<unsigned int, CPoint> anchors;
		for(std::set<unsigned int>::reverse_iterator rit = m_Anchors.rbegin(); rit!=m_Anchors.rend(); rit++)
		{
			unsigned int nAnchor = *rit;
			anchors[nAnchor] = m_pGeom->GetAnchor(nAnchor);

			m_pGeom->RemoveAnchor(nAnchor);
		}

		Undoredo::CRemove<CWnd*, CGeom*, std::map<unsigned int, CPoint>>* pRemove = new Undoredo::CRemove<CWnd*, CGeom*, std::map<unsigned int, CPoint>>(pWnd, m_pGeom, anchors);
		pRemove->undo = Undoredo::Document::EditGeom::Undo_Remove_Anchors;
		pRemove->redo = Undoredo::Document::EditGeom::Redo_Remove_Anchors;
		m_actionstack.Record(pRemove);

		unsigned last = -1;
		int removed = 0;
		for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
		{
			const unsigned int nAnchor = *it-removed++;

			if(last==-1)
				last = nAnchor-1;
			if(nAnchor!=last+1)
			{
				m_pLayer->InvalidateSegment(pWnd, m_Viewinfo, m_pGeom, last);
				last = nAnchor-1;
			}
		}
		m_pLayer->InvalidateSegment(pWnd, m_Viewinfo, m_pGeom, last);
	}

	m_Anchors.clear();
	m_pGeom = nullptr;

	pWnd->SendMessage(WM_DOCMODIFIED, 0, true);
	return true;
}

bool CEditGeom::Corner(CWnd* pWnd)
{
	if(m_pLayer==nullptr)
		return false;
	if(m_pGeom==nullptr)
		return false;
	if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==FALSE)
		return false;

	if(m_Anchors.size()<1)
		return false;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	SwitchAnchorsNormal(&dc);
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		const unsigned int nAnchor = *it;
		const CBezier* bezier = (CBezier*)m_pGeom;
		bezier->m_pPoints[(nAnchor-1)*3] = bezier->m_pPoints[(nAnchor-1)*3+1];
		bezier->m_pPoints[(nAnchor-1)*3+2] = bezier->m_pPoints[(nAnchor-1)*3+1];
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->DrawPentip(&dc, m_Viewinfo, nAnchor);
		m_pGeom->DrawAnchor(&dc, m_Viewinfo, nAnchor);
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, true);

	return true;
}

bool CEditGeom::Cornnect(CWnd* pWnd)
{
	if(m_pLayer==nullptr)
		return false;
	if(m_pGeom==nullptr)
		return false;

	if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==FALSE)
		return false;

	if(m_Anchors.size()<1)
		return false;

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	SwitchAnchorsNormal(&dc);
	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		const unsigned int nAnchor = *it;
		const CBezier* bezier = (CBezier*)m_pGeom;
		bezier->m_pPoints[(nAnchor-1)*3] = bezier->m_pPoints[(nAnchor-1)*3+1];
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pLayer->InvalidateAnchor(pWnd, m_Viewinfo, m_pGeom, nAnchor);
	}

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		unsigned int nAnchor = *it;
		m_pGeom->DrawPentip(&dc, m_Viewinfo, nAnchor);
		m_pGeom->DrawAnchor(&dc, m_Viewinfo, nAnchor);
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, true);

	return true;
}

bool CEditGeom::IsBezierNull()
{
	if(m_Anchors.size()==0)
		m_pGeom = nullptr;
	if(m_pLayer==nullptr)
		return false;
	if(m_pGeom==nullptr)
		return false;

	if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==FALSE)
		return true;

	if(m_pGeom->m_bClosed==true)
		return true;

	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		const unsigned int nAnchor = *it;
		if(nAnchor==1||nAnchor==((CBezier*)m_pGeom)->m_nAnchors)
			return false;
	}
	return true;
}

bool CEditGeom::IsTextPolyNull()
{
	if(m_Anchors.size()==0)
		m_pGeom = nullptr;
	if(m_pLayer==nullptr)
		return false;
	else if(m_pGeom==nullptr)
		return false;
	else if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==TRUE)
		return true;
	else if(m_pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly))==FALSE)
		return true;
	else if(m_pGeom->m_bClosed==true)
		return true;
	else if(m_Anchors.size()==0)
		return true;
	else if(m_Anchors.size()>1)
		return true;
	else if(*(m_Anchors.begin())==1)
		return false;
	else if(*(m_Anchors.begin())==((CPoly*)m_pGeom)->m_nAnchors)
		return false;
	else
		return true;
}

void CEditGeom::NewGeom(CWnd* pWnd, CGeom* pGeom)
{
	UnFocusAll(pWnd);

	//	m_pLayer = pLayer;
	m_pGeom = pGeom;
}

bool CEditGeom::PickControl(CWnd* pWnd, const CPoint& docPoint)
{
	if(m_pLayer==nullptr)
		return false;
	if(m_pGeom==nullptr)
		return false;

	if(m_pGeom->IsKindOf(RUNTIME_CLASS(CBezier))==FALSE)
		return false;
	const Gdiplus::Point cliPoint = m_Viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	const CBezier* bezier = (CBezier*)m_pGeom;
	for(std::set<unsigned int>::iterator it = m_Anchors.begin(); it!=m_Anchors.end(); it++)
	{
		const unsigned int nAnchor = *it;
		Gdiplus::Point local1 = m_Viewinfo.DocToClient<Gdiplus::Point>(bezier->m_pPoints[(nAnchor-1)*3]);

		Gdiplus::Rect rect1 = Gdiplus::Rect(local1, Gdiplus::Size(0, 0));
		rect1.Inflate(3, 3);
		if(rect1.Contains(cliPoint))
		{
			m_Handle.first = nAnchor;
			m_Handle.second = -1;
			return true;
		}

		Gdiplus::Point local2 = m_Viewinfo.DocToClient<Gdiplus::Point>(bezier->m_pPoints[(nAnchor-1)*3+2]);
		Gdiplus::Rect rect2 = Gdiplus::Rect(local2, Gdiplus::Size(0, 0));
		rect2.Inflate(3, 3);
		if(rect2.Contains(cliPoint))
		{
			m_Handle.first = nAnchor;
			m_Handle.second = 1;
			return true;
		}
	}

	return false;
}
