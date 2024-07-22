#pragma once

#include "DataInfo.h"
#include "../Public/Global.h"
#include "../Projection/Antimeridian.h"
#include "../Projection/Geocentric.h"
class CSpot;
class CLine;
class CFillGeneral;
class CType;
class CHint;
class CDatainfo;
namespace projection
{
	class	Clipper;
}
struct Context
{
	CSpot* pSpot;
	CLine* pLine;
	CFillGeneral* pFill;
	CType* pType;
	CHint* pHint;

	Gdiplus::Pen* penStroke;
	Gdiplus::Brush* brushFill;
	Gdiplus::Font* font;
	Gdiplus::Brush* textbrush;

	float ratioMark;
	float ratioLine;
	float ratioFill;
	float ratioType;

	float tolerance;
	Context()
	{
		pSpot = nullptr;
		pLine = nullptr;
		pFill = nullptr;
		pType = nullptr;
		pHint = nullptr;

		penStroke = nullptr;
		brushFill = nullptr;
		font = nullptr;
		textbrush = nullptr;

		ratioMark = 1.0f;
		ratioLine = 1.0f;
		ratioFill = 1.0f;
		ratioType = 1.0f;

		tolerance = INFINITE;
	}
};
class __declspec(dllexport) CViewinfo //abstract : public CMapinfo
{
public:
	CViewinfo(const CDatainfo& foreground);
	~CViewinfo();

public:
	const CDatainfo& m_datainfo;
public:
	CPointF m_mapOrigin;
	CSizeF m_mapCanvas;
	CGeocentric* m_pGeocentric = nullptr;
	projection::Clipper* m_clipper;

private:
	void DetermineLevel();
protected:
	CRectF TransformRange(CGeocentric* pGeocentric) const;

public:
	void ResetProjection(CGeocentric* pGeocentric, bool action = false);
	void ChangeProjection(std::optional<int> data);
	void Recenter(std::optional<double> lng0, std::optional<double> lat0);
	void ConfigGeocentric();
	float GetPrecision(int pixels) const
	{
		return pixels/m_xFactorMapToView;
	}
public:
	CSize m_sizeDPI;
	CSize m_sizeView;
	mutable CPoint m_ptViewPos;
	bool m_bViewer;

public:
	float m_xFactorMapToView;
	float m_yFactorMapToView;//defining this is because of exporting to eps and ai

	float m_scaleCurrent;
	float m_levelCurrent;
	BYTE m_levelMinimum;
	BYTE m_levelMaximum;
public:
	double CurrentRatio() const;

public:
	virtual bool ClientToDoc(const CPoint& cliPoint, CPoint& docPoint) const;
	virtual CSize ClientToDoc(const Gdiplus::SizeF& cliSize) const;
	virtual CPoint ClientToDoc(const CPoint& cliPoint, bool inonly = false) const;
	virtual CRect ClientToDoc(const CRect& cliRect) const;
	virtual CPointF ClientToDataMap(const CPoint& cliPoint) const;
	virtual CPointF ClientToGeo(const CPoint& cliPoint) const;
	virtual Gdiplus::PointF ViewMapToClient(double x, double y) const;

protected:
	virtual Gdiplus::PointF DataMapToView(double x, double y) const;
	virtual Gdiplus::RectF DataMapToView(const CRectF& mapRect) const;
public:
	const CViewinfo& operator =(const CViewinfo& viewinfo);
public:
	template<class T>
	void DataMapToView(const double& x1, const double& y1, T& x2, T& y2) const
	{
		if(m_pGeocentric == nullptr)
		{
			x2 = x1 - m_datainfo.m_mapOrigin.x;
			y2 = y1 - m_datainfo.m_mapOrigin.y;
		}
		else if(m_datainfo.m_pProjection == nullptr)
		{
			x2 = x1 - m_datainfo.m_mapOrigin.x;
			y2 = y1 - m_datainfo.m_mapOrigin.y;
		}
		else
		{
			CPointF geoPoint = m_datainfo.m_pProjection->MapToGeo(x1, y1);
			m_clipper->convertin(geoPoint.x, geoPoint.y);
			const CPointF mapPoint = m_pGeocentric->GeoToMap(geoPoint);
			x2 = mapPoint.x - m_mapOrigin.x;
			y2 = mapPoint.y - m_mapOrigin.y;
		}

		x2 *= m_xFactorMapToView;
		y2 *= m_yFactorMapToView;
		y2 = m_sizeView.cy - y2;
	}

	template<class T>
	T DocToClient(const CPoint& docPoint) const
	{
		const CPointF mapPoint = m_datainfo.DocToMap(docPoint);
		return DataMapToClient<T>(mapPoint);
	};
	template<typename T>
	void DocToClient(const int& xi, const int& yi, T& xo, T& yo) const
	{
		double xm;
		double ym;
		m_datainfo.DocToMap(xi, yi, xm, ym);
		DataMapToClient<T>(xm, ym, xo, yo);
	};
	template<class T>
	T* DocToClient(const CPoint* pPoints, int nCount) const
	{
		if(nCount <= 0)
			return nullptr;

		double dilationDocToMap = m_datainfo.m_dilationDocToMap;
		T* points = new T[nCount];
		T* pointer2 = points;
		for(int index = 0; index < nCount; index++)
		{
			DataMapToView(pPoints->x*dilationDocToMap, pPoints->y*dilationDocToMap, pointer2->X, pointer2->Y);
			pointer2->X -= m_ptViewPos.x;
			pointer2->Y -= m_ptViewPos.y;
			pPoints++;
			pointer2++;
		}
		return points;
	};
	template<class T>
	bool DocToClient_IfVisible(const CPoint& docPoint, T& cliPoint) const
	{
		if(m_clipper != nullptr)
		{
			CPointF geoPoint = m_datainfo.DocToGeo(docPoint);
			m_clipper->convertin(geoPoint.x, geoPoint.y);
			if(m_clipper->IsVisible(geoPoint.x, geoPoint.y))
			{
				PreviewGeoToClient(geoPoint.x, geoPoint.y, cliPoint.X, cliPoint.Y);
				return true;
			}
			else
				return false;
		}
		else
		{
			T point = DocToClient<T>(docPoint);
			cliPoint.X = point.X;
			cliPoint.Y = point.Y;
		}
		return true;
	};
	template<class T>
	T DocToClient(const CRect& docRect) const
	{
		Gdiplus::Point cliPoint1 = this->DocToClient<Gdiplus::Point>(CPoint(docRect.left, docRect.top));
		Gdiplus::Point cliPoint2 = this->DocToClient<Gdiplus::Point>(CPoint(docRect.left, docRect.bottom));
		Gdiplus::Point cliPoint3 = this->DocToClient<Gdiplus::Point>(CPoint(docRect.right, docRect.bottom));
		Gdiplus::Point cliPoint4 = this->DocToClient<Gdiplus::Point>(CPoint(docRect.right, docRect.top));

		T cliRect;
		cliRect.X = min(min(cliPoint1.X, cliPoint2.X), min(cliPoint3.X, cliPoint4.X));
		cliRect.Y = min(min(cliPoint1.Y, cliPoint2.Y), min(cliPoint3.Y, cliPoint4.Y));
		cliRect.Width = max(max(cliPoint1.X, cliPoint2.X), max(cliPoint3.X, cliPoint4.X)) - cliRect.X;
		cliRect.Height = max(max(cliPoint1.Y, cliPoint2.Y), max(cliPoint3.Y, cliPoint4.Y)) - cliRect.Y;
		return cliRect;
	}
	virtual CSize DocToClient(const CSize& docSize) const;

	template<class T>
	T DataMapToClient(const CPointF& mapPoint) const
	{
		const Gdiplus::PointF point = DataMapToView(mapPoint.x, mapPoint.y);
		return T(point.X - m_ptViewPos.x, point.Y - m_ptViewPos.y);
	};
	template<typename T>
	void DataMapToClient(const double& x1, const double& y1, T& x2, T& y2) const
	{
		DataMapToView(x1, y1, x2, y2);
		x2 -= m_ptViewPos.x;
		y2 -= m_ptViewPos.y;
	};
	template<class T>
	T DataMapToClient(const double& x, const double& y) const
	{
		const Gdiplus::PointF point = DataMapToView(x, y);
		return T(point.X - m_ptViewPos.x, point.Y - m_ptViewPos.y);
	};
	Gdiplus::Rect DataMapToClient(const CRectF& mapRect) const
	{
		const Gdiplus::RectF rect = DataMapToView(mapRect);
		return Gdiplus::Rect((int)(rect.X - m_ptViewPos.x), (int)(rect.Y - m_ptViewPos.y), (int)rect.Width, (int)rect.Height);
	};
	void DocToClient(const CPoint* pPoints, const int& nCount, bool closed, std::list<std::pair<unsigned int, Gdiplus::PointF*>>& paths, const float& tolerance, optional<bool>& southpolin) const;
	virtual Gdiplus::PointF GeoToClient(const CPointF& geoPoint) const;
	virtual void PreviewGeoToClient(const double& lam, const double& phi, float& x, float& y) const;
	virtual std::pair<int, Gdiplus::PointF*> PreviewGeoToClient(Link::Vertex* link, const Interpolation& interpolation, const float& tolerance) const;
	virtual Gdiplus::PointF WGS84ToClient(const CPointF& geoPoint) const;
	virtual CPointF ClientToWGS84(const CPoint& cliPoint) const;

public:
	virtual void LevelTo(CWnd* pWnd, const float& fLevel, const CPointF& mapPoint, bool force = false);
	virtual void ScaleTo(CWnd* pWnd, const float& fScale, CPointF mapPoint);
	virtual void FactorTo(const float& xFactorMapToView, const float& yFactorMapToView);
protected:
	void Originate(CWnd* pWnd, CPoint vewPoint, CPoint cliPoint);
public:
	static Gdiplus::SizeF PointsToPixels(float ptsValue, const CSize& dpi);
	static bool PointInPolygon(const CPoint& point, const CPoint* pPoints, const unsigned int& nAnchors)
	{
		long odd = 0;
		for(unsigned int i = 0; i < nAnchors - 1; i++)
		{
			if(point == pPoints[i])
				return true;
			else if(point.x > max(pPoints[i].x, pPoints[i + 1].x))
				continue;
			else if(point.y > max(pPoints[i].y, pPoints[i + 1].y))
				continue;
			else if(point.y < min(pPoints[i].y, pPoints[i + 1].y))
				continue;
			else if(pPoints[i].y == pPoints[i + 1].y)
				continue;
			else if(point.y == pPoints[i].y && pPoints[i].y < pPoints[i + 1].y)//excludes the starting point for a downward edge 
				continue;
			else if(point.y == pPoints[i + 1].y && pPoints[i].y > pPoints[i + 1].y)//excludes the ending point for a upward edge
				continue;
			else
			{
				long xtemp = round(pPoints[i].x + (double)(point.y - pPoints[i].y)*(pPoints[i + 1].x - pPoints[i].x)/(double)(pPoints[i + 1].y - pPoints[i].y));
				if(xtemp == point.x)
					return true;
				else if(xtemp > point.x)
					odd++;
			}
		}
		return odd % 2 == 1 ? true : false;
	};
};

//none of the solution works for export an template method
//template<>
//void CViewinfo::DocToClient<Gdiplus::PointF>(const CPoint* pPoints, const int& nCount, bool closed, std::list<std::pair<unsigned int, Gdiplus::PointF*>>& paths, const Interpolation& interpolation, const float& tolerance) const;