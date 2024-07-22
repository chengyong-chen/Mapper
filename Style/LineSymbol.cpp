#include "stdafx.h"
#include "Line.h"
#include "LineSymbol.h"
#include "Symbol.h"
#include "SymbolLine.h"
#include "Library.h"
#include "SymbolMid.hpp"
#include "Psboard.h"

#include "../Geometry/Geom.h"
#include "../Geometry/Poly.h"
#include "../Geometry/Mark.h"
#include "../Geometry/Pole.h"
#include "../Dataview/viewinfo.h"
#include "../Public/BinaryStream.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CLineSymbol, CLine, 0)

CLineSymbol::CLineSymbol()
{
	m_libId = -1;
	m_symId = -1;
	m_nScale = 100;
}

CLineSymbol::~CLineSymbol()
{
}
void CLineSymbol::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CLine::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_symId, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
}
BOOL CLineSymbol::operator==(const CLine& line) const
{
	if(CLine::operator==(line) == FALSE)
		return FALSE;
	else if(line.IsKindOf(RUNTIME_CLASS(CLineSymbol)) == FALSE)
		return FALSE;
	else if(m_libId != ((CLineSymbol*)&line)->m_libId)
		return FALSE;
	else if(m_symId != ((CLineSymbol*)&line)->m_symId)
		return FALSE;
	else if(m_nScale != ((CLineSymbol*)&line)->m_nScale)
		return FALSE;
	else
		return TRUE;
}

void CLineSymbol::CopyTo(CLine* line) const
{
	CLine::CopyTo(line);

	if(line->IsKindOf(RUNTIME_CLASS(CLineSymbol)) == TRUE)
	{
		((CLineSymbol*)line)->m_libId = m_libId;
		((CLineSymbol*)line)->m_symId = m_symId;
		((CLineSymbol*)line)->m_nScale = m_nScale;
	}
}

Gdiplus::Pen* CLineSymbol::GetPen(const float& fZoom, const CSize& dpi) const
{
	return nullptr;
}

void CLineSymbol::Draw(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fStyle) const
{
	CSymbolMid<CSymbolLine>* pMid = library.m_libLineSymbol.GetMid(m_libId);
	if(pMid == nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol != nullptr)
	{
		Context context;
		context.ratioLine = fStyle*m_nScale/100.f;
		context.tolerance = viewinfo.GetPrecision(10);
		((CSymbolLine*)pSymbol)->Draw(g, viewinfo, pPath, context, m_nScale/100.f);
	}
}
void CLineSymbol::Preoccupy(Gdiplus::Graphics& g, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath, float fStyle) const
{
	CSymbolMid<CSymbolLine>* pMid = library.m_libLineSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		Context context;
		context.ratioLine = fStyle*m_nScale/100.f;
		context.tolerance = viewinfo.GetPrecision(10);
		((CSymbolLine*)pSymbol)->Preoccupy(g, viewinfo, pPath, context, m_nScale/100.f);
	}
}
void CLineSymbol::SetWidth(const CMidtable<CSymbolMid<CSymbolLine>>& tagtable)
{
	CSymbolMid<CSymbolLine>* pMid = tagtable.GetMid(m_libId);
	if(pMid == nullptr)
		m_nWidth = 0;
	else
	{
		CSymbol* pSymbol = pMid->GetSymbol(m_symId);
		if(pSymbol == nullptr)
			m_nWidth = 0;
		else
			m_nWidth = ((CSymbolLine*)pSymbol)->m_nWidth;
	}
}

void CLineSymbol::Decrease(CLibrary& library) const
{
	library.m_libLineSymbol.Decrease(m_libId);
}

void CLineSymbol::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar << m_libId;
		ar << m_symId;
		ar << m_nScale;
	}
	else
	{
		ar >> m_libId;
		ar >> m_symId;
		ar >> m_nScale;
	}
}

void CLineSymbol::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar << m_libId;
		ar << m_symId;
		ar << m_nScale;
		ar << m_nWidth;
	}
}

void CLineSymbol::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar << m_libId;
		ar << m_symId;
		ar << m_nScale;
	}
	else
	{
		ar >> m_libId;
		ar >> m_symId;
		ar >> m_nScale;
	}
}

DWORD CLineSymbol::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile != nullptr)
	{
		DWORD size = 0;
		pFile->Write(&m_libId, sizeof(WORD));
		pFile->Write(&m_symId, sizeof(WORD));
		pFile->Write(&m_nScale, sizeof(unsigned short));
		pFile->Write(&m_nWidth, sizeof(unsigned short));

		size += sizeof(WORD);
		size += sizeof(WORD);
		size += sizeof(unsigned short);
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

		m_nWidth = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CLineSymbol::ReleaseWeb(CFile& file) const
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

	unsigned short nWidth = m_nWidth;
	ReverseOrder(nWidth);
	file.Write(&nWidth, sizeof(unsigned short));
}

void CLineSymbol::ReleaseWeb(BinaryStream& stream) const
{
	stream << m_libId;
	stream << m_symId;
	stream << m_nScale;
	stream << m_nWidth;
}
void CLineSymbol::ReleaseWeb(boost::json::object& json) const
{
	json["Mid"] = m_libId;
	json["Sym"] = m_symId;
	json["Scale"] = m_nScale;
	json["Width"] = m_nWidth;
}
void CLineSymbol::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_libId);
	writer.add_variant_uint16(m_symId);
	writer.add_variant_uint16(m_nScale);
	writer.add_variant_uint16(m_nWidth);
}
void CLineSymbol::ExportPs3(FILE* file, const CLibrary& library) const
{
	CSymbolMid<CSymbolLine>* pMid = library.m_libLineSymbol.GetMid(m_libId);
	if(pMid == nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol != nullptr)
	{
		CPsboard psboard(library);
		long nLine = 0;
		long nPole = 0;
		POSITION pos = pSymbol->m_geomlist.GetHeadPosition();
		while(pos != nullptr)
		{
			CGeom* geom = pSymbol->m_geomlist.GetNext(pos);
			if(geom->IsKindOf(RUNTIME_CLASS(CPoly)))
			{
				nLine++;

				CLine* line = geom->m_pLine;
				_ftprintf(file, _T("/line%d {\n"), nLine);
				if(line != nullptr)
				{
					line->ExportPs3(file, psboard);
				}
				_ftprintf(file, _T("} def\n"));
			}
			else if(geom->IsKindOf(RUNTIME_CLASS(CPole)))
			{
				nPole++;

				_ftprintf(file, _T("/pole%d {\n"), nPole);
				Gdiplus::Matrix matrix(0.1, 0, 0, 0.1, 0, 0);
				geom->ExportPs3(file, matrix, psboard);
				_ftprintf(file, _T("} def\n"));
			}
		}
	}
}

void CLineSymbol::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CPath* pPath) const
{
	CSymbolMid<CSymbolLine>* pMid = aiKey.m_library.m_libLineSymbol.GetMid(m_libId);
	if(pMid == nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol != nullptr)
	{
		((CSymbolLine*)pSymbol)->ExportPs3(file, viewinfo, aiKey, pPath);
	}
}

void CLineSymbol::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CViewinfo& viewinfo, const CLibrary& library, const CPath* pPath) const
{
	CSymbolMid<CSymbolLine>* pMid = library.m_libLineSymbol.GetMid(m_libId);
	if(pMid == nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol != nullptr)
	{
		((CSymbolLine*)pSymbol)->ExportPdf(pdf, page, viewinfo, pPath);
	}
}
