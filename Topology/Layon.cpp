#include "stdafx.h"
#include "Layon.h"
#include "Polygon.h"
#include "Polying.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLayon::CLayon(CTree<CLayer>& tree)
	: CLayer(tree)
{
}
CLayon::CLayon(CTree<CLayer>& tree, float minLevelObj, float maxLevelObj, float minLevelTag, float maxLevelTag)
	: CLayer(tree, minLevelObj, maxLevelObj, minLevelTag, maxLevelTag)
{
}

CLayon::~CLayon()
{

}

void CLayon::Serializelist(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << (int)m_geomlist.GetCount();
		POSITION pos = m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* pGeom = m_geomlist.GetNext(pos);
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
			switch(type)
			{
				case 51:
					{
						CPolygon* pPolygon = new CPolygon(*m_pRegionTopoer);
						pPolygon->Serialize(ar, dwVersion);
						m_geomlist.AddTail(pPolygon);
					}
					break;
				case 52:
					{
						CGonroup* pGonroup = new CGonroup(*m_pRegionTopoer);
						pGonroup->Serialize(ar, dwVersion);
						m_geomlist.AddTail(pGonroup);
					}
					break;
				case 61:
					{
						CPolying* pPolying = new CPolying(*m_pRegionTopoer);
						pPolying->Serialize(ar, dwVersion);
						m_geomlist.AddTail(pPolying);
					}
					break;
				case 62:
					{
						CIngroup* pIngroup = new CIngroup(*m_pRegionTopoer);
						pIngroup->Serialize(ar, dwVersion);
						m_geomlist.AddTail(pIngroup);
					}
					break;
				default:
					break;
			}
		}
	}
}