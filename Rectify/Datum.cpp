#include "stdafx.h"

#include "Datum.h"
#include "../Dataview/viewinfo.h"

#include <stdio.h>

IMPLEMENT_SERIAL(CDatum, CObject, 0)

CDatum::CDatum()
{
	m_rActive = false;
	m_bActive = false;
	m_rPoint = CPoint(-1, -1);
	m_bPoint = CPoint(-1, -1);
}

CDatum::CDatum(const CPoint& point)
{
	m_rPoint = point;
	m_bPoint = point;

	m_rActive = false;
	m_bActive = true;
}

CDatum::~CDatum()
{
}

void CDatum::Moving(CWnd* pWnd, const CViewinfo& viewinfo, const CSize& Δ)
{
	if(Δ==CSize(0, 0))
		return;
	if(m_rActive==false&&m_bActive==false)
		return;

	CClientDC dc(pWnd);
	dc.SetMapMode(MM_TEXT);
	dc.SetViewportOrg(0, 0);
	dc.SetWindowOrg(0, 0);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	CPoint point1 = (m_rActive==true ? m_rPoint : m_bPoint);
	const CPoint point2 = (m_rActive==true ? m_bPoint : m_rPoint);
	point1.Offset(Δ);
	const Gdiplus::Point point11 = viewinfo.DocToClient<Gdiplus::Point>(point1);
	const Gdiplus::Point point22 = viewinfo.DocToClient<Gdiplus::Point>(point2);

	CBrush brush(m_rActive==true ? RGB(255, 0, 0) : RGB(0, 0, 255));
	CBrush* oldBrush = (CBrush*)dc.SelectObject(&brush);
	dc.Ellipse(point11.X-5, point11.Y-5, point11.X+5, point11.Y+5);
	dc.SelectObject(oldBrush);

	CPen* oldPen1 = (CPen*)dc.SelectStockObject(WHITE_PEN);
	dc.MoveTo(point11.X-5, point11.Y);
	dc.LineTo(point11.X+5, point11.Y);
	dc.MoveTo(point11.X, point11.Y-5);
	dc.LineTo(point11.X, point11.Y+5);
	dc.SelectObject(oldPen1);

	CPen pen(PS_SOLID, 1, RGB(125, 125, 0));
	CPen* oldPen2 = (CPen*)dc.SelectObject(&pen);
	dc.MoveTo(point11.X, point11.Y);
	dc.LineTo(point22.X, point22.Y);
	dc.SelectObject(oldPen2);

	dc.SetROP2(OldROP);
}

void CDatum::Move(const CSize& Δ)
{
	if(m_rActive==TRUE)
	{
		m_rPoint.Offset(Δ);
	}
	if(m_bActive==TRUE)
	{
		m_bPoint.Offset(Δ);
	}
}

bool CDatum::Pick(const CViewinfo& viewinfo, const CPoint& docPoint) const
{
	const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(docPoint);
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_rPoint);
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_bPoint);

	if(abs(point.X-point2.X)<=5&&abs(point.Y-point2.Y)<=5)
	{
		m_rActive = false;
		m_bActive = true;
		return TRUE;
	}
	else if(abs(point.X-point1.X)<=5&&abs(point.Y-point1.Y)<=5)
	{
		m_rActive = true;
		m_bActive = false;
		return TRUE;
	}
	else
		return false;
}

void CDatum::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	if(pWnd==nullptr)
		return;
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_rPoint);
	CRect rect1(point1.X, point1.Y, point1.X, point1.Y);
	rect1.InflateRect(5, 5);
	pWnd->InvalidateRect(rect1, TRUE);
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_bPoint);
	CRect rect2(point2.X, point2.Y, point2.X, point2.Y);
	rect2.InflateRect(5, 5);
	pWnd->InvalidateRect(rect2, TRUE);

	CRect rect3(point1.X, point1.Y, point2.X, point2.Y);
	rect3.NormalizeRect();
	pWnd->InvalidateRect(rect3, TRUE);
}

void CDatum::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo) const
{
	const Gdiplus::SolidBrush brush1(Gdiplus::Color(255, 255, 0, 0));
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_rPoint);
	g.FillEllipse(&brush1, point1.X-5, point1.Y-5, 10, 10);
	const Gdiplus::Pen pen1(Gdiplus::Color(255, 255, 255, 255), 1);
	g.DrawLine(&pen1, point1.X-5, point1.Y, point1.X+5, point1.Y);
	g.DrawLine(&pen1, point1.X, point1.Y-5, point1.X, point1.Y+5);

	const Gdiplus::SolidBrush brush2(Gdiplus::Color(255, 0, 0, 255));
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_bPoint);
	g.FillEllipse(&brush2, point2.X-5, point2.Y-5, 10, 10);
	const Gdiplus::Pen pen2(Gdiplus::Color(255, 255, 255, 255), 1);
	g.DrawLine(&pen2, point2.X-5, point2.Y, point2.X+5, point2.Y);
	g.DrawLine(&pen2, point2.X, point2.Y-5, point2.X, point2.Y+5);

	const Gdiplus::Pen pen3(Gdiplus::Color(255, 125, 125, 0), 1);
	g.DrawLine(&pen3, point1.X, point1.Y, point2.X, point2.Y);
}

void CDatum::Draw(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	const CClientDC dc(pWnd);
	Gdiplus::Graphics g(dc.m_hDC);
	CDatum::Draw(g, viewinfo);
	g.ReleaseHDC(dc.m_hDC);
}

void CDatum::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_rPoint;
		ar<<m_bPoint;
	}
	else
	{
		ar>>m_rPoint;
		ar>>m_bPoint;
	}
}

long GetSymbol(double a)
{
	if(a>0)
		return 1;
	else if(a<0)
		return -1;
	else
		return 0;
}
