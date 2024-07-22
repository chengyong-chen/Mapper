#include "stdafx.h"
#include "Spot.h"
#include "SpotSymbol.h"
#include "SpotFont.h"
#include "SpotPattern.h"
#include "SpotFlash.h"

#include "../Dataview/Datainfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSpot::CSpot()
{
	m_bInitialized = false;
	m_tmpRect = CRect(0, 0, 0, 0);
}

CSpot::~CSpot()
{
}
void CSpot::Sha1(boost::uuids::detail::sha1::digest_type& hash) const
{
	boost::uuids::detail::sha1 sha1;
	this->Sha1(sha1);
	sha1.get_digest(hash);
}
void CSpot::Sha1(boost::uuids::detail::sha1& sha1) const
{

}
BOOL CSpot::operator==(const CSpot& spot) const
{
	CRuntimeClass* pRuntimeClass1 = this->GetRuntimeClass();
	CRuntimeClass* pRuntimeClass2 = spot.GetRuntimeClass();
	if(pRuntimeClass1!=pRuntimeClass2)
		return FALSE;
	else
		return TRUE;
}

BOOL CSpot::operator !=(const CSpot& spot) const
{
	return !(*this==spot);
}

CSpot* CSpot::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CSpot* pSpot = (CSpot*)pRuntimeClass->CreateObject();
	this->CopyTo(pSpot);
	return pSpot;
}

void CSpot::CopyTo(CSpot* spot) const
{
	spot->m_tmpRect = m_tmpRect;
}

CSpot* CSpot::Apply(BYTE type)
{
	CSpot* spot = nullptr;

	switch(type)
	{
	case 1:
		spot = new CSpotSymbol;
		break;
	case 2:
		spot = new CSpotFont;
		break;
	case 3:
		spot = new CSpotPattern;
		break;
	case 4:
		spot = new CSpotFlash;
		break;
	}

	return spot;
}
CRect CSpot::GetWanRect(const CLibrary& library) const
{
	return CRect(-10000, -10000, 10000, 10000);
}

CRect CSpot::GetDocRect(const CDatainfo& datainfo, const CLibrary& library) const
{
	if(m_bInitialized==false)
	{
		const CRect rect = this->GetWanRect(library);
		m_tmpRect = datainfo.WanToDoc(rect);
		m_bInitialized = true;
	}
	return m_tmpRect;
}


DWORD CSpot::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile!=nullptr)
	{
		const DWORD size = 0;
		return size;
	}
	else
	{
		return 0;
	}
}