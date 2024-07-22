#include "stdafx.h"

#include "FlashMid.h"
#include "Spot.h"
#include "SpotFlash.h"
#include "Library.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSpotFlash, CSpot, 0)

CSpotFlash::CSpotFlash()
{
	m_libId = -1;
	m_nScale = 100;
}

CSpotFlash::~CSpotFlash()
{
}
void CSpotFlash::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CSpot::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
}
BOOL CSpotFlash::operator==(const CSpot& spot) const
{
	if(CSpot::operator==(spot)==FALSE)
		return FALSE;
	else if(spot.IsKindOf(RUNTIME_CLASS(CSpotFlash))==FALSE)
		return FALSE;
	else if(m_libId!=((CSpotFlash*)&spot)->m_libId)
		return FALSE;
	else if(m_nScale!=((CSpotFlash*)&spot)->m_nScale)
		return FALSE;
	else
		return TRUE;
}

void CSpotFlash::CopyTo(CSpot* spot) const
{
	CSpot::CopyTo(spot);

	if(spot->IsKindOf(RUNTIME_CLASS(CSpotFlash))==TRUE)
	{
		((CSpotFlash*)spot)->m_libId = m_libId;
		((CSpotFlash*)spot)->m_nScale = m_nScale;
	}
}

void CSpotFlash::Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const
{
	CFlashMid* pFlashMid = (CFlashMid*)library.m_libSpotFlash.GetMid(m_libId);
	if(pFlashMid==nullptr)
		return;

	pFlashMid->Draw(g, (fZoom*m_nScale)/100);
}

CRect CSpotFlash::GetWanRect(const CLibrary& library) const
{
	CFlashMid* pFlashMid = (CFlashMid*)library.m_libSpotFlash.GetMid(m_libId);
	if(pFlashMid==nullptr)
		return CRect(-10000, -10000, 10000, 10000);
	else
	{
		const CSize size = pFlashMid->GetSize();
		return CRect(-size.cx*10000/10.f/2, -size.cy*10000/10.f/2, size.cx*10000/10.f/2, size.cy*10000/10.f/2);
	}
}

void CSpotFlash::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSpot::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nScale;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nScale;
	}
}

void CSpotFlash::ReleaseCE(CArchive& ar) const
{
	CSpot::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nScale;
	}
}

void CSpotFlash::ReleaseDCOM(CArchive& ar)
{
	CSpot::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nScale;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nScale;
	}
}

void CSpotFlash::Decrease(CLibrary& library) const
{
	library.m_libSpotFlash.Decrease(m_libId);
}

DWORD CSpotFlash::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CSpot::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(short));
		size += sizeof(short);

		pFile->Write(&m_nScale, sizeof(unsigned short));
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_libId = *(short*)bytes;
		bytes += sizeof(short);

		m_nScale = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CSpotFlash::ReleaseWeb(CFile& file) const
{
	unsigned short wId = m_libId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(unsigned short));

	unsigned short nScale = m_nScale;
	ReverseOrder(nScale);
	file.Write(&nScale, sizeof(unsigned short));
}

void CSpotFlash::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_libId;
	stream<<m_nScale;
}
void CSpotFlash::ReleaseWeb(boost::json::object& json) const
{
	json["Mid"] = m_libId;
	json["Scale"] = m_nScale;
}
void CSpotFlash::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_libId);
	writer.add_variant_uint16(m_nScale);
}
