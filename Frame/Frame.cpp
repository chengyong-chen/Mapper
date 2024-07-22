#include "stdafx.h"
#include "Frame.h"

#include "../DataView/Viewinfo.h"

#include "../Style/ColorSpot.h"
#include "../Style/Library.h"
#include "../Utility/Scale.hpp"
#include "../Pbf/writer.hpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFrame::CFrame()
{
	m_bEnabledSphere = FALSE;
	m_bEnabledGraticule = FALSE;
}

CFrame::~CFrame()
{
}

void CFrame::Draw1(Gdiplus::Graphics& g, const CViewinfo& viewinfo)
{
	if(viewinfo.m_datainfo.m_pProjection == nullptr)
		return;
	if(viewinfo.m_clipper == nullptr)
		return;

	if(m_bEnabledSphere)
	{
		float tolerance = viewinfo.GetPrecision(10);
		if(abs(tolerance - PrevPrecision) > constants::ε1)
		{
			CPointF center = viewinfo.m_clipper->GetCenter();
			viewinfo.m_clipper->recenter(0, 0);
			m_sphere.Reform(viewinfo, tolerance);
			viewinfo.m_clipper->recenter(center.x, center.y);
			PrevPrecision = tolerance;
		}
		{
			CLibrary library;
			Context context;
			context.tolerance = tolerance;
			CPointF center = viewinfo.m_clipper->GetCenter();
			viewinfo.m_clipper->recenter(0, 0);
			m_sphere.Draw1(g, viewinfo, library, context);
			viewinfo.m_clipper->recenter(center.x, center.y);
		}
	}
}
void CFrame::Draw2(Gdiplus::Graphics& g, const CViewinfo& viewinfo)
{
	if(viewinfo.m_datainfo.m_pProjection == nullptr)
		return;
	if(viewinfo.m_clipper == nullptr)
		return;

	CLibrary library;
	Context context;
	context.tolerance = viewinfo.GetPrecision(10);
	if(m_bEnabledSphere)
	{
		CPointF center = viewinfo.m_clipper->GetCenter();
		viewinfo.m_clipper->recenter(0, 0);
		m_sphere.Draw2(g, viewinfo, library, context);
		viewinfo.m_clipper->recenter(center.x, center.y);
	}
	if(m_bEnabledGraticule)
	{	
		m_graticule.Draw(g, viewinfo, library, context);
	}
}

void CFrame::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_bEnabledSphere;
		ar << m_bEnabledGraticule;
	}
	else
	{
		ar >> m_bEnabledSphere;
		ar >> m_bEnabledGraticule;
	}

	if(m_bEnabledSphere)
		m_sphere.Serialize(ar, dwVersion);
	if(m_bEnabledGraticule)
		m_graticule.Serialize(ar, dwVersion);
}
void CFrame::ReleaseWeb(pbf::writer& writer, pbf::tag tag) const
{
	writer.add_tag(tag, pbf::type::bytes);

	if(m_bEnabledSphere)
	{
		writer.add_bool(true);
		m_sphere.ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);

	if(m_bEnabledGraticule)
	{
		writer.add_bool(true);
		m_graticule.ReleaseWeb(writer);
	}
	else
		writer.add_bool(false);
}
