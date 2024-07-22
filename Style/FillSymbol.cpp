#include "stdafx.h"
#include <math.h>

#include "FillComplex.h"
#include "FillSymbol.h"

#include "Symbol.h"
#include "SymbolFill.h"
#include "Library.h"
#include "SymbolMid.hpp"

#include "FillSymbolDlg.h"
#include "Psboard.h"

#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Dataview/Viewinfo.h"
#include "../Public/BinaryStream.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillSymbol, CFillComplex, 0)

CFillSymbol::CFillSymbol()
{
	m_libId = -1;
	m_symId = -1;
	m_nScale = 100;
	m_nAngle = 0;
}

CFillSymbol::~CFillSymbol()
{
}
void CFillSymbol::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	sha1.process_bytes(&m_libId, sizeof(unsigned short));
	sha1.process_bytes(&m_symId, sizeof(unsigned short));
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
	sha1.process_bytes(&m_nAngle, sizeof(unsigned short));
}
BOOL CFillSymbol::operator==(const CFill& fill) const
{
	if(CFillComplex::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillSymbol))==FALSE)
		return FALSE;
	else if(m_libId!=((CFillSymbol*)&fill)->m_libId)
		return FALSE;
	else if(m_symId!=((CFillSymbol*)&fill)->m_symId)
		return FALSE;
	else if(m_nScale!=((CFillSymbol*)&fill)->m_nScale)
		return FALSE;
	else if(m_nAngle!=((CFillSymbol*)&fill)->m_nAngle)
		return FALSE;
	else
		return TRUE;
}

void CFillSymbol::CopyTo(CFill* fill) const
{
	CFillComplex::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillSymbol))==TRUE)
	{
		((CFillSymbol*)fill)->m_libId = m_libId;
		((CFillSymbol*)fill)->m_symId = m_symId;
		((CFillSymbol*)fill)->m_nScale = m_nScale;
		((CFillSymbol*)fill)->m_nAngle = m_nAngle;
	}
}

Gdiplus::Brush* CFillSymbol::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return nullptr;
}

Gdiplus::Brush* CFillSymbol::GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const
{
	CSymbolMid<CSymbolFill>* pMid = library.m_libFillSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return nullptr;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol==nullptr)
		return nullptr;

	std::tuple<int, int, const Gdiplus::Bitmap*> cache = pSymbol->GetBitmap1(fRatio*m_nScale/100.f, dpi);	
	if(std::get<2>(cache)==nullptr)
		return nullptr;

	const Gdiplus::Bitmap* gBitmap = std::get<2>(cache);
	Gdiplus::TextureBrush* brush = ::new Gdiplus::TextureBrush((Gdiplus::Bitmap*)gBitmap, Gdiplus::WrapMode::WrapModeTile);
	if(m_nAngle!=0)
	{
		brush->RotateTransform(m_nAngle);
	}	
	return brush;
}

void CFillSymbol::ExportPs3(FILE* file, const CLibrary& library) const
{
	CSymbolMid<CSymbolFill>* pMid = library.m_libFillSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		_ftprintf(file, _T("/FillSymbol {\n"));
		pSymbol->ExportPs3(file);
		_ftprintf(file, _T("} def\n"));
	}
}

void CFillSymbol::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const
{
	if(pGeom->m_bClosed==false)
		return;

	CSymbolMid<CSymbolFill>* pMid = aiKey.m_library.m_libFillSymbol.GetMid(m_libId);
	if(pMid==nullptr)
		return;

	CSymbol* pSymbol = pMid->GetSymbol(m_symId);
	if(pSymbol!=nullptr)
	{
		double m11 = 1;
		double m12 = 0;
		double m21 = 0;
		double m22 = 1;
		double m31 = 0;
		double m32 = 0;

		CRect ORect = pGeom->m_Rect;
		CPRect IRect(ORect, nullptr, nullptr);

		CRect symbolRect = pSymbol->GetRect();
		symbolRect.left = (long)(symbolRect.left*(float)m_nScale/100);
		symbolRect.top = (long)(symbolRect.top*(float)m_nScale/100);
		symbolRect.right = (long)(symbolRect.right*(float)m_nScale/100);
		symbolRect.bottom = (long)(symbolRect.bottom*(float)m_nScale/100);
		const CSize symbolSize = symbolRect.Size();
		CSize xDelta = CSize(symbolSize.cx, 0);
		CSize yDelta = CSize(0, symbolSize.cy);

		if(m_nAngle!=0||m_nScale!=100)
		{
			const double angle = m_nAngle/180*M_PI;

			m11 = cos(angle);
			m12 = sin(angle);
			m21 = -sin(angle);
			m22 = cos(angle);
			m31 = 0;
			m32 = 0;

			xDelta.cy = (long)(xDelta.cx*m12);
			xDelta.cx = (long)(xDelta.cx*m11);

			yDelta.cx = (long)(yDelta.cy*m21);
			yDelta.cy = (long)(yDelta.cy*m22);

			IRect.Transform(m11, -m21, m31, -m12, m22, m32);
			ORect = IRect.m_Rect;
		}
		const long lcol = ORect.left%symbolSize.cx<0 ? ORect.left/symbolSize.cx-1 : ORect.left/symbolSize.cx;
		const long rcol = ORect.right%symbolSize.cx<=0 ? ORect.right/symbolSize.cx-1 : ORect.right/symbolSize.cx;
		const long trow = ORect.top%symbolSize.cy<0 ? ORect.top/symbolSize.cy-1 : ORect.top/symbolSize.cy;
		const long brow = ORect.bottom%symbolSize.cy<=0 ? ORect.bottom/symbolSize.cy-1 : ORect.bottom/symbolSize.cy;

		CPoint origin;
		origin.x = lcol*symbolSize.cx-symbolRect.left;
		origin.y = trow*symbolSize.cy-symbolRect.top;

		_ftprintf(file, _T("vms\n"));
		_ftprintf(file, _T("u\n"));

		if(m_nAngle!=0||m_nScale!=100)
		{
			m31 = origin.x*m11+origin.y*m21;
			m32 = origin.x*m12+origin.y*m22;

			m11 *= (float)m_nScale/100;
			m12 *= (float)m_nScale/100;
			m21 *= (float)m_nScale/100;
			m22 *= (float)m_nScale/100;

			_ftprintf(file, _T("[%.4f %.4f %.4f %.4f %.2f %.2f] concat\n"), m11, m12, m21, m22, m31, m32);
		}
		else
		{
			_ftprintf(file, _T("%d %d translate\n"), origin.x, origin.y);
		}
		const CRect srect = pSymbol->GetRect();
		for(long row = trow; row<=brow; row++)
		{
			for(long col = lcol; col<=rcol; col++)
			{
				CRect rect = symbolRect;
				rect.OffsetRect(col*symbolSize.cx, row*symbolSize.cy);
				rect.OffsetRect(-symbolRect.left, -symbolRect.top);

				if(IRect.PickIn(rect)==true)
				{
					_ftprintf(file, _T("q\n"));
					_ftprintf(file, _T("FillSymbol\n"));
					_ftprintf(file, _T("Q\n"));
				}
				_ftprintf(file, _T("%d 0 translate\n"), srect.Width());
			}
			const long countCol = rcol-lcol+1;
			_ftprintf(file, _T("%d %d mul neg %d translate\n"), countCol, srect.Width(), srect.Height());
		}

		_ftprintf(file, _T("U\n"));

		_ftprintf(file, _T("vmr\n"));
	}
}

void CFillSymbol::Decrease(CLibrary& library) const
{
	library.m_libFillSymbol.Decrease(m_libId);
}

void CFillSymbol::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillComplex::Serialize(ar, dwVersion);

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

void CFillSymbol::ReleaseCE(CArchive& ar) const
{
	//	CFillComplex::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_symId;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
}

void CFillSymbol::ReleaseDCOM(CArchive& ar)
{
	//	CFillComplex::ReleaseDCOM(ar);

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

DWORD CFillSymbol::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = 0;//CFillComplex::PackPC(pFile,bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(WORD));
		pFile->Write(&m_symId, sizeof(WORD));
		pFile->Write(&m_nScale, sizeof(unsigned short));
		pFile->Write(&m_nAngle, sizeof(unsigned short));

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

		m_nAngle = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CFillSymbol::ReleaseWeb(CFile& file) const
{
	//	CFillComplex:ReleaseWeb(file);

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
	file.Write(&m_nAngle, sizeof(unsigned short));
}

void CFillSymbol::ReleaseWeb(BinaryStream& stream) const
{
	//	CFillComplex:ReleaseWeb(file);
	stream<<m_libId;
	stream<<m_symId;
	stream<<m_nScale;
	stream<<m_nAngle;
}
void CFillSymbol::ReleaseWeb(boost::json::object& json) const
{
	//	CFillComplex:ReleaseWeb(file);
	json["Mid"] = m_libId;
	json["Sym"] = m_symId;
	json["Scale"] = m_nScale;
	json["Angle"] = m_nAngle;
}
void CFillSymbol::ReleaseWeb(pbf::writer& writer) const
{
	//	CFillComplex:ReleaseWeb(file);

	writer.add_byte(m_libId);
	writer.add_byte(m_symId);
	writer.add_variant_int16(m_nScale);
	writer.add_variant_int16(m_nAngle);
}