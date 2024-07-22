#include "stdafx.h"
#include "Pyramid.h"
#include "../Geometry/Geom.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL AFXAPI AfxComparePath(LPCTSTR lpszPath1, LPCTSTR lpszPath2);

IMPLEMENT_SERIAL(CPyramid, CObject, 0)

/////////////////////////////////////////////////////////////////////////////
// CPyramid

CPyramid::CPyramid()
{
	m_pApex = nullptr;
}

CPyramid::~CPyramid()
{
}

bool CPyramid::BuildRelation()
{
	m_pApex = nullptr;

	POSITION pos1 = m_DeckList.GetHeadPosition();
	while(pos1!=nullptr)
	{
		CDeck* pDeck = m_DeckList.GetNext(pos1);
		pDeck->m_pParent = nullptr;
		pDeck->m_Children.RemoveAll();
	}

	CDeckList decklist;
	POSITION pos2 = m_DeckList.GetHeadPosition();
	while(pos2!=nullptr)
	{
		CDeck* pDeck = m_DeckList.GetNext(pos2);
		if(pDeck->m_wParent==0)
		{
			if(m_pApex==nullptr)
				m_pApex = pDeck;
			else
				return false;
		}
		else
			decklist.AddTail(pDeck);
	}

	if(m_pApex!=nullptr)
	{
		FindChildren(m_pApex, decklist);

		if(decklist.GetCount()>0)
			return false;
		else
			return true;
	}
	else
		return false;
}

void CPyramid::FindChildren(CDeck* pParent, CDeckList& decklist)
{
	if(decklist.GetCount()==0)
	{
		return;
	}
	else
	{
		POSITION pos1 = decklist.GetHeadPosition();
		POSITION pos2;
		while((pos2 = pos1)!=nullptr)
		{
			CDeck* pDeck = decklist.GetNext(pos1);
			if(pDeck->m_wParent==pParent->m_wId)
			{
				decklist.RemoveAt(pos2);

				pDeck->m_pParent = pParent;
				pParent->m_Children.AddTail(pDeck);

				FindChildren(pDeck, decklist);

				pos1 = decklist.GetHeadPosition();
			}
		}
	}
}

DWORD CPyramid::GetMaxItemId()
{
	WORD wMaxId = 0;
	POSITION pos = m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CTreeNode1* node = m_DeckList.GetNext(pos);
		if(node->m_wId>wMaxId)
		{
			wMaxId = node->m_wId;
		}
	}

	return wMaxId;
}

CDeck* CPyramid::GetDeck(const CPointF& geoPoint) const
{
	if(m_pApex==nullptr)
		return nullptr;

	if(m_pApex->PointIn(geoPoint)==true)
	{
		CDeck* pParent = nullptr;
		CDeck* pChild = m_pApex;

		while(pChild!=nullptr)
		{
			pParent = pChild;
			pChild = nullptr;

			POSITION pos = pParent->m_Children.GetHeadPosition();
			while(pos!=nullptr)
			{
				CDeck* pDeck = pParent->m_Children.GetNext(pos);;
				if(pDeck->PointIn(geoPoint)==true)
				{
					pChild = pDeck;
					break;
				}
			}
		}

		return pParent;
	}
	else
	{
		return nullptr;
	}
}

CDeck* CPyramid::GetDeck(const CPointF& geoPoint1, const CPointF& geoPoint2) const
{
	if(m_pApex==nullptr)
		return nullptr;

	if(m_pApex->PointIn(geoPoint1)==true&&m_pApex->PointIn(geoPoint2)==true)
	{
		CDeck* pParent = nullptr;
		CDeck* pChild = m_pApex;

		while(pChild!=nullptr)
		{
			pParent = pChild;
			pChild = nullptr;

			POSITION pos = pParent->m_Children.GetHeadPosition();
			while(pos!=nullptr)
			{
				CDeck* pDeck = pParent->m_Children.GetNext(pos);;
				if(pDeck->PointIn(geoPoint1)==true&&pDeck->PointIn(geoPoint2)==true)
				{
					pChild = pDeck;
					break;
				}
			}
		}

		return pParent;
	}
	else
	{
		return nullptr;
	}
}

CDeck* CPyramid::GetDeck(WORD wId) const
{
	POSITION pos = m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDeck* pDeck = m_DeckList.GetNext(pos);
		if(pDeck->m_wId==wId)
		{
			return pDeck;
		}
	}

	return nullptr;
}

CDeck* CPyramid::GetDeck(CString strFile) const
{
	POSITION pos = m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDeck* pDeck = m_DeckList.GetNext(pos);
		if(pDeck->m_strFile.CompareNoCase(strFile)==0)
		{
			return pDeck;
		}
	}

	return nullptr;
}

CDeck* CPyramid::GetNavyDeck(const CPointF& geoPoint1, const CPointF& geoPoint2) const
{
	if(m_pApex==nullptr)
		return nullptr;

	if(m_pApex->PointIn(geoPoint1)==true&&m_pApex->PointIn(geoPoint2)==true)
	{
		CDeck* naviDeck = nullptr;
		CDeck* pParent = nullptr;
		CDeck* pChild = m_pApex;

		while(pChild!=nullptr)
		{
			pParent = pChild;
			pChild = nullptr;

			POSITION pos = pParent->m_Children.GetHeadPosition();
			while(pos!=nullptr)
			{
				CDeck* pDeck = pParent->m_Children.GetNext(pos);;
				if(pDeck->PointIn(geoPoint1)==true&&pDeck->PointIn(geoPoint2)==true)
				{
					if(pDeck->m_bNavigatable==false)
					{
						naviDeck = pDeck;
					}
					pChild = pDeck;
					break;
				}
			}
		}

		return naviDeck;
	}
	else
	{
		return nullptr;
	}
}

void CPyramid::Release()
{
	POSITION pos = m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		const CDeck* pDeck = m_DeckList.GetNext(pos);
		delete pDeck;
	}
	m_DeckList.RemoveAll();
}

void CPyramid::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
	}
	else
	{
	}

	m_DeckList.Serialize(ar);
}

void CPyramid::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
	}
}

void CPyramid::SerializeWEB(CArchive& ar) const
{
}

void CPyramid::PublishPC(CDaoDatabase* pDB, CDaoRecordset& rs, CString strTable)
{
	POSITION pos = m_DeckList.GetHeadPosition();
	while(pos!=nullptr)
	{
		CDeck* pDeck = m_DeckList.GetNext(pos);
		pDeck->PublishPC(pDB, rs, strTable);
	}
}

void CPyramid::PublishCE()
{
}

void CPyramid::PublishWEB()
{
}
