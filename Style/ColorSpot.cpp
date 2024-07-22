#include "stdafx.h"
#include "Color.h"
#include "ColorSpot.h"
#include "ColorProcess.h"
#include <io.h>
#include "../Public/Global.h"
#include "../Utility/string.hpp"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HTRANSFORM CColorSpot::m_icmToCMYK = nullptr;
HTRANSFORM CColorSpot::m_icmToMonitor = nullptr;
CColorSpot CColorSpot::_white(255, 255, 255, 255);
CColorSpot CColorSpot::_black(0, 0, 0, 255);

IMPLEMENT_SERIAL(CColorSpot, CColor, 0)

CColorSpot::CColorSpot()
	: CColor(255)
{
	m_bR = 0;
	m_bG = 0;
	m_bB = 0;
}

CColorSpot::CColorSpot(BYTE alpha)
	: CColor(alpha)
{
	m_bR = 0;
	m_bG = 0;
	m_bB = 0;
}

CColorSpot::CColorSpot(DWORD rgb)
	: CColor(255)
{
	m_bR = (rgb<<8)>>24;
	m_bG = (rgb<<16)>>24;
	m_bB = (rgb<<24)>>24;
}

CColorSpot::CColorSpot(DWORD rgb, BYTE alpha)
	: CColor(alpha)
{
	m_bR = (rgb<<8)>>24;
	m_bG = (rgb<<16)>>24;
	m_bB = (rgb<<24)>>24;
}

CColorSpot::CColorSpot(BYTE r, BYTE g, BYTE b, BYTE alpha = 255)
	: CColor(alpha)
{
	m_bR = r;
	m_bG = g;
	m_bB = b;
}

CColorSpot::CColorSpot(const char* pszHex)
	: CColor(255)
{
	if(pszHex!=nullptr&&strlen(pszHex)>=7)
	{
		const char* pszValue = (pszHex+1);
		const DWORD rgb = strtoul(pszValue, nullptr, 16);
		m_bAlpha = strlen(pszValue)>6 ? (rgb)>>24 : 0XFF;
		m_bR = (rgb<<8)>>24;
		m_bG = (rgb<<16)>>24;
		m_bB = (rgb<<24)>>24;
	}
}
void CColorSpot::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CColor::Sha1(sha1);

	sha1.process_byte(m_bR);
	sha1.process_byte(m_bG);
	sha1.process_byte(m_bB);
}
BOOL CColorSpot::operator==(const CColor& color) const
{
	if(CColor::operator==(color)==FALSE)
		return FALSE;
	else if(color.IsKindOf(RUNTIME_CLASS(CColorSpot))==FALSE)
		return FALSE;
	else if(m_bR!=((CColorSpot&)color).m_bR)
		return FALSE;
	else if(m_bG!=((CColorSpot&)color).m_bG)
		return FALSE;
	else if(m_bB!=((CColorSpot&)color).m_bB)
		return FALSE;
	else
		return TRUE;
}

void CColorSpot::CopyTo(CColor* color) const
{
	CColor::CopyTo(color);

	if(color->IsKindOf(RUNTIME_CLASS(CColorSpot))==TRUE)
	{
		((CColorSpot*)color)->m_bR = m_bR;
		((CColorSpot*)color)->m_bG = m_bG;
		((CColorSpot*)color)->m_bB = m_bB;
	}
}

void CColorSpot::LoadCMM()
{
	const DWORD dwCMM = CColor::FindCMM(_T("ADBE"));
	if(dwCMM!=-1)
	{
		SelectCMM(dwCMM);
	}

	char szColorDirectory[_MAX_PATH];
	DWORD dwSize = _MAX_PATH;
	GetColorDirectoryA(nullptr, szColorDirectory, &dwSize);
	if(IsWow64()==TRUE)
	{
		char* realdirectory = str::replace(szColorDirectory, "system32", "SysWOW64");
		if(realdirectory!=nullptr)
		{
			strcpy_s(szColorDirectory, realdirectory);
			free(realdirectory);
			if(PathFileExistsA(szColorDirectory)==FALSE)
			{
				CreateDeepDirectory(szColorDirectory);
			}
		}
	}

	char szAdobeRGB1998[_MAX_PATH];
	strcpy_s(szAdobeRGB1998, szColorDirectory);
	strcat_s(szAdobeRGB1998, "\\AdobeRGB1998.icc");
	if(_access(szAdobeRGB1998, 00)==-1)
	{
		CopyFileA(GetExeDirectory()+"Color\\AdobeRGB1998.icc", szAdobeRGB1998, FALSE);
	}

	{
		char szUSWebCoatedSWOP[_MAX_PATH];
		strcpy_s(szUSWebCoatedSWOP, szColorDirectory);
		strcat_s(szUSWebCoatedSWOP, "\\USWebCoatedSWOP.icc");
		if(_access(szUSWebCoatedSWOP, 00)==-1)
		{
			CopyFileA(GetExeDirectory()+"Color\\USWebCoatedSWOP.icc", szUSWebCoatedSWOP, FALSE);
		}
		PROFILE profile1; //Destination profile 
		PROFILE profile2; //Destination profile 
		profile1.dwType = PROFILE_FILENAME;
		profile2.dwType = PROFILE_FILENAME;
		profile1.pProfileData = szAdobeRGB1998;
		profile2.pProfileData = szUSWebCoatedSWOP;
		profile1.cbDataSize = strlen(szAdobeRGB1998);
		profile2.cbDataSize = strlen(szUSWebCoatedSWOP);
		const HPROFILE hProfile1 = OpenColorProfileA(&profile1, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
		const HPROFILE hProfile2 = OpenColorProfileA(&profile2, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
		if(hProfile1!=nullptr&&hProfile2!=nullptr)
		{
			HPROFILE hProfiles[2];
			DWORD intent = INTENT_PERCEPTUAL;

			hProfiles[0] = hProfile1;
			hProfiles[1] = hProfile2;
			const HTRANSFORM hICMxform = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
			if(hICMxform!=nullptr)
			{
				CColorSpot::m_icmToCMYK = hICMxform;
			}
		}
		CloseColorProfile(hProfile1);
		CloseColorProfile(hProfile2);
	}

	{
		char szsRGBColorSpace[_MAX_PATH];
		strcpy_s(szsRGBColorSpace, szColorDirectory);
		strcat_s(szsRGBColorSpace, "\\sRGB Color Space Profile.icm");
		if(_access(szsRGBColorSpace, 00)==-1)
		{
			CopyFileA(GetExeDirectory()+"Color\\sRGB Color Space Profile.icm", szsRGBColorSpace, FALSE);
		}

		PROFILE profile1; //Destination profile 
		PROFILE profile2; //Destination profile 
		profile1.dwType = PROFILE_FILENAME;
		profile2.dwType = PROFILE_FILENAME;
		profile1.pProfileData = szAdobeRGB1998;
		profile2.pProfileData = szsRGBColorSpace;
		profile1.cbDataSize = strlen(szAdobeRGB1998);
		profile2.cbDataSize = strlen(szsRGBColorSpace);
		const HPROFILE hProfile1 = OpenColorProfileA(&profile1, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
		const HPROFILE hProfile2 = OpenColorProfileA(&profile2, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
		if(hProfile1!=nullptr&&hProfile2!=nullptr)
		{
			HPROFILE hProfiles[2];
			DWORD intent = INTENT_PERCEPTUAL;

			hProfiles[0] = hProfile1;
			hProfiles[1] = hProfile2;
			const HTRANSFORM hICMxform = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
			if(hICMxform!=nullptr)
			{
				CColorSpot::m_icmToMonitor = hICMxform;
			}
		}
		else
		{
		}
		CloseColorProfile(hProfile1);
		CloseColorProfile(hProfile2);
	}
}

void CColorSpot::ReleaseCMM()
{
	if(CColorSpot::m_icmToCMYK!=nullptr)
	{
		//	DeleteColorTransform(CColorSpot::m_icmToCMYK);
		//	CloseHandle(CColorSpot::m_icmToCMYK);
		CColorSpot::m_icmToCMYK = nullptr;
	}
	if(CColorSpot::m_icmToMonitor!=nullptr)
	{
		//	DeleteColorTransform(CColorSpot::m_icmToMonitor);
		//	CloseHandle(CColorSpot::m_icmToMonitor);
		CColorSpot::m_icmToMonitor = nullptr;
	}
}

Gdiplus::Color CColorSpot::GetColor() const
{
	return Gdiplus::Color(m_bAlpha, m_bR, m_bG, m_bB);
}

COLORREF CColorSpot::GetMonitorRGB() const
{
	if(CColorSpot::m_icmToMonitor!=nullptr)
	{
		COLOR ic;
		COLOR oc;
		ic.rgb.red = m_bR*0XFF;
		ic.rgb.green = m_bG*0XFF;
		ic.rgb.blue = m_bB*0XFF;
		if(TranslateColors(CColorSpot::m_icmToMonitor, &ic, 1, COLOR_RGB, &oc, COLOR_RGB)==TRUE)
		{
			BYTE R = oc.rgb.red>>8;
			BYTE G = oc.rgb.green>>8;
			BYTE B = oc.rgb.blue>>8;
			return RGB(R, G, B);
		}
		else
			return RGB(m_bR, m_bG, m_bB);
	}
	else
		return RGB(m_bR, m_bG, m_bB);
}

COLORREF CColorSpot::ToRGB() const
{
	return RGB(m_bR, m_bG, m_bB);
}

COLORREF CColorSpot::ToHSV() const
{
	float s = 0;
	float h = -1;
	const float v = max(m_bR, m_bG, m_bB);
	if(v==0)// m_bR = m_bG = m_bB = 0		// s = 0, v is undefined 
	{
		s = 0;
		h = -1;
	}
	else
	{
		const float chroma = v-min(m_bR, m_bG, m_bB);
		float s = chroma/v;
		float factor = 0;
		if(m_bR==v)
			factor = (m_bG-m_bB)/chroma; // between yellow & magenta
		else if(m_bG==v)
			factor = 2+(m_bB-m_bR)/chroma; // between cyan & yellow
		else
			factor = 4+(m_bR-m_bG)/chroma; // between magenta & cyan

		h = (int)(factor*60+360)%360; // degrees
	}

	return RGB(m_bR, m_bG, m_bB);
}

DWORD CColorSpot::ToCMYK() const
{
	DWORD C = (255-m_bR)*100/255;
	DWORD M = (255-m_bG)*100/255;
	DWORD Y = (255-m_bB)*100/255;
	DWORD K = min(C, M, Y);
	C = min(100, max(0, C-K));
	M = min(100, max(0, M-K));
	Y = min(100, max(0, Y-K));
	K = min(100, max(0, K));

	if(CColorSpot::m_icmToCMYK!=nullptr)
	{
		COLOR ic;
		COLOR oc;
		ic.rgb.red = m_bR*0XFF;
		ic.rgb.green = m_bG*0XFF;
		ic.rgb.blue = m_bB*0XFF;
		if(TranslateColors(CColorSpot::m_icmToCMYK, &ic, 1, COLOR_RGB, &oc, COLOR_CMYK)==TRUE)
		{
			C = (oc.cmyk.cyan/65535.f)*100;
			M = (oc.cmyk.magenta/65535.f)*100;
			Y = (oc.cmyk.yellow/65535.f)*100;
			K = (oc.cmyk.black/65535.f)*100;
		}
	}

	DWORD CMYK = 0B00000000000000000000000000000000;
	CMYK |= C<<24;
	CMYK |= M<<16;
	CMYK |= Y<<8;
	CMYK |= K<<0;

	return CMYK;
}

void CColorSpot::SetRGB(BYTE r, BYTE g, BYTE b)
{
	m_bR = r;
	m_bG = g;
	m_bB = b;
}

void CColorSpot::SetRGB(COLORREF color)
{
	m_bR = GetRValue(color);
	m_bG = GetGValue(color);
	m_bB = GetBValue(color);
}

void CColorSpot::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CColor::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_bR;
		ar<<m_bG;
		ar<<m_bB;
	}
	else
	{
		ar>>m_bR;
		ar>>m_bG;
		ar>>m_bB;
	}
}

void CColorSpot::ReleaseCE(CArchive& ar) const
{
	CColor::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_bR;
		ar<<m_bG;
		ar<<m_bB;
	}
}

void CColorSpot::ReleaseDCOM(CArchive& ar)
{
	CColor::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_bR;
		ar<<m_bG;
		ar<<m_bB;
	}
	else
	{
		ar>>m_bR;
		ar>>m_bG;
		ar>>m_bB;
	}
}

DWORD CColorSpot::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CColor::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_bR, sizeof(BYTE));
		pFile->Write(&m_bG, sizeof(BYTE));
		pFile->Write(&m_bB, sizeof(BYTE));

		size += 3*sizeof(BYTE);

		return size;
	}
	else
	{
		m_bR = *bytes;
		bytes += sizeof(BYTE);

		m_bG = *bytes;
		bytes += sizeof(BYTE);

		m_bB = *bytes;
		bytes += sizeof(BYTE);
		return 0;
	}
}

void CColorSpot::ExportPs3(FILE* file) const
{
	const DWORD cmyk = ToCMYK();

	DWORD c = cmyk;
	DWORD m = cmyk;
	DWORD y = cmyk;
	DWORD k = cmyk;

	c = c<<0;
	c = c>>24;

	m = m<<8;
	m = m>>24;

	y = y<<16;
	y = y>>24;

	k = k<<24;
	k = k>>24;
	const float C = (float)c/100;
	const float M = (float)m/100;
	const float Y = (float)y/100;
	const float K = (float)k/100;

	_ftprintf(file, _T("[%g %g %g %g]"), C, M, Y, K);
}

void CColorSpot::ExportPdf(HPDF_Page page, bool bFill) const
{
	const float R = m_bR/255.f;
	const float G = m_bG/255.f;
	const float B = m_bB/255.f;

	bFill==true ? HPDF_Page_SetRGBFill(page, R, G, B) : HPDF_Page_SetRGBStroke(page, R, G, B);
}

CColorSpot* CColorSpot::White()
{
	return  &_white;
}
CColorSpot* CColorSpot::Black()
{
	return  &_black;
}