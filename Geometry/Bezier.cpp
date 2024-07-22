#include "stdafx.h"
#include <cmath>
#include <map>
#include <vector>

#include "Geom.h"
#include "Poly.h"
#include "Bezier.h"
#include "Text.h"
#include "Global.h"
#include "LLine.h"
#include "LBezier.h"
#include "Tag.h"

#include "../Style/Line.h"
#include "../Style/LineSymbol.h"
#include "../Style/LineRiver.h"

#include "../Style/FillCompact.h"

#include "../Dataview/viewinfo.h"

#include "../Public/Equation.h"
#include "../Public/BinaryStream.h"
#include "../Rectify/Tin.h"
#include "../Pbf/writer.hpp"
#include "../Utility/Rect.hpp"

#include "../Action/Modify.h"
#include "../Action/ActionStack.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Poly.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Bezier.h"
#include <corecrt_math_defines.h>
#include <boost/json.hpp>
using namespace boost;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern float d_fVersion;

using namespace std;
using namespace Undoredo;

IMPLEMENT_SERIAL(CBezier, CPoly, 0)

CBezier::CBezier()
	: CPoly()
{
	m_bType = 2;
}

CBezier::CBezier(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CPoly(rect, pLine, pFill)
{
	m_bType = 2;
}
CBezier::CBezier(const std::vector<CPoint>& points, int anchors, bool bCLosed)
	: CBezier()
{
	m_pPoints = new CPoint[anchors*3 + 1];
	for(int index = 0; index < points.size(); index++)
	{
		m_pPoints[index] = points[index];
	}
	m_nAnchors = anchors;
	m_nAllocs = anchors*3 + 1;
	
	m_bClosed = bCLosed;
	CBezier::RecalRect();
}
BOOL CBezier::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CBezier)) == FALSE)
		return FALSE;
	else if(m_nAnchors != ((CBezier&)geom).m_nAnchors)
		return FALSE;
	else if(memcmp(m_pPoints, ((CBezier&)geom).m_pPoints, m_nAnchors*sizeof(CPoint)*3) != 0)
		return FALSE;
	else
		return TRUE;
}
void CBezier::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CPath::Sha1(sha1);

	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		sha1.process_bytes(&m_pPoints[i].x, sizeof(int));
		sha1.process_bytes(&m_pPoints[i].y, sizeof(int));
	}
}
void CBezier::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CPath::Sha1(sha1, offset);

	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		LONG x = m_pPoints[i].x-offset.cx;
		LONG y = m_pPoints[i].y-offset.cy;
		sha1.process_bytes(&x, sizeof(int));
		sha1.process_bytes(&y, sizeof(int));
	}
}
void CBezier::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	DWORD reserved1 = 0B00000000000000000000000000000100;
	if(ar.IsStoring())
	{
		if(m_geogroid != nullptr)
			reserved1 |= 0B00000000000000000000000000000001;
		if(m_labeloid != nullptr)
			reserved1 |= 0B00000000000000000000000000000010;
		ar << reserved1;
		ar << m_nAnchors;
		ar.Write(m_pPoints, m_nAnchors*3*sizeof(CPoint));
		ar << m_centroid;
		if(m_geogroid != nullptr)
			ar << *m_geogroid;
		if(m_labeloid != nullptr)
			ar << *m_labeloid;
	}
	else
	{
		ar >> reserved1;
		ar >> m_nAnchors;
		m_nAllocs = (m_nAnchors + 1)*3;
		delete[] m_pPoints;
		m_pPoints = new CPoint[m_nAllocs];
		ar.Read(m_pPoints, m_nAnchors*3*sizeof(CPoint));
		ar >> m_centroid;
		if(m_bClosed == true)
		{
			if(m_nAnchors < 3)
				m_bClosed = false;
			else if(m_pPoints[1] != m_pPoints[(m_nAnchors - 1)*3 + 1])
			{
				m_nAnchors++;
				m_pPoints[(m_nAnchors - 1)*3 + 0] = m_pPoints[0];
				m_pPoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[1];
				m_pPoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
			}
		}
		if((reserved1 & 0B00000000000000000000000000000001) == 0B00000000000000000000000000000001)
		{
			m_geogroid = new CPoint();
			ar >> *m_geogroid;
		}
		if((reserved1 & 0B00000000000000000000000000000010) == 0B00000000000000000000000000000010)
		{
			m_labeloid = new CPoint();
			ar >> *m_labeloid;
		}
	}
}

void CBezier::ReleaseCE(CArchive& ar) const
{
	CGeom::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar << m_nAnchors;

		for(unsigned int i = 0; i < m_nAnchors*3; i++)
		{
			CPoint point = m_pPoints[i];
			point.x /= 10000;
			point.y /= 10000;
			ar << point.x;
			ar << point.y;
		}
	}
	else
	{
	}
}

void CBezier::ReleaseDCOM(CArchive& ar)
{
	CGeom::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_nAnchors;
		ar.Write(m_pPoints, m_nAnchors*3*sizeof(CPoint));
	}
	else
	{
		ar >> m_nAnchors;
		m_pPoints = new CPoint[(m_nAnchors + 1)*3];
		ar.Read(m_pPoints, m_nAnchors*3*sizeof(CPoint));
	}
}

void CBezier::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
	const Gdiplus::Point* points1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints, m_nAnchors*3);
	CPoint* points2 = new CPoint[m_nAnchors*3];
	::memcpy(points2, points1, sizeof(Gdiplus::Point)*m_nAnchors*3);
	::delete[] points1;

	int radius = (nTimes % 5)*2;

	CClientDC dc(pWnd);
	CPen pen;
	pen.CreatePen(PS_SOLID, (nTimes % 5)*2, RGB(255, 0, 0));
	CPen* oldpen = dc.SelectObject(&pen);

	if(nTimes % 5 == 0)
	{
		CPen pen1;
		pen1.CreatePen(PS_SOLID, 8, RGB(255, 255, 255));
		CPen* oldpen1 = dc.SelectObject(&pen1);

		dc.PolyBezier(points2 + 1, m_nAnchors*3 - 2);

		dc.SelectObject(oldpen1);
		pen1.DeleteObject();
	}

	dc.PolyBezier(points2 + 1, m_nAnchors*3 - 2);

	dc.SelectObject(oldpen);
	pen.DeleteObject();

	delete[] points2;
	points2 = nullptr;
}

void CBezier::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	if(m_nAnchors < 2)
		return;

	Gdiplus::PointF* points = CGeom::Transform(matrix, m_pPoints, m_nAnchors*3);
	if(m_bClosed == true)
	{
		path.AddBeziers(points + 1, m_nAnchors*3 - 2);
		path.CloseFigure();
	}
	else
	{
		path.AddBeziers(points + 1, m_nAnchors*3 - 2);
	}
	::delete[] points;
	points = nullptr;
}

void CBezier::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	if(m_nAnchors < 2)
		return;

	Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, m_nAnchors*3);
	if(m_bClosed == true)
	{
		path.AddBeziers(points + 1, m_nAnchors*3 - 2);
		path.CloseFigure();
	}
	else
	{
		path.AddBeziers(points + 1, m_nAnchors*3 - 2);
	}
	::delete[] points;
	points = nullptr;
}

void CBezier::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned long& inflate) const
{
	if(pWnd == nullptr)
		return;

	//if(pWnd->IsKindOf(RUNTIME_CLASS(CScrollView)) == FALSE)
	//{
	//	CRect rect;
	//	pWnd->GetClientRect(rect);
	//	pWnd->InvalidateRect(rect,FALSE);
	//	return;
	//}
	if(m_bClosed == true)
	{
		return CGeom::Invalidate(pWnd, viewinfo, inflate);
	}
	else if(m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i < m_nAnchors; i++)
		{
			const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(GetAnchor(i));
			const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(i - 1)*3 + 2]);
			const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(i + 1 - 1)*3 + 0]);
			const Gdiplus::Point point4 = viewinfo.DocToClient<Gdiplus::Point>(GetAnchor(i + 1));

			CPoint fpoint = CPoint(point1.X, point1.Y);
			CPoint fctrol = CPoint(point2.X, point2.Y);
			CPoint tctrol = CPoint(point3.X, point3.Y);
			CPoint tpoint = CPoint(point4.X, point4.Y);

			CRect rect = LBezier::Box(fpoint, fctrol, tctrol, tpoint);
			rect.InflateRect(inflate, inflate);
			pWnd->InvalidateRect(rect, FALSE);
		}
	}
}

void CBezier::DrawFATToTAT(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const Gdiplus::Pen* pen, const unsigned int& fAnchor, const double& st, const unsigned int& tAnchor, const double& et) const
{
	if(tAnchor < fAnchor)
		return;
	if(tAnchor == fAnchor && et <= st)
		return;
	if(pen == nullptr)
		return;

	CPoint fPoint[4];
	fPoint[0] = m_pPoints[(fAnchor - 1)*3 + 1];
	fPoint[1] = m_pPoints[(fAnchor - 1)*3 + 2];
	fPoint[2] = m_pPoints[(fAnchor + 1 - 1)*3];
	fPoint[3] = m_pPoints[(fAnchor + 1 - 1)*3 + 1];

	CPoint tPoint[3];
	tPoint[0] = m_pPoints[(tAnchor - 1)*3 + 2];
	tPoint[1] = m_pPoints[(tAnchor + 1 - 1)*3];
	tPoint[2] = m_pPoints[(tAnchor + 1 - 1)*3 + 1];

	if(st != 0)
	{
		LBezier segment(m_pPoints[(fAnchor - 1)*3 + 1], m_pPoints[(fAnchor - 1)*3 + 2], m_pPoints[(fAnchor + 1 - 1)*3], m_pPoints[(fAnchor + 1 - 1)*3 + 1]);

		LBezier L, R;
		segment.Split(L, R, st);

		fPoint[0] = R.fpoint;
		fPoint[1] = R.fctrol;
		fPoint[2] = R.tctrol;
	}

	if(et != 1.0f)
	{
		if(fAnchor == tAnchor)
		{
			LBezier segment(fPoint[0], fPoint[1], fPoint[2], fPoint[3]);
			LBezier L, R;

			double t = 1.0f - (1.0f - et)/(1.0f - st);
			segment.Split(L, R, t);

			fPoint[1] = L.fctrol;
			fPoint[2] = L.tctrol;
			fPoint[3] = L.tpoint;
		}
		else
		{
			LBezier segment(m_pPoints[(tAnchor - 1)*3 + 1], m_pPoints[(tAnchor - 1)*3 + 2], m_pPoints[(tAnchor + 1 - 1)*3 + 0], m_pPoints[(tAnchor + 1 - 1)*3 + 1]);
			LBezier L, R;

			segment.Split(L, R, et);

			tPoint[0] = L.fctrol;
			tPoint[1] = L.tctrol;
			tPoint[2] = L.tpoint;
		}
	}

	Gdiplus::PointF* fPoints = viewinfo.DocToClient<Gdiplus::PointF>(fPoint, 4);
	g.DrawBezier(pen, fPoints[0], fPoints[1], fPoints[2], fPoints[3]);
	::delete[] fPoints;

	if(tAnchor > fAnchor + 1)
	{
		Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints + (fAnchor + 1 - 1)*3 + 1, (tAnchor - fAnchor)*3 - 2);
		g.DrawBeziers(pen, points, (tAnchor - fAnchor)*3 - 2);
		::delete[] points;
	}
	if(tAnchor > fAnchor)
	{
		Gdiplus::PointF* tPoints = viewinfo.DocToClient<Gdiplus::PointF>(tPoint, 3);
		Gdiplus::PointF t1 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints[(tAnchor - 1)*3 + 1]);
		g.DrawBezier(pen, t1, tPoints[0], tPoints[1], tPoints[2]);
		::delete[] tPoints;
	}
}

void CBezier::DrawPentip(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor) const
{
	Gdiplus::Point points[3];
	points[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3]);
	points[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 1]);
	points[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 2]);

	pDC->MoveTo(points[0].X, points[0].Y);
	pDC->LineTo(points[1].X, points[1].Y);
	pDC->MoveTo(points[2].X, points[2].Y);
	pDC->LineTo(points[1].X, points[1].Y);

	CRect rect = CRect(CPoint(points[1].X, points[1].Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	pDC->Rectangle(rect);

	if(points[0].Equals(points[1]) == FALSE)
		CBezier::DrawCross(pDC, points[0]);
	if(points[2].Equals(points[1]) == FALSE)
		CBezier::DrawCross(pDC, points[2]);
}

void CBezier::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	Gdiplus::Point* points1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints, m_nAnchors*3);
	CPoint* points2 = new CPoint[m_nAnchors*3];
	::memcpy(points2, points1, sizeof(Gdiplus::Point)*m_nAnchors*3);
	::delete[] points1;

	pDC->PolyBezier(points2 + 1, m_nAnchors*3 - 2);
	delete[] points2;

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

CPoly* CBezier::GetByFHTToTHT(unsigned int fAnchor, double st, unsigned int tAnchor, double et) const
{
	CBezier* bezier = new CBezier();
	bezier->m_pPoints = new CPoint[(tAnchor + 1 - fAnchor + 1 + 1)*3];
	bezier->m_nAnchors = tAnchor + 1 - fAnchor + 1;
	bezier->m_nAllocs = (tAnchor + 1 - fAnchor + 1 + 1)*3;

	for(unsigned int i = fAnchor; i <= tAnchor + 1; i++)
	{
		bezier->m_pPoints[(i - fAnchor)*3] = m_pPoints[(i - 1)*3];
		bezier->m_pPoints[(i - fAnchor)*3 + 1] = m_pPoints[(i - 1)*3 + 1];
		bezier->m_pPoints[(i - fAnchor)*3 + 2] = m_pPoints[(i - 1)*3 + 2];
	}

	if(st != 0)
	{
		const LBezier* segment = (LBezier*)GetSegment(fAnchor);

		LBezier L, R;
		segment->Split(L, R, st);
		bezier->m_pPoints[0] = R.fpoint;
		bezier->m_pPoints[1] = R.fpoint;
		bezier->m_pPoints[2] = R.fctrol;
		bezier->m_pPoints[3] = R.tctrol;

		delete segment;
	}

	if(et != 1)
	{
		const LBezier* segment = (LBezier*)bezier->GetSegment(tAnchor - fAnchor + 1);
		LBezier L, R;

		if(fAnchor == tAnchor)
		{
			const double t = 1.f - (1.0f - et)/(1.0f - st);
			segment->Split(L, R, t);
		}
		else
		{
			segment->Split(L, R, et);
		}

		bezier->m_pPoints[(tAnchor - fAnchor + 1)*3 - 1] = L.fctrol;
		bezier->m_pPoints[(tAnchor - fAnchor + 1)*3] = L.tctrol;
		bezier->m_pPoints[(tAnchor - fAnchor + 1)*3 + 1] = L.tpoint;
		bezier->m_pPoints[(tAnchor - fAnchor + 1)*3 + 2] = L.tpoint;
		delete segment;
	}

	return bezier;
}

void CBezier::DrawBezierTrack(CWnd* pWnd, const CViewinfo& viewinfo, CPoint points1[], CPoint points2[])
{
	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	const int OldROP = dc.SetROP2(R2_NOTXORPEN);

	Gdiplus::Point points[4];
	points[0] = viewinfo.DocToClient<Gdiplus::Point>(points1[0]);
	points[1] = viewinfo.DocToClient<Gdiplus::Point>(points1[1]);
	points[2] = viewinfo.DocToClient<Gdiplus::Point>(points2[0]);
	points[3] = viewinfo.DocToClient<Gdiplus::Point>(points2[1]);

	dc.MoveTo(points[0].X, points[0].Y);
	CPoint del[3];
	del[0].x = points[1].X;
	del[0].y = points[1].Y;
	del[1].x = points[2].X;
	del[1].y = points[2].Y;
	del[2].x = points[3].X;
	del[2].y = points[3].Y;
	dc.PolyBezierTo(del, 3);

	dc.SelectObject(OldPen);
	dc.SelectObject(OldBrush);
	dc.SetROP2(OldROP);
}

void CBezier::DrawSquareAnchor(CWnd* pWnd, const CViewinfo& viewinfo, CPoint point[])
{
	CPoly::DrawSquareAnchor(pWnd, viewinfo, point[1]);

	CClientDC dc(pWnd);
	CPen* OldPen = (CPen*)dc.SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	int OldROP = dc.SetROP2(R2_NOTXORPEN);
	const Gdiplus::Point cliStart = viewinfo.DocToClient<Gdiplus::Point>(point[0]);
	const Gdiplus::Point cliCenter = viewinfo.DocToClient<Gdiplus::Point>(point[1]);
	const Gdiplus::Point cliEnd = viewinfo.DocToClient<Gdiplus::Point>(point[2]);

	dc.MoveTo(cliStart.X, cliStart.Y);
	dc.LineTo(cliCenter.X, cliCenter.Y);
	dc.MoveTo(cliEnd.X, cliEnd.Y);
	dc.LineTo(cliCenter.X, cliCenter.Y);

	CBezier::DrawCross(&dc, cliStart);
	CBezier::DrawCross(&dc, cliEnd);
}

void CBezier::DrawCross(CDC* pDC, const Gdiplus::Point point)
{
	pDC->MoveTo(point.X + 2, point.Y);
	pDC->LineTo(point.X + 4, point.Y);
	pDC->MoveTo(point.X - 2, point.Y);
	pDC->LineTo(point.X - 4, point.Y);
	pDC->MoveTo(point.X, point.Y - 2);
	pDC->LineTo(point.X, point.Y - 4);
	pDC->MoveTo(point.X, point.Y + 2);
	pDC->LineTo(point.X, point.Y + 4);
}

void CBezier::RecalRect()
{
	CGeom::RecalRect();

	CRect allRect(m_pPoints[1], CSize(0, 0));
	for(unsigned int nAnchor = 1; nAnchor <= m_nAnchors - 1; nAnchor++)
	{
		const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(nAnchor + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(nAnchor + 1 - 1)*3 + 1];

		CRect oneRect = LBezier::Box(fpoint, fctrol, tctrol, tpoint);
		allRect = UnionRect(allRect, oneRect);
	}

	allRect.NormalizeRect();
	m_Rect = allRect;
}
CPoint CBezier::FindCentroid() const
{
	if(m_bClosed == true)
	{
		double area = 0;
		double x = 0;
		double y = 0;
		for(unsigned int index = 1; index < m_nAnchors; index++)
		{
			CPoint point1 = this->GetAnchor(index);
			CPoint point2 = this->GetAnchor(index + 1);
			point1.Offset(-m_Rect.TopLeft());
			point2.Offset(-m_Rect.TopLeft());
			const double factor = (double)point1.x*point2.y - (double)point2.x*point1.y;
			x += point1.x*factor + point2.x*factor;
			y += point1.y*factor + point2.y*factor;
			area += factor/2;
		}
		CPoint centrioid;
		centrioid.x = m_Rect.left + (area == 0 ? (m_Rect.right - m_Rect.left)/2 : x/(6*area));
		centrioid.y = m_Rect.top + (area == 0 ? (m_Rect.bottom - m_Rect.top)/2 : y/(6*area));
		if(this->PickIn(centrioid) == FALSE)
		{
			int maxlen = 0;
			int x = centrioid.x;
			int y = centrioid.y;

			std::vector<int> vertices1 = XRadialBezier(Direction::DI_Horizontal, centrioid, this->m_pPoints, this->m_nAnchors);
			std::sort(vertices1.begin(), vertices1.end());
			for(long k = 0; k < vertices1.size()/2; k++)
			{
				const long x1 = vertices1[k*2];
				const long x2 = vertices1[k*2 + 1];
				if(abs(x1 - x2) > maxlen)
				{
					maxlen = abs(x2 - x1);
					x = (x1 + x2)/2;
					y = centrioid.y;
				}
			}

			std::vector<int> vertices2 = YRadialBezier(Direction::DI_Vertical, centrioid, this->m_pPoints, this->m_nAnchors);
			std::sort(vertices2.begin(), vertices2.end());
			for(long k = 0; k < vertices2.size()/2; k++)
			{
				const long y1 = vertices2[k*2];
				const long y2 = vertices2[k*2 + 1];
				if(abs(y2 - y1) > maxlen)
				{
					maxlen = abs(y2 - y1);
					y = (y1 + y2)/2;
					x = centrioid.x;
				}
			}
			return CPoint(x, y);
		}
		else
			return centrioid;
	}
	else
	{
		unsigned long length = GetLength(100.0f/2);
		length = length/2;
		unsigned int nAnchor;
		double t;

		GetAandT(1, 0, length, nAnchor, t);

		return GetPoint(nAnchor, t);
	}
}

CRect CBezier::GetSegRect(const unsigned int& nAnchor) const
{
	if(nAnchor < 1 || nAnchor >= m_nAnchors)
		return CRect(0,0,0,0);

	const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
	const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
	const CPoint& tctrol = m_pPoints[(nAnchor + 1 - 1)*3];
	const CPoint& tpoint = m_pPoints[(nAnchor + 1 - 1)*3 + 1];

	return LBezier::Box(fpoint, fctrol, tctrol, tpoint);
}

LLine* CBezier::GetSegment(const unsigned int& nAnchor) const
{
	if(nAnchor < 1 || nAnchor >= m_nAnchors)
		return nullptr;

	const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
	const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
	const CPoint& tctrol = m_pPoints[(nAnchor + 1 - 1)*3];
	const CPoint& tpoint = m_pPoints[(nAnchor + 1 - 1)*3 + 1];

	LBezier* lbezier = new LBezier(fpoint, fctrol, tctrol, tpoint);
	return lbezier;
}

CPoint& CBezier::GetAnchor(const unsigned int& nAnchor) const
{
	ASSERT(nAnchor >= 1 && nAnchor <= m_nAnchors);
	return m_pPoints[(nAnchor - 1)*3 + 1];
}

CPoint CBezier::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	ASSERT(nAnchor >= 1);
	ASSERT(nAnchor < m_nAnchors || (t == 0.0f && nAnchor == m_nAnchors));
	ASSERT(t >= 0 && t <= 1);

	if(t == 0.0f)
		return m_pPoints[(nAnchor - 1)*3 + 1];
	if(t == 1.0f)
		return m_pPoints[(nAnchor + 1 - 1)*3 + 1];
	const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
	const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
	const CPoint& tctrol = m_pPoints[(nAnchor + 1 - 1)*3];
	const CPoint& tpoint = m_pPoints[(nAnchor + 1 - 1)*3 + 1];

	return BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
}

double CBezier::GetAngle(const unsigned int& nAnchor, const double& t) const
{
	const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
	const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
	const CPoint& tctrol = m_pPoints[(nAnchor - 1 + 1)*3];
	const CPoint& tpoint = m_pPoints[(nAnchor - 1 + 1)*3 + 1];
	const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, t);
	const double Dx = derivate.cx;
	const double Dy = derivate.cy;
	if(Dy == 0.0f && Dx == 0.0f)
		return 0;

	double dAngle = atan2((double)Dy, (double)Dx);
	if(dAngle < 0.0f)
		dAngle += 2*M_PI;

	return dAngle;
}

CPoint CBezier::GetSample(const short& direction, const CPoint& center, const long& radius) const
{
	CPoint point = direction == 1 ? GetAnchor(1) : GetAnchor(m_nAnchors);

	double distance = sqrt(pow((double)(point.x - center.x), 2) + pow((double)(point.y - center.y), 2));
	if(distance > radius)
	{
		return point;
	}

	unsigned int tAnchor = direction == 1 ? 1 : m_nAnchors;

	do
	{
		tAnchor = tAnchor + direction;
		if(tAnchor < 1 || tAnchor > m_nAnchors)
		{
			return point;
		}

		point = GetAnchor(tAnchor);
		distance = sqrt(pow((double)(point.x - center.x), 2) + pow((double)(point.y - center.y), 2));
	} while(distance < radius);
	const unsigned int fAnchor = tAnchor - direction;

	CPoint fpoint;
	CPoint fctrol;
	CPoint tctrol;
	CPoint tpoint;
	double st = 0.0f;
	double et = 1.0f;
	if(direction == 1)
	{
		fpoint = m_pPoints[(fAnchor - 1)*3 + 1];
		fctrol = m_pPoints[(fAnchor - 1)*3 + 2];
		tctrol = m_pPoints[(tAnchor - 1)*3 + 0];
		tpoint = m_pPoints[(tAnchor - 1)*3 + 1];

		st = 0;
		et = 1;
	}
	if(direction == -1)
	{
		fpoint = m_pPoints[(tAnchor - 1)*3 + 1];
		fctrol = m_pPoints[(tAnchor - 1)*3 + 2];
		tctrol = m_pPoints[(fAnchor - 1)*3 + 0];
		tpoint = m_pPoints[(fAnchor - 1)*3 + 1];

		st = 0;
		et = 1;
	}

	fpoint = fpoint - center;
	fctrol = fctrol - center;
	tctrol = tctrol - center;
	tpoint = tpoint - center;

	do
	{
		const double t = (st + et)/2;
		point = BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
		const long distance = ::lround(sqrt(pow((double)point.x, 2) + pow((double)point.y, 2)));
		point = point + center;

		if(distance >= radius && distance - radius < (radius/10000 + 1))
			break;

		if(distance > radius)
		{
			if(direction == 1)
				et = t;
			if(direction == -1)
				st = t;
		}
		if(distance < radius)
		{
			if(direction == 1)
				st = t;
			if(direction == -1)
				et = t;
		}
	} while(st != et);

	return point;
}

unsigned long CBezier::GetLength(const double& tolerance) const
{
	unsigned long length = 0;
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		LBezier lbezier(fpoint, fctrol, tctrol, tpoint);
		length += lbezier.GetLength(tolerance);
	}

	return length;
}

double CBezier::GetLength(const CDatainfo& datainfo, const double& tolerance) const
{
	float length = 0;
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];;
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		LBezier lbezier(fpoint, fctrol, tctrol, tpoint);
		length += lbezier.GetLength(tolerance);
	}

	return ::round(length);
}

double CBezier::GetArea() const
{
	if(m_bClosed == true)
	{
		long minY = 0X0FFFFFFF;
		for(unsigned int nAnchor = 1; nAnchor <= m_nAnchors; nAnchor++)
		{
			minY = min(minY, m_pPoints[(nAnchor - 1)*3 + 1].y);
		}

		double area = 0;
		for(unsigned int i = 1; i < m_nAnchors; i++)
		{
			const double a = (m_pPoints[i*3 + 1].x - m_pPoints[(i - 1)*3 + 1].x);
			const double b = (m_pPoints[(i - 1)*3 + 1].y + m_pPoints[i*3 + 1].y - 2*minY)/2;

			area += a*b;
		}

		return area;
	}
	else
		return 0.0f;
}

double CBezier::GetArea(const CDatainfo& datainfo) const
{
	return 0;
}

bool CBezier::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor >= m_nAnchors)
	{
		tAnchor = m_nAnchors;
		et = 0;
		return false;
	}
	else if(length == 0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	for(unsigned int i = fAnchor; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		double leftlen = 0.0f;
		if(fpoint == fctrol && tctrol == tpoint)
		{
			const double len = pow((double)(tpoint.x - fpoint.x), 2) + pow((double)(tpoint.y - fpoint.y), 2);
			const double segment = sqrt(len);
			leftlen = segment - st*segment;
			if(length < leftlen)
			{
				tAnchor = i;
				et = st + (double)length/segment;

				return true;
			}
		}
		else
		{
			LBezier lbezier(fpoint, fctrol, tctrol, tpoint);

			double tolerance = 10000.0f/2;
			const unsigned long segment = lbezier.GetLength(tolerance);
			const unsigned long lastlen = lbezier.GetLengthByT(st, tolerance);
			leftlen = segment - lastlen;
			if(length - leftlen < tolerance)
			{
				length = lastlen + length;

				tAnchor = i;
				et = lbezier.GetTByLength(length, tolerance);

				return true;
			}
		}
		if(abs(length - leftlen) <= 0.5)
		{
			tAnchor = i;
			et = 1;

			return true;
		}
		length -= leftlen;
		st = 0;
	}

	tAnchor = m_nAnchors - 1;
	et = 1;

	return false;
}
// position must be in logical coordinates
void CBezier::Move(const int& dx, const int& dy)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		m_pPoints[i].x += dx;
		m_pPoints[i].y += dy;
	}
	m_centroid.x += dx;
	m_centroid.y += dy;
	if(m_geogroid != nullptr)
	{
		m_geogroid->x += dx;
		m_geogroid->y += dy;
	}
	if(m_labeloid != nullptr)
	{
		m_labeloid->x += dx;
		m_labeloid->y += dy;
	}
	CGeom::Move(dx, dy);
}
// position must be in logical coordinates
void CBezier::Move(const CSize& Δ)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		m_pPoints[i].x += Δ.cx;
		m_pPoints[i].y += Δ.cy;
	}
	m_centroid.x += Δ.cx;
	m_centroid.y += Δ.cy;
	if(m_geogroid != nullptr)
	{
		m_geogroid->x += Δ.cx;
		m_geogroid->y += Δ.cy;
	}
	if(m_labeloid != nullptr)
	{
		m_labeloid->x += Δ.cx;
		m_labeloid->y += Δ.cy;
	}
	CGeom::Move(Δ);
}

void CBezier::ChangeAnchor(const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch, Undoredo::CActionStack* pActionstack)
{
	std::map<unsigned int, CPoint> oldpoints;
	std::map<unsigned int, CPoint> newpoints;
	if(nAnchor != 0)
	{
		oldpoints[(nAnchor - 1)*3] = m_pPoints[(nAnchor - 1)*3];
		oldpoints[(nAnchor - 1)*3 + 1] = m_pPoints[(nAnchor - 1)*3 + 1];
		oldpoints[(nAnchor - 1)*3 + 2] = m_pPoints[(nAnchor - 1)*3 + 2];

		m_pPoints[(nAnchor - 1)*3].x += Δ.cx;
		m_pPoints[(nAnchor - 1)*3].y += Δ.cy;
		m_pPoints[(nAnchor - 1)*3 + 1].x += Δ.cx;
		m_pPoints[(nAnchor - 1)*3 + 1].y += Δ.cy;
		m_pPoints[(nAnchor - 1)*3 + 2].x += Δ.cx;
		m_pPoints[(nAnchor - 1)*3 + 2].y += Δ.cy;

		newpoints[(nAnchor - 1)*3] = m_pPoints[(nAnchor - 1)*3];
		newpoints[(nAnchor - 1)*3 + 1] = m_pPoints[(nAnchor - 1)*3 + 1];
		newpoints[(nAnchor - 1)*3 + 2] = m_pPoints[(nAnchor - 1)*3 + 2];
	}
	if(m_bClosed == true && nAnchor == 1)
	{
		oldpoints[(m_nAnchors - 1)*3] = m_pPoints[(m_nAnchors - 1)*3];
		oldpoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[(m_nAnchors - 1)*3 + 1];
		oldpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[(m_nAnchors - 1)*3 + 2];

		m_pPoints[(m_nAnchors - 1)*3].x += Δ.cx;
		m_pPoints[(m_nAnchors - 1)*3].y += Δ.cy;
		m_pPoints[(m_nAnchors - 1)*3 + 1].x += Δ.cx;
		m_pPoints[(m_nAnchors - 1)*3 + 1].y += Δ.cy;
		m_pPoints[(m_nAnchors - 1)*3 + 2].x += Δ.cx;
		m_pPoints[(m_nAnchors - 1)*3 + 2].y += Δ.cy;

		newpoints[(m_nAnchors - 1)*3] = m_pPoints[(m_nAnchors - 1)*3];
		newpoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[(m_nAnchors - 1)*3 + 1];
		newpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[(m_nAnchors - 1)*3 + 2];
	}
	if(pActionstack != nullptr)
	{
		Undoredo::CModify<CWnd*, CPoly*, std::map<unsigned int, CPoint>>* pModify = new Undoredo::CModify<CWnd*, CPoly*, std::map<unsigned int, CPoint>>(nullptr, this, oldpoints, newpoints);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Anchors;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Anchors;
		pActionstack->Record(pModify);
	}
	RecalRect();

	m_bModified = true;
}

bool CBezier::ChangeContrl(std::pair<unsigned int, BYTE> handle, const CSize& Δ)
{
	if(Δ == CSize(0, 0))
		return false;

	CPoint ctrl1 = m_pPoints[(handle.first - 1)*3];
	CPoint ctrl2 = m_pPoints[(handle.first - 1)*3 + 2];
	ctrl1 = ctrl1 + m_pPoints[(handle.first - 1)*3 + 1];
	ctrl2 = ctrl2 + m_pPoints[(handle.first - 1)*3 + 1];

	if(handle.second == (BYTE)-1)
	{
		//		if(ctrl1.x*ctrl2.y == ctrl1.y*ctrl2.x)
		{
			m_pPoints[(handle.first - 1)*3 + 2].x -= Δ.cx;
			m_pPoints[(handle.first - 1)*3 + 2].y -= Δ.cy;
		}

		m_pPoints[(handle.first - 1)*3].x += Δ.cx;
		m_pPoints[(handle.first - 1)*3].y += Δ.cy;
	}
	else if(handle.second == (BYTE)1)
	{
		//		if(ctrl1.x*ctrl2.y == ctrl1.y*ctrl2.x)
		{
			m_pPoints[(handle.first - 1)*3].x -= Δ.cx;
			m_pPoints[(handle.first - 1)*3].y -= Δ.cy;
		}

		m_pPoints[(handle.first - 1)*3 + 2].x += Δ.cx;
		m_pPoints[(handle.first - 1)*3 + 2].y += Δ.cy;
	}

	//	m_pPoints[(nAnchor-1)*3  ].Offset(handle.second ==-1 ? Δ : -Δ);
	//	m_pPoints[(nAnchor-1)*3+2].Offset(handle.second == 1 ? Δ : -Δ);

	if(m_bClosed == true && handle.first == 1)
	{
		m_pPoints[(m_nAnchors - 1)*3] = m_pPoints[0];
		m_pPoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
	}

	RecalRect();

	m_bModified = true;
	return true;
}

void CBezier::SetContrl(const unsigned int& nAnchor, const BYTE& nCtrle, const CPoint& point, bool ignore)
{
	if(nAnchor < 1)
		return;
	if(nAnchor > m_nAnchors)
		return;

	if(nCtrle == (BYTE)-1)
	{
		m_pPoints[(nAnchor - 1)*3 + 0] = point;
	}
	else if(nCtrle == (BYTE)1)
	{
		m_pPoints[(nAnchor - 1)*3 + 2] = point;
	}
	if(ignore == false)
	{
		RecalRect();
	}
	m_bModified = true;
}
void CBezier::MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch)
{
	ASSERT(nAnchor >= 1 && nAnchor <= m_nAnchors);

	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	if(nAnchor != 1 && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 2)*3 + 1]);
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 2)*3 + 2]);
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3] + CPoint(Δ.cx, Δ.cy));
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 1] + CPoint(Δ.cx, Δ.cy));
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}
	else if(m_bClosed == true && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1 - 1)*3 + 1]);
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1 - 1)*3 + 2]);
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1)*3] + CPoint(Δ.cx, Δ.cy));
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1)*3 + 1] + CPoint(Δ.cx, Δ.cy));
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}

	if(nAnchor != m_nAnchors && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 1] + CPoint(Δ.cx, Δ.cy));
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 2] + CPoint(Δ.cx, Δ.cy));
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor*3]);
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[nAnchor*3 + 1]);
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}

	Gdiplus::Point cliStart = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3] + Δ);
	const Gdiplus::Point cliCenter = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 1] + Δ);
	Gdiplus::Point cliEnd = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(nAnchor - 1)*3 + 2] + Δ);

	pDC->MoveTo(cliStart.X, cliStart.Y);
	pDC->LineTo(cliCenter.X, cliCenter.Y);
	pDC->MoveTo(cliEnd.X, cliEnd.Y);
	pDC->LineTo(cliCenter.X, cliCenter.Y);

	CRect rect = CRect(CPoint(cliCenter.X, cliCenter.Y), CSize(0, 0));
	rect.InflateRect(3, 3);
	pDC->Rectangle(rect);

	if(cliStart.Equals(cliCenter) == FALSE)
		CBezier::DrawCross(pDC, cliStart);
	if(cliEnd.Equals(cliCenter) == FALSE)
		CBezier::DrawCross(pDC, cliEnd);

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

void CBezier::MoveContrl(CDC* pDC, const CViewinfo& viewinfo, std::pair<unsigned int, BYTE> handle, const CSize& Δ)
{
	ASSERT(handle.first >= 1 && handle.first <= m_nAnchors);
	ASSERT(handle.second == (BYTE)1 || handle.second == (BYTE)-1);

	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	if(handle.first != 1 && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 2)*3 + 1]);
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 2)*3 + 2]);
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 1)*3] + (handle.second == (BYTE)-1 ? CPoint(Δ.cx, Δ.cy) : CPoint(-Δ.cx, -Δ.cy)));
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 1)*3 + 1]);
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}
	else if(m_bClosed == true && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1 - 1)*3 + 1]);
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1 - 1)*3 + 2]);
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1)*3] + (handle.second == (BYTE)-1 ? CPoint(Δ.cx, Δ.cy) : CPoint(-Δ.cx, -Δ.cy)));
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(m_nAnchors - 1)*3 + 1]);
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}

	if(handle.first != m_nAnchors && Δ != CSize(0, 0))
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 1)*3 + 1]);
		pDC->MoveTo(point.X, point.Y);

		Gdiplus::Point del1[3];
		del1[0] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[(handle.first - 1)*3 + 2] + (handle.second == (BYTE)1 ? CPoint(Δ.cx, Δ.cy) : CPoint(-Δ.cx, -Δ.cy)));
		del1[1] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[handle.first*3]);
		del1[2] = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[handle.first*3 + 1]);
		CPoint del2[3];
		memcpy(del2, del1, sizeof(Gdiplus::Point)*3);
		pDC->PolyBezierTo(del2, 3);
	}

	CPoint start = m_pPoints[(handle.first - 1)*3];
	CPoint end = m_pPoints[(handle.first - 1)*3 + 2];
	const CPoint& center = m_pPoints[(handle.first - 1)*3 + 1];

	start.x += (handle.second == (BYTE)-1 ? Δ.cx : -Δ.cx);
	start.y += (handle.second == (BYTE)-1 ? Δ.cy : -Δ.cy);

	end.x += (handle.second == (BYTE)1 ? Δ.cx : -Δ.cx);
	end.y += (handle.second == (BYTE)1 ? Δ.cy : -Δ.cy);
	const Gdiplus::Point cliStart = viewinfo.DocToClient<Gdiplus::Point>(start);
	const Gdiplus::Point cliEnd = viewinfo.DocToClient<Gdiplus::Point>(end);
	const Gdiplus::Point cliCenter = viewinfo.DocToClient<Gdiplus::Point>(center);

	pDC->MoveTo(cliStart.X, cliStart.Y);
	pDC->LineTo(cliCenter.X, cliCenter.Y);
	pDC->MoveTo(cliEnd.X, cliEnd.Y);
	pDC->LineTo(cliCenter.X, cliCenter.Y);

	if(start != center)
		DrawCross(pDC, cliStart);
	if(end != center)
		DrawCross(pDC, cliEnd);

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

bool CBezier::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CGeom::PickOn(point, gap) == false)
		return false;

	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		double t;
		CPoint dpoint = point;
		if(PointOnBezier(dpoint, fpoint, fctrol, tctrol, tpoint, gap, t))
			return true;
	}

	return false;
}

unsigned int CBezier::PickOn(const CPoint& point, const unsigned long& gap, double& t) const
{
	if(CGeom::PickOn(point, gap) == false)
		return false;

	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		CPoint dpoint = point;
		if(PointOnBezier(dpoint, fpoint, fctrol, tctrol, tpoint, gap, t))
		{
			return i;
		}
	}

	return 0;
}

bool CBezier::PickIn(const CPoint& point) const
{
	if(CGeom::PickIn(point) == false)
		return false;
	else
		return PointInBeziergon(point, m_pPoints, m_nAnchors);
}

bool CBezier::PickIn(const CRect& rect) const
{
	if(CGeom::PickIn(rect) == false)
		return false;

	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		const CPoint& point = GetAnchor(i);
		if(rect.PtInRect(point) == TRUE)
			return true;
	}

	if(m_bClosed == true)
	{
		const CPoint point1 = CPoint(rect.left, rect.top);
		const CPoint point2 = CPoint(rect.left, rect.bottom);
		const CPoint point3 = CPoint(rect.right, rect.top);
		const CPoint point4 = CPoint(rect.right, rect.bottom);
		if(this->PickIn(point1) == true || this->PickIn(point2) == true || this->PickIn(point3) == true || this->PickIn(point4) == true)
		{
			return true;
		}
	}
	/*
	for( i=1;i<m_nAnchors;i++)
	{
	long x1 = m_pPoints[i-1].x;
	long y1 = m_pPoints[i-1].y;
	long x2 = m_pPoints[i  ].x;
	long y2 = m_pPoints[i  ].y;

	for(long j=1; j<5; j++)
	{
	long u1 = j==1 || j==4 ? rect.left  : rect.right;
	long v1 = j==1 || j==2 ? rect.top   : rect.bottom;
	long u2 = j==1 || j==2 ? rect.right : rect.left;
	long v2 = j==1 || j==4 ? rect.top   : rect.bottom;

	if((u1==u2)&&(v1==v2))
	continue;

	long a1 = (y2-v1)*(x2-x1) > (y2-y1)*(x2-u1)  ? 1 : ((y2-v1)*(x2-x1) == (y2-y1)*(x2-u1) ? 0 : -1);
	long a2 = (y2-v2)*(x2-x1) > (y2-y1)*(x2-u2)  ? 1 : ((y2-v2)*(x2-x1) == (y2-y1)*(x2-u2) ? 0 : -1);
	long a3 = (v2-y1)*(u2-u1) > (v2-v1)*(u2-x1)  ? 1 : ((v2-y1)*(u2-u1) == (v2-v1)*(u2-x1) ? 0 : -1);
	long a4 = (v2-y2)*(u2-u1) > (v2-v1)*(u2-x2)  ? 1 : ((v2-y2)*(u2-u1) == (v2-v1)*(u2-x2) ? 0 : -1);

	if(a1*a2<0 && a3*a4<0)
	{
	return true;
	}
	}
	}
	*/

	return false;
}

bool CBezier::PickIn(const CPoly& polygon) const
{
	if(CGeom::PickIn(polygon.m_Rect) == false)
		return false;

	CRect rect = polygon.m_Rect;
	for(unsigned int i = 1; i <= m_nAnchors; i++)
	{
		const CPoint& point = GetAnchor(i);
		if(polygon.PickIn(point, 0) == true)
		{
			return true;
		}
	}

	if(m_bClosed == true)
	{
		for(unsigned int j = 1; j < polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point) == true)
			{
				return true;
			}
		}
	}
	/*
	for( i=1;i<m_nAnchors;i++)
	{
	long x1 = m_pPoints[i-1].x;
	long y1 = m_pPoints[i-1].y;
	long x2 = m_pPoints[i  ].x;
	long y2 = m_pPoints[i  ].y;

	for(unsigned int j=1; j<poly.m_nAnchors; j++)
	{
	long u1 = poly.m_pPoints[j-1].x;
	long v1 = poly.m_pPoints[j-1].y;
	long u2 = poly.m_pPoints[j  ].x;
	long v2 = poly.m_pPoints[j  ].y;

	if((u1==u2)&&(v1==v2))
	continue;

	long a1 = (y2-v1)*(x2-x1) > (y2-y1)*(x2-u1)  ? 1 : ((y2-v1)*(x2-x1) == (y2-y1)*(x2-u1) ? 0 : -1);
	long a2 = (y2-v2)*(x2-x1) > (y2-y1)*(x2-u2)  ? 1 : ((y2-v2)*(x2-x1) == (y2-y1)*(x2-u2) ? 0 : -1);
	long a3 = (v2-y1)*(u2-u1) > (v2-v1)*(u2-x1)  ? 1 : ((v2-y1)*(u2-u1) == (v2-v1)*(u2-x1) ? 0 : -1);
	long a4 = (v2-y2)*(u2-u1) > (v2-v1)*(u2-x2)  ? 1 : ((v2-y2)*(u2-u1) == (v2-v1)*(u2-x2) ? 0 : -1);

	if(a1*a2<0 && a3*a4<0)
	{
	return true;
	}
	}
	}
	*/
	return false;
}

void CBezier::Reverse()
{
	for(unsigned int i = 0; i < (m_nAnchors % 2 == 0 ? m_nAnchors/2 : m_nAnchors/2 + 1); i++)
	{
		const CPoint buffer1 = m_pPoints[i*3];
		const CPoint buffer2 = m_pPoints[i*3 + 1];
		const CPoint buffer3 = m_pPoints[i*3 + 2];

		m_pPoints[i*3] = m_pPoints[(m_nAnchors - 1 - i)*3 + 2];
		m_pPoints[i*3 + 1] = m_pPoints[(m_nAnchors - 1 - i)*3 + 1];
		m_pPoints[i*3 + 2] = m_pPoints[(m_nAnchors - 1 - i)*3];

		m_pPoints[(m_nAnchors - 1 - i)*3 + 2] = buffer1;
		m_pPoints[(m_nAnchors - 1 - i)*3 + 1] = buffer2;
		m_pPoints[(m_nAnchors - 1 - i)*3] = buffer3;
	}
}

void CBezier::Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	//	coFilter(m_pPoints, m_nAnchors,m_nAllocs);
	//	RecalRect();
}

void CBezier::Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	//	coFilter(m_pPoints, m_nAnchors,m_nAllocs);
	//RecalRect();
}
void CBezier::Simplify(const unsigned int& tolerance)
{

}
void CBezier::Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms)
{

}
void CBezier::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CBezier)) == TRUE)
	{
		CBezier* pBezier = (CBezier*)pGeom;
		if(pBezier->m_pPoints == nullptr)
		{
			pBezier->m_pPoints = new CPoint[m_nAnchors*3];
			pBezier->m_nAllocs = m_nAnchors*3;
		}
		else if(pBezier->m_nAllocs < m_nAnchors*3)
		{
			delete[] pBezier->m_pPoints;
			pBezier->m_pPoints = new CPoint[m_nAnchors*3];
			pBezier->m_nAllocs = m_nAnchors*3;
		}

		if(m_pPoints != nullptr)
		{
			memcpy(pBezier->m_pPoints, m_pPoints, sizeof(CPoint)*m_nAnchors*3);
			pBezier->m_nAnchors = m_nAnchors;
		}
	}
}

bool CBezier::Close(bool pForce, const unsigned int& tolerance, Undoredo::CActionStack* pActionstack, bool ignore)
{
	if(m_bClosed == true)
		return false;
	if(m_nAnchors < 3)
		return false;
	const CPoint Δ = GetAnchor(1) - GetAnchor(m_nAnchors);
	if(abs(Δ.x) <= tolerance && abs(Δ.y) <= tolerance)
	{
		std::map<unsigned int, CPoint> oldpoints;
		std::map<unsigned int, CPoint> newpoints;

		oldpoints[(m_nAnchors - 1)*3] = m_pPoints[(m_nAnchors - 1)*3];
		oldpoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[(m_nAnchors - 1)*3 + 1];
		oldpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[(m_nAnchors - 1)*3 + 2];
		oldpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
		oldpoints[0] = m_pPoints[(m_nAnchors - 1)*3];

		m_pPoints[(m_nAnchors - 1)*3] += Δ;
		m_pPoints[(m_nAnchors - 1)*3 + 1] += Δ;
		m_pPoints[(m_nAnchors - 1)*3 + 2] += Δ;

		m_pPoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
		m_pPoints[0] = m_pPoints[(m_nAnchors - 1)*3];
		m_bClosed = true;

		newpoints[(m_nAnchors - 1)*3] = m_pPoints[(m_nAnchors - 1)*3];
		newpoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[(m_nAnchors - 1)*3 + 1];
		newpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[(m_nAnchors - 1)*3 + 2];
		newpoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
		newpoints[0] = m_pPoints[(m_nAnchors - 1)*3];
		if(pActionstack != nullptr)
		{
		Undoredo::CModify<CWnd*, CBezier*, std::map<unsigned int, CPoint>>* pModify = new Undoredo::CModify<CWnd*, CBezier*, std::map<unsigned int, CPoint>>(nullptr, this, oldpoints, newpoints);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Undo_Modify_Close;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Redo_Modify_Close;
			pActionstack->Record(pModify);
		}
	}
	else if(pForce == true)
	{
		CPoint* points = new CPoint[(m_nAnchors + 1)*3];
		memcpy(points, m_pPoints, m_nAnchors*3*sizeof(CPoint));
		m_nAnchors += 1;
		m_nAllocs = m_nAnchors*3;
		points[(m_nAnchors - 1)*3] = m_pPoints[0];
		points[(m_nAnchors - 1)*3 + 1] = m_pPoints[1];
		points[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
		delete[] m_pPoints;
		m_pPoints = points;
		m_bClosed = true;
		if(pActionstack != nullptr)
		{
		Undoredo::CModify<CWnd*, CBezier*, CPoint>* pModify = new Undoredo::CModify<CWnd*, CBezier*, CPoint>(nullptr, this, m_pPoints[0], m_pPoints[0]);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Undo_Modify_ForceClose;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Bezier::Redo_Modify_ForceClose;
			pActionstack->Record(pModify);
		}
	}
	if(ignore == false)
	{
		RecalCentroid();
	}
	return m_bClosed;
}

CPoly* CBezier::Join(const CPoly* pPoly, const BYTE& mode) const
{
	if(m_bClosed == true || pPoly->m_bClosed == true)
		return nullptr;
	if(pPoly->IsKindOf(RUNTIME_CLASS(CBezier)) == FALSE)
		return nullptr;
	if(pPoly == this)
		return nullptr;

	const CBezier* pBezier = (CBezier*)pPoly;
	
	unsigned int nPoints = 0;
	CPoint* pPoints = nullptr;
	switch(mode)
	{
		case 1:
			{
				nPoints = m_nAnchors - 1 + pBezier->m_nAnchors;
				pPoints = new CPoint[nPoints*3 + 3];

				memcpy(pPoints, pBezier->m_pPoints, sizeof(CPoint)*(pBezier->m_nAnchors*3));
				ReverseOrder(pPoints, pBezier->m_nAnchors*3);
				memcpy(pPoints + pBezier->m_nAnchors*3 - 1, m_pPoints + 2, sizeof(CPoint)*(m_nAnchors*3 - 2));
			}
			break;
		case 2:
			{
				nPoints = m_nAnchors - 1 + pBezier->m_nAnchors;
				pPoints = new CPoint[nPoints*3 + 3];

				memcpy(pPoints, pBezier->m_pPoints, sizeof(CPoint)*(pBezier->m_nAnchors*3 - 1));
				memcpy(pPoints + pBezier->m_nAnchors*3 - 1, m_pPoints + 2, sizeof(CPoint)*(m_nAnchors*3 - 2));
			}
			break;
		case 3:
			{
				nPoints = m_nAnchors + pBezier->m_nAnchors - 1;
				pPoints = new CPoint[nPoints*3 + 3];

				memcpy(pPoints, m_pPoints, sizeof(CPoint)*(m_nAnchors*3 - 1));
				memcpy(pPoints + m_nAnchors*3 - 1, pBezier->m_pPoints + 2, sizeof(CPoint)*(pBezier->m_nAnchors*3 - 2));
			}
			break;
		case 4:
			{
				nPoints = m_nAnchors + pBezier->m_nAnchors - 1;
				pPoints = new CPoint[nPoints*3 + 3];
				memcpy(pPoints + m_nAnchors*3 - 3, pBezier->m_pPoints, sizeof(CPoint)*pBezier->m_nAnchors*3);
				ReverseOrder(pPoints + m_nAnchors*3 - 3, pBezier->m_nAnchors*3);
				memcpy(pPoints, m_pPoints, sizeof(CPoint)*(m_nAnchors*3 - 1));
			}
			break;
		default:
			return nullptr;
	}
	if(nPoints >= 2 && pPoints != nullptr)
	{
		CBezier* pBezier = new CBezier();
		pBezier->m_pPoints = pPoints;
		pBezier->m_nAnchors = nPoints;
		pBezier->m_nAllocs = m_nAnchors*3;
		CGeom::CopyTo(pBezier);
		pBezier->RecalRect();
		return pBezier;
	}
	else if(pPoints != nullptr)
	{
		delete pPoints;
		pPoints = nullptr;
		return nullptr;
	}
	else
		return nullptr;
}

std::list<CGeom*> CBezier::Split(const CPoint& point, const unsigned long& gap) const
{
	std::list<CGeom*> polys;
	if(m_bLocked == true)
		return polys;

	if(!m_Rect.PtInRect(point))
		return polys;

	if(m_bClosed == false && abs(point.x - m_pPoints[1].x) < gap && abs(point.y - m_pPoints[1].y) < gap)
		return polys;
	if(m_bClosed == false && abs(point.x - m_pPoints[(m_nAnchors - 1)*3 + 1].x) < gap && abs(point.y - m_pPoints[(m_nAnchors - 1)*3 + 1].y) < gap)
		return polys;
	int x = point.x;
	int y = point.y;
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		const CPoint& fpoint = m_pPoints[(i - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(i - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[(i + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[(i + 1 - 1)*3 + 1];

		double t;
		CPoint L[4];
		CPoint R[4];

		if(abs(x - fpoint.x) < gap && abs(y - fpoint.y) < gap)
		{
			x = fpoint.x;
			y = fpoint.y;
			t = 0.0f;
			goto jump;
		}
		if(abs(x - tpoint.x) < gap && abs(y - tpoint.y) < gap)
		{
			x = tpoint.x;
			y = tpoint.y;

			t = 1.f;
			goto jump;
		}

		if(!PointOnBezier(point, fpoint, fctrol, tctrol, tpoint, gap, t))
			continue;

		SplitBezier(fpoint, fctrol, tctrol, tpoint, L, R, t);

	jump:
		if(m_bClosed == true)
		{
			CBezier* bezier = new CBezier;
			CGeom::CopyTo(bezier);
			bezier->m_bClosed = false;
			if(x == fpoint.x && y == fpoint.y)
			{
				bezier->m_nAnchors = m_nAnchors;
				bezier->m_pPoints = new CPoint[m_nAllocs];
				bezier->m_nAllocs = m_nAllocs;
				memcpy(bezier->m_pPoints, m_pPoints + (i - 1)*3, sizeof(CPoint)*(m_nAnchors - i)*3);
				memcpy(bezier->m_pPoints + (m_nAnchors - i)*3, m_pPoints, sizeof(CPoint)*i*3);
			}
			else if(x == tpoint.x && y == tpoint.y)
			{
				bezier->m_nAnchors = m_nAnchors;
				bezier->m_pPoints = new CPoint[m_nAllocs];
				bezier->m_nAllocs = m_nAllocs;
				memcpy(bezier->m_pPoints, m_pPoints + (i + 1 - 1)*3, sizeof(CPoint)*(m_nAnchors - i - 1)*3);
				memcpy(bezier->m_pPoints + (m_nAnchors - i - 1)*3, m_pPoints, sizeof(CPoint)*(i + 1)*3);
			}
			else
			{
				bezier->m_nAnchors = m_nAnchors + 1;;
				bezier->m_pPoints = new CPoint[m_nAllocs + 3];
				bezier->m_nAllocs = m_nAllocs + 3;
				bezier->m_pPoints[0] = L[2];
				bezier->m_pPoints[1] = L[3];
				bezier->m_pPoints[2] = R[1];
				memcpy(bezier->m_pPoints + 1*3, m_pPoints + (i + 1 - 1)*3, sizeof(CPoint)*(m_nAnchors - i - 1)*3);
				bezier->m_pPoints[3] = R[2];
				memcpy(bezier->m_pPoints + (m_nAnchors - i)*3, m_pPoints, sizeof(CPoint)*i*3);
				bezier->m_pPoints[m_nAnchors*3 - 1] = L[1];
				bezier->m_pPoints[m_nAnchors*3] = L[2];
				bezier->m_pPoints[m_nAnchors*3 + 1] = L[3];
				bezier->m_pPoints[m_nAnchors*3 + 2] = R[1];
			}

			bezier->m_pPoints[0] = bezier->m_pPoints[1];
			bezier->m_pPoints[(bezier->m_nAnchors - 1)*3 + 2] = bezier->m_pPoints[(bezier->m_nAnchors - 1)*3 + 1];
			bezier->RecalRect();
			polys.push_back(bezier);
		}
		else
		{
			CBezier* bezier1 = new CBezier;
			CBezier* bezier2 = new CBezier;
			CGeom::CopyTo(bezier1);
			CGeom::CopyTo(bezier2);
			bezier1->m_bClosed = false;
			bezier2->m_bClosed = false;
			if(x == fpoint.x && y == fpoint.y)
			{
				bezier1->m_nAnchors = i;
				bezier1->m_pPoints = new CPoint[i*3];
				bezier1->m_nAllocs = i*3;
				memcpy(bezier1->m_pPoints, m_pPoints, sizeof(CPoint)*i*3);

				bezier2->m_nAnchors = m_nAnchors - i + 1;
				bezier2->m_nAllocs = (m_nAnchors - i + 1)*3;
				bezier2->m_pPoints = new CPoint[(m_nAnchors - i + 1)*3];
				memcpy(bezier2->m_pPoints, m_pPoints + (i - 1)*3, sizeof(CPoint)*(m_nAnchors - i + 1)*3);
			}
			else if(x == tpoint.x && y == tpoint.y)
			{
				bezier1->m_nAnchors = i + 1;
				bezier1->m_pPoints = new CPoint[(i + 1)*3];
				bezier1->m_nAllocs = (i + 1)*3;
				memcpy(bezier1->m_pPoints, m_pPoints, sizeof(CPoint)*(i + 1)*3);

				bezier2->m_nAnchors = m_nAnchors - i;
				bezier2->m_nAllocs = (m_nAnchors - i)*3;
				bezier2->m_pPoints = new CPoint[(m_nAnchors - i)*3];
				memcpy(bezier2->m_pPoints, m_pPoints + i*3, sizeof(CPoint)*(m_nAnchors - i)*3);
			}
			else
			{
				bezier1->m_nAnchors = i + 1;
				bezier1->m_pPoints = new CPoint[(i + 1)*3];
				bezier1->m_nAllocs = (i + 1)*3;
				memcpy(bezier1->m_pPoints, m_pPoints, sizeof(CPoint)*(i + 1)*3);
				bezier1->m_pPoints[(i + 1 - 1)*3 - 1] = L[1];
				bezier1->m_pPoints[(i + 1 - 1)*3] = L[2];
				bezier1->m_pPoints[(i + 1 - 1)*3 + 1] = L[3];
				bezier1->m_pPoints[(i + 1 - 1)*3 + 2] = R[1];

				bezier2->m_nAnchors = m_nAnchors - i + 1;
				bezier2->m_nAllocs = (m_nAnchors - i + 1)*3;
				bezier2->m_pPoints = new CPoint[(m_nAnchors - i + 1)*3];
				bezier2->m_pPoints[0] = L[2];
				bezier2->m_pPoints[1] = L[3];
				bezier2->m_pPoints[2] = R[1];
				memcpy(bezier2->m_pPoints + 3, m_pPoints + i*3, sizeof(CPoint)*(m_nAnchors - i)*3);
				bezier2->m_pPoints[3] = R[2];
			}

			bezier2->RecalRect();
			bezier2->RecalRect();
			polys.push_back(bezier1);
			polys.push_back(bezier2);
		}
	}

	return polys;
}

// point is in logical coordinates
bool CBezier::AddAnchor(const CPoint& point, bool pTail, bool ignore)
{
	if(m_nAnchors*3 >= m_nAllocs)
	{
		CPoint* newPoints = new CPoint[m_nAllocs + 30];
		if(m_pPoints != nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPoint)*m_nAllocs);
			delete[] m_pPoints;
		}
		m_pPoints = newPoints;
		m_nAllocs += 30;
	}

	if(pTail)
	{
		if(m_nAnchors > 0 && m_pPoints[(m_nAnchors - 1)*3 + 1] == point)
			return false;

		m_pPoints[m_nAnchors*3] = point;
		m_pPoints[m_nAnchors*3 + 1] = point;
		m_pPoints[m_nAnchors*3 + 2] = point;
	}
	else
	{
		if(m_nAnchors > 0 && m_pPoints[1] == point)
			return false;

		for(unsigned int i = m_nAnchors; i >= 1; i--)
		{
			m_pPoints[i*3] = m_pPoints[(i - 1)*3];
			m_pPoints[i*3 + 1] = m_pPoints[(i - 1)*3 + 1];
			m_pPoints[i*3 + 2] = m_pPoints[(i - 1)*3 + 2];
		}
		m_pPoints[0] = point;
		m_pPoints[1] = point;
		m_pPoints[2] = point;
	}
	if(m_nAnchors == 0)
	{
		m_Rect = CRect(point, point);
	}
	else
	{
		const CRect rect = LBezier::Box(point, point, m_pPoints[3], m_pPoints[4]);
		m_Rect = UnionRect(m_Rect, rect);
	}
	m_nAnchors++;
	if(ignore == false)
	{
		RecalCentroid();
	}
	return true;
}

// point is in logical coordinates
bool CBezier::AddAnchor(const CPoint points[], bool pTail, bool ignore)
{
	if(CBezier::AddAnchor(points[1], pTail, ignore) == false)
		return false;

	if(pTail == true)
	{
		m_pPoints[(m_nAnchors - 1)*3] = points[0];
		m_pPoints[(m_nAnchors - 1)*3 + 2] = points[2];

		if(m_nAnchors > 1)
		{
			const CRect rect = LBezier::Box(m_pPoints[(m_nAnchors - 1 - 1)*3 + 1], m_pPoints[(m_nAnchors - 1 - 1)*3 + 2], points[0], points[1]);
			m_Rect.left = min(m_Rect.left, rect.left);
			m_Rect.top = min(m_Rect.top, rect.top);
			m_Rect.right = max(m_Rect.right, rect.right);
			m_Rect.bottom = max(m_Rect.bottom, rect.bottom);
		}
	}
	else
	{
		m_pPoints[0] = points[2];
		m_pPoints[2] = points[0];

		const CRect rect = LBezier::Box(points[1], points[2], m_pPoints[3], m_pPoints[4]);
		m_Rect.left = min(m_Rect.left, rect.left);
		m_Rect.top = min(m_Rect.top, rect.top);
		m_Rect.right = max(m_Rect.right, rect.right);
		m_Rect.bottom = max(m_Rect.bottom, rect.bottom);
	}
	if(ignore == false)
	{
		RecalCentroid();
	}
	return true;
}

void CBezier::InsertAnchor(const unsigned nAnchor, const CPoint& point)
{
	if(m_nAnchors*3 >= m_nAllocs)
	{
		CPoint* newPoints = new CPoint[m_nAllocs + 30];
		if(m_pPoints != nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPoint)*m_nAllocs);
			delete[] m_pPoints;
		}
		m_pPoints = newPoints;
		m_nAllocs += 30;
	}

	for(unsigned int i = m_nAnchors; i >= nAnchor; i--)
	{
		m_pPoints[i*3] = m_pPoints[(i - 1)*3];
		m_pPoints[i*3 + 1] = m_pPoints[(i - 1)*3 + 1];
		m_pPoints[i*3 + 2] = m_pPoints[(i - 1)*3 + 2];
	}

	m_pPoints[(nAnchor - 1)*3] = point;
	m_pPoints[(nAnchor - 1)*3 + 1] = point;
	m_pPoints[(nAnchor - 1)*3 + 2] = point;

	m_nAnchors++;
	RecalRect();
}

void CBezier::InsertAnchor(const unsigned nAnchor, const CPoint& fctrol, const CPoint points[], const CPoint& tctrol)
{
	CBezier::InsertAnchor(nAnchor, points[1]);

	m_pPoints[(nAnchor - 1 - 1)*3 + 2] = fctrol;

	m_pPoints[(nAnchor - 1)*3] = points[0];
	m_pPoints[(nAnchor - 1)*3 + 2] = points[2];

	m_pPoints[(nAnchor + 1 - 1)*3] = tctrol;

	RecalRect();
}

bool CBezier::Add(const CPoly* poly)
{
	if(m_bClosed == true || poly->m_bClosed == true)
		return false;
	const unsigned int nAnchors = m_nAnchors + poly->GetAnchors();
	const unsigned int nAlloPointFs = nAnchors*3;

	if(nAnchors*3 > m_nAllocs)
	{
		CPoint* newPoints = new CPoint[nAlloPointFs];

		memcpy(newPoints, m_pPoints, sizeof(CPoint)*m_nAnchors*3);
		delete[] m_pPoints;

		m_pPoints = newPoints;
		m_nAllocs = nAlloPointFs;
		newPoints = nullptr;
	}

	if(poly->IsKindOf(RUNTIME_CLASS(CBezier)) == TRUE)
	{
		if(m_nAnchors > 0 && GetAnchor(m_nAnchors) == poly->GetAnchor(1))
		{
			memcpy(m_pPoints + m_nAnchors*3 - 1, poly->m_pPoints + 2, sizeof(CPoint)*(poly->m_nAnchors*3 - 2));
			m_nAnchors = nAnchors - 1;
		}
		else
		{
			memcpy(m_pPoints + m_nAnchors*3, poly->m_pPoints, sizeof(CPoint)*(poly->m_nAnchors*3));
			m_nAnchors = nAnchors;
		}
	}
	else
	{
		unsigned int from = 1;
		if(m_nAnchors > 0 && GetAnchor(m_nAnchors) == poly->GetAnchor(1))
		{
			from = 2;
		}

		for(unsigned int i = from; i <= poly->m_nAnchors; i++)
		{
			m_nAnchors++;
			m_pPoints[(m_nAnchors - 1)*3 + 0] = poly->m_pPoints[i - 1];
			m_pPoints[(m_nAnchors - 1)*3 + 1] = poly->m_pPoints[i - 1];
			m_pPoints[(m_nAnchors - 1)*3 + 2] = poly->m_pPoints[i - 1];
		}
	}

	RecalRect();

	m_bModified = true;
	return true;
}

void CBezier::RemoveAnchor(const unsigned int& nAnchor)
{
	if(nAnchor < 1 || nAnchor > m_nAnchors)
		return;

	for(unsigned int i = nAnchor; i < m_nAnchors; i++)
	{
		m_pPoints[(i - 1)*3] = m_pPoints[i*3];
		m_pPoints[(i - 1)*3 + 1] = m_pPoints[i*3 + 1];
		m_pPoints[(i - 1)*3 + 2] = m_pPoints[i*3 + 2];
	}

	m_nAnchors--;

	if(m_nAnchors*3 + 3 == m_nAllocs - 30)
	{
		CPoint* newPoints = new CPoint[m_nAllocs - 30];
		if(m_pPoints != nullptr)
		{
			memcpy(newPoints, m_pPoints, sizeof(CPoint)*(m_nAllocs - 30));
			delete[] m_pPoints;
		}
		m_pPoints = newPoints;
		m_nAllocs -= 30;
	}

	if(nAnchor == 1 && m_bClosed == true)
	{
		m_pPoints[(m_nAnchors - 1)*3] = m_pPoints[0];
		m_pPoints[(m_nAnchors - 1)*3 + 1] = m_pPoints[1];
		m_pPoints[(m_nAnchors - 1)*3 + 2] = m_pPoints[2];
	}

	RecalRect();

	m_bModified = true;
}

void CBezier::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	CPoint* points = new CPoint[m_nAnchors*3];
	memcpy(points, m_pPoints, sizeof(CPoint)*m_nAnchors*3);
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		const long x = ::lround(points[i].x*m11 + points[i].y*m21 + m31);
		const long y = ::lround(points[i].x*m12 + points[i].y*m22 + m32);
		points[i].x = x;
		points[i].y = y;
	}

	CPoint* replace = m_pPoints;
	m_pPoints = points;

	DrawPath(pDC, viewinfo);

	m_pPoints = replace;

	delete[] points;
}

void CBezier::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		const long x = ::lround(m_pPoints[i].x*m11 + m_pPoints[i].y*m21 + m31);
		const long y = ::lround(m_pPoints[i].x*m12 + m_pPoints[i].y*m22 + m32);

		m_pPoints[i].x = x;
		m_pPoints[i].y = y;
	}
	{
		const long x = ::lround(m_centroid.x*m11 + m_centroid.y*m21 + m31);
		const long y = ::lround(m_centroid.x*m12 + m_centroid.y*m22 + m32);
		m_centroid.x = x;
		m_centroid.y = y;
	}
	if(m_geogroid != nullptr)
	{
		const long x = ::lround(m_geogroid->x*m11 + m_geogroid->y*m21 + m31);
		const long y = ::lround(m_geogroid->x*m12 + m_geogroid->y*m22 + m32);
		m_geogroid->x = x;
		m_geogroid->y = y;
	}
	if(m_labeloid != nullptr)
	{
		const long x = ::lround(m_labeloid->x*m11 + m_labeloid->y*m21 + m31);
		const long y = ::lround(m_labeloid->x*m12 + m_labeloid->y*m22 + m32);
		m_labeloid->x = x;
		m_labeloid->y = y;
	}
	CGeom::Transform(m11, m21, m31, m12, m22, m32);
}

void CBezier::ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;

	Gdiplus::PointF* points = new Gdiplus::PointF[m_nAnchors*3];
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		points[i].X = m_pPoints[i].x;
		points[i].Y = m_pPoints[i].y;
	}
	matrix.TransformPoints(points, m_nAnchors*3);

	_ftprintf(file, _T("%g %g m\n"), points[1].X, points[1].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i - 1)*3 + 2].X, points[(i - 1)*3 + 2].Y, points[i*3].X, points[i*3].Y, points[i*3 + 1].X, points[i*3 + 1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CBezier::ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;

	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, m_nAnchors*3);

	_ftprintf(file, _T("%g %g m\n"), points[1].X, points[1].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		fprintf(file, "%g %g %g %g %g %g C\n", points[(i - 1)*3 + 2].X, points[(i - 1)*3 + 2].Y, points[i*3].X, points[i*3].Y, points[i*3 + 1].X, points[i*3 + 1].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CBezier::ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;
	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, m_nAnchors*3);
	HPDF_Page_MoveTo(page, points[1].X, points[1].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		HPDF_Page_CurveTo(page, points[(i - 1)*3 + 2].X, points[(i - 1)*3 + 2].Y, points[i*3].X, points[i*3].Y, points[i*3 + 1].X, points[i*3 + 1].Y);
	}

	delete[] points;
}

void CBezier::ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;

	Gdiplus::PointF* points = new Gdiplus::PointF[m_nAnchors*3];
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		points[i].X = m_pPoints[i].x;
		points[i].Y = m_pPoints[i].y;
	}
	matrix.TransformPoints(points, m_nAnchors*3);

	HPDF_Page_MoveTo(page, points[1].X, points[1].Y);
	for(unsigned int i = 1; i < m_nAnchors; i++)
	{
		HPDF_Page_CurveTo(page, points[(i - 1)*3 + 2].X, points[(i - 1)*3 + 2].Y, points[i*3].X, points[i*3].Y, points[i*3 + 1].X, points[i*3 + 1].Y);
	}

	delete[] points;
}

CGeom* CBezier::Buffer(const unsigned int& distance, const unsigned int& tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype) const
{
	return CPoly::Buffer(distance, tolerance, jointype, endtype);
}
CGeom* CBezier::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;
	
	CPoly* pPoly = this->Polylize(tolerance);
	if(pPoly == nullptr)
		return nullptr;

	CGeom* pCliped = pPoly->Clip(bound, clips, in, tolerance, bStroke);
	if(pCliped == nullptr)
	{
		delete pPoly;
		return nullptr;
	}
	else if(pCliped == pPoly)
	{
		delete pPoly;
		return this;
	}
	else
	{
		delete pPoly;
		return pCliped;
	}
}
void CBezier::Transform(const CViewinfo& viewinfo)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		const Gdiplus::Point point = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[i]);
		m_pPoints[i].x = point.X;
		m_pPoints[i].y = point.Y;
	}

	CGeom::Transform(viewinfo);
}

void CBezier::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		m_pPoints[i] = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_pPoints[i]);
	}

	m_centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, m_centroid);
	if(m_geogroid != nullptr)
	{
		const CPoint centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, *m_geogroid);
		m_geogroid->x = centroid.x;
		m_geogroid->y = centroid.y;
	}
	if(m_labeloid != nullptr)
	{
		const CPoint centroid = CGeom::Rotate(mapinfo, deltaLng, deltaLat, facing, *m_labeloid);
		m_labeloid->x = centroid.x;
		m_labeloid->y = centroid.y;
	}
	RecalRect();
	m_bModified = true;
}

CGeom* CBezier::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		m_pPoints[i] = CDatainfo::Project(fMapinfo, tMapinfo, m_pPoints[i]);
	}

	m_centroid = CDatainfo::Project(fMapinfo, tMapinfo, m_centroid);
	if(m_geogroid != nullptr)
	{
		const CPoint centroid = CDatainfo::Project(fMapinfo, tMapinfo, *m_geogroid);
		m_geogroid->x = centroid.x;
		m_geogroid->y = centroid.y;
	}
	if(m_labeloid != nullptr)
	{
		const CPoint centroid = CDatainfo::Project(fMapinfo, tMapinfo, *m_labeloid);
		m_labeloid->x = centroid.x;
		m_labeloid->y = centroid.y;
	}
	RecalRect();
	m_bModified = true;
	return this;
}

void CBezier::Rectify(CTin& tin)
{
	CPoint point;
	for(unsigned int i = 0; i < m_nAnchors*3; i++)
	{
		m_pPoints[i] = tin.Rectify(m_pPoints[i]);
	}

	RecalRect();
	m_bModified = true;
}

void CBezier::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(this->IsValid() == false)
		return;

	CPoly* poly = this->Polylize(tolerance);
	if(poly != nullptr)
	{
		poly->ExportMapInfoMif(datainfo, fileMif, bTransGeo, tolerance);
		delete poly;
	}
}

CPoly* CBezier::Polylize(const unsigned int& tolerance) const
{
	CPoly* poly = new CPoly();
	CGeom::CopyTo(poly);

	poly->m_nAllocs = 0;
	poly->m_nAnchors = 0;
	poly->AddAnchor(m_pPoints[1], true, true);
	for(int index = 0; index < (m_nAnchors - 1)*3; index += 3)//È¡Ò»¶Î
	{
		const CPoint& fpoint = m_pPoints[index + 1];
		const CPoint& fctrol = m_pPoints[index + 2];
		const CPoint& tctrol = m_pPoints[index + 3];
		const CPoint& tpoint = m_pPoints[index + 4];

		if(fpoint != fctrol || tctrol != tpoint)
		{
			const double A_x = -fpoint.x + 3*fctrol.x - 3*tctrol.x + tpoint.x;
			const double B_x = 3*fpoint.x - 6*fctrol.x + 3*tctrol.x;
			const double C_x = -3*fpoint.x + 3*fctrol.x;
			const double D_x = fpoint.x;
			const double A_y = -fpoint.y + 3*fctrol.y - 3*tctrol.y + tpoint.y;
			const double B_y = 3*fpoint.y - 6*fctrol.y + 3*tctrol.y;
			const double C_y = -3*fpoint.y + 3*fctrol.y;
			const double D_y = fpoint.y;

			//µÃµ½Ê×Ä©µãÐ±ÂÊ
			const double st = 0; //ftÎªÆðµãtÖµ
			const double et = 1; //ttÎªÄ©µãtÖµ
			double* RecordT = (double*)malloc(sizeof(double)*50);; //RecordT[0]ÓÃÀ´¼ÇÂ¼ÇóµÃµãµÄ¸öÊý,RecordT[1]ÓÃÀ´¼ÇÂ¼ÉêÇëÄÚ´æÊý
			RecordT[0] = 0;
			RecordT[1] = 50;
			GetMoreBezierT(st, et, A_x, B_x, C_x, D_x, A_y, B_y, C_y, D_y, fpoint, tpoint, RecordT, tolerance);

			if(RecordT[0] > 0)//¶ÔtÖµ½øÐÐÅÅÐò
			{
				double tempt = 0;
				for(int i = 0; i < RecordT[0]; i++)
				{
					for(int j = i + 1; j < RecordT[0]; j++)
					{
						if(RecordT[i + 2] > RecordT[j + 2])
						{
							tempt = RecordT[i + 2];
							RecordT[i + 2] = RecordT[j + 2];
							RecordT[j + 2] = tempt;
						}
					}
				}
			}

			for(int i = 0; i < RecordT[0]; i++)
			{
				const double t = RecordT[i + 2];
				const long x = ::lround((float)(A_x*t*t*t + B_x*t*t + C_x*t + D_x));
				const long y = ::lround((float)(A_y*t*t*t + B_y*t*t + C_y*t + D_y));
				poly->AddAnchor(CPoint(x, y), true, true);
			}
			free(RecordT);
		}
		poly->AddAnchor(tpoint, true, true);
	}

	poly->RecalRect();
	return poly;
}

DWORD CBezier::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CGeom::PackPC(pFile, bytes);

	if(pFile != nullptr)
	{
		const double area = GetArea();
		if(area < 0)
		{
			Reverse();
		}
		const unsigned short nCode = (((m_geoId << 4) + m_geoId) ^ 0X8768);
		const unsigned int nAnchors = (m_nAnchors ^ nCode);
		pFile->Write(&nAnchors, sizeof(unsigned int));
		size += sizeof(unsigned int);

		for(unsigned int i = 0; i < m_nAnchors*3; i++)
		{
			CPoint point = m_pPoints[i];
			const DWORD nCode1 = (((m_geoId << 10) + i*m_geoId) ^ 0X54668768);
			const DWORD nCode2 = (((m_geoId << 16) + i*m_geoId) ^ 0X65477878);
			point.x = point.x ^ nCode1;
			point.y = point.y ^ nCode2;

			pFile->Write(&point, sizeof(CPoint));
		}

		size += 3*m_nAnchors*sizeof(CPoint);
		return size;
	}
	else
	{
		const unsigned short nCode = (((m_geoId << 4) + m_geoId) ^ 0X8768);
		const unsigned int nAnchors = (unsigned int)*(unsigned int*)bytes;
		m_nAnchors = (nAnchors ^ nCode);

		bytes += sizeof(unsigned int);

		m_pPoints = new CPoint[m_nAnchors*3];
		memcpy(m_pPoints, bytes, m_nAnchors*sizeof(CPoint)*3);

		bytes += 3*m_nAnchors*sizeof(CPoint);

		for(unsigned int i = 0; i < m_nAnchors*3; i++)
		{
			const DWORD nCode1 = (((m_geoId << 10) + i*m_geoId) ^ 0X54668768);
			const DWORD nCode2 = (((m_geoId << 16) + i*m_geoId) ^ 0X65477878);
			m_pPoints[i].x = m_pPoints[i].x ^ nCode1;
			m_pPoints[i].y = m_pPoints[i].y ^ nCode2;
		}
		return 0;
	}
}

DWORD CBezier::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CGeom::ReleaseCE(file, type);
	const double area = GetArea();

	file.Write(&m_nAnchors, sizeof(unsigned int));
	size += sizeof(unsigned int);

	for(unsigned int nAnchor = 0; nAnchor < m_nAnchors*3; nAnchor++)
	{
		CPoint point = m_pPoints[nAnchor - 1];
		point.x /= 10000;
		point.y /= 10000;

		file.Write(&point, sizeof(CPoint));
	}
	size += m_nAnchors*3*sizeof(CPoint);

	return size;
}

void CBezier::ReleaseWeb(CFile& file, CSize offset) const
{
	CGeom::ReleaseWeb(file, offset);;

	unsigned int nAnchors = m_nAnchors;
	ReverseOrder(nAnchors);
	file.Write(&nAnchors, sizeof(unsigned int));
	for(unsigned int index = 0; index < m_nAnchors*3; index++)
	{
		short x = (short)(m_pPoints[index].x - offset.cx);
		short y = (short)(m_pPoints[index].y - offset.cy);
		ReverseOrder(x);
		file.Write(&x, sizeof(short));
		ReverseOrder(y);
		file.Write(&y, sizeof(short));
	}

	if(m_bClosed == true)
	{
		{
			short x = (short)(m_centroid.x - offset.cx);
			short y = (short)(m_centroid.y - offset.cy);
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
		if(m_geogroid != nullptr)
		{
			short x = (short)(m_geogroid->x - offset.cx);
			short y = (short)(m_geogroid->y - offset.cy);
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
		if(m_labeloid != nullptr)
		{
			short x = (short)(m_labeloid->x - offset.cx);
			short y = (short)(m_labeloid->y - offset.cy);
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
	}
}
void CBezier::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CGeom::ReleaseWeb(stream, offset);;

	stream << m_nAnchors;
	for(int index = 0; index < m_nAnchors*3; index++)
	{
		const short x = (short)(m_pPoints[index].x - offset.cx);
		const short y = (short)(m_pPoints[index].y - offset.cy);
		stream << x;
		stream << y;
	}

	if(m_bClosed == true)
	{
		{
			const short x = (short)(m_centroid.x - offset.cx);
			const short y = (short)(m_centroid.y - offset.cy);
			stream << x;
			stream << y;
		}
		if(m_geogroid != nullptr)
		{
			const short x = (short)(m_geogroid->x - offset.cx);
			const short y = (short)(m_geogroid->y - offset.cy);
			stream << x;
			stream << y;
		}
		if(m_labeloid != nullptr)
		{
			const short x = (short)(m_labeloid->x - offset.cx);
			const short y = (short)(m_labeloid->y - offset.cy);
			stream << x;
			stream << y;
		}
	}
}
void CBezier::ReleaseWeb(boost::json::object& json) const
{
	CGeom::ReleaseWeb(json);

	json["Anchors"] = m_nAnchors;

	std::vector<int> anchors;
	CPoint prev = m_pPoints[1];
	anchors.push_back(m_pPoints[0].x - prev.x);
	anchors.push_back(m_pPoints[0].y - prev.y);
	anchors.push_back(m_pPoints[1].x);
	anchors.push_back(m_pPoints[1].y);
	anchors.push_back(m_pPoints[2].x - prev.x);
	anchors.push_back(m_pPoints[2].y - prev.y);
	for(unsigned int index = 1; index < m_nAnchors; index++)
	{
		anchors.push_back(m_pPoints[index*3 + 0].x - m_pPoints[index*3 + 1].x);
		anchors.push_back(m_pPoints[index*3 + 0].y - m_pPoints[index*3 + 1].y);
		anchors.push_back(m_pPoints[index*3 + 1].x - prev.x);
		anchors.push_back(m_pPoints[index*3 + 1].y - prev.y);
		anchors.push_back(m_pPoints[index*3 + 2].x - m_pPoints[index*3 + 1].x);
		anchors.push_back(m_pPoints[index*3 + 2].y - m_pPoints[index*3 + 1].y);
		prev = m_pPoints[index*3 + 1];
	}
	json["Points"] = boost::json::value_from(anchors);

	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		if(geogroid.x != 0 && geogroid.y != 0)
		{
			const std::vector<long> vector{geogroid.x, geogroid.y};
			json["Geogroid"] = boost::json::value_from(vector);
		}
		const CPoint labeloid = this->GetLabeloid();
		if(labeloid.x != 0 && labeloid.y != 0)
		{
			const std::vector<long> vector{labeloid.x, labeloid.y};
			json["Labeloid"] = boost::json::value_from(vector);
		}
	}
}
void CBezier::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPath::ReleaseWeb(writer, offset);

	if(m_bClosed == false && m_strName.IsEmpty() == FALSE)
	{
		CObArray labelarray;
		CText::AnalyzeString(m_strName, labelarray);
		const BYTE bWords = min(255, labelarray.GetCount());
		writer.add_variant_uint16(bWords);
		for(int index = 0; index < bWords; index++)
		{
			const CText::CWord* word = (CText::CWord*)labelarray.GetAt(index);
			writer.add_string(EscapedASCII(word->str));
			delete word;
		}
		labelarray.RemoveAll();
	}

	writer.add_variant_uint32(m_nAnchors);
	for(unsigned int index = 0; index<m_nAnchors; index++)
	{
		writer.add_variant_sint32(m_pPoints[index*3+0].x-m_pPoints[index*3+1].x);
		writer.add_variant_sint32(m_pPoints[index*3+0].y-m_pPoints[index*3+1].y);
		writer.add_variant_sint32(m_pPoints[index*3+1].x-offset.cx);
		writer.add_variant_sint32(m_pPoints[index*3+1].y-offset.cy);
		writer.add_variant_sint32(m_pPoints[index*3+2].x-m_pPoints[index*3+1].x);
		writer.add_variant_sint32(m_pPoints[index*3+2].y-m_pPoints[index*3+1].y);
	}

	if(m_bClosed == true)
	{
		const CPoint geogroid = this->GetGeogroid();
		writer.add_variant_sint32(geogroid.x - offset.cx);
		writer.add_variant_sint32(geogroid.y - offset.cy);

		const CPoint labeloid = this->GetLabeloid();
		writer.add_variant_sint32(labeloid.x - offset.cx);
		writer.add_variant_sint32(labeloid.y - offset.cy);
	}
}
double CBezier::Shortcut(const CPoint& point, CPoint& trend) const
{
	if(m_bClosed == true && PickIn(point) == true)
		return 0;
	else if(m_pPoints != nullptr)
	{
		long min = 0X00FFFFFF;
		for(unsigned int nAnchor = 1; nAnchor < m_nAnchors; nAnchor++)
		{
			const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
			const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
			const CPoint& tctrol = m_pPoints[(nAnchor + 1 - 1)*3];
			const CPoint& tpoint = m_pPoints[(nAnchor + 1 - 1)*3 + 1];

			double t;
			const long distance = ::Distance1(point, fpoint, fctrol, tctrol, tpoint, t);
			if(distance < min && t >= 0 && t <= 1)
			{
				min = distance;
				const CPoint trend1 = BezierPoint(fpoint, fctrol, tctrol, tpoint, t);
				trend.x = trend1.x;
				trend.y = trend1.y;
			}
		}
		return min;
	}
	else
		return 0X00FFFFFF;
}

bool CBezier::PointInBeziergon(const CPoint& point, CPoint* points, const unsigned int& ptCount)
{
	long odd = 0;
	for(unsigned int I = 0; I < ptCount - 1; I++)
	{
		CPoint fpoint = points[I*3 + 1];
		CPoint fctrol = points[I*3 + 2];
		CPoint tctrol = points[(I + 1)*3];
		CPoint tpoint = points[(I + 1)*3 + 1];
		const CRect rect = FPointRect(fpoint, fctrol, tctrol, tpoint);
		if(point.y > rect.bottom)
			continue;
		if(point.y < rect.top)
			continue;
		if(point.x > rect.right)
			continue;

		if(point.y == fpoint.y)
		{
			const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, 0.0f);
			if(derivate.cy > 0.0f)
				odd++;
		}
		if(point.y == tpoint.y)
		{
			const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, 1.f);
			if(derivate.cy < 0.0f)
				odd++;
		}
		const double ay = -1.0f*fpoint.y + 3.0f*fctrol.y - 3.0f*tctrol.y + tpoint.y;
		const double by = 3.0f*fpoint.y - 6.0f*fctrol.y + 3.0f*tctrol.y;
		const double cy = -3.0f*fpoint.y + 3.0f*fctrol.y;
		const double dy = fpoint.y - point.y;

		int roots = 0;
		const double* pRoot = Q3Roots(ay, by, cy, dy, roots);
		for(unsigned int index = 0; index < roots; index++)
		{
			if(pRoot[index] == -1.f)
				continue;
			if(pRoot[index] < 0.0f)
				continue;
			if(pRoot[index] > 1.f)
				continue;
			if(std::isnan(pRoot[index]))
				continue;
			const CPoint inter = BezierPoint(fpoint, fctrol, tctrol, tpoint, pRoot[index]);
			const CSizeF derivate = BezierDerivate(fpoint, fctrol, tctrol, tpoint, pRoot[index]);
			if(inter.x == point.x)
			{
				delete[] pRoot;
				return true;
			}
			if(inter.x < point.x)
				continue;
			if(derivate.cy == 0.0f)
				continue;
			if(inter.y == fpoint.y)
				continue;
			if(inter.y == tpoint.y)
				continue;

			odd++;
			//inter.y = point.y;//this line is very important
			//if(inter == fpoint)	
			//{
			//	if(derivate.cy < 0.0f)
			//		continue;
			//}	
			//if(inter == tpoint)	
			//{
			//	if(derivate.cy > 0.0f)
			//		continue;
			//}
		}
		delete[] pRoot;
	}

	return odd % 2 == 1;
}

std::list<std::tuple<int, int, CGeom*>> CBezier::Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minRow, int maxRow, int minCol, int maxCol) const
{
	if(m_pPoints == nullptr)
		return std::list<std::tuple<int, int, CGeom*>>{};
	if(m_nAnchors <= 1)
		return std::list<std::tuple<int, int, CGeom*>>{};
	if(m_pPoints == nullptr)
		return std::list<std::tuple<int, int, CGeom*>>{};
	if(m_nAnchors <= 1)
		return std::list<std::tuple<int, int, CGeom*>>{};

	int minX = m_Rect.left/cxTile;
	int maxX = m_Rect.right/cxTile;
	int minY = m_Rect.top/cyTile;
	int maxY = m_Rect.bottom/cyTile;
	maxX = m_Rect.right % cxTile == 0 ? maxX - 1 : maxX;
	maxY = m_Rect.bottom % cyTile == 0 ? maxY - 1 : maxY;
	if(maxX <= minX && maxY <= minY)
		return std::list<std::tuple<int, int, CGeom*>>{make_tuple(minX, minY, this->Clone())};
	else
	{
		CPoly* pPoly = this->Polylize(200);
		if(pPoly == nullptr)
			return std::list<std::tuple<int, int, CGeom*>>{};

		CGeom::CopyTo(pPoly);
		bStroke = (m_pLine != nullptr && m_pLine->Gettype() != CLine::LINETYPE::Blank) ? true : bStroke;
		std::list<std::tuple<int, int, CGeom*>> cellgeoms = pPoly->Swim(bStroke, bFill, cxTile, cyTile, minRow, maxRow, minCol, maxCol);

		delete pPoly;
		pPoly = nullptr;
		return cellgeoms;
	}
}

unsigned long long CBezier::Distance2(const CPoint& point) const
{
	unsigned long long min = MAXUINT64;
	for(unsigned int nAnchor = 1; nAnchor <= (m_bClosed ? m_nAnchors : m_nAnchors - 1); nAnchor++)
	{
		const CPoint& fpoint = m_pPoints[(nAnchor - 1)*3 + 1];
		const CPoint& fctrol = m_pPoints[(nAnchor - 1)*3 + 2];
		const CPoint& tctrol = m_pPoints[nAnchor == m_nAnchors ? (0 + 1 - 1)*3 : (nAnchor + 1 - 1)*3];
		const CPoint& tpoint = m_pPoints[nAnchor == m_nAnchors ? (0 + 1 - 1)*3 + 1 : (nAnchor + 1 - 1)*3 + 1];
		min = min(min, ::Distance2(point, fpoint, fctrol, tctrol, tpoint));
	}

	return min;
}

void CBezier::GetPaths64(Clipper2Lib::Paths64& paths64, int tolerance) const
{
	CPoly* pPoly = this->Polylize(tolerance);
	pPoly->GetPaths64(paths64, tolerance);	
	delete pPoly;
}