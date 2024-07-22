#include "stdafx.h"
#include "Sphere.h"
#include "Global.h"

#include "../Geometry/Poly.h"
#include "../Style/ColorSpot.h"
#include "../Style/Library.h"
#include "../Style/Line.h"
#include "../Style/FillAlone.h"

#include "../Utility/Lnglat.hpp"
#include "../Utility/Scale.hpp"
#include "../Utility/Constants.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSphere::CSphere()
	:m_fWidth(0.5), m_pColor2(nullptr)
{
	m_pColor1 = new CColorSpot(0, 0, 0, 255);
}
CSphere::~CSphere()
{
	if(m_pColor1 != nullptr)
	{
		delete	m_pColor1;
		m_pColor1 = nullptr;
	}
	if(m_pColor2 != nullptr)
	{
		delete m_pColor2;
		m_pColor2 = nullptr;
	}
}
void CSphere::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_fWidth;
		const BYTE type1 = m_pColor1 == nullptr ? 0XFF : m_pColor1->Gettype();
		ar << type1;
		const BYTE type2 = m_pColor2 == nullptr ? 0XFF : m_pColor2->Gettype();
		ar << type2;
	}
	else
	{
		ar >> m_fWidth;
		BYTE type1 = 0XFF;
		ar >> type1;
		delete m_pColor1;
		m_pColor1 = CColor::Apply(type1);

		BYTE type2 = 0XFF;
		ar >> type2;
		delete m_pColor2;
		m_pColor2 = CColor::Apply(type2);
	}
	if(m_pColor1 != nullptr)
	{
		m_pColor1->Serialize(ar, dwVersion);
	}
	if(m_pColor2 != nullptr)
	{
		m_pColor2->Serialize(ar, dwVersion);
	}
}
void CSphere::ReleaseWeb(boost::json::object& json) const
{
}
void CSphere::ReleaseWeb(pbf::writer& writer) const
{
	if(m_pColor1 != nullptr)
	{
		writer.add_bool(true);
		writer.add_float(m_fWidth*10);
		m_pColor1->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);

	if(m_pColor2 != nullptr)
	{
		writer.add_bool(true);
		m_pColor2->ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);
}
void CSphere::Reform(const CViewinfo& viewinfo, float tolerance)
{
	samples.clear();

	std::function<void(const double& λ, const double& ψ, double& x, double& y)> geoto = [&](const double& λ, const double& ψ, double& x, double& y) {
		viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GeoToMap(λ, ψ, x, y) : viewinfo.m_datainfo.m_pProjection->GeoToMap(λ, ψ, x, y);
		};

	Link::Vertex* link = viewinfo.m_clipper->Rounding();
	Link::Vertex* pointer = link;
	Interpolator<CPointF> interpolator(samples, geoto, viewinfo.m_pGeocentric != nullptr ? Interpolation::GreatCircle : Interpolation::Nothing, tolerance);
	interpolator.MoveTo(pointer->λ, pointer->ψ, false);
	pointer = pointer->next;
	while(pointer!=nullptr)
	{
		interpolator.LineTo(pointer->λ, pointer->ψ);
		pointer = pointer->next;
	}
	Link::Dispose(link);
}

void CSphere::Draw1(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CLibrary& library, Context& context)
{
	if(m_pColor2!=nullptr)
	{
		int anchors = samples.size();
		Gdiplus::PointF* points = new Gdiplus::PointF[anchors];
		Gdiplus::PointF* pointer = points;
		for(CPointF& point : samples)
		{
			viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GeoToMap(point.x, point.y, pointer->X, pointer->Y) : viewinfo.m_datainfo.m_pProjection->GeoToMap(point.x, point.y, pointer->X, pointer->Y);
			*pointer = viewinfo.ViewMapToClient(pointer->X, pointer->Y);
			pointer++;
		}
		Gdiplus::SolidBrush brush(m_pColor2->GetColor());
		g.FillPolygon(&brush, points, anchors);
		::delete[] points;
	}
}
void CSphere::Draw2(Gdiplus::Graphics& g, const CViewinfo& viewinfo, CLibrary& library, Context& context)
{
	if(m_pColor1 != nullptr)
	{
		int anchors = samples.size();
		Gdiplus::PointF* points = new Gdiplus::PointF[anchors];
		Gdiplus::PointF* pointer = points;
		for(CPointF& point : samples)
		{
			viewinfo.m_pGeocentric != nullptr ? viewinfo.m_pGeocentric->GeoToMap(point.x, point.y, pointer->X, pointer->Y) : viewinfo.m_datainfo.m_pProjection->GeoToMap(point.x, point.y, pointer->X, pointer->Y);
			*pointer = viewinfo.ViewMapToClient(pointer->X, pointer->Y);
			pointer++;
		}
		Gdiplus::Pen pen(m_pColor1->GetColor(), m_fWidth);
		g.DrawPolygon(&pen, points, anchors);
		::delete[] points;
	}
}