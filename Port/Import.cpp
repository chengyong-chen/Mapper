#include "stdafx.h"
#include "Import.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Text.h"
#include "../Geometry/Group.h"

#include "../Style/Spot.h"
#include "../Style/Type.h"
#include "../Style/FillCompact.h"
#include "../Style/Line.h"
#include "../Style/Hint.h"
#include "../layer/Layer.h"
#include "../layer/LayerTree.h"
#include <ogr_spatialref.h>
#include "ImportGdal.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

CImport::CImport(CDatainfo& datainfo, CLayerTree& layertree, DWORD& dwMaxGeomId)
	: m_datainfo(datainfo), m_layertree(layertree), m_dwMaxGeomId(dwMaxGeomId)
{
	m_fXScale = 10000;
	m_fYScale = 10000;
	m_fXOffset = 0;
	m_fYOffset = 0;
}

CImport::~CImport()
{
}

CPoint CImport::Change(const CPointF& Scr) const
{
	const long X = std::lround((Scr.x-m_fXOffset)*m_fXScale);
	const long Y = std::lround((Scr.y-m_fYOffset)*m_fYScale);
	return CPoint(X, Y);
}

CPoint CImport::Change(const CPoint& Scr) const
{
	const long X = std::lround((Scr.x-m_fXOffset)*m_fXScale);
	const long Y = std::lround((Scr.y-m_fYOffset)*m_fYScale);
	return CPoint(X, Y);
}

CPoint CImport::Change(const double& x, const double& y) const
{
	const long X = std::lround((x-m_fXOffset)*m_fXScale);
	const long Y = std::lround((y-m_fYOffset)*m_fYScale);
	return CPoint(X, Y);
}

BOOL CImport::Import(LPCTSTR lpszFile)
{
	CRect rect;
	rect.SetRectEmpty();
	for(CTree<CLayer>::forwiterator it = m_layertree.forwbegin(); it != m_layertree.forwend(); ++it)
	{
		CLayer* layer = *it;
		//	if(layer->GetGeomCount()==1)
		//	{
		//		CGeom* pGeom = layer->m_geomlist.GetHead();
		//		if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup)))
		//		{
		//			CTypedPtrList<CObList, CGeom*> geomlist;
		//			if(pGeom->UnGroup(geomlist))
		//			{
		//				layer->m_geomlist.RemoveAll();
		//				POSITION pos = geomlist.GetHeadPosition();
		//				while(pos != nullptr)
		//				{
		//					CGeom* pGeom2 = geomlist.GetNext(pos);
		//					if(pGeom2==nullptr)
		//						continue;
		//					layer->m_geomlist.AddTail(pGeom2);
		//				}
		//			}
		//		}
		//	}

		rect = UnionRect(rect, layer->GetRect());
	}
	double x1, y1;
	double x2, y2;
	m_datainfo.DocToMap(rect.left, rect.top, x1, y1);
	m_datainfo.DocToMap(rect.right, rect.bottom, x2, y2);

	m_datainfo.m_mapOrigin.x = min(x1, x2);
	m_datainfo.m_mapOrigin.y = min(y1, y2);
	m_datainfo.m_mapCanvas.cx = abs(x2-x1);
	m_datainfo.m_mapCanvas.cy = abs(y2-y1);
	return TRUE;
}

void CImport::CountInStyles(CGeom* pGeom, CValueCounter<CLine>& lines, CValueCounter<CFillCompact>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, CValueCounter<CHint>& hints)
{
	if(pGeom==nullptr)
		return;

	if(pGeom->m_pLine!=nullptr)
		lines.CountIn(pGeom->m_pLine);
	if(pGeom->m_pFill!=nullptr)
		fills.CountIn(pGeom->m_pFill);
	if(pGeom->m_pHint!=nullptr)
		hints.CountIn(pGeom->m_pHint);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE&&((CMark*)pGeom)->m_pSpot!=nullptr)
		spots.CountIn(((CMark*)pGeom)->m_pSpot);
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==TRUE&&((CText*)pGeom)->m_pType!=nullptr)
		types.CountIn(((CText*)pGeom)->m_pType);
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup))==TRUE)
	{
		POSITION pos = ((CGroup*)pGeom)->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* geom = ((CGroup*)pGeom)->m_geomlist.GetNext(pos);
			CountInStyles(geom, lines, fills, spots, types, hints);
		}
	}
}

void CImport::RemoveStyles(CGeom* pGeom, const CLine* pLine, const CFillCompact* pFill, const CSpot* pSpot, const CType* pType, const CHint* pHint)
{
	if(pGeom==nullptr)
		return;

	if(pLine==nullptr)
	{
	}
	else if(pGeom->m_pLine==pLine)
	{
		pGeom->m_pLine = nullptr;
	}
	else if(pGeom->m_pLine!=nullptr&&*pGeom->m_pLine==*pLine)
	{
		delete pGeom->m_pLine;
		pGeom->m_pLine = nullptr;
	}

	if(pFill==nullptr)
	{
	}
	else if(pGeom->m_pFill==pFill)
	{
		pGeom->m_pFill = nullptr;
	}
	else if(pGeom->m_pFill!=nullptr&&*(pGeom->m_pFill)==*pFill)
	{
		delete pGeom->m_pFill;
		pGeom->m_pFill = nullptr;
	}
	if(pSpot==nullptr)
	{
	}
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE&&((CMark*)pGeom)->m_pSpot==pSpot)
	{
		((CMark*)pGeom)->m_pSpot = nullptr;
	}
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CMark))==TRUE&&((CMark*)pGeom)->m_pSpot!=nullptr&&*(((CMark*)pGeom)->m_pSpot)==*pSpot)
	{
		delete ((CMark*)pGeom)->m_pSpot;
		((CMark*)pGeom)->m_pSpot = nullptr;
	}

	if(pType==nullptr)
	{
	}
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==TRUE&&((CText*)pGeom)->m_pType==pType)
	{
		((CText*)pGeom)->m_pType = nullptr;
	}
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CText))==TRUE&&((CText*)pGeom)->m_pType!=nullptr&&*(((CText*)pGeom)->m_pType)==*pType)
	{
		delete ((CText*)pGeom)->m_pType;
		((CText*)pGeom)->m_pType = nullptr;
	}
	if(pHint==nullptr)
	{
	}
	else if(pGeom->m_pHint==pHint)
	{
		pGeom->m_pHint = nullptr;
	}
	else if(pGeom->m_pHint!=nullptr&&*pGeom->m_pHint==*pHint)
	{
		delete pGeom->m_pHint;
		pGeom->m_pHint = nullptr;
	}

	if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup))==TRUE)
	{
		POSITION pos = ((CGroup*)pGeom)->m_geomlist.GetHeadPosition();
		while(pos!=nullptr)
		{
			CGeom* geom = ((CGroup*)pGeom)->m_geomlist.GetNext(pos);
			RemoveStyles(geom, pLine, pFill, pSpot, pType, pHint);
		}
	}
}