#include "stdafx.h"
#include "Resource.h"
#include "Global.h"

#include "Tool.h"
#include "PolyTool.h"
#include "EditGeom.h"

#include "../Geometry/Global.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

__declspec(dllexport) CPolyTool polyTool;

void CPolyTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(m_pEditgeom->m_Mode==CEditGeom::Mode::FirstPolying)
		m_pEditgeom->m_Mode = CEditGeom::Mode::SecondPolying;
	else if(m_pEditgeom->m_pGeom==nullptr)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstPolying;
	else if(m_pEditgeom->m_pGeom->Gettype()!=1&&m_pEditgeom->m_pGeom->Gettype()!=10)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstPolying;
	else if(m_pEditgeom->m_Anchors.size()>1)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstPolying;
	else if(m_pEditgeom->m_Anchors.size()==0)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstPolying;
	else if(*(m_pEditgeom->m_Anchors.begin())==m_pEditgeom->m_pGeom->GetAnchors())
	{
		m_PrevPoint = m_pEditgeom->m_pGeom->GetAnchor(m_pEditgeom->m_pGeom->GetAnchors());
		m_pEditgeom->m_Mode = CEditGeom::Mode::Tailing;
	}
	else if(*(m_pEditgeom->m_Anchors.begin())==1)
	{
		m_NextPoint = m_pEditgeom->m_pGeom->GetAnchor(1);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Heading;
	}
	else
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstPolying;

	if(m_pEditgeom->m_Mode==CEditGeom::Mode::FirstPolying)
	{
		m_pEditgeom->UnFocusAll(pWnd);

		CGeom* pGeom = (CGeom*)pWnd->SendMessage(WM_PICKACTIVEGEOM, (LONG)0, (LONG)&docPoint);
		if(pGeom!=nullptr&&pGeom->Gettype()==1)
		{
			CPoly* pPoly = (CPoly*)pGeom;
			const CSize nInflate = viewinfo.ClientToDoc(Gdiplus::SizeF(3, 3));
			double t = 0;
			const int anchor = pPoly->PickOn(docPoint, nInflate.cx, t);
			if(t>0&&t<1)
			{
				m_PrevAnchor = anchor;
				m_PrevPoint = pPoly->GetAnchor(m_PrevAnchor);
				m_NextPoint = pPoly->GetAnchor(m_PrevAnchor+1);
				m_pEditgeom->NewGeom(pWnd, pPoly);
				m_pEditgeom->m_Mode = CEditGeom::Mode::Inserting;
			}
		}
		else
		{
			pWnd->SendMessage(WM_COMMAND, 33068, 0);
		}
	}

	m_pEditgeom->UnFocusAnchors(pWnd);
	switch(m_pEditgeom->m_Mode)
	{
	case CEditGeom::Mode::FirstPolying:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	case CEditGeom::Mode::SecondPolying:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, m_PrevPoint);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_PrevPoint);
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, point);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
	}
	break;
	case CEditGeom::Mode::Tailing:
	{
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, point);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_NextPoint, docPoint, 8) : docPoint;
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, point);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, m_NextPoint);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	default:
		break;
	}

	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CPolyTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	if(docPoint==c_docLast)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	switch(m_pEditgeom->m_Mode)
	{
	case CEditGeom::Mode::FirstPolying:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	case CEditGeom::Mode::SecondPolying:
	case CEditGeom::Mode::Tailing:
	{
		const CPoint last = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, c_docLast, 8) : c_docLast;
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;

		CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, last);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, point);
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		const CPoint last = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_NextPoint, c_docLast, 8) : c_docLast;
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_NextPoint, docPoint, 8) : docPoint;

		CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, last);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, point);
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, c_docLast);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, c_docLast);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, docPoint);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, docPoint);
	}
	break;
	default:
		break;
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CPolyTool::OnKeyDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	CTool::OnKeyDown(pWnd, viewinfo, nChar, nRepCnt, nFlags, pTool, oldTool);

	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return;

	if((nFlags&0X4000)==0X4000)
		return;

	if((nChar&0x00FF)==VK_SHIFT)
	{
		switch(m_pEditgeom->m_Mode)
		{
		case CEditGeom::Mode::FirstPolying:
			break;
		case CEditGeom::Mode::SecondPolying:
		case CEditGeom::Mode::Tailing:
		case CEditGeom::Mode::Inserting:
		{
			CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, c_docLast);
			const CPoint last = RegulizePoint(m_PrevPoint, c_docLast, 8);

			CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, last);
		}
		break;
		case CEditGeom::Mode::Heading:
		{
			CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, c_docLast);
			const CPoint last = RegulizePoint(m_NextPoint, c_docLast, 8);

			CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, last);
		}
		default:
			break;
		}
	}
}

void CPolyTool::OnKeyUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nChar, UINT nRepCnt, UINT nFlags, CTool*& pTool, CTool*& oldTool)
{
	CTool::OnKeyUp(pWnd, viewinfo, nChar, nRepCnt, nFlags, pTool, oldTool);

	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return;

	if((nChar&0x00FF)==VK_SHIFT)
	{
		switch(m_pEditgeom->m_Mode)
		{
		case CEditGeom::Mode::FirstPolying:
			break;
		case CEditGeom::Mode::SecondPolying:
		case CEditGeom::Mode::Tailing:
		case CEditGeom::Mode::Inserting:
		{
			const CPoint last = RegulizePoint(m_PrevPoint, c_docLast, 8);
			CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, last);

			CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, c_docLast);
		}
		break;
		case CEditGeom::Mode::Heading:
		{
			const CPoint last = RegulizePoint(m_NextPoint, c_docLast, 8);
			CPoly::DrawSquareAnchor(pWnd, viewinfo, last);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, last);

			CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
			CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, c_docLast);
		}
		default:
			break;
		}
	}
}

void CPolyTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	switch(m_pEditgeom->m_Mode)
	{
	case CEditGeom::Mode::FirstPolying:
	{
		m_PrevPoint = docPoint;
	}
	break;
	case CEditGeom::Mode::SecondPolying:
	{
		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_PrevPoint);
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);

		CPoly* pPoly = new CPoly(CRect(point, CSize(0, 0)), nullptr, nullptr);
		pPoly->AddAnchor(m_PrevPoint);
		pPoly->AddAnchor(point);
		if(pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pPoly, 0)==TRUE)
		{
			m_pEditgeom->NewGeom(pWnd, pPoly);
			m_pEditgeom->FocusAnchor(pWnd, 2, false);
		}
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_NextPoint, docPoint, 8) : docPoint;
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, point);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, point);
		m_pEditgeom->AddAnchor(pWnd, point, false);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, 1, 1);
		m_pEditgeom->FocusAnchor(pWnd, 1, false);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Tailing:
	{
		const CPoint point = ::GetKeyState(VK_SHIFT)<0 ? RegulizePoint(m_PrevPoint, docPoint, 8) : docPoint;
		CPoly::DrawSquareAnchor(pWnd, viewinfo, point);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, point);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, point);
		m_pEditgeom->AddAnchor(pWnd, point, true);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, m_pEditgeom->m_pGeom->GetAnchors()-1, 1);
		m_pEditgeom->FocusAnchor(pWnd, m_pEditgeom->m_pGeom->GetAnchors(), false);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, c_docLast);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_PrevPoint, c_docLast);
		CPoly::DrawLineTrack(pWnd, viewinfo, m_NextPoint, c_docLast);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, m_PrevAnchor, 1);
		m_pEditgeom->m_pGeom->InsertAnchor(m_PrevAnchor+1, docPoint);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, m_PrevAnchor+1, 1);

		m_pEditgeom->FocusAnchor(pWnd, m_PrevAnchor+1, false);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	default:
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
		break;
	}

	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

bool CPolyTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
	}
	return true;
}
