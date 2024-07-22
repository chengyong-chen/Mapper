#include "stdafx.h"
#include "FillComplex.h"
#include "Mid.h"
#include "PatternMid.h"
#include "FillPattern.h"
#include "Library.h"
#include "Psboard.h"

#include "../Geometry/Geom.h"
#include "../Geometry/PRect.h"
#include "../Public/BinaryStream.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CFillPattern, CFillComplex, 0)

CFillPattern::CFillPattern()
{
	m_libId = -1;
	m_nIndex = 0;
	m_nScale = 100;
	m_nAngle = 0;
}

CFillPattern::~CFillPattern()
{
}
void CFillPattern::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CFill::Sha1(sha1);

	sha1.process_byte(m_libId);
	sha1.process_byte(m_nIndex);
	sha1.process_bytes(&m_nScale, sizeof(unsigned short));
	sha1.process_bytes(&m_nAngle, sizeof(unsigned short));
}
BOOL CFillPattern::operator==(const CFill& fill) const
{
	if(CFillComplex::operator==(fill)==FALSE)
		return FALSE;
	else if(fill.IsKindOf(RUNTIME_CLASS(CFillPattern))==FALSE)
		return FALSE;
	else if(m_libId!=((CFillPattern*)&fill)->m_libId)
		return FALSE;
	else if(m_nIndex!=((CFillPattern*)&fill)->m_nIndex)
		return FALSE;
	else if(m_nScale!=((CFillPattern*)&fill)->m_nScale)
		return FALSE;
	else if(m_nAngle!=((CFillPattern*)&fill)->m_nAngle)
		return FALSE;
	else
		return TRUE;
}

void CFillPattern::CopyTo(CFill* fill) const
{
	CFillComplex::CopyTo(fill);

	if(fill->IsKindOf(RUNTIME_CLASS(CFillPattern))==TRUE)
	{
		((CFillPattern*)fill)->m_libId = m_libId;
		((CFillPattern*)fill)->m_nIndex = m_nIndex;
		((CFillPattern*)fill)->m_nScale = m_nScale;
		((CFillPattern*)fill)->m_nAngle = m_nAngle;
	}
}

Gdiplus::Brush* CFillPattern::GetBrush(const float& fRatio, const CSize& dpi) const
{
	return nullptr;
}

Gdiplus::Brush* CFillPattern::GetBrush(CLibrary& library, const float& fRatio, const CSize& dpi) const
{
	CPatternMid* pPatternMid = (CPatternMid*)library.m_libFillPattern.GetMid(m_libId);
	if(pPatternMid==nullptr)
		return nullptr;

	Gdiplus::Bitmap* pBitmap = pPatternMid->GetBitmap(m_nIndex);
	if(pBitmap==nullptr)
		return nullptr;

	Gdiplus::TextureBrush* brush = ::new Gdiplus::TextureBrush(pBitmap, Gdiplus::WrapMode::WrapModeTile);
	if(m_nScale!=100)
		brush->ScaleTransform(m_nScale/100.f, m_nScale/100.f);
	if(m_nAngle!=0)
		brush->RotateTransform(m_nAngle);

	delete pBitmap;
	pBitmap = nullptr;

	return brush;
}

void CFillPattern::ExportPs3(FILE* file, const CViewinfo& viewinfo, CPsboard& aiKey, const CGeom* pGeom) const
{
	if(pGeom->m_bClosed==false)
		return;

	CPatternMid* pattern = (CPatternMid*)aiKey.m_library.m_libFillPattern.GetMid(m_libId);
	if(pattern==nullptr)
		return;

	_ftprintf(file, _T("/FillPattern {\n"));

	//	pattern->ExportEps(file);

	_ftprintf(file, _T("} def\n"));
	const CRect ORect = pGeom->m_Rect;
	const CPRect IRect(ORect, nullptr, nullptr);

	CRect sRect(0, 0, 0, 0);// = Pattern->GetRect();
	sRect.left = (long)(sRect.left*(float)m_nScale/100);
	sRect.top = (long)(sRect.top*(float)m_nScale/100);
	sRect.right = (long)(sRect.right*(float)m_nScale/100);
	sRect.bottom = (long)(sRect.bottom*(float)m_nScale/100);
	const CSize sSize = sRect.Size();
	const long lcol = ORect.left%sSize.cx<0 ? ORect.left/sSize.cx-1 : ORect.left/sSize.cx;
	const long rcol = ORect.right%sSize.cx<=0 ? ORect.right/sSize.cx-1 : ORect.right/sSize.cx;
	const long trow = ORect.top%sSize.cy<0 ? ORect.top/sSize.cy-1 : ORect.top/sSize.cy;
	const long brow = ORect.bottom%sSize.cy<=0 ? ORect.bottom/sSize.cy-1 : ORect.bottom/sSize.cy;

	CPoint origin;
	origin.x = lcol*sSize.cx-sRect.left;
	origin.y = trow*sSize.cy-sRect.top;

	_ftprintf(file, _T("vms\n"));
	_ftprintf(file, _T("u\n"));

	if(m_nScale!=100)
	{
		const double m11 = m_nScale/100.0f;
		const double m12 = 0.0f;
		const double m21 = 0.0f;
		const double m22 = m_nScale/100.0f;
		const double m31 = origin.x;
		const double m32 = origin.y;

		_ftprintf(file, _T("[%g %g %g %g %g %g] concat\n"), m11, m12, m21, m22, m31, m32);
	}
	else
	{
		_ftprintf(file, _T("%g %g translate\n"), origin.x, origin.y);
	}
	const CRect srect;// = Pattern->GetRect();
	for(long row = trow; row<=brow; row++)
	{
		for(long col = lcol; col<=rcol; col++)
		{
			CRect rect = sRect;
			rect.OffsetRect(col*sSize.cx, row*sSize.cy);
			rect.OffsetRect(-sRect.left, -sRect.top);

			if(IRect.PickIn(rect)==true)
			{
				_ftprintf(file, _T("q\n"));
				_ftprintf(file, _T("FillPattern\n"));
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

void CFillPattern::Decrease(CLibrary& library) const
{
	library.m_libFillPattern.Decrease(m_libId);
}

void CFillPattern::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CFillComplex::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nIndex;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

void CFillPattern::ReleaseDCOM(CArchive& ar)
{
CFillComplex:ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
	else
	{
		ar>>m_libId;
		ar>>m_nIndex;
		ar>>m_nScale;
		ar>>m_nAngle;
	}
}

void CFillPattern::ReleaseCE(CArchive& ar) const
{
	CFillComplex:ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_libId;
		ar<<m_nIndex;
		ar<<m_nScale;
		ar<<m_nAngle;
	}
}

DWORD CFillPattern::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = 0;//CFillComplex::PackPC(pFile,bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_libId, sizeof(BYTE));
		pFile->Write(&m_nIndex, sizeof(BYTE));
		pFile->Write(&m_nScale, sizeof(unsigned short));
		pFile->Write(&m_nAngle, sizeof(unsigned short));

		size += sizeof(BYTE);
		size += sizeof(BYTE);
		size += sizeof(unsigned short);
		size += sizeof(unsigned short);

		return size;
	}
	else
	{
		m_libId = *(BYTE*)bytes;
		bytes += sizeof(BYTE);
		m_nIndex = *(BYTE*)bytes;
		bytes += sizeof(BYTE);

		m_nScale = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		m_nAngle = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);
		return 0;
	}
}

void CFillPattern::ReleaseWeb(CFile& file) const
{
	//	CFillComplex:ReleaseWeb(file);

	file.Write(&m_libId, sizeof(BYTE));
	file.Write(&m_nIndex, sizeof(BYTE));

	unsigned short nScale = m_nScale;
	ReverseOrder(nScale);
	file.Write(&nScale, sizeof(unsigned short));

	unsigned short nAngle = m_nAngle;
	ReverseOrder(nAngle);
	file.Write(&m_nAngle, sizeof(unsigned short));
}


void CFillPattern::ReleaseWeb(BinaryStream& stream) const
{
	//	CFillComplex:ReleaseWeb(file);

	stream<<m_libId;
	stream<<m_nIndex;

	stream<<m_nScale;
	stream<<m_nAngle;
}
void CFillPattern::ReleaseWeb(boost::json::object& json) const
{
	//	CFillComplex:ReleaseWeb(file);
	json["Mid"] = m_libId;
	json["Index"] = m_nIndex;
	json["Scale"] = m_nScale;
	json["Angle"] = m_nAngle;
}

void CFillPattern::ReleaseWeb(pbf::writer& writer) const
{
	//	CFillComplex:ReleaseWeb(file);

	writer.add_byte(m_libId);
	writer.add_byte(m_nIndex);
	writer.add_variant_int16(m_nScale);
	writer.add_variant_int16(m_nAngle);
}

