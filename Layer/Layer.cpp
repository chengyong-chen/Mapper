#include "stdafx.h"
#include <array>
#include "Global.h"
#include "Layer.h"
#include "LayerTree.h"
#include "Laypt.h"
#include "Laypy.h"
#include "Laypp.h"
#include "Laylg.h"
#include "Layif.h"
#include "LayerAttrDlg.h"
#include "Holder.h"

#include "../Public/Global.h"
#include "../Public/ODBCDatabase.h"
#include "../Database/ODBCRecordset.h"
#include "../Geometry/DynamicTag.h"
#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Bezier.h"
#include "../Geometry/Text.h"
#include "../Geometry/TextPoly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Imager.h"
#include "../Geometry/Doughnut.h"
#include "../Geometry/TextEditDlg.h"
#include "../Geometry/Group.h"
#include "../Geometry/Clip.h"
#include "../Geometry/Mask.h"
#include "../Geometry/Tag.h"
#include "../Geometry/VectorTile.h"
//
#include "../Style/Spot.h"
#include "../Style/Type.h"
#include "../Style/Hint.h"
#include "../Style/Line.h"
#include "../Style/LineSymbol.h"
#include "../Style/LineRiver.h"
#include "../Style/Fill.h"
#include "../Style/FillSymbol.h"
#include "../Style/FillPattern.h"
#include "../Style/SpotSymbol.h"
#include "../Style/SpotPattern.h"
#include "../Style/SpotFlash.h"
#include "../Style/ColorSpot.h"

#include "../Dataview/Global.h"
//
#include "../Mapper/Global.h"

#include "../Thematics/Theme.h"
#include "../Thematics/Choropleth.h"
#include "../Database/Datasource.h"

#include "../Pbf/writer.hpp"

#include "../Action/Append.h"
#include "../Action/Remove.h"
#include "../Action/Modify.h"
#include "../Action/Swap.h"
#include "../Action/Document/LayerTree/Layerlist/Layer.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Collection.h"
#include "../Action/Document/LayerTree/Layerlist/Geomlist/Poly.h"
#include "../Utility/array.hpp"

#include "../../ThirdParty/clipper/2.0/CPP/Clipper2Lib/include/clipper2/clipper.h"

#include <boost/json/array.hpp>
#include <boost/json.hpp>


using namespace std;
using namespace Undoredo;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern __declspec(dllimport) CArray<Gdiplus::RectF, Gdiplus::RectF&> d_oRectArray;
CLayer::CLayer(CTree<CLayer>& tree)
	: m_tree((CLayerTree&)tree)
{
	m_minLevelObj = 0;
	m_maxLevelObj = 20;
	m_minLevelTag = 0;
	m_maxLevelTag = 20;
}
CLayer::CLayer(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: m_tree((CLayerTree&)tree)
{
	m_minLevelObj = minLevelObj;
	m_maxLevelObj = maxLevelObj;
	m_minLevelTag = minLevelTag;
	m_maxLevelTag = maxLevelTag;
}
CLayer::CLayer(CTree<CLayer>& tree, CLayer* pParent, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	:CTreeNode(pParent), m_tree((CLayerTree&)tree)
{
	m_minLevelObj = minLevelObj;
	m_maxLevelObj = maxLevelObj;
	m_minLevelTag = minLevelTag;
	m_maxLevelTag = maxLevelTag;
}

CLayer::~CLayer()
{
	delete m_attDatasource;
	m_attDatasource = nullptr;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetPrev(pos);
		delete pGeom;
	}
	m_geomlist.RemoveAll();

	POSITION pos1 = m_groupTaglist.GetHeadPosition();
	while(pos1 != nullptr)
	{
		const CGroupTag* pTag = m_groupTaglist.GetNext(pos1);
		delete pTag;
	}
	m_groupTaglist.RemoveAll();

	POSITION pos2 = m_pointTaglist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		const CPointTag* pTag = m_pointTaglist.GetNext(pos2);
		delete pTag;
	}
	m_pointTaglist.RemoveAll();

	POSITION Pos3 = m_fixedTaglist.GetHeadPosition();
	while(Pos3 != nullptr)
	{
		const CFixedTag* pTag = m_fixedTaglist.GetNext(Pos3);
		delete pTag;
	}
	m_fixedTaglist.RemoveAll();

	POSITION Pos4 = m_rectTaglist.GetHeadPosition();
	while(Pos4 != nullptr)
	{
		const CRectTag* pTag = m_rectTaglist.GetNext(Pos4);
		delete pTag;
	}
	m_rectTaglist.RemoveAll();

	delete m_pSpot;
	delete m_pLine;
	delete m_pFill;
	delete m_pType;
	delete m_pHint;
	delete m_pThem;

	CLayer* pChild = this->FirstChild();
	while(pChild != nullptr)
	{
		const CLayer* layer = pChild;
		pChild = pChild->Nextsibling();
		delete layer;
	}
	m_firstchild = nullptr;
	m_parent = nullptr;
}

CLayer* CLayer::Apply(CLayerTree& tree, BYTE type)
{
	switch(type)
	{
		case 0:
		case 1:
			return new CLayer(tree);
		case 6:
			return new CLaypt(tree);
		case 7:
			return new CLaypy(tree);
		case 8:
			return new CLaypp(tree);
		case 9:
			return new CLaylg(tree);
		case 10:
			return new CLayif(tree);
		default:
			return nullptr;
	}
}

CSpot* CLayer::GetSpot() const
{
	if(m_pSpot != nullptr)
		return m_pSpot;
	else if(m_parent != nullptr)
		return m_parent->GetSpot();
	else
		return nullptr;
}

CLine* CLayer::GetLine() const
{
	if(m_pLine != nullptr)
		return m_pLine;
	else if(m_parent != nullptr)
		return m_parent->GetLine();
	else
		return nullptr;
}

CFillGeneral* CLayer::GetFill() const
{
	if(m_pFill != nullptr)
		return m_pFill;
	else if(m_parent != nullptr)
		return m_parent->GetFill();
	else
		return nullptr;
}

CType* CLayer::GetType() const
{
	if(m_pType != nullptr)
		return m_pType;
	else if(m_parent != nullptr)
		return m_parent->GetType();
	else
		return CType::Default();
}

CHint* CLayer::GetHint() const
{
	if(m_pHint != nullptr)
		return m_pHint;
	else if(m_parent != nullptr)
		return m_parent->GetHint();
	else
		return CHint::Default();
}

CTheme* CLayer::GetTheme() const
{
	if(m_pThem != nullptr)
		return m_pThem;
	else if(m_parent != nullptr)
		return m_parent->GetTheme();
	else
		return nullptr;
}
void CLayer::EnableAttribute(CString database)
{
	if(m_attDatasource == nullptr)
		m_attDatasource = new CATTDatasource(database);
	else
		m_attDatasource->m_strDatabase = database;

	m_bAttribute = true;
}
void CLayer::DisableAttribute()
{
	if(m_attDatasource != nullptr)
	{
		delete	m_attDatasource;
		m_attDatasource = nullptr;
	}
	m_bAttribute = false;
}
void CLayer::Purify() const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Purify();
	}
}
void CLayer::CleanOrphan()
{
	POSITION pos1 = m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos1);
		pGeom->CleanOrphan();
		if(pGeom->IsOrphan())
		{
			delete pGeom;
			m_geomlist.RemoveAt(pos2);
		}
	}
}
void CLayer::Wipeout()
{
	delete m_attDatasource;
	m_attDatasource = nullptr;
	m_bAttribute = false;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetPrev(pos);
		delete pGeom;
	}
	m_geomlist.RemoveAll();

	POSITION pos1 = m_groupTaglist.GetHeadPosition();
	while(pos1 != nullptr)
	{
		const CGroupTag* pTag = m_groupTaglist.GetNext(pos1);
		delete pTag;
	}
	m_groupTaglist.RemoveAll();

	POSITION pos2 = m_pointTaglist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		const CPointTag* pTag = m_pointTaglist.GetNext(pos2);
		delete pTag;
	}
	m_pointTaglist.RemoveAll();

	POSITION Pos3 = m_fixedTaglist.GetHeadPosition();
	while(Pos3 != nullptr)
	{
		const CFixedTag* pTag = m_fixedTaglist.GetNext(Pos3);
		delete pTag;
	}
	m_fixedTaglist.RemoveAll();

	POSITION Pos4 = m_rectTaglist.GetHeadPosition();
	while(Pos4 != nullptr)
	{
		const CRectTag* pTag = m_rectTaglist.GetNext(Pos4);
		delete pTag;
	}
	m_rectTaglist.RemoveAll();

	m_nActiveCount = 0;
}
void CLayer::Normalize()
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		pGeom->Normalize();
	}
}
CATTDatasource* CLayer::GetAttDatasource() const
{
	if(m_bAttribute == false)
		return nullptr;
	else if(m_attDatasource != nullptr)
		return m_attDatasource;
	else if(m_parent != nullptr)
		return m_parent->GetAttDatasource();
	else
		return nullptr;
}

Gdiplus::SizeF CLayer::GetVewInflation(const CViewinfo& viewinfo, const CGeom* pGeom) const
{
	CLine* pLine = this->GetLine();
	CType* pType = this->GetType();
	const float ratioLine = (m_bDynamic & LineDynamic) == LineDynamic ? viewinfo.CurrentRatio() : 1.0f;
	const float ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
	const float ptsInflation = pGeom->GetInflation(pLine, pType, ratioLine, ratioType);
	if(ptsInflation != 0.f)
		return CViewinfo::PointsToPixels(ptsInflation, viewinfo.m_sizeDPI) + Gdiplus::SizeF(3, 3);
	else
		return Gdiplus::SizeF(3, 3);
}

inline CSize CLayer::GetDocInflation(const CViewinfo& viewinfo, const CGeom* pGeom) const
{
	switch(pGeom->m_bType)
	{
		case 8:
		case 9:
		case 16:
		case 17:
		case 18:
		case 41:
			return 0;
		case 10:
			{
				if(((CTextPoly*)pGeom)->m_pType != nullptr)
				{
					const Gdiplus::SizeF inflation = this->GetVewInflation(viewinfo, pGeom);
					return viewinfo.ClientToDoc(inflation);
				}
				else
					return m_tmpTypeInflation;
			}
		default:
			if(pGeom->m_pLine != nullptr)
			{
				const Gdiplus::SizeF inflation = this->GetVewInflation(viewinfo, pGeom);
				return viewinfo.ClientToDoc(inflation);
			}
			else
				return m_tmpLineInflation;
	}
}

CRect CLayer::GetGeomDocBoundary(const CViewinfo& viewinfo, const CGeom* pGeom) const
{
	switch(pGeom->m_bType)
	{
		case 8:
			{
				if((m_bDynamic & SpotDynamic) != SpotDynamic)
				{
					const double ratio = viewinfo.CurrentRatio();
					const CMark* mark = (CMark*)pGeom;
					CSpot* pSpot = mark->m_pSpot != nullptr ? mark->m_pSpot : this->GetSpot();
					if(pSpot != nullptr)
					{
						CRect rect = pSpot->GetDocRect(viewinfo.m_datainfo, m_tree.m_library);
						rect.left = (long)(rect.left / ratio);
						rect.top = (long)(rect.top / ratio);
						rect.right = (long)(rect.right / ratio);
						rect.bottom = (long)(rect.bottom / ratio);
						rect.OffsetRect(mark->m_Origin.x, mark->m_Origin.y);
						return rect;
					}
				}
			}
			break;
		case 9:
			if((m_bDynamic & TypeDynamic) != TypeDynamic)
			{
				const double ratio = viewinfo.CurrentRatio();
				const double fPointToDoc = viewinfo.m_datainfo.m_zoomPointToDoc;
				CText* text = (CText*)pGeom;
				CType* pType = this->GetType();
				text->CalCorner(pType, fPointToDoc, ratio);
				return pGeom->m_Rect;
			}
			break;
		case 3:
			break;
		default:
			{
				const CSize gap = this->GetDocInflation(viewinfo, pGeom);
				CRect rect = pGeom->m_Rect;
				rect.InflateRect(gap.cx, gap.cy);
				return rect;
			}
			break;
	}

	return pGeom->m_Rect;
}

int CLayer::GetGeomCount() const
{
	return m_geomlist.GetCount();
}

CGeom* CLayer::GetGeom(const DWORD& dwId) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_geoId == dwId)
		{
			return pGeom;
		}
	}

	return nullptr;
}

CRect CLayer::GetRect() const
{
	CRect rect;
	rect.SetRectEmpty();

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetNext(pos);
		rect = UnionRect(rect, pGeom->m_Rect);
	}

	return rect;
}

CRect CLayer::GetActiveRect() const
{
	CRect rect;
	rect.SetRectEmpty();
	if(m_nActiveCount == 0)
		return rect;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == false)
			continue;

		rect = UnionRect(rect, pGeom->m_Rect);
	}

	return rect;
}

Gdiplus::Rect CLayer::GetActiveArea(const CViewinfo& viewinfo) const
{
	Gdiplus::Rect area(0, 0, 0, 0);
	if(m_nActiveCount == 0)
		return area;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == false)
			continue;

		Gdiplus::Rect rect = viewinfo.DocToClient <Gdiplus::Rect>(pGeom->m_Rect);
		const Gdiplus::SizeF size = this->GetVewInflation(viewinfo, pGeom);
		rect.Inflate(size.Width, size.Height);

		area = UnionRect(area, rect);
	}

	return area;
}

std::list<CGeom*> CLayer::GetActivatedGeoms() const
{
	std::list<CGeom*> result;
	if(m_nActiveCount == 0)
		return result;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		result.push_back(pGeom);
	}
	return result;
}
void CLayer::CountActiveStyles(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types) const
{
	if(m_nActiveCount == 0)
		return;
	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFillGeneral* pFill = this->GetFill();
	CType* pType = this->GetType();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		pGeom->CountStyle(lines, fills, spots, types, pLine, pFill, pSpot, pType);
	}
}

void CLayer::ActivateAll(CWnd* pWnd, const ACTIVEALL& activeall, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const
{
	if(m_bVisible == false)
	{
		InactivateAll(pWnd, datainfo, viewinfo, inRect);
		return;
	}

	CClientDC dc(pWnd);

	CLine* pLine = this->GetLine();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(CanActivateGeom(pGeom) == false)
			continue;

		bool bAligble = false;
		switch(activeall)
		{
			case ACTIVEALL::All:
				bAligble = true;
				break;
			case ACTIVEALL::Mark:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CMark));
				break;
			case ACTIVEALL::Line:
				bAligble = pGeom->m_bClosed == false && (pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) || pGeom->IsKindOf(RUNTIME_CLASS(CPoly)));
				break;
			case ACTIVEALL::Area:
				bAligble = pGeom->m_bClosed == true && (pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == FALSE && pGeom->IsKindOf(RUNTIME_CLASS(CText)) == FALSE);
				break;
			case ACTIVEALL::Text:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CText)) || pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly));
				break;
			case ACTIVEALL::PolyText:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly));
				break;
			case ACTIVEALL::Image:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CImager));
				break;
			case ACTIVEALL::Group:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CGroup));
				break;
			case ACTIVEALL::Doughnut:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut));
				break;
			case ACTIVEALL::Clip:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CClip));
				break;
			case ACTIVEALL::Mask:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CMask));
				break;
			case ACTIVEALL::HasName:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CText)) == FALSE && pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == FALSE && pGeom->m_strName.IsEmpty() == FALSE;
				break;
			case ACTIVEALL::NoName:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CText)) == FALSE && pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == FALSE && pGeom->m_strName.IsEmpty() == TRUE;
				break;
			case ACTIVEALL::Rebel:
				bAligble = pGeom->IsDissident();
				break;
			case ACTIVEALL::Bezier:
				bAligble = pGeom->IsKindOf(RUNTIME_CLASS(CBezier));
				break;
		}

		if(bAligble == true && pGeom->m_bActive == false)
		{
			pGeom->m_bActive = true;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
			}

			GeomActivated(pGeom);
			m_nActiveCount++;
		}
		else if(pGeom->m_bActive == true)
		{
			pGeom->m_bActive = false;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
			}

			m_nActiveCount--;
		}
	}
}
void CLayer::ActivateSameStyle(CWnd* pWnd, const ACTIVEALL& activeall, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const CLine* pLine, const CFillGeneral* pFill, const CSpot* pSpot, const CType* pType) const
{
	InactivateAll(pWnd, datainfo, viewinfo, inRect);

	if(m_bVisible == false)
		return;
	else if(activeall == ACTIVEALL::Stroke && pLine == nullptr)
		return;
	else if(activeall == ACTIVEALL::Fill && pFill == nullptr)
		return;
	else if(activeall == ACTIVEALL::Style && (pLine == nullptr || pFill == nullptr))
		return;
	else if(activeall == ACTIVEALL::Type && pType == nullptr)
		return;
	else if(activeall == ACTIVEALL::Spot && pSpot == nullptr)
		return;

	CSpot* pSpotGeneral = this->GetSpot();
	CLine* pLineGeneral = this->GetLine();
	CFill* pFillGeneral = this->GetFill();
	CType* pTypeGeneral = this->GetType();

	CClientDC dc(pWnd);
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == true)
			continue;
		if(CanActivateGeom(pGeom) == false)
			continue;

		if(pGeom->HasSameStyle(activeall, pLine, pFill, pSpot, pType, pLineGeneral, (CFillGeneral*)pFillGeneral, pSpotGeneral, pTypeGeneral) == true)
		{
			pGeom->m_bActive = true;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
			}

			GeomActivated(pGeom);
			m_nActiveCount++;
		}
	}
}
void CLayer::ActivateSameStyle(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const unsigned int target[5]) const
{
	InactivateAll(pWnd, datainfo, viewinfo, inRect);
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CSpot* pSpot = this->GetSpot();
	CType* pType = this->GetType();

	CClientDC dc(pWnd);
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(CanActivateGeom(pGeom) == false)
			continue;

		CPoint origin = pGeom->GetOrigin();
		boost::uuids::detail::sha1 sha1;
		pGeom->Sha1(sha1, pLine, pFill, pSpot, pType);
		unsigned int hash[5] = {0};
		sha1.get_digest(hash);
		if(arr::equal(hash, target) == true)
		{
			pGeom->m_bActive = true;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
			}

			GeomActivated(pGeom);
			m_nActiveCount++;
		}
	}
}
void CLayer::ActivateIdentical(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, const unsigned int target1[5], const unsigned int target2[5]) const
{
	InactivateAll(pWnd, datainfo, viewinfo, inRect);
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CSpot* pSpot = this->GetSpot();
	CType* pType = this->GetType();

	CClientDC dc(pWnd);
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(CanActivateGeom(pGeom) == false)
			continue;

		boost::uuids::detail::sha1 sha11;
		pGeom->Sha1(sha11, pLine, pFill, pSpot, pType);
		unsigned int hash1[5] = {0};
		sha11.get_digest(hash1);

		CPoint origin = pGeom->GetOrigin();
		boost::uuids::detail::sha1 sha12;
		pGeom->Sha1(sha12, CSize(origin.x, origin.y));
		unsigned int hash2[5] = {0};
		sha12.get_digest(hash2);

		if(arr::equal(hash1, target1) == true && arr::equal(hash2, target2) == true)
		{
			pGeom->m_bActive = true;
			if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
			}

			GeomActivated(pGeom);
			m_nActiveCount++;
		}
	}
}
void CLayer::ActivateInverse(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const
{
	if(m_bVisible == false)
	{
		InactivateAll(pWnd, datainfo, viewinfo, inRect);
	}
	else
	{
		CClientDC dc(pWnd);

		CLine* pLine = this->GetLine();
		m_nActiveCount = 0;
		POSITION pos = m_geomlist.GetTailPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetPrev(pos);
			if(pGeom->m_bActive == true)
			{
				PreInactivateGeom(datainfo, pGeom);

				pGeom->m_bActive = false;

				if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
				{
					pGeom->DrawAnchors(&dc, viewinfo);
				}
			}
			else if(CanActivateGeom(pGeom) == true)
			{
				pGeom->m_bActive = true;

				if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
				{
					pGeom->DrawAnchors(&dc, viewinfo);
				}
				GeomActivated(pGeom);

				m_nActiveCount++;
			}
		}
	}
}

void CLayer::ActivateAll(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect, CString& strKey) const
{
	InactivateAll(pWnd, datainfo, viewinfo, inRect);

	if(m_bVisible == false)
		return;

	CClientDC dc(pWnd);

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_strName.IsEmpty() == TRUE)
			continue;
		if(pGeom->m_strName.Find(strKey) == -1)
			continue;
		if(pGeom->m_bActive == true)
			continue;
		if(CanActivateGeom(pGeom) == false)
			continue;

		pGeom->m_bActive = true;
		if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
		}
		m_nActiveCount++;
		GeomActivated(pGeom);
	}
}

void CLayer::InactivateAll(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CRect& inRect) const
{
	CClientDC dc(pWnd);

	CLine* pLine = this->GetLine();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		PreInactivateGeom(datainfo, pGeom);

		pGeom->m_bActive = false;

		if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
		}
	}

	m_nActiveCount = 0;
}

CGeom* CLayer::Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& docPoint, const bool& bEnlock) const
{
	if(m_bVisible == false)
		return nullptr;
	const bool bVisible = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bVisible == false)
		return nullptr;

	CFill* pFill = this->GetFill();

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(bEnlock == false && pGeom->m_bLocked == true)
			continue;

		CRect rect = pGeom->m_Rect;
		const CSize gap = this->GetDocInflation(viewinfo, pGeom);
		rect.InflateRect(gap);
		if(rect.PtInRect(docPoint) == FALSE)
			continue;

		const bool bIn = pGeom->IsArea() || (pGeom->m_bClosed && (pFill != nullptr && pFill->Gettype() != CFill::FILLTYPE::Blank) || (pGeom->m_pFill != nullptr && pGeom->m_pFill->Gettype() != CFill::FILLTYPE::Blank));// if this is used to determine wether clicked in or not it is very confusing for user
		if(pGeom->PickOn(docPoint, gap.cx) || (bIn && pGeom->PickIn(docPoint)))
		{
			if(pGeom->m_bActive == true)
			{
				if(::GetKeyState(VK_CONTROL) < 0)
					continue;

				if(::GetKeyState(VK_SHIFT) < 0)
				{
					PreInactivateGeom(viewinfo.m_datainfo, pGeom);

					CClientDC dc(pWnd);

					pGeom->m_bActive = false;
					m_nActiveCount--;
					pGeom->DrawAnchors(&dc, viewinfo);
					return nullptr;
				}
			}
			return pGeom;
		}
	}

	return nullptr;
}
inline bool CLayer::IsVisible(float levelCurrent) const
{
	if(m_bVisible == false)
		return false;
	else if(m_bShowGeom == false)
		return false;
	else if(levelCurrent < m_minLevelObj)
		return false;
	else if(levelCurrent >= m_maxLevelObj)
		return false;
	else
		return true;
}
void CLayer::Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CRect& rect, const bool& bEnlock) const
{
	if(m_bVisible == false)
		return;
	const bool bVisible = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bVisible == false)
		return;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == true)
			continue;
		else if(bEnlock == false && pGeom->m_bLocked == true)
			continue;
		else if(pGeom->PickIn(rect) == false)
			continue;
		else if(CanActivateGeom(pGeom) == false)
			continue;
		else
		{
			CClientDC dc(pWnd);

			pGeom->m_bActive = true;
			pGeom->DrawAnchors(&dc, viewinfo);

			m_nActiveCount++;
			GeomActivated(pGeom);
		}
	}
}

void CLayer::Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& center, const unsigned long& radius, const bool& bEnlock) const
{
	if(m_bVisible == false)
		return;
	const bool bVisible = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bVisible == false)
		return;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == true)
			continue;
		if(bEnlock == false && pGeom->m_bLocked == true)
			continue;
		if(pGeom->PickIn(center, radius) == false)
			continue;
		if(CanActivateGeom(pGeom) == false)
			continue;

		CClientDC dc(pWnd);

		pGeom->m_bActive = true;
		pGeom->DrawAnchors(&dc, viewinfo);

		m_nActiveCount++;
		GeomActivated(pGeom);
	}
}

void CLayer::Pick(CWnd* pWnd, const CViewinfo& viewinfo, const CPoly& polygon, const bool& bEnlock) const
{
	if(m_bVisible == false)
		return;
	const bool bVisible = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bVisible == false)
		return;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == true)
			continue;
		if(bEnlock == false && pGeom->m_bLocked == true)
			continue;
		if(pGeom->PickIn(polygon) == false)
			continue;
		if(CanActivateGeom(pGeom) == false)
			continue;

		CClientDC dc(pWnd);

		pGeom->m_bActive = true;
		pGeom->DrawAnchors(&dc, viewinfo);

		m_nActiveCount++;
		GeomActivated(pGeom);
	}
}

CGeom* CLayer::Look(const CViewinfo& viewinfo, const CPoint& point) const
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		CRect rect = pGeom->m_Rect;
		if(rect.PtInRect(point) == FALSE)
			continue;
		const CSize gap = this->GetDocInflation(viewinfo, pGeom);
		if(pGeom->PickOn(point, gap.cx) == true)
			return pGeom;
		else if(pGeom->PickIn(point) == true)
			return pGeom;
	}

	return nullptr;
}

bool CLayer::NewGeom(CWnd* pWnd, const CDatainfo& datainfo, CGeom* pGeom, Undoredo::CActionStack& actionstack)
{
	if(m_bLocked == true)
	{
		ReleaseCapture();
		AfxMessageBox(IDS_LAYERLOCKEDNOGEOM);
		return false;
	}
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) && this->GetSpot() == nullptr)
	{
		ReleaseCapture();
		AfxMessageBox(IDS_LAYERNEEDSPOT);
		return false;
	}
	pGeom->m_bActive = true;
	m_nActiveCount++;

	if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
		((CMark*)pGeom)->CalCorner(this->GetSpot(), datainfo, m_tree.m_library);
	else if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
		((CText*)pGeom)->CalCorner(this->GetType(), datainfo.m_zoomPointToDoc, 1.f);

	std::list<std::pair<CGeom*, unsigned int>> geoms;
	geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, m_geomlist.GetCount()));
	Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
	pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
	pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
	pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
	actionstack.Record(pAppend);

	m_geomlist.AddTail(pGeom);
	SetModifiedFlag(true);
	return true;
}

void CLayer::AddHead(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack)
{
	if(from == nullptr)
		return;
	if(from->m_nActiveCount == 0)
		return;

	CTypedPtrList<CObList, CGeom*>& geomlist = from->GetGeomList();
	if(geomlist.GetCount() == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVEON);
		return;
	}
	if(from->m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVEOFF);
		return;
	}
	CATTDatasource* pATTDatasource1 = this->GetAttDatasource();
	CATTDatasource* pATTDatasource2 = from->GetAttDatasource();
	if(pATTDatasource1 != pATTDatasource2)
	{
		if(AfxMessageBox(_T("Can't move geometries from one layer to another if they have different attribute database table!"), MB_YESNO) == IDNO)
			return;
	}

	POSITION pos1, pos2;
	pos1 = geomlist.GetTailPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = geomlist.GetPrev(pos1);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		m_geomlist.AddHead(pGeom);
		geomlist.RemoveAt(pos2);

		from->m_nActiveCount--;
		m_nActiveCount++;

		this->Invalidate(pWnd, viewinfo, pGeom);

		if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE && pGeom->m_pType == nullptr)
			((CText*)pGeom)->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE && ((CMark*)pGeom)->m_pSpot == nullptr)
			((CMark*)pGeom)->CalCorner(this->GetSpot(), viewinfo.m_datainfo, m_tree.m_library);

		from->Invalidate(pWnd, viewinfo, pGeom);

		SetModifiedFlag(true);
	}
}

void CLayer::AddTail(CWnd* pWnd, const CViewinfo& viewinfo, CLayer* from, Undoredo::CActionStack& actionstack)
{
	if(from == nullptr)
		return;
	if(from->m_nActiveCount == 0)
		return;

	CTypedPtrList<CObList, CGeom*>& geomlist = from->GetGeomList();
	if(geomlist.GetCount() == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVEON);
		return;
	}

	if(from->m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVEOFF);
		return;
	}

	CATTDatasource* pATTDatasource1 = this->GetAttDatasource();
	CATTDatasource* pATTDatasource2 = from->GetAttDatasource();
	if(pATTDatasource1 != pATTDatasource2)
	{
		if(AfxMessageBox(_T("Can't move geometries from one layer to another if they have different attribute database table!"), MB_YESNO) == IDNO)
			return;
	}

	POSITION pos1, pos2;
	pos1 = geomlist.GetHeadPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = geomlist.GetNext(pos1);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		m_geomlist.AddTail(pGeom);
		geomlist.RemoveAt(pos2);

		from->m_nActiveCount--;
		m_nActiveCount++;

		this->Invalidate(pWnd, viewinfo, pGeom);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE && pGeom->m_pType == nullptr)
			((CText*)pGeom)->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE && ((CMark*)pGeom)->m_pSpot == nullptr)
			((CMark*)pGeom)->CalCorner(this->GetSpot(), viewinfo.m_datainfo, m_tree.m_library);

		from->Invalidate(pWnd, viewinfo, pGeom);

		SetModifiedFlag(true);
	}
}

void CLayer::Duplicate(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, const CSize& Δ, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOCOPY);
		return;
	}

	CClientDC dc(pWnd);

	std::list<std::pair<CGeom*, unsigned int>> geoms;
	unsigned int index = -1;
	POSITION pos1 = m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		index++;
		CGeom* pGeom1 = m_geomlist.GetNext(pos1);
		if(pGeom1->m_bLocked == true)
			continue;
		if(pGeom1->m_bActive == false)
			continue;

		PreInactivateGeom(datainfo, pGeom1);

		pGeom1->m_bActive = false;
		CGeom* pGeom2 = pGeom1->Clone();
		if(pGeom2 != nullptr)
		{
			pGeom2->m_bActive = true;
			if(Δ != CSize(0, 0))
			{
				pGeom1->DrawAnchors(&dc, viewinfo);
				pGeom2->Move(Δ);

				this->Invalidate(pWnd, viewinfo, pGeom1);
				this->Invalidate(pWnd, viewinfo, pGeom2);
				pGeom2->DrawAnchors(&dc, viewinfo);
			}

			m_geomlist.InsertAfter(pos2, pGeom2);
			SetModifiedFlag(true);
			pWnd->SendMessage(WM_NEWGEOMBRED, (UINT)this, (UINT)pGeom2);

			index++;
			geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom2, index));
		}
	}

	if(geoms.size() > 0)
	{
		geoms.reverse();//index must be from big to small
		Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
		pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
		actionstack.Record(pAppend);
	}
}

void CLayer::CopyTo(CLayer* pLayer) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetNext(pos);
		CGeom* pCloned = pGeom->Clone();
		pLayer->m_geomlist.AddTail(pCloned);
	}
	pLayer->m_wId = m_wId;
	pLayer->m_bViewLevel = m_bViewLevel;
	pLayer->m_bReserved = m_bReserved;
	pLayer->m_strName = m_strName;
	pLayer->m_bVisible = m_bVisible;

	pLayer->m_minLevelObj = m_minLevelObj;
	pLayer->m_maxLevelObj = m_maxLevelObj;
	pLayer->m_minLevelTag = m_minLevelTag;
	pLayer->m_maxLevelTag = m_maxLevelTag;

	pLayer->m_bShowTag = m_bShowTag;
	pLayer->m_bLocked = m_bLocked;
	pLayer->m_bDetour = m_bDetour;
	pLayer->m_bHide = m_bHide;
	pLayer->m_bPivot = m_bPivot;
	pLayer->m_bKeyQuery = m_bKeyQuery;
	pLayer->m_attDatasource = m_attDatasource == nullptr ? nullptr : (CATTDatasource*)m_attDatasource->Clone();
	pLayer->m_bAttribute = m_bAttribute;
	pLayer->m_bCentroid = m_bCentroid;

	pLayer->m_bDynamic = m_bDynamic;
	pLayer->m_bDetour = m_bDetour;

	delete pLayer->m_pSpot;
	pLayer->m_pSpot = nullptr;
	delete pLayer->m_pLine;
	pLayer->m_pLine = nullptr;
	delete pLayer->m_pFill;
	pLayer->m_pFill = nullptr;
	delete pLayer->m_pType;
	pLayer->m_pType = nullptr;
	delete pLayer->m_pHint;
	pLayer->m_pHint = nullptr;
	delete pLayer->m_pThem;
	pLayer->m_pThem = nullptr;

	if(m_pSpot != nullptr)
		pLayer->m_pSpot = m_pSpot->Clone();
	if(m_pLine != nullptr)
		pLayer->m_pLine = m_pLine->Clone();
	if(m_pFill != nullptr)
		pLayer->m_pFill = (CFillGeneral*)m_pFill->Clone();
	if(m_pType != nullptr)
		pLayer->m_pType = m_pType->Clone();
	if(m_pHint != nullptr)
		pLayer->m_pHint = m_pHint->Clone();
	if(m_pThem != nullptr)
		pLayer->m_pThem = m_pThem->Clone();
}
void CLayer::Migrate(CLayer* layer)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		layer->m_geomlist.AddTail(pGeom);
	}
	m_geomlist.RemoveAll();

	layer->m_wId = m_wId;
	layer->m_bViewLevel = m_bViewLevel;
	layer->m_bReserved = m_bReserved;
	layer->m_strName = m_strName;
	layer->m_bVisible = m_bVisible;

	layer->m_minLevelObj = m_minLevelObj;
	layer->m_maxLevelObj = m_maxLevelObj;
	layer->m_minLevelTag = m_minLevelTag;
	layer->m_maxLevelTag = m_maxLevelTag;

	layer->m_bShowTag = m_bShowTag;
	layer->m_bLocked = m_bLocked;
	layer->m_bDetour = m_bDetour;
	layer->m_bHide = m_bHide;
	layer->m_bPivot = m_bPivot;
	layer->m_bKeyQuery = m_bKeyQuery;
	layer->m_bAttribute = m_bAttribute;
	layer->m_bCentroid = m_bCentroid;

	layer->m_bDynamic = m_bDynamic;
	layer->m_bDetour = m_bDetour;

	delete layer->m_attDatasource;
	layer->m_attDatasource = nullptr;
	delete layer->m_pSpot;
	layer->m_pSpot = nullptr;
	delete layer->m_pLine;
	layer->m_pLine = nullptr;
	delete layer->m_pFill;
	layer->m_pFill = nullptr;
	delete layer->m_pType;
	layer->m_pType = nullptr;
	delete layer->m_pHint;
	layer->m_pHint = nullptr;
	delete layer->m_pThem;
	layer->m_pThem = nullptr;

	if(m_attDatasource != nullptr)
	{
		layer->m_attDatasource = m_attDatasource;
		m_attDatasource = nullptr;
	}
	if(m_pSpot != nullptr)
	{
		layer->m_pSpot = m_pSpot;
		m_pSpot = nullptr;
	}
	if(m_pLine != nullptr)
	{
		layer->m_pLine = m_pLine;
		m_pLine = nullptr;
	}
	if(m_pFill != nullptr)
	{
		layer->m_pFill = m_pFill;
		m_pFill = nullptr;
	}
	if(m_pType != nullptr)
	{
		layer->m_pType = m_pType;
		m_pType = nullptr;
	}
	if(m_pHint != nullptr)
	{
		layer->m_pHint = m_pHint;
		m_pHint = nullptr;
	}
	if(m_pThem != nullptr)
	{
		layer->m_pThem = m_pThem;
		m_pThem = nullptr;
	}
}

bool CLayer::CopyGeomTo(CLayer* pLayer, function<void(CString)> dbcopied) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == true)
		{
			pLayer->m_geomlist.AddTail(pGeom);
			pLayer->m_nActiveCount++;
		}
	}

	pLayer->m_wId = m_wId;
	pLayer->m_bViewLevel = m_bViewLevel;
	pLayer->m_bReserved = m_bReserved;
	pLayer->m_strName = m_strName;
	pLayer->m_bVisible = m_bVisible;

	pLayer->m_minLevelObj = m_minLevelObj;
	pLayer->m_maxLevelObj = m_maxLevelObj;
	pLayer->m_minLevelTag = m_minLevelTag;
	pLayer->m_maxLevelTag = m_maxLevelTag;

	pLayer->m_bShowTag = m_bShowTag;
	pLayer->m_bLocked = m_bLocked;
	pLayer->m_bDetour = m_bDetour;
	pLayer->m_bHide = m_bHide;
	pLayer->m_bPivot = m_bPivot;
	pLayer->m_bKeyQuery = m_bKeyQuery;
	pLayer->m_attDatasource = m_attDatasource == nullptr ? nullptr : (CATTDatasource*)m_attDatasource->Clone();
	pLayer->m_bAttribute = m_bAttribute;
	pLayer->m_bDynamic = m_bDynamic;
	pLayer->m_bDetour = m_bDetour;
	pLayer->m_bCentroid = m_bCentroid;

	delete pLayer->m_pSpot;
	pLayer->m_pSpot = nullptr;
	delete pLayer->m_pLine;
	pLayer->m_pLine = nullptr;
	delete pLayer->m_pFill;
	pLayer->m_pFill = nullptr;
	delete pLayer->m_pType;
	pLayer->m_pType = nullptr;
	delete pLayer->m_pThem;
	pLayer->m_pThem = nullptr;

	if(pLayer->m_geomlist.GetCount() != 0)
	{
		pLayer->m_pSpot = m_pSpot;
		pLayer->m_pLine = m_pLine;
		pLayer->m_pFill = m_pFill;
		pLayer->m_pType = m_pType;
		pLayer->m_pThem = m_pThem;
	}
	else
	{
		m_tree.m_library.m_libLineSymbol.Copy<CLineSymbol>(m_pLine, pLayer->m_tree.m_library.m_libLineSymbol);
		m_tree.m_library.m_libFillSymbol.Copy<CFillSymbol>(m_pFill, pLayer->m_tree.m_library.m_libFillSymbol);
		m_tree.m_library.m_libFillPattern.Copy<CFillPattern>(m_pFill, pLayer->m_tree.m_library.m_libFillPattern);
		m_tree.m_library.m_libFillFlash.Copy<CFillPattern>(m_pFill, pLayer->m_tree.m_library.m_libFillFlash);
		m_tree.m_library.m_libSpotSymbol.Copy<CSpotSymbol>(m_pSpot, pLayer->m_tree.m_library.m_libSpotSymbol);
		m_tree.m_library.m_libSpotPattern.Copy<CSpotPattern>(m_pSpot, pLayer->m_tree.m_library.m_libSpotPattern);
		m_tree.m_library.m_libSpotFlash.Copy<CSpotFlash>(m_pSpot, pLayer->m_tree.m_library.m_libSpotFlash);
	}
	if(pLayer->m_attDatasource != nullptr && dbcopied != nullptr)
	{
		dbcopied(pLayer->m_attDatasource->m_strDatabase);
	}
	return true;
}

void CLayer::GetEditableActives(std::list<CGeom*>& geoms) const
{
	if(m_nActiveCount == 0)
		return;
	else if(m_bLocked == true)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		geoms.push_back(pGeom);
	}
}

bool CLayer::Clear(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return false;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNODELETE);
		return false;
	}
	int index = m_geomlist.GetCount();
	std::list<std::pair<CGeom*, unsigned int>> geoms;
	POSITION pos1, pos2;
	pos1 = m_geomlist.GetTailPosition();
	while((pos2 = pos1) != nullptr)
	{
		index--;
		CGeom* pGeom = m_geomlist.GetPrev(pos1);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		pWnd->SendMessage(WM_TOREMOVEEDGE, (LONG)this, (UINT)pGeom);
		pWnd->SendMessage(WM_PREREMOVEGEOM, (LONG)this, (UINT)pGeom);
		this->Invalidate(pWnd, viewinfo, pGeom);
		m_geomlist.RemoveAt(pos2);
		pGeom->m_bActive = false;
		geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));

		m_nActiveCount--;

		SetModifiedFlag(true);
	}

	if(geoms.size() > 0)
	{
		Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(nullptr, this, geoms);
		pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
		actionstack.Record(pRemove);
	}

	return true;
}

void CLayer::Group(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount < 2)
		return;
	else if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOGROUP);
		return;
	}

	CClientDC dc(pWnd);

	std::list<std::pair<CGeom*, unsigned int>> geoms;
	std::pair<CGeom*, unsigned int> group;
	DWORD dwATT = 0XFFFFFFFF;
	POSITION pos2;
	POSITION pos1 = m_geomlist.GetHeadPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos1);
		if(pGeom->m_bActive == false)
			continue;
		else
		{
			int index = GetIndexFromOBList(m_geomlist, pGeom);
			geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));
		}
	}

	if(geoms.size() > 1)
	{
		CGroup* pGroup = new CGroup();
		pGroup->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
		pGroup->m_bActive = true;
		for(std::list<std::pair<CGeom*, unsigned int>>::reverse_iterator it = geoms.rbegin(); it != geoms.rend(); it++)
		{
			CGeom* pGeom = it->first;
			pGeom->m_bActive = false;
			pGeom->DrawAnchors(&dc, viewinfo);
			pGroup->m_geomlist.AddHead(pGeom);
			pGroup->m_Rect = UnionRect(pGroup->m_Rect, pGeom->m_Rect);
			const POSITION pos = m_geomlist.FindIndex(it->second);
			if(it == (--geoms.rend()))
			{
				group = std::pair<CGeom*, unsigned int>(pGroup, it->second);
				m_geomlist.SetAt(pos, pGroup);
			}
			else
			{
				pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom);
				m_geomlist.RemoveAt(pos);
			}
		}
		actionstack.Start();
		geoms.reverse();//index big->small
		Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
		pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		actionstack.Record(pRemove);

		Undoredo::CAppend<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, group);
		pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
		pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
		pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungrouped_Group;
		actionstack.Record(pAppend);
		actionstack.Stop();
		this->Invalidate(pWnd, viewinfo, pGroup);
		m_nActiveCount = 1;
	}
}

void CLayer::UnGroup(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOUNGROUP);
		return;
	}

	actionstack.Start();
	CClientDC dc(pWnd);
	POSITION pos2;
	POSITION pos1 = m_geomlist.GetTailPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetPrev(pos1);
		if(pGeom1->m_bActive == false)
			continue;
		else if(pGeom1->m_bLocked == true)
			continue;
		else if(pGeom1->IsKindOf(RUNTIME_CLASS(CClip)) == true)
			continue;
		else if(pGeom1->m_bGroup == false)
			continue;
		else if(pGeom1->IsKindOf(RUNTIME_CLASS(CMask)) == true)
			continue;

		CTypedPtrList<CObList, CGeom*> geomlist;
		if(pGeom1->UnGroup(geomlist) == false)
			continue;

		if(geomlist.GetCount() == 1)
		{
			CGeom* pGeom2 = geomlist.GetHead();
			int index = GetIndexFromOBList(m_geomlist, pGeom1);
			m_geomlist.SetAt(pos2, pGeom2);
			pGeom1->m_bActive = false;
			pGeom1->DrawAnchors(&dc, viewinfo);
			pGeom2->DrawAnchors(&dc, viewinfo);
			pGeom2->m_bActive = true;

			actionstack.Start();
			const auto pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom1, index));
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungrouped_Group;
			actionstack.Record(pRemove);

			const auto pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom2, index));
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			actionstack.Record(pAppend);
			actionstack.Stop();

			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom1);
		}
		else if(geomlist.GetCount() > 1)
		{
			int index = GetIndexFromOBList(m_geomlist, pGeom1);
			std::list<std::pair<CGeom*, unsigned int>> geoms;

			POSITION pos3 = geomlist.GetTailPosition();
			while(pos3 != nullptr)
			{
				CGeom* pGeom2 = geomlist.GetPrev(pos3);
				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom2, geomlist.GetCount() + index--));
				m_geomlist.InsertAfter(pos2, pGeom2);
				pGeom2->m_bActive = true;
				pGeom2->DrawAnchors(&dc, viewinfo);
				m_nActiveCount++;
			}

			actionstack.Start();
			Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			actionstack.Record(pAppend);

			const auto pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom1, index));
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungrouped_Group;
			actionstack.Record(pRemove);
			actionstack.Stop();

			pGeom1->DrawAnchors(&dc, viewinfo);
			pGeom1->m_bActive = false;
			m_nActiveCount--;
			m_geomlist.RemoveAt(pos2);

			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom1);
		}
	}
	actionstack.Stop();
}

void CLayer::Letitgo(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOUNGROUP);
		return;
	}

	actionstack.Start();
	CClientDC dc(pWnd);
	POSITION pos2;
	POSITION pos1 = m_geomlist.GetTailPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetPrev(pos1);
		if(pGeom1->m_bActive == false)
			continue;
		else if(pGeom1->m_bLocked == true)
			continue;
		else if(pGeom1->m_bGroup == false)
			continue;
		else if(pGeom1->IsKindOf(RUNTIME_CLASS(CClip)) == false && pGeom1->IsKindOf(RUNTIME_CLASS(CMask)) == false)
			continue;

		CTypedPtrList<CObList, CGeom*> geomlist;
		if(pGeom1->Letitgo(geomlist) == false)
			continue;

		if(geomlist.GetCount() == 1)
		{
			CGeom* pGeom2 = geomlist.GetHead();
			int index = GetIndexFromOBList(m_geomlist, pGeom1);
			m_geomlist.SetAt(pos2, pGeom2);
			pGeom1->m_bActive = false;
			pGeom1->DrawAnchors(&dc, viewinfo);
			pGeom2->DrawAnchors(&dc, viewinfo);
			pGeom2->m_bActive = true;

			actionstack.Start();
			const auto pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom1, index));
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungrouped_Group;
			actionstack.Record(pRemove);

			const auto pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom2, index));
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			actionstack.Record(pAppend);
			actionstack.Stop();

			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom1);
		}
		else if(geomlist.GetCount() > 1)
		{
			int index = GetIndexFromOBList(m_geomlist, pGeom1);
			std::list<std::pair<CGeom*, unsigned int>> geoms;

			POSITION pos3 = geomlist.GetTailPosition();
			while(pos3 != nullptr)
			{
				CGeom* pGeom2 = geomlist.GetPrev(pos3);
				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom2, geomlist.GetCount() + index--));
				m_geomlist.InsertAfter(pos2, pGeom2);
				pGeom2->m_bActive = true;
				pGeom2->DrawAnchors(&dc, viewinfo);
				m_nActiveCount++;
			}

			actionstack.Start();
			Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			actionstack.Record(pAppend);

			const auto pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::pair<CGeom*, unsigned int>>(pWnd, this, std::pair<CGeom*, unsigned int>(pGeom1, index));
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geom;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geom;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungrouped_Group;
			actionstack.Record(pRemove);
			actionstack.Stop();

			pGeom1->DrawAnchors(&dc, viewinfo);
			pGeom1->m_bActive = false;
			m_nActiveCount--;
			m_geomlist.RemoveAt(pos2);

			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom1);
		}
	}
	actionstack.Stop();
}
void CLayer::ResetGeogroid(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(!m_bVisible)
		return;
	if(!m_bCentroid)
		return;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bClosed == false)
			continue;

		pGeom->ResetGeogroid();
		pGeom->Invalidate(pWnd, viewinfo, 0);
	}
}

void CLayer::ResetLabeloid(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(!m_bVisible)
		return;
	if(!m_bCentroid)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bClosed == false)
			continue;

		pGeom->InvalidateTag(pWnd, viewinfo, pHint);
		pGeom->ResetLabeloid();
		pGeom->Invalidate(pWnd, viewinfo, 0);
		pGeom->InvalidateTag(pWnd, viewinfo, pHint);
	}
}
void CLayer::PlantGeogroid(CWnd* pWnd, const CViewinfo& viewinfo, std::list<CMark*>& seeds)
{
	if(m_bVisible == false)
		return;
	if(seeds.size() == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bClosed == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
			continue;

		for(std::list<CMark*>::iterator it2 = seeds.begin(); it2 != seeds.end(); it2++)
		{
			const CMark* pMark = *it2;
			if(pGeom->PickIn(pMark->m_Origin) == true)
			{
				pGeom->InvalidateTag(pWnd, viewinfo, pHint);
				if(((CPoly*)pGeom)->m_geogroid == nullptr)
					((CPoly*)pGeom)->m_geogroid = new CPoint;
				*((CPoly*)pGeom)->m_geogroid = pMark->m_Origin;
				pGeom->m_strName = pMark->m_strName;
				pGeom->m_attId = pMark->m_attId;
				seeds.erase(it2);
				pGeom->InvalidateTag(pWnd, viewinfo, pHint);
				break;
			}
		}
	}
}

void CLayer::PlantLabeloid(CWnd* pWnd, const CViewinfo& viewinfo, std::list<CMark*>& seeds)
{
	if(m_bVisible == false)
		return;
	if(seeds.size() == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bClosed == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
			continue;

		for(std::list<CMark*>::iterator it2 = seeds.begin(); it2 != seeds.end(); it2++)
		{
			const CMark* pMark = *it2;
			if(pGeom->PickIn(pMark->m_Origin) == true)
			{
				pGeom->InvalidateTag(pWnd, viewinfo, pHint);
				if(((CPoly*)pGeom)->m_labeloid == nullptr)
					((CPoly*)pGeom)->m_labeloid = new CPoint;
				*((CPoly*)pGeom)->m_labeloid = pMark->m_Origin;
				pGeom->m_strName = pMark->m_strName;
				pGeom->m_attId = pMark->m_attId;
				seeds.erase(it2);
				pGeom->InvalidateTag(pWnd, viewinfo, pHint);
				break;
			}
		}
	}
}
bool CLayer::Copyable() const
{
	if(m_nActiveCount > 0)
		return true;

	CLayer* pChild = this->FirstChild();
	while(pChild != nullptr)
	{
		if(pChild->Copyable() == true)
			return true;

		pChild = pChild->Nextsibling();
	}

	return false;
}
bool CLayer::SuportGeoCode() const
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->HasGeoCode())
			return true;
	}

	CLayer* pChild = this->FirstChild();
	while(pChild != nullptr)
	{
		if(pChild->SuportGeoCode() == true)
			return true;

		pChild = pChild->Nextsibling();
	}

	return false;
}
void CLayer::Align(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, CRect rect, ALIGN align)
{
	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKED);
		return;
	}

	actionstack.Start();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == true)
			continue;
		else if(pGeom->m_bActive == false)
			continue;

		CSize Δ(0, 0);
		CRect geomRect = pGeom->GetRect();
		switch(align)
		{
			case ALIGN::Left:
				Δ.cx = rect.left - geomRect.left;
				break;
			case ALIGN::HCenter:
				Δ.cx = (rect.left + rect.Width() / 2) - (geomRect.left + geomRect.Width() / 2);
				break;
			case ALIGN::Right:
				Δ.cx = rect.right - geomRect.right;
				break;
			case ALIGN::Top:
				Δ.cy = rect.top - geomRect.top;
				break;
			case ALIGN::VCenter:
				Δ.cy = (rect.top + rect.Height() / 2) - (geomRect.top + geomRect.Height() / 2);
				break;
			case ALIGN::Bottom:
				Δ.cy = rect.bottom - geomRect.bottom;
				break;
			case ALIGN::Center:
				Δ.cx = (rect.left + rect.Width() / 2) - (geomRect.left + geomRect.Width() / 2);
				Δ.cy = (rect.top + rect.Height() / 2) - (geomRect.top + geomRect.Height() / 2);
				break;
			case ALIGN::YRight:
				Δ.cx = -geomRect.left;
				Δ.cy = 0;
				break;
			case ALIGN::YCenter:
				Δ.cx = -geomRect.CenterPoint().x;
				Δ.cy = 0;
				break;
			case ALIGN::YLeft:
				Δ.cx = -geomRect.right;
				Δ.cy = 0;
				break;
			case ALIGN::XOver:
				Δ.cx = 0;
				Δ.cy = -geomRect.top;
				break;
			case ALIGN::XCenter:
				Δ.cx = 0;
				Δ.cy = -geomRect.CenterPoint().y;
				break;
			case ALIGN::XUnder:
				Δ.cx = 0;
				Δ.cy = -geomRect.bottom;
				break;
			case ALIGN::Origin:
				Δ.cx = -geomRect.CenterPoint().x;
				Δ.cy = -geomRect.CenterPoint().y;
				break;
		}

		Undoredo::CModify<CWnd*, CLayer*, std::pair<CGeom*, CSize>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::pair<CGeom*, CSize>>(nullptr, this, std::pair<CGeom*, CSize>(pGeom, Δ), std::pair<CGeom*, CSize>(pGeom, Δ));
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Move;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Move;
		actionstack.Record(pModify);

		this->Invalidate(pWnd, viewinfo, pGeom);
		pGeom->Move(Δ);
		this->Invalidate(pWnd, viewinfo, pGeom);
	}
	actionstack.Stop();
}

bool CLayer::Join(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack, std::function<bool(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joinable, std::function<void(WORD wLayer, DWORD dwPoly1, DWORD dwPoly2, const BYTE& mode)>& joined)
{
	if(m_bVisible == false)
		return false;
	if(m_nActiveCount == 0)
		return false;
	else if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOJOIN);
		return false;
	}
	actionstack.Start();
	POSITION pos1 = m_geomlist.GetTailPosition();
	POSITION pos2 = pos1;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetPrev(pos1);
		if(pGeom1->m_bLocked == true)
			continue;
		else if(pGeom1->m_bActive == false)
			continue;
		else if(pGeom1->m_bClosed == true)
			continue;
		else if(pGeom1->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
			continue;
		POSITION pos3 = pos1;
		POSITION pos4;
		while((pos4 = pos3) != nullptr)
		{
			CGeom* pGeom2 = m_geomlist.GetPrev(pos3);
			if(pGeom2->m_bLocked == true)
				continue;
			else if(pGeom2->m_bActive == false)
				continue;
			else if(pGeom2->m_bClosed == true)
				continue;
			else if(pGeom2->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
				continue;
			else if(pGeom1->m_strName != pGeom2->m_strName) //ÌØÊâ
				continue;

			unsigned short mode = CPoly::Joinable((CPoly*)pGeom1, (CPoly*)pGeom2, tolerance);
			if(mode == 0)
				continue;
			if(mode > 4)
				continue;
			if(joinable(this->m_wId, pGeom1->m_geoId, pGeom2->m_geoId, mode) == false)
				continue;

			CPoly* pPoly = ((CPoly*)pGeom1)->Join((CPoly*)pGeom2, mode);
			if(pPoly != nullptr)
			{
				int index1 = GetIndexFromOBList(m_geomlist, pGeom1);
				int index2 = GetIndexFromOBList(m_geomlist, pGeom2);
				std::list<std::pair<CGeom*, unsigned int>> geoms;
				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom1, index1));
				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom2, index2));
				actionstack.Start();
				{
					Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
					pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
					pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
					pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
					actionstack.Record(pRemove);
				}
				pGeom1->m_bActive = false;
				pGeom2->m_bActive = false;
				this->Invalidate(pWnd, viewinfo, pGeom1);
				this->Invalidate(pWnd, viewinfo, pGeom2);
				m_geomlist.RemoveAt(pos2);
				this->Invalidate(pWnd, viewinfo, pPoly);
				m_geomlist.SetAt(pos4, pPoly);
				pWnd->SendMessage(WM_PREREMOVEGEOM, (LONG)this, (UINT)pGeom2);
				pPoly->m_bActive = true;
				m_nActiveCount--;
				SetModifiedFlag(true);

				{
					std::list<std::pair<CGeom*, unsigned int>> geoms;
					geoms.push_back(std::pair<CGeom*, unsigned int>(pPoly, index2));
					Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
					pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
					pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
					pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
					actionstack.Record(pAppend);
				}

				joined(this->m_wId, pGeom1->m_geoId, pGeom2->m_geoId, mode);
				actionstack.Stop();

				break;
			}
		}
	}

	POSITION pos5 = m_geomlist.GetTailPosition();
	while(pos5 != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos5);
		if(pGeom->m_bLocked == true)
			continue;
		else if(pGeom->m_bActive == false)
			continue;
		else if(pGeom->m_bClosed == true)
			continue;
		else if(pGeom->Close(false, tolerance, &actionstack) == true)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
	}
	actionstack.Stop();
	return true;
}

bool CLayer::Combine(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_bVisible == false)
		return false;
	if(m_nActiveCount == 0)
		return false;
	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOGROUP);
		return false;
	}

	long nLine = 0;
	long nArea = 0;
	CTypedPtrList<CObList, CPath*> geomlist;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPath)) == TRUE)
		{
			if(pGeom->m_bClosed == true)
				nArea++;
			else
				nLine++;

			geomlist.AddTail((CPath*)pGeom);
		}
	}

	if(nArea != 0 && nLine != 0)
	{
		AfxMessageBox(_T("Can't combine lines and areas together!"));
		geomlist.RemoveAll();
		return false;
	}
	else if(geomlist.GetCount() > 1)
	{
		CDoughnut* donut = new CDoughnut;
		donut->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);

		CGeom* toppest = geomlist.GetTail();
		std::list<std::pair<CGeom*, unsigned int>> group;
		const int indexToppest = GetIndexFromOBList(m_geomlist, toppest);
		group.push_back(std::pair<CGeom*, unsigned int>(donut, indexToppest + 1));
		const POSITION posToppest = m_geomlist.Find(toppest);
		m_geomlist.InsertAfter(posToppest, donut);

		Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, group);
		pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungroup_Groups;
		actionstack.Record(pAppend);

		if(nLine != 0)
			donut->m_bClosed = false;
		if(nArea != 0)
			donut->m_bClosed = true;

		SetModifiedFlag(true);

		std::list<std::pair<CGeom*, unsigned int>> geoms;
		POSITION pos = geomlist.GetTailPosition();
		while(pos != nullptr)
		{
			CPath* pGeom = geomlist.GetPrev(pos);
			PreInactivateGeom(datainfo, pGeom);
			pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom);

			pGeom->m_bActive = false;
			m_nActiveCount--;

			int index = GetIndexFromOBList(m_geomlist, pGeom);
			geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));
			donut->m_geomlist.AddHead(pGeom);
			m_geomlist.RemoveAt(m_geomlist.Find(pGeom));

			SetModifiedFlag(true);
		}
		geomlist.RemoveAll();

		donut->m_bActive = true;
		m_nActiveCount++;
		donut->RecalRect();
		this->Invalidate(pWnd, viewinfo, donut);

		actionstack.Start();
		Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
		pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
		pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
		actionstack.Record(pRemove);
		actionstack.Stop();
		return true;
	}
	else
		return false;
}

bool CLayer::Disband(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return false;
	else if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOUNGROUP);
		return false;
	}

	POSITION pos2, pos1;
	pos2 = pos1 = m_geomlist.GetTailPosition();
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetPrev(pos1);
		if(pGeom1->m_bActive == false)
			continue;
		if(pGeom1->m_bLocked == true)
			continue;

		if(pGeom1->IsKindOf(RUNTIME_CLASS(CDoughnut)) == TRUE)
		{
			pGeom1->m_bActive = false;
			m_nActiveCount--;
			this->Invalidate(pWnd, viewinfo, pGeom1);

			int index = GetIndexFromOBList(m_geomlist, pGeom1);
			std::list<std::pair<CGeom*, unsigned int>> geoms;
			std::list<std::pair<CGeom*, unsigned int>> group;
			group.push_back(std::pair<CGeom*, unsigned int>(pGeom1, index));

			CDoughnut* donut = (CDoughnut*)pGeom1;
			index += donut->m_geomlist.GetCount();
			POSITION Pos3 = donut->m_geomlist.GetTailPosition();
			while(Pos3 != nullptr)
			{
				CGeom* pGeom2 = donut->m_geomlist.GetPrev(Pos3);
				pGeom2->m_strName = pGeom2->m_strName == _T("") ? donut->m_strName : pGeom2->m_strName;
				pGeom2->m_bActive = true;
				m_nActiveCount++;

				geoms.push_back(std::pair<CGeom*, unsigned int>(pGeom2, index--));
				m_geomlist.InsertAfter(pos2, pGeom2);
				pGeom2->m_bActive = true;
				//	pGeom2->DrawAnchors(&dc,viewinfo);
				m_nActiveCount++;
			}

			pWnd->SendMessage(WM_PREREMOVEGEOM, (LONG)this, (UINT)pGeom1);
			m_geomlist.RemoveAt(pos2);

			actionstack.Start();
			Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			actionstack.Record(pAppend);

			Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, group);
			pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Collection::Free_Ungroup_Groups;
			actionstack.Record(pRemove);
			actionstack.Stop();

			SetModifiedFlag(true);
		}
	}

	return true;
}

bool CLayer::Split(CWnd* pWnd, const CViewinfo& viewinfo, const CPoint& local, Undoredo::CActionStack& actionstack, std::function<void(CLayer* pLayer, DWORD dwPoly, const CPoint& point, DWORD dwBred)>& splitted)
{
	if(m_nActiveCount == 0)
		return false;
	if(m_bLocked == true)
		return false;

	POSITION pos1 = m_geomlist.GetTailPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos1);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		const CSize gap = this->GetDocInflation(viewinfo, pGeom);
		std::list<CGeom*> polys = pGeom->Split(local, gap.cx);
		if(polys.size() > 0)
		{
			if(this->Replace(pWnd, viewinfo, pGeom, polys, actionstack) == true)
			{
				CClientDC dc(pWnd);
				for(std::list<CGeom*>::iterator it = polys.begin(); it != polys.end(); it++)
				{
					CGeom* geom = *it;
					geom->DrawAnchors(&dc, viewinfo);
					geom->m_bActive = true;
					m_nActiveCount++;
				}
				if(polys.size() == 2)
				{
					auto it = polys.begin();
					std::advance(it, 1);
					CPoly* second = (CPoly*)*(it);
					splitted(this, pGeom->m_geoId, second->GetAnchor(1), second->m_geoId);
				}
				return true;
			}
			else
			{
				for(std::list<CGeom*>::iterator it = polys.begin(); it != polys.end(); it++)
				{
					const CGeom* geom = *it;
					delete geom;
				}
				polys.clear();
			}
		}
	}

	return false;
}

bool CLayer::Replace(CWnd* pWnd, const CViewinfo& viewinfo, CGeom* pGeom, std::list<CGeom*> geoms, Undoredo::CActionStack& actionstack)
{
	if(pGeom == nullptr)
		return false;
	if(geoms.empty() == true)
		return false;

	int index = GetIndexFromOBList(m_geomlist, pGeom);
	if(index == -1)
		return false;
	const POSITION pos = m_geomlist.Find(pGeom);
	if(pos == nullptr)
		return false;

	std::list<std::pair<CGeom*, unsigned int>> original;
	std::list<std::pair<CGeom*, unsigned int>> modified;
	original.push_back(std::pair<CGeom*, unsigned int>(pGeom, index));

	for(std::list<CGeom*>::iterator it = geoms.begin(); it != geoms.end(); it++)
	{
		CGeom* geom = *it;
		modified.push_back(std::pair<CGeom*, unsigned int>(geom, geoms.size() + index--));
		if(it == geoms.begin())
		{
			geom->m_geoId = pGeom->m_geoId;
		}
		else
		{
			geom->m_geoId = pWnd->SendMessage(WM_APPLYGEOMID, 0, 0);
			pWnd->SendMessage(WM_NEWGEOMBRED, (UINT)this, (UINT)geom);
		}
		m_geomlist.InsertAfter(pos, geom);
	}

	actionstack.Start();
	Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, modified);
	pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
	pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
	pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
	actionstack.Record(pAppend);

	Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, original);
	pRemove->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
	pRemove->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
	pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
	actionstack.Record(pRemove);
	actionstack.Stop();

	if(pGeom->m_bActive == true)
	{
		CClientDC dc(pWnd);
		pGeom->DrawAnchors(&dc, viewinfo);
		pGeom->m_bActive = false;
		m_nActiveCount--;
	}
	pWnd->SendMessage(WM_PREREMOVEGEOM, (UINT)this, (UINT)pGeom);
	m_geomlist.RemoveAt(pos);
	this->Invalidate(pWnd, viewinfo, pGeom);

	SetModifiedFlag(true);
	return true;
}
void CLayer::ReplaceWithMark(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	CSpot* pSpot = this->GetSpot();
	POSITION pos1 = m_geomlist.GetTailPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos1);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		this->Invalidate(pWnd, viewinfo, pGeom);

		CPoint origin = pGeom->GetOrigin();
		CMark* pMark = new CMark(origin);
		pMark->m_strName = pGeom->m_strName;
		m_geomlist.SetAt(pos2, pMark);
		delete pGeom;

		pMark->m_bActive = true;
		pMark->CalCorner(pMark->m_pSpot == nullptr ? pSpot : pMark->m_pSpot, viewinfo.m_datainfo, m_tree.m_library);

		this->Invalidate(pWnd, viewinfo, pMark);
	}
}
bool CLayer::CanDrawDirect(const CGeom* pGeom) const
{
	CLine* pLine = this->GetLine();
	if(pLine != nullptr && (pLine->IsKindOf(RUNTIME_CLASS(CLineSymbol)) || pLine->IsKindOf(RUNTIME_CLASS(CLineRiver))))
		return false;

	CFill* pFill = this->GetFill();
	if(pFill != nullptr && (pFill->IsKindOf(RUNTIME_CLASS(CFillSymbol)) || pGeom->IsKindOf(RUNTIME_CLASS(CFillPattern))))
		return pGeom->m_bClosed == false;
	else
		return true;
}

void CLayer::DrawAnchors(CDC* pDC, const CViewinfo& viewinfo, const CRect& inRect) const
{
	if(m_nActiveCount == 0)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
		{
			pGeom->DrawAnchors(pDC, viewinfo);
		}
	}
}
void CLayer::DrawCentroid(CDC* pDC, const CViewinfo& viewinfo, const CRect& inRect, const bool& labeloid, const bool& geogroid) const
{
	if(!m_bVisible)
		return;

	if(!m_bCentroid)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		const CGeom* pGeom = m_geomlist.GetNext(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, inRect) == true)
		{
			pGeom->DrawCentroid(pDC, viewinfo, labeloid, geogroid);
		}
	}
}
void CLayer::Move(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, const CSize& Δ)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVE);
		return;
	}

	actionstack.Start();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		Undoredo::CModify<CWnd*, CLayer*, std::pair<CGeom*, CSize>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::pair<CGeom*, CSize>>(nullptr, this, std::pair<CGeom*, CSize>(pGeom, Δ), std::pair<CGeom*, CSize>(pGeom, Δ));
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Move;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Move;
		actionstack.Record(pModify);

		this->Invalidate(pWnd, viewinfo, pGeom);
		pGeom->Move(Δ);
		this->Invalidate(pWnd, viewinfo, pGeom);

		SetModifiedFlag(true);
	}
	actionstack.Stop();
}

void CLayer::Arrange(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, ARRANGE arrange)
{
	if(m_nActiveCount == 0)
		return;
	else if(m_nActiveCount == m_geomlist.GetCount())
		return;
	else if(m_geomlist.GetCount() < 2)
		return;
	else if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOMOVE);
		return;
	}
	switch(arrange)
	{
		case ARRANGE::Tofront:
			{
				int indexTo = -1;
				POSITION pos1 = m_geomlist.GetTailPosition();
				POSITION pos2 = m_geomlist.GetTailPosition();
				while((pos2 = pos1) != nullptr)
				{
					CGeom* pGeom = m_geomlist.GetPrev(pos1);
					if(pGeom->m_bActive == true)
						continue;
					else
					{
						indexTo = GetIndexFromOBList(m_geomlist, pGeom);
						break;
					}
				}
				if(indexTo == -1)
					return;
				if(pos2 == nullptr)
					return;
				else if(pos1 == nullptr)
					return;

				std::list<int> original;
				std::list<int> modified;

				POSITION Pos3 = pos1;
				POSITION Pos4 = Pos3;
				while((Pos4 = Pos3) != nullptr)
				{
					CGeom* pGeom = m_geomlist.GetPrev(Pos3);
					if(pGeom->m_bActive == false)
						continue;
					else if(pGeom->m_bLocked == true)
					{
						AfxMessageBox(_T("The selected geometry is locked!"));
						continue;
					}
					else
					{
						int indexFrom = GetIndexFromOBList(m_geomlist, pGeom);
						original.push_back(indexFrom);
						modified.push_back(indexTo--);

						m_geomlist.RemoveAt(Pos4);
						m_geomlist.InsertAfter(pos2, pGeom);
						this->Invalidate(pWnd, viewinfo, pGeom);
					}

					SetModifiedFlag(true);
				}
				if(original.size() > 0 && modified.size() > 0)
				{
					Undoredo::CModify<CWnd*, CLayer*, std::list<int>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::list<int>>(pWnd, this, original, modified);
					pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Position;
					pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Position;
					actionstack.Record(pModify);
				}
			}
			break;
		case ARRANGE::Toback:
			{
				int indexTo = -1;
				POSITION pos1 = m_geomlist.GetHeadPosition();
				POSITION pos2 = m_geomlist.GetHeadPosition();
				while((pos2 = pos1) != nullptr)
				{
					CGeom* pGeom = m_geomlist.GetNext(pos1);
					if(pGeom->m_bActive == true)
						continue;
					else
					{
						indexTo = GetIndexFromOBList(m_geomlist, pGeom);
						break;
					}
				}
				if(indexTo == -1)
					return;
				if(pos2 == nullptr)
					return;
				else if(pos1 == nullptr)
					return;

				std::list<int> original;
				std::list<int> modified;

				POSITION Pos3 = pos1;
				POSITION Pos4 = Pos3;
				while((Pos4 = Pos3) != nullptr)
				{
					CGeom* pGeom = m_geomlist.GetNext(Pos3);
					if(pGeom->m_bActive == false)
						continue;
					else if(pGeom->m_bLocked == true)
					{
						AfxMessageBox(_T("The selected geometry is locked!"));
						continue;
					}
					else
					{
						int indexFrom = GetIndexFromOBList(m_geomlist, pGeom);
						modified.push_back(indexFrom);
						original.push_back(indexTo++);

						m_geomlist.RemoveAt(Pos4);
						m_geomlist.InsertBefore(pos2, pGeom);
						this->Invalidate(pWnd, viewinfo, pGeom);
					}

					SetModifiedFlag(true);
				}
				if(original.size() > 0 && modified.size() > 0)
				{
					original.reverse();
					modified.reverse();
					Undoredo::CModify<CWnd*, CLayer*, std::list<int>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::list<int>>(pWnd, this, original, modified);
					pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Position;
					pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Position;
					actionstack.Record(pModify);
				}
			}
			break;
		case ARRANGE::Forward:
			{
				std::list<int> original;
				std::list<int> modified;

				POSITION pos2 = m_geomlist.GetTailPosition();
				CGeom* pGeom2 = m_geomlist.GetTail();

				POSITION pos1 = pos2;
				m_geomlist.GetPrev(pos1);
				CGeom* pGeom1 = nullptr;

				POSITION pos = pos1;
				while((pos1 = pos) != nullptr)
				{
					pGeom1 = m_geomlist.GetPrev(pos);
					if(pGeom1 == nullptr)
					{
						pos2 = pos1;
						pGeom2 = nullptr;
						continue;
					}
					if(pGeom1->m_bLocked == true)
					{
						pos2 = pos1;
						pGeom2 = pGeom1;
						continue;
					}
					else if(pGeom1->m_bActive == true && pGeom2->m_bActive == false)
					{
						int index = GetIndexFromOBList(m_geomlist, pGeom1);
						original.push_back(index);
						modified.push_back(index + 1);

						m_geomlist.SetAt(pos1, pGeom2);
						m_geomlist.SetAt(pos2, pGeom1);

						this->Invalidate(pWnd, viewinfo, pGeom1);
						this->Invalidate(pWnd, viewinfo, pGeom2);
					}
					else
					{
						pGeom2 = pGeom1;
					}
					pos2 = pos1;

					SetModifiedFlag(true);
				}

				if(original.size() > 0 && modified.size() > 0)
				{
					Undoredo::CModify<CWnd*, CLayer*, std::list<int>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::list<int>>(pWnd, this, original, modified);
					pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Position;
					pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Position;
					actionstack.Record(pModify);
				}
			}
			break;
		case ARRANGE::Backward:
			{
				std::list<int> original;
				std::list<int> modified;

				POSITION pos2 = m_geomlist.GetHeadPosition();
				CGeom* pGeom2 = m_geomlist.GetHead();

				POSITION pos1 = pos2;
				m_geomlist.GetNext(pos1);
				CGeom* pGeom1 = nullptr;

				POSITION pos = pos1;
				while((pos1 = pos) != nullptr)
				{
					pGeom1 = m_geomlist.GetNext(pos);
					if(pGeom1->m_bLocked == true)
					{
						pos2 = pos1;
						pGeom2 = pGeom1;
						continue;
					}
					else if(pGeom1->m_bActive == true && pGeom2->m_bActive == false)
					{
						int index = GetIndexFromOBList(m_geomlist, pGeom1);
						modified.push_back(index);
						original.push_back(index - 1);

						m_geomlist.SetAt(pos1, pGeom2);
						m_geomlist.SetAt(pos2, pGeom1);

						this->Invalidate(pWnd, viewinfo, pGeom1);
						this->Invalidate(pWnd, viewinfo, pGeom2);
					}
					else
					{
						pGeom2 = pGeom1;
					}
					pos2 = pos1;

					SetModifiedFlag(true);
				}

				if(original.size() > 0 && modified.size() > 0)
				{
					original.reverse();
					modified.reverse();
					Undoredo::CModify<CWnd*, CLayer*, std::list<int>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::list<int>>(pWnd, this, original, modified);
					pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Position;
					pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Position;
					actionstack.Record(pModify);
				}
			}
			break;
	}
}

void CLayer::Lock(Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	std::list<CGeom*> geoms;
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		pGeom->m_bLocked = true;
		geoms.push_back(pGeom);
		SetModifiedFlag(true);
	}
	if(geoms.size() > 0)
	{
		Undoredo::CModify<CWnd*, std::list<CGeom*>, bool>* pModify = new Undoredo::CModify<CWnd*, std::list<CGeom*>, bool>(nullptr, geoms, false, true);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Lock;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Lock;
		actionstack.Record(pModify);
	}
}

void CLayer::UnLock(Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	std::list<CGeom*> geoms;
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == false)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		pGeom->m_bLocked = false;
		geoms.push_back(pGeom);
		SetModifiedFlag(true);
	}
	if(geoms.size() > 0)
	{
		Undoredo::CModify<CWnd*, std::list<CGeom*>, bool>* pModify = new Undoredo::CModify<CWnd*, std::list<CGeom*>, bool>(nullptr, geoms, false, true);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Lock;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Lock;
		actionstack.Record(pModify);
	}
}

void CLayer::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		pGeom->Transform(m11, m21, m31, m12, m22, m32);
	}
}
void CLayer::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_nActiveCount == 0)
		return;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		pGeom->Transform(pDC, viewinfo, m11, m21, m31, m12, m22, m32);
	}
}

void CLayer::Transform(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	if(m_nActiveCount == 0)
		return;

	actionstack.Start();
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bLocked == true)
			continue;
		if(pGeom->m_bActive == false)
			continue;

		if(pWnd != nullptr)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
		}
		CGeom* pOld = pGeom->Clone();
		pGeom->Transform(m11, m21, m31, m12, m22, m32);

		if(pWnd != nullptr)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
		}

		Undoredo::CSwap<CWnd*, CLayer*, CGeom*>* pSwap = new Undoredo::CSwap<CWnd*, CLayer*, CGeom*>(pWnd, this, pOld, pGeom);
		pSwap->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Both_Modify_Swap;
		pSwap->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Both_Modify_Swap;
		pSwap->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_Modify_Swap;
		actionstack.Record(pSwap);

		SetModifiedFlag(true);
	}
	actionstack.Stop();
}
void CLayer::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		pGeom->Rotate(mapinfo, deltaLng, deltaLat, facing);
	}
}

void CLayer::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	POSITION pos1 = m_geomlist.GetTailPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetPrev(pos1);
		CGeom* pGeom2 = pGeom1->Project(fMapinfo, tMapinfo, interpolation, tolerance);
		if(pGeom2 == nullptr)
		{
			m_geomlist.RemoveAt(pos2);
			delete pGeom1;
		}
		else if(pGeom2 != pGeom1)
		{
			m_geomlist.SetAt(pos2, pGeom2);
			delete pGeom1;
		}
	}
}

void CLayer::Rectify(CTin& tin)
{
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == false)
			continue;
		pGeom->Rectify(tin);
	}
}

bool CLayer::Clip(CWnd* pWnd, const CPath* pMask, bool bIn, int tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_bVisible == false)
		return false;
	else if(pMask->m_bClosed == false)
		return false;
	else if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOCLIP);
		return false;
	}

	CRect rect = pMask->m_Rect;
	Clipper2Lib::Paths64 clips;
	pMask->GetPaths64(clips, tolerance);
	if(clips.size() == 0)
		return false;

	//	std::list<CGeom*> deleted;
	//	std::list<CGeom*> created;
	const bool bStroke = this->GetLine() == nullptr ? false : true;

	//	std::list<CGeom*> original;
	CTypedPtrList<CObList, CGeom*> geomlist;
	POSITION pos1 = m_geomlist.GetHeadPosition();
	POSITION pos2 = pos1;
	while((pos2 = pos1) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos1);
		//	original.push_back(pGeom);
		if(pGeom == pMask)
			continue;

		CGeom* pCliped = pGeom->Clip(rect, clips, bIn, tolerance, bStroke);
		if(pCliped == nullptr)
		{
			pWnd->SendMessage(WM_PREREMOVEGEOM, (LONG)this, (UINT)pGeom);
			m_geomlist.RemoveAt(pos2);
			delete  pGeom;
			//	deleted.push_back(pGeom);
		}
		else if(pCliped == pGeom)
			continue;
		else
		{
			//	created.push_back(pCliped);
			pWnd->SendMessage(WM_PREREMOVEGEOM, (LONG)this, (UINT)pGeom);
			//	deleted.push_back(pGeom);
			delete pGeom;
			m_geomlist.SetAt(pos2, pCliped);
		}
	}

	//	std::list<CGeom*> modified;
	//	m_geomlist.RemoveAll();
	//	POSITION pos = geomlist.GetHeadPosition();
	//	while(pos != nullptr)
	//	{
	//		CGeom* pGeom = geomlist.GetNext(pos);
	//		modified.push_back(pGeom);
	//
	//		pWnd->SendMessage(WM_NEWGEOMBRED, (UINT)this, (UINT)pGeom);
	//		m_geomlist.AddTail(pGeom);
	//		SetModifiedFlag(true);
	//	}
	//	geomlist.RemoveAll();

	//	actionstack.Start();
	//	Undoredo::CModify<CWnd*, CLayer*, std::list<CGeom*>>* pModify = new Undoredo::CModify<CWnd*, CLayer*, std::list<CGeom*>>(pWnd, this, original, modified);
	//	pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_ReplaceAll;
	//	pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_ReplaceAll;
	//	actionstack.Record(pModify);
	//	Undoredo::CAppend<CWnd*, CLayer*, std::list<CGeom*>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<CGeom*>>(pWnd, this, created);
	//	pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_CreateOrDelete_Geoms;
	//	actionstack.Record(pAppend);
	//	Undoredo::CRemove<CWnd*, CLayer*, std::list<CGeom*>>* pRemove = new Undoredo::CRemove<CWnd*, CLayer*, std::list<CGeom*>>(pWnd, this, deleted);
	//	pRemove->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_CreateOrDelete_Geoms;
	//	actionstack.Record(pRemove);
	//	actionstack.Stop();

	return true;
}

void CLayer::ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const
{
	if(m_bVisible == false)
		return;

	if(m_geomlist.GetCount() == 0)
		return;

	CATTDatasource* pATTDatasource = this->GetAttDatasource();
	_ftprintf(file, _T("Layer:%s %s\n"), m_strName, pATTDatasource != nullptr ? pATTDatasource->GetHeaderProfile().m_strKeyTable : CString(""));
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsValid() == false)
			continue;

		pGeom->ExportPlaintext(file, matrix);
	}

	_ftprintf(file, _T("EndLayer\n"));
}

void CLayer::ExportGeomPs3(FILE* file, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	if(m_geomlist.GetCount() == 0)
		return;

	_ftprintf(file, _T("%%AI5_BeginLayer\n"));
	_ftprintf(file, _T("1 1 1 1 0 1 1 0 79 128 255 0 50 Lb\n"));
	const CString strName = ConvertToOctalString(m_strName);
	_ftprintf(file, _T("(%s) Ln\n"), strName);
	_ftprintf(file, _T("0 A\n"));
	_ftprintf(file, _T("800 Ar\n"));

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	CPsboard aiKey(m_tree.m_library);
	aiKey.pubLine = this->GetLine();
	aiKey.pubFill = pFillGeneral;
	aiKey.pubSpot = this->GetSpot();
	aiKey.pubType = this->GetType();
	aiKey.pubHint = this->GetHint();

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsValid() == false)
			continue;

		pGeom->ExportPs3(file, viewinfo, aiKey);
	}

	_ftprintf(file, _T("LB\n"));
	_ftprintf(file, _T("%%AI5_EndLayer--\n"));
}

void CLayer::ExportTagPs3(FILE* file, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	if(m_geomlist.GetCount() == 0)
		return;

	_ftprintf(file, _T("%%AI5_BeginLayer\n"));
	_ftprintf(file, _T("1 1 1 1 0 1 1 0 79 128 255 0 50 Lb\n"));
	const CString strName = ConvertToOctalString(m_strName + _T("_Anno"));
	_ftprintf(file, _T("(%s) Ln\n"), strName);
	_ftprintf(file, _T("0 A\n"));
	_ftprintf(file, _T("800 Ar\n"));

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	CPsboard aiKey(m_tree.m_library);
	Context context;
	context.pLine = aiKey.pubLine = this->GetLine();
	context.pFill = aiKey.pubFill = pFillGeneral;
	context.pSpot = aiKey.pubSpot = this->GetSpot();
	context.pType = aiKey.pubType = this->GetType();
	aiKey.pubHint = this->GetHint();
	context.ratioMark = 1.0f;
	context.ratioLine = 1.0f;
	context.ratioType = 1.0f;
	context.tolerance = viewinfo.GetPrecision(10);

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	pHint->ExportPs3(file, aiKey);

	POSITION pos2 = m_geomlist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos2);
		if(pGeom->m_pTag == nullptr)
			continue;
		if(pGeom->IsValid() == false)
			continue;

		Gdiplus::RectF geomRect = viewinfo.DocToClient<Gdiplus::RectF>(pGeom->m_Rect);

		Gdiplus::PointF origin = CTag::GetAnchorPoint<Gdiplus::RectF, Gdiplus::PointF>(geomRect, pGeom->m_pTag->GetAnchor());
		pGeom->m_pTag->ExportPs3(file, viewinfo, aiKey, origin, pHint);
	}

	_ftprintf(file, _T("LB\n"));
	_ftprintf(file, _T("%%AI5_EndLayer--\n"));
}

void CLayer::ExportGeomPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	if(m_geomlist.GetCount() == 0)
		return;

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	Context context;
	context.ratioMark = 1.0f;
	context.ratioLine = 1.0f;
	context.ratioType = 1.0f;
	context.pSpot = pSpot;
	context.pLine = pLine;
	context.pFill = pFillGeneral;
	context.pType = pType;
	context.tolerance = viewinfo.GetPrecision(10);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsValid() == false)
			continue;

		pGeom->ExportPdf(pdf, page, viewinfo, m_tree.m_library, context);
	}
}

void CLayer::ExportTagPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	if(m_geomlist.GetCount() == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	pHint->ExportPdf(pdf, page);

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	Context context;
	context.ratioMark = 1.0f;
	context.ratioLine = 1.0f;
	context.ratioType = 1.0f;
	context.pSpot = this->GetSpot();
	context.pLine = this->GetLine();
	context.pFill = pFillGeneral;
	context.pType = this->GetType();
	context.tolerance = viewinfo.GetPrecision(10);

	POSITION pos2 = m_geomlist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos2);
		if(pGeom->HasTag() == false)
			continue;
		if(pGeom->IsValid() == false)
			continue;

		pGeom->ExportPdfTag(pdf, page, viewinfo, pHint);
	}
}

void CLayer::ExportMapInfoTab(const CDatainfo& datainfo, LPCTSTR lpszPath, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(m_bVisible == false)
		return;

	CString mifFileName = lpszPath;
	CString midFileName = lpszPath;
	mifFileName += m_strName;
	midFileName += m_strName;
	mifFileName += _T(".mif");
	midFileName += _T(".mid");
	FILE* fileMif = _tfopen(mifFileName, _T("wt"));
	FILE* fileMid = _tfopen(midFileName, _T("wt"));
	if(fileMif == nullptr || fileMid == nullptr)
	{
		AfxMessageBox(_T("Cann't create a writble mif file!"));
		return;
	}

	_ftprintf(fileMif, _T("Version 450\n"));
	_ftprintf(fileMif, _T("Charset \"WindowsSimpChinese\"\n"));
	_ftprintf(fileMif, _T("Delimiter \",\"\n"));

	if(bTransGeo == true)
	{
		_ftprintf(fileMif, _T("CoordSys Earth Projection 1, 104\n"));
	}
	else
	{
		const CRect rect = GetRect();
		_ftprintf(fileMif, _T("CoordSys NonEarth Units \"m\" Bounds (%d, %d) (%d, %d)\n"), rect.left, rect.top, rect.right, rect.bottom);
	}
	_ftprintf(fileMif, _T("Columns 2\n"));
	_ftprintf(fileMif, _T("	 Id Integer\n"));
	_ftprintf(fileMif, _T("	 Name Char\(50\)\n"));
	_ftprintf(fileMif, _T("Data\n"));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsValid() == false)
			continue;

		//	pGeom->ExportMapInfoTab(datainfo,fileMif,bTransGeo,tolerance);
	}

	fclose(fileMif);
	fclose(fileMid);
}

void CLayer::ExportMapInfoMif(const CDatainfo& datainfo, LPCTSTR lpszPath, const bool& bTransGeo, const unsigned int& tolerance) const
{
	if(m_bVisible == false)
		return;

	CString mifFileName = lpszPath;
	CString midFileName = lpszPath;
	mifFileName += m_strName;
	midFileName += m_strName;
	mifFileName += _T(".mif");
	midFileName += _T(".mid");
	FILE* fileMif = _tfopen(mifFileName, _T("wt"));
	FILE* fileMid = _tfopen(midFileName, _T("wt"));
	if(fileMif == nullptr || fileMid == nullptr)
	{
		AfxMessageBox(_T("Cann't create a writble mif file!"));
		return;
	}

	_ftprintf(fileMif, _T("Version 450\n"));
	_ftprintf(fileMif, _T("Charset \"WindowsSimpChinese\"\n"));
	_ftprintf(fileMif, _T("Delimiter \",\"\n"));

	if(bTransGeo == true)
	{
		_ftprintf(fileMif, _T("CoordSys Earth Projection 1, 104\n"));
	}
	else
	{
		const CRect rect = GetRect();
		_ftprintf(fileMif, _T("CoordSys NonEarth Units \"m\" Bounds (%d, %d) (%d, %d)\n"), rect.left, rect.top, rect.right, rect.bottom);
	}
	_ftprintf(fileMif, _T("Columns 2\n"));
	_ftprintf(fileMif, _T("	 ID Integer\n"));
	_ftprintf(fileMif, _T("	 Name Char\(50\)\n"));
	_ftprintf(fileMif, _T("Data\n"));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsValid() == false)
			continue;

		pGeom->ExportMapInfoMif(datainfo, fileMif, bTransGeo, tolerance);
		pGeom->ExportMid(fileMid);
	}

	fclose(fileMif);
	fclose(fileMid);
}
void CLayer::Serializelist(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << (int)m_geomlist.GetCount();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = (CGeom*)m_geomlist.GetNext(pos);
			const BYTE type = pGeom->Gettype();
			ar << type;
			pGeom->Serialize(ar, dwVersion);
		}
	}
	else
	{
		int nCount;
		ar >> nCount;
		for(int index = 0; index < nCount; index++)
		{
			BYTE type;
			ar >> type;
			CGeom* pGeom = CGeom::Apply(type);
			pGeom->Serialize(ar, dwVersion);
			m_geomlist.AddTail(pGeom);
		}
	}
}
void CLayer::Serialize(CArchive& ar, const DWORD& dwVersion, bool bIgnoreGeoms)
{
	DWORD reserved1 = 0B00000000000000000000000100000000;
	if(ar.IsStoring())
	{
		reserved1 |= (m_bCentroid ? 0B00000000000000000000000010000000 : 0B00000000000000000000000000000000);
		ar << reserved1;
		ar << m_wId;
		ar << m_strName;
		ar << m_bViewLevel;
		ar << m_strGeoCatogery;
		ar << m_bDynamic;

		ar << m_minLevelObj;
		ar << m_maxLevelObj;
		ar << m_minLevelTag;
		ar << m_maxLevelTag;

		ar << m_bVisible;
		ar << m_bShowTag;
		ar << m_bKeyQuery;
		ar << m_bLocked;
		ar << m_bDetour;
		ar << m_bHide;
		ar << m_bPivot;
		ar << m_bAttribute;
		ar << ​m_bTagResistance;
		const CLine::LINETYPE linetype = m_pLine == nullptr ? CLine::LINETYPE::Inherit : m_pLine->Gettype();
		const CFill::FILLTYPE filltype = m_pFill == nullptr ? CFill::FILLTYPE::Inherit : m_pFill->Gettype();
		const BYTE spottype = m_pSpot == nullptr ? 0XFF : m_pSpot->Gettype();
		const BYTE typetype = m_pType == nullptr ? 0X00 : 0X01;
		const BYTE hinttype = m_pHint == nullptr ? 0X00 : 0X01;
		const BYTE themtype = m_pThem == nullptr ? 0XFF : m_pThem->Gettype();
		ar << (BYTE)linetype;
		ar << (BYTE)filltype;
		ar << (BYTE)spottype;
		ar << (BYTE)typetype;
		ar << (BYTE)hinttype;
		ar << (BYTE)themtype;
		ar << (BYTE)(m_attDatasource == nullptr ? 0X00 : 0X01);
	}
	else
	{
		delete m_pSpot;
		delete m_pLine;
		delete m_pFill;
		delete m_pType;
		delete m_pHint;

		ar >> reserved1;
		m_bCentroid = (reserved1 & 0B00000000000000000000000010000000) == 0B00000000000000000000000010000000;
		ar >> m_wId;
		ar >> m_strName;
		ar >> m_bViewLevel;
		ar >> m_strGeoCatogery;
		ar >> m_bDynamic;

		ar >> m_minLevelObj;
		ar >> m_maxLevelObj;
		ar >> m_minLevelTag;
		ar >> m_maxLevelTag;
		ar >> m_bVisible;
		ar >> m_bShowTag;
		ar >> m_bKeyQuery;
		ar >> m_bLocked;
		ar >> m_bDetour;
		ar >> m_bHide;
		ar >> m_bPivot;
		if(dwVersion >= 2)
		{
			ar >> m_bAttribute;
		}
		if(dwVersion >= 4)
		{
			ar >> ​m_bTagResistance;
		}
		BYTE lineindex = -1;
		BYTE fillindex = -1;
		BYTE spotindex = -1;
		BYTE typeindex = -1;
		BYTE hintindex = -1;
		BYTE themindex = -1;
		ar >> lineindex;
		ar >> fillindex;
		ar >> spotindex;
		ar >> typeindex;
		ar >> hintindex;
		ar >> themindex;

		m_pLine = CLine::Apply(lineindex);
		m_pFill = CFillGeneral::Apply(fillindex);
		m_pSpot = CSpot::Apply(spotindex);
		m_pThem = CTheme::Apply(themindex);
		m_pType = typeindex == 0X00 ? nullptr : new CType;
		m_pHint = hintindex == 0X00 ? nullptr : new CHint;
		BYTE byte;
		ar >> byte;
		m_attDatasource = (byte == 0X01 ? new CATTDatasource() : nullptr);
		m_bAttribute |= m_attDatasource != nullptr;
	}

	if(m_pLine != nullptr)
		m_pLine->Serialize(ar, dwVersion);
	if(m_pFill != nullptr)
		m_pFill->Serialize(ar, dwVersion);
	if(m_pSpot != nullptr)
		m_pSpot->Serialize(ar, dwVersion);
	if(m_pType != nullptr)
		m_pType->Serialize(ar, dwVersion);
	if(m_pHint != nullptr)
		m_pHint->Serialize(ar, dwVersion);
	if(m_pThem != nullptr)
		m_pThem->Serialize(ar, dwVersion);

	if(m_attDatasource != nullptr)
	{
		m_attDatasource->Serialize(ar, dwVersion);
	}
	if(ar.IsStoring())
	{
	}
	else if(m_pLine != nullptr && m_pLine->IsKindOf(RUNTIME_CLASS(CLineSymbol)) == TRUE)
	{
		m_pLine->SetWidth(m_tree.m_library.m_libLineSymbol);
	}
	if(bIgnoreGeoms == false)
	{
		Serializelist(ar, dwVersion);
	}

	if(this->IsInfertile())
	{
	}
	else if(ar.IsStoring())
	{
		const int nCount = this->childrensize();
		ar << nCount;
		CLayer* pChild = m_firstchild;
		while(pChild != nullptr)
		{
			const BYTE type = pChild->Gettype();
			ar << type;
			if(type == 11)//11 is CLayon
				ar << pChild->m_wId;
			else if(type == 12)//11 is CLayon
				ar << pChild->m_wId;
			else
				pChild->Serialize(ar, dwVersion);
			pChild = pChild->m_nextsibling;
		}
	}
	else
	{
		int nCount;
		ar >> nCount;
		for(int index = 0; index < nCount; index++)
		{
			BYTE type;
			ar >> type;
			CLayer* pChild = CLayer::Apply(m_tree, type);
			if(pChild != nullptr)
			{
				this->AddLastChild(pChild);
				pChild->Serialize(ar, dwVersion);
			}
			else
			{
				WORD wId;
				ar >> wId;
				CHolder* pHolder = new CHolder(m_tree);
				pHolder->m_wId = wId;
				this->AddLastChild(pHolder);
			}
		}
	}

	if(m_pFill != nullptr && m_pFill->Gettype() == 1)
	{
		delete m_pFill;
		m_pFill = nullptr;
	}
}

void CLayer::ReleaseCE(CArchive& ar) const
{
	ar << m_wId;
	ar << m_bViewLevel;
	SerializeStrCE(ar, m_strName);
	ar << m_minLevelObj;
	ar << m_maxLevelObj;
	ar << m_minLevelTag;
	ar << m_maxLevelTag;
	ar << m_bDynamic;

	ar << m_bVisible;
	ar << m_bShowTag;
	ar << m_bKeyQuery;
	ar << m_bHide;
	ar << m_bPivot;

	ar << (BYTE)(m_pLine == nullptr ? CLine::LINETYPE::Inherit : m_pLine->Gettype());
	ar << (BYTE)(m_pFill == nullptr ? CFill::FILLTYPE::Inherit : m_pFill->Gettype());
	ar << (BYTE)(m_pSpot == nullptr ? -1 : m_pSpot->Gettype());
	ar << (BYTE)(m_pType == nullptr ? -1 : 1);
	ar << (BYTE)(m_pHint == nullptr ? -1 : 1);
	ar << (BYTE)(m_pThem == nullptr ? -1 : m_pThem->Gettype());
	if(m_pLine != nullptr)
		m_pLine->ReleaseCE(ar);
	if(m_pFill != nullptr)
		m_pFill->ReleaseCE(ar);
	if(m_pSpot != nullptr)
		m_pSpot->ReleaseCE(ar);
	if(m_pType != nullptr)
		m_pType->ReleaseCE(ar);
	if(m_pHint != nullptr)
		m_pHint->ReleaseCE(ar);
	//	if(m_pThem != nullptr)
	//		m_pThem->ReleaseCE(ar);

	if(ar.IsStoring())
	{
		const int nCount = m_geomlist.GetCount();
		ar << nCount;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			const BYTE type = pGeom->Gettype();
			ar << type;
			pGeom->ReleaseCE(ar);
		}
	}

	if(ar.IsStoring())
	{
		ar << CTreeNode<CLayer>::childrensize();
		CLayer* pChild = this->FirstChild();
		while(pChild != nullptr)
		{
			ar << pChild->Gettype();
			pChild = pChild->Nextsibling();
		}
	}
	CLayer* pChild = this->FirstChild();
	while(pChild != nullptr)
	{
		pChild->ReleaseCE(ar);
		pChild = pChild->m_nextsibling;
	}
}

void CLayer::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_wId;
		ar << m_minLevelObj;
		ar << m_maxLevelObj;
		ar << m_minLevelTag;
		ar << m_maxLevelTag;
		ar << m_bDynamic;

		WORD wSwitch = 0;
		if(m_bVisible == true) wSwitch |= 0X0001;
		if(m_bShowTag == true) wSwitch |= 0X0002;
		if(m_bKeyQuery == true) wSwitch |= 0X0004;
		if(m_bLocked == true) wSwitch |= 0X0008;
		if(m_bDetour == true) wSwitch |= 0X0010;
		if(m_bHide == true) wSwitch |= 0X0020;
		if(m_bPivot == true) wSwitch |= 0X0040;
		if(m_bAttribute == true) wSwitch |= 0X0080;

		if(m_pLine != nullptr) wSwitch |= 0X0100;
		if(m_pFill != nullptr) wSwitch |= 0X0200;
		if(m_pSpot != nullptr) wSwitch |= 0X0400;
		if(m_pType != nullptr) wSwitch |= 0X0800;
		if(m_pHint != nullptr) wSwitch |= 0X1000;
		if(m_pThem != nullptr) wSwitch |= 0X2000;
		ar << wSwitch;

		if(m_pLine != nullptr)
		{
			const CLine::LINETYPE linetype = m_pLine->Gettype();
			ar << (BYTE)linetype;
		}
		if(m_pFill != nullptr)
		{
			const CFill::FILLTYPE filltype = m_pFill->Gettype();
			ar << (BYTE)filltype;
		}
		if(m_pSpot != nullptr)
		{
			const BYTE spottype = m_pSpot->Gettype();
			ar << spottype;
		}
		if(m_pThem != nullptr)
		{
			const BYTE themtype = m_pThem->Gettype();
			ar << themtype;
		}
	}
	else
	{
		ar >> m_wId;
		ar >> m_minLevelObj;
		ar >> m_maxLevelObj;
		ar >> m_minLevelTag;
		ar >> m_maxLevelTag;
		ar >> m_bDynamic;

		WORD wSwitch = 0;
		ar >> wSwitch;

		m_bVisible = (wSwitch & 0X0001) == 0X0001;
		m_bShowTag = (wSwitch & 0X0002) == 0X0002;
		m_bKeyQuery = (wSwitch & 0X0004) == 0X0004;
		m_bLocked = (wSwitch & 0X0008) == 0X0008;
		m_bDetour = (wSwitch & 0X0010) == 0X0010;
		m_bHide = (wSwitch & 0X0020) == 0X0020;
		m_bPivot = (wSwitch & 0X0040) == 0X0040;
		m_bAttribute = (wSwitch & 0X0080) == 0X0080;

		char lineindex = -1;
		char fillindex = -1;
		char spotindex = -1;
		const char themindex = -1;
		if((wSwitch & 0X0800) == 0X0800)
		{
			ar >> lineindex;
		}
		if((wSwitch & 0X1000) == 0X1000)
		{
			ar >> fillindex;
		}
		if((wSwitch & 0X2000) == 0X2000)
		{
			ar >> spotindex;
		}
		if((wSwitch & 0X2000) == 0X2000)
		{
			ar >> spotindex;
		}
		if(m_pLine != nullptr)
		{
			//			m_pLine->Decrease(nullptr);
			delete m_pLine;
			m_pLine = nullptr;
		}
		if(m_pFill != nullptr)
		{
			//			m_pFill->Decrease(nullptr);
			delete m_pFill;
			m_pFill = nullptr;
		}
		if(m_pSpot != nullptr)
		{
			//			m_pSpot->Decrease(nullptr);
			delete m_pSpot;
			m_pSpot = nullptr;
		}

		m_pLine = CLine::Apply(lineindex);
		m_pFill = CFillGeneral::Apply(fillindex);
		m_pSpot = CSpot::Apply(spotindex);
		m_pThem = CTheme::Apply(themindex);
		if((wSwitch & 0X4000) == 0X4000 && m_pType == nullptr)
		{
			m_pType = new CType;
		}
		if((wSwitch & 0X8000) == 0X8000 && m_pHint == nullptr)
		{
			m_pHint = new CHint;
		}
	}

	if(m_pLine != nullptr)
		m_pLine->ReleaseDCOM(ar);
	if(m_pFill != nullptr)
		m_pFill->ReleaseDCOM(ar);
	if(m_pSpot != nullptr)
		m_pSpot->ReleaseDCOM(ar);
	if(m_pType != nullptr)
		m_pType->ReleaseDCOM(ar);
	if(m_pHint != nullptr)
		m_pHint->ReleaseDCOM(ar);
	//	if(m_pThem != nullptr)
	//		m_pThem->ReleaseDCOM(ar);
}

void CLayer::ReleaseCap(CFile& file, const CDatainfo& datainfo) const
{
	if(m_bVisible == false)
		return;

	WORD wId = m_wId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(WORD));

	WORD wParent = this->Parent() == nullptr ? 0XFFFF : this->Parent()->m_wId;
	ReverseOrder(wParent);
	file.Write(&wParent, sizeof(WORD));

	SaveAsUTF8(file, m_strName);

	CRectF mapRect = datainfo.GetMapRect();
	double minLevelObj = ReverseOrder(m_minLevelObj);
	file.Write(&minLevelObj, sizeof(double));

	double maxLevelObj = ReverseOrder(m_maxLevelObj);
	file.Write(&maxLevelObj, sizeof(double));

	double minLevelTag = ReverseOrder(m_minLevelTag);
	file.Write(&minLevelTag, sizeof(double));

	double maxLevelTag = ReverseOrder(m_maxLevelTag);
	file.Write(&maxLevelTag, sizeof(double));

	const BYTE bDynamicSpot = (m_bDynamic & SpotDynamic) == SpotDynamic ? 0X01 : 0X00;
	const BYTE bDynamicLine = (m_bDynamic & LineDynamic) == LineDynamic ? 0X01 : 0X00;
	const BYTE bDynamicFill = (m_bDynamic & FillDynamic) == FillDynamic ? 0X01 : 0X00;
	const BYTE bDynamicType = (m_bDynamic & TypeDynamic) == TypeDynamic ? 0X01 : 0X00;
	file.Write(&bDynamicSpot, sizeof(BYTE));
	file.Write(&bDynamicLine, sizeof(BYTE));
	file.Write(&bDynamicFill, sizeof(BYTE));
	file.Write(&bDynamicType, sizeof(BYTE));

	WORD wSwitch = 0;
	wSwitch |= m_bVisible ? 0X0001 : 0X0000;
	wSwitch |= m_bShowTag ? 0X0002 : 0X0000;
	wSwitch |= m_bKeyQuery ? 0X0004 : 0X0000;
	wSwitch |= m_bLocked ? 0X0008 : 0X0000;
	wSwitch |= m_bDetour ? 0X0010 : 0X0000;
	wSwitch |= m_bHide ? 0X0020 : 0X0000;
	wSwitch |= m_bPivot ? 0X0040 : 0X0000;
	wSwitch |= m_bAttribute ? 0X0080 : 0X0000;

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CHint* pHint = this->GetHint();
	CTheme* pThem = this->GetTheme();
	if(pLine != nullptr) wSwitch |= 0X0100;
	if(pFill != nullptr) wSwitch |= 0X0200;
	if(pSpot != nullptr) wSwitch |= 0X0400;
	if(pType != nullptr) wSwitch |= 0X0800;
	if(pHint != nullptr) wSwitch |= 0X1000;
	if(pThem != nullptr) wSwitch |= 0X2000;

	ReverseOrder(wSwitch);
	file.Write(&wSwitch, sizeof(WORD));

	if(pLine != nullptr)
	{
		const CLine::LINETYPE linetype = pLine->Gettype();
		file.Write(&linetype, sizeof(BYTE));
	}
	if(pFill != nullptr)
	{
		const CFill::FILLTYPE filltype = pFill->Gettype();
		file.Write(&filltype, sizeof(BYTE));
	}
	if(pSpot != nullptr)
	{
		const BYTE spottype = pSpot->Gettype();
		file.Write(&spottype, sizeof(BYTE));
	}

	if(pLine != nullptr)
		pLine->ReleaseWeb(file);
	if(pFill != nullptr)
		pFill->ReleaseWeb(file);
	if(pSpot != nullptr)
		pSpot->ReleaseWeb(file);
	if(pType != nullptr)
		pType->ReleaseWeb(file);
	if(pHint != nullptr)
		pHint->ReleaseWeb(file);
	//	if(m_pThem != nullptr)
	//		m_pThem->ReleaseWeb(file);
}
void CLayer::ReleaseCap(BinaryStream& stream, const CDatainfo& datainfo) const
{
	if(m_bVisible == false)
		return;

	stream << m_wId;
	const WORD wParent = this->Parent() == nullptr ? 0XFFFF : this->Parent()->m_wId;
	stream << wParent;

	SaveAsUTF8(stream, m_strName);

	CRectF mapRect = datainfo.GetMapRect();
	stream << m_minLevelObj;
	stream << m_maxLevelObj;
	stream << m_minLevelTag;
	stream << m_maxLevelTag;
	const BYTE bDynamicSpot = (m_bDynamic & SpotDynamic) == SpotDynamic ? 0X01 : 0X00;
	const BYTE bDynamicLine = (m_bDynamic & LineDynamic) == LineDynamic ? 0X01 : 0X00;
	const BYTE bDynamicFill = (m_bDynamic & FillDynamic) == FillDynamic ? 0X01 : 0X00;
	const BYTE bDynamicType = (m_bDynamic & TypeDynamic) == TypeDynamic ? 0X01 : 0X00;
	stream << bDynamicSpot;
	stream << bDynamicLine;
	stream << bDynamicFill;
	stream << bDynamicType;

	WORD wSwitch = 0;
	wSwitch |= m_bVisible ? 0X0001 : 0X0000;
	wSwitch |= m_bShowTag ? 0X0002 : 0X0000;
	wSwitch |= m_bKeyQuery ? 0X0004 : 0X0000;
	wSwitch |= m_bLocked ? 0X0008 : 0X0000;
	wSwitch |= m_bDetour ? 0X0010 : 0X0000;
	wSwitch |= m_bHide ? 0X0020 : 0X0000;
	wSwitch |= m_bPivot ? 0X0040 : 0X0000;
	wSwitch |= m_bAttribute ? 0X0080 : 0X0000;

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CHint* pHint = this->GetHint();
	CTheme* pThem = this->GetTheme();
	if(pLine != nullptr) wSwitch |= 0X0100;
	if(pFill != nullptr) wSwitch |= 0X0200;
	if(pSpot != nullptr) wSwitch |= 0X0400;
	if(pType != nullptr) wSwitch |= 0X0800;
	if(pHint != nullptr) wSwitch |= 0X1000;
	if(pThem != nullptr) wSwitch |= 0X2000;

	stream << wSwitch;

	if(pLine != nullptr)
	{
		const CLine::LINETYPE linetype = pLine->Gettype();
		stream << linetype;
	}
	if(pFill != nullptr)
	{
		const CFill::FILLTYPE filltype = pFill->Gettype();
		stream << filltype;
	}
	if(pSpot != nullptr)
	{
		const BYTE spottype = pSpot->Gettype();
		stream << spottype;
	}

	if(pLine != nullptr)
		pLine->ReleaseWeb(stream);
	if(pFill != nullptr)
		pFill->ReleaseWeb(stream);
	if(pSpot != nullptr)
		pSpot->ReleaseWeb(stream);
	if(pType != nullptr)
		pType->ReleaseWeb(stream);
	if(pHint != nullptr)
		pHint->ReleaseWeb(stream);
	//	if(m_pThem != nullptr)
	//		m_pThem->ReleaseWeb(file);
}
void CLayer::ReleaseCap(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel) const
{
	json["Id"] = m_wId;
	json["Parent"] = this->Parent() == nullptr ? 0XFFFF : this->Parent()->m_wId;
	SaveAsEscapedASCII(json, "Name", m_strName);
	json["Geocoder"] = m_strGeoCatogery;
	if(m_bVisible)
	{
		CRectF mapRect = datainfo.GetMapRect();
		json["minLevelObj"] = m_minLevelObj;
		json["maxLevelObj"] = m_maxLevelObj;
		json["minLevelTag"] = m_minLevelTag;
		json["maxLevelTag"] = m_maxLevelTag;
		const BYTE bDynamicSpot = (m_bDynamic & SpotDynamic) == SpotDynamic ? 0X01 : 0X00;
		const BYTE bDynamicLine = (m_bDynamic & LineDynamic) == LineDynamic ? 0X01 : 0X00;
		const BYTE bDynamicFill = (m_bDynamic & FillDynamic) == FillDynamic ? 0X01 : 0X00;
		const BYTE bDynamicType = (m_bDynamic & TypeDynamic) == TypeDynamic ? 0X01 : 0X00;
		json["DynamicSpot"] = bDynamicSpot;
		json["DynamicLine"] = bDynamicLine;
		json["DynamicFill"] = bDynamicFill;
		json["DynamicType"] = bDynamicType;

		WORD wSwitch = 0;
		wSwitch |= m_bVisible ? 0X0001 : 0X0000;
		wSwitch |= m_bShowTag ? 0X0002 : 0X0000;
		wSwitch |= m_bKeyQuery ? 0X0004 : 0X0000;
		wSwitch |= m_bLocked ? 0X0008 : 0X0000;
		wSwitch |= m_bDetour ? 0X0010 : 0X0000;
		wSwitch |= m_bHide ? 0X0020 : 0X0000;
		wSwitch |= m_bPivot ? 0X0040 : 0X0000;
		wSwitch |= m_bAttribute ? 0X0080 : 0X0000;

		CSpot* pSpot = this->GetSpot();
		CLine* pLine = this->GetLine();
		CFill* pFill = this->GetFill();
		CType* pType = this->GetType();
		CHint* pHint = this->GetHint();
		CTheme* pThem = this->GetTheme();
		if(pLine != nullptr) wSwitch |= 0X0100;
		if(pFill != nullptr) wSwitch |= 0X0200;
		if(pSpot != nullptr) wSwitch |= 0X0400;
		if(pType != nullptr) wSwitch |= 0X0800;
		if(pHint != nullptr) wSwitch |= 0X1000;
		if(pThem != nullptr) wSwitch |= 0X2000;

		json["Wswitch"] = wSwitch;
		if(pLine != nullptr)
		{
			boost::json::object child;
			const CLine::LINETYPE type = pLine->Gettype();
			child["Type"] = type;
			pLine->ReleaseWeb(child);
			json["Line"] = child;
		}
		if(pFill != nullptr)
		{
			boost::json::object child;
			const CFill::FILLTYPE type = pFill->Gettype();
			child["Type"] = type;
			pFill->ReleaseWeb(child);
			json["Fill"] = child;
		}
		if(pSpot != nullptr)
		{
			boost::json::object child;
			const BYTE type = pSpot->Gettype();
			child["Type"] = type;
			pSpot->ReleaseWeb(child);
			json["Spot"] = child;
		}

		if(pType != nullptr)
		{
			boost::json::object child;
			pType->ReleaseWeb(child);
			json["Font"] = child;
		}
		if(pHint != nullptr)
		{
			boost::json::object child;
			pHint->ReleaseWeb(child);
			json["Hint"] = child;
		}
		//	if(m_pThem != nullptr)
		//		m_pThem->ReleaseWeb(file);
	}
}
void CLayer::ReleaseCap(pbf::writer& writer, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, bool noFollowing) const
{
	writer.add_variant_uint16(m_wId);
	writer.add_variant_uint16(this->Parent() == nullptr ? 0XFFFF : this->Parent()->m_wId);
	writer.add_string(EscapedASCII(m_strName));
	writer.add_string(m_strGeoCatogery);

	writer.add_bool(m_bVisible);
	if(m_bVisible)
	{
		CRectF mapRect = datainfo.GetMapRect();
		writer.add_double(m_minLevelObj);
		writer.add_double(m_maxLevelObj);
		writer.add_double(m_minLevelTag);
		writer.add_double(m_maxLevelTag);
		const BYTE bDynamicSpot = (m_bDynamic & SpotDynamic) == SpotDynamic ? 0X01 : 0X00;
		const BYTE bDynamicLine = (m_bDynamic & LineDynamic) == LineDynamic ? 0X01 : 0X00;
		const BYTE bDynamicFill = (m_bDynamic & FillDynamic) == FillDynamic ? 0X01 : 0X00;
		const BYTE bDynamicType = (m_bDynamic & TypeDynamic) == TypeDynamic ? 0X01 : 0X00;
		writer.add_bool(bDynamicSpot);
		writer.add_bool(bDynamicLine);
		writer.add_bool(bDynamicFill);
		writer.add_bool(bDynamicType);

		WORD wSwitch = 0;
		wSwitch |= m_bVisible ? 0X0001 : 0X0000;
		wSwitch |= m_bShowTag ? 0X0002 : 0X0000;
		wSwitch |= m_bKeyQuery ? 0X0004 : 0X0000;
		wSwitch |= m_bLocked ? 0X0008 : 0X0000;
		wSwitch |= m_bDetour ? 0X0010 : 0X0000;
		wSwitch |= m_bHide ? 0X0020 : 0X0000;
		wSwitch |= m_bPivot ? 0X0040 : 0X0000;
		wSwitch |= m_bAttribute ? 0X0080 : 0X0000;

		CSpot* pSpot = this->GetSpot();
		CLine* pLine = this->GetLine();
		CFill* pFill = this->GetFill();
		CType* pType = this->GetType();
		CHint* pHint = this->GetHint();
		CTheme* pThem = this->GetTheme();
		wSwitch |= pLine != nullptr ? 0X0100 : 0X0000;
		wSwitch |= pFill != nullptr ? 0X0200 : 0X0000;
		wSwitch |= pSpot != nullptr ? 0X0400 : 0X0000;
		wSwitch |= pType != nullptr ? 0X0800 : 0X0000;
		wSwitch |= pHint != nullptr ? 0X1000 : 0X0000;
		wSwitch |= pThem != nullptr ? 0X2000 : 0X0000;

		writer.add_fixed_uint16(wSwitch);
		if(pLine != nullptr)
		{
			const CLine::LINETYPE type = pLine->Gettype();
			writer.add_byte(type);
			pLine->ReleaseWeb(writer);
		}
		if(pFill != nullptr)
		{
			const CFill::FILLTYPE type = pFill->Gettype();
			writer.add_byte(type);
			pFill->ReleaseWeb(writer);
		}
		if(pSpot != nullptr)
		{
			const BYTE type = pSpot->Gettype();
			writer.add_byte(type);
			pSpot->ReleaseWeb(writer);
		}

		if(pType != nullptr)
		{
			pType->ReleaseWeb(writer);
		}
		if(pHint != nullptr)
		{
			pHint->ReleaseWeb(writer);
		}
		//	if(m_pThem != nullptr)
		//		m_pThem->ReleaseWeb(file);
		if(noFollowing == true)
		{
			writer.add_variant_uint32(0);
		}
	}
}
void CLayer::ReleaseWeb(CFile& file, const CDatainfo& datainfo) const
{
	if(m_bVisible == false)
		return;

	this->ReleaseCap(file, datainfo);

	int count = m_geomlist.GetCount();
	ReverseOrder(count);
	file.Write(&count, sizeof(int));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(file);
	}
}
void CLayer::ReleaseWeb(BinaryStream& stream, const CDatainfo& datainfo) const
{
	if(m_bVisible == false)
		return;

	this->ReleaseCap(stream, datainfo);
	const int count = m_geomlist.GetCount();
	stream << count;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(stream);
	}
}
void CLayer::ReleaseWeb(boost::json::object& json, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, CDatabase* pDatabase, CString strSQL, CString strIdField) const
{
	this->ReleaseCap(json, datainfo, minMapLevel, maxMapLevel);

	if(m_bVisible)
	{
		boost::json::array child1;
		CSize offset(0, 0);
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			boost::json::object child2;
			pGeom->ReleaseWeb(child2, datainfo, offset, pDatabase, strSQL, strIdField);
			child1.push_back(child2);
		}
		json["Geoms"] = child1;
	}
}
void CLayer::ReleaseWeb(pbf::writer& writer, pbf::tag tag, const CDatainfo& datainfo, const BYTE& minMapLevel, const BYTE& maxMapLevel, CDatabase* pDatabase, CString strSQL, CString strIdField) const
{
	writer.add_tag(tag, pbf::type::bytes);
	this->ReleaseCap(writer, datainfo, minMapLevel, maxMapLevel, false);
	if(m_bVisible)
	{
		unsigned int count = m_geomlist.GetCount();
		writer.add_variant_uint32(count);

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			pGeom->ReleaseWeb(writer);
			pGeom->ReleaseWeb(writer, pDatabase, strSQL, strIdField);
		}
	}
}
void CLayer::Tilize(CVectorTile** matrixTiles, int minRow, int maxRow, int minCol, int maxCol, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bShowTag == false || viewinfo.m_levelCurrent < m_minLevelTag || viewinfo.m_levelCurrent >= m_maxLevelTag) ? false : true;
	if(bDrawGeom == false && bDrawTag == false)
		return;

	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	const bool bStroke = (pLine == nullptr || pLine->Gettype() == CLine::LINETYPE::Blank) ? false : true;
	const bool bFill = (pFill == nullptr || pFill->Gettype() == CFill::FILLTYPE::Blank) ? false : true;
	const Gdiplus::Rect vewRect(minCol * 256, minRow * 256, (maxCol + 1) * 256, (maxRow + 1) * 256);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom1 = m_geomlist.GetNext(pos);
		if(pGeom1->IsVisible(viewinfo.m_levelCurrent) == false)
			continue;
		if(pGeom1->IsIn(viewinfo, vewRect) == false)
			continue;

		CGeom* pGeom2 = pGeom1->Clone();
		if(pGeom2->m_bType == 8 || pGeom2->m_bType == 9)
		{
			pGeom2->m_Rect = this->GetGeomDocBoundary(viewinfo, pGeom2);
		}
		pGeom2->Simplify(viewinfo.m_datainfo.m_dilationMapToDoc / viewinfo.m_xFactorMapToView);
		pGeom2->Transform(viewinfo);
		std::list<CGeom*> subs;
		pGeom2->Correct(1, subs);
		if(subs.size() > 0)
		{
			for(auto sub : subs)
			{
				std::list<std::tuple<int, int, CGeom*>> cellgeoms = sub->Swim(bStroke, bFill, 256, 256, minRow, maxRow, minCol, maxCol);
				for(auto& it : cellgeoms)
				{
					const int col = std::get<0>(it);
					const int row = std::get<1>(it);
					CGeom* pCelled = std::get<2>(it);
					if(row >= minRow && row <= maxRow && col >= minCol && col <= maxCol)
					{
						std::list<CGeom*>& layergeomlist = matrixTiles[col - minCol][row - minRow].GetGeomList(m_wId);
						layergeomlist.push_back(pCelled);
					}
					else
					{
						delete pCelled;
						pCelled = nullptr;
					}
				}
				cellgeoms.clear();

				delete sub;
			}
		}
		else
		{
			std::list<std::tuple<int, int, CGeom*>> cellgeoms = pGeom2->Swim(bStroke, bFill, 256, 256, minRow, maxRow, minCol, maxCol);
			for(auto& it : cellgeoms)
			{
				const int col = std::get<0>(it);
				const int row = std::get<1>(it);
				CGeom* pCelled = std::get<2>(it);
				if(row >= minRow && row <= maxRow && col >= minCol && col <= maxCol)
				{
					std::list<CGeom*>& layergeomlist = matrixTiles[col - minCol][row - minRow].GetGeomList(m_wId);
					layergeomlist.push_back(pCelled);
				}
				else
				{
					delete pCelled;
					pCelled = nullptr;
				}
			}
			cellgeoms.clear();
		}
		delete pGeom2;
		pGeom2 = nullptr;
	}
}

void CLayer::Invalid(CDocument& document) const
{
	document.UpdateAllViews(nullptr);
}

void CLayer::Invalidate(CDocument& document, const CGeom* pGeom) const
{
	POSITION pos = document.GetFirstViewPosition();
	{
		CView* pView = document.GetNextView(pos);
		CViewinfo* pViewinfo = (CViewinfo*)pView->SendMessage(WM_GETVIEWINFO, 0, 0);
		if(pViewinfo != nullptr)
		{
			this->Invalidate(pView, *pViewinfo, pGeom);
		}
	}
}

void CLayer::Invalidate(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom) const
{
	if(pGeom == nullptr)
		return;

	const Gdiplus::SizeF inflation = this->GetVewInflation(viewinfo, pGeom);
	pGeom->Invalidate(pWnd, viewinfo, max(inflation.Width, inflation.Height));
	if(pGeom->HasTag())
	{
		pGeom->InvalidateTag(pWnd, viewinfo, this->GetHint());
	}
}

void CLayer::InvalidateAnchor(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom, const unsigned int& nAnchor) const
{
	if(pGeom == nullptr)
		return;
	const Gdiplus::SizeF inflation = GetVewInflation(viewinfo, pGeom);
	pGeom->InvalidateAnchor(pWnd, viewinfo, nAnchor, inflation.Width);
}

void CLayer::InvalidateSegment(CWnd* pWnd, const CViewinfo& viewinfo, const CGeom* pGeom, const unsigned int& nAnchor) const
{
	if(pGeom == nullptr)
		return;
	const Gdiplus::SizeF inflation = this->GetVewInflation(viewinfo, pGeom);
	pGeom->InvalidateSegment(pWnd, viewinfo, nAnchor, inflation.Width);
}

void CLayer::StyleChanged(CWnd* pView, const CViewinfo& viewinfo) const
{
	if(m_bVisible == false)
		return;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
			continue;

		this->Invalidate(pView, viewinfo, pGeom);
	}
}

void CLayer::SpotChanged(CWnd* pView, const CViewinfo& viewinfo) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
		{
			if(m_bVisible == true)
			{
				this->Invalidate(pView, viewinfo, pGeom);
			}
			((CMark*)pGeom)->CalCorner(this->GetSpot(), viewinfo.m_datainfo, m_tree.m_library);
			if(m_bVisible == true)
			{
				this->Invalidate(pView, viewinfo, pGeom);
			}
		}
	}
}

void CLayer::TypeChanged(CWnd* pView, const CViewinfo& viewinfo) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) && pGeom->m_pType == nullptr)
		{
			if(m_bVisible == true)
				this->Invalidate(pView, viewinfo, pGeom);
			((CText*)pGeom)->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
			if(m_bVisible == true)
				this->Invalidate(pView, viewinfo, pGeom);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) && pGeom->m_pType == nullptr && m_bVisible == true)
		{
			this->Invalidate(pView, viewinfo, pGeom);
		}
	}
}

void CLayer::HintChanged(CWnd* pView, const CViewinfo& viewinfo) const
{
	if(m_bVisible == true)
	{
		CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			pGeom->InvalidateTag(pView, viewinfo, pHint);
		}
	}
}

void CLayer::Regress(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;
		else if(pGeom->IsDissident() == true)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);

			pGeom->Regress(pWnd, m_tree.m_library);

			if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)) == TRUE)
			{
				CMark* mark = (CMark*)pGeom;
				mark->CalCorner(this->GetSpot(), viewinfo.m_datainfo, m_tree.m_library);
			}
			else if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) == TRUE)
			{
				CText* text = (CText*)pGeom;
				text->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
			}

			this->Invalidate(pWnd, viewinfo, pGeom);
		}
	}
}

void CLayer::Relieve()
{
	if(m_pLine != nullptr)
	{
		m_pLine->Decrease(m_tree.m_library);
	}
	if(m_pFill != nullptr)
	{
		m_pFill->Decrease(m_tree.m_library);
	}
	if(m_pSpot != nullptr)
	{
		m_pSpot->Decrease(m_tree.m_library);
	}
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Regress(nullptr, m_tree.m_library);
	}

	CLayer* pChild = m_firstchild;
	while(pChild != nullptr)
	{
		pChild->Relieve();
		pChild = pChild->m_nextsibling;
	}
}
void CLayer::ExtremizeScale(const float& oldMinimum, const float& oldMaximum, const float& newMinimum, const float& newMaximum)
{
	if(this->m_minLevelObj == oldMinimum)
		this->m_minLevelObj = newMinimum;
	if(this->m_maxLevelObj == oldMaximum)
		this->m_maxLevelObj = newMaximum;
	if(this->m_minLevelTag == oldMinimum)
		this->m_minLevelTag = newMinimum;
	if(this->m_maxLevelTag == oldMaximum)
		this->m_maxLevelTag = newMaximum;
}
void CLayer::RecalRect()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->RecalRect();
	}
}
void CLayer::RecalRect(const CDatainfo& datainfo)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CMark)))
		{
			((CMark*)pGeom)->CalCorner(this->GetSpot(), datainfo, m_tree.m_library);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CText)) && pGeom->m_pType == nullptr)
		{
			((CText*)pGeom)->CalCorner(this->GetType(), datainfo.m_zoomPointToDoc, 1.f);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) && pGeom->m_pType == nullptr && m_bVisible == true)
		{
		}
	}
}

void CLayer::SetupAnno(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;

	if(m_bLocked == true)
	{
		AfxMessageBox(IDS_LAYERLOCKEDNOINFO);
		return;
	}

	if(m_nActiveCount > 1)
	{
		HINSTANCE hInst = ::LoadLibrary(_T("Geometry.dll"));
		const HINSTANCE hInstOld = AfxGetResourceHandle();
		AfxSetResourceHandle(hInst);

		CString string;
		CTextEditDlg dlg(pWnd, string);
		if(dlg.DoModal() == IDOK)
		{
			string = dlg.m_String;

			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
		}
		else
		{
			AfxSetResourceHandle(hInstOld);
			::FreeLibrary(hInst);
			return;
		}

		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			if(pGeom->m_bActive == false)
				continue;

			pGeom->m_strName = string;
			if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
			{
				CText* text = (CText*)pGeom;
				this->Invalidate(pWnd, viewinfo, text);
				text->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
				this->Invalidate(pWnd, viewinfo, text);
				return;
			}
		}
	}
	else
	{
		CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			if(pGeom->m_bActive == false)
				continue;

			pGeom->Information(pWnd, viewinfo, pHint);

			if(pGeom->IsKindOf(RUNTIME_CLASS(CText)))
			{
				CText* text = (CText*)pGeom;
				this->Invalidate(pWnd, viewinfo, text);
				text->CalCorner(this->GetType(), viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);
				this->Invalidate(pWnd, viewinfo, text);
				return;
			}
		}
	}
	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::ClearAnno(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(pGeom->HasTag())
		{
			pGeom->InvalidateTag(pWnd, viewinfo, pHint);
			pGeom->DeleteTag();
		}
		pGeom->m_strName.Empty();
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::GeomAttribute(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		this->Invalidate(pWnd, viewinfo, pGeom);
		pGeom->Attribute(pWnd, viewinfo);
		this->Invalidate(pWnd, viewinfo, pGeom);
	}
	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::GeomName(CWnd* pWnd, const CDatainfo& datainfo, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	CType* pType = this->GetType();
	if(pType == nullptr)
		return;
	const float fontSize = pType->m_fSize * viewinfo.m_sizeDPI.cx / 72.f;

	CClientDC dc(pWnd);

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == false)
			continue;

		PreInactivateGeom(datainfo, pGeom);

		pGeom->m_bActive = false;
		pGeom->DrawAnchors(&dc, viewinfo);
		m_nActiveCount--;

		CObList oblist;
		if(pGeom->CreateName(pWnd, fontSize, viewinfo.m_datainfo.m_zoomPointToDoc, oblist) == false)
			continue;

		POSITION pos = oblist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pName = (CGeom*)oblist.GetNext(pos);
			m_geomlist.AddTail(pName);
			pName->m_bActive = true;
			m_nActiveCount++;

			if(pName->IsKindOf(RUNTIME_CLASS(CText)))
				((CText*)pName)->CalCorner(pType, viewinfo.m_datainfo.m_zoomPointToDoc, 1.f);

			pName->Invalidate(pWnd, viewinfo, 3);
		}

		SetModifiedFlag(true);
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::CreateOptimizedTag(CArray<Gdiplus::RectF, Gdiplus::RectF&>& OccupiedRect)
{
	if(m_nActiveCount == 0)
		return;
	CHint* pHint = this->GetHint();
	if(pHint == nullptr)
		return;
	const int fontSize = pHint->m_fSize * 10;
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(pGeom->m_bActive == false)
			continue;
		else if(pGeom->HasTag())
			continue;
		else if(pGeom->m_strName.IsEmpty() == FALSE)
		{
			if(pGeom->m_bType == 8)
			{
				pGeom->CreateOptimizedTag(OccupiedRect, fontSize);
			}
			else
			{
			}
		}
	}
}

void CLayer::CreateTag(CWnd* pWnd, const CViewinfo& viewinfo, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	if(m_nActiveCount == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->HasTag())
			continue;

		pGeom->CreateTag(anchor, hAlign, vAlign);
		pGeom->InvalidateTag(pWnd, viewinfo, pHint);

		SetModifiedFlag(true);
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::SetupTag(CWnd* pWnd, const CViewinfo& viewinfo, const ANCHOR& anchor, const HALIGN& hAlign, const VALIGN& vAlign)
{
	if(m_nActiveCount == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(pGeom->HasTag())
			pGeom->InvalidateTag(pWnd, viewinfo, pHint);
		pGeom->SetupTag(anchor, hAlign, vAlign);
		if(pGeom->HasTag())
			pGeom->InvalidateTag(pWnd, viewinfo, pHint);

		SetModifiedFlag(true);
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::DeleteTag(CWnd* pWnd, const CViewinfo& viewinfo)
{
	if(m_nActiveCount == 0)
		return;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(pGeom->HasTag())
		{
			pGeom->InvalidateTag(pWnd, viewinfo, pHint);
			pGeom->DeleteTag();

			SetModifiedFlag(true);
		}
	}

	pWnd->SendMessage(WM_DOCMODIFIED, 0, TRUE);
}

void CLayer::SetAttribute(CDocument& document)
{
	CLayerAttrDlg dlg(nullptr, m_wId, m_bViewLevel, m_minLevelObj, m_maxLevelObj, m_minLevelTag, m_maxLevelTag, m_bKeyQuery, m_bLocked, m_bHide, m_bShowTag, m_bDynamic, m_bDetour, m_bPivot, m_bCentroid, m_strGeoCatogery, ​m_bTagResistance == 0XFF);
	if(dlg.DoModal() == IDOK)
	{
		m_strGeoCatogery = dlg.m_strGeoCatogery;
		m_bViewLevel = dlg.m_dwLevel;
		m_minLevelObj = dlg.m_minLevelObj;
		m_maxLevelObj = dlg.m_maxLevelObj;
		m_minLevelTag = dlg.m_minLevelTag;
		m_maxLevelTag = dlg.m_maxLevelTag;
		m_bKeyQuery = dlg.m_bKeyQuery == TRUE;
		m_bLocked = dlg.m_bLocked == TRUE;
		m_bHide = dlg.m_bHide == TRUE;
		m_bShowTag = dlg.m_bShowTag == TRUE;
		m_bDetour = dlg.m_bDetour == TRUE;
		m_bPivot = dlg.m_bPivot == TRUE;
		m_bCentroid = dlg.m_bCentroid == TRUE;
		m_bCentroid = dlg.m_bCentroid == TRUE;
		​m_bTagResistance = dlg.m_bTagResistance ? 0XFF : 0X00;

		m_bDynamic = 0X00;
		m_bDynamic |= dlg.m_bSpotDynamic == true ? SpotDynamic : 0X00;
		m_bDynamic |= dlg.m_bLineDynamic == true ? LineDynamic : 0X00;
		m_bDynamic |= dlg.m_bFillDynamic == true ? FillDynamic : 0X00;
		m_bDynamic |= dlg.m_bTypeDynamic == true ? TypeDynamic : 0X00;

		this->Invalid(document);
		document.SetModifiedFlag(TRUE);
	}
}

void CLayer::PathReverse(CWnd* pWnd, const CViewinfo& viewinfo, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
			continue;
		const int times = pGeom->IsKindOf(RUNTIME_CLASS(CBezier)) ? 3 : 1;
		CPoint* pPoints = new CPoint[((CPoly*)pGeom)->m_nAnchors * times];
		memcpy(pPoints, ((CPoly*)pGeom)->m_pPoints, sizeof(CPoint) * ((CPoly*)pGeom)->m_nAnchors * times);
		const std::pair<unsigned int, CPoint*> original(((CPoly*)pGeom)->m_nAnchors, pPoints);
		((CPoly*)pGeom)->Reverse();
		const std::pair<unsigned int, CPoint*> modified(((CPoly*)pGeom)->m_nAnchors, ((CPoly*)pGeom)->m_pPoints);

		this->Invalidate(pWnd, viewinfo, pGeom);
		SetModifiedFlag(true);

		Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>* pModify = new Undoredo::CModify<CWnd*, CPoly*, std::pair<unsigned int, CPoint*>>(nullptr, ((CPoly*)pGeom), original, modified);
		pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Undo_Modify_Points;
		pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Redo_Modify_Points;
		pModify->free = Undoredo::Document::LayerTree::Layer::Geomlist::Poly::Free_Modify_Points;
		actionstack.Record(pModify);
	}
}

void CLayer::PolyToBezier(CWnd* pWnd, const CViewinfo& viewinfo, const float& scale, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	actionstack.Start();
	POSITION pos1;
	POSITION pos = m_geomlist.GetHeadPosition();
	while((pos1 = pos) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CBezier)) == TRUE)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CTextPoly)) == TRUE)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == false)
			continue;

		CPoly* poly = (CPoly*)pGeom;
		CBezier* bezier = poly->Bezierize(scale);
		if(bezier != nullptr)
		{
			int index = GetIndexFromOBList(m_geomlist, pGeom);
			m_geomlist.SetAt(pos1, bezier);

			Undoredo::CModify<CWnd*, std::pair<CLayer*, unsigned int>, CGeom*>* pModify = new Undoredo::CModify<CWnd*, std::pair<CLayer*, unsigned int>, CGeom*>(pWnd, std::pair<CLayer*, unsigned int>(this, index), pGeom, bezier);
			pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Replace;
			pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Replace;
			pModify->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_Modify_Replace;
			actionstack.Record(pModify);

			this->Invalidate(pWnd, viewinfo, bezier);
			SetModifiedFlag(true);
		}
	}
	actionstack.Stop();
}

void CLayer::Densify(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	actionstack.Start();
	CClientDC dc(pWnd);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
			((CPoly*)pGeom)->Densify(tolerance, actionstack);
			pGeom->DrawAnchors(&dc, viewinfo);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup)) == TRUE)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
			((CGroup*)pGeom)->Densify(tolerance, actionstack);
			pGeom->DrawAnchors(&dc, viewinfo);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CCluster)) == TRUE)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
			((CCluster*)pGeom)->Densify(tolerance, actionstack);
			pGeom->DrawAnchors(&dc, viewinfo);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
	}
	actionstack.Stop();
}

void CLayer::Simplify(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
			((CPoly*)pGeom)->Simplify(tolerance, actionstack);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CGroup)) == TRUE)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
			((CGroup*)pGeom)->Simplify(tolerance, actionstack);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
		else if(pGeom->IsKindOf(RUNTIME_CLASS(CDoughnut)) == TRUE)
		{
			this->Invalidate(pWnd, viewinfo, pGeom);
			((CDoughnut*)pGeom)->Simplify(tolerance, actionstack);
			this->Invalidate(pWnd, viewinfo, pGeom);
			SetModifiedFlag(true);
		}
	}
}

void CLayer::PathBuffer(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int distance, const unsigned int tolerance, const Clipper2Lib::JoinType& jointype, const Clipper2Lib::EndType& endtype, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == FALSE)
			continue;

		CGeom* buffer = ((CPoly*)pGeom)->Buffer(distance, tolerance, jointype, endtype);
		if(buffer != nullptr)
		{
			pGeom->CGeom::CopyTo(buffer, true);
			std::list<std::pair<CGeom*, unsigned int>> geoms;
			int index = m_geomlist.GetCount();
			geoms.push_back(std::pair<CGeom*, unsigned int>(buffer, index));
			Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>* pAppend = new Undoredo::CAppend<CWnd*, CLayer*, std::list<std::pair<CGeom*, unsigned int>>>(pWnd, this, geoms);
			pAppend->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Remove_Geoms;
			pAppend->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Append_Geoms;
			pAppend->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_AppendRemove_Geoms;
			actionstack.Record(pAppend);

			m_geomlist.AddHead(buffer);
			this->Invalidate(pWnd, viewinfo, buffer);
			SetModifiedFlag(true);
		}
	}
}

void CLayer::BezierToPoly(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	actionstack.Start();
	POSITION pos1;
	POSITION pos = m_geomlist.GetHeadPosition();
	while((pos1 = pos) != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPath)) == FALSE)
			continue;

		CPoly* poly = ((CPath*)pGeom)->Polylize(tolerance);
		if(poly != nullptr)
		{
			int index = GetIndexFromOBList(m_geomlist, pGeom);
			m_geomlist.SetAt(pos1, poly);

			Undoredo::CModify<CWnd*, std::pair<CLayer*, unsigned int>, CGeom*>* pModify = new Undoredo::CModify<CWnd*, std::pair<CLayer*, unsigned int>, CGeom*>(pWnd, std::pair<CLayer*, unsigned int>(this, index), pGeom, poly);
			pModify->undo = Undoredo::Document::LayerTree::Layer::Geomlist::Undo_Modify_Replace;
			pModify->redo = Undoredo::Document::LayerTree::Layer::Geomlist::Redo_Modify_Replace;
			pModify->free = Undoredo::Document::LayerTree::Layer::Geomlist::Free_Modify_Replace;
			actionstack.Record(pModify);

			SetModifiedFlag(true);
			this->Invalidate(pWnd, viewinfo, poly);
		}
	}
	actionstack.Stop();
}

void CLayer::Close(CWnd* pWnd, const CViewinfo& viewinfo, const unsigned int& tolerence, Undoredo::CActionStack& actionstack)
{
	if(m_nActiveCount == 0)
		return;
	if(m_bLocked == true)
		return;

	actionstack.Start();
	CClientDC dc(pWnd);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;
		if(pGeom->m_bLocked == true)
			continue;

		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			pGeom->DrawAnchors(&dc, viewinfo);
			if(((CPoly*)pGeom)->Close(false, tolerence, &actionstack) == true)
			{
				pGeom->DrawAnchors(&dc, viewinfo);
				this->Invalidate(pWnd, viewinfo, pGeom);
				pGeom->DrawAnchors(&dc, viewinfo);
			}
			SetModifiedFlag(true);
		}
	}
	actionstack.Stop();
}

void CLayer::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible == false)
		return;

	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	const bool bDrawTag = (m_bShowTag == false || viewinfo.m_levelCurrent < m_minLevelTag || viewinfo.m_levelCurrent >= m_maxLevelTag) ? false : true;
	if(bDrawGeom == false && bDrawTag == false)
		return;

	RecalculateTemperyVariables(viewinfo);

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	Context context;
	context.pSpot = pSpot;
	context.pLine = pLine;
	context.pFill = pFillGeneral;
	context.pType = pType;
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioLine = (m_bDynamic & LineDynamic) == LineDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioFill = (m_bDynamic & FillDynamic) == FillDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.tolerance = viewinfo.GetPrecision(10);

	Gdiplus::Pen* pen = pLine == nullptr ? nullptr : pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
	Gdiplus::Brush* brush = pFill == nullptr ? nullptr : pFill->GetBrush(m_tree.m_library, context.ratioFill, viewinfo.m_sizeDPI);

	Gdiplus::Font* font = nullptr;
	Gdiplus::SolidBrush* textbrush = nullptr;
	if(pType != nullptr)
	{
		font = pType->GetFont(context.ratioType, viewinfo.m_sizeDPI);
		const Gdiplus::Color color = pType->GetColor();
		textbrush = ::new Gdiplus::SolidBrush(color);
	}

	context.penStroke = pen;
	context.brushFill = brush;
	context.font = font;
	context.textbrush = textbrush;

	bool bDrawTheme = false;
	std::map<DWORD, CGeom*> attgeomlist;
	CDatabase* pDatabase = nullptr;
	CATTDatasource* pDatasource = nullptr;
	if(m_pThem != nullptr)
	{
		pDatasource = this->GetAttDatasource();
		if(pDatasource != nullptr)
		{
			pDatabase = m_tree.GetAttDatabase(pDatasource->m_strDatabase);
			bDrawTheme = pDatabase != nullptr;
		}
	}
	CRect extended = InflateRect(invalidRect, ((double)invalidRect.right - (double)invalidRect.left) / 2.f, ((double)invalidRect.bottom - (double)invalidRect.top) / 2.f);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		if(bDrawTheme == true && pGeom->m_attId != 0XFFFFFFFF)
		{
			if(m_pThem->Category == CTheme::Area)
			{
				if(pGeom->m_bClosed == true)
				{
					attgeomlist[pGeom->m_attId] = pGeom;
					continue;
				}
			}
			else
				attgeomlist[pGeom->m_attId] = pGeom;
		}

		if(bDrawGeom == true)
		{
			pGeom->Draw(g, viewinfo, m_tree.m_library, context);
			if(pGeom->m_bType == 8)
			{
				pGeom->m_Rect = boundary;
			}
			else if(pGeom->m_bType == 9)
			{
				pGeom->m_Rect = boundary;
			}
		}
		if(bDrawTag == true && viewinfo.m_bViewer == true && pGeom->m_strName.IsEmpty() == FALSE)
		{
			switch(pGeom->m_bType)
			{
				case 8:
					{
						const CMark* mark = (CMark*)pGeom;
						CSpot* pSpot = mark->m_pSpot != nullptr ? mark->m_pSpot : m_pSpot;
						if(pSpot != nullptr)
						{
							Gdiplus::RectF cliRect = viewinfo.DocToClient <Gdiplus::RectF>(boundary);
							d_oRectArray.Add(cliRect);

							CRectTag* pTag = new CRectTag;
							pTag->m_rect = cliRect;
							pTag->m_string = pGeom->m_strName;
							if(pGeom->m_pTag != nullptr)
							{
								pTag->defAnchor = pGeom->m_pTag->GetAnchor();
							}
							m_rectTaglist.AddTail(pTag);
						}
					}
					break;
				case 9:
				case 10:
					break;
				case 1:
				case 2:
				case 7:
				case 12:
				case 13:
					{
						CGroupTag* pTag = CGroupTag::GetGroup(m_groupTaglist, pGeom);
						if(pTag == nullptr || pTag->m_bDelete == true)
						{
							pTag = new CGroupTag;
							pTag->m_string = pGeom->m_strName;
							pTag->m_bDelete = false;
							m_groupTaglist.AddTail(pTag);
						}
						pTag->m_geomlist.AddTail(pGeom);
					}
					break;
				default:
					{
						CPointTag* pTag = new CPointTag;
						CPoint docPoint = pGeom->GetLabeloid();
						pTag->m_point = viewinfo.DocToClient<Gdiplus::PointF>(docPoint);
						pTag->m_string = pGeom->m_strName;
						m_pointTaglist.AddTail(pTag);
					}
					break;
			}
		}
	}

	if(bDrawTheme == true)
	{
		switch(m_pThem->Category)
		{
			case CTheme::Area:
				((CChoropleth*)m_pThem)->Draw(g, *pDatabase, *pDatasource, viewinfo, pen, attgeomlist);
				break;
			default:
				m_pThem->Draw(g, *pDatabase, *pDatasource, viewinfo, attgeomlist);
				break;
		}
	}
	attgeomlist.clear();

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
	::delete font;
	font = nullptr;
	::delete textbrush;
	textbrush = nullptr;
}
void CLayer::DrawTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, const bool showCreated, const bool showDynamic) const
{
	if(m_bVisible == false)
		return;
	else if(m_bShowTag == false)
		return;
	else if(viewinfo.m_levelCurrent < m_minLevelTag)
		return;
	else if(viewinfo.m_levelCurrent > m_maxLevelTag)
		return;

	float scaleMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	float scaleHint = (m_bDynamic & HintDynamic) == HintDynamic ? viewinfo.CurrentRatio() : 1.0f;

	CHint* pHint = this->GetHint() != nullptr ? this->GetHint() : CHint::Default();
	if(viewinfo.m_bViewer == true)
	{
		POSITION pos1 = m_groupTaglist.GetTailPosition();
		while(pos1 != nullptr)
		{
			CGroupTag* pTag = m_groupTaglist.GetPrev(pos1);
			if(pTag != nullptr)
			{
				pTag->Draw(g, viewinfo, pHint, m_bPivot);
				delete pTag;
				pTag = nullptr;
			}
		}
		m_groupTaglist.RemoveAll();

		POSITION pos2 = m_pointTaglist.GetHeadPosition();
		while(pos2 != nullptr)
		{
			CPointTag* pTag = m_pointTaglist.GetNext(pos2);
			if(pTag != nullptr)
			{
				pTag->Draw(g, viewinfo, pHint);
				delete pTag;
				pTag = nullptr;
			}
		}
		m_pointTaglist.RemoveAll();

		POSITION Pos3 = m_fixedTaglist.GetHeadPosition();
		while(Pos3 != nullptr)
		{
			CFixedTag* pTag = m_fixedTaglist.GetNext(Pos3);
			if(pTag != nullptr)
			{
				pTag->Draw(g, viewinfo, pHint);
				delete pTag;
				pTag = nullptr;
			}
		}
		m_fixedTaglist.RemoveAll();

		POSITION Pos4 = m_rectTaglist.GetHeadPosition();
		while(Pos4 != nullptr)
		{
			CRectTag* pTag = m_rectTaglist.GetNext(Pos4);
			if(pTag != nullptr)
			{
				pTag->Draw(g, viewinfo, pHint);
				delete pTag;
				pTag = nullptr;
			}
		}
		m_rectTaglist.RemoveAll();
	}
	else
	{
		CSpot* pSpot = this->GetSpot();
		CLine* pLine = this->GetLine();
		CFill* pFill = this->GetFill();
		CType* pType = this->GetType();
		CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

		Context context;
		context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
		context.ratioLine = (m_bDynamic & LineDynamic) == LineDynamic ? viewinfo.CurrentRatio() : 1.0f;
		context.ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
		context.pSpot = pSpot;
		context.pLine = pLine;
		context.pFill = pFillGeneral;
		context.pType = pType;
		context.tolerance = viewinfo.GetPrecision(10);

		Gdiplus::Rect bounds;
		g.GetClipBounds(&bounds);
		const Gdiplus::SizeF pxlExtend = CViewinfo::PointsToPixels(12 * 10, viewinfo.m_sizeDPI);//10¸ö´óÐ¡Îª12P¸öºº×Ö
		const CSize docExtend = viewinfo.ClientToDoc(pxlExtend);
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
			CRect rect = pGeom->m_Rect;
			rect.InflateRect(docExtend.cx, docExtend.cy);//¼ÙÉèÍâÀ©10´óÐ¡Îª12P¸öºº×ÖµÄ¾àÀë»¹²»ÔÚÊÓÒ°ÄÚµÄ»°£¬ÅÅ³ýµô¡£
			if(rect.IntersectRect(rect, invalidRect) == FALSE)
				continue;

			if(pGeom->HasTag() && showCreated)
				pGeom->DrawTag(g, viewinfo, pHint);
			else if(showDynamic == false)
				continue;
			else
				pGeom->DrawTag(g, viewinfo, pHint, false);
		}
	}
}
void CLayer::Capture(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible == false)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bDrawGeom == false)
		return;

	RecalculateTemperyVariables(viewinfo);

	CSpot* pSpot = this->GetSpot();
	CLine* pLine = this->GetLine();
	CFill* pFill = this->GetFill();
	CType* pType = this->GetType();
	CFillGeneral* pFillGeneral = pFill == nullptr ? nullptr : (pFill->IsKindOf(RUNTIME_CLASS(CFillGeneral)) == TRUE ? (CFillGeneral*)pFill : nullptr);

	Context context;
	context.pSpot = pSpot;
	context.pLine = pLine;
	context.pFill = pFillGeneral;
	context.pType = pType;
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioLine = (m_bDynamic & LineDynamic) == LineDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.tolerance = viewinfo.GetPrecision(10);

	Gdiplus::Pen* pen = pLine == nullptr ? nullptr : pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
	Gdiplus::Brush* brush = pFill == nullptr ? nullptr : pFill->GetBrush(m_tree.m_library, context.ratioFill, viewinfo.m_sizeDPI);

	Gdiplus::Font* font = nullptr;
	Gdiplus::SolidBrush* textbrush = nullptr;
	if(pType != nullptr)
	{
		font = pType->GetFont(context.ratioType, viewinfo.m_sizeDPI);
		const Gdiplus::Color color = pType->GetColor();
		textbrush = ::new Gdiplus::SolidBrush(color);
	}

	context.penStroke = pen;
	context.brushFill = brush;
	context.font = font;
	context.textbrush = textbrush;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bActive == false)
			continue;

		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		pGeom->Draw(g, viewinfo, m_tree.m_library, context);
	}

	::delete pen;
	pen = nullptr;
	::delete brush;
	brush = nullptr;
	::delete font;
	font = nullptr;
	::delete textbrush;
	textbrush = nullptr;
}
void CLayer::Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible == false)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bDrawGeom == false)
		return;

	RecalculateTemperyVariables(viewinfo);

	CSpot* pSpot = this->GetSpot();
	Context context;
	context.pSpot = pSpot;
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		pGeom->Alreadyin(g, viewinfo, m_tree.m_library, context);
	}
}
void CLayer::Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible == false)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bDrawGeom == false)
		return;

	int tolerance = max(0, floor(viewinfo.m_levelCurrent - m_minLevelObj)) * 5;
	RecalculateTemperyVariables(viewinfo);

	CSpot* pSpot = this->GetSpot();
	Context context;
	context.pSpot = pSpot;
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		pGeom->Squeezein(g, viewinfo, m_tree.m_library, context, tolerance);
	}
}
void CLayer::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect) const
{
	if(m_bVisible == false)
		return;
	if(​m_bTagResistance == 0X00)
		return;

	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bDrawGeom == false)
		return;

	RecalculateTemperyVariables(viewinfo);

	CLine* pLine = this->GetLine();
	CSpot* pSpot = this->GetSpot();
	CType* pType = this->GetType();
	CColor* pOldLineColor = pLine == nullptr ? nullptr : pLine->m_pColor;
	if(pLine != nullptr)
	{
		pLine->m_pColor = CColorSpot::White();
	}
	CColor* pOldTypeColor = pType == nullptr ? nullptr : pType->m_pColor;
	if(pType != nullptr)
	{
		pType->m_pColor = CColorSpot::White();
	}

	Context context;
	context.pLine = pLine;
	context.pSpot = pSpot;
	context.pType = pType;
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;
	context.ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
	Gdiplus::Pen* pen = pLine == nullptr ? nullptr : pLine->GetPen(context.ratioLine, viewinfo.m_sizeDPI);
	Gdiplus::Font* font = nullptr;
	Gdiplus::SolidBrush* textbrush = nullptr;
	if(pType != nullptr)
	{
		font = pType->GetFont(context.ratioType, viewinfo.m_sizeDPI);
		textbrush = ::new Gdiplus::SolidBrush(Gdiplus::Color::White);
	}
	context.penStroke = pen;
	context.font = font;
	context.textbrush = textbrush;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		pGeom->Preoccupy(g, viewinfo, m_tree.m_library, context);
	}
	::delete pen;
	::delete font;
	::delete textbrush;

	if(pLine != nullptr)
	{
		pLine->m_pColor = pOldLineColor;
	}
	if(pType != nullptr)
	{
		pType->m_pColor = pOldTypeColor;
	}
}
void CLayer::PositeTag(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CRect& invalidRect, bool reset) const
{
	if(m_bVisible == false)
		return;
	const bool bDrawGeom = (m_bShowGeom == false || viewinfo.m_levelCurrent < m_minLevelObj || viewinfo.m_levelCurrent >= m_maxLevelObj) ? false : true;
	if(bDrawGeom == false)
		return;

	RecalculateTemperyVariables(viewinfo);

	Context context;
	context.pSpot = this->GetSpot();
	context.pHint = this->GetHint();
	context.ratioMark = (m_bDynamic & SpotDynamic) == SpotDynamic ? viewinfo.CurrentRatio() : 1.0f;

	POSITION pos = m_geomlist.GetTailPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos);
		if(Util::Rect::Intersect(pGeom->m_Rect, invalidRect) == false)//not accurate here			
			continue;

		CRect boundary = this->GetGeomDocBoundary(viewinfo, pGeom);
		if(Util::Rect::Intersect(boundary, invalidRect) == false)
			continue;

		pGeom->PositeTag(g, viewinfo, m_tree.m_library, context, reset);
	}
}
void CLayer::PreRemoveGeom(CGeom* pGeom) const
{
	if(pGeom == nullptr)
		return;
}
void CLayer::RecalculateTemperyVariables(const CViewinfo& viewinfo) const
{
	CLine* pLine = this->GetLine();
	if(pLine != nullptr)
	{
		const float ratioLine = (m_bDynamic & LineDynamic) == LineDynamic ? viewinfo.CurrentRatio() : 1.0f;
		const float ptsLine = pLine != nullptr ? ratioLine * pLine->m_nWidth / 20.f : 0;
		const Gdiplus::SizeF pxlLine = CViewinfo::PointsToPixels(ptsLine, viewinfo.m_sizeDPI) + Gdiplus::SizeF(3, 3);
		m_tmpLineInflation = viewinfo.ClientToDoc(pxlLine);
	}
	else
	{
		const Gdiplus::SizeF pxlLine = Gdiplus::SizeF(3, 3);
		m_tmpLineInflation = viewinfo.ClientToDoc(pxlLine);
	}

	CType* pType = this->GetType();
	if(pType != nullptr)
	{
		const float ratioType = (m_bDynamic & TypeDynamic) == TypeDynamic ? viewinfo.CurrentRatio() : 1.0f;
		const float pxlType = ratioType * pType->m_fSize * viewinfo.m_sizeDPI.cx / (2 * 72.f) + 3;
		m_tmpTypeInflation = viewinfo.ClientToDoc(Gdiplus::SizeF(pxlType, pxlType));
	}
	m_tmpLineInflation.cx = max(0, m_tmpLineInflation.cx);
	m_tmpLineInflation.cy = max(0, m_tmpLineInflation.cy);
}

void CLayer::Assign(CODBCDatabase* pDatabase, const CATTHeaderProfile& profile)
{
	if(pDatabase == nullptr)
		return;

	CString strSQL = profile.MakeSelectStatment();
	CODBCDatabase::ParseSQL(pDatabase, strSQL);
	try
	{
		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF() == FALSE)
		{
			const int attId = rs.Field(profile.m_strIdField).AsInt();
			POSITION pos = m_geomlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = m_geomlist.GetNext(pos);
				if(pGeom->m_attId == false)
					continue;

				if(pGeom->m_attId == attId) {
					if(profile.m_strAnnoField.IsEmpty() == FALSE)
					{
						pGeom->m_strName = rs.Field(profile.m_strAnnoField).AsString();
					}
					if(profile.m_strCodeField.IsEmpty() == FALSE)
					{
						pGeom->m_geoCode = rs.Field(profile.m_strCodeField).AsString();
					}
				}
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}


void CLayer::ApplyName(CDatabase* pDatabase, CATTHeaderProfile& profile, CString strField)
{
	CString strSQL = profile.MakeSelectStatment(strField);
	CODBCDatabase::ParseSQL(pDatabase, strSQL);
	try
	{
		profile.m_strAnnoField = strField;

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF() == FALSE)
		{
			CDBField& fvalue = rs.Field(strField);
			if(fvalue.IsNull())
			{
				rs.MoveNext();
				continue;
			}

			const int attId = rs.Field(profile.m_strIdField).AsInt();
			POSITION pos = m_geomlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = m_geomlist.GetNext(pos);
				if(pGeom->m_attId == attId)
				{
					pGeom->m_strName = fvalue.AsString();
					break;
				}
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}
void CLayer::ApplyCode(CDatabase* pDatabase, CATTHeaderProfile& profile, CString strField)
{
	CString strSQL = profile.MakeSelectStatment(strField);
	CODBCDatabase::ParseSQL(pDatabase, strSQL);
	try
	{
		profile.m_strCodeField = strField;

		CODBCRecordset rs(pDatabase);
		rs.Open(strSQL, CRecordset::snapshot, CRecordset::readOnly);
		while(rs.IsEOF() == FALSE)
		{
			CDBField& fvalue = rs.Field(strField);
			if(fvalue.IsNull())
			{
				rs.MoveNext();
				continue;
			}
			CDBField& ivalue = rs.Field(profile.m_strIdField);
			if(ivalue.IsNull())
			{
				rs.MoveNext();
				continue;
			}

			const int attId = ivalue.AsInt();
			POSITION pos = m_geomlist.GetHeadPosition();
			while(pos != nullptr)
			{
				CGeom* pGeom = m_geomlist.GetNext(pos);
				if(pGeom->m_attId == attId)
				{
					pGeom->m_geoCode = fvalue.AsString();
					break;
				}
			}

			rs.MoveNext();
		}
		rs.Close();
	}
	catch(CDBException* ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		ex->Delete();
	}
}