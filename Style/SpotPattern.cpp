#include "stdafx.h"

#include "PatternMid.h"
#include "Spot.h"
#include "SpotPattern.h"
#include "Library.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSpotPattern, CSpot, 0)

CSpotPattern::CSpotPattern()
{
	m_libId = -1;
	m_nIndex = 0;
	m_nScale = 100;
}

CSpotPattern::~CSpotPattern()
{
}
void CSpotPattern::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CSpot::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_nIndex, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
}
BOOL CSpotPattern::operator==(const CSpot& spot) const
{
	if(CSpot::operator==(spot)==FALSE)
		return FALSE;
	else if(spot.IsKindOf(RUNTIME_CLASS(CSpotPattern))==FALSE)
		return FALSE;
	else if(m_libId!=((CSpotPattern*)&spot)->m_libId)
		return FALSE;
	else if(m_nIndex!=((CSpotPattern*)&spot)->m_nIndex)
		return FALSE;
	else if(m_nScale!=((CSpotPattern*)&spot)->m_nScale)
		return FALSE;
	else
		return TRUE;
}

void CSpotPattern::CopyTo(CSpot* spot) const
{
	CSpot::CopyTo(spot);

	if(spot->IsKindOf(RUNTIME_CLASS(CSpotPattern))==TRUE)
	{
		((CSpotPattern*)spot)->m_libId = m_libId;
		((CSpotPattern*)spot)->m_nIndex = m_nIndex;
		((CSpotPattern*)spot)->m_nScale = m_nScale;
	}
}

void CSpotPattern::Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const
{
	CPatternMid* pPattern = (CPatternMid*)library.m_libSpotPattern.GetMid(m_libId);
	if(pPattern==nullptr)
		return;
	
	pPattern->Draw(g, m_nIndex, (fZoom*m_nScale)/100);
}

CRect CSpotPattern::GetWanRect(const CLibrary& library) const
{
	CPatternMid* pPattern = (CPatternMid*)library.m_libSpotPattern.GetMid(m_libId);
	if(pPattern==nullptr)
		return CRect(-10000, -10000, 10000, 10000);
	else
	{
		const CSize size = pPattern->GetSize();
		return CRect(-size.cx*10000/10.f/2, -size.cy*10000/10.f/2, size.cx*10000/10.f/2, size.cy*10000/10.f/2);
	}
}

void CSpotPattern::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSpot::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nIndex;
		ar>>m_nScale;
	}
}

void CSpotPattern::ExportPs3(FILE* file, const CLibrary& library) const
{
}

void CSpotPattern::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
}

void CSpotPattern::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CLibrary& library, const Gdiplus::Matrix& matrix) const
{
}

void CSpotPattern::ReleaseCE(CArchive& ar) const
{
	CSpot::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
	}
}

void CSpotPattern::ReleaseDCOM(CArchive& ar)
{
	CSpot::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nIndex;
		ar>>m_nScale;
	}
}

void CSpotPattern::Decrease(CLibrary& library) const
{
	library.m_libSpotPattern.Decrease(m_libId);
}

DWORD CSpotPattern::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CSpot::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(short));
		size += sizeof(short);
		pFile->Write(&m_nIndex, sizeof(short));
		size += sizeof(short);

		pFile->Write(&m_nScale, sizeof(unsigned short));
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_libId = *(short*)bytes;
		bytes += sizeof(short);
		m_nIndex = *(short*)bytes;
		bytes += sizeof(short);

		m_nScale = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CSpotPattern::ReleaseWeb(CFile& file) const
{
	unsigned short wId = m_libId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(unsigned short));

	unsigned short nPatternIndex = m_nIndex;
	ReverseOrder(nPatternIndex);
	file.Write(&nPatternIndex, sizeof(unsigned short));

	unsigned short nScale = m_nScale;
	ReverseOrder(nScale);
	file.Write(&nScale, sizeof(unsigned short));
}

void CSpotPattern::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_libId;
	stream<<m_nIndex;
	stream<<m_nScale;
}
void CSpotPattern::ReleaseWeb(boost::json::object& json) const
{
	json["Mid"] = m_libId;
	json["Index"] = m_nIndex;
	json["Scale"] = m_nScale;
}
void CSpotPattern::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_libId);
	writer.add_variant_uint16(m_nIndex);
	writer.add_variant_uint16(m_nScale);
}