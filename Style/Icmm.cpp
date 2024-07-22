// Icm.cpp : implementation file
//

#include "stdafx.h"
#include "Icmm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CIcmm

CIcmm::CIcmm()
{
	m_strEngine = "ADOBEACE";

	m_profileCMYK = "USWebCoatedSWOP.icc";;
	m_profileRGB = "AdobeRGB1998.icc";
	m_profileMonitor = "sRGB Color Space Profile.icm";

	m_icmCMYKToRGB = nullptr;
	m_icmRGBToCMYK = nullptr;

	m_icmCMYKToMonitor = nullptr;
	m_icmRGBToMonitor = nullptr;
}

CIcmm::~CIcmm()
{
	if(m_icmCMYKToRGB==nullptr)
	{
		DeleteColorTransform(m_icmCMYKToRGB);
		m_icmCMYKToRGB = nullptr;
	}
	if(m_icmRGBToCMYK==nullptr)
	{
		DeleteColorTransform(m_icmRGBToCMYK);
		m_icmRGBToCMYK = nullptr;
	}
	if(m_icmCMYKToMonitor==nullptr)
	{
		DeleteColorTransform(m_icmCMYKToMonitor);
		m_icmCMYKToMonitor = nullptr;
	}
	if(m_icmRGBToMonitor==nullptr)
	{
		DeleteColorTransform(m_icmRGBToMonitor);
		m_icmRGBToMonitor = nullptr;
	}
}

void CIcmm::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	if(ar.IsStoring())
	{
		ar<<m_strEngine;
	}
	else
	{
		ar>>m_strEngine;
	}
}

void CIcmm::Load()
{
	PROFILE profileCMYK;
	PROFILE profileRGB;
	PROFILE profileMonitor;
	profileCMYK.dwType = PROFILE_FILENAME;
	profileRGB.dwType = PROFILE_FILENAME;
	profileMonitor.dwType = PROFILE_FILENAME;
	profileCMYK.pProfileData = (PVOID)(LPCTSTR)m_profileCMYK;
	profileRGB.pProfileData = (PVOID)(LPCTSTR)m_profileRGB;
	profileMonitor.pProfileData = (PVOID)(LPCTSTR)m_profileMonitor;
	profileCMYK.cbDataSize = m_profileCMYK.GetLength();
	profileRGB.cbDataSize = m_profileRGB.GetLength();
	profileMonitor.cbDataSize = m_profileMonitor.GetLength();
	HPROFILE hProfileCMYK = OpenColorProfile(&profileCMYK, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
	HPROFILE hProfileRGB = OpenColorProfile(&profileRGB, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);
	HPROFILE hProfileMonitor = OpenColorProfile(&profileMonitor, PROFILE_READ, FILE_SHARE_READ, OPEN_EXISTING);

	if(hProfileCMYK!=nullptr&&hProfileRGB!=nullptr)
	{
		HPROFILE hProfiles[2];
		DWORD intent = INTENT_PERCEPTUAL;

		hProfiles[0] = hProfileCMYK;
		hProfiles[1] = hProfileRGB;
		const HTRANSFORM hICMxform1 = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
		if(hICMxform1!=nullptr)
		{
			m_icmCMYKToRGB = hICMxform1;
		}

		hProfiles[0] = hProfileRGB;
		hProfiles[1] = hProfileCMYK;
		const HTRANSFORM hICMxform2 = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
		if(hICMxform2!=nullptr)
		{
			m_icmRGBToCMYK = hICMxform2;
		}
	}
	if(hProfileCMYK!=nullptr&&hProfileMonitor!=nullptr)
	{
		HPROFILE hProfiles[2];
		DWORD intent = INTENT_PERCEPTUAL;

		hProfiles[0] = hProfileCMYK;
		hProfiles[1] = hProfileMonitor;
		const HTRANSFORM hICMxform = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
		if(hICMxform!=nullptr)
		{
			m_icmCMYKToMonitor = hICMxform;
		}
	}
	if(hProfileRGB!=nullptr&&hProfileMonitor!=nullptr)
	{
		HPROFILE hProfiles[2];
		DWORD intent = INTENT_PERCEPTUAL;

		hProfiles[0] = hProfileRGB;
		hProfiles[1] = hProfileMonitor;
		const HTRANSFORM hICMxform = CreateMultiProfileTransform(hProfiles, 2, &intent, 1, ENABLE_GAMUT_CHECKING|BEST_MODE|PRESERVEBLACK, INDEX_DONT_CARE);
		if(hICMxform!=nullptr)
		{
			m_icmRGBToMonitor = hICMxform;
		}
	}
	if(hProfileCMYK!=nullptr)
	{
		CloseColorProfile(hProfileCMYK);
		hProfileCMYK = nullptr;
	}
	if(hProfileRGB!=nullptr)
	{
		CloseColorProfile(hProfileRGB);
		hProfileRGB = nullptr;
	}
	if(hProfileMonitor!=nullptr)
	{
		CloseColorProfile(hProfileMonitor);
		hProfileMonitor = nullptr;
	}
}
