#include "stdafx.h"
#include "Resource.h"
#include "Global.h"
#include "EditGeom.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/LLine.h"
#include "../Geometry/LBezier.h"

#include "../Dataview/viewinfo.h"

#include "Tool.h"
#include "BezierTool.h"

__declspec(dllexport) CBezierTool bezierTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CBezierTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(m_pEditgeom->m_Mode==CEditGeom::Mode::FirstBeziering)
		m_pEditgeom->m_Mode = CEditGeom::Mode::SecondBeziering;
	else if(m_pEditgeom->m_pGeom==nullptr)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstBeziering;
	else if(m_pEditgeom->m_pGeom->Gettype()!=2)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstBeziering;
	else if(m_pEditgeom->m_Anchors.size()>1)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstBeziering;
	else if(m_pEditgeom->m_Anchors.size()==0)
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstBeziering;
	else if(*(m_pEditgeom->m_Anchors.begin())==m_pEditgeom->m_pGeom->GetAnchors())
	{
		const CBezier* pBezier = (CBezier*)(m_pEditgeom->m_pGeom);
		const unsigned int nAnchor = m_pEditgeom->m_pGeom->GetAnchors();
		m_PrevPoint[0] = pBezier->m_pPoints[(nAnchor-1)*3+0];
		m_PrevPoint[1] = pBezier->m_pPoints[(nAnchor-1)*3+1];
		m_PrevPoint[2] = pBezier->m_pPoints[(nAnchor-1)*3+2];
		m_pEditgeom->m_Mode = CEditGeom::Mode::Tailing;
	}
	else if(*(m_pEditgeom->m_Anchors.begin())==1)
	{
		const CBezier* pBezier = (CBezier*)(m_pEditgeom->m_pGeom);
		m_NextPoint[0] = pBezier->m_pPoints[2];
		m_NextPoint[1] = pBezier->m_pPoints[1];
		m_NextPoint[2] = pBezier->m_pPoints[0];
		m_pEditgeom->m_Mode = CEditGeom::Mode::Heading;
	}
	else
		m_pEditgeom->m_Mode = CEditGeom::Mode::FirstBeziering;

	if(m_pEditgeom->m_Mode==CEditGeom::Mode::FirstBeziering)
	{
		m_pEditgeom->UnFocusAll(pWnd);

		CGeom* pGeom = (CGeom*)pWnd->SendMessage(WM_PICKACTIVEGEOM, (LONG)0, (LONG)&docPoint);
		if(pGeom!=nullptr&&pGeom->Gettype()==2)
		{
			CBezier* pBezier = (CBezier*)pGeom;
			const CSize nInflate = viewinfo.ClientToDoc(Gdiplus::SizeF(3, 3));
			double t = 0;
			const int nAnchor = pBezier->PickOn(docPoint, nInflate.cx, t);
			if(t>0&&t<1)
			{
				m_PrevAnchor = nAnchor;
				m_PrevPoint[0] = pBezier->m_pPoints[(nAnchor-1)*3+0];
				m_PrevPoint[1] = pBezier->m_pPoints[(nAnchor-1)*3+1];
				m_PrevPoint[2] = pBezier->m_pPoints[(nAnchor-1)*3+2];
				m_NextPoint[0] = pBezier->m_pPoints[(nAnchor+1-1)*3+2];
				m_NextPoint[1] = pBezier->m_pPoints[(nAnchor+1-1)*3+1];
				m_NextPoint[2] = pBezier->m_pPoints[(nAnchor+1-1)*3];

				m_pEditgeom->NewGeom(pWnd, pBezier);
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
	case CEditGeom::Mode::FirstBeziering:
	{
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
		m_PrevPoint[0] = docPoint;
		m_PrevPoint[1] = docPoint;
		m_PrevPoint[2] = docPoint;
	}
	break;
	case CEditGeom::Mode::SecondBeziering:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, m_PrevPoint);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_PrevPoint[1]);

		CPoint points[2];
		points[0] = docPoint;
		points[1] = docPoint;
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points);

		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	case CEditGeom::Mode::Tailing:
	{
		CPoint points[2];
		points[0] = docPoint;
		points[1] = docPoint;

		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		CPoint points[2];
		points[0] = docPoint;
		points[1] = docPoint;

		CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points);
		CPoly::DrawSquareAnchor(pWnd, viewinfo, docPoint);
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		const LBezier segment(m_PrevPoint[1], m_PrevPoint[2], m_NextPoint[2], m_NextPoint[1]);
		CPoint point(docPoint);
		const CSize nInflate = viewinfo.ClientToDoc(Gdiplus::SizeF(3, 3));
		double t = 0;
		if(segment.PointOn(point, nInflate.cx, t)==true)
		{
			LBezier L;
			LBezier R;
			segment.Split(L, R, t);
			m_PrevPoint[2] = L.fctrol;
			m_NextPoint[2] = R.tctrol;
			CPoint points1[2];
			CPoint points2[2];
			CPoint points[3];
			points1[0] = L.tctrol;
			points1[1] = L.tpoint;
			points2[0] = R.fctrol;
			points2[1] = R.fpoint;
			m_InsertPoint[0] = points1[0];
			m_InsertPoint[1] = points1[1];
			m_InsertPoint[2] = points2[0];
			CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points1);
			CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points2);
			CBezier::DrawSquareAnchor(pWnd, viewinfo, m_InsertPoint);
		}
		else
			m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	default:
		break;
	}

	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CBezierTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(docPoint==c_docLast)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	if((nFlags&MK_LBUTTON)!=MK_LBUTTON)
		return CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
	const CSize delta1 = c_docLast-c_docDown;
	const CSize delta2 = docPoint-c_docDown;

	CPoint points1[3];
	CPoint points2[3];
	points1[0] = c_docDown-delta1;
	points1[1] = c_docDown;
	points1[2] = c_docDown+delta1;
	points2[0] = c_docDown-delta2;
	points2[1] = c_docDown;
	points2[2] = c_docDown+delta2;

	switch(m_pEditgeom->m_Mode)
	{
	case CEditGeom::Mode::FirstBeziering:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points1);
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points2);
	}
	break;
	case CEditGeom::Mode::SecondBeziering:
	case CEditGeom::Mode::Tailing:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points1);
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points1);
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points2);
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points2);
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points1);
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points1);
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points2);
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points2);
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		points1[0] = m_InsertPoint[0]+delta1;
		points1[1] = m_InsertPoint[1]+delta1;
		points1[2] = m_InsertPoint[2]+delta1;
		points2[0] = m_InsertPoint[0]+delta2;
		points2[1] = m_InsertPoint[1]+delta2;
		points2[2] = m_InsertPoint[2]+delta2;

		CBezier::DrawSquareAnchor(pWnd, viewinfo, points1);
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points2);

		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points1);
		points1[0] = points1[2];
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points1);

		CBezier::DrawBezierTrack(pWnd, viewinfo, m_PrevPoint+1, points2);
		points2[0] = points2[2];
		CBezier::DrawBezierTrack(pWnd, viewinfo, m_NextPoint+1, points2);
	}
	break;
	default:
		break;
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CBezierTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	const CSize Δ = docPoint-c_docDown;
	CPoint points[3];
	points[0] = c_docDown-Δ;
	points[1] = c_docDown;
	points[2] = c_docDown+Δ;

	switch(m_pEditgeom->m_Mode)
	{
	case CEditGeom::Mode::FirstBeziering:
	{
		m_PrevPoint[0] = c_docDown-Δ;
		m_PrevPoint[2] = c_docDown+Δ;
	}
	break;
	case CEditGeom::Mode::SecondBeziering:
	{
		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_PrevPoint[1]);
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points);

		CBezier* pBezier = new CBezier(CRect(docPoint, CSize(0, 0)), nullptr, nullptr);
		pBezier->AddAnchor(m_PrevPoint);
		pBezier->AddAnchor(points);
		if(pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)pBezier, 0)==TRUE)
		{
			m_pEditgeom->NewGeom(pWnd, pBezier);
			m_pEditgeom->FocusAnchor(pWnd, 2, false);
		}
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Heading:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, points[1]);
		m_pEditgeom->AddAnchor(pWnd, points, false);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, 1, 1);
		m_pEditgeom->FocusAnchor(pWnd, 1, false);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Tailing:
	{
		CBezier::DrawSquareAnchor(pWnd, viewinfo, points);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, points[1]);

		m_pEditgeom->AddAnchor(pWnd, points, true);
		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, m_pEditgeom->m_pGeom->GetAnchors()-1, 1);
		m_pEditgeom->FocusAnchor(pWnd, m_pEditgeom->m_pGeom->GetAnchors(), false);
		m_pEditgeom->m_Mode = CEditGeom::Mode::Nothing;
	}
	break;
	case CEditGeom::Mode::Inserting:
	{
		m_InsertPoint[0] += Δ;
		m_InsertPoint[1] += Δ;
		m_InsertPoint[2] += Δ;

		CBezier::DrawSquareAnchor(pWnd, viewinfo, m_InsertPoint);
		CPoly::DrawBlackAnchor(pWnd, viewinfo, m_InsertPoint[1]);

		m_pEditgeom->m_pGeom->InvalidateAnchor(pWnd, viewinfo, m_PrevAnchor, 1);
		((CBezier*)(m_pEditgeom->m_pGeom))->InsertAnchor(m_PrevAnchor+1, m_PrevPoint[1], m_InsertPoint, m_NextPoint[1]);
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

bool CBezierTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_MYCROSS);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
		return false;
}
