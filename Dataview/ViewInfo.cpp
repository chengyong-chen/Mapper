#include "stdafx.h"
#include "Viewinfo.h"
#include "Datainfo.h"
#include <cmath>
#include <vector>

#include "../Projection/Projection1.h"
#include "../Projection/Geographic.h"
#include "../Projection/Geocentric.h"
#include "../Projection/ProjectDlg.h"
#include "../Projection/ParameterDlg.h"

#include "../Projection/Interpolator.hpp"
#include "../Projection/Antimeridian.h"
#include "../Projection/Circler.h"

#include "../Public/Function.h"
#include "../Utility/Link.hpp"
#include "../Utility/Scale.hpp"
#include "../Utility/RectangleWalker.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CViewinfo::CViewinfo(const CDatainfo& foreground)
	: m_datainfo(foreground), m_sizeDPI(72, 72), m_bViewer(false), m_clipper(nullptr)
{
	m_xFactorMapToView = 0.1f;
	m_yFactorMapToView = 0.1f;
	m_ptViewPos.x = 0;
	m_ptViewPos.y = 0;
	m_mapOrigin = m_datainfo.m_mapOrigin;
	m_mapCanvas = m_datainfo.m_mapCanvas;
	m_scaleCurrent = -1;
	m_levelCurrent = 0;
	m_pGeocentric = nullptr;

	if(m_datainfo.m_pProjection ==nullptr)
		m_clipper = nullptr;
	else if(m_datainfo.m_pProjection->IsLngLat())
		m_clipper = new projection::Antimeridian(0, 0);

	CViewinfo::DetermineLevel();
}
void CViewinfo::DetermineLevel()
{
	const double square = max(m_mapCanvas.cx, m_mapCanvas.cy);
	const double factorLevel0 = 256.f/square;
	const double ratioMaximum = CDatainfo::GetMapToViewFactorFromScale(this->m_pGeocentric != nullptr ? this->m_pGeocentric : m_datainfo.m_pProjection, m_datainfo.m_scaleMaximum, CPointF(m_mapOrigin.x + m_mapCanvas.cx/2, m_mapOrigin.y + m_mapCanvas.cy/2), CSize(72, 72));
	const double ratioMinimum = CDatainfo::GetMapToViewFactorFromScale(this->m_pGeocentric != nullptr ? this->m_pGeocentric : m_datainfo.m_pProjection, m_datainfo.m_scaleMinimum, CPointF(m_mapOrigin.x + m_mapCanvas.cx/2, m_mapOrigin.y + m_mapCanvas.cy/2), CSize(72, 72));
	m_levelMinimum = ratioMinimum < factorLevel0 ? 0 : floor(log(ratioMinimum/factorLevel0)/log(2.0));
	m_levelMaximum = ratioMaximum < factorLevel0 ? 0 : ceil(log(ratioMaximum/factorLevel0)/log(2.0));
}

CViewinfo::~CViewinfo()
{
	if(m_clipper!=nullptr)
	{
		delete m_clipper;
		m_clipper = nullptr;
	}
}
const CViewinfo& CViewinfo::operator=(const CViewinfo& source)
{
	m_xFactorMapToView = source.m_xFactorMapToView;
	m_yFactorMapToView = source.m_yFactorMapToView;
	m_ptViewPos = source.m_ptViewPos;
	m_mapOrigin = source.m_mapOrigin;
	m_mapCanvas = source.m_mapCanvas;
	m_scaleCurrent = source.m_scaleCurrent;
	m_levelCurrent = source.m_levelCurrent;
	m_sizeView = source.m_sizeView;
	delete m_pGeocentric;
	if(source.m_pGeocentric != nullptr)
	{
		m_pGeocentric = (CGeocentric*)(source.m_pGeocentric->Clone());
	}
	return *this;
}
double CViewinfo::CurrentRatio() const
{
	return m_datainfo.m_scaleSource/m_scaleCurrent;
}
void CViewinfo::ResetProjection(CGeocentric* pGeocentric, bool action)
{
	if(m_datainfo.m_pProjection == nullptr)
	{
		delete pGeocentric;
		delete m_pGeocentric;
		m_pGeocentric = nullptr;
	}
	else if(pGeocentric == nullptr)
	{
		delete m_pGeocentric;
		m_pGeocentric = nullptr;
	}
	else if(*pGeocentric != *m_datainfo.m_pProjection)
	{
		delete m_pGeocentric;
		m_pGeocentric = pGeocentric;
	}
	else
	{
		delete pGeocentric;
		return;
	}

	CPointF center = m_clipper!=nullptr ? m_clipper->GetCenter() : CPointF(0, 0);
	delete m_clipper;
	m_clipper = nullptr;

	if(m_pGeocentric == nullptr)
	{
		m_mapOrigin = m_datainfo.m_mapOrigin;
		m_mapCanvas = m_datainfo.m_mapCanvas;
		if(m_datainfo.m_pProjection!= nullptr &&  m_datainfo.m_pProjection->IsLngLat())
		{
			m_clipper = new projection::Antimeridian(center.x, center.y);
		}
	}
	else
	{
		optional<double> radius = pGeocentric->GetRadius();
		pGeocentric->SetCentral(0, 0);

		CRectF range = TransformRange(pGeocentric);
		m_mapOrigin.x = range.left;
		m_mapOrigin.y = range.top;
		m_mapCanvas.cx = range.right - range.left;
		m_mapCanvas.cy = range.bottom - range.top;

		m_clipper = (radius != nullopt) ? (projection::Clipper*)(new projection::Circler(center.x, center.y, radius.value())) : (projection::Clipper*)(new projection::Antimeridian(center.x, center.y));
	}
	DetermineLevel();
}
CRectF CViewinfo::TransformRange(CGeocentric* pGeocentric) const
{
	double minx = 100000000;
	double miny = 100000000;
	double maxx = -100000000;
	double maxy = -100000000;

	CPointF central = pGeocentric->GetCenter();
	pGeocentric->SetCentral(0, 0);
	CRectF  rect = m_datainfo.GetMapRect();
	optional<double> radius = pGeocentric->GetRadius();
	double left = radius==nullopt ? -constants::pi : max(-constants::pi, max(rect.left, -radius.value()));
	double top = radius==nullopt ? -constants::halfpi : max(-constants::halfpi, max(rect.top, -radius.value()));
	double right = radius==nullopt ? constants::pi : min(constants::pi, min(rect.right, +radius.value()));
	double bottom = radius==nullopt ? constants::halfpi : min(constants::halfpi, min(rect.bottom, +radius.value()));

	CMapinfo::Detail(CPointF(left, top), CPointF(right, top), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);
	CMapinfo::Detail(CPointF(right, top), CPointF(right, bottom), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);
	CMapinfo::Detail(CPointF(right, bottom), CPointF(left, bottom), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);
	CMapinfo::Detail(CPointF(left, bottom), CPointF(left, top), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);
	CMapinfo::Detail(CPointF(left, (top+bottom)/2), CPointF(right, (top+bottom)/2), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);
	CMapinfo::Detail(CPointF((left+right)/2, top), CPointF((left+right)/2, bottom), 100, minx, miny, maxx, maxy, m_datainfo.m_pProjection, pGeocentric);

	return CRectF(minx, miny, maxx, maxy);
}
CPointF CViewinfo::ClientToDataMap(const CPoint& cliPoint) const//if there is a backgrounp the result will be the backgroung' map coordinate, otherwise it's foreground's coordinate
{
	double x = cliPoint.x + m_ptViewPos.x;//Client to View
	double y = cliPoint.y + m_ptViewPos.y;
	x /= m_xFactorMapToView;//View to Map b
	y /= m_yFactorMapToView;
	y = m_mapCanvas.cy - y;
	x += m_mapOrigin.x;
	y += m_mapOrigin.y;
	if(x<m_mapOrigin.x)
		x = m_mapOrigin.x;
	else if(x>(m_mapOrigin.x+m_mapCanvas.cx))
		x = m_mapOrigin.x+m_mapCanvas.cx;
	if(y<m_mapOrigin.y)
		y = m_mapOrigin.y;
	else if(y>(m_mapOrigin.y+m_mapCanvas.cy))
		y = m_mapOrigin.y+m_mapCanvas.cy;

	if(m_pGeocentric != nullptr)
	{
		CPointF geoPoint = m_pGeocentric->MapToGeo(x, y);
		if(isinf(geoPoint.x) || isinf(geoPoint.y))
			return geoPoint;
		if(isnan(geoPoint.x) || isnan(geoPoint.y))
			return geoPoint;

		if(m_clipper != nullptr)
		{
			m_clipper->convertout(geoPoint.x, geoPoint.y);
		}
		return m_datainfo.m_pProjection->GeoToMap(geoPoint);
	}
	else if(m_datainfo.IsLngLat() == false)
		return CPointF(x, y);
	else if(m_clipper!= nullptr && m_clipper->IsSpinned())
	{
		m_datainfo.m_pProjection->MapToGeo(x, y, x, y);
		if(m_clipper != nullptr)
		{
			m_clipper->convertout(x, y);
		}
		return m_datainfo.m_pProjection->GeoToMap(x, y);
	}
	else
		return CPointF(x, y);
}
CSize CViewinfo::ClientToDoc(const Gdiplus::SizeF& cliSize) const
{
	if(m_pGeocentric != nullptr)
	{
		double cxMap = cliSize.Width/m_xFactorMapToView;
		double cyMap = cliSize.Height/m_yFactorMapToView;

		cxMap /= (m_mapCanvas.cy/m_datainfo.m_mapCanvas.cy);
		cyMap /= (m_mapCanvas.cy/m_datainfo.m_mapCanvas.cy);

		return CSize(abs(cxMap*m_datainfo.m_dilationMapToDoc), abs(cyMap*m_datainfo.m_dilationMapToDoc));
	}
	else if(m_datainfo.m_pProjection != nullptr)
		return CSize(round(m_datainfo.m_dilationMapToDoc*cliSize.Width/m_xFactorMapToView), round(m_datainfo.m_dilationMapToDoc*cliSize.Height/m_xFactorMapToView));
	else
		return CSize(round(10000*cliSize.Width/m_xFactorMapToView), round(10000*cliSize.Height/m_xFactorMapToView));
}
CPoint CViewinfo::ClientToDoc(const CPoint& cliPoint, bool inonly) const
{
	const CPointF mapPoint = this->ClientToDataMap(cliPoint);
	return m_datainfo.MapToDoc(mapPoint);
}
bool CViewinfo::ClientToDoc(const CPoint& cliPoint, CPoint& docPoint) const
{
	const CPointF mapPoint = this->ClientToDataMap(cliPoint);
	if(isinf(mapPoint.x) || isinf(mapPoint.y))
		return false;
	else
	{
		docPoint = m_datainfo.MapToDoc(mapPoint);
		return true;
	}
}
CRect CViewinfo::ClientToDoc(const CRect& cliRect) const
{
	CRect docRect(INT32_MAX, INT32_MAX, INT32_MIN, INT32_MIN);
	CRect bound = m_datainfo.GetDocRect();
	if(cliRect.Width()>=m_sizeView.cx)
	{
		docRect.left = bound.left;
		docRect.right = bound.right;
	}
	else if(m_pGeocentric==nullptr)
	{
		CPoint docPoint1 = this->ClientToDoc(CPoint(cliRect.left, cliRect.top));
		CPoint docPoint2 = this->ClientToDoc(CPoint(cliRect.right, cliRect.top));
		docRect.left = docPoint1.x;
		docRect.right = docPoint2.x;
	}
	else
	{
		RectangleWalker<int> walker(cliRect.left, cliRect.top, cliRect.right, cliRect.bottom, 1);
		float step = 0.25;
		int x;
		int y;
		{
			float d = 3;
			while(d<=4)
			{
				if(walker.GetPosition(d, x, y))
				{
					CPoint docPoint;
					if(this->ClientToDoc(CPoint(x, y), docPoint))
						docRect.left = min(docRect.left, docPoint.x);
				}
				d += step;
			}
		}
		{
			float d = 1;
			while(d<=2)
			{
				if(walker.GetPosition(d, x, y))
				{
					CPoint docPoint;
					if(this->ClientToDoc(CPoint(x, y), docPoint))
						docRect.right = max(docRect.right, docPoint.x);
				}
				d += step;
			}
		}
	}

	if(cliRect.Height()>=m_sizeView.cy)
	{
		docRect.top = bound.top;
		docRect.bottom = bound.bottom;
	}
	else if(m_pGeocentric == nullptr)
	{
		CPoint docPoint1 = this->ClientToDoc(CPoint(cliRect.right, cliRect.bottom));
		docRect.top = docPoint1.y;
		CPoint docPoint2 = this->ClientToDoc(CPoint(cliRect.left, cliRect.top));		
		docRect.bottom = docPoint2.y;
	}
	else
	{
		RectangleWalker<int> walker(cliRect.left, cliRect.top, cliRect.right, cliRect.bottom, 1);
		float step = 0.25;
		int x;
		int y;
		{
			float d = 0;
			while(d<=1)
			{
				if(walker.GetPosition(d, x, y))
				{
					CPoint docPoint;
					if(this->ClientToDoc(CPoint(x, y), docPoint))
						docRect.top = min(docRect.top, docPoint.y);
				}
				d += step;
			}
		}
		{
			float d = 2;
			while(d<=3)
			{
				if(walker.GetPosition(d, x, y))
				{
					CPoint docPoint;
					if(this->ClientToDoc(CPoint(x, y), docPoint))
						docRect.bottom = max(docRect.bottom, docPoint.y);
				}
				d += step;
			}
		}
	}
	if(cliRect.Width()<=0)
	{
	}
	else if(docRect.left>=docRect.right)
	{
		docRect.left = bound.left;
		docRect.right = bound.right;
	}
	if(cliRect.Height()<=0)
	{
	}
	else if(docRect.top>=docRect.bottom)
	{
		docRect.top = bound.top;
		docRect.bottom = bound.bottom;
	}
	return docRect;
}

Gdiplus::PointF CViewinfo::ViewMapToClient(double x, double y) const
{
	x -= m_mapOrigin.x;
	y -= m_mapOrigin.y;

	x *= m_xFactorMapToView;
	y *= m_yFactorMapToView;
	y = m_sizeView.cy - y;
	x -= m_ptViewPos.x;
	y -= m_ptViewPos.y;
	return Gdiplus::PointF(x, y);
}

Gdiplus::PointF CViewinfo::DataMapToView(double x, double y) const
{	
	if(m_datainfo.m_pProjection == nullptr)
	{
		x -= m_datainfo.m_mapOrigin.x;
		y -= m_datainfo.m_mapOrigin.y;
	}	
	else if(m_pGeocentric == nullptr)
	{
		if(m_datainfo.IsLngLat() && m_clipper->IsSpinned())
		{
			double λ;
			double ψ;
			m_datainfo.m_pProjection->MapToGeo(x, y, λ, ψ);
			m_clipper->convertin(λ, ψ);
			m_datainfo.m_pProjection->GeoToMap(λ, ψ, x, y);
		}
		x -= m_datainfo.m_mapOrigin.x;
		y -= m_datainfo.m_mapOrigin.y;
	}
	else if(m_datainfo.IsLngLat() && m_clipper->IsSpinned())
	{
		double λ;
		double ψ;
		m_datainfo.m_pProjection->MapToGeo(x, y, λ, ψ);
		m_clipper->convertin(λ, ψ);
		const CPointF mapPoint = m_pGeocentric->GeoToMap(λ, ψ);
		x = mapPoint.x - m_mapOrigin.x;
		y = mapPoint.y - m_mapOrigin.y;
	}
	else
	{
		CPointF geoPoint = m_datainfo.m_pProjection->MapToGeo(x, y);
		m_clipper->convertin(geoPoint.x, geoPoint.y);
		const CPointF mapPoint2 = m_pGeocentric->GeoToMap(geoPoint);
		x = mapPoint2.x - m_mapOrigin.x;
		y = mapPoint2.y - m_mapOrigin.y;
	}

	x *= m_xFactorMapToView;
	y *= m_yFactorMapToView;
	y = m_sizeView.cy - y;
	return Gdiplus::PointF(x, y);
}
Gdiplus::RectF CViewinfo::DataMapToView(const CRectF& mapRect) const
{
	const Gdiplus::PointF point21 = DataMapToView(mapRect.left, mapRect.top);
	const Gdiplus::PointF point22 = DataMapToView(mapRect.right, mapRect.top);
	const Gdiplus::PointF point23 = DataMapToView(mapRect.left, mapRect.bottom);
	const Gdiplus::PointF point24 = DataMapToView(mapRect.right, mapRect.bottom);
	const double left = min(min(point21.X, point22.X), min(point23.X, point24.X));
	const double top = min(min(point21.Y, point22.Y), min(point23.Y, point24.Y));
	const double right = min(min(point21.X, point22.X), min(point23.X, point24.X));
	const double bottom = min(min(point21.Y, point22.Y), min(point23.Y, point24.Y));
	return Gdiplus::RectF(left, top, right - left, bottom - top);
}

CSize CViewinfo::DocToClient(const CSize& docSize) const
{
	const CPoint point1(0, 0);
	const CPoint point2(docSize.cx, docSize.cy);
	const Gdiplus::Point point3 = this->DocToClient<Gdiplus::Point>(point1);
	const Gdiplus::Point point4 = this->DocToClient<Gdiplus::Point>(point2);
	return CSize(abs(point4.X - point3.X), abs(point4.Y - point3.Y));
}

CPointF CViewinfo::ClientToWGS84(const CPoint& cliPoint) const
{
	if(m_pGeocentric != nullptr)
	{
		const int x = cliPoint.x + m_ptViewPos.x;//view x
		const int y = cliPoint.y + m_ptViewPos.y;//view y

		CPointF mapPoint;
		mapPoint.x = m_mapOrigin.y + x/m_xFactorMapToView;//map x
		mapPoint.y = m_mapOrigin.x + (m_sizeView.cy - y)/m_xFactorMapToView;//map y
		CPointF geoPoint = m_pGeocentric->MapToGeo(mapPoint);
		if(m_clipper != nullptr)
		{
			m_clipper->convertout(geoPoint.x, geoPoint.y);
		}
		return m_pGeocentric->GCS.ThistoWGS84(geoPoint);
	}
	else if(m_datainfo.m_pProjection != nullptr)
	{
		const CPointF mapPoint = ClientToDataMap(cliPoint);
		const CPointF geoPoint = m_datainfo.m_pProjection->MapToGeo(mapPoint);
		return m_datainfo.m_pProjection->GCS.ThistoWGS84(geoPoint);
	}
	else
		return CPointF(cliPoint.x, cliPoint.y);
}

Gdiplus::PointF CViewinfo::WGS84ToClient(const CPointF& wgs84) const
{
	if(m_pGeocentric != nullptr)
	{
		CPointF geoPoint = m_pGeocentric->GCS.WGS84toThis(wgs84);
		if(m_clipper != nullptr)
		{
			m_clipper->convertin(geoPoint.x, geoPoint.y);
		}
		CPointF mapPoint = m_pGeocentric->GeoToMap(geoPoint);
		mapPoint.x -= m_mapOrigin.x;
		mapPoint.y -= m_mapOrigin.y;
		const float x = mapPoint.x*m_xFactorMapToView;
		const float y = m_sizeView.cy - mapPoint.y*m_yFactorMapToView;
		return Gdiplus::PointF(x - m_ptViewPos.x, y - m_ptViewPos.y);
	}
	else if(m_datainfo.m_pProjection != nullptr)
	{
		CPointF geoPoint = m_datainfo.m_pProjection->GCS.WGS84toThis(wgs84);
		if(m_clipper != nullptr)
		{
			m_clipper->convertin(geoPoint.x, geoPoint.y);
		}
		CPointF mapPoint = m_datainfo.m_pProjection->GeoToMap(geoPoint);
		return DataMapToClient<Gdiplus::PointF>(mapPoint);
	}
	else
		return Gdiplus::PointF(wgs84.x, wgs84.y);
}

Gdiplus::PointF CViewinfo::GeoToClient(const CPointF& geoPoint) const
{
	const CPointF mapPoint = m_datainfo.m_pProjection->GeoToMap(geoPoint);
	return DataMapToClient<Gdiplus::PointF>(mapPoint);
}

CPointF CViewinfo::ClientToGeo(const CPoint& cliPoint) const
{
	CPointF mapPoint = this->ClientToDataMap(cliPoint);
	if(m_datainfo.m_pProjection != nullptr)
		return m_datainfo.m_pProjection->MapToGeo(mapPoint);
	else
		return mapPoint;
}

Gdiplus::SizeF CViewinfo::PointsToPixels(float ptsValue, const CSize& dpi)
{
	return Gdiplus::SizeF(ptsValue*dpi.cx/72.f, ptsValue*dpi.cy/72.f);
}
void CViewinfo::LevelTo(CWnd* pWnd, const float& fLevel, const CPointF& mapPoint, bool force)
{
	float level = fLevel;
	if(fLevel < m_levelMinimum)
		level = m_levelMinimum;
	else if(fLevel > m_levelMaximum)
		level = m_levelMaximum;

	if(level != m_levelCurrent || force==true)
	{
		const CSizeF mapCanvas = m_pGeocentric != nullptr ? m_mapCanvas : m_datainfo.m_mapCanvas;
		const CPointF mapOrigin = m_pGeocentric != nullptr ? m_mapOrigin : m_datainfo.m_mapOrigin;
		double maxSquare = max(mapCanvas.cx, mapCanvas.cy);
		double factorMapToView = (pow(2, level)*256)/maxSquare;
		m_sizeView.cx = mapCanvas.cx*factorMapToView;
		m_sizeView.cy = mapCanvas.cy*factorMapToView;
		m_xFactorMapToView = factorMapToView;
		m_yFactorMapToView = factorMapToView;

		const double mapX = mapPoint.x - mapOrigin.x;
		const int x = mapX*factorMapToView;
		const double mapY = mapCanvas.cy - (mapPoint.y - mapOrigin.y);
		const int y = mapY*factorMapToView;
		CViewinfo::Originate(pWnd, CPoint(x, y), CPoint(0, 0));

		if(m_pGeocentric != nullptr)
			m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_pGeocentric, factorMapToView, mapPoint, m_sizeDPI);
		else if(m_datainfo.m_pProjection != nullptr)
			m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_datainfo.m_pProjection, factorMapToView, mapPoint, m_sizeDPI);
		else
			m_scaleCurrent = m_sizeDPI.cx/(factorMapToView*m_sizeDPI.cx);

		m_levelCurrent = level;
	}
}
void CViewinfo::ScaleTo(CWnd* pWnd, const float& fScale, CPointF mapPoint)
{
	float scale = fScale;
	if(fScale < m_datainfo.m_scaleMaximum)
		scale = m_datainfo.m_scaleMaximum;
	else if(fScale > m_datainfo.m_scaleMinimum)
		scale = m_datainfo.m_scaleMinimum;

	if(scale != m_scaleCurrent)
	{
		CClientDC dc(nullptr);
		float factorMapToView;
		if(m_pGeocentric != nullptr && m_datainfo.m_pProjection != nullptr)
		{
			CPointF geoPoint = m_datainfo.m_pProjection->MapToGeo(mapPoint);
			if(m_clipper != nullptr)
			{
				m_clipper->convertin(geoPoint.x, geoPoint.y);
			}
			mapPoint = m_pGeocentric->GeoToMap(geoPoint);
			factorMapToView = CDatainfo::GetMapToViewFactorFromScale(m_pGeocentric, scale, mapPoint, m_sizeDPI);
		}
		else if(m_datainfo.m_pProjection != nullptr)
			factorMapToView = CDatainfo::GetMapToViewFactorFromScale(m_datainfo.m_pProjection, scale, mapPoint, m_sizeDPI);
		else
			factorMapToView = m_sizeDPI.cx/(scale*m_sizeDPI.cx);

		CPoint cliPoint(0, 0);
		if(pWnd != nullptr)
		{
			CRect rect(0, 0, 0, 0);
			pWnd->GetClientRect(rect);
			cliPoint = rect.CenterPoint();
		}
		const CSizeF mapCanvas = m_pGeocentric != nullptr ? m_mapCanvas : m_datainfo.m_mapCanvas;
		const CPointF mapOrigin = m_pGeocentric != nullptr ? m_mapOrigin : m_datainfo.m_mapOrigin;

		m_sizeView.cx = mapCanvas.cx*factorMapToView;
		m_sizeView.cy = mapCanvas.cy*factorMapToView;
		m_xFactorMapToView = factorMapToView;
		m_yFactorMapToView = factorMapToView;

		const double mapX = mapPoint.x - mapOrigin.x;
		const int x = mapX*factorMapToView;
		const double mapY = mapCanvas.cy - (mapPoint.y - mapOrigin.y);
		const int y = mapY*factorMapToView;

		Originate(pWnd, CPoint(x, y), cliPoint);		

		//		m_scaleCurrent = fScale; in order to make the current scale more accurate
		if(scale == m_datainfo.m_scaleMaximum)
			m_scaleCurrent = m_datainfo.m_scaleMaximum;//invoide disppearing when there is a tine fractrion
		else if(scale == m_datainfo.m_scaleMinimum)
			m_scaleCurrent = m_datainfo.m_scaleMinimum;//invoide disppearing when there is a tine fractrion
		else if(m_pGeocentric != nullptr)
			m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_pGeocentric, factorMapToView, mapPoint, m_sizeDPI);
		else if(m_datainfo.m_pProjection != nullptr)
			m_scaleCurrent = CDatainfo::GetScaleFromMapToViewRatio(m_datainfo.m_pProjection, factorMapToView, mapPoint, m_sizeDPI);
		else
			m_scaleCurrent = m_sizeDPI.cx/(factorMapToView*m_sizeDPI.cx);

		m_levelCurrent = log(max(m_sizeView.cx, m_sizeView.cy)/256.f)/log(2.f);
	}
}

void CViewinfo::FactorTo(const float& xFactorMapToView, const float& yFactorMapToView)
{
	const CSizeF mapCanvas = m_datainfo.m_mapCanvas;
	const CPointF mapOrigin = m_datainfo.m_mapOrigin;
	m_sizeView.cx = mapCanvas.cx*xFactorMapToView;
	m_sizeView.cy = mapCanvas.cy*yFactorMapToView;
	m_xFactorMapToView = xFactorMapToView;
	m_yFactorMapToView = yFactorMapToView;
	m_ptViewPos.x = 0;
	m_ptViewPos.x = 0;
}
void CViewinfo::ChangeProjection(std::optional<int> data)
{
	if(data.has_value())
	{
		CGeocentric* pGeocentric = nullptr;
		switch(data.value())
		{
			case 0:
				pGeocentric = nullptr;
				break;
			default:
				const CGeocentric::ProjType* pProjType = (const CGeocentric::ProjType*)data.value();
				const char* ellps = m_datainfo.m_pProjection->GCS.m_Key;
				pGeocentric = new CGeocentric(pProjType->key, ellps);
				break;
		}
		if(pGeocentric != nullptr && m_pGeocentric != nullptr)
		{
			m_pGeocentric->CopyTo(pGeocentric);
		}
		ResetProjection(pGeocentric);
	}
}
void CViewinfo::Recenter(std::optional<double> lng0, std::optional<double> lat0)
{
	if(m_clipper == nullptr)
		return;
	else if(lng0.has_value() || lat0.has_value())
	{
		CPointF center = m_clipper->GetCenter();
		m_clipper->recenter(lng0.has_value() ? lng0.value() : center.x, lat0.has_value() ? lat0.value() : center.y);
	}
}
void CViewinfo::ConfigGeocentric()
{
	if(m_pGeocentric!=nullptr)
	{
		HINSTANCE hInst = ::LoadLibrary(_T("Projection.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);
		CPointF center = m_clipper->GetCenter();
		m_pGeocentric->SetCentral(center.x, center.y);
		CParameterDlg dlg(nullptr, m_pGeocentric);
		if(dlg.DoModal() == IDOK)
		{
			center = m_pGeocentric->GetCenter();
			if(m_clipper != nullptr)
			{
				m_clipper->recenter(center.x, center.y);
			}
		}
		m_pGeocentric->SetCentral(0, 0);

		AfxSetResourceHandle(hInstOld);
		::FreeLibrary(hInst);
	}
}
void CViewinfo::DocToClient(const CPoint* pPoints, const int& nCount, bool closed, std::list<std::pair<unsigned int, Gdiplus::PointF*>>& paths, const float& tolerance, optional<bool>& southpolin) const
{
	if(nCount <= 0)
		return;

	if(m_clipper == nullptr)
	{
		Gdiplus::PointF* points = DocToClient<Gdiplus::PointF>(pPoints, nCount);
		paths.push_back(std::make_pair(nCount, points));
	}
	else
	{
		m_clipper->start(closed);
		bool output = false;
		for(int index = 0; index < nCount; index++)
		{
			const CPointF geoPoint1 = m_datainfo.DocToGeo(pPoints[index]);
			if(output)
			{
				CStringA string;
				string.Format("[%.7f,%.7f],", pPoints[index].x/10000000.f, pPoints[index].y/10000000.f);
				OutputDebugStringA(string);
			}
			m_clipper->Anchor(geoPoint1.x, geoPoint1.y);
		}
		m_clipper->finialize();
		if(closed)
		{
			std::list<Link::Vertex*> polygons = m_clipper->close(southpolin);
			Interpolation interpolation = m_pGeocentric != nullptr ? Interpolation::GreatCircle : Interpolation::Nothing;
			for(std::list<Link::Vertex*>::iterator it = polygons.begin(); it != polygons.end(); ++it)
			{
				Link::Vertex* link = *it;			    
				std::pair<int, Gdiplus::PointF*> pair = CViewinfo::PreviewGeoToClient(link, interpolation, tolerance);
				paths.push_back(pair);
				Link::Dispose(link);
			}
			polygons.clear();
		}
		else
		{
			Interpolation interpolation2 = m_pGeocentric != nullptr ? Interpolation::GreatCircle : Interpolation::Nothing;
			for(std::list<Link::Vertex*>::iterator it = m_clipper->polylines.begin(); it != m_clipper->polylines.end(); ++it)
			{
				Link::Vertex* link = *it;
				int length = Link::GetLength(link);
				std::pair<int, Gdiplus::PointF*> pair = CViewinfo::PreviewGeoToClient(link, interpolation2, tolerance);
				paths.push_back(pair);
				Link::Dispose(link);
			}
			m_clipper->polylines.clear();
		}
	}
};
void CViewinfo::PreviewGeoToClient(const double& lam, const double& phi, float& x, float& y) const
{
	(m_pGeocentric!=nullptr ? m_pGeocentric : m_datainfo.m_pProjection)->GeoToMap(lam, phi, x, y);
	x = (x - m_mapOrigin.x)*m_xFactorMapToView;
	y = (y - m_mapOrigin.y)*m_yFactorMapToView;
	y = m_sizeView.cy - y;

	x -= m_ptViewPos.x;
	y -= m_ptViewPos.y;
}
std::pair<int, Gdiplus::PointF*> CViewinfo::PreviewGeoToClient(Link::Vertex* link, const Interpolation& interpolation, const float& tolerance) const
{
	if(interpolation == Interpolation::Nothing)
	{
		int length = Link::GetLength(link);
		Gdiplus::PointF* points = new Gdiplus::PointF[length];
		Link::Vertex* pointer1 = link;
		Gdiplus::PointF* pointer2 = points;
		while(pointer1!=nullptr)
		{
			(m_pGeocentric!=nullptr ? m_pGeocentric : m_datainfo.m_pProjection)->GeoToMap(pointer1->λ, pointer1->ψ, pointer2->X, pointer2->Y);
			pointer2->X = (pointer2->X - m_mapOrigin.x)*m_xFactorMapToView;
			pointer2->Y = (pointer2->Y - m_mapOrigin.y)*m_yFactorMapToView;
			pointer2->Y = m_sizeView.cy - pointer2->Y;

			pointer2->X -= m_ptViewPos.x;
			pointer2->Y -= m_ptViewPos.y;

			pointer1 = pointer1->next;
			pointer2++;
		}
		return std::pair<int, Gdiplus::PointF*>(length, points);
	}
	else
	{
		std::function<void(const double& λ, const double& ψ, double& x, double& y)> geoto = [&](const double& λ, const double& ψ, double& x, double& y) {
			(m_pGeocentric!=nullptr ? m_pGeocentric : m_datainfo.m_pProjection)->GeoToMap(λ, ψ, x, y);
			};

		std::vector<CPointF> points1;
		Interpolator<CPointF> interpolator(points1, geoto, interpolation, tolerance);
		Link::Vertex* pointer1 = link;
		interpolator.MoveTo(pointer1->λ, pointer1->ψ, false);
		pointer1 = pointer1->next;
		while(pointer1!=nullptr)
		{
			interpolator.LineTo(pointer1->λ, pointer1->ψ);
			pointer1 = pointer1->next;
		}

		Gdiplus::PointF* points2 = new Gdiplus::PointF[points1.size()];
		Gdiplus::PointF* pointer2 = points2;
		for(CPointF& point : points1)
		{
			(m_pGeocentric!=nullptr ? m_pGeocentric : m_datainfo.m_pProjection)->GeoToMap(point.x, point.y, pointer2->X, pointer2->Y);
			pointer2->X = (pointer2->X - m_mapOrigin.x)*m_xFactorMapToView;
			pointer2->Y = (pointer2->Y - m_mapOrigin.y)*m_yFactorMapToView;
			pointer2->Y = m_sizeView.cy - pointer2->Y;

			pointer2->X -= m_ptViewPos.x;
			pointer2->Y -= m_ptViewPos.y;
			pointer2++;
		}
		return std::pair<int, Gdiplus::PointF*>(points1.size(), points2);
	}
}
void CViewinfo::Originate(CWnd* pWnd, CPoint vewPoint, CPoint cliPoint)
{
	if(pWnd != nullptr)
	{
		CRect cliRect;
		pWnd->GetClientRect(&cliRect);
		if(m_sizeView.cx <= cliRect.Width())
			m_ptViewPos.x = (m_sizeView.cx - cliRect.Width())/2;
		else if(vewPoint.x < cliPoint.x)
			m_ptViewPos.x = 0;
		else if(vewPoint.x + (cliRect.Width() - cliPoint.x) > m_sizeView.cx)
			m_ptViewPos.x = m_sizeView.cx - cliRect.Width();
		else
			m_ptViewPos.x = vewPoint.x - cliPoint.x;
		
		if(m_sizeView.cy <= cliRect.Height())
			m_ptViewPos.y = (m_sizeView.cy - cliRect.Height())/2;
		else if(vewPoint.y < cliPoint.y)
			m_ptViewPos.y = 0;
		else if(vewPoint.y + (cliRect.Height() - cliPoint.y) > m_sizeView.cy)
			m_ptViewPos.y = m_sizeView.cy - cliRect.Height();
		else
			m_ptViewPos.y = vewPoint.y - cliPoint.y;
		
		if(pWnd->IsKindOf(RUNTIME_CLASS(CScrollView)))
		{
			((CScrollView*)pWnd)->SetScrollSizes(MM_TEXT, m_sizeView);
			((CScrollView*)pWnd)->SetScrollPos(SB_HORZ, m_ptViewPos.x);
			((CScrollView*)pWnd)->SetScrollPos(SB_VERT, m_ptViewPos.y);
		}
		else if(pWnd != nullptr)
		{
			pWnd->Invalidate(FALSE);
		}
	}
	else
	{
		m_ptViewPos.x = 0;
		m_ptViewPos.y = 0;
	}
}

//none of the solution works for export an template method
//template void CViewinfo::DocToClient<Gdiplus::PointF>(const CPoint* pPoints, const int& nCount, bool closed, std::list<std::pair<unsigned int, Gdiplus::PointF*>>& paths, const Interpolation& interpolation, const float& tolerance) const;
