#include "stdafx.h"
#include "Mapinfo.h"
#include <cmath>
#include "../Projection/Projection1.h"
#include "../Projection/ProjectDlg.h"
#include "../Projection/Interpolator.hpp"
#include "../Utility/Link.hpp"
#include "../Utility/Constants.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CMapinfo::CMapinfo()
	:m_bResamplable(false)
{
	m_mapOrigin = CPointF(0, 0);
	m_mapCanvas = CSizeF(595, 842);

	m_dilationDocToMap = 0.0001;
	m_dilationMapToDoc = 10000;
}

CMapinfo::~CMapinfo()
{
	if(m_pProjection != nullptr)
	{
		delete m_pProjection;
		m_pProjection = nullptr;
	}
}
bool CMapinfo::IsLngLat() const
{
	return m_pProjection == nullptr ? false : m_pProjection->IsLngLat();
}
CRectF CMapinfo::GetMapRect() const
{
	return CRectF(m_mapOrigin, m_mapCanvas);
}
inline void CMapinfo::DocToMap(const int& xi, const int& yi, double& xo, double& yo) const
{
	xo = xi*m_dilationDocToMap;
	yo = yi*m_dilationDocToMap;
}
CPointF CMapinfo::DocToMap(const long& x, const long& y) const
{
	CPointF mapPoint;
	mapPoint.x = x*m_dilationDocToMap;
	mapPoint.y = y*m_dilationDocToMap;
	return mapPoint;
}

CSizeF CMapinfo::DocToMap(const unsigned long& cx, const unsigned long& cy) const
{
	CSizeF mapSize;
	mapSize.cx = cx*m_dilationDocToMap;
	mapSize.cy = cy*m_dilationDocToMap;
	return mapSize;
}

CPointF CMapinfo::DocToMap(const CPoint& docPoint) const
{
	CPointF mapPoint;
	mapPoint.x = docPoint.x*m_dilationDocToMap;
	mapPoint.y = docPoint.y*m_dilationDocToMap;
	return mapPoint;
}

CSizeF CMapinfo::DocToMap(const CSize& docSize) const
{
	CSizeF mapSize;
	mapSize.cx = docSize.cx*m_dilationDocToMap;
	mapSize.cy = docSize.cy*m_dilationDocToMap;
	return mapSize;
}

CRectF CMapinfo::DocToMap(const CRect& docRect) const
{
	CRectF mapRect;
	mapRect.left = docRect.left*m_dilationDocToMap;
	mapRect.top = docRect.top*m_dilationDocToMap;
	mapRect.right = docRect.right*m_dilationDocToMap;
	mapRect.bottom = docRect.bottom*m_dilationDocToMap;
	return mapRect;
}

double CMapinfo::DocToMap(const double& Δ) const
{
	return  Δ*m_dilationDocToMap;
}
inline void CMapinfo::DocToGeo(const int& x, const int& y, double& λ, double& ψ) const
{
	this->DocToMap(x,y, λ, ψ);
	if(m_pProjection != nullptr)
		m_pProjection->MapToGeo(λ, ψ, λ, ψ);
}
CPointF CMapinfo::DocToGeo(const CPoint& docPoint) const
{
	const CPointF mapPoint = this->DocToMap(docPoint);
	return m_pProjection == nullptr ? mapPoint : m_pProjection->MapToGeo(mapPoint);
}
CPoint CMapinfo::GeoToDoc(const double& λ, const double& ψ) const
{
	const CPointF mapPoint = m_pProjection == nullptr ? CPointF(λ, ψ) : m_pProjection->GeoToMap(λ, ψ);
	return this->MapToDoc(mapPoint);
}

CPoint CMapinfo::GeoToDoc(const CPointF& geoPoint) const
{
	const CPointF mapPoint = m_pProjection == nullptr ? geoPoint : m_pProjection->GeoToMap(geoPoint);
	return this->MapToDoc(mapPoint);
}
std::pair<unsigned int, CPoint*> CMapinfo::GeoToDoc(Link::Vertex* vertex)
{
	unsigned int count = Link::GetLength(vertex);
	CPoint* pPoints = new CPoint[count];
	CPoint* mover = pPoints;
	while(vertex != nullptr)
	{
		CPointF mapPoint = m_pProjection->GeoToMap(vertex->λ, vertex->ψ);
		*mover = this->MapToDoc(mapPoint);
		mover++;
		vertex = vertex->next;
	}
	return std::make_pair(count, pPoints);
}
CPointF CMapinfo::DocToWGS84(const CPoint& docPoint) const
{
	const CPointF geoPoint = this->DocToGeo(docPoint);
	return m_pProjection != nullptr ? m_pProjection->GCS.ThistoWGS84(geoPoint) : geoPoint;
}

CPoint CMapinfo::WGS84ToDoc(const CPointF& geoPoint) const
{
	if(m_pProjection != nullptr)
	{
		const CPointF point = m_pProjection->GCS.WGS84toThis(geoPoint);
		return GeoToDoc(point);
	}
	else
		return this->GeoToDoc(geoPoint);
}

CPoint CMapinfo::MapToDoc(const double& x, const double& y) const
{
	CPoint docPoint;
	docPoint.x = std::lround(x*m_dilationMapToDoc);
	docPoint.y = std::lround(y*m_dilationMapToDoc);
	return docPoint;
}

CPoint CMapinfo::MapToDoc(const CPointF& mapPoint) const
{
	CPoint docPoint;
	docPoint.x = std::lround(mapPoint.x*m_dilationMapToDoc);
	docPoint.y = std::lround(mapPoint.y*m_dilationMapToDoc);
	return docPoint;
}

CSize CMapinfo::MapToDoc(const CSizeF& mapSize) const
{
	CSize docSize;
	docSize.cx = std::lround(mapSize.cx*m_dilationMapToDoc);
	docSize.cy = std::lround(mapSize.cy*m_dilationMapToDoc);
	return docSize;
}

CRect CMapinfo::MapToDoc(const CRectF& mapRect) const
{
	CRect docRect;
	docRect.left = std::lround(mapRect.left*m_dilationMapToDoc);
	docRect.top = std::lround(mapRect.top*m_dilationMapToDoc);
	docRect.right = std::lround(mapRect.right*m_dilationMapToDoc);
	docRect.bottom = std::lround(mapRect.bottom*m_dilationMapToDoc);
	return docRect;
}

int CMapinfo::MapToDoc(const double& Δ) const
{
	return std::lround(Δ*m_dilationMapToDoc);
}
void CMapinfo::Detail(CPointF point1, CPointF point2, unsigned int steps, double& minx, double& miny, double& maxx, double& maxy, CProjection* pProjection1, CProjection* pProjection2)
{
	double length = sqrt((point1.x - point2.x)*(point1.x - point2.x) + (point1.y - point2.y)*(point1.y - point2.y));
	for(int step = 0; step <= steps; step++)
	{
		double t = (double)step/steps;
		CPointF mapPoint(point1.x + t*(point2.x - point1.x), point1.y + t*(point2.y - point1.y));
		CPointF geoPoint1 = pProjection1->MapToGeo(mapPoint);
		CPointF geoPoint2 = pProjection1->GCS.GeoToGeo(pProjection2->GCS, geoPoint1);
		CPointF mapPoint2 = pProjection2->GeoToMap(geoPoint2);

		minx = isinf(mapPoint2.x) ? minx : min(mapPoint2.x, minx);
		miny = isinf(mapPoint2.y) ? miny : min(mapPoint2.y, miny);
		maxx = isinf(mapPoint2.x) ? maxx : max(mapPoint2.x, maxx);
		maxy = isinf(mapPoint2.y) ? maxy : max(mapPoint2.y, maxy);
	}
}
void CMapinfo::ResetProjection(CProjection* pProjection, bool action)
{
	m_dilationDocToMap = pProjection==nullptr ? 0.0001 : pProjection->GetDilationDocToMap();
	m_dilationMapToDoc = pProjection==nullptr ? 10000 : pProjection->GetDilationMapToDoc();

	if(m_pProjection == nullptr && pProjection != nullptr)
	{
		m_pProjection = pProjection;
	}
	else if(m_pProjection != nullptr && pProjection == nullptr)
	{
		delete m_pProjection;
		m_pProjection = nullptr;
	}
	else if(m_pProjection == nullptr && pProjection == nullptr)
	{
	}
	else if(*m_pProjection != *pProjection)
	{
		delete m_pProjection;
		m_pProjection = pProjection;
	}
	else
	{
		delete pProjection;
	}	
}

bool CMapinfo::ChangeProjection(bool action)
{
	HINSTANCE hInst = ::LoadLibrary(_T("Projection.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);

	CProjectDlg dlg(nullptr, m_pProjection, action);
	if(dlg.DoModal() == IDOK)
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);

		CProjection* pProjection = dlg.d_pProjection;
		dlg.d_pProjection = nullptr;

		ResetProjection(pProjection, action);

		return action && dlg.m_bInterpolate;
	}
	else
	{
		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
		return false;
	}
}

double CMapinfo::CalLength(CPoint* pPoints, int nAnchors) const
{
	if(m_pProjection != nullptr)
	{
		CPointF* points = new CPointF[nAnchors];
		for(int index = 0; index < nAnchors; index++)
		{
			this->DocToMap(pPoints[index].x, pPoints[index].y, points[index].x, points[index].y);
			m_pProjection->MapToGeo(points[index].x, points[index].y, points[index].x, points[index].y);
		}
		const double length = m_pProjection->CalLength(points, nAnchors);
		delete[] points;
		points = nullptr;
		return length;
	}
	else
		return 0;
}

double CMapinfo::CalArea(CPoint* pPoints, int nAnchors) const
{
	if(m_pProjection != nullptr)
	{
		CPointF* points = new CPointF[nAnchors];
		for(int index = 0; index < nAnchors; index++)
		{
			this->DocToMap(pPoints[index].x, pPoints[index].y, points[index].x, points[index].y);
		}
		const double area = m_pProjection->CalArea(points, nAnchors);
		delete[] points;
		points = nullptr;
		return area;
	}
	else
		return 0.0f;
}