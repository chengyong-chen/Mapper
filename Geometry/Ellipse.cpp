#include "stdafx.h"
#include <cmath>
#include "Geom.h"
#include "Poly.h"
#include "Bezier.h"
#include "PRect.h"
#include "Ellipse.h"
#include "LLine.h"
#include "LBezier.h"
#include "Fuction.h"

#include "Global.h"
#include "../Style/Line.h"
#include "../Style/lineSymbol.h"
#include "../Style/lineRiver.h"
#include "../Style/FillCompact.h"
#include "../DataView/viewinfo.h"
#include <corecrt_math_defines.h>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CEllipse, CPRect, 0)

CEllipse::CEllipse()
	: CPRect()
{
	m_bType = 13;
}

CEllipse::CEllipse(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CPRect(rect, pLine, pFill)
{
	m_bType = 13;
}
BOOL CEllipse::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom)==FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CEllipse))==FALSE)
		return FALSE;
	else
		return TRUE;
}

bool CEllipse::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor>=5)
	{
		tAnchor = 5;
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
	for(unsigned int i = fAnchor; i<5; i++)
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

		if(abs((long)length-leftlen)<=0.5)
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

	tAnchor = 4;
	et = 1.0f;

	delete[] pPoints;
	pPoints = nullptr;
	return false;
}

CPoint CEllipse::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	ASSERT(nAnchor>=1);
	ASSERT(nAnchor<5||(t==0.f&&nAnchor==5));
	ASSERT(t>=0&&t<=1);

	if(t==0.0f)
		return this->GetAnchor(nAnchor);
	if(t==1.0f)
		return this->GetAnchor(nAnchor+1);

	const CPoint* pPoints = GetPoints();
	const CPoint fpoint = pPoints[(nAnchor-1)*3+1];
	const CPoint fctrol = pPoints[(nAnchor-1)*3+2];
	const CPoint tctrol = pPoints[(nAnchor+1-1)*3];
	const CPoint tpoint = pPoints[(nAnchor+1-1)*3+1];
	delete[] pPoints;
	pPoints = nullptr;

	return BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
}

double CEllipse::GetAngle(const unsigned int& nAnchor, const double& t) const
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

	if(Dy==0.f&&Dx==0.f)
		return 0;

	double dAngle = atan2(Dy, Dx);
	if(dAngle<0.0f)
		dAngle += 2*M_PI;

	return dAngle;
}

bool CEllipse::UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	CBezier* bezier = new CBezier;
	bezier->m_nAnchors = 5;
	bezier->m_nAllocs = 5*3;
	bezier->m_pPoints = GetPoints();

	CGeom::CopyTo(bezier);

	geomlist.AddHead(bezier);
	return true;
}

CPoint* CEllipse::GetPoints() const
{
	CPoint* pPoints = new CPoint[5*3];

	const float EtoB = 0.552f;
	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		pPoints[1].x = round(m_pPoints[0].x/2.0+m_pPoints[1].x/2.0);
		pPoints[1].y = round(m_pPoints[0].y/2.0+m_pPoints[1].y/2.0);

		pPoints[4].x = round(m_pPoints[1].x/2.0+m_pPoints[2].x/2.0);
		pPoints[4].y = round(m_pPoints[1].y/2.0+m_pPoints[2].y/2.0);

		pPoints[7].x = round(m_pPoints[2].x/2.0+m_pPoints[3].x/2.0);
		pPoints[7].y = round(m_pPoints[2].y/2.0+m_pPoints[3].y/2.0);

		pPoints[10].x = round(m_pPoints[0].x/2.0+m_pPoints[3].x/2.0);
		pPoints[10].y = round(m_pPoints[0].y/2.0+m_pPoints[3].y/2.0);

		pPoints[0].x = round(pPoints[1].x+(m_pPoints[0].x-pPoints[1].x)*EtoB);
		pPoints[0].y = round(pPoints[1].y+(m_pPoints[0].y-pPoints[1].y)*EtoB);

		pPoints[2].x = round(pPoints[1].x+(m_pPoints[1].x-pPoints[1].x)*EtoB);
		pPoints[2].y = round(pPoints[1].y+(m_pPoints[1].y-pPoints[1].y)*EtoB);

		pPoints[3].x = round(pPoints[4].x+(m_pPoints[1].x-pPoints[4].x)*EtoB);
		pPoints[3].y = round(pPoints[4].y+(m_pPoints[1].y-pPoints[4].y)*EtoB);

		pPoints[5].x = round(pPoints[4].x+(m_pPoints[2].x-pPoints[4].x)*EtoB);
		pPoints[5].y = round(pPoints[4].y+(m_pPoints[2].y-pPoints[4].y)*EtoB);

		pPoints[6].x = round(pPoints[7].x+(m_pPoints[2].x-pPoints[7].x)*EtoB);
		pPoints[6].y = round(pPoints[7].y+(m_pPoints[2].y-pPoints[7].y)*EtoB);

		pPoints[8].x = round(pPoints[7].x+(m_pPoints[3].x-pPoints[7].x)*EtoB);
		pPoints[8].y = round(pPoints[7].y+(m_pPoints[3].y-pPoints[7].y)*EtoB);

		pPoints[9].x = round(pPoints[10].x+(m_pPoints[3].x-pPoints[10].x)*EtoB);
		pPoints[9].y = round(pPoints[10].y+(m_pPoints[3].y-pPoints[10].y)*EtoB);

		pPoints[11].x = round(pPoints[10].x+(m_pPoints[0].x-pPoints[10].x)*EtoB);
		pPoints[11].y = round(pPoints[10].y+(m_pPoints[0].y-pPoints[10].y)*EtoB);
	}
	else
	{
		pPoints[1].x = m_Rect.left;
		pPoints[1].y = round(m_Rect.top/2.0+m_Rect.bottom/2.0);

		pPoints[4].x = round(m_Rect.left/2.0+m_Rect.right/2.0);
		pPoints[4].y = m_Rect.bottom;

		pPoints[7].x = m_Rect.right;
		pPoints[7].y = round(m_Rect.top/2.0+m_Rect.bottom/2.0);

		pPoints[10].x = round(m_Rect.left/2.0+m_Rect.right/2.0);
		pPoints[10].y = m_Rect.top;

		pPoints[0].x = round(pPoints[1].x+(m_Rect.left-pPoints[1].x)*EtoB);
		pPoints[0].y = round(pPoints[1].y+(m_Rect.top-pPoints[1].y)*EtoB);

		pPoints[2].x = round(pPoints[1].x+(m_Rect.left-pPoints[1].x)*EtoB);
		pPoints[2].y = round(pPoints[1].y+(m_Rect.bottom-pPoints[1].y)*EtoB);

		pPoints[3].x = round(pPoints[4].x+(m_Rect.left-pPoints[4].x)*EtoB);
		pPoints[3].y = round(pPoints[4].y+(m_Rect.bottom-pPoints[4].y)*EtoB);

		pPoints[5].x = round(pPoints[4].x+(m_Rect.right-pPoints[4].x)*EtoB);
		pPoints[5].y = round(pPoints[4].y+(m_Rect.bottom-pPoints[4].y)*EtoB);

		pPoints[6].x = round(pPoints[7].x+(m_Rect.right-pPoints[7].x)*EtoB);
		pPoints[6].y = round(pPoints[7].y+(m_Rect.bottom-pPoints[7].y)*EtoB);

		pPoints[8].x = round(pPoints[7].x+(m_Rect.right-pPoints[7].x)*EtoB);
		pPoints[8].y = round(pPoints[7].y+(m_Rect.top-pPoints[7].y)*EtoB);

		pPoints[9].x = round(pPoints[10].x+(m_Rect.right-pPoints[10].x)*EtoB);
		pPoints[9].y = round(pPoints[10].y+(m_Rect.top-pPoints[10].y)*EtoB);

		pPoints[11].x = round(pPoints[10].x+(m_Rect.left-pPoints[10].x)*EtoB);
		pPoints[11].y = round(pPoints[10].y+(m_Rect.top-pPoints[10].y)*EtoB);
	}

	pPoints[12] = pPoints[0];
	pPoints[13] = pPoints[1];

	return pPoints;
}

CPoly* CEllipse::GetPoly() const
{
	CBezier* bezier = new CBezier;
	bezier->m_bClosed = true;
	bezier->m_nAnchors = 5;
	bezier->m_nAllocs = 5*3;
	bezier->m_Rect = m_Rect;
	bezier->m_pPoints = GetPoints();

	return bezier;
}
CPoly* CEllipse::Polylize(const unsigned int& tolerance) const
{
	CBezier* pBezier = (CBezier*)CEllipse::GetPoly();
	CPoly* pPoly = pBezier->Polylize(tolerance);
	delete pBezier;
	return pPoly;
}
void CEllipse::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
}

void CEllipse::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	if(m_bRegular==true)
	{
		const Gdiplus::Rect rect = CGeom::Transform(matrix, m_Rect);
		path.AddEllipse(rect);
	}
	else
	{
		CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = CGeom::Transform(matrix, points1, 5*3);

		path.AddBeziers(points2+1, 5*3-2);

		delete[] points1;
		::delete[] points2;
	}
}

void CEllipse::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	if(m_bRegular==true)
	{
		const Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
		path.AddEllipse(rect);
	}
	else
	{
		const CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 5*3);

		path.AddBeziers(points2+1, 5*3-2);

		::delete[] points1;
		::delete[] points2;
	}
}

void CEllipse::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	if(m_pLine==nullptr&&m_pFill==nullptr)
		return;

	Gdiplus::Pen* pen = m_pLine!=nullptr ? m_pLine->GetPen(context.ratioLine, dpi) : nullptr;
	Gdiplus::Brush* brush = m_bClosed==true&&m_pFill!=nullptr ? m_pFill->GetBrush(context.ratioFill, dpi) : nullptr;
	const Gdiplus::Rect rect = CGeom::Transform(matrix, m_Rect);
	if(m_bRegular==true)
	{
		if(brush!=nullptr)
		{
			if(brush->GetType()==Gdiplus::BrushTypePathGradient)
			{
				Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
				path.AddEllipse(rect);

				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, matrix, &path);
				g.FillEllipse(local, rect);
				::delete local;
			}
			else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, matrix);
				g.FillEllipse(local, rect);
				::delete local;
			}
			else
			{
				g.FillEllipse(brush, rect);
			}
		}
		if(pen!=nullptr)
		{
			g.DrawEllipse(pen, rect);
		}
	}
	else
	{
		CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = CGeom::Transform(matrix, points1, 5*3);
		if(brush!=nullptr)
		{
			Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
			path.AddBeziers(points2+1, 5*3-2);
			path.CloseFigure();

			if(brush->GetType()==Gdiplus::BrushTypePathGradient)
			{
				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, matrix, &path);
				g.FillPath(local, &path);
				::delete local;
			}
			else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, matrix);
				g.FillPath(local, &path);
				::delete local;
			}
			else
			{
				g.FillPath(brush, &path);
			}
		}

		if(pen!=nullptr)
		{
			g.DrawBeziers(pen, points2+1, 5*3-2);
		}

		delete[] points1;
		::delete[] points2;
	}

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}

void CEllipse::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	Gdiplus::Pen* pen = m_pLine!=nullptr ? m_pLine->GetPen(ratio, viewinfo.m_sizeDPI) : nullptr;
	Gdiplus::Brush* brush = m_pFill!=nullptr ? m_pFill->GetBrush(ratio, viewinfo.m_sizeDPI) : nullptr;
	const Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
	if(m_bRegular==true)
	{
		if(brush!=nullptr)
		{
			if(brush->GetType()==Gdiplus::BrushTypePathGradient)
			{
				Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
				this->AddPath(path, viewinfo, viewinfo.GetPrecision(10));

				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
				g.FillEllipse(local, rect);
				::delete local;
			}
			else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillEllipse(local, rect);
				::delete local;
			}
			else
			{
				g.FillEllipse(brush, rect);
			}
		}
		if(pen!=nullptr)
		{
			g.DrawEllipse(pen, rect);
		}
	}
	else
	{
		const CPoint* points1 = this->GetPoints();
		Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 5*3);
		if(brush!=nullptr)
		{
			Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
			path.AddBeziers(points2+1, 5*3-2);
			path.CloseFigure();

			if(brush->GetType()==Gdiplus::BrushTypePathGradient)
			{
				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
				g.FillPath(local, &path);
				::delete local;
			}
			else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillPath(local, &path);
				::delete local;
			}
			else
			{
				g.FillPath(brush, &path);
			}
		}

		if(pen!=nullptr)
		{
			g.DrawBeziers(pen, points2+1, 5*3-2);
		}

		delete[] points1;
		::delete[] points2;
	}

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
}

void CEllipse::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	Gdiplus::Pen* pen = context.penStroke;
	Gdiplus::Brush* brush = context.brushFill;
	Gdiplus::Pen* pen1 = nullptr;
	if(m_pLine!=nullptr)
	{
		pen1 = m_pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
		pen = pen1;
	}
	Gdiplus::Brush* brush1 = nullptr;
	if(m_bClosed==true&&m_pFill!=nullptr)
	{
		brush1 = m_pFill->GetBrush(context.ratioFill, viewinfo.m_sizeDPI);
		brush = brush1;
	}
	if(pen!=nullptr||brush!=nullptr)
	{
		const Gdiplus::Rect rect = viewinfo.DocToClient<Gdiplus::Rect>(m_Rect);
		if(m_bRegular==true)
		{
			if(brush!=nullptr)
			{
				if(brush->GetType()==Gdiplus::BrushTypePathGradient)
				{
					Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
					this->AddPath(path, viewinfo, context.tolerance);

					const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
					g.FillEllipse(local, rect);
					::delete local;
				}
				else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
				{
					const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
					g.FillEllipse(local, rect);
					::delete local;
				}
				else
				{
					g.FillEllipse(brush, rect);
				}
			}
			if(pen!=nullptr)
			{
				g.DrawEllipse(pen, rect);
			}
		}
		else
		{
			const CPoint* points1 = this->GetPoints();
			Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 5*3);
			if(brush!=nullptr)
			{
				Gdiplus::GraphicsPath path(Gdiplus::FillMode::FillModeAlternate);
				path.AddBeziers(points2+1, 5*3-2);
				path.CloseFigure();

				if(brush->GetType()==Gdiplus::BrushTypePathGradient)
				{
					const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &path);
					g.FillPath(local, &path);
					::delete local;
				}
				else if(brush->GetType()==Gdiplus::BrushTypeLinearGradient)
				{
					const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
					g.FillPath(local, &path);
					::delete local;
				}
				else
				{
					g.FillPath(brush, &path);
				}
			}

			if(pen!=nullptr)
			{
				g.DrawBeziers(pen, points2+1, 5*3-2);
			}

			delete[] points1;
			::delete[] points2;
		}

		::delete pen1;
		pen1 = nullptr;
		::delete brush1;
		brush1 = nullptr;
	}

	if(m_pLine!=nullptr&&m_pLine->Gettype()==CLine::LINETYPE::Symbol)
		((CLineSymbol*)m_pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine!=nullptr&&m_pLine->Gettype()==CLine::LINETYPE::River)
		((CLineRiver*)m_pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(m_pLine!=nullptr)
	{
	}
	else if(context.pLine!=nullptr&&context.pLine->IsKindOf(RUNTIME_CLASS(CLineSymbol))==TRUE)
		((CLineSymbol*)context.pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
	else if(context.pLine!=nullptr&&context.pLine->IsKindOf(RUNTIME_CLASS(CLineRiver))==TRUE)
		((CLineRiver*)m_pLine)->Draw(g, viewinfo, library, this, context.ratioLine);
}
void CEllipse::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
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

		pDC->Ellipse(rect);
	}
	else
	{
		const CPoint* points1 = this->GetPoints();
		const Gdiplus::PointF* points2 = viewinfo.DocToClient<Gdiplus::PointF>(points1, 5*3);
		delete[] points1;

		CPoint* points3 = new CPoint[5*3];
		memcpy(points3, points2, 5*3*sizeof(Gdiplus::PointF));
		::delete[] points2;

		pDC->PolyBezier(points3+1, 5*3-2);
		delete[] points3;
	}

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CEllipse::DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const
{
	if(tAnchor<fAnchor)
		return;
	if(tAnchor==fAnchor&&et<=st)
		return;

	CPoint* points1 = GetPoints();
	Gdiplus::Point* points2 = viewinfo.DocToClient<Gdiplus::Point>(points1, 5*3);
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
		LBezier segment(points1[(fAnchor-1)*3+1], points1[(fAnchor-1)*3+2], points1[(fAnchor+1-1)*3], points1[(fAnchor+1-1)*3+1]);

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
			LBezier segment(points1[(tAnchor-1)*3+1], points1[(tAnchor-1)*3+2], points1[(tAnchor+1-1)*3+0], points1[(tAnchor+1-1)*3+1]);
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
		g.DrawBezier(pen, points2[(tAnchor-1)*3+1], tPoint[0], tPoint[1], tPoint[2]);
	}

	delete[] points1;
	::delete[] points2;
}

bool CEllipse::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CGeom::PickOn(point, gap)==false)
		return false;

	if(m_bRegular==false)
	{
		const CPoint* pPoints = GetPoints();
		for(unsigned int i = 1; i<5; i++)
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
		CPoint trend;
		const double shortcut = Shortcut(point, trend);
		if(shortcut<=gap)
			return true;
		else
			return false;
	}
}

bool CEllipse::PickIn(const CPoint& point) const
{
	if(CPRect::PickIn(point)==false)
		return false;

	if(m_bRegular==false)
	{
		CPoint* pPoints = GetPoints();
		const bool bRet = CBezier::PointInBeziergon(point, pPoints, 5);
		delete[] pPoints;
		pPoints = nullptr;
		return bRet;
	}
	else
	{
		const CPoint center = m_Rect.CenterPoint();
		const double a = m_Rect.Width()/2;
		const double b = m_Rect.Height()/2;
		const double AA = a*a;
		const double BB = b*b;
		const double xx = (point.x-center.x)*(point.x-center.x);
		const double yy = (point.y-center.y)*(point.y-center.y);
		if(BB*xx+AA*yy>AA*BB)
			return false;
		else
			return true;
	}
}

bool CEllipse::PickIn(const CRect& rect) const
{
	if(CPRect::PickIn(rect)==false)
		return false;

	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		for(unsigned int i = 1; i<5; i++)
		{
			const CPoint& point = GetAnchor(i);
			if(rect.PtInRect(point)==TRUE)
				return true;
		}
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
		for(unsigned int i = 1; i<5; i++)
		{
			const CPoint& point = GetAnchor(i);
			if(rect.PtInRect(point)==TRUE)
				return true;
		}
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

bool CEllipse::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CPRect::PickIn(center, radius)==false)
		return false;

	if(this->PickIn(center)==true)
		return true;

	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		const long a = m_Rect.Width();
		const long b = m_Rect.Height();

		CPoint center1 = m_Rect.CenterPoint();
		center1.x = m_Rect.left+m_Rect.Width()/2;//��zoomRect��ֵ����ʱ��CenterPoint��������ִ������
		center1.y = m_Rect.top+m_Rect.Height()/2;
		if(::LineLength1(CPoint(center.x, center.y), CPoint(center1.x, center1.y))<(radius+max(a, b)))
		{//�˲��ֲ����ƣ��д�����
			return true;
		}

		return false;
	}
	else
	{
		CPoint trend;
		const double d = Shortcut(center, trend);
		if(d<radius)
			return true;
		else
			return false;
	}
}

bool CEllipse::PickIn(const CPoly& polygon) const
{
	if(CPRect::PickIn(polygon.m_Rect)==false)
		return false;

	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		CRect rect = polygon.m_Rect;
		for(unsigned int i = 1; i<5; i++)
		{
			const CPoint& point = GetAnchor(i);
			if(polygon.PickIn(point.x, point.y)==true)
			{
				return true;
			}
		}

		for(unsigned int j = 1; j<polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point.x, point.y)==true)
			{
				return true;
			}
		}
	}
	else
	{
		CRect rect = polygon.m_Rect;
		for(unsigned int i = 1; i<5; i++)
		{
			const CPoint& point = GetAnchor(i);
			if(polygon.PickIn(point)==true)
			{
				return true;
			}
		}

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

void CEllipse::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		if(m_bRegular==false)
		{
			CGeom::ReleaseCE(ar);

			ar<<(unsigned int)5;
			const CPoint* pPoints = GetPoints();
			for(unsigned int i = 0; i<5*3; i++)
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
		else
		{
			CPRect::ReleaseCE(ar);
		}
	}
	else
	{
	}
}

DWORD CEllipse::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);

	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		const unsigned int nAnchors = 5;
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

	return size;
}

void CEllipse::ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[5*3];
	for(unsigned int i = 0; i<5*3; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 5*3);

	_ftprintf(file, _T("%g %g m\n"), points[1].X, points[1].Y);
	for(unsigned int i = 1; i<5; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CEllipse::ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(pPoints, 5*3);
	delete[] pPoints;

	_ftprintf(file, _T("%g %g m\n"), points[1].X, points[1].Y);
	for(unsigned int i = 1; i<5; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CEllipse::ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(this->IsValid()==false)
		return;

	if(m_bRegular==false&&m_pPoints!=nullptr)
	{
		const CPoint* pPoints = this->GetPoints();
		const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(pPoints, 5*3);
		delete[] pPoints;

		HPDF_Page_MoveTo(page, points[1].X, points[1].Y);
		for(unsigned int i = 1; i<5; i++)
		{
			HPDF_Page_CurveTo(page, points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
		}
		delete[] points;
	}
	else
	{
		const Gdiplus::RectF rect = viewinfo.DocToClient<Gdiplus::RectF>(m_Rect);
		HPDF_Page_Ellipse(page, rect.X+rect.Width/2, rect.Y+rect.Height/2, rect.Width/2, rect.Height/2);
	}
}

void CEllipse::ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid()==false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[5*3];
	for(unsigned int i = 0; i<5*3; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 5*3);

	HPDF_Page_MoveTo(page, points[1].X, points[1].Y);
	for(unsigned int i = 1; i<5; i++)
	{
		HPDF_Page_CurveTo(page, points[(i-1)*3+2].X, points[(i-1)*3+2].Y, points[i*3].X, points[i*3].Y, points[i*3+1].X, points[i*3+1].Y);
	}
	delete[] points;
}

double CEllipse::Shortcut(const CPoint& point, CPoint& trend) const
{
	if(m_Rect.Width()==m_Rect.Height())
	{
		const double cx = m_Rect.Width()/2.f;
		const double cy = m_Rect.Height()/2.f;
		const double x0 = point.x-(m_Rect.left+cx);
		const double y0 = point.y-(m_Rect.top+cy);
		const double fRadius1 = (cx+cy)/2;
		const double fRadius2 = sqrt(x0*x0+y0*y0);
		return abs(fRadius1-fRadius2);
	}
	else
	{
		const double x0 = point.x-(m_Rect.left+m_Rect.Width()/2.f);
		const double y0 = point.y-(m_Rect.top+m_Rect.Height()/2.f);
		const double A = m_Rect.Width()/2.f;
		const double B = m_Rect.Height()/2.f;
		const double AA = A*A;
		const double BB = B*B;

		double c[5];
		c[4] = (BB-AA)*(BB-AA);
		c[3] = -2*y0*B*(BB-AA);
		c[2] = y0*y0*BB+x0*x0*AA-(BB-AA)*(BB-AA);
		c[1] = 2*y0*B*(BB-AA);
		c[0] = -y0*y0*BB;

		double** a = new double* [4];
		for(unsigned int i = 0; i<4; i++)
		{
			a[i] = new double[4];
			ZeroMemory(a[i], sizeof(double)*4);
		}

		for(unsigned int j = 0; j<4; j++)
			a[0][j] = -c[4-j-1]/c[4];
		for(unsigned int i = 1; i<4; i++)
			a[i][i-1] = 1.0;

		double xr[4];
		double xi[4];

		QRroot(a, 4, xr, xi, 0.0001, 60);

		for(unsigned int i = 0; i<4; i++)
		{
			delete a[i];
		}
		delete[] a;

		long miniD = 10000000;
		for(unsigned int i = 0; i<4; i++)
		{
			if(xi[i]!=0.f)
				continue;

			if(xr[i]<-1)
				continue;
			if(xr[i]>1)
				continue;
			const float sinq = xr[i];
			const int y = B*sinq;
			const int x = abs(A*sqrt(1-sinq*sinq)-x0)>abs(A*sqrt(1-sinq*sinq)+x0) ? -A*sqrt(1-sinq*sinq) : A*sqrt(1-sinq*sinq);
			const int d = sqrt((double)(x-x0)*(x-x0)+(double)(y-y0)*(y-y0));
			if(d<miniD)
			{
				miniD = d;
				trend.x = x;
				trend.y = y;
			}
		}

		return miniD;
	}
}
