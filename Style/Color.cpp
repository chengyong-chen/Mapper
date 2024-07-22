#include "stdafx.h"
#include "Color.h"

#include "ColorProcess.h"
#include "ColorSpot.h"
#include "ColorGray.h"
#include "Psboard.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CColor, CObject)

CColor::CColor(BYTE alpha)
{
	m_bId = 0;
	m_bAlpha = alpha;
}

CColor::~CColor()
{
}
void CColor::Sha1(boost::uuids::detail::sha1& sha1) const
{
	sha1.process_byte(m_bAlpha);
}
BOOL CColor::operator==(const CColor& color) const
{
	CRuntimeClass* pRuntimeClass1 = this->GetRuntimeClass();
	CRuntimeClass* pRuntimeClass2 = color.GetRuntimeClass();
	if(pRuntimeClass1!=pRuntimeClass2)
		return FALSE;
	else if(m_bAlpha!=color.m_bAlpha)
		return FALSE;
	else
		return TRUE;
}

BOOL CColor::operator !=(const CColor& color) const
{
	return !(*this==color);
}

CColor* CColor::Clone() const
{
	CRuntimeClass* pRuntimeClass = this->GetRuntimeClass();
	CColor* pClone = (CColor*)pRuntimeClass->CreateObject();
	this->CopyTo(pClone);
	return pClone;
}

void CColor::CopyTo(CColor* color) const
{
	color->m_bAlpha = m_bAlpha;
}

void CColor::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	CString strName;
	if(ar.IsStoring())
	{
		ar<<strName;
		ar<<m_bId;
		ar<<m_bAlpha;
	}
	else
	{
		ar>>strName;
		ar>>m_bId;
		ar>>m_bAlpha;
	}
}

void CColor::ReleaseCE(CArchive& ar) const
{
	if(ar.IsStoring())
	{
		ar<<m_bId;
		ar<<m_bAlpha;
	}
}

void CColor::ReleaseDCOM(CArchive& ar)
{
	if(ar.IsStoring())
	{
		ar<<m_bId;
		ar<<m_bAlpha;
	}
	else
	{
		ar>>m_bId;
		ar>>m_bAlpha;
	}
}

DWORD CColor::PackPC(CFile* pFile, BYTE*& bytes)
{
	pFile->Write(&m_bId, sizeof(BYTE));
	pFile->Write(&m_bAlpha, sizeof(BYTE));

	return sizeof(BYTE)+sizeof(BYTE);
}

void CColor::ReleaseWeb(CFile& file) const
{
	file.Write(&m_bAlpha, sizeof(byte));
	COLORREF rgb = this->GetMonitorRGB();
	if(rgb==0XFFFFFF)
		rgb = 0XFEFEFE;

	rgb = rgb<<8;
	rgb = rgb|0XFF;
	//	ReverseOrder(rgb);
	file.Write(&rgb, sizeof(DWORD));
}

void CColor::ReleaseWeb(BinaryStream& stream) const
{
	stream<<m_bAlpha;
	COLORREF rgb = this->GetMonitorRGB();
	if(rgb==0XFFFFFF)
		rgb = 0XFEFEFE;

	rgb = rgb<<8;
	rgb = rgb|0XFF;
	stream<<rgb;
}
void CColor::ReleaseWeb(boost::json::object& json) const
{
	const COLORREF rgb = this->GetMonitorRGB();

	CStringA rgba;
	rgba.Format("rgba(%d,%d,%d,%.2f)", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), m_bAlpha/255.f);
	json["Color"] = rgba;
}
void CColor::ReleaseWeb(pbf::writer& writer) const
{
	const COLORREF rgb= this->GetMonitorRGB();

	CStringA rgba;
	rgba.Format("rgba(%d,%d,%d,%.2f)", GetRValue(rgb), GetGValue(rgb), GetBValue(rgb), m_bAlpha/255.f);
	writer.add_string(rgba);
}
void CColor::ExportAIStroke(FILE* file, CPsboard& aiKey) const
{
	const DWORD CMYK = this->ToCMYK();
	if(CMYK!=aiKey.K)
	{
		DWORD c = CMYK;
		DWORD m = CMYK;
		DWORD y = CMYK;
		DWORD k = CMYK;

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

		_ftprintf(file, _T("%g %g %g %g K\n"), C, M, Y, K);

		aiKey.K = CMYK;
	}
}

void CColor::ExportAIBrush(FILE* file, CPsboard& aiKey) const
{
	const DWORD CMYK = this->ToCMYK();
	if(CMYK!=aiKey.k)
	{
		DWORD c = CMYK;
		DWORD m = CMYK;
		DWORD y = CMYK;
		DWORD k = CMYK;

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

		_ftprintf(file, _T("%g %g %g %g k\n"), C, M, Y, K);

		aiKey.k = CMYK;
	}
}

CColor* CColor::Apply(BYTE index)
{
	CColor* color = nullptr;
	switch(index)
	{
	case 0:
		break;
	case 1:
		color = new CColorProcess((BYTE)0XFF);
		break;
	case 2:
		color = new CColorSpot((BYTE)0XFF);
		break;
	case 3:
		color = new CColorGray;
		break;
	}

	return color;
}

#include <cstdio>
#include <Windows.h>

DWORD CColor::FindCMM(CString strCMM)
{
	const DWORD dwNumCMM = 0;
	DWORD dwMaxName, dwMaxValue;
	HKEY hkCMM = nullptr;
	const DWORD dwErr = RegCreateKey(HKEY_LOCAL_MACHINE, _T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\ICM\\ICMatchers"), &hkCMM);
	DWORD dwInfoErr = RegQueryInfoKey(hkCMM, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &dwMaxName, &dwMaxValue, nullptr, nullptr);
	if(dwErr==ERROR_SUCCESS)
	{
		TCHAR szValueName[MAX_PATH];
		DWORD dwValueNameSize = sizeof(szValueName)-1;
		BYTE szValueData[MAX_PATH];
		DWORD dwValueDataSize = sizeof(szValueData)-1;

		DWORD dwType;
		while(RegEnumValue(hkCMM, dwNumCMM, szValueName, &dwValueNameSize, nullptr, &dwType, szValueData, &dwValueDataSize)==ERROR_SUCCESS)
		{
			if(strCMM==szValueName)
			{
				return dwNumCMM;;
			}
		}
	}

	RegCloseKey(hkCMM);

	return -1;
}

Gdiplus::ARGB CColor::RGBtoARGB(COLORREF rgb)
{
	const BYTE r = GetRValue(rgb);
	const BYTE g = GetGValue(rgb);
	const BYTE b = GetBValue(rgb);
	return Gdiplus::Color::MakeARGB(255, r, g, b);
}

COLORREF CColor::ARGBtoRGB(Gdiplus::ARGB argb)
{
	return Gdiplus::Color(argb).ToCOLORREF();
}
