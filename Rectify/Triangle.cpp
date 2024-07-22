#include "stdafx.h"

#include "Triangle.h"
#include "Tin.h"
#include "Datum.h"

#include "../Dataview/viewinfo.h"
#include <stdio.h>

IMPLEMENT_SERIAL(CTriangle, CObject, 0)

CTriangle::CTriangle()
{
	m_pDatum1 = nullptr;
	m_pDatum2 = nullptr;
	m_pDatum3 = nullptr;

	a11 = 0;
	a21 = 0;
	a31 = 0;
	a12 = 0;
	a22 = 0;
	a32 = 0;
}

CTriangle::~CTriangle()
{
	m_pDatum1 = nullptr;
	m_pDatum2 = nullptr;
	m_pDatum3 = nullptr;
}

double CTriangle::PointTo(const CPoint& point) const
{
	const CPoint point1 = m_pDatum1->m_rPoint;
	const CPoint point2 = m_pDatum2->m_rPoint;
	const CPoint point3 = m_pDatum3->m_rPoint;

	return sqrt(pow((double)(point1.x-point.x), 2)+pow((double)(point1.y-point.y), 2))+sqrt(pow((double)(point2.x-point.x), 2)+pow((double)(point2.y-point.y), 2))+sqrt(pow((double)(point3.x-point.x), 2)+pow((double)(point3.y-point.y), 2));
}

bool CTriangle::PointIn(const CPoint& point) const
{
	const CPoint point1 = m_pDatum1->m_rPoint;
	const CPoint point2 = m_pDatum2->m_rPoint;
	const CPoint point3 = m_pDatum3->m_rPoint;

	double a1 = (double)(point1.x-point2.x)*(point.y-point2.y)-(double)(point1.y-point2.y)*(point.x-point2.x);
	double a11 = (double)(point1.x-point2.x)*(point3.y-point2.y)-(double)(point1.y-point2.y)*(point3.x-point2.x);

	double a2 = (double)(point2.x-point3.x)*(point.y-point3.y)-(double)(point2.y-point3.y)*(point.x-point3.x);
	double a22 = (double)(point2.x-point3.x)*(point1.y-point3.y)-(double)(point2.y-point3.y)*(point1.x-point3.x);

	double a3 = (double)(point3.x-point1.x)*(point.y-point1.y)-(double)(point3.y-point1.y)*(point.x-point1.x);
	double a33 = (double)(point3.x-point1.x)*(point2.y-point1.y)-(double)(point3.y-point1.y)*(point2.x-point1.x);

	if(a1>0)
		a1 = 1;
	else if(a1<0)
		a1 = -1;
	if(a11>0)
		a11 = 1;
	else if(a11<0)
		a11 = -1;

	if(a2>0)
		a2 = 1;
	else if(a2<0)
		a2 = -1;
	if(a22>0)
		a22 = 1;
	else if(a22<0)
		a22 = -1;

	if(a3>0)
		a3 = 1;
	else if(a3<0)
		a3 = -1;
	if(a33>0)
		a33 = 1;
	else if(a33<0)
		a33 = -1;

	if(a1*a11>=0&&a2*a22>=0&&a3*a33>=0)
		return TRUE;
	else
		return false;
}

CPoint CTriangle::Rectify(const CPoint& point) const
{
	const double x = a11+a21*point.x+a31*point.y;
	const double y = a12+a22*point.x+a32*point.y;

	return CPoint(x, y);
}

void CTriangle::CalParameter()
{
	const double x1 = m_pDatum1->m_rPoint.x;
	const double y1 = m_pDatum1->m_rPoint.y;
	const double u1 = m_pDatum1->m_bPoint.x;
	const double v1 = m_pDatum1->m_bPoint.y;
	const double x2 = m_pDatum2->m_rPoint.x;
	const double y2 = m_pDatum2->m_rPoint.y;
	const double u2 = m_pDatum2->m_bPoint.x;
	const double v2 = m_pDatum2->m_bPoint.y;
	const double x3 = m_pDatum3->m_rPoint.x;
	const double y3 = m_pDatum3->m_rPoint.y;
	const double u3 = m_pDatum3->m_bPoint.x;
	const double v3 = m_pDatum3->m_bPoint.y;

	//����ϵ��
	a31 = ((u1-u2)*(x3-x2)-(u3-u2)*(x1-x2))/((y1-y2)*(x3-x2)-(y3-y2)*(x1-x2));
	if((x1-x2)!=0)
	{
		a21 = (u1-u2-a31*(y1-y2))/(x1-x2);
		a11 = u1-(a21*x1+a31*y1);
	}
	else
	{
		a21 = (u3-u2-a31*(y3-y2))/(x3-x2);
		a11 = u3-(a21*x3+a31*y3);
	}

	a32 = ((v1-v2)*(x3-x2)-(v3-v2)*(x1-x2))/((y1-y2)*(x3-x2)-(y3-y2)*(x1-x2));
	if((x1-x2)!=0)
	{
		a22 = (v1-v2-a32*(y1-y2))/(x1-x2);
		a12 = v1-(a22*x1+a32*y1);
	}
	else
	{
		a22 = (v3-v2-a32*(y3-y2))/(x3-x2);
		a12 = v3-(a22*x3+a32*y3);
	}
}

void CTriangle::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo) const
{
	if(pWnd==nullptr)
		return;
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum1->m_rPoint);
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum2->m_rPoint);
	const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum3->m_rPoint);

	CRect rect1(point1.X, point1.Y, point2.X, point2.Y);
	rect1.NormalizeRect();
	pWnd->InvalidateRect(rect1, TRUE);

	CRect rect2(point1.X, point1.Y, point3.X, point3.Y);
	rect2.NormalizeRect();
	pWnd->InvalidateRect(rect2, TRUE);

	CRect rect3(point2.X, point2.Y, point3.X, point3.Y);
	rect3.NormalizeRect();
	pWnd->InvalidateRect(rect3, TRUE);
}

void CTriangle::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo) const
{
	const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum1->m_rPoint);
	const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum2->m_rPoint);
	const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(m_pDatum3->m_rPoint);

	const Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0), 1);

	g.DrawLine(&pen, point1.X, point1.Y, point2.X, point2.Y);
	g.DrawLine(&pen, point1.X, point1.Y, point3.X, point3.Y);
	g.DrawLine(&pen, point2.X, point2.Y, point3.X, point3.Y);
}
