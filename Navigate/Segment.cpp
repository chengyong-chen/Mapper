#include "stdafx.h"

#include "Segment.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Global.h"
#include "../Utility/Rect.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSegment, CObject, 0)

CSegment::CSegment()
{
	m_nTurnType = 0;
	m_bPromted = false;
	m_pPoly = nullptr;
}

CSegment::~CSegment()
{
	delete m_pPoly;
}

bool CSegment::Over()
{
	return false;
}

void CSegment::Draw(CView* pView, const CViewinfo& viewinfo, Gdiplus::Graphics& g, Gdiplus::Pen* pen, const CRectF& geoRect) const
{
	if(m_pPoly==nullptr)
		return;
	const CRect rect1 = CRect(geoRect.left*10000000, geoRect.top*10000000, geoRect.right*10000000, geoRect.bottom*10000000);
	if(Util::Rect::Intersect(rect1, m_pPoly->m_Rect)==true)
	{
		Gdiplus::PointF* points = new Gdiplus::PointF[m_pPoly->m_nAnchors];
		for(unsigned int index = 0; index<m_pPoly->m_nAnchors; index++)
		{
			const CPointF geoPoint = CPointF(m_pPoly->m_pPoints[index].x/10000000.f, m_pPoly->m_pPoints[index].y/10000000.f);
			points[index] = viewinfo.WGS84ToClient(geoPoint);
		}

		g.DrawLines(pen, points, m_pPoly->m_nAnchors);

		delete[] points;
		points = nullptr;
	}
}
