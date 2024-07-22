#include "stdafx.h"
#include "Color.h"
#include "ColorGray.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

IMPLEMENT_SERIAL(CColorGray, CColor, 0)

CColorGray::CColorGray()
	: CColor(255)
{
	m_gray = 100;
}

CColorGray::CColorGray(BYTE gray, BYTE alpha)
	: CColor(alpha)
{
	m_gray = gray;
}
void CColorGray::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CColor::Sha1(sha1);

	sha1.process_byte(m_gray);
}
BOOL CColorGray::operator==(const CColor& color) const
{
	if(CColor::operator==(color)==FALSE)
		return FALSE;
	else if(color.IsKindOf(RUNTIME_CLASS(CColorGray))==FALSE)
		return FALSE;
	else if(m_gray!=((CColorGray&)color).m_gray)
		return FALSE;
	else
		return TRUE;
}

void CColorGray::CopyTo(CColor* color) const
{
	CColor::CopyTo(color);

	if(color->IsKindOf(RUNTIME_CLASS(CColorGray))==TRUE)
	{
		((CColorGray*)color)->m_gray = m_gray;
	}
}

Gdiplus::Color CColorGray::GetColor() const
{
	return Gdiplus::Color(m_bAlpha, m_gray, m_gray, m_gray);
}

COLORREF CColorGray::ToRGB() const
{
	return RGB(m_gray, m_gray, m_gray);
}

DWORD CColorGray::ToCMYK() const
{
	return 0;
}

void CColorGray::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CColor::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_gray;
	}
	else
	{
		ar>>m_gray;
	}
}

void CColorGray::ReleaseCE(CArchive& ar) const
{
	CColor::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_gray;
	}
}

void CColorGray::ReleaseDCOM(CArchive& ar)
{
	CColor::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_gray;
	}
	else
	{
		ar>>m_gray;
	}
}

DWORD CColorGray::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CColor::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_gray, sizeof(BYTE));

		size += sizeof(BYTE);
		return size;
	}
	else
	{
		m_gray = *bytes;
		bytes += sizeof(BYTE);
		return 0;
	}
}

void CColorGray::ExportPs3(FILE* file) const
{
	//	float C = m_bR/100;
	//	float M = m_bG/100;
	//	float Y = m_B/100;
	//	fprintf(file, _T("[%g %g %g %g] "),C,M,Y,K);
}

void CColorGray::ExportPdf(HPDF_Page page, bool bFill) const
{
	const float T = m_gray/100.f;

	bFill==true ? HPDF_Page_SetGrayFill(page, T) : HPDF_Page_SetGrayStroke(page, T);
}
