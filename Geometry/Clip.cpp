#include "stdafx.h"
#include <array>
#include "Clip.h"

#include "../Style/lineSymbol.h"
#include "../Pbf/writer.hpp"

using namespace boost;
#include <boost/json/array.hpp>
#include <boost/json.hpp>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CClip, CCollection<CGeom>, 0)

CClip::CClip()
	: CCollection<CGeom>()
{
	m_bType = 16;
	m_bClosed = true;
}
CClip::CClip(Gdiplus::FillMode mode)
	: CClip()
{
	m_mode = mode;
}
CClip::~CClip()
{
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		delete pPath;
	}
	m_paths.RemoveAll();
}
void CClip::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CCollection<CGeom>::Sha1(sha1);

	sha1.process_byte(m_mode);
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		pPath->Sha1(sha1);
	}
}
void CClip::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CCollection<CGeom>::Sha1(sha1, offset);

	sha1.process_byte(m_mode);
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		pPath->Sha1(sha1, offset);
	}
}
BOOL CClip::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CClip)) == FALSE)
		return FALSE;
	else
		return TRUE;
}
void CClip::RecalRect()
{
	CCollection::RecalRect();

	CRect bounds;
	bounds.SetRectEmpty();
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_paths.GetNext(pos);
		pPath->RecalRect();
		CRect rect = pPath->GetRect();
		bounds = UnionRect(bounds, rect);
	}
	bounds.NormalizeRect();

	if(m_Rect.IsRectEmpty())
		m_Rect = bounds;
	else
		m_Rect.IntersectRect(m_Rect, bounds);
}
void CClip::Move(const int& dx, const int& dy)
{
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_paths.GetNext(pos);
		pPath->Move(dx, dy);
	}

	CCollection::Move(dx, dy);
}
void CClip::Move(const CSize& Δ)
{
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_paths.GetNext(pos);
		pPath->Move(Δ);
	}

	CCollection::Move(Δ);
}
void CClip::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrix, Context& context, const CSize& dpi) const
{
	if(CCollection::m_geomlist.GetCount() > 0)
	{
		Gdiplus::Region oldRegion;
		g.GetClip(&oldRegion);

		Gdiplus::GraphicsPath gpath;
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			pPath->AddPath(gpath, matrix);
		}
		g.SetClip(&gpath, Gdiplus::CombineMode::CombineModeIntersect);

		CCollection::Draw(g, matrix, context, dpi);

		g.SetClip(&oldRegion);
	}
	else if(m_pFill != nullptr)
	{
	}
}

void CClip::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	float tolerance = viewinfo.GetPrecision(10);
	Gdiplus::Region oldRegion;
	g.GetClip(&oldRegion);

	Gdiplus::GraphicsPath gpath;
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		pPath->AddPath(gpath, viewinfo, tolerance);
	}
	g.SetClip(&gpath, Gdiplus::CombineMode::CombineModeIntersect);

	CCollection::Draw(g, viewinfo, ratio);

	g.SetClip(&oldRegion);
}

void CClip::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	Gdiplus::GraphicsPath gpath(Gdiplus::FillMode::FillModeAlternate);
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		pPath->AddPath(gpath, viewinfo, context.tolerance);
	}

	if(CCollection::m_geomlist.GetCount() > 0)
	{
		Gdiplus::Region oldRegion;
		g.GetClip(&oldRegion);
		g.SetClip(&gpath, Gdiplus::CombineMode::CombineModeIntersect);

		CCollection::Draw(g, viewinfo, library, context);

		g.SetClip(&oldRegion);
	}
	else if(m_pFill != nullptr)
	{
		Gdiplus::Brush* brush = m_pFill->GetBrush(context.ratioFill, viewinfo.m_sizeDPI);
		if(brush != nullptr)
		{
			if(brush->GetType() == Gdiplus::BrushTypePathGradient)
			{
				const Gdiplus::PathGradientBrush* local = CGeom::SetGradientBrush((RadialBrush*)brush, viewinfo, &gpath);
				g.FillPath(local, &gpath);
				::delete local;
			}
			else if(brush->GetType() == Gdiplus::BrushTypeLinearGradient)
			{
				const Gdiplus::LinearGradientBrush* local = CGeom::SetGradientBrush((LinearBrush*)brush, viewinfo);
				g.FillPath(local, &gpath);
				::delete local;
			}
			::delete brush;
			brush = nullptr;
		}
	}
}
void CClip::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CCollection::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar << (BYTE)m_mode;
	}
	else
	{
		BYTE mode;
		ar >> mode;
		m_mode = (Gdiplus::FillMode)mode;
	}
	CGeom::Serializelist(ar, dwVersion, m_paths);
}
void CClip::ReleaseWeb(boost::json::object& json) const
{
	CCollection::ReleaseWeb(json);

	boost::json::array paths;
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_paths.GetNext(pos);
		boost::json::object geom;
		pPath->ReleaseWeb(geom);
		paths.push_back(geom);
	}
	json["Paths"] = paths;
}
void CClip::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CCollection::ReleaseWeb(writer, offset);

	writer.add_variant_uint32(m_paths.GetCount());
	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		CPath* pPath = m_paths.GetNext(pos);
		pPath->ReleaseWeb(writer, offset);
	}
}
bool CClip::Letitgo(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	CCollection::UnGroup(geomlist);

	POSITION pos = m_paths.GetHeadPosition();
	while(pos != nullptr)
	{
		const CPath* pPath = m_paths.GetNext(pos);
		delete pPath;
	}
	m_paths.RemoveAll();
	return true;
}


bool CClip::PickIn(const CPoint& point) const
{
	if(CGeom::PickIn(point) == false)
		return false;

	if(CCollection::m_geomlist.GetCount() > 0)
		return CCollection::PickIn(point);
	else if(m_paths.GetCount() > 0)
	{
		long nIn = 0;
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			if(pPath->PickIn(point) == true)
				nIn++;
		}

		return nIn % 2 == 0 ? false : true;
	}
	else
		return CGeom::PickIn(point);
}
bool CClip::PickOn(const CPoint& point, const unsigned long& gap) const
{
	if(CClip::PickIn(point) == true)
		return true;
	else if(m_paths.GetCount() > 0)
	{
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			if(pPath->PickOn(point, gap) == true)
				return true;
		}
		return false;
	}
	else
		return CGeom::PickOn(point, gap);
}
bool CClip::PickIn(const CRect& rect) const
{
	if(CGeom::PickIn(rect) == false)
		return false;

	if(CCollection::m_geomlist.GetCount() > 0)
		return CCollection::PickIn(rect);
	else if(m_paths.GetCount() > 0)
	{
		long nIn = 0;
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			if(pPath->PickIn(rect) == true)
				return true;
		}

		return false;
	}
	else
		return CGeom::PickIn(rect);
}


bool CClip::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CGeom::PickIn(center, radius) == false)
		return false;

	if(CCollection::m_geomlist.GetCount() > 0)
		return CCollection::PickIn(center, radius);
	else if(m_paths.GetCount() > 0)
	{
		long nIn = 0;
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			if(pPath->m_bClosed == false)
				continue;
			if(pPath->PickIn(center, radius) == true)
				return true;
		}

		return false;
	}
	else
		return CGeom::PickIn(center, radius);
}


bool CClip::PickIn(const CPoly& polygon) const
{
	if(CGeom::PickIn(polygon) == false)
		return false;

	if(CCollection::m_geomlist.GetCount() > 0)
		return CCollection::PickIn(polygon);
	else if(m_paths.GetCount() > 0)
	{
		long nIn = 0;
		POSITION pos = m_paths.GetHeadPosition();
		while(pos != nullptr)
		{
			const CPath* pPath = m_paths.GetNext(pos);
			if(pPath->m_bClosed == false)
				continue;
			if(pPath->PickIn(polygon) == true)
				return true;
		}

		return false;
	}
	else
		return CGeom::PickIn(polygon);
}
