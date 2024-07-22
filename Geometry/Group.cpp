#include "stdafx.h"

#include "Geom.h"
#include "Group.h"

#include "../Style/Line.h"

#include  <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CGroup, CCollection<CGeom>, 0)

CGroup::CGroup()
	: CCollection<CGeom>()
{
	m_bGroup = true;
	m_bClosed = true;
	m_bType = 5;
}
CGroup::CGroup(CLine* pLine, CFillCompact* pFill)
	: CCollection<CGeom>(pLine, pFill)
{
	m_bGroup = true;
	m_bClosed = true;
	m_bType = 5;
}
CGroup::~CGroup()
{
}
void CGroup::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint, const bool& bPivot) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->DrawTag(g, viewinfo, pHint, bPivot);
	}
}
void CGroup::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CHint* pHint) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->DrawTag(g, viewinfo, pHint);
	}
}

void CGroup::DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const bool& labeloid, const bool& geogroid) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->DrawCentroid(pDC, viewinfo, labeloid, geogroid);
	}
}

void CGroup::CreateTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->CreateTag(anchor, hAlign, vAlign);
	}
}
void CGroup::CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect, const int& sizeFont)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->CreateOptimizedTag(OccupiedRect, sizeFont);
	}
}
void CGroup::SetupTag(const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->SetupTag(anchor, hAlign, vAlign);
	}
}
void CGroup::DeleteTag()
{
	CGeom::DeleteTag();

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->DeleteTag();
	}
}
void CGroup::InvalidateTag(CWnd* pWnd, const CViewinfo& viewinfo, const CHint* pHint) const
{
	CGeom::InvalidateTag(pWnd, viewinfo, pHint);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->InvalidateTag(pWnd, viewinfo, pHint);
	}
}
bool CGroup::Tagable() const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->Tagable()==TRUE)
			return true;
	}
	return false;
}


