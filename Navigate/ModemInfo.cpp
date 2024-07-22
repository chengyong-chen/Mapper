#include "stdafx.h"
#include "ModemInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

#define MAX_ENUM_COUNT		10

/////////////////////////////////////////////////////////////////////////////
// CModemInfo

CModemInfo::CModemInfo()
{
}

CModemInfo::~CModemInfo()
{
}

/////////////////////////////////////////////////////////////////////////////
// CModemInfo message handlers

CString CModemInfo::GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault, HKEY hKey) const
{
	CString strValue=lpszDefault;

	// get section key
	HKEY hSectionKey=nullptr;
	::RegOpenKeyEx(hKey, lpszSection, 0, KEY_READ, &hSectionKey);

	// if section does not exist, return default
	if(hSectionKey != nullptr)
	{
		// get desired entry
		DWORD dwType=0;
		DWORD dwCount=0;

		if(::RegQueryValueEx(hSectionKey, (LPTSTR)lpszEntry, nullptr, &dwType, nullptr, &dwCount) == ERROR_SUCCESS)
		{
			ASSERT(dwType == REG_SZ);

			if(::RegQueryValueEx(hSectionKey, (LPTSTR)lpszEntry, nullptr, &dwType, (LPBYTE)strValue.GetBuffer(dwCount/sizeof(TCHAR)), &dwCount) == ERROR_SUCCESS)
			{
				strValue.ReleaseBuffer();
			}
			else
			{
				strValue.ReleaseBuffer();
				strValue=lpszDefault; // just in case
			}
		}

		::RegCloseKey(hSectionKey);
	}

	return strValue;
}

CString CModemInfo::GetPaddedIndex(const int nIndex)
{
	CString strIndex;
	strIndex.Format(_T("%d"), nIndex);
	return CString(_T("0000")).Left(4 - strIndex.GetLength()) + strIndex;
}

int CModemInfo::GetModemIndex(CString strName) const
{
	int nResult=-1;

	if(!strName.IsEmpty())
	{
		strName.MakeUpper();

		int nCount;
		for(nCount=0; nCount < MAX_ENUM_COUNT; nCount++)
		{
			CString strGetModemName=GetModemName(nCount);
			strGetModemName.MakeUpper();
			if(strName == strGetModemName && GetModemComPort(nCount) > 0)
			{
				nResult=nCount;
				break;
			}
		}
	}

	return nResult;
}

int CModemInfo::GetModemCount() const
{
	int nResult=0;

	for(int nCount=0; nCount < MAX_ENUM_COUNT; nCount++)
	{
		CString strName=GetModemName(nCount);
		if(!strName.IsEmpty() && strName.Find(_T("IP Virtual Modem")) == -1) // don't count COM/IP Virtual Modem
		{
			nResult++;
		}
	}

	return nResult;
}

CString CModemInfo::GetModemRegistrySection(const int nIndex) const
{
	CString strResult;

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize=sizeof(OSVERSIONINFO);
	GetVersionEx(&osvi);

	if(osvi.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
	{
		strResult=_T("SYSTEM\\CurrentControlSet\\Services\\Class\\Modem\\") + GetPaddedIndex(nIndex);
	}
	else
	{
		strResult=_T("SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E96D-E325-11CE-BFC1-08002BE10318}\\") + GetPaddedIndex(nIndex);
	}

	return strResult;
}

CString CModemInfo::GetFirstModemName() const
{
	CString strResult;

	int nCount;
	for(nCount=0; nCount < MAX_ENUM_COUNT; nCount++)
	{
		CString strName=GetModemName(nCount);
		if(!strName.IsEmpty() && strName.Find(_T("IP Virtual Modem")) == -1) // don't add COM/IP Virtual Modem
		{
			strResult=strName;
			break;
		}
	}

	return strResult;
}

CString CModemInfo::GetModemName(const int nIndex) const
{
	return GetProfileString(GetModemRegistrySection(nIndex), _T("Model"), _T(""), HKEY_LOCAL_MACHINE);
}

CString CModemInfo::GetModemInitString(const CString strName, const int nSpeakerVolume) const
{
	CString strInit=_T("AT");

	const CString strKey=GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings");

	CString strCallSetupFailTimer=GetProfileString(strKey, _T("CallSetupFailTimer"), _T(""), HKEY_LOCAL_MACHINE);
	strCallSetupFailTimer.Replace(_T("<#>"), _T("60"));

	CString strInactivityTimeout=GetProfileString(strKey, _T("InactivityTimeout"), _T(""), HKEY_LOCAL_MACHINE);
	strInactivityTimeout.Replace(_T("<#>"), _T("0"));

	strInit+=strCallSetupFailTimer + strInactivityTimeout;

	if(nSpeakerVolume != -1 && GetModemHasSpeaker(strName))
	{
		const int nNumVolumeLevels=GetModemVolumeLevelsCount(strName);
		const CString strSpeakerOn=GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerMode_Dial"), _T(""), HKEY_LOCAL_MACHINE);

		switch(nSpeakerVolume)
		{
			case 0:
				strInit+=GetProfileString(strKey, _T("SpeakerMode_OFF"), _T(""), HKEY_LOCAL_MACHINE);
				break;
			case 1:
				strInit+=nNumVolumeLevels == 0 ? strSpeakerOn : GetProfileString(strKey, _T("SpeakerVolume_Low"), _T(""), HKEY_LOCAL_MACHINE) + strSpeakerOn;
				break;
			case 2:
				strInit+=nNumVolumeLevels > 0 ? GetProfileString(strKey, _T("SpeakerVolume_Med"), _T(""), HKEY_LOCAL_MACHINE) + strSpeakerOn : CString(_T(""));
				break;
			case 3:
				strInit+=nNumVolumeLevels > 0 ? GetProfileString(strKey, _T("SpeakerVolume_High"), _T(""), HKEY_LOCAL_MACHINE) + strSpeakerOn : CString(_T(""));
				break;
			default:
				ASSERT(FALSE);
		}
	}

	strInit+=GetProfileString(strKey, _T("ErrorControl_On"), _T(""), HKEY_LOCAL_MACHINE);
	strInit+=GetProfileString(strKey, _T("Compression_On"), _T(""), HKEY_LOCAL_MACHINE);
	strInit+=GetProfileString(strKey, _T("FlowControl_Hard"), _T(""), HKEY_LOCAL_MACHINE);
	strInit+=GetProfileString(strKey, _T("Modulation_CCITT"), _T(""), HKEY_LOCAL_MACHINE);
	strInit+=GetProfileString(strKey, _T("Blind_Off"), _T(""), HKEY_LOCAL_MACHINE);

	return strInit;
}

BOOL CModemInfo::GetModemHasSpeaker(const CString strName) const
{
	return !GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerMode_Dial"), _T(""), HKEY_LOCAL_MACHINE).IsEmpty() && !GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerMode_OFF"), _T(""), HKEY_LOCAL_MACHINE).IsEmpty();
}

int CModemInfo::GetModemVolumeLevelsCount(const CString strName) const
{
	int nResult=0;

	if(!GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerVolume_Low"), _T(""), HKEY_LOCAL_MACHINE).IsEmpty()) nResult++;
	if(!GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerVolume_Med"), _T(""), HKEY_LOCAL_MACHINE).IsEmpty()) nResult++;
	if(!GetProfileString(GetModemRegistrySection(GetModemIndex(strName)) + _T("\\Settings"), _T("SpeakerVolume_High"), _T(""), HKEY_LOCAL_MACHINE).IsEmpty()) nResult++;

	return nResult;
}

int CModemInfo::GetModemBps(const CString strName) const
{
	int nResult=0;

	HKEY hKey;
	const CString strSection=GetModemRegistrySection(GetModemIndex(strName));
	if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSection, 0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
	{
		const int nSize=255;
		DWORD dwType;
		DWORD dwSize=255;
		char szString[255];

		if(::RegQueryValueEx(hKey, _T("DCB"), nullptr, &dwType, (BYTE*)szString, &dwSize) == ERROR_SUCCESS)
		{
			memcpy(&nResult, &szString[4], 4);
			if(nResult != 230400 &&
				nResult != 115200 &&
				nResult != 57600 &&
				nResult != 38400 &&
				nResult != 19200 &&
				nResult != 9600 &&
				nResult != 4800 &&
				nResult != 2400 &&
				nResult != 1200 &&
				nResult != 300 &&
				nResult != 110)
			{
				nResult=0;
			}
		}

		RegCloseKey(hKey);
	}

	return nResult;
}

int CModemInfo::GetModemComPort(const int nIndex) const
{
	int nResult=0;

	const CString strPort=GetProfileString(GetModemRegistrySection(nIndex), _T("AttachedTo"), _T(""), HKEY_LOCAL_MACHINE);
	if(!strPort.IsEmpty())
	{
		nResult=_ttol(strPort.Mid(3)); // ie, COM3
	}
	else
	{
		HKEY hKey;
		if(::RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T(""), 0L, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)
		{
			const CString strKey;// = RegistryEx.FindKey("Enum", "Driver", "Modem\\" + GetPaddedIndex(nIndex));
			if(!strKey.IsEmpty())
			{
				const CString strPort=GetProfileString(strKey, _T("PORTNAME"), _T(""), HKEY_LOCAL_MACHINE);
				if(!strPort.IsEmpty())
				{
					nResult=_ttol(strPort.Mid(3));
				}
			}

			RegCloseKey(hKey);
		}
	}

	return nResult;
}

void CModemInfo::FillComboBoxWithModemNames(CComboBox* pComboBox) const
{
	// delete exiting items
	while(pComboBox->DeleteString(0) != CB_ERR);

	// add all detected modems
	int nCount;
	for(nCount=0; nCount < MAX_ENUM_COUNT; nCount++)
	{
		CString strName=GetModemName(nCount);
		if(!strName.IsEmpty() && strName.Find(_T("IP Virtual Modem")) == -1) // don't add COM/IP Virtual Modem
		{
			pComboBox->AddString(strName);
		}
	}
}
