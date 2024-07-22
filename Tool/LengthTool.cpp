#include "stdafx.h"
#include "Resource.h"

#include "Tool.h"
#include "LengthTool.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Dataview/viewinfo.h"

__declspec(dllexport) CLengthTool lengthTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void CLengthTool::OnLButtonDown(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	CTool::OnLButtonDown(pWnd, viewinfo, nFlags, cliPoint, docPoint);

	CPoint local = docPoint;
	m_points.Add(local);

	CPoint last = docPoint;
	const long nPoints = m_points.GetSize();
	if(nPoints>1)
		last = m_points[nPoints-2];

	CClientDC dc(pWnd);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0, 0);
	const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(last);
	const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(docPoint);

	CPen pen;
	pen.CreatePen(PS_SOLID, 3, RGB(255, 255, 0));
	CPen* OldPen = (CPen*)dc.SelectObject(&pen);
	dc.MoveTo(MoveTo.X, MoveTo.Y);
	dc.LineTo(LineTo.X, LineTo.Y);
	dc.SelectObject(OldPen);
	pen.DeleteObject();

	OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	dc.MoveTo(MoveTo.X, MoveTo.Y);
	dc.LineTo(LineTo.X, LineTo.Y);
	dc.SelectObject(OldPen);
}

void CLengthTool::OnMouseMove(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& cliPoint, const CPoint& docPoint)
{
	const long nPoints = m_points.GetSize();
	if(nPoints)
	{
		CClientDC dc(pWnd);
		dc.SetMapMode(MM_TEXT);
		dc.SetViewportOrg(0, 0);
		dc.SetWindowOrg(0, 0);

		CPen pen;
		pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
		CPen* OldPen = (CPen*)dc.SelectObject(&pen);
		const int OldROP = dc.SetROP2(R2_NOTXORPEN);
		const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[nPoints-1]);
		Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);

		LineTo = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
		dc.MoveTo(MoveTo.X, MoveTo.Y);
		dc.LineTo(LineTo.X, LineTo.Y);

		dc.SelectObject(OldPen);
		dc.SetROP2(OldROP);
		pen.DeleteObject();
	}

	CTool::OnMouseMove(pWnd, viewinfo, nFlags, cliPoint, docPoint);
}

void CLengthTool::OnLButtonDblClk(CWnd* pWnd, const CViewinfo& viewinfo, UINT nFlags, const CPoint& docPoint)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const unsigned short nPoints = (unsigned short)m_points.GetSize();
	if(nPoints>=2)
	{
		CPoly* poly = new CPoly;
		poly->m_pPoints = new CPoint[nPoints];
		for(unsigned short i = 0; i<nPoints; i++)
		{
			poly->m_pPoints[i].x = m_points[i].x;
			poly->m_pPoints[i].y = m_points[i].y;
		}

		poly->m_nAnchors = nPoints;
		poly->m_nAllocs = nPoints;

		poly->RecalRect();

		m_points.RemoveAll();
		const double length = viewinfo.m_datainfo.CalLength(poly->m_pPoints, poly->m_nAnchors);

		CString str;
		if(length>1)
		{
			CString r;
			r.Format(_T("%.2f"), length);
			r.TrimRight(_T("0"));
			r.TrimRight(_T("."));
			str.LoadString(IDS_LENGTH1);
			str.Replace(_T("%s"), r);
		}
		else
		{
			CString r;
			r.Format(_T("%.0f"), length*1000);
			r.TrimRight(_T("0"));
			r.TrimRight(_T("."));
			str.LoadString(IDS_LENGTH2);
			str.Replace(_T("%s"), r);
		}

		AfxMessageBox(str);

		poly->Invalidate(pWnd, viewinfo, 2);
		delete poly;
	}

	CTool::OnLButtonDblClk(pWnd, viewinfo, nFlags, docPoint);
}

bool CLengthTool::SetCursor(CWnd* pWnd, const CViewinfo& viewinfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	const HCURSOR hCursor = AfxGetApp()->LoadCursor(IDC_LENGTH);
	if(hCursor!=nullptr)
	{
		::SetCursor(hCursor);
		return TRUE;
	}
	else
		return false;
}

void CLengthTool::Draw(CWnd* pWnd, const CViewinfo& viewinfo, CDC* pDC)
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

			CPen* OldPen = (CPen*)pDC->SelectObject(&pen);
			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);

			pDC->SelectObject(OldPen);

			OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);
			pDC->SelectObject(OldPen);
		}
		pen.DeleteObject();

		if(nPoints)
		{
			CPen pen;
			pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
			CPen* OldPen = (CPen*)pDC->SelectObject(&pen);
			const int OldROP = pDC->SetROP2(R2_NOTXORPEN);
			const Gdiplus::Point MoveTo = viewinfo.DocToClient<Gdiplus::Point>(m_points[nPoints-1]);
			const Gdiplus::Point LineTo = viewinfo.DocToClient<Gdiplus::Point>(c_docLast);
			pDC->MoveTo(MoveTo.X, MoveTo.Y);
			pDC->LineTo(LineTo.X, LineTo.Y);

			pDC->SelectObject(OldPen);
			pDC->SetROP2(OldROP);
			pen.DeleteObject();
		}

		pDC->RestoreDC(-1);
	}

	CTool::Draw(pWnd, viewinfo, pDC);
}

bool CLengthTool::CanPan()
{
	const long nPoints = m_points.GetSize();
	if(nPoints>0)
		return false;

	return CTool::CanPan();
}
