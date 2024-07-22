#include "stdafx.h"
#include "Symbol.h"
#include "SymbolFill.h"
#include "Library.h"
#include "Line.h"
#include "FillCompact.h"
#include "Psboard.h"
#include "../Geometry/Global.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSymbolFill, CSymbol, 0)

CSymbolFill::CSymbolFill()
{
	m_wId = 0;
	m_strName = _T("Mapper");
}

CSymbolFill::~CSymbolFill()
{
}

void CSymbolFill::ExportPs3(FILE* file) const
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
			if(fill = nullptr)
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

void CSymbolFill::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSymbol::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		RecalRect();
		ar<<m_Rect;
	}
	else
	{
		ar>>m_Rect;
	}
}

void CSymbolFill::ReleaseCE(CArchive& ar) const
{
	CSymbol::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_Rect.left;
		ar<<m_Rect.top;
		ar<<m_Rect.Width();
		ar<<m_Rect.Height();
	}
}

void CSymbolFill::ReleaseDCOM(CArchive& ar)
{
	CSymbol::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_Rect;
	}
	else
	{
		ar>>m_Rect;
	}
}

void CSymbolFill::ReleaseWeb(CFile& file) const
{
	CSymbol::ReleaseWeb(file);

	int x = m_Rect.left;
	ReverseOrder(x);
	file.Write(&x, sizeof(int));

	int y = m_Rect.top;
	ReverseOrder(y);
	file.Write(&y, sizeof(int));

	int width = m_Rect.Width();
	ReverseOrder(width);
	file.Write(&width, sizeof(int));

	int height = m_Rect.Height();
	ReverseOrder(height);
	file.Write(&height, sizeof(int));
}

void CSymbolFill::ReleaseWeb(BinaryStream& stream) const
{
	CSymbol::ReleaseWeb(stream);

	stream<<m_Rect.left;
	stream<<m_Rect.top;
	stream<<m_Rect.Width();
	stream<<m_Rect.Height();
}
void CSymbolFill::ReleaseWeb(boost::json::object& json) const
{
	CSymbol::ReleaseWeb(json);

	std::vector<int> child;
	child.push_back(m_Rect.left);
	child.push_back(m_Rect.top);
	child.push_back(m_Rect.right);
	child.push_back(m_Rect.bottom);
	json["Rect"] = boost::json::value_from(child);
}
void CSymbolFill::ReleaseWeb(pbf::writer& writer) const
{
	CSymbol::ReleaseWeb(writer);
		
	writer.add_variant_sint32(m_Rect.left);
	writer.add_variant_sint32(m_Rect.top);
	writer.add_variant_sint32(m_Rect.right);
	writer.add_variant_sint32(m_Rect.bottom);
}
void CSymbolFill::RecalRect()
{
	m_Rect.SetRectEmpty();

	POSITION pos1 = m_geomlist.GetTailPosition();
	while(pos1!=nullptr)
	{
		CGeom* pGeom = m_geomlist.GetPrev(pos1);
		m_Rect = UnionRect(m_Rect, pGeom->m_Rect);
	}
}
