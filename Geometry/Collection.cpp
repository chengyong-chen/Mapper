#include "Geom.h"
#include "Poly.h"
#include "Group.h"
#include "Global.h"

#include "../DataView/Viewinfo.h"
#include "../Public/Enumeration.h"
#include "../Public/BinaryStream.h"
#include "../Style/Psboard.h"
#include "../Style/Line.h"
#include "../Style/FillCompact.h"
#include "../Pbf/writer.hpp"
#include "../Utility/Rect.hpp"
#include <boost/json.hpp>
using namespace boost;

#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>


template<class T>
CCollection<T>::CCollection()
	:CGeom()
{
	m_bGroup = true;
	m_bClosed = false;
}

template<class T>
CCollection<T>::CCollection(CLine* pLine, CFillCompact* pFill)
	:CGeom(pLine, pFill)
{
	m_bGroup = true;
	m_bClosed = false;
}

template<class T>
CCollection<T>::~CCollection()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		delete pGeom;
	}
	m_geomlist.RemoveAll();
}
template<class T>
bool CCollection<T>::IsOrphan() const
{
	if(m_geomlist.GetSize() == 0)
		return true;
	else 
	{
		bool orphan = true;
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			T* pGeom = m_geomlist.GetNext(pos);
			orphan = orphan && pGeom->IsOrphan();
		}
		return orphan;
	}
}

template<class T>
void CCollection<T>::Purify() const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Purify();
	}
}
template<class T>
void CCollection<T>::Clear()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		delete pGeom;
	}
	m_geomlist.RemoveAll();
}
template<class T>
void CCollection<T>::CleanOrphan()
{
	POSITION pos1 = m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2=pos1) != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos1);
		pGeom->CleanOrphan();
		if(pGeom->IsOrphan())
		{
			delete pGeom;
			m_geomlist.RemoveAt(pos2);
		}
	}
}
template<class T>
void CCollection<T>::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CGeom::Sha1(sha1);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Sha1(sha1);
	}
}
template<class T>
void CCollection<T>::Sha1(boost::uuids::detail::sha1& sha1, const CSize offset) const
{
	CGeom::Sha1(sha1, offset);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Sha1(sha1, offset);
	}
}
template<class T>
void CCollection<T>::Sha1(boost::uuids::detail::sha1& sha1, const CLine* pLine, const CFill* pFill, const CSpot* pSpot, const CType* pType) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->Sha1(sha1, pLine, pFill, pSpot, pType);
	}
}
template<class T>
BOOL CCollection<T>::operator==(const CGeom& geom) const
{
	if(CGeom::operator==(geom) == FALSE)
		return FALSE;
	else if(geom.IsKindOf(RUNTIME_CLASS(CCollection<T>)) == FALSE)
		return FALSE;
	else if(m_geomlist.GetSize() != ((CCollection<T>&)geom).m_geomlist.GetSize())
		return FALSE;
	else
	{
		for(int index = 0; index < min(m_geomlist.GetSize(), ((CCollection<T>&)geom).m_geomlist.GetSize()); index++)
		{
			T* pGeom1 = m_geomlist.GetAt(m_geomlist.FindIndex(index));
			T* pGeom2 = ((CCollection<T>&)geom).m_geomlist.GetAt(((CCollection<T>&)geom).m_geomlist.FindIndex(index));
			if(*pGeom1 != *pGeom2)
				return FALSE;
		}
		return TRUE;
	}
}
template<class T>
void CCollection<T>::ResetGeogroid()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ResetGeogroid();
	}
}
template<class T>
void CCollection<T>::ResetLabeloid()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ResetLabeloid();
	}
}
template<class T>
void CCollection<T>::RecalCentroid()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->RecalCentroid();
	}
}
template<class T>
void CCollection<T>::Draw(Gdiplus::Graphics& g, const Gdiplus::Matrix& matrixData, Context& context, const CSize& dpi) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CFillCompact* pOldFill = pGeom->m_pFill;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine != nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pFill = pGeom->m_pFill != nullptr ? pGeom->m_pFill : m_pFill;
		pGeom->m_pType = pGeom->m_pType != nullptr ? pGeom->m_pType : m_pType;

		pGeom->Draw(g, matrixData, context, dpi);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pFill = pOldFill;
		pGeom->m_pType = pOldType;
	}
}

template<class T>
void CCollection<T>::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CFillCompact* pOldFill = pGeom->m_pFill;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine != nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pFill = pGeom->m_pFill != nullptr ? pGeom->m_pFill : m_pFill;
		pGeom->m_pType = pGeom->m_pType != nullptr ? pGeom->m_pType : m_pType;
		pGeom->Draw(g, viewinfo, ratio);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pFill = pOldFill;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const float& ratio) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;		
		CType* pOldType = pGeom->m_pType;
		pGeom->m_pLine = pGeom->m_pLine!=nullptr ? pGeom->m_pLine : m_pLine;		
		pGeom->m_pType = pGeom->m_pType!=nullptr ? pGeom->m_pType : m_pType;

		pGeom->Preoccupy(g, viewinfo, ratio);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CFillCompact* pOldFill = pGeom->m_pFill;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine != nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pFill = pGeom->m_pFill != nullptr ? pGeom->m_pFill : m_pFill;
		pGeom->m_pType = pGeom->m_pType != nullptr ? pGeom->m_pType : m_pType;
		pGeom->Draw(g, viewinfo, library, context);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pFill = pOldFill;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine != nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pType = pGeom->m_pType != nullptr ? pGeom->m_pType : m_pType;
		pGeom->Preoccupy(g, viewinfo, library, context);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::Alreadyin(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine!=nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pType = pGeom->m_pType!=nullptr ? pGeom->m_pType : m_pType;
		pGeom->Alreadyin(g, viewinfo, library, context);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::Squeezein(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, Context& context, const int tolerance) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CLine* pOldLine = pGeom->m_pLine;
		CType* pOldType = pGeom->m_pType;

		pGeom->m_pLine = pGeom->m_pLine!=nullptr ? pGeom->m_pLine : m_pLine;
		pGeom->m_pType = pGeom->m_pType!=nullptr ? pGeom->m_pType : m_pType;
		pGeom->Squeezein(g, viewinfo, library, context, tolerance);

		pGeom->m_pLine = pOldLine;
		pGeom->m_pType = pOldType;
	}
}
template<class T>
void CCollection<T>::DrawHatch(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const int& width) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->DrawHatch(g, viewinfo, width);
	}
}
template<class T>
T* CCollection<T>::GetBiggest() const
{
	double biggestArea = 0;
	T* biggestGeom = nullptr;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bClosed == false)
			continue;

		double area = abs(pGeom->GetArea());
		if(area > biggestArea)
		{
			biggestArea = area;
			biggestGeom = pGeom;
		}
	}
	return biggestGeom;
}
template<class T>
void CCollection<T>::OrderByArea()
{
	if(m_geomlist.GetCount() <= 1)
		return;

	std::vector<std::pair<T*, double>> pairs;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);

		double area = pGeom->m_bClosed == false ? 0 : abs(pGeom->GetArea());
		pairs.push_back(std::make_pair(pGeom, area));
	}
	std::sort(pairs.begin(), pairs.end(), [](const std::pair<T*, double>& x, const std::pair<T*, double>& y)
	{
		return x.second < y.second;
	});
	while(m_geomlist.GetCount() > 0)
		m_geomlist.RemoveTail();
	for(auto it = pairs.rbegin(); it != pairs.rend(); ++it)
	{
		m_geomlist.AddTail(it->first);
	}
}
template<class T>
bool CCollection<T>::PickIn(const CPoint& point) const
{
	if(CGeom::PickIn(point) == false)
		return false;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bClosed == false)
			continue;

		if(pGeom->PickIn(point) == true)
			return true;
	}

	return false;
}
template<class T>
float CCollection<T>::GetInflation(const CLine* pLine, const CType* pType, const float& ratioLine, const float& ratioType) const
{
	float fPts = 0.f;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		fPts = max(fPts, pGeom->GetInflation(pLine, pType, ratioLine, ratioType));
	}
	return fPts;
}

template<class T>
void CCollection<T>::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CGeom::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		CPoint point;
		ar << point;
	}
	else
	{
		CPoint point;
		ar >> point;
	}
	CGeom::Serializelist(ar, dwVersion, m_geomlist);
}

template<class T> void CCollection<T>::Flash(CWnd* pWnd, const CViewinfo& viewinfo, const int& nTimes) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Flash(pWnd, viewinfo, nTimes);
	}
}


template<class T>
void CCollection<T>::CountStyle(CValueCounter<CLine>& lines, CValueCounter<CFillGeneral>& fills, CValueCounter<CSpot>& spots, CValueCounter<CType>& types, const CLine* pLine, const CFillGeneral* pFill, const CSpot* pSpot, const CType* pType) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->CountStyle(lines, fills, spots, types, m_pLine != nullptr ? m_pLine : pLine, m_pFill != nullptr ? m_pFill : pFill, pSpot, pType);
	}
}

template<class T>
void CCollection<T>::ChangeAnchor(const unsigned int& nAnchor, const CSize& Δ, const bool& bMatch, Undoredo::CActionStack* pActionstack)
{
	if(m_Rect.Width() == 0)
		return;
	if(m_Rect.Height() == 0)
		return;

	CRect rect1 = m_Rect;
	CGeom::ChangeAnchor(nAnchor, Δ, bMatch, nullptr);
	CRect rect2 = m_Rect;


	double m11 = (double)rect2.Width()/rect1.Width();
	double m21 = 0;
	double m31 = rect2.left - m11*rect1.left;
	double m12 = 0;
	double m22 = (double)rect2.Height()/rect1.Height();
	double m32 = rect2.top - m22*rect1.top;

	m_Rect = rect1;
	this->Transform(m11, m21, m31, m12, m22, m32);
}

template<class T>
bool CCollection<T>::HasSameStyle(const ACTIVEALL& activeall, const CLine* pLine1, const CFillGeneral* pFill1, const CSpot* pSpot1, const CType* pType1, const CLine* pLine2, const CFillGeneral* pFill2, const CSpot* pSpot2, const CType* pType2) const
{
	pLine2 = m_pLine != nullptr ? m_pLine : pLine2;
	pFill2 = m_pFill != nullptr ? m_pFill : pFill2;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->HasSameStyle(activeall, pLine1, pFill1, pSpot1, pType1, pLine2, pFill2, pSpot2, pType2) == false)
			return false;
	}

	return true;
}

template<class T>
bool CCollection<T>::IsDissident() const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsDissident() == true)
			return true;
	}

	return CGeom::IsDissident();
}

template<class T>
bool CCollection<T>::HasTag() const
{
	if(CGeom::HasTag())
		return true;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->HasTag())
			return true;
	}

	return false;
}
template<class T>
bool CCollection<T>::HasGeoCode() const
{
	if(CGeom::HasGeoCode())
		return true;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->HasGeoCode())
			return true;
	}

	return false;
}
template<class T>
void CCollection<T>::Regress(CWnd* pWnd, CLibrary& library)
{
	CGeom::Regress(pWnd, library);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Regress(pWnd, library);
	}
}
template<class T>
void CCollection<T>::Fleefrom(CGeom* pGeom) const
{
	pGeom->m_geoId = m_geoId;
	pGeom->m_attId = m_attId;
	pGeom->m_bLocked = m_bLocked;
	pGeom->m_bActive = m_bActive;
	if(m_strName.IsEmpty() == FALSE && pGeom->m_strName.IsEmpty() == TRUE)
		pGeom->m_strName = m_strName;
	if(m_pLine != nullptr && pGeom->m_pLine == nullptr)
		pGeom->m_pLine = m_pLine->Clone();
	if(m_pFill != nullptr && pGeom->m_pFill == nullptr)
		pGeom->m_pFill = (CFillCompact*)m_pFill->Clone();
}
template<class T>
void CCollection<T>::AddMember(CGeom* pGeom)
{
	if(pGeom == nullptr)
		return;

	POSITION pos = m_geomlist.Find(pGeom);
	if(pos != nullptr)
		return;

	m_geomlist.AddTail((T*)pGeom);
	if(m_Rect.IsRectNull())
		m_Rect = pGeom->m_Rect;
	else if(pGeom->m_Rect.IsRectNull() == FALSE)
	{
		m_Rect.left = min(m_Rect.left, pGeom->m_Rect.left);
		m_Rect.top = min(m_Rect.top, pGeom->m_Rect.top);
		m_Rect.right = max(m_Rect.right, pGeom->m_Rect.right);
		m_Rect.bottom = max(m_Rect.bottom, pGeom->m_Rect.bottom);
	}
}
template<class T>
void CCollection<T>::AddMembers(std::list<CPoly*>& polys)
{
	for(CPoly* pPoly : polys)
	{
		AddMember(pPoly);
	}
}
template<class T>
void CCollection<T>::AddMembers(std::list<CGeom*>& geoms)
{
	for(CGeom* pGeom : geoms)
	{
		AddMember(pGeom);
	}
}
template<class T>
bool CCollection<T>::RemoveMember(T* pGeom)
{
	if(pGeom == nullptr)
		return false;

	POSITION pos = m_geomlist.Find(pGeom);
	if(pos != nullptr)
	{
		m_geomlist.RemoveAt(pos);
		delete pGeom;
		this->RecalRect();
		return true;
	}
	else
		return false;
}

template<class T>
void CCollection<T>::CopyTo(CGeom* pGeom, bool ignore) const
{
	CGeom::CopyTo(pGeom, ignore);

	if(pGeom->IsKindOf(RUNTIME_CLASS(CCollection)) == TRUE)
	{
		CCollection* pGroup = (CCollection*)pGeom;
		POSITION pos1 = pGroup->m_geomlist.GetHeadPosition();
		while(pos1 != nullptr)
		{
			T* geom = pGroup->m_geomlist.GetNext(pos1);
			delete geom;
		}
		pGroup->m_geomlist.RemoveAll();

		POSITION pos2 = m_geomlist.GetHeadPosition();
		while(pos2 != nullptr)
		{
			T* geom1 = m_geomlist.GetNext(pos2);
			T* geom2 = (T*)geom1->Clone();
			pGroup->m_geomlist.AddTail(geom2);
		}
	}
}


template<class T>
void CCollection<T>::Swap(CGeom* pGeom)
{
	if(pGeom->IsKindOf(RUNTIME_CLASS(CCollection<T>)) == TRUE)
	{
		CCollection* pGroup = (CCollection*)pGeom;
		Swaplist<CObList>(pGroup->m_geomlist, m_geomlist);
	}

	CGeom::Swap(pGeom);
}


template<class T>
bool CCollection<T>::UnGroup(CTypedPtrList<CObList, CGeom*>& geomlist)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		geomlist.AddTail(pGeom);
		CGeom::Inherit(pGeom);
	}
	return true;
}

template<class T>
void CCollection<T>::Move(const int& dx, const int& dy)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Move(dx, dy);
	}
	CGeom::Move(dx, dy);
}
template<class T>
void CCollection<T>::Move(const CSize& Δ)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Move(Δ);
	}
	CGeom::Move(Δ);
}

template<class T>
void CCollection<T>::RecalRect()
{
	CGeom::RecalRect();

	CRect bounds;
	bounds.SetRectEmpty();
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->RecalRect();
		CRect rect = pGeom->GetRect();
		bounds = UnionRect(bounds, rect);
	}

	bounds.NormalizeRect();
	m_Rect = bounds;
}

template<class T>
void CCollection<T>::Densify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_bLocked)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			((CPoly*)pGeom)->Densify(tolerance, actionstack);
		}
	}

	m_bModified = true;

	RecalRect();
}

template<class T>
void CCollection<T>::Simplify(const unsigned int& tolerance, Undoredo::CActionStack& actionstack)
{
	if(m_bLocked)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Simplify(tolerance, actionstack);
	}

	m_bModified = true;

	RecalRect();
}
template<class T>
void CCollection<T>::Correct(const unsigned int& tolerance, std::list<CGeom*>& geoms)
{
	std::list<CGeom*> news;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		std::list<CGeom*> subs;
		pGeom->Correct(tolerance, subs);
		if(subs.size()==0)
		{
			news.push_back(pGeom);
		}
		else
		{
			news.insert(news.end(), subs.begin(), subs.end());
			delete pGeom;
		}
		subs.clear();
	}
	m_geomlist.RemoveAll();

	for(const CGeom* it : news)
	{
		m_geomlist.AddTail((T*)it);
	}
	news.clear();
}
template<class T>
void CCollection<T>::Simplify(const unsigned int& tolerance)
{
	if(m_bLocked)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Simplify(tolerance);
	}
}
template<class T>
void CCollection<T>::Normalize()
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Normalize();
	}
}
template<class T>
bool CCollection<T>::PickOn(const CPoint& point, const unsigned long& gap) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->PickOn(point, gap) == true)
			return true;
	}

	return false;
}

template<class T>
bool CCollection<T>::PickIn(const CRect& rect) const
{
	if(CGeom::PickIn(rect) == false)
		return false;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->PickIn(rect) == true)
			return true;
	}

	return false;
}

template<class T>
bool CCollection<T>::PickIn(const CPoint& center, const unsigned long& radius) const
{
	if(CGeom::PickIn(center, radius) == false)
		return false;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bClosed == false)
			continue;

		if(pGeom->PickIn(center, radius) == true)
			return true;
	}

	return false;
}

template<class T>
bool CCollection<T>::PickIn(const CPoly& polygon) const
{
	if(CGeom::PickIn(polygon) == false)
		return false;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		if(pGeom->m_bClosed == false)
			continue;

		if(pGeom->PickIn(polygon) == true)
			return true;
	}

	return false;
}

template<class T>
void CCollection<T>::Transform(const CViewinfo& viewinfo)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Transform(viewinfo);
	}

	CGeom::Transform(viewinfo);
}

template<class T>
void CCollection<T>::Rotate(const CDatainfo& mapinfo, const float& deltaLng, const float& deltaLat, const float& facing)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Rotate(mapinfo, deltaLng, deltaLat, facing);
	}
	RecalRect();

	m_bModified = true;
}

template<class T>
CGeom* CCollection<T>::Project(const CDatainfo& fMapinfo, const CDatainfo& tMapinfo, const Interpolation& interpolation, const float& tolerance)
{
	POSITION pos1 = m_geomlist.GetHeadPosition();
	POSITION pos2;
	while((pos2 = pos1) != nullptr)
	{
		T* pGeom1 = m_geomlist.GetNext(pos1);
		CGeom* pGeom2 = pGeom1->Project(fMapinfo, tMapinfo, interpolation, tolerance);
		if(pGeom2 == nullptr)
		{
			m_geomlist.RemoveAt(pos2);
			delete pGeom1;
		}
		else if(pGeom2 != pGeom1)
		{
			CCollection<CGeom>* collection = (CCollection<CGeom>*)pGeom2;
			POSITION Pos3 = collection->m_geomlist.GetHeadPosition();
			while(Pos3 != nullptr)
			{
				T* pGeom3 = (T*)collection->m_geomlist.GetNext(Pos3);
				m_geomlist.InsertBefore(pos2, pGeom3);
			}
			collection->m_geomlist.RemoveAll();
			m_geomlist.RemoveAt(pos2);
			delete pGeom1;
			delete pGeom2;
		}
	}
	RecalRect();

	m_bModified = true;
	return m_geomlist.GetHeadPosition() == nullptr ? nullptr : this;
}
template<class T>
void CCollection<T>::Rectify(CTin& tin)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Rectify(tin);
	}

	RecalRect();
	m_bModified = true;
}

template<class T>
void CCollection<T>::Transform(CDC* pDC, const CViewinfo& viewinfo, const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Transform(pDC, viewinfo, m11, m21, m31, m12, m22, m32);
	}

	//	CGeom::Transform(pDC,viewinfo,m11,m21,m31,m12,m22,m32);
}

template<class T>
void CCollection<T>::Transform(const double& m11, const double& m21, const double& m31, const double& m12, const double& m22, const double& m32)
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->Transform(m11, m21, m31, m12, m22, m32);
	}

	CGeom::Transform(m11, m21, m31, m12, m22, m32);
}
template<class T>
CGeom* CCollection<T>::Clip(const CRect& bound, Clipper2Lib::Paths64& clips, bool in, int tolerance, bool bStroke)
{
	if(CGeom::Clip(bound, clips, in, tolerance, bStroke) == nullptr)
		return nullptr;

	std::list<CGeom*> geomlist;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		CGeom* pCliped = pGeom->Clip(bound, clips, in, tolerance, bStroke);
		if(pCliped == nullptr)
			delete pGeom;
		else if(pCliped == pGeom)
			geomlist.push_back(pGeom);
		else
		{
			delete pGeom;
			geomlist.push_back(pCliped);
		}
	}
	m_geomlist.RemoveAll();

	if(geomlist.size() == 0)
		return nullptr;
	else if(geomlist.size() == 1)
	{
		CGeom* pGeom = geomlist.front();
		CGeom::CopyTo(pGeom, true);
		return pGeom;
	}
	else
	{
		CCollection<T>* pCollection = (CCollection<T>*)CGeom::Apply(this->m_bType);
		CGeom::CopyTo(pCollection, true);
		pCollection->AddMembers(geomlist);
		pCollection->RecalRect();
		return pCollection;
	}
}

template<class T>
std::list<std::tuple<int, int, CGeom*>> CCollection<T>::Swim(bool bStroke, bool bFill, int cxTile, int cyTile, int minCol, int maxCol, int minRow, int maxRow) const
{
	int minX = m_Rect.left/cxTile;
	int maxX = m_Rect.right/cxTile;
	int minY = m_Rect.top/cyTile;
	int maxY = m_Rect.bottom/cyTile;
	maxX = m_Rect.right % cxTile == 0 ? maxX - 1 : maxX;
	maxY = m_Rect.bottom % cyTile == 0 ? maxY - 1 : maxY;
	if(maxX <= minX && maxY <= minY)
		return std::list < std::tuple<int, int, CGeom*> > {make_tuple(minX, minY, this->Clone())};


	std::list<std::tuple<int, int, CGeom*>> children;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		std::list<std::tuple<int, int, CGeom*>> geoms = pGeom->Swim(bStroke, bFill, cxTile, cyTile, minCol, maxCol, minRow, maxRow);
		children.insert(children.end(), geoms.begin(), geoms.end());
	}
	std::map<std::pair<int, int>, std::list<CGeom*>> cellitems;
	for(auto& it : children)
	{
		std::pair<int, int> key = make_pair(std::get<0>(it), std::get<1>(it));
		if(cellitems.count(key) == 0)
			cellitems[key] = {std::get<2>(it)};
		else
			cellitems[key].push_back(std::get<2>(it));
	}
	children.clear();

	for(auto& cell : cellitems)
	{
		//in order to keep the same struture
		if(cell.second.size() == 1)
		{
			CGeom* pGeom = cell.second.front();
			this->Fleefrom(pGeom);
			children.push_back(std::make_tuple(cell.first.first, cell.first.second, pGeom));
		}
		else if(cell.second.size() == 2 && *cell.second.front() == *cell.second.back())//it is a hole
		{
			delete cell.second.front();
			delete cell.second.back();
			cell.second.clear();
		}
		else if(cell.second.size() > 1)
		{
			CCollection<T>* pCollection = (CCollection<T>*)CGeom::Apply(this->m_bType);
			CGeom::CopyTo(pCollection);
			pCollection->AddMembers(cell.second);
			pCollection->RecalRect();
			children.push_back(std::make_tuple(cell.first.first, cell.first.second, pCollection));
		}
	}
	return children;
}

template<class T>
void CCollection<T>::GatherFonts(std::list<CStringA>& fontlist) const
{
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->GatherFonts(fontlist);
	}
}

template<class T>
void CCollection<T>::ExportPlaintext(FILE* file, const Gdiplus::Matrix& matrix) const
{
	_ftprintf(file, _T("Id:%u ATT:%u Type:%d C:%d %s\n"), m_geoId, m_attId, m_bType, m_bClosed, m_strName);
	_ftprintf(file, _T("Children:%d\n"), m_geomlist.GetCount());

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPlaintext(file, matrix);
	}

	_ftprintf(file, _T("EndGeom\n"));
}

template<class T>
void CCollection<T>::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	if(m_geomlist.GetCount() == 0)
		return;

	_ftprintf(file, _T("u\n"));

	CLine* oldLine = aiKey.pubLine;
	CFillGeneral* oldFill = aiKey.pubFill;
	CType* oldType = aiKey.pubType;
	CHint* oldHint = aiKey.pubHint;
	aiKey.pubLine = m_pLine != nullptr ? m_pLine : aiKey.pubLine;
	aiKey.pubFill = m_pFill != nullptr ? m_pFill : aiKey.pubFill;
	aiKey.pubType = m_pType != nullptr ? m_pType : aiKey.pubType;
	aiKey.pubHint = m_pHint != nullptr ? m_pHint : aiKey.pubHint;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPs3(file, matrix, aiKey);
	}
	aiKey.pubLine = oldLine;
	aiKey.pubFill = oldFill;
	aiKey.pubType = oldType;
	aiKey.pubHint = oldHint;

	_ftprintf(file, _T("U\n"));
}

template<class T>
void CCollection<T>::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey) const
{
	if(m_geomlist.GetCount() == 0)
		return;

	_ftprintf(file, _T("u\n"));

	CLine* oldLine = aiKey.pubLine;
	CFillGeneral* oldFill = aiKey.pubFill;
	CType* oldType = aiKey.pubType;
	CHint* oldHint = aiKey.pubHint;
	aiKey.pubLine = m_pLine != nullptr ? m_pLine : aiKey.pubLine;
	aiKey.pubFill = m_pFill != nullptr ? m_pFill : aiKey.pubFill;
	aiKey.pubType = m_pType != nullptr ? m_pType : aiKey.pubType;
	aiKey.pubHint = m_pHint != nullptr ? m_pHint : aiKey.pubHint;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPs3(file, viewinfo, aiKey);
	}
	aiKey.pubLine = oldLine;
	aiKey.pubFill = oldFill;
	aiKey.pubType = oldType;
	aiKey.pubHint = oldHint;

	_ftprintf(file, _T("U\n"));
}

template<class T>
void CCollection<T>::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	if(m_geomlist.GetCount() == 0)
		return;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPdf(pdf, page, matrix);
	}
}


template<class T>
void CCollection<T>::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, Context& context) const
{
	if(m_geomlist.GetCount() == 0)
		return;

	CLine* oldLine = (CLine*)context.pLine;
	CFillCompact* oldFill = (CFillCompact*)context.pFill;
	context.pLine = m_pLine != nullptr ? m_pLine : (CLine*)context.pLine;
	context.pFill = m_pFill != nullptr ? m_pFill : (CFillCompact*)context.pFill;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPdf(pdf, page, viewinfo, library, context);
	}
	context.pLine = oldLine;
	context.pFill = oldFill;
}

template<class T>
void CCollection<T>::ExportMapInfoMif(const CDatainfo& datainfo, FILE* fileMif, const bool& bTransGeo, const unsigned int& tolerance) const
{
	int count = 0;
	POSITION pos1 = m_geomlist.GetHeadPosition();
	while(pos1 != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos1);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			count++;
		}
	}

	fprintf(fileMif, "Region %d\n", count);

	POSITION pos2 = m_geomlist.GetHeadPosition();
	while(pos2 != nullptr)
	{
		T* pGeom = (T*)m_geomlist.GetNext(pos2);
		if(pGeom->IsKindOf(RUNTIME_CLASS(CPoly)) == TRUE)
		{
			CPoly* pPoly = (CPoly*)pGeom;
			fprintf(fileMif, "%  d\n", pPoly->m_nAnchors);
			for(unsigned int nAnchor = 1; nAnchor <= pPoly->m_nAnchors; nAnchor++)
			{
				if(bTransGeo == true)
				{
					CPointF geoPoint = datainfo.DocToGeo(pPoly->m_pPoints[nAnchor - 1]);
					fprintf(fileMif, "%g %g\n", geoPoint.x, geoPoint.y);
				}
				else
				{
					fprintf(fileMif, "%d %d\n", pPoly->m_pPoints[nAnchor - 1].x, pPoly->m_pPoints[nAnchor - 1].y);
				}
			}
		}
	}
}

template<class T>
void CCollection<T>::ExportMid(FILE* fileMid) const
{
}


template<class T>
void CCollection<T>::ReleaseWeb(CFile& file, CSize offset) const
{
	CGeom::ReleaseWeb(file, offset);;

	unsigned short nCount = m_geomlist.GetCount();
	ReverseOrder(nCount);
	file.Write(&nCount, sizeof(unsigned short));

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(file, offset);;
	}
}
template<class T>
void CCollection<T>::ReleaseWeb(BinaryStream& stream, CSize offset) const
{
	CGeom::ReleaseWeb(stream, offset);;

	unsigned short nCount = m_geomlist.GetCount();
	stream << nCount;

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(stream, offset);;
	}
}
template<class T>
void CCollection<T>::ReleaseWeb(boost::json::object& json) const
{
	CGeom::ReleaseWeb(json);

	boost::json::array geoms;
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		boost::json::object geom;
		pGeom->ReleaseWeb(geom);
		geoms.push_back(geom);
	}
	json["Geoms"] = geoms;
}
template<class T>
void CCollection<T>::ReleaseWeb(pbf::writer& writer, const CSize offset) const
{
	CGeom::ReleaseWeb(writer, offset);

	writer.add_variant_uint32(m_geomlist.GetSize());
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos != nullptr)
	{
		T* pGeom = m_geomlist.GetNext(pos);
		pGeom->ReleaseWeb(writer, offset);
	}
}