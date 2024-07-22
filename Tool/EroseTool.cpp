#include "stdafx.h"
#include "Resource.h"

#include "Global.h"
#include "Tool.h"
#include "EroseTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"

#include "../Dataview/viewinfo.h"

__declspec(dllexport) CEroseTool eroseTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CEroseTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CPoint local = docPoint;
	m_points.Add(local);
}

void CEroseTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CClientDC dc(pWnd);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0, 0);

	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const long nPoints = m_points.GetSize();
	if(nPoints)
	{
		const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[nPoints-1]);
		Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);

		LineTo = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);
	}

	if(nPoints>1)
	{
		const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[0]);
		Gdiplus::Point LineTo;
		if(c_docLast!=m_points[1])
		{
			LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
			dc.MoveTo(MoveTo.X, MoveTo.Y);
			dc.LineTo(LineTo.X, LineTo.Y);
		}

		LineTo = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);
	}

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CEroseTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	const long nPoints = m_points.GetSize();
	if(nPoints>=3)
	{
		CPoly* poly = new CPoly;
		poly->m_bClosed = true;
		poly->m_pPoints = new CPoint[nPoints+1];
		poly->m_nAnchors = nPoints+1;
		poly->m_nAllocs = nPoints+1;

		for(long i = 0; i<nPoints; i++)
		{
			poly->m_pPoints[i].x = m_points[i].x;
			poly->m_pPoints[i].y = m_points[i].y;
		}
		poly->m_pPoints[nPoints] = poly->m_pPoints[0];
		poly->RecalRect();

		pWnd->SendMessage(WM_PICKBYPOLY, (DWORD)poly, 0);
		//		pWnd->SendMessage(WM_TOPO_PICKBYPOLYGON,(DWORD)poly,0);

		poly->Invalidate(pWnd, viewinfo, 1);
		delete poly;
	}

	m_points.RemoveAll();

	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
}

bool CEroseTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
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
