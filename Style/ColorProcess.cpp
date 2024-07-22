#include "stdafx.h"
#include <io.h>

#include "Color.h"
#include "ColorProcess.h"

#include "../Public/Global.h"
#include "../Utility/string.hpp"
#include "../../Thirdparty/libharu/2.4.4/include/hpdf.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HTRANSFORM CColorProcess::m_icmToRGB = nullptr;
HTRANSFORM CColorProcess::m_icmToMonitor = nullptr;

IMPLEMENT_SERIAL(CColorProcess, CColor, 0)
CColorProcess::CColorProcess()
	: CColor(255)
{
	m_bC = 0;
	m_bM = 0;
	m_bY = 0;
	m_bK = 0;
}
CColorProcess::CColorProcess(BYTE alpha)
	: CColor(alpha)
{
	m_bC = 0;
	m_bM = 0;
	m_bY = 0;
	m_bK = 0;
}

CColorProcess::CColorProcess(BYTE c, BYTE m, BYTE y, BYTE k, BYTE alpha=255)
	: CColor(alpha)
{
	m_bC = c;
	m_bM = m;
	m_bY = y;
	m_bK = k;
}
void CColorProcess::Sha1(boost::uuids::detail::sha1& sha1) const
{
	CColor::Sha1(sha1);

	sha1.process_byte(m_bC);
	sha1.process_byte(m_bM);
	sha1.process_byte(m_bY);
	sha1.process_byte(m_bK);
}
BOOL CColorProcess::operator==(const CColor& color) const
{
	if(CColor::operator==(color)==FALSE)
		return FALSE;
	else if(color.IsKindOf(RUNTIME_CLASS(CColorProcess))==FALSE)
		return FALSE;
	else if(m_bC!=((CColorProcess&)color).m_bC)
		return FALSE;
	else if(m_bM!=((CColorProcess&)color).m_bM)
		return FALSE;
	else if(m_bY!=((CColorProcess&)color).m_bY)
		return FALSE;
	else if(m_bK!=((CColorProcess&)color).m_bK)
		return FALSE;
	else
		return TRUE;
}

void CColorProcess::CopyTo(CColor* color) const
{
	CColor::CopyTo(color);

	if(color->IsKindOf(RUNTIME_CLASS(CColorProcess))==TRUE)
	{
		((CColorProcess*)color)->m_bC = m_bC;
		((CColorProcess*)color)->m_bM = m_bM;
		((CColorProcess*)color)->m_bY = m_bY;
		((CColorProcess*)color)->m_bK = m_bK;
	}
}

void CColorProcess::LoadCMM()
{
	const DWORD dwCMM = CColor::FindCMM(_T("ADBE"));
	if(dwCMM!=-1)
	{
		SelectCMM(dwCMM);
	}

	char szColorDirectory[_MAX_PATH];
	DWORD dwSize = _MAX_PATH;
	GetColorDirectoryA(nullptr, szColorDirectory, &dwSize); //wrong on 64-bit window
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

	char szUSWebCoatedSWOP[_MAX_PATH];
	strcpy_s(szUSWebCoatedSWOP, szColorDirectory);
	strcat_s(szUSWebCoatedSWOP, "\\USWebCoatedSWOP.icc");
	if(_access(szUSWebCoatedSWOP, 00)==-1)
	{
		CopyFileA(GetExeDirectory()+"Color\\USWebCoatedSWOP.icc", szUSWebCoatedSWOP, FALSE);
	}
	{
		char szAdobeRGB1998[_MAX_PATH];
		strcpy_s(szAdobeRGB1998, szColorDirectory);
		strcat_s(szAdobeRGB1998, "\\AdobeRGB1998.icc");
		if(_access(szAdobeRGB1998, 00)==-1)
		{
			CopyFileA(GetExeDirectory()+"Color\\AdobeRGB1998.icc", szAdobeRGB1998, FALSE);
		}

		PROFILE profile1; //Destination profile 
		PROFILE profile2; //Destination profile 
		profile1.dwType = PROFILE_FILENAME;
		profile2.dwType = PROFILE_FILENAME;
		profile1.pProfileData = szUSWebCoatedSWOP;
		profile2.pProfileData = szAdobeRGB1998;
		profile1.cbDataSize = strlen(szUSWebCoatedSWOP);
		profile2.cbDataSize = strlen(szAdobeRGB1998);
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
				CColorProcess::m_icmToRGB = hICMxform;
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
		profile1.pProfileData = szUSWebCoatedSWOP;
		profile2.pProfileData = szsRGBColorSpace;
		profile1.cbDataSize = strlen(szUSWebCoatedSWOP);
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
				CColorProcess::m_icmToMonitor = hICMxform;
			}
		}
		CloseColorProfile(hProfile1);
		CloseColorProfile(hProfile2);
	}
}

void CColorProcess::ReleaseCMM()
{
	if(CColorProcess::m_icmToRGB!=nullptr)
	{
	//	DeleteColorTransform(CColorProcess::m_icmToRGB);
		CColorProcess::m_icmToRGB = nullptr;
	}
	if(CColorProcess::m_icmToMonitor!=nullptr)
	{
	//	DeleteColorTransform(CColorProcess::m_icmToMonitor);
		CColorProcess::m_icmToMonitor = nullptr;
	}
}

Gdiplus::Color CColorProcess::GetColor() const
{
	const COLORREF rgb = this->GetMonitorRGB();

	return Gdiplus::Color(m_bAlpha, GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
}

COLORREF CColorProcess::GetMonitorRGB() const
{
	const double c = m_bC/100.f;
	const double m = m_bM/100.f;
	const double y = m_bY/100.f;
	const double k = m_bK/100.f;
	const double r = 1-((0.2*c+1-0.25*m-0.25*y-0.7*k)*c+(0.09+0.08*y-0.05*k)*m+(-0.05+0.05*k)*y+(0.15*k+0.7)*k);
	const double g = 1-((0.34-0.3*m-0.18*k)*c+(0.1*m+0.8-0.05*y-0.62*k)*m+(0.1-0.1*k)*y+(0.15*k+0.7)*k);
	const double b = 1-((0.09-0.1*m-0.1*y)*c+(0.48-0.3*y-0.2*k)*m+(0.1*y+0.8-0.74*k)*y+(0.15*k+0.7)*k);
	const BYTE R = max(0, min(round(r*255), 255));
	const BYTE G = max(0, min(round(g*255), 255));
	const BYTE B = max(0, min(round(b*255), 255));
	//if(CColorProcess::m_icmToMonitor!=nullptr)
	//{		
	//	COLOR ic;
	//	COLOR oc;
	//	ic.cmyk.cyan = (m_bC/100.f)*0XFFFF;
	//	ic.cmyk.magenta = (m_bM/100.f)*0XFFFF;
	//	ic.cmyk.yellow = (m_bY/100.f)*0XFFFF;
	//	ic.cmyk.black = (m_bK/100.f)*0XFFFF;
	//	if(TranslateColors(CColorProcess::m_icmToMonitor, &ic, 1, COLOR_CMYK, &oc, COLOR_RGB)==TRUE)
	//	{
	//		R = oc.rgb.red/256.f;
	//		G = oc.rgb.green/256.f;
	//		B = oc.rgb.blue/256.f;
	//	}
	//	
	//}
	return RGB(R, G, B);
}

COLORREF CColorProcess::ToRGB() const
{
	double c = m_bC/100.f;
	double m = m_bM/100.f;
	double y = m_bY/100.f;
	double k = m_bK/100.f;
	double r = 1-((0.2*c+1-0.25*m-0.25*y-0.7*k)*c+(0.09+0.08*y-0.05*k)*m+(-0.05+0.05*k)*y+(0.15*k+0.7)*k);
	double g = 1-((0.34-0.3*m-0.18*k)*c+(0.1*m+0.8-0.05*y-0.62*k)*m+(0.1-0.1*k)*y+(0.15*k+0.7)*k);
	double b = 1-((0.09-0.1*m-0.1*y)*c+(0.48-0.3*y-0.2*k)*m+(0.1*y+0.8-0.74*k)*y+(0.15*k+0.7)*k);
	BYTE R = max(0, min(round(r*255), 255));
	BYTE G = max(0, min(round(g*255), 255));
	BYTE B = max(0, min(round(b*255), 255));
	if(CColorProcess::m_icmToRGB!=nullptr)
	{
		COLOR ic;
		COLOR oc;
		ic.cmyk.cyan = (m_bC/100.f)*0XFFFF;
		ic.cmyk.magenta = (m_bM/100.f)*0XFFFF;
		ic.cmyk.yellow = (m_bY/100.f)*0XFFFF;
		ic.cmyk.black = (m_bK/100.f)*0XFFFF;
		if(TranslateColors(CColorProcess::m_icmToRGB, &ic, 1, COLOR_CMYK, &oc, COLOR_RGB)==TRUE)
		{
			R = oc.rgb.red/256.f;
			G = oc.rgb.green/256.f;
			B = oc.rgb.blue/256.f;
		}
	}
	return RGB(R, G, B);
}

DWORD CColorProcess::ToCMYK() const
{
	const DWORD C = m_bC;
	const DWORD M = m_bM;
	const DWORD Y = m_bY;
	const DWORD K = m_bK;

	DWORD cmyk = 0B00000000000000000000000000000000;

	cmyk |= C<<24;
	cmyk |= M<<16;
	cmyk |= Y<<8;
	cmyk |= K<<0;

	return cmyk;
}

void CColorProcess::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CColor::Serialize(ar, dwVersion);

	if(ar.IsStoring())
	{
		ar<<m_bC;
		ar<<m_bM;
		ar<<m_bY;
		ar<<m_bK;
	}
	else
	{
		ar>>m_bC;
		ar>>m_bM;
		ar>>m_bY;
		ar>>m_bK;
	}
}

void CColorProcess::ReleaseCE(CArchive& ar) const
{
	CColor::ReleaseCE(ar);

	if(ar.IsStoring())
	{
		ar<<m_bC;
		ar<<m_bM;
		ar<<m_bY;
		ar<<m_bK;
	}
}

void CColorProcess::ReleaseDCOM(CArchive& ar)
{
	CColor::ReleaseDCOM(ar);

	if(ar.IsStoring())
	{
		ar<<m_bC;
		ar<<m_bM;
		ar<<m_bY;
		ar<<m_bK;
	}
	else
	{
		ar>>m_bC;
		ar>>m_bM;
		ar>>m_bY;
		ar>>m_bK;
	}
}

DWORD CColorProcess::PackPC(CFile* pFile, BYTE*& bytes)
{
	DWORD size = CColor::PackPC(pFile, bytes);

	if(pFile!=nullptr)
	{
		pFile->Write(&m_bC, sizeof(BYTE));
		pFile->Write(&m_bM, sizeof(BYTE));
		pFile->Write(&m_bY, sizeof(BYTE));
		pFile->Write(&m_bK, sizeof(BYTE));

		size += 4*sizeof(BYTE);

		return size;
	}
	else
	{
		m_bC = *bytes;
		bytes += sizeof(BYTE);

		m_bM = *bytes;
		bytes += sizeof(BYTE);

		m_bY = *bytes;
		bytes += sizeof(BYTE);

		m_bK = *bytes;
		bytes += sizeof(BYTE);
		return 0;
	}
}

void CColorProcess::ExportPs3(FILE* file) const
{
	const float C = (float)m_bC/100;
	const float M = (float)m_bM/100;
	const float Y = (float)m_bY/100;
	const float K = (float)m_bK/100;
	_ftprintf(file, _T("[%g %g %g %g]"), C, M, Y, K);
}

void CColorProcess::ExportPdf(HPDF_Page page, bool bFill) const
{
	const float C = (float)m_bC/100;
	const float M = (float)m_bM/100;
	const float Y = (float)m_bY/100;
	const float K = (float)m_bK/100;
	bFill==true ? HPDF_Page_SetCMYKFill(page, C, M, Y, K) : HPDF_Page_SetCMYKStroke(page, C, M, Y, K);
}
