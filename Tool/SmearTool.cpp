#include "stdafx.h"
#include "Global.h"
#include "EditGeom.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

#include "../Dataview/viewinfo.h"

#include "Tool.h"
#include "SmearTool.h"

CSmearTool smearTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPoly* CSmearTool::poly = nullptr;

void CSmearTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(m_pEditgeom!=nullptr)
	{
		m_pEditgeom->UnFocusAll(pWnd);
	}

	delete poly;
	poly = new CPoly(CRect(docPoint, CSize(0, 0)), nullptr, nullptr);
	poly->AddAnchor(CPoint(docPoint.x, docPoint.y));
	poly->RecalRect();
}

void CSmearTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	if(poly!=nullptr)
	{
		CClientDC dc(pWnd);
		CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
		CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);
		const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(CPoint(c_docLast.x, c_docLast.y));
		const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(CPoint(docPoint.x, docPoint.y));
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);

		dc.SelectObject(OldPen);
		dc.SelectObject(OldBrush);
		dc.SetROP2(OldROP);

		poly->AddAnchor(CPoint(docPoint.x, docPoint.y));
		poly->RecalRect();
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CSmearTool::OnLButtonUp(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonUp(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	if(poly!=nullptr)
	{
		if(poly->GetAnchors()==1)
		{
			delete poly;
			poly = nullptr;
		}
		else
		{
			pWnd->SendMessage(WM_NEWGEOMDREW, (DWORD)poly, 0);
		}

		poly = nullptr;
	}
}

bool CSmearTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	const HCURSOR hCursor = nullptr;
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return true;
	}
	else
	{
		return false;
	}
}
