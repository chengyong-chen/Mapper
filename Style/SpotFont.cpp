#include "stdafx.h"

#include "Spot.h"
#include "SpotFont.h"
#include "Color.h"

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Dataview/viewinfo.h"
#include "../Pbf/writer.hpp"
#include "../Utility/cstring.hpp"

#include "FontDesc.h"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_SERIAL(CSpotFont, CSpot, 0)

CSpotFont::CSpotFont()
	:m_nSize(120), m_wId(40)
{
	m_strFontReal = _T("Wingdings");
	m_strFontReal = _T("Wingdings");
	m_pColor = nullptr;
}

CSpotFont::~CSpotFont()
{
	delete m_pColor;
}
void CSpotFont::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CSpot::Sha1(sha1);

	cstr::sha1(m_strFontReal, sha1);
	cstr::sha1(m_strFontReal, sha1);
	sha1.process_bytes(&m_nSize, sizeof(unsigned short));
	sha1.process_bytes(&m_wId, sizeof(unsigned short));
	if(m_pColor!=nullptr)
	{
		m_pColor->Sha1(sha1);
	}
}

BOOL CSpotFont::operator==(const CSpot& spot) const
{
	if(CSpot::operator==(spot)==FALSE)
		return FALSE;
	else if(spot.IsKindOf(RUNTIME_CLASS(CSpotFont))==FALSE)
		return FALSE;
	else if(m_pColor!=nullptr&&((CSpotFont*)&spot)->m_pColor!=nullptr&&*m_pColor!=*((CSpotFont*)&spot)->m_pColor)
		return FALSE;
	else if(m_nSize!=((CSpotFont*)&spot)->m_nSize)
		return FALSE;
	else if(m_strFontReal!=((CSpotFont*)&spot)->m_strFontReal)
		return FALSE;
	else if(m_wId!=((CSpotFont*)&spot)->m_wId)
		return FALSE;
	else
		return TRUE;
}

void CSpotFont::CopyTo(CSpot* spot) const
{
	CSpot::CopyTo(spot);

	if(spot->IsKindOf(RUNTIME_CLASS(CSpotFont))==TRUE)
	{
		delete ((CSpotFont*)spot)->m_pColor;
		((CSpotFont*)spot)->m_pColor = nullptr;
		if(m_pColor!=nullptr)
		{
			((CSpotFont*)spot)->m_pColor = m_pColor->Clone();
		}

		((CSpotFont*)spot)->m_wId = m_wId;
		((CSpotFont*)spot)->m_nSize = m_nSize;
		((CSpotFont*)spot)->m_strFontReal = m_strFontReal;
		((CSpotFont*)spot)->m_strFontFace = m_strFontFace;
	}
}

void CSpotFont::Draw(Gdiplus::Graphics& g, const CLibrary& library, const float& fSin, const float& fCos, const float& fZoom, const CSize& dpi) const
{
	const _bstr_t btrFont(m_strFontFace);
	const Gdiplus::FontFamily fontFamily(btrFont);
	::SysFreeString(btrFont);

	const Gdiplus::SizeF fontSize = CViewinfo::PointsToPixels(m_nSize*fZoom/10.f, dpi);
	const Gdiplus::Font font(&fontFamily, fontSize.Height, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
	const UINT16 emHeight = fontFamily.GetEmHeight(Gdiplus::FontStyleRegular);
	const UINT16 emLineSpacing = fontFamily.GetLineSpacing(Gdiplus::FontStyleRegular);
	const UINT16 emAscent = fontFamily.GetCellAscent(Gdiplus::FontStyleRegular);
	const UINT16 emDescent = fontFamily.GetCellDescent(Gdiplus::FontStyleRegular);

	Gdiplus::Color textcolor(0, 0, 0);
	if(m_pColor!=nullptr)
	{
		textcolor = m_pColor->GetColor();
	}
	const Gdiplus::SolidBrush textbrush(textcolor);

	CString strChar;
	strChar.Format(_T("%c"), m_wId);

	Gdiplus::StringFormat stringFormat;
	stringFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
	stringFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
	if(fSin!=0.0f)
	{
		const _bstr_t bstr(strChar);
		g.DrawString(bstr, -1, &font, Gdiplus::PointF(0, -(emLineSpacing-(emAscent+emDescent)/2)*fontSize.Width/emHeight), &stringFormat, &textbrush);//����̫��ȷ
	}
	else
	{
		const _bstr_t bstr(strChar);
		g.DrawString(bstr, -1, &font, Gdiplus::PointF(0, 0), &stringFormat, &textbrush);
	}
}
//CRect CSpotFont::GetWanRect(CLibrary& library) const
//{
//	return CRect(-m_nSize*10000/10.f/2,-m_nSize*10000/10.f/2,m_nSize*10000/10.f/2,m_nSize*10000/10.f/2);
//}

void CSpotFont::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CSpot::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_wId;
		ar<<m_nSize;
		ar<<m_strFontReal;
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}
	else
	{
		ar>>m_wId;
		ar>>m_nSize;
		ar>>m_strFontReal;

		BYTE colorindex = -1;
		ar>>colorindex;
		delete m_pColor;
		m_pColor = CColor::Apply(colorindex);

		m_strFontFace = CFontDesc::GetFaceByReal(m_strFontReal);
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->Serialize(ar, dwVersion);
	}
}

CRect CSpotFont::GetWanRect(const CLibrary& library) const
{
	return CRect(-m_nSize*10000/10.f/2, -m_nSize*10000/10.f/2, m_nSize*10000/10.f/2, m_nSize*10000/10.f/2);
}

void CSpotFont::ExportPs3(FILE* file, const CLibrary& library) const
{
}

void CSpotFont::ExportPs3(FILE* file, const Gdiplus::Matrix& matrix, CPsboard& aiKey) const
{
}

void CSpotFont::ExportPdf(HPDF_Doc pdf, HPDF_Page page, const CLibrary& library, const Gdiplus::Matrix& matrix) const
{
}

void CSpotFont::ReleaseDCOM(CArchive& ar)
{
	CSpot::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_wId;
		ar<<m_nSize;
		ar<<m_strFontReal;
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}
	else
	{
		ar>>m_wId;
		ar>>m_nSize;
		ar>>m_strFontReal;

		BYTE colorindex = -1;
		ar>>colorindex;
		delete m_pColor;
		m_pColor = CColor::Apply(colorindex);
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseDCOM(ar);
	}
}

void CSpotFont::ReleaseCE(CArchive& ar) const
{
	CSpot::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_wId;
		ar<<m_nSize;

		SerializeStrCE(ar, m_strFontReal);
		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		ar<<colortype;
	}

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseCE(ar);
	}
}

DWORD CSpotFont::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CSpot::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_wId, sizeof(WORD));
		size += sizeof(WORD);

		pFile->Write(&m_nSize, sizeof(unsigned short));
		size += sizeof(unsigned short);
		const unsigned short length = PackStrPC(m_strFontReal, pFile);
		size += length;

		const BYTE colortype = m_pColor==nullptr ? -1 : m_pColor->Gettype();
		pFile->Write(&colortype, sizeof(BYTE));
		size += sizeof(BYTE);

		if(m_pColor!=nullptr)
		{
			BYTE* bytes = nullptr;
			size += m_pColor->PackPC(pFile, bytes);
		}

		return size;
	}
	else
	{
		m_wId = *(WORD*)bytes;
		bytes += sizeof(WORD);

		m_nSize = *(unsigned short*)bytes;
		bytes += sizeof(unsigned short);

		m_strFontReal = UnpackStrPC(bytes);

		delete m_pColor;
		const BYTE colorindex = *(BYTE*)bytes;
		bytes += sizeof(BYTE);
		m_pColor = CColor::Apply(colorindex);

		if(m_pColor!=nullptr)
		{
			m_pColor->PackPC(nullptr, bytes);
		}
		return 0;
	}
}

void CSpotFont::ReleaseWeb(CFile& file) const
{
	WORD wId = m_wId;
	ReverseOrder(wId);
	file.Write(&wId, sizeof(WORD));

	unsigned short nSize = m_nSize;
	ReverseOrder(nSize);
	file.Write(&nSize, sizeof(unsigned short));

	SaveAsUTF8(file, m_strFontReal);

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseWeb(file);
	}
	else
	{
		const BYTE alpha = 255;
		file.Write(&alpha, sizeof(BYTE));

		const DWORD color = 0B00000000000000000000000000000000;
		file.Write(&color, sizeof(DWORD));
	}
}

void CSpotFont::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_wId;
	stream<<m_nSize;
	SaveAsUTF8(stream, m_strFontReal);

	if(m_pColor!=nullptr)
	{
		m_pColor->ReleaseWeb(stream);
	}
	else
	{
		const BYTE alpha = 255;
		stream<<alpha;
		const DWORD color = 0B00000000000000000000000000000000;
		stream<<color;
	}
}
void CSpotFont::ReleaseWeb(boost::json::object& json) const
{
	json["Id"] = m_wId;
	json["Size"] = m_nSize;
	json["Font"] = CStringA(m_strFontReal);

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(json);
	else
		json["Color"] = "rgba(0,0,0,1)";
}

void CSpotFont::ReleaseWeb(pbf::writer& writer) const
{
	writer.add_variant_uint16(m_wId);
	writer.add_variant_uint16(m_nSize);
	writer.add_string(m_strFontReal);

	if(m_pColor != nullptr)
		m_pColor->ReleaseWeb(writer);
	else
		writer.add_string("rgba(0,0,0,1)");
}
