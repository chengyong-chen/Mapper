#include "stdafx.h"

#include "Geom.h"
#include "Path.h"
#include "Poly.h"
#include "PRect.h"
#include "Group.h"
#include "Global.h"
#include "Compound.h"

#include "../Database/ODBCRecordset.h"
#include "../Public/Global.h"
#include "../Public/BinaryStream.h"

#include "../Style/Line.h"
#include "../Style/LineEmpty.h"
#include "../Style/LineSymbol.h"
#include "../Style/lineRiver.h"
#include "../Style/FillCompact.h"

#include "../Action/Modify.h"
#include "../Action/ActionStack.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/PRect.h"
#include <boost/json.hpp>
using namespace boost;
#include <math.h>
#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CPRect, CPath, 0)

CPRect::CPRect()
	: CPath()
{
	m_bClosed = true;
	m_bRegular = true;
	m_bGroup = true;

	m_pPoints = nullptr;

	m_bType = 7;
}

CPRect::CPRect(const CRect& rect, CLine* pLine = nullptr, CFillCompact* pFill = nullptr)
	: CPath(rect, pLine, pFill)
{
	ASSERT_VALID(this);
	m_originX = rect.left;
	m_originY = rect.top;
	m_Width = rect.Width();
	m_Height = rect.Height();

	m_bClosed = true;
	m_bRegular = true;
	m_bGroup = true;

	m_pPoints = nullptr;

	m_bType = 7;
}

CPRect::~CPRect()
{
	delete[] m_pPoints;
}
void CPRect::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CPath::Sha1(sha1);
	
	sha1.process_bytes(&m_originX, sizeof(int));
	sha1.process_bytes(&m_originY, sizeof(int));
	sha1.process_bytes(&m_Width, sizeof(int));
	sha1.process_bytes(&m_Height, sizeof(int));
	if(m_pIMatrix2!=nullptr)
	{
		sha1.process_bytes(&m_pIMatrix2->m11, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m12, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m21, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m22, sizeof(double));
	}
}
void CPRect::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	LONG  x = m_originX-offset.cx;
	LONG  y = m_originX-offset.cy;	
	sha1.process_bytes(&x, sizeof(int));
	sha1.process_bytes(&y, sizeof(int));
	sha1.process_bytes(&m_Width, sizeof(int));
	sha1.process_bytes(&m_Height, sizeof(int));
	if(m_pIMatrix2!=nullptr)
	{
		sha1.process_bytes(&m_pIMatrix2->m11, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m12, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m21, sizeof(double));
		sha1.process_bytes(&m_pIMatrix2->m22, sizeof(double));
	}
}
BOOL CPRect::operator==(const CGeom& geom) const
{
	if(CPath::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CPRect)) == FALSE)
		return FALSE;
	else if(m_bRegular != ((CPRect&)geom).m_bRegular)
		return FALSE;
	else if(m_bRegular == false && memcmp(m_pPoints, ((CPRect&)geom).m_pPoints, sizeof(CPoint)*5) != 0)
		return FALSE;
	else
		return TRUE;
}

CPoint& CPRect::GetAnchor(const unsigned int& nAnchor) const
{
	if(m_bRegular == false && m_pPoints != nullptr)
	{
		return m_pPoints[nAnchor - 1];
	}
	else
	{
		return CPath::GetAnchor(nAnchor);
	}
}

bool CPRect::GetAandT(const unsigned int fAnchor, double st, unsigned long length, unsigned int& tAnchor, double& et) const
{
	if(fAnchor >= 5)
	{
		tAnchor = 5;
		et = 0;
		return false;
	}
	else if(length == 0)
	{
		tAnchor = fAnchor;
		et = st;
		return true;
	}

	if(m_bRegular == false && m_pPoints != nullptr)
	{
		if(length == 0)
		{
			tAnchor = fAnchor;
			et = st;
			return true;
		}

		for(unsigned int i = fAnchor; i < 5; i++)
		{
			const CPoint Δ = GetAnchor(i) - GetAnchor(i + 1);
			const long segment = sqrt((double)(Δ.x*Δ.x + Δ.y*Δ.y));
			const long leftlen = segment - (long)(st*segment);

			if(length < leftlen)
			{
				tAnchor = i;
				et = st + (double)length/segment;

				return true;
			}

			if(length == leftlen)
			{
				tAnchor = i;
				et = 1;

				return true;
			}
			length -= leftlen;
			st = 0.0f;
		}

		tAnchor = 4;
		et = 1.0f;

		return false;
	}
	else
		return CPath::GetAandT(fAnchor, st, length, tAnchor, et);
}

CPoint CPRect::GetPoint(const unsigned int& nAnchor, const double& t) const
{
	if(m_bRegular == false && m_pPoints != nullptr)
	{
		if(t == 0.0f)
			return m_pPoints[nAnchor - 1];
		else if(t == 1.0f)
			return m_pPoints[nAnchor + 1 - 1];
		else
		{
			const CPoint& fpoint = m_pPoints[nAnchor - 1];
			const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];

			CPoint point;
			point.x = fpoint.x + round((tpoint.x - fpoint.x)*t);
			point.y = fpoint.y + round((tpoint.y - fpoint.y)*t);

			return point;
		}
	}
	else
		return CPath::GetPoint(nAnchor, t);
}

double CPRect::GetAngle(const unsigned int& nAnchor, const double& t) const
{
	if(nAnchor >= 5)
		return 0.0f;

	if(m_bRegular == false && m_pPoints != nullptr)
	{
		const CPoint& fpoint = m_pPoints[nAnchor - 1];
		const CPoint& tpoint = m_pPoints[nAnchor + 1 - 1];

		if(t == 0.f)
		{
			const double tdAngle = LineAngle(fpoint, tpoint);
			double fdAngle = 0;
			if(nAnchor == 1)
				fdAngle = LineAngle(m_pPoints[5 - 1 - 1], fpoint);
			else
				fdAngle = LineAngle(m_pPoints[nAnchor - 1 - 1], fpoint);

			return (fdAngle + tdAngle)/2;
		}
		else if(t == 1.f)
		{
			const double fdAngle = LineAngle(fpoint, tpoint);
			double tdAngle = 0.0f;
			if(nAnchor == 4)
				tdAngle = LineAngle(tpoint, m_pPoints[1]);
			else
				tdAngle = LineAngle(tpoint, m_pPoints[nAnchor + 1 - 1]);
			return (fdAngle + tdAngle)/2;
		}
		else
		{
			const double dAngle = LineAngle(fpoint, tpoint);
			return dAngle;
		}
	}
	else
		return CPath::GetAngle(nAnchor, t);
}

CPoint* CPRect::GetPoints() const
{
	CPoint* pPoints = new CPoint[5];

	if(m_pPoints == nullptr)
	{
		pPoints[0] = CPoint(m_Rect.left, m_Rect.top);
		pPoints[1] = CPoint(m_Rect.left, m_Rect.bottom);
		pPoints[2] = CPoint(m_Rect.right, m_Rect.bottom);
		pPoints[3] = CPoint(m_Rect.right, m_Rect.top);
		pPoints[4] = pPoints[0];
	}
	else
	{
		memcpy(pPoints, m_pPoints, 5*sizeof(CPoint));
	}

	return pPoints;
}

CPoly* CPRect::GetPoly() const
{
	CPoly* poly = new CPoly;
	poly->m_bClosed = true;
	poly->m_nAnchors = 5;
	poly->m_nAllocs = 5;
	poly->m_Rect = m_Rect;
	poly->m_pPoints = this->GetPoints();

	return poly;
}
CPoly* CPRect::Polylize(const unsigned int& tolerance) const
{
	return CPRect::GetPoly();
}
void CPRect::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CPRect)) == TRUE)
	{
		CPRect* pPRect = (CPRect*)pGeom;

		pPRect->m_bRegular = m_bRegular;
		if(m_bRegular == false && m_pPoints != nullptr)
		{
			if(pPRect->m_pPoints == nullptr)
			{
				pPRect->m_pPoints = new CPoint[5];
			}

			memcpy(pPRect->m_pPoints, m_pPoints, 5*sizeof(CPoint));
		}
	}
}

void CPRect::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CPRect)) == TRUE)
	{
		CPRect* pPRect = (CPRect*)pGeom;

		Swapdata<bool>(pPRect->m_bRegular, m_bRegular);
		Swapclass<CPoint*>(pPRect->m_pPoints, m_pPoints);
	}

	CPath::Swap(pGeom);
}

void CPRect::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
}

void CPRect::AddPath(Gdiplus::GraphicsPath& path, const Gdiplus::Matrix& matrix) const
{
	if(m_bRegular == true)
	{
		const Gdiplus::Rect rect = CPath::Transform(matrix, m_Rect);
		path.AddRectangle(rect);
	}
	else if(m_pPoints != nullptr)
	{
		const Gdiplus::PointF* points = CPath::Transform(matrix, m_pPoints, 5);
		path.AddPolygon(points, 5);
		::delete[] points;
	}
}

void CPRect::AddPath(Gdiplus::GraphicsPath& path, const CViewinfo& viewinfo, const float& tolerance) const
{
	if(m_bRegular == true)
	{
		const Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(m_Rect);
		path.AddRectangle(rect);
	}
	else if(m_pPoints != nullptr)
	{
		const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints, 5);
		path.AddPolygon(points, 5);
		::delete[] points;
	}
}

void CPRect::DrawPath(CDC* pDC, const CViewinfo& viewinfo) const
{
	CPen* OldPen = (CPen*)pDC->SelectStockObject(BLACK_PEN);
	CBrush* OldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	const int OldROP = pDC->SetROP2(R2_NOTXORPEN);

	if(m_pPoints != nullptr)
	{
		const Gdiplus::PointF point0 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints[0]);
		const Gdiplus::PointF point1 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints[1]);
		const Gdiplus::PointF point2 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints[2]);
		const Gdiplus::PointF point3 = viewinfo.DocToClient<Gdiplus::PointF>(m_pPoints[3]);

		pDC->MoveTo(point0.X, point0.Y);
		pDC->LineTo(point1.X, point1.Y);
		pDC->LineTo(point2.X, point2.Y);
		pDC->LineTo(point3.X, point3.Y);
		pDC->LineTo(point0.X, point0.Y);
	}
	else
	{
		const Gdiplus::PointF point1 = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(m_Rect.left, m_Rect.top));
		const Gdiplus::PointF point2 = viewinfo.DocToClient<Gdiplus::PointF>(CPoint(m_Rect.right, m_Rect.bottom));

		CRect rect = CRect(point1.X, point1.Y, point2.X, point2.Y);
		rect.NormalizeRect();

		pDC->Rectangle(rect);
	}

	pDC->SelectObject(OldPen);
	pDC->SelectObject(OldBrush);
	pDC->SetROP2(OldROP);
}

bool CPRect::UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	CPoly* poly = new CPoly;
	poly->m_Rect = m_Rect;
	poly->m_bClosed = true;
	poly->m_nAnchors = 5;
	poly->m_nAllocs = 5;
	poly->m_pPoints = GetPoints();

	CGeom::CopyTo(poly);

	geomlist.AddHead(poly);
	return true;
}
void CPRect::Move(const int& dx, const int& dy)
{
	if(m_pPoints != nullptr)
	{
		for(unsigned int i = 0; i < 5; i++)
		{
			m_pPoints[i].x += dx;
			m_pPoints[i].y += dy;
		}
	}

	CPath::Move(dx, dy);
}
void CPRect::Move(const CSize& Δ)
{
	if(m_pPoints != nullptr)
	{
		for(unsigned int i = 0; i < 5; i++)
		{
			m_pPoints[i].x += Δ.cx;
			m_pPoints[i].y += Δ.cy;
		}
	}

	CPath::Move(Δ);
}

void CPRect::MoveAnchor(CDC* pDC, const CViewinfo& viewinfo, const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch)
{
	if(m_bRegular == true)
	{
		CPath::MoveAnchor(pDC, viewinfo, nAnchor, Δ, bMatch);
	}
	else if(m_pPoints != nullptr)
	{
		const CRect rect = m_Rect;

		CPoint point[4];
		point[0] = m_pPoints[0];
		point[1] = m_pPoints[1];
		point[2] = m_pPoints[2];
		point[3] = m_pPoints[3];

		ChangeAnchor(nAnchor, Δ, bMatch, nullptr);

		DrawPath(pDC, viewinfo);
		DrawAnchors(pDC, viewinfo);

		m_pPoints[0] = point[0];
		m_pPoints[1] = point[1];
		m_pPoints[2] = point[2];
		m_pPoints[3] = point[3];
		m_pPoints[4] = m_pPoints[0];
		m_Rect = rect;
	}
}

void CPRect::ChangeAnchor(const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch, Undoredo::CActionStack* pActionstack)
{
	if(Δ == CSize(0, 0))
		return;

	if(m_bRegular == true)
	{
		CPath::ChangeAnchor(nAnchor, Δ, bMatch, pActionstack);
	}
	else if(m_pPoints != nullptr)
	{
		const unsigned int fAnchor = (nAnchor + 2) % 4 == 0 ? 4 : (nAnchor + 2) % 4;
		const unsigned int lAnchor = (nAnchor - 1) % 4 == 0 ? 4 : (nAnchor - 1) % 4;
		const unsigned int rAnchor = (nAnchor + 1) % 4 == 0 ? 4 : (nAnchor + 1) % 4;

		CPoint local = GetAnchor(nAnchor) + CPoint(Δ.cx, Δ.cy);
		CPoint face = GetAnchor(fAnchor);
		CPoint left = GetAnchor(lAnchor);
		CPoint right = GetAnchor(rAnchor);

		local = local - face;
		left = left - face;
		right = right - face;
		face = face - face;

		if(bMatch == true)
		{
			if(abs(local.x) > abs(local.y))
				local.y = abs(local.x)*(local.y >= 0 ? 1 : -1);
			else
				local.x = abs(local.y)*(local.x >= 0 ? 1 : -1);
		}

		if(::GetKeyState(VK_CONTROL) < 0)
		{
			const CPoint newDelta = local - (GetAnchor(nAnchor) - GetAnchor(fAnchor));
			face = face - newDelta;
		}

		if(right.x == 0 && left.x != 0)
		{
			const float k1 = (float)left.y/left.x;
			right.x = face.x;
			right.y = round(k1*(face.x - local.x) + local.y);
			left.x = local.x;
			left.y = round(k1*(local.x - face.x) + face.y);
		}
		else if(left.x == 0 && right.x != 0)
		{
			const float k2 = (float)right.y/right.x;
			left.x = face.x;
			left.y = round(k2*(face.x - local.x) + local.y);
			right.x = local.x;
			right.y = round(k2*(local.x - face.x) + face.y);
		}
		else if(left.x == 0 && right.x == 0)
		{
			left.x = local.x;
			left.y = face.y;
			right.x = face.x;
			right.y = local.y;
		}
		else if(left.x != 0 && right.x != 0)
		{
			const float k1 = (float)left.y/left.x;
			const float k2 = (float)right.y/right.x;
			left.x = round(((local.y - face.y) + (k1*face.x - k2*local.x))/(k1 - k2));
			left.y = round(((k2*face.y - k1*local.y) - (k1*k2*face.x - k1*k2*local.x))/(k2 - k1));
			right.x = round(((local.y - face.y) + (k2*face.x - k1*local.x))/(k2 - k1));
			right.y = round(((k1*face.y - k2*local.y) - (k1*k2*face.x - k1*k2*local.x))/(k1 - k2));
		}

		if(m_pPoints != nullptr)
		{
			std::map<unsigned int, CPoint> oldpoints;
			std::map<unsigned int, CPoint> newpoints;

			local = local + m_pPoints[fAnchor - 1];
			left = left + m_pPoints[fAnchor - 1];
			right = right + m_pPoints[fAnchor - 1];
			face = face + m_pPoints[fAnchor - 1];

			oldpoints[nAnchor - 1] = m_pPoints[nAnchor - 1];
			oldpoints[fAnchor - 1] = m_pPoints[fAnchor - 1];
			oldpoints[lAnchor - 1] = m_pPoints[lAnchor - 1];
			oldpoints[rAnchor - 1] = m_pPoints[rAnchor - 1];
			oldpoints[4] = m_pPoints[4];

			m_pPoints[nAnchor - 1] = local;
			m_pPoints[fAnchor - 1] = face;
			m_pPoints[lAnchor - 1] = left;
			m_pPoints[rAnchor - 1] = right;

			oldpoints[nAnchor - 1] = m_pPoints[nAnchor - 1];
			oldpoints[fAnchor - 1] = m_pPoints[fAnchor - 1];
			oldpoints[lAnchor - 1] = m_pPoints[lAnchor - 1];
			oldpoints[rAnchor - 1] = m_pPoints[rAnchor - 1];
			m_pPoints[4] = m_pPoints[0];

			newpoints[nAnchor - 1] = m_pPoints[nAnchor - 1];
			newpoints[fAnchor - 1] = m_pPoints[fAnchor - 1];
			newpoints[lAnchor - 1] = m_pPoints[lAnchor - 1];
			newpoints[rAnchor - 1] = m_pPoints[rAnchor - 1];
			newpoints[4] = m_pPoints[4];

			RecalRect();
			m_bModified = true;

			if(pActionstack != nullptr)
			{
				Undoredo::CModify<CWnd*, CPRect*, std::map<unsigned int, CPoint>>* pModify = new Undoredo::CModify<CWnd*, CPRect*, std::map<unsigned int, CPoint>>(nullptr, this, oldpoints, newpoints);
				pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::PRect::Undo_Modify_Anchors;
				pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::PRect::Redo_Modify_Anchors;
				pActionstack->Record(pModify);
			}
		}
	}
}

bool CPRect::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CPath::PickOn(point, gap) == false)
		return false;

	if(m_bRegular == false && m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i < 5; i++)
		{
			const CPoint& fpoint = m_pPoints[i - 1];
			const CPoint& tpoint = m_pPoints[i + 1 - 1];

			if(PointOnLine(point, fpoint, tpoint, gap) == true)
				return true;
		}
	}
	else
	{
		CRect rect = m_Rect;
		rect.InflateRect(gap, gap);
		if(rect.PtInRect(point) == TRUE)
		{
			const int deltaLeft = abs(point.x - m_Rect.left);
			if(deltaLeft <= gap)
				return true;
			const int deltaRight = abs(point.x - m_Rect.right);
			if(deltaRight <= gap)
				return true;
			const int deltaTop = abs(point.y - m_Rect.top);
			if(deltaTop <= gap)
				return true;
			const int deltaBottom = abs(point.y - m_Rect.bottom);
			if(deltaBottom <= gap)
				return true;
		}
	}

	return false;
}

bool CPRect::PickIn(const CPoint& point) const
{
	if(CPath::PickIn(point) == false)
		return false;
	else if(m_bRegular == false && m_pPoints != nullptr)
		return CPoly::PointInPolygon(point, m_pPoints, 5);
	else
		return CPath::PickIn(point);
}

bool CPRect::PickIn(const CRect& rect) const
{
	if(CPath::PickIn(rect) == false)
		return false;
	const CPoint point1 = CPoint(rect.left, rect.top);
	const CPoint point2 = CPoint(rect.left, rect.bottom);
	const CPoint point3 = CPoint(rect.right, rect.top);
	const CPoint point4 = CPoint(rect.right, rect.bottom);
	if(this->PickIn(point1) == true || this->PickIn(point2) == true || this->PickIn(point3) == true || this->PickIn(point4) == true)
	{
		return true;
	}

	if(m_bRegular == false && m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i <= 5; i++)
		{
			const CPoint point = GetAnchor(i);
			if(rect.PtInRect(point) == TRUE)
				return true;
		}

		for(unsigned int i = 1; i < 5; i++)
		{
			const long x1 = m_pPoints[i - 1].x;
			const long y1 = m_pPoints[i - 1].y;
			const long x2 = m_pPoints[i].x;
			const long y2 = m_pPoints[i].y;

			for(unsigned int j = 1; j < 5; j++)
			{
				const long u1 = j == 1 || j == 4 ? rect.left : rect.right;
				const long v1 = j == 1 || j == 2 ? rect.top : rect.bottom;
				const long u2 = j == 1 || j == 2 ? rect.right : rect.left;
				const long v2 = j == 1 || j == 4 ? rect.top : rect.bottom;

				if((u1 == u2) && (v1 == v2))
					continue;
				const long a1 = (y2 - v1)*(x2 - x1) > (y2 - y1)*(x2 - u1) ? 1 : ((y2 - v1)*(x2 - x1) == (y2 - y1)*(x2 - u1) ? 0 : -1);
				const long a2 = (y2 - v2)*(x2 - x1) > (y2 - y1)*(x2 - u2) ? 1 : ((y2 - v2)*(x2 - x1) == (y2 - y1)*(x2 - u2) ? 0 : -1);
				const long a3 = (v2 - y1)*(u2 - u1) > (v2 - v1)*(u2 - x1) ? 1 : ((v2 - y1)*(u2 - u1) == (v2 - v1)*(u2 - x1) ? 0 : -1);
				const long a4 = (v2 - y2)*(u2 - u1) > (v2 - v1)*(u2 - x2) ? 1 : ((v2 - y2)*(u2 - u1) == (v2 - v1)*(u2 - x2) ? 0 : -1);

				if(a1*a2 < 0 && a3*a4 < 0)
				{
					return true;
				}
			}
		}

		return false;
	}
	else
		return CPath::PickIn(rect);
}

bool CPRect::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CPath::PickIn(center, radius) == false)
		return false;

	if(this->PickIn(center) == true)
		return true;

	int sqradius = radius*radius;
	if(m_bRegular == false)
	{
		for(unsigned int i = 1; i < 5; i++)
		{
			const CPoint& point1 = GetAnchor(i - 1);
			const CPoint& point2 = GetAnchor(i + 1 - 1);
			if(::Distance2(center, point1, point2) <= sqradius)
			{
				return true;
			}
		}

		return false;
	}
	else
		return true;
}

bool CPRect::PickIn(const CPoly& polygon) const
{
	if(CPath::PickIn(polygon.m_Rect) == false)
		return false;

	if(m_bRegular == false && m_pPoints != nullptr)
	{
		for(unsigned int i = 1; i <= 5; i++)
		{
			const CPoint& point = GetAnchor(i);
			if(polygon.PickIn(point) == true)
			{
				return true;
			}
		}

		for(unsigned int j = 1; j < polygon.m_nAnchors; j++)
		{
			const CPoint& point = polygon.GetAnchor(j);
			if(this->PickIn(point) == true)
			{
				return true;
			}
		}

		for(unsigned int i = 1; i < 5; i++)
		{
			const long x1 = m_pPoints[i - 1].x;
			const long y1 = m_pPoints[i - 1].y;
			const long x2 = m_pPoints[i].x;
			const long y2 = m_pPoints[i].y;

			for(unsigned int j = 1; j < polygon.m_nAnchors; j++)
			{
				const long u1 = polygon.m_pPoints[j - 1].x;
				const long v1 = polygon.m_pPoints[j - 1].y;
				const long u2 = polygon.m_pPoints[j].x;
				const long v2 = polygon.m_pPoints[j].y;

				if((u1 == u2) && (v1 == v2))
					continue;
				const long a1 = (y2 - v1)*(x2 - x1) > (y2 - y1)*(x2 - u1) ? 1 : ((y2 - v1)*(x2 - x1) == (y2 - y1)*(x2 - u1) ? 0 : -1);
				const long a2 = (y2 - v2)*(x2 - x1) > (y2 - y1)*(x2 - u2) ? 1 : ((y2 - v2)*(x2 - x1) == (y2 - y1)*(x2 - u2) ? 0 : -1);
				const long a3 = (v2 - y1)*(u2 - u1) > (v2 - v1)*(u2 - x1) ? 1 : ((v2 - y1)*(u2 - u1) == (v2 - v1)*(u2 - x1) ? 0 : -1);
				const long a4 = (v2 - y2)*(u2 - u1) > (v2 - v1)*(u2 - x2) ? 1 : ((v2 - y2)*(u2 - u1) == (v2 - v1)*(u2 - x2) ? 0 : -1);

				if(a1*a2 < 0 && a3*a4 < 0)
				{
					return true;
				}
			}
		}

		return false;
	}
	else
		return CPath::PickIn(polygon);
}

void CPRect::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CPath::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << m_bRegular;
		if(m_bRegular == false)
		{
			ar.Write(m_pPoints, 4*sizeof(CPoint));
		}
	}
	else
	{
		ar >> m_bRegular;
		if(m_bRegular == false)
		{
			m_pPoints = new CPoint[5];

			ar.Read(m_pPoints, 4*sizeof(CPoint));
			m_pPoints[4] = m_pPoints[0];
			const double fTg = this->GetTg();
			const double fSin = this->GetSin();
			const double fCos = this->GetCos();
			if(fTg == 0 && fSin == 0 && fCos == 1)
			{
				delete m_pPoints;
				m_pPoints = nullptr;
				m_bRegular = true;
			}
		}
	}
}

void CPRect::ReleaseCE(CArchive& ar) const
{
	CPath::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		if(m_bRegular == false)
		{
			ar << (unsigned int)5;
			for(unsigned int i = 0; i < 5; i++)
			{
				CPoint point = m_pPoints[i];
				point.x /= 10000;
				point.y /= 10000;
				ar << point.x;
				ar << point.y;
			}
		}
	}
	else
	{
	}
}

void CPRect::ReleaseDCOM(CArchive& ar)
{
	CPath::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar << m_bRegular;
		if(m_bRegular == true)
		{
			ar << m_Rect; //´óÔ¼Õ¼×ÜÊý¾ÓÁ¿µÄ16%
		}
		else
		{
			ar.Write(m_pPoints, 4*sizeof(CPoint));
		}
	}
	else
	{
		ar >> m_bRegular;
		if(m_bRegular == true)
		{
			ar >> m_Rect; //´óÔ¼Õ¼×ÜÊý¾ÓÁ¿µÄ16%	
		}
		else
		{
			m_pPoints = new CPoint[5];

			ar.Read(m_pPoints, 4*sizeof(CPoint));
			m_pPoints[4] = m_pPoints[0];
		}
	}
}

bool CPRect::GetValues(const CODBCRecordset& rs, const CDatainfo& datainfo, CString& strTag)
{
	return CPath::GetValues(rs, datainfo, strTag);
}

bool CPRect::PutValues(CODBCRecordset& rs, const CDatainfo& datainfo) const
{
	try
	{
		const double minX = datainfo.DocToMap(m_Rect.left);
		const double minY = datainfo.DocToMap(m_Rect.top);
		const double maxX = datainfo.DocToMap(m_Rect.right);
		const double maxY = datainfo.DocToMap(m_Rect.bottom);
		rs.Field(_T("minX")) = minX;
		rs.Field(_T("minY")) = minY;
		rs.Field(_T("maxX")) = maxX;
		rs.Field(_T("maxY")) = maxY;

		rs.Field(_T("Shape")) = 7;
		return true;
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->Delete();
	}
	catch(CException* ex)
	{
		ex->Delete();
	}
	return false;
}

void CPRect::RecalRect()
{
	if(m_bRegular == false && m_pPoints != nullptr)
	{
		CRect rect(CPoint(m_pPoints[0].x, m_pPoints[0].y), CSize(0, 0));

		for(unsigned int i = 1; i <= 5; i++)
		{
			if(m_pPoints[i - 1].x < rect.left)
				rect.left = m_pPoints[i - 1].x;
			if(m_pPoints[i - 1].x > rect.right)
				rect.right = m_pPoints[i - 1].x;
			if(m_pPoints[i - 1].y < rect.top)
				rect.top = m_pPoints[i - 1].y;
			if(m_pPoints[i - 1].y > rect.bottom)
				rect.bottom = m_pPoints[i - 1].y;
		}

		rect.NormalizeRect();
		m_Rect = rect;
	}
}

void CPRect::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	const bool oldRegular = m_bRegular;
	m_bRegular = false;
	CPoint* oldPoints = nullptr;
	if(m_pPoints != nullptr)
	{
		oldPoints = new CPoint[5];
		memcpy(oldPoints, m_pPoints, 5*sizeof(CPoint));
	}
	else
	{
		m_pPoints = new CPoint[5];

		m_pPoints[0] = CPoint(m_Rect.left, m_Rect.top);
		m_pPoints[1] = CPoint(m_Rect.left, m_Rect.bottom);
		m_pPoints[2] = CPoint(m_Rect.right, m_Rect.bottom);
		m_pPoints[3] = CPoint(m_Rect.right, m_Rect.top);
		m_pPoints[4] = m_pPoints[0];
	}

	for(unsigned int i = 0; i < 5; i++)
	{
		const long x = round(m_pPoints[i].x*m11 + m_pPoints[i].y*m21 + m31);
		const long y = round(m_pPoints[i].x*m12 + m_pPoints[i].y*m22 + m32);
		m_pPoints[i].x = x;
		m_pPoints[i].y = y;
	}

	DrawPath(pDC, viewinfo);

	m_bRegular = oldRegular;
	delete[] m_pPoints;
	m_pPoints = nullptr;
	if(oldPoints != nullptr)
	{
		m_pPoints = oldPoints;
	}
}

void CPRect::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_pPoints != nullptr)
	{
		for(unsigned int i = 0; i < 5; i++)
		{
			const long x = round(m_pPoints[i].x*m11 + m_pPoints[i].y*m21 + m31);
			const long y = round(m_pPoints[i].x*m12 + m_pPoints[i].y*m22 + m32);
			m_pPoints[i].x = x;
			m_pPoints[i].y = y;
		}
	}
	else if(m21 == 0.0f && m12 == 0.0f)
	{
		const long x = round(m_Rect.left*m11 + m31);
		const long y = round(m_Rect.top*m22 + m32);
		const long cx = round(m_Rect.Width()*m11);
		const long cy = round(m_Rect.Height()*m22);
		m_Rect.left = x;
		m_Rect.top = y;
		m_Rect.right = m_Rect.left + cx;
		m_Rect.right = m_Rect.top + cy;
	}
	else {
		m_bRegular = false;
		if(m_pPoints == nullptr)
		{
			m_pPoints = new CPoint[5];
			m_pPoints[0] = CPoint(m_Rect.left, m_Rect.top);
			m_pPoints[1] = CPoint(m_Rect.left, m_Rect.bottom);
			m_pPoints[2] = CPoint(m_Rect.right, m_Rect.bottom);
			m_pPoints[3] = CPoint(m_Rect.right, m_Rect.top);
			m_pPoints[4] = m_pPoints[0];
		}

		for(unsigned int i = 0; i < 5; i++)
		{
			const long x = round(m_pPoints[i].x*m11 + m_pPoints[i].y*m21 + m31);
			const long y = round(m_pPoints[i].x*m12 + m_pPoints[i].y*m22 + m32);
			m_pPoints[i].x = x;
			m_pPoints[i].y = y;
		}

		if(m_pPoints[0].x != m_pPoints[1].x)
		{
		}
		if(m_pPoints[2].x != m_pPoints[3].x)
		{
		}
		if(m_pPoints[0].y != m_pPoints[3].y)
		{
		}
		if(m_pPoints[1].y != m_pPoints[2].y)
		{
		}
		else
		{
			m_bRegular = true;
			delete[] m_pPoints;
			m_pPoints = nullptr;
		}
	}

	CPath::Transform(m11, m21, m31, m12, m22, m32);
}

void CPRect::Transform(const CViewinfo& viewinfo)
{
	if(m_bRegular == false && m_pPoints != nullptr)
	{
		const Gdiplus::Point point0 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[0]);
		const Gdiplus::Point point1 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[1]);
		const Gdiplus::Point point2 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[2]);
		const Gdiplus::Point point3 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[3]);
		const Gdiplus::Point point4 = viewinfo.DocToClient<Gdiplus::Point>(m_pPoints[4]);
		m_pPoints[0] = CPoint(point0.X, point0.Y);
		m_pPoints[1] = CPoint(point1.X, point1.Y);
		m_pPoints[2] = CPoint(point2.X, point2.Y);
		m_pPoints[3] = CPoint(point3.X, point3.Y);
		m_pPoints[4] = CPoint(point4.X, point4.Y);

		RecalRect();
	}
	else
		CPath::Transform(viewinfo);
}

void CPRect::ExportPts(FILE* file, const Gdiplus::Matrix& matrix, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[5];
	for(unsigned int i = 0; i < 5; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 5);

	fprintf(file, "%g %g m\n", points[0].X, points[0].Y);
	for(unsigned int i = 1; i < 5; i++)
	{
		fprintf(file, "%g %g L\n", points[i].X, points[i].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CPRect::ExportPts(FILE* file, const CViewinfo& viewinfo, const CString& suffix) const
{
	if(this->IsValid() == false)
		return;

	const CPoint* pPoints = this->GetPoints();
	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(pPoints, 5);
	delete[] pPoints;

	fprintf(file, "%g %g m\n", points[0].X, points[0].Y);
	for(unsigned int i = 1; i < 5; i++)
	{
		fprintf(file, "%g %g L\n", points[i].X, points[i].Y);
	}
	_ftprintf(file, _T("%s\n"), suffix);

	delete[] points;
}

void CPRect::ExportPts(HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(this->IsValid() == false)
		return;

	if(GetAnchors() < 2)
		return;

	const CPoint* pPoints = this->GetPoints();
	const Gdiplus::PointF* points = viewinfo.DocToClient<Gdiplus::PointF>(pPoints, 5);
	delete[] pPoints;

	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i < 5; i++)
	{
		HPDF_Page_LineTo(page, points[i].X, points[i].Y);
	}
	delete[] points;
}

void CPRect::ExportPts(HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(this->IsValid() == false)
		return;

	const CPoint* pPoints = this->GetPoints();
	Gdiplus::PointF* points = new Gdiplus::PointF[5];
	for(unsigned int i = 0; i < 5; i++)
	{
		points[i].X = pPoints[i].x;
		points[i].Y = pPoints[i].y;
	}
	delete[] pPoints;
	matrix.TransformPoints(points, 5);

	HPDF_Page_MoveTo(page, points[0].X, points[0].Y);
	for(unsigned int i = 1; i < 5; i++)
	{
		HPDF_Page_LineTo(page, points[i].X, points[i].Y);
	}
	delete[] points;
}

void CPRect::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(this->IsValid() == false)
		return;

	if(m_bClosed == true)
	{
		if(bTransGeo == true)
		{
			fprintf(fileMif, "Rect\n");
			const CPoint docPoint1(m_Rect.left, m_Rect.top);
			const CPointF geoPoint1 = datainfo.DocToGeo(docPoint1);
			fprintf(fileMif, "%g %g ", geoPoint1.x, geoPoint1.y);
			const CPoint docPoint2 = CPoint(m_Rect.right, m_Rect.bottom);
			const CPointF geoPoint2 = datainfo.DocToGeo(docPoint2);
			fprintf(fileMif, "%g %g\n", geoPoint2.x, geoPoint2.y);
		}
		else
		{
			fprintf(fileMif, "Rect\n");
			fprintf(fileMif, "%d %d %d %d\n", m_Rect.left, m_Rect.top, m_Rect.right, m_Rect.bottom);
		}
	}
	else
		CPath::ExportMapInfoMif(datainfo, fileMif, bTransGeo, tolerance);
}

DWORD CPRect::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CPath::PackPC(pFile, bytes);

	if(pFile != nullptr)
	{
		const BYTE bRegular = m_bRegular == false && m_pPoints != nullptr ? 0X00 : 0X01;
		pFile->Write(&bRegular, sizeof(BYTE));
		size += sizeof(BYTE);

		if(m_bRegular == false && m_pPoints != nullptr)
		{
			pFile->Write(m_pPoints, sizeof(CPoint)*5);
			size += 5*sizeof(CPoint);
		}

		return size;
	}
	else
	{
		const BYTE bRegular = *(BYTE*)bytes;
		bytes += sizeof(BYTE);
		m_bRegular = bRegular == 0X01 ? true : false;
		if(m_bRegular == false)
		{
			m_pPoints = new CPoint[5];
			memcpy(m_pPoints, bytes, sizeof(CPoint)*5);
			bytes += sizeof(CPoint)*5;
		}
	}
	return size;
}

DWORD CPRect::ReleaseCE(CFile& file, const BYTE& type) const
{
	DWORD size = CPath::ReleaseCE(file, type);

	if(m_bRegular == false)
	{
		const WORD nAnchors = 5;
		file.Write(&nAnchors, sizeof(WORD));
		file.Write(m_pPoints, nAnchors*sizeof(CPoint));

		size += sizeof(WORD) + nAnchors*sizeof(CPoint);
	}

	return size;
}

void CPRect::ReleaseWeb(CFile& file, CSize offset) const
{
	CPath::ReleaseWeb(file, offset);;

	const BYTE bRegular = m_bRegular == false && m_pPoints != nullptr ? 0X00 : 0X01;
	file.Write(&bRegular, sizeof(BYTE));
	if(bRegular == true)
	{
		short left = m_Rect.left - offset.cx;
		ReverseOrder(left);
		file.Write(&left, sizeof(short));

		short top = m_Rect.top - offset.cy;
		ReverseOrder(top);
		file.Write(&top, sizeof(short));

		short width = m_Rect.Width();
		ReverseOrder(width);
		file.Write(&width, sizeof(short));

		short height = m_Rect.Height();
		ReverseOrder(height);
		file.Write(&height, sizeof(short));
	}
	else
	{
		for(int index = 0; index < 5; index++)
		{
			short x = m_pPoints[index].x - offset.cx;
			short y = m_pPoints[index].y - offset.cy;
			ReverseOrder(x);
			file.Write(&x, sizeof(short));
			ReverseOrder(y);
			file.Write(&y, sizeof(short));
		}
	}
}

void CPRect::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CPath::ReleaseWeb(stream, offset);;
	const BYTE bRegular = m_bRegular == false && m_pPoints != nullptr ? 0X00 : 0X01;
	stream << bRegular;
	if(bRegular == true)
	{
		const short left = m_Rect.left - offset.cx;
		stream << left;
		const short top = m_Rect.top - offset.cy;
		stream << top;
		const short width = m_Rect.Width();
		stream << width;
		const short height = m_Rect.Height();
		stream << height;
	}
	else
	{
		for(int index = 0; index < 5; index++)
		{
			const short x = m_pPoints[index].x - offset.cx;
			const short y = m_pPoints[index].y - offset.cy;
			stream << x;
			stream << y;
		}
	}
}
void CPRect::ReleaseWeb(boost::json::object& json) const
{
	CPath::ReleaseWeb(json);

	if(m_bRegular == true)
	{
		std::vector<int> child;
		child.push_back(m_Rect.left);
		child.push_back(m_Rect.top);
		child.push_back(m_Rect.right);
		child.push_back(m_Rect.bottom);
		json["Rect"] = boost::json::value_from(child);
	}
	else
	{
		std::vector<int> child;
		for(int index = 0; index < 5; index++)
		{
			const int x = m_pPoints == nullptr ? 0 : m_pPoints[index].x;
			const int y = m_pPoints == nullptr ? 0 : m_pPoints[index].y;
			child.push_back(x);
			child.push_back(y);
		}
		json["Points"] = boost::json::value_from(child);
	}
}
void CPRect::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CPath::ReleaseWeb(writer, offset);

	writer.add_bool(m_bRegular);
	if(m_bRegular == false)
	{
		for(int index = 0; index < 5; index++)
		{
			const int x = m_pPoints == nullptr ? 0 : m_pPoints[index].x - offset.cx;
			const int y = m_pPoints == nullptr ? 0 : m_pPoints[index].y - offset.cy;
			writer.add_variant_sint32(x);
			writer.add_variant_sint32(y);
		}
	}
}
double CPRect::Shortcut(const CPoint& point, CPoint& trend) const
{
	if(m_bRegular == true)
		return CPath::Shortcut(point, trend);
	else
		return CPath::Shortcut(point, trend);
}
double CPRect::GetSin() const
{
	if(m_bRegular == true)
		return 0.0f;
	else if(m_pPoints != nullptr)
	{
		const double cx = m_pPoints[3].x - m_pPoints[0].x;
		const double cy = m_pPoints[3].y - m_pPoints[0].y;
		if(cx != 0 || cy != 0)
			return cy/sqrt(cx*cx + cy*cy);
		else
			return 0.0f;
	}
	else
		return 0.0f;
}
double CPRect::GetCos() const
{
	if(m_bRegular == true)
		return 1.0f;
	else if(m_pPoints != nullptr)
	{
		const double cx = m_pPoints[3].x - m_pPoints[0].x;
		const double cy = m_pPoints[3].y - m_pPoints[0].y;
		if(cx != 0 || cy != 0)
			return cx/sqrt(cx*cx + cy*cy);
		else
			return 1.0f;
	}
	else
		return 1.0f;
}
double CPRect::GetTg() const
{
	if(m_bRegular == true)
		return 0.0f;
	else if(m_pPoints != nullptr)
	{
		const double fSin = this->GetSin();
		const double fCos = this->GetCos();
		const double cx = m_pPoints[1].x - m_pPoints[0].x;
		const double cy = m_pPoints[1].y - m_pPoints[0].y;
		const double x = cx*fCos + cy*fSin;
		const double y = -cx*fSin + cy*fCos;
		if(y != 0.0f)
			return x/y;
		else
			return 0.0f;
	}
	else
		return 0.0f;
}
