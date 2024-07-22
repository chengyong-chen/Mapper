#include "stdafx.h"
#include "Spot.h"
#include "SpotSymbol.h"
#include "Symbol.h"
#include "SymbolSpot.h"
#include "SymbolMid.hpp"
#include "Library.h"
#include "Psboard.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include "../Utility/array.hpp"
#include "../Utility/monochrome.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSpotSymbol, CSpot, 0)

CSpotSymbol::CSpotSymbol()
{
	m_libId = -1;
	m_symId = -1;
	m_nScale = 100;
	m_nAngle = 0;
}

CSpotSymbol::~CSpotSymbol()
{
}
void CSpotSymbol::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CSpot::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_symId, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
	sha1.process_bytes(&m_nAngle, sizeof(unsigned short));
}
BOOL CSpotSymbol::operator==(const CSpot& spot) const
{
	if(CSpot::operator==(spot)==FALSE)
		return FALSE;
	else if(spot.IsKindOf(RUNTIME_CLASS(CSpotSymbol))==FALSE)
		return FALSE;
	else if(m_libId!=((CSpotSymbol*)&spot)->m_libId)
		return FALSE;
	else if(m_symId!=((CSpotSymbol*)&spot)->m_symId)
		return FALSE;
	else if(m_nScale!=((CSpotSymbol*)&spot)->m_nScale)
		return FALSE;
	else
		return TRUE;
}
std::tuple<int, int, const Gdiplus::Bitmap*, bool> CSpotSymbol::GetBitmap1(const CLibrary& library, const float& fSin, const float& fCos, float fZoom, const CSize& dpi) const
{
	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return {0, 0, nullptr, true};

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol==nullptr)
		return {0, 0, nullptr, true};

	std::tuple<int, int, const Gdiplus::Bitmap*> tuple = pSymbol->GetBitmap1(fZoom*m_nScale/100.f, dpi);
	if(std::get<2>(tuple) == nullptr)
		return {0, 0, nullptr, true};
	else if(fSin==0 && fCos==1)
		return {std::get<0>(tuple), std::get<1>(tuple), (Gdiplus::Bitmap*)std::get<2>(tuple), true};
	else
	{
		const Gdiplus::Matrix matrix(fCos, fSin, -fSin, fCos, -std::get<0>(tuple), -std::get<1>(tuple));
		//	Gdiplus::Bitmap* gBitmap = bitmap::Transforma(std::get<2>(tuple), nullptr);
		//	return {std::get<0>(tuple),std::get<1>(tuple),gBitmap,false};
		return {0, 0, nullptr, true};
	}
}
std::tuple<int, int, HBITMAP, bool> CSpotSymbol::GetBitmap2(const CLibrary& library, const float& fSin, const float& fCos, float fZoom, const CSize& dpi) const
{
	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return {0, 0, nullptr, true};

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol==nullptr)
		return {0, 0, nullptr, true};

	std::tuple<int, int, HBITMAP> tuple = pSymbol->GetBitmap2(fZoom*m_nScale/100.f, dpi);
	if(std::get<2>(tuple) == nullptr)
		return {0, 0, nullptr, true};
	else if(fSin==0 && fCos==1)
		return {std::get<0>(tuple), std::get<1>(tuple), (HBITMAP)std::get<2>(tuple), true};
	else
	{
		const Gdiplus::Matrix matrix(fCos, fSin, -fSin, fCos, -std::get<0>(tuple), -std::get<1>(tuple));
		HBITMAP hBitmap = monochrome::Transform(std::get<2>(tuple), matrix);
		return {std::get<0>(tuple), std::get<1>(tuple), hBitmap, false};
	}
}
void CSpotSymbol::CopyTo(CSpot* spot) const
{
	CSpot::CopyTo(spot); 

	if(spot->IsKindOf(RUNTIME_CLASS(CSpotSymbol))==TRUE)
	{
		((CSpotSymbol*)spot)->m_libId = m_libId;
		((CSpotSymbol*)spot)->m_symId = m_symId;
		((CSpotSymbol*)spot)->m_nScale = m_nScale;
		((CSpotSymbol*)spot)->m_nAngle = m_nAngle;
	}
}

void CSpotSymbol::Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const
{
	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		pSymbol->Draw(g, fSin, fCos, fZoom*m_nScale/100.f, dpi);
	}
}

void CSpotSymbol::ExportPs3(FILE* file, const CLibrary& library) const
{
	_ftprintf(file, _T("/SpotSymbol {\n"));

	_ftprintf(file, _T("u\n"));

	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid!=nullptr)
	{
		CSymbol* pSymbol = pMid->GetSymbol(m_symId);
		if(pSymbol!=nullptr)
		{
			const float s = (float)m_nScale/100;
			_ftprintf(file, _T("%g %g scale\n"), s, s);

			pSymbol->ExportPs3(file);
		}
	}

	_ftprintf(file, _T("U\n"));
	_ftprintf(file, _T("} def\n"));
}

void CSpotSymbol::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	CSymbolMid<CSymbolSpot>* pMid = aiKey.m_library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		_ftprintf(file, _T("u\n"));
		const float scale = (float)m_nScale/100;

		Gdiplus::REAL m[6];
		matrix.GetElements(m);
		Gdiplus::Matrix matrix1(m[0]*scale, m[1]*scale, m[2]*scale, m[3]*scale, m[4], m[5]);
		((CSymbolSpot*)pSymbol)->ExportPs3(file, matrix1, aiKey);

		_ftprintf(file, _T("U\n"));
	}
}

void CSpotSymbol::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CLibrary& library, const Gdiplus::Matrix& matrix) const
{
	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		const float scale = (float)m_nScale/100;

		Gdiplus::REAL m[6];
		matrix.GetElements(m);
		Gdiplus::Matrix matrix1(m[0]*scale, m[1]*scale, m[2]*scale, m[3]*scale, m[4], m[5]);
		((CSymbolSpot*)pSymbol)->ExportPdf(pdf, page, matrix1);
	}
}

CRect CSpotSymbol::GetWanRect(const CLibrary& library) const
{
	CSymbolMid<CSymbolSpot>* pMid = library.m_libSpotSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return CRect(-10000, -10000, 10000, 10000);

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol==nullptr)
		return CRect(-10000, -10000, 10000, 10000);
	else
		return pSymbol->GetRect();
}

void CSpotSymbol::Decrease(CLibrary& library) const
{
	library.m_libSpotSymbol.Decrease(m_libId);
}

void CSpotSymbol::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSpot::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_symId;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

void CSpotSymbol::ReleaseCE(CArchive& ar) const
{
	CSpot::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
}

void CSpotSymbol::ReleaseDCOM(CArchive& ar)
{
	CSpot::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_symId;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

DWORD CSpotSymbol::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CSpot::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(WORD));
		size += sizeof(WORD);
		pFile->Write(&m_symId, sizeof(WORD));
		size += sizeof(WORD);
		pFile->Write(&m_nScale, sizeof(unsigned short));
		size += sizeof(unsigned short);

		pFile->Write(&m_nAngle, sizeof(unsigned short));
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_libId = *(WORD*)bytes;
		bytes += sizeof(WORD);
		m_symId = *(WORD*)bytes;
		bytes += sizeof(WORD);

		m_nScale = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_nAngle = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CSpotSymbol::ReleaseWeb(CFile& file) const
{
	WORD tagId = m_libId;
	ReverseOrder(tagId);
	file.Write(&tagId, sizeof(WORD));
	WORD symId = m_symId;
	ReverseOrder(symId);
	file.Write(&symId, sizeof(WORD));

	unsigned short nScale = m_nScale;
	ReverseOrder(nScale);
	file.Write(&nScale, sizeof(unsigned short));

	unsigned short nAngle = m_nAngle;
	ReverseOrder(nAngle);
	file.Write(&nAngle, sizeof(unsigned short));
}

void CSpotSymbol::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_libId;
	stream<<m_symId;
	stream<<m_nScale;
	stream<<m_nAngle;
}
void CSpotSymbol::ReleaseWeb(boost::json::object& json) const
{
	json["Mid"] = m_libId;
	json["Sym"] = m_symId;
	json["Scale"] = m_nScale;
	json["Angle"] = m_nAngle;
}
void CSpotSymbol::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_libId);
	writer.add_variant_uint16(m_symId);
	writer.add_variant_uint16(m_nScale);
	writer.add_variant_uint16(m_nAngle);
}