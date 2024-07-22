#include "stdafx.h"
#include "Symbol.h"
#include "SymbolSpot.h"
#include "Library.h"
#include "Line.h"
#include "FillCompact.h"
#include "Psboard.h"

#include "../Geometry/Geom.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include "../Utility/array.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSymbolSpot, CSymbol, 0)

CSymbolSpot::CSymbolSpot()
{
	m_Anchor = CPoint(0, 0);
	m_bDirection = true;

	m_wId = 0;
	m_strName = _T("Mapper");
}

CSymbolSpot::~CSymbolSpot()
{
}

void CSymbolSpot::CopyTo(CSymbol* symbol) const
{
	CSymbol::CopyTo(symbol);

	if(symbol->IsKindOf(RUNTIME_CLASS(CSymbolSpot))==TRUE)
	{
		((CSymbolSpot*)symbol)->m_Anchor = m_Anchor;
		((CSymbolSpot*)symbol)->m_bDirection = m_bDirection;
	}
}
void CSymbolSpot::ExportPs3(FILE* file) const
{
	CLibrary library;
	CPsboard psboard(library);
	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		CLine* line = (CLine*)(pGeom->m_pLine);
		CFillCompact* fill = (CFillCompact*)(pGeom->m_pFill);

		if(fill!=nullptr)
			fill->ExportPs3(file, psboard);
		if(line!=nullptr)
			line->ExportPs3(file, psboard);

		Gdiplus::Matrix matrix(0.1, 0, 0, 0.1, 0, 0);
		if(line==nullptr)
		{
			if(fill==nullptr)
			{
				if(pGeom->m_bClosed==true)
				{
				}
				else
				{
				}
			}
			else
			{
				if(pGeom->m_bClosed==true)
				{
					//					_ftprintf(file, _T("n\n"));
					pGeom->ExportPs3(file, matrix, psboard);
					_ftprintf(file, _T("f\n"));
				}
				else
				{
				}
			}
		}
		else
		{
			if(fill==nullptr)
			{
				if(pGeom->m_bClosed==true)
				{
					//					_ftprintf(file, _T("n\n"));
					pGeom->ExportPs3(file, matrix, psboard);
					_ftprintf(file, _T("s\n"));
				}
				else
				{
					//					_ftprintf(file, _T("n\n"));
					pGeom->ExportPs3(file, matrix, psboard);
					_ftprintf(file, _T("S\n"));
				}
			}
			else
			{
				if(pGeom->m_bClosed==true)
				{
					//					_ftprintf(file, _T("n\n"));
					pGeom->ExportPs3(file, matrix, psboard);
					_ftprintf(file, _T("b\n"));
				}
				else
				{
					//					_ftprintf(file, _T("n\n"));
					pGeom->ExportPs3(file, matrix, psboard);
					_ftprintf(file, _T("S\n"));
				}
			}
		}
	}
}

void CSymbolSpot::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
	Gdiplus::REAL m[6];
	matrix.GetElements(m);
	Gdiplus::Matrix matrix1(m[0]/10000.f, m[1]/10000.f, m[2]/10000.f, m[3]/10000.f, m[4], m[5]);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPs3(file, matrix1, aiKey);
	}
}

void CSymbolSpot::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const Gdiplus::Matrix& matrix) const
{
	Gdiplus::REAL m[6];
	matrix.GetElements(m);
	Gdiplus::Matrix matrix1(m[0]/10000.f, m[1]/10000.f, m[2]/10000.f, m[3]/10000.f, m[4], m[5]);

	POSITION pos = m_geomlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetNext(pos);
		pGeom->ExportPdf(pdf, page, matrix1);
	}
}

void CSymbolSpot::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSymbol::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		RecalRect();
		ar<<m_Rect;
		ar<<m_Anchor;
		ar<<m_bDirection;
	}
	else
	{
		ar>>m_Rect;
		ar>>m_Anchor;
		ar>>m_bDirection;
	}
}

void CSymbolSpot::ReleaseCE(CArchive& ar) const
{
	CSymbol::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_Rect.left;
		ar<<m_Rect.top;
		ar<<m_Rect.Width();
		ar<<m_Rect.Height();
		ar<<m_Anchor;
		ar<<m_bDirection;
	}
}

void CSymbolSpot::ReleaseDCOM(CArchive& ar)
{
	CSymbol::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_Rect;
		ar<<m_Anchor;
		ar<<m_bDirection;
	}
	else
	{
		ar>>m_Rect;
		ar>>m_Anchor;
		ar>>m_bDirection;
	}
}

void CSymbolSpot::ReleaseWeb(CFile& file) const
{
	CSymbol::ReleaseWeb(file);

	int left = m_Rect.left;
	ReverseOrder(left);
	file.Write(&left, sizeof(int));

	int top = m_Rect.top;
	ReverseOrder(top);
	file.Write(&top, sizeof(int));

	int width = m_Rect.Width();
	ReverseOrder(width);
	file.Write(&width, sizeof(int));

	int height = m_Rect.Height();
	ReverseOrder(height);
	file.Write(&height, sizeof(int));

	int x = m_Anchor.x;
	ReverseOrder(x);
	file.Write(&x, sizeof(int));

	int y = m_Anchor.y;
	ReverseOrder(y);
	file.Write(&y, sizeof(int));

	const BYTE bDirection = (BYTE)(m_bDirection==true ? 0X01 : 0X00);
	file.Write(&bDirection, sizeof(BYTE));
}

void CSymbolSpot::ReleaseWeb(BinaryStream& stream) const
{
	CSymbol::ReleaseWeb(stream);

	stream<<m_Rect.left;
	stream<<m_Rect.top;
	stream<<m_Rect.Width();
	stream<<m_Rect.Height();

	stream<<m_Anchor.x;
	stream<<m_Anchor.y;
	const BYTE bDirection = (BYTE)(m_bDirection==true ? 0X01 : 0X00);
	stream<<bDirection;
}
void CSymbolSpot::ReleaseWeb(boost::json::object& json) const
{
	CSymbol::ReleaseWeb(json);

	{
		std::vector<int> child;
		child.push_back(m_Rect.left);
		child.push_back(m_Rect.top);
		child.push_back(m_Rect.right);
		child.push_back(m_Rect.bottom);
		json["Rect"] = boost::json::value_from(child);
	}
	{
		std::vector<int> child;
		child.push_back(m_Anchor.x);
		child.push_back(m_Anchor.y);
		json["Anchor"] = boost::json::value_from(child);
	}
	json["Direction"] = m_bDirection;
}
void CSymbolSpot::ReleaseWeb(pbf::writer& writer) const
{
	CSymbol::ReleaseWeb(writer);

	{
		writer.add_variant_sint32(m_Rect.left);
		writer.add_variant_sint32(m_Rect.top);
		writer.add_variant_sint32(m_Rect.right);
		writer.add_variant_sint32(m_Rect.bottom);	
	}
	{	
		writer.add_variant_sint32(m_Anchor.x);
		writer.add_variant_sint32(m_Anchor.y);	
	}
	writer.add_byte(m_bDirection);
}