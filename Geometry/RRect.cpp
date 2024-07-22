#include "stdafx.h"

#include "Geom.h"
#include "Poly.h"
#include "Bezier.h"
#include "PRect.h"
#include "RRect.h"
#include "LLine.h"
#include "LBezier.h"

#include "Global.h"
#include "RRectDlg.h"

#include <cmath>
#include <string>

#include "../Dataview/viewinfo.h"
#include "../Pbf/writer.hpp"

#include "../Style/line.h"
#include "../Style/lineSymbol.h"
#include "../Style/FillCompact.h"
#include "../Style/lineRiver.h"
#include <corecrt_math_defines.h>
#include <boost/json.hpp>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CRRect, CPRect, 0)

CRRect::CRRect()
	: CPRect()
{
	m_bType = 12;
}

CRRect::CRRect(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CPRect(rect, pLine, pFill)
{
	m_nRadius = CPoint(20000, 20000);

	m_bType = 12;
}
BOOL CRRect::operator==(const CGeom& geom) const
{
	if(CPRect::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CRRect))==FALSE)
		return FALSE;
	else if(m_bRegular!=((CRRect&)geom).m_bRegular)
		return FALSE;
	else if(m_nRadius!=((CRRect&)geom).m_nRadius)
		return FALSE;
	else if(m_bRegular==false&&memcmp(m_pPoints, ((CRRect&)geom).m_pPoints, sizeof(CPoint)*9)!=0)
		return FALSE;
	else
		return TRUE;
}

bool CRRect::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor>=9)
	{
		tAnchor = 9;
		et = 0;
		return false;
	}
	else if(length==0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	const CPoint* pPoints = GetPoints();
	for(unsigned int i = fAnchor; i<9; i++)
	{
		CPoint fpoint = pPoints[(i-1)*3+1];
		CPoint fctrol = pPoints[(i-1)*3+2];
		CPoint tctrol = pPoints[(i+1-1)*3];
		CPoint tpoint = pPoints[(i+1-1)*3+1];

		LBezier lbezier(fpoint, fctrol, tctrol, tpoint);
		double tolerance = 10000.f/2;
		const unsigned long segment = lbezier.GetLength(tolerance);
		const unsigned long lastlen = lbezier.GetLengthByT(st, tolerance);
		const long leftlen = segment-lastlen;

		if(length-leftlen<tolerance)
		{
			length = lastlen+length;

			tAnchor = i;
			et = lbezier.GetTByLength(length, tolerance);

			delete[] pPoints;
			pPoints = nullptr;
			return true;
		}

		if(abs((long)length-leftlen)<=tolerance)
		{
			tAnchor = i;
			et = 1;

			delete[] pPoints;
			pPoints = nullptr;
			return true;
		}
		length -= leftlen;
		st = 0;
	}

	tAnchor = 8;
	et = 1.0f;

	delete[] pPoints;
	pPoints = nullptr;
	return false;
}

CPoint CRRect::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	ASSERT(nAnchor>=1);
	ASSERT(nAnchor<9||(t==0&&nAnchor==9));
	ASSERT(t>=0&&t<=1);

	/*if(t == 0.0f)
		return this->GetAnchor(nAnchor);
	if(t == 1.0f)
		return this->GetAnchor(nAnchor+1);*/

	const CPoint* pPoints = GetPoints();
	const CPoint& fpoint = pPoints[(nAnchor-1)*3+1];
	const CPoint& fctrol = pPoints[(nAnchor-1)*3+2];
	const CPoint& tctrol = pPoints[(nAnchor+1-1)*3];
	const CPoint& tpoint = pPoints[(nAnchor+1-1)*3+1];
	delete[] pPoints;
	pPoints = nullptr;

	return BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
}

double CRRect::GetAngle(const unsigned int& nAnchor, const double& t) const
{
	const CPoint* pPoints = GetPoints();
	const CPoint fpoint = pPoints[(nAnchor-1)*3+1];
	const CPoint fctrol = pPoints[(nAnchor-1)*3+2];
	const CPoint tctrol = pPoints[(nAnchor-1+1)*3];
	const CPoint tpoint = pPoints[(nAnchor-1+1)*3+1];
	delete[] pPoints;
	pPoints = nullptr;
	const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, t);
	const double Dx = derivate.cx;
	const double Dy = derivate.cy;
	if(Dy==0&&Dx==0)
		return 0;

	double dAngle = atan2(Dy, Dx);
	if(dAngle<0.0f)
		dAngle += 2*M_PI;

	return dAngle;
}

void CRRect::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPRect::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_nRadius;
	}
	else
	{
		ar>>m_nRadius;
	}
}

void CRRect::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		if(m_bRegular==true)
		{
			CPRect::ReleaseCE(ar);

			CPoint rdius = m_nRadius;
			rdius.x /= 10000;
			rdius.y /= 10000;
			ar<<rdius;
		}
		else
		{
			CGeom::ReleaseCE(ar);

			ar<<(unsigned short)9;

			const CPoint* pPoints = this->GetPoints();
			for(long i = 0; i<9*3; i++)
			{
				CPoint point = pPoints[i];
				point.x /= 10000;
				point.y /= 10000;

				ar<<point.x;
				ar<<point.y;
			}

			delete[] pPoints;
			pPoints = nullptr;
		}
	}
}

void CRRect::ReleaseDCOM(CArchive& ar)
{
	CPRect::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_nRadius;
	}
	else
	{
		ar>>m_nRadius;
	}
}

void CRRect::ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[9*3];
	for(unsigned int i = 0; i<9*3; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 9*3);

	fprintf(file, "%g %g m\n", points[1].X, points[1].Y);
	for(unsigned int i = 1; i<9; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CRRect::ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(pPoints, 9*3);
	delete[] pPoints;

	fprintf(file, "%g %g m\n", points[1].X, points[1].Y);
	for(unsigned int i = 1; i<9; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CRRect::ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	const Gdiplus::PointF* points = viewinfo.DocToClient <Gdiplus::PointF>(pPoints, 9*3);
	delete[] pPoints;

	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i<9; i++)
	{
		HPDF_Page_CurveTo(page, points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	delete[] points;
}

void CRRect::ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[9*3];
	for(unsigned int i = 0; i<9*3; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 9*3);

	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i<9; i++)
	{
		HPDF_Page_CurveTo(page, points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	delete[] points;
}

void CRRect::CopyTo(CGeom* pGeom, bool ignore) const
{
	CPRect::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CRRect))==TRUE)
	{
		CRRect* pRRect = (CRRect*)pGeom;

		pRRect->m_nRadius = m_nRadius;
	}
}

void CRRect::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CRRect))==TRUE)
	{
		CRRect* pRRect = (CRRect*)pGeom;

		Swapclass<CSize>(pRRect->m_nRadius, m_nRadius);
	}

	CPRect::Swap(pGeom);
}

bool CRRect::UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	CBezier* bezier = new CBezier;

	bezier->m_nAnchors = 9;
	bezier->m_nAllocs = 9*3;
	bezier->m_pPoints = GetPoints();

	CGeom::CopyTo(bezier);

	geomlist.AddHead(bezier);
	return true;
}

CPoint* CRRect::GetPoints() const
{
	CPoint* pPoints = new CPoint[9*3];

	const float EtoB = 0.552f;
	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		double h = sqrt(pow((double)(m_pPoints[3].x-m_pPoints[0].x), 2)+pow((double)(m_pPoints[3].y-m_pPoints[0].y), 2))*2;
		double v = sqrt(pow((double)(m_pPoints[1].x-m_pPoints[0].x), 2)+pow((double)(m_pPoints[1].y-m_pPoints[0].y), 2))*2;
		h = m_nRadius.cx/h;
		v = m_nRadius.cy/v;

		if(h>0.50)
			h = 0.50;
		if(v>0.50)
			v = 0.50;

		pPoints[1].x = m_pPoints[0].x+round((m_pPoints[3].x-m_pPoints[0].x)*h);
		pPoints[1].y = m_pPoints[0].y+round((m_pPoints[3].y-m_pPoints[0].y)*h);

		pPoints[4].x = m_pPoints[0].x+round((m_pPoints[1].x-m_pPoints[0].x)*v);
		pPoints[4].y = m_pPoints[0].y+round((m_pPoints[1].y-m_pPoints[0].y)*v);

		pPoints[7].x = m_pPoints[1].x+round((m_pPoints[0].x-m_pPoints[1].x)*v);
		pPoints[7].y = m_pPoints[1].y+round((m_pPoints[0].y-m_pPoints[1].y)*v);

		pPoints[10].x = m_pPoints[1].x+round((m_pPoints[2].x-m_pPoints[1].x)*h);
		pPoints[10].y = m_pPoints[1].y+round((m_pPoints[2].y-m_pPoints[1].y)*h);

		pPoints[13].x = m_pPoints[2].x+round((m_pPoints[1].x-m_pPoints[2].x)*h);
		pPoints[13].y = m_pPoints[2].y+round((m_pPoints[1].y-m_pPoints[2].y)*h);

		pPoints[16].x = m_pPoints[2].x+round((m_pPoints[3].x-m_pPoints[2].x)*v);
		pPoints[16].y = m_pPoints[2].y+round((m_pPoints[3].y-m_pPoints[2].y)*v);

		pPoints[19].x = m_pPoints[3].x+round((m_pPoints[2].x-m_pPoints[3].x)*v);
		pPoints[19].y = m_pPoints[3].y+round((m_pPoints[2].y-m_pPoints[3].y)*v);

		pPoints[22].x = m_pPoints[3].x+round((m_pPoints[0].x-m_pPoints[3].x)*h);
		pPoints[22].y = m_pPoints[3].y+round((m_pPoints[0].y-m_pPoints[3].y)*h);

		pPoints[0] = pPoints[1];
		pPoints[2].x = pPoints[1].x+round((m_pPoints[0].x-pPoints[1].x)*EtoB);
		pPoints[2].y = pPoints[1].y+round((m_pPoints[0].y-pPoints[1].y)*EtoB);

		pPoints[3].x = pPoints[4].x+round((m_pPoints[0].x-pPoints[4].x)*EtoB);
		pPoints[3].y = pPoints[4].y+round((m_pPoints[0].y-pPoints[4].y)*EtoB);
		pPoints[5] = pPoints[4];

		pPoints[6] = pPoints[7];
		pPoints[8].x = pPoints[7].x+round((m_pPoints[1].x-pPoints[7].x)*EtoB);
		pPoints[8].y = pPoints[7].y+round((m_pPoints[1].y-pPoints[7].y)*EtoB);

		pPoints[9].x = pPoints[10].x+round((m_pPoints[1].x-pPoints[10].x)*EtoB);
		pPoints[9].y = pPoints[10].y+round((m_pPoints[1].y-pPoints[10].y)*EtoB);
		pPoints[11] = pPoints[10];

		pPoints[12] = pPoints[13];
		pPoints[14].x = pPoints[13].x+round((m_pPoints[2].x-pPoints[13].x)*EtoB);
		pPoints[14].y = pPoints[13].y+round((m_pPoints[2].y-pPoints[13].y)*EtoB);

		pPoints[15].x = pPoints[16].x+round((m_pPoints[2].x-pPoints[16].x)*EtoB);
		pPoints[15].y = pPoints[16].y+round((m_pPoints[2].y-pPoints[16].y)*EtoB);
		pPoints[17] = pPoints[16];

		pPoints[18] = pPoints[19];
		pPoints[20].x = pPoints[19].x+round((m_pPoints[3].x-pPoints[19].x)*EtoB);
		pPoints[20].y = pPoints[19].y+round((m_pPoints[3].y-pPoints[19].y)*EtoB);

		pPoints[21].x = pPoints[22].x+round((m_pPoints[3].x-pPoints[22].x)*EtoB);
		pPoints[21].y = pPoints[22].y+round((m_pPoints[3].y-pPoints[22].y)*EtoB);
		pPoints[23] = pPoints[22];

		pPoints[24] = pPoints[0];
		pPoints[25] = pPoints[1];
	}
	else
	{
		double h = sqrt(pow((double)m_Rect.Width(), 2))*2;
		double v = sqrt(pow((double)m_Rect.Height(), 2))*2;
		h = m_nRadius.cx/h;
		v = m_nRadius.cy/v;

		if(h>0.50)
			h = 0.50;
		if(v>0.50)
			v = 0.50;

		pPoints[1].x = m_Rect.left+round(m_Rect.Width()*h);
		pPoints[1].y = m_Rect.top;

		pPoints[4].x = m_Rect.left;
		pPoints[4].y = m_Rect.top+round(m_Rect.Height()*v);

		pPoints[7].x = m_Rect.left;
		pPoints[7].y = m_Rect.bottom-round(m_Rect.Height()*v);

		pPoints[10].x = m_Rect.left+round(m_Rect.Width()*h);
		pPoints[10].y = m_Rect.bottom;

		pPoints[13].x = m_Rect.right-round(m_Rect.Width()*h);
		pPoints[13].y = m_Rect.bottom;

		pPoints[16].x = m_Rect.right;
		pPoints[16].y = m_Rect.bottom-round(m_Rect.Height()*v);

		pPoints[19].x = m_Rect.right;
		pPoints[19].y = m_Rect.top+round(m_Rect.Height()*v);

		pPoints[22].x = m_Rect.right-round(m_Rect.Width()*h);
		pPoints[22].y = m_Rect.top;

		pPoints[0] = pPoints[1];
		pPoints[2].x = pPoints[1].x+round((m_Rect.left-pPoints[1].x)*EtoB);
		pPoints[2].y = m_Rect.top;

		pPoints[3].x = m_Rect.left;
		pPoints[3].y = pPoints[4].y+round((m_Rect.top-pPoints[4].y)*EtoB);
		pPoints[5] = pPoints[4];

		pPoints[6] = pPoints[7];
		pPoints[8].x = m_Rect.left;
		pPoints[8].y = pPoints[7].y+round((m_Rect.bottom-pPoints[7].y)*EtoB);

		pPoints[9].x = pPoints[10].x+round((m_Rect.left-pPoints[10].x)*EtoB);
		pPoints[9].y = m_Rect.bottom;
		pPoints[11] = pPoints[10];

		pPoints[12] = pPoints[13];
		pPoints[14].x = pPoints[13].x+round((m_Rect.right-pPoints[13].x)*EtoB);
		pPoints[14].y = m_Rect.bottom;

		pPoints[15].x = m_Rect.right;
		pPoints[15].y = pPoints[16].y+round((m_Rect.bottom-pPoints[16].y)*EtoB);
		pPoints[17] = pPoints[16];

		pPoints[18] = pPoints[19];
		pPoints[20].x = m_Rect.right;
		pPoints[20].y = pPoints[19].y+round((m_Rect.top-pPoints[19].y)*EtoB);

		pPoints[21].x = pPoints[22].x+round((m_Rect.right-pPoints[22].x)*EtoB);
		pPoints[21].y = m_Rect.top;
		pPoints[23] = pPoints[22];

		pPoints[24] = pPoints[0];
		pPoints[25] = pPoints[1];
	}

	return pPoints;
}

CPoly* CRRect::GetPoly() const
{
	CBezier* bezier = new CBezier;
	bezier->m_bClosed = true;
	bezier->m_nAnchors = 9;
	bezier->m_nAllocs = 9*3;
	bezier->m_Rect = m_Rect;
	bezier->m_pPoints = this->GetPoints();

	return bezier;
}
CPoly* CRRect::Polylize(const unsigned int& tolerance) const
{
	CBezier* pBezier = (CBezier*)CRRect::GetPoly();
	CPoly* pPoly = pBezier->Polylize(tolerance);
	delete pBezier;
	return pPoly;
}
void CRRect::Attribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	double h = 0;
	double v = 0;
	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		h = sqrt(pow((double)(m_pPoints[3].x-m_pPoints[0].x), 2)+pow((double)(m_pPoints[3].y-m_pPoints[0].y), 2))*2;
		v = sqrt(pow((double)(m_pPoints[1].x-m_pPoints[0].x), 2)+pow((double)(m_pPoints[1].y-m_pPoints[0].y), 2))*2;

		h = h*0.5;
		v = v*0.5;
	}
	else
	{
		h = sqrt(pow((double)m_Rect.Width(), 2))*2;
		v = sqrt(pow((double)m_Rect.Height(), 2))*2;

		h = h*0.5;
		v = v*0.5;
	}
	const long nMax = (long)min(h, v);
	CRRectDlg dlg(pWnd, m_nRadius.cx/10000, nMax/10000);
	if(dlg.DoModal()==IDOK)
	{
		Invalidate(pWnd, viewinfo, 0);
		m_nRadius.cx = m_nRadius.cy = dlg.m_nRadius*10000;
		Invalidate(pWnd, viewinfo, 0);
	}
}

void CRRect::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
}

void CRRect::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	if(m_bRegular==true)
	{
		const Gdiplus::Rect rect = CGeom::Transform(matrix, m_Rect);
		Gdiplus::PointF point1(0, 0);
		Gdiplus::PointF point2(m_nRadius.cx, m_nRadius.cy);
		matrix.TransformPoints(&point1);
		matrix.TransformPoints(&point2);
		CSize radius(abs(point2.X-point1.X), abs(point2.Y-point1.Y));
		if(rect.Width<radius.cx*2)
			radius.cx = rect.Width/2;
		if(rect.Height<radius.cy*2)
			radius.cy = rect.Height/2;

		path.AddArc(rect.GetLeft(), rect.GetBottom()-2*radius.cy, 2*radius.cx, 2*radius.cy, 90, 90);
		path.AddLine(rect.GetLeft(), rect.GetBottom()-radius.cx, rect.GetLeft(), rect.GetTop()+radius.cy);

		path.AddArc(rect.GetLeft(), rect.GetTop(), 2*radius.cx, 2*radius.cy, 180, 90);
		path.AddLine(rect.GetLeft()+radius.cx, rect.GetTop(), rect.GetRight()-radius.cx, rect.GetTop());

		path.AddArc(rect.GetRight()-2*radius.cx, rect.GetTop(), 2*radius.cx, 2*radius.cy, 270, 90);
		path.AddLine(rect.GetRight(), rect.GetTop()+radius.cx, rect.GetRight(), rect.GetBottom()-radius.cy);

		path.AddArc((INT)(rect.GetRight()-2*radius.cx), (INT)(rect.GetBottom()-2*radius.cy), (INT)2*radius.cx, (INT)(2*radius.cy), 0.0f, 90.0f);
		path.AddLine(rect.GetRight()-radius.cx, rect.GetBottom(), rect.GetLeft()+radius.cx, rect.GetBottom());
	}
	else
	{
		CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = CGeom::Transform(matrix, points1, 9*3);
		delete[] points1;

		path.AddBeziers(points2+1, 9*3-1);

		::delete[] points2;
		points2 = nullptr;
	}
}

void CRRect::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	if(m_bRegular==true)
	{
		const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(m_Rect);
		CSize radius = viewinfo.DocToClient(m_nRadius);
		if(rect.Width<radius.cx*2)
			radius.cx = rect.Width/2;
		if(rect.Height<radius.cy*2)
			radius.cy = rect.Height/2;

		path.AddArc(rect.GetLeft(), rect.GetBottom()-2*radius.cy, 2*radius.cx, 2*radius.cy, 90, 90);
		path.AddLine(rect.GetLeft(), rect.GetBottom()-radius.cx, rect.GetLeft(), rect.GetTop()+radius.cy);

		path.AddArc(rect.GetLeft(), rect.GetTop(), 2*radius.cx, 2*radius.cy, 180, 90);
		path.AddLine(rect.GetLeft()+radius.cx, rect.GetTop(), rect.GetRight()-radius.cx, rect.GetTop());

		path.AddArc(rect.GetRight()-2*radius.cx, rect.GetTop(), 2*radius.cx, 2*radius.cy, 270, 90);
		path.AddLine(rect.GetRight(), rect.GetTop()+radius.cx, rect.GetRight(), rect.GetBottom()-radius.cy);

		path.AddArc((INT)(rect.GetRight()-2*radius.cx), (INT)(rect.GetBottom()-2*radius.cy), (INT)(2*radius.cx), (INT)(2*radius.cy), 0.0f, 90.0f);
		path.AddLine(rect.GetRight()-radius.cx, rect.GetBottom(), rect.GetLeft()+radius.cx, rect.GetBottom());
	}
	else
	{
		const CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 9*3);
		::delete[] points1;

		path.AddBeziers(points2+1, 9*3-1);

		::delete[] points2;
		points2 = nullptr;
	}
}

void CRRect::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	if(m_bRegular==true)
	{
		const CPoint point1 = CPoint(m_Rect.left, m_Rect.top);
		const CPoint point2 = CPoint(m_Rect.right, m_Rect.bottom);
		const Gdiplus::PointF cliPoint1 = viewinfo.DocToClient<Gdiplus::PointF>(point1);
		const Gdiplus::PointF cliPoint2 = viewinfo.DocToClient<Gdiplus::PointF>(point2);

		CRect rect = CRect(cliPoint1.X, cliPoint1.Y, cliPoint2.X, cliPoint2.Y);
		rect.NormalizeRect();
		CSize radius = viewinfo.DocToClient(m_nRadius);
		if(rect.Width()<radius.cx*2)
			radius.cx = rect.Width()/2;
		if(rect.Height()<radius.cy*2)
			radius.cy = rect.Height()/2;

		pDC->RoundRect(rect, CPoint(radius.cx, radius.cy));
	}
	else
	{
		CPoint* points1 = this->GetPoints();
		const Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 9*3);
		memcpy(points1, points2, sizeof(CPoint)*9*3);

		pDC->PolyBezier(points1+1, 9*3-2);

		::delete[] points2;
		delete[] points1;
	}

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CRRect::DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const
{
	if(tAnchor<fAnchor)
		return;
	if(tAnchor==fAnchor&&et<=st)
		return;

	CPoint* points1 = GetPoints();
	Gdiplus::Point* points2 = viewinfo.DocToClient<Gdiplus::Point>(points1, 9*3);
	delete[] points1;

	Gdiplus::Point fPoint[4];
	fPoint[0] = points2[(fAnchor-1)*3+1];
	fPoint[1] = points2[(fAnchor-1)*3+2];
	fPoint[2] = points2[(fAnchor+1-1)*3];
	fPoint[3] = points2[(fAnchor+1-1)*3+1];

	Gdiplus::Point tPoint[3];
	tPoint[0] = points2[(tAnchor-1)*3+2];
	tPoint[1] = points2[(tAnchor+1-1)*3];
	tPoint[2] = points2[(tAnchor+1-1)*3+1];

	if(st!=0)
	{
		LBezier segment(points2[(fAnchor-1)*3+1], points2[(fAnchor-1)*3+2], points2[(fAnchor+1-1)*3], points2[(fAnchor+1-1)*3+1]);

		LBezier L, R;
		segment.Split(L, R, st);

		fPoint[0] = Gdiplus::Point(R.fpoint.x, R.fpoint.y);
		fPoint[1] = Gdiplus::Point(R.fctrol.x, R.fctrol.y);
		fPoint[2] = Gdiplus::Point(R.tctrol.x, R.tctrol.y);
	}

	if(et!=1)
	{
		if(fAnchor==tAnchor)
		{
			LBezier segment(fPoint[0], fPoint[1], fPoint[2], fPoint[3]);
			LBezier L, R;

			double t = 1.f-(1.0f-et)/(1.0f-st);
			segment.Split(L, R, t);

			fPoint[1] = Gdiplus::Point(L.fctrol.x, L.fctrol.y);
			fPoint[2] = Gdiplus::Point(L.tctrol.x, L.tctrol.y);
			fPoint[3] = Gdiplus::Point(L.tpoint.x, L.tpoint.y);
		}
		else
		{
			LBezier segment(points2[(tAnchor-1)*3+1], points2[(tAnchor-1)*3+2], points2[(tAnchor+1-1)*3+0], points2[(tAnchor+1-1)*3+1]);
			LBezier L, R;

			segment.Split(L, R, et);

			tPoint[0] = Gdiplus::Point(L.fctrol.x, L.fctrol.y);
			tPoint[1] = Gdiplus::Point(L.tctrol.x, L.tctrol.y);
			tPoint[2] = Gdiplus::Point(L.tpoint.x, L.tpoint.y);
		}
	}

	g.DrawBezier(pen, fPoint[0], fPoint[1], fPoint[2], fPoint[3]);
	if(tAnchor>fAnchor+1)
	{
		g.DrawBeziers(pen, points2+(fAnchor+1-1)*3+1, (tAnchor-fAnchor-1)*3);
	}
	if(tAnchor>fAnchor)
	{
		g.DrawBezier(pen, fPoint[3], tPoint[0], tPoint[1], tPoint[2]);
	}

	::delete[] points2;
}

bool CRRect::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CPRect::PickOn(point, gap)==false)
		return false;

	if(m_bRegular==false)
	{
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			CPoint fpoint = pPoints[(i-1)*3+1];
			CPoint fctrol = pPoints[(i-1)*3+2];
			CPoint tctrol = pPoints[(i+1-1)*3];
			CPoint tpoint = pPoints[(i+1-1)*3+1];

			double t;
			CPoint dpoint = point;
			if(PointOnBezier(dpoint, fpoint, fctrol, tctrol, tpoint, gap, t))
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}

		delete[] pPoints;
		pPoints = nullptr;
		return false;
	}
	else
	{
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			CPoint fpoint = pPoints[(i-1)*3+1];
			CPoint fctrol = pPoints[(i-1)*3+2];
			CPoint tctrol = pPoints[(i+1-1)*3];
			CPoint tpoint = pPoints[(i+1-1)*3+1];

			double t;
			CPoint dpoint = point;
			if(PointOnBezier(dpoint, fpoint, fctrol, tctrol, tpoint, gap, t))
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}

		delete[] pPoints;
		pPoints = nullptr;
		return false;
	}

	return false;
}

bool CRRect::PickIn(const CPoint& point) const
{
	if(CPRect::PickIn(point)==false)
		return false;

	if(m_bRegular==false)
	{
		CPoint* pPoints = GetPoints();
		const bool bRet = CBezier::PointInBeziergon(point, pPoints, 9);
		delete[] pPoints;
		pPoints = nullptr;
		return bRet;
	}
	else
	{
		CPoint* pPoints = GetPoints();
		const bool bRet = CBezier::PointInBeziergon(point, pPoints, 9);
		delete[] pPoints;
		pPoints = nullptr;
		return bRet;
	}
}

bool CRRect::PickIn(const CRect& rect) const
{
	if(CPRect::PickIn(rect)==false)
		return false;

	if(m_bRegular==false)
	{
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			const CPoint point = pPoints[(i-1)*3+1];
			if(rect.PtInRect(point)==TRUE)
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}
		delete[] pPoints;
		pPoints = nullptr;
		const CPoint point1 = CPoint(rect.left, rect.top);
		const CPoint point2 = CPoint(rect.left, rect.bottom);
		const CPoint point3 = CPoint(rect.right, rect.top);
		const CPoint point4 = CPoint(rect.right, rect.bottom);
		if(this->PickIn(point1)==true||this->PickIn(point2)==true||this->PickIn(point3)==true||this->PickIn(point4)==true)
		{
			return true;
		}
	}
	else
	{
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			const CPoint point = pPoints[(i-1)*3+1];
			if(rect.PtInRect(point)==TRUE)
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}
		delete[] pPoints;
		pPoints = nullptr;
		const CPoint point1 = CPoint(rect.left, rect.top);
		const CPoint point2 = CPoint(rect.left, rect.bottom);
		const CPoint point3 = CPoint(rect.right, rect.top);
		const CPoint point4 = CPoint(rect.right, rect.bottom);
		if(this->PickIn(point1)==true||this->PickIn(point2)==true||this->PickIn(point3)==true||this->PickIn(point4)==true)
		{
			return true;
		}
	}

	return false;
}

bool CRRect::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CPRect::PickIn(center, radius)==false)
		return false;

	if(this->PickIn(center)==true)
		return true;

	CPoint* pPoints = GetPoints();
	for(unsigned int i = 1; i<9; i++)
	{
		pPoints[i].x -= center.x;
		pPoints[i].y -= center.y;
	}

	int sqradius =radius*radius;
	const CPoint point(0, 0);
	for(unsigned int i = 1; i<9; i++)
	{
		CPoint fpoint = pPoints[(i-1)*3+1];
		CPoint fctrol = pPoints[(i-1)*3+2];
		CPoint tctrol = pPoints[(i+1-1)*3];
		CPoint tpoint = pPoints[(i+1-1)*3+1];

		if(::Distance2(point, fpoint, fctrol, tctrol, tpoint)<=sqradius)
		{
			delete[] pPoints;
			pPoints = nullptr;
			return true;
		}
	}
	delete[] pPoints;
	pPoints = nullptr;

	return false;
}

bool CRRect::PickIn(const CPoly& polygon) const
{
	if(CPRect::PickIn(polygon.m_Rect)==false)
		return false;

	if(m_bRegular==false)
	{
		CRect rect = polygon.m_Rect;
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			CPoint point = pPoints[(i-1)*3+1];
			if(polygon.PickIn(point)==true)
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}
		delete[] pPoints;
		pPoints = nullptr;

		for(unsigned int j = 1; j<polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point)==true)
			{
				return true;
			}
		}
	}
	else
	{
		CRect rect = polygon.m_Rect;
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<9; i++)
		{
			CPoint point = pPoints[(i-1)*3+1];
			if(polygon.PickIn(point)==true)
			{
				delete[] pPoints;
				pPoints = nullptr;
				return true;
			}
		}
		delete[] pPoints;
		pPoints = nullptr;

		for(unsigned int j = 1; j<polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point)==true)
			{
				return true;
			}
		}
	}

	return false;
}

DWORD CRRect::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CPRect::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_nRadius, sizeof(CPoint));
		size += sizeof(CPoint);

		return size;
	}
	else
	{
		m_nRadius = *(CPoint*)bytes;
		bytes += sizeof(CPoint);
		return 0;
	}
}

DWORD CRRect::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);

	if(m_bRegular==false)
	{
		const unsigned int nAnchors = 9;
		file.Write(&nAnchors, sizeof(unsigned int));
		size += sizeof(unsigned int);

		const CPoint* pPoints = this->GetPoints();
		for(long i = 0; i<nAnchors*3; i++)
		{
			CPoint point = pPoints[i];

			point.x = point.x/10000;
			point.y = point.y/10000;

			file.Write(&point, sizeof(CPoint));
		}

		delete[] pPoints;
		size += 3*nAnchors*sizeof(CPoint);
	}
	else
	{
		file.Write(&m_nRadius, sizeof(CPoint));
		size += sizeof(CPoint);
	}

	return size;
}

void CRRect::ReleaseWeb(CFile& file, CSize offset) const
{
	CPRect::ReleaseWeb(file, offset);

	short cx = m_nRadius.cx;
	short cy = m_nRadius.cy;
	ReverseOrder(cx);
	file.Write(&cx, sizeof(short));
	ReverseOrder(cy);
	file.Write(&cy, sizeof(short));
}

void CRRect::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CPRect::ReleaseWeb(stream, offset);

	stream<<m_nRadius.cx;
	stream<<m_nRadius.cy;
}

void CRRect::ReleaseWeb(boost::json::object& json) const
{
	CPRect::ReleaseWeb(json);

	std::vector<int>  child;
	child.push_back(m_nRadius.cx);
	child.push_back(m_nRadius.cy);
	json["Radius"] = boost::json::value_from(child);
}
void CRRect::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPRect::ReleaseWeb(writer, offset);
		
	writer.add_variant_uint32(m_nRadius.cx);
	writer.add_variant_uint32(m_nRadius.cy);
}