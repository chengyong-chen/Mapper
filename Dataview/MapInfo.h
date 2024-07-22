#pragma once

#include "../Public/Global.h"

#include <utility>

using namespace std;;

class CProjection;

namespace Link
{
	struct Vertex;
}
enum Interpolation : BYTE;
class __declspec(dllexport) CMapinfo
{
public:
	CMapinfo();
	virtual ~CMapinfo();

public:
	CPointF m_mapOrigin;
	CSizeF m_mapCanvas;
	CProjection* m_pProjection = nullptr;
public:
	double m_dilationDocToMap;
	double m_dilationMapToDoc;
	bool m_bResamplable;

public:
	virtual CPointF GetMapOrigin() const { return m_mapOrigin; };
	virtual CRectF GetMapRect() const;
public:
	inline bool IsLngLat() const;
public:
	CPoint MapToDoc(const double& x, const double& y) const;
	CPoint MapToDoc(const CPointF& mapPoint) const;
	CSize MapToDoc(const CSizeF& mapSize) const;
	CRect MapToDoc(const CRectF& mapRect) const;
	int MapToDoc(const double& mapDelta) const;
	void DocToMap(const int& xi, const int& yi, double& xo, double& yo) const;
	CPointF DocToMap(const long& x, const long& y) const;
	CSizeF DocToMap(const unsigned long& cx, const unsigned long& cy) const;
	CPointF DocToMap(const CPoint& docPoint) const;
	CSizeF DocToMap(const CSize& docSize) const;
	CRectF DocToMap(const CRect& docRect) const;	
	double DocToMap(const double& docDelta) const;

	CPointF DocToGeo(const CPoint& docPoint) const;
	void DocToGeo(const int& x, const int& y, double& λ, double& ψ) const;
	CPoint GeoToDoc(const double& λ, const double& ψ) const;
	CPoint GeoToDoc(const CPointF& geoPoint) const;

	std::pair<unsigned int, CPoint*> GeoToDoc(Link::Vertex* vertex);
	CPointF DocToWGS84(const CPoint& docPoint) const;
	CPoint WGS84ToDoc(const CPointF& geoPoint) const;

public:
	virtual void ResetProjection(CProjection* pProjection, bool action);
	virtual bool ChangeProjection(bool action);

public:
	double CalLength(CPoint* pPoints, int nAnchors) const;
	double CalArea(CPoint* pPoints, int nAnchors) const;

public:
	static void Detail(CPointF point1, CPointF point2, unsigned int steps, double& minx, double& miny, double& maxx, double& maxy, CProjection* pProjection1, CProjection* pProjection2);
};
