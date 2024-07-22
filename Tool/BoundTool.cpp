#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "BoundTool.h"

#include "../Viewer/Global.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CBoundTool boundTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CBoundTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CPoint local = docPoint;
	m_points.Add(local);
}

void CBoundTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
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

void CBoundTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
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

		pWnd->SendMessage(WM_RGNQUERY, (DWORD)poly, 0);
		poly->Invalidate(pWnd, viewinfo, 1);
		delete poly;
	}

	m_points.RemoveAll();
}

bool CBoundTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
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

void CBoundTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
{
	if(m_points.GetCount()>1)
	{
		pDC->SaveDC();

		pDC->SetMapMode(MM_TEXT);
		pDC->SetViewportOrg(0, 0);
		pDC->SetWindowOrg(0, 0);

		CPen pen;
		pen.CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
		CPen* OldPen = (CPen*)pDC->SelectObject(&pen);
		const long nPoints = m_points.GetSize();
		for(int i = 0; i<nPoints-1; i++)
		{
			const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[i]);
			const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[i+1]);

			CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);
			pDC->SelectObject(OldPen);
		}

		OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
		if(nPoints)
		{
			const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[nPoints-1]);
			const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);
		}

		if(nPoints>1)
		{
			const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[0]);
			const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);

			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);
		}
		pDC->SelectObject(OldPen);
		pen.DeleteObject();

		pDC->RestoreDC(-1);
	}

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CBoundTool::CanPan()
{
	const long nPoints = m_points.GetSize();
	if(nPoints>0)
		return false;

	return CTool::CanPan();
}
