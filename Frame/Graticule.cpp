#include "stdafx.h"
#include "Global.h"
#include "Graticule.h"

#include "../Style/ColorSpot.h"
#include "../Style/Library.h"
#include "../Style/Line.h"
#include "../Utility/Lnglat.hpp"
#include "../Utility/Scale.hpp"
#include "../Utility/Constants.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CGraticule::CGraticule()
	:m_lngInterval(10), m_latInterval(10), m_fWidth(0.1)
{
	m_pColor = new CColorSpot(0, 0, 0, 255);
}
CGraticule::~CGraticule()
{
	if(m_pColor != nullptr)
	{
		delete m_pColor;
		m_pColor = nullptr;
	}
}
void CGraticule::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_fWidth;
		ar << m_lngInterval;
		ar << m_latInterval;

		const BYTE type = m_pColor == nullptr ? 0XFF : m_pColor->Gettype();
		ar << type;
	}
	else
	{
		ar >> m_fWidth;
		ar >> m_lngInterval;
		ar >> m_latInterval;

		BYTE type = 0XFF;
		ar >> type;
		delete m_pColor;
		m_pColor = CColor::Apply(type);
	}
	if(m_pColor != nullptr)
	{
		m_pColor->Serialize(ar, dwVersion);
	}
}
void CGraticule::ReleaseWeb(boost::json::object& json) const
{
}
void CGraticule::ReleaseWeb(pbf::writer& writer) const
{
	if(m_pColor != nullptr)
	{
		writer.add_bool(true);
		writer.add_float(m_fWidth*10);
		m_pColor->ReleaseWeb(writer);
		writer.add_float(m_lngInterval);
		writer.add_float(m_latInterval);
	}
	else
		writer.add_bool(false);

}

void CGraticule::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context)
{
	optional<double> radius = viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GetRadius() : (optional<double>)std::nullopt;
	Gdiplus::Pen pen(m_pColor->GetColor(), m_fWidth);
	for(int lng = -180; lng <= 180; lng += m_lngInterval)
	{
		viewinfo.m_clipper->start(false);
		for(float lat = 90-(lng%90==0 ? 0: m_latInterval); lat >= -90+(lng%90==0 ? 0: m_latInterval); lat -= 2.5)
		{
			double λ = lng*constants::degreeToradian;
			double ψ = lat*constants::degreeToradian;
			viewinfo.m_clipper->Anchor(λ, ψ);
		}
		viewinfo.m_clipper->finialize();
		for(std::list<Link::Vertex*>::iterator it = viewinfo.m_clipper->polylines.begin(); it != viewinfo.m_clipper->polylines.end(); ++it)
		{
			Link::Vertex* link = *it;			
			std::pair<int, CPointF*> pair = viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GeoToMap(link, radius==nullopt ? Interpolation::Linear : Interpolation::GreatCircle, context.tolerance) : viewinfo.m_datainfo.m_pProjection->GeoToMap(link, Interpolation::Linear, context.tolerance);
			Link::Dispose(link);
			Gdiplus::PointF* points = new Gdiplus::PointF[pair.first];
			for(int index = 0; index < pair.first; index++)
			{
				points[index] = viewinfo.ViewMapToClient(pair.second[index].x, pair.second[index].y);
			}
			g.DrawLines(&pen, points, pair.first);
			::delete[] points;
			delete[] pair.second;
		}
		viewinfo.m_clipper->polylines.clear();
	}
	for(int lat = -90 + m_latInterval; lat < 90; lat += m_latInterval)
	{
		viewinfo.m_clipper->start(false);
		for(float lng = 180; lng >= -180; lng -= 2.5)
		{
			double λ = lng*constants::degreeToradian;
			double ψ = lat*constants::degreeToradian;
			viewinfo.m_clipper->Anchor(λ, ψ);
		}
		viewinfo.m_clipper->finialize();
		for(std::list<Link::Vertex*>::iterator it = viewinfo.m_clipper->polylines.begin(); it != viewinfo.m_clipper->polylines.end(); ++it)
		{			
			Link::Vertex* link = *it;
			std::pair<int, CPointF*> pair = viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GeoToMap(link, radius==nullopt ? Interpolation::Linear : Interpolation::GreatCircle, context.tolerance) : viewinfo.m_datainfo.m_pProjection->GeoToMap(link, Interpolation::Linear, context.tolerance);
			Link::Dispose(link);
			
			Gdiplus::PointF* points = new Gdiplus::PointF[pair.first];
			for(int index = 0; index < pair.first; index++)
			{
				points[index] = viewinfo.ViewMapToClient(pair.second[index].x, pair.second[index].y);
			}
			g.DrawLines(&pen, points, pair.first);
			::delete[] points;
			delete[] pair.second;
		}
		viewinfo.m_clipper->polylines.clear();
	}
}
