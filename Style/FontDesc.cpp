#include "stdafx.h"

#include "FontDesc.h"
#include "GetFontFile.h"
#include "GetNameValue.h"

#include "../Public/Function.h"
#include "../Public/BinaryStream.h"
#include "../Pbf/writer.hpp"
#include "../Utility/cstring.hpp"
#include <filesystem> 

#include <boost/json.hpp>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define PRINTER_FONT 0x0100
#define TTF_FONT     0x0200
#define DEVICE_FONT  0x0400


// Specialization of the template to use the custom hash function
template<>
AFX_INLINE UINT AFXAPI HashKey(const CStringA& key) {
	const char* str = key.GetString();
	UINT hash = 0;
	while(*str) {
		hash = (hash << 5) + hash + *str++;
	}
	return hash;
}

CMap<CStringA, const CStringA&, CString, const CString&> CFontDesc::m_RealToFace;  //one to one
CMap<CStringA, const CStringA&, CString, const CString&> CFontDesc::m_RealToFamily;//multiple to one

CFontDesc::CFontDesc()
{
	m_strReal = "ArialMT";
	m_style = Gdiplus::FontStyle::FontStyleRegular;
}

CFontDesc::CFontDesc(LPCTSTR lpszFace)
{
	m_strReal = CFontDesc::GetRealByFace(lpszFace);
	m_style = Gdiplus::FontStyle::FontStyleRegular;
}
void CFontDesc::Sha1(boost::uuids::detail::sha1& sha1) const
{
	cstr::sha1(m_strReal, sha1);

	sha1.process_byte(m_style);
}
const CFontDesc& CFontDesc::operator =(const CFontDesc& descSrc)
{
	if(&descSrc != this)
	{
		m_strReal = descSrc.m_strReal;
		m_strFamily = CFontDesc::GetFamilyByReal(m_strReal);
		m_style = descSrc.m_style;
	}
	return *this;
}

const BOOL CFontDesc::operator==(const CFontDesc& descSrc) const
{
	if(m_style != descSrc.m_style)
		return FALSE;
	else if(m_strReal != descSrc.m_strReal)
		return FALSE;

	return TRUE;
}

CStringA CFontDesc::GetEncoding()
{
	const LANGID nCharSet = GetUserDefaultUILanguage();
	CStringA strEncoding = "";
	switch(nCharSet)
	{
		case 0x00: //ANSI_CHARSET 		
		case 0x01: //DEFAULT_CHARSET		
		case 0x02: //SYMBOL_CHARSET 		
		case 0x80: //SHIFTJIS_CHARSET	
			strEncoding = "StandardEncoding";
		case 0x81: //HANGUL_CHARSET 					Microsoft Code Page 949 (lfCharSet 0x81), KS X 1001:1992 character set plus 8822 additional hangul, Unified Hangul Code (UHC) encoding (proportional)
			strEncoding = "KSCms-UHC-H";
		case 0x86: //GB2312_CHARSET					Microsoft Code Page 936 (lfCharSet 0x86) GBK encoding 
			strEncoding = "GBK-EUC-H";
		case 0x88: //CHINESEBIG5_CHARSET				Microsoft Code Page 950 (lfCharSet 0x88) Big Five character set with ETen extensions //EUC-CN encoding
			strEncoding = "ETen-B5-H";
		case 0xA1: //GREEK_CHARSET 	
		case 0xA2: //TURKISH_CHARSET 	
		case 0xB1: //HEBREW_CHARSET 	
		case 0xB2: //ARABIC_CHARSET 	
		case 0xBA: //BALTIC_CHARSET 	
		case 0xCC: //RUSSIAN_CHARSET 	
		case 0xDE: //THAI_CHARSET 		
		case 0xEE: //EE_CHARSET 		
		case 0xFF: //OEM_CHARSET 		
			strEncoding = "";
		default:
			strEncoding = "";
	}
	strEncoding = "UTF-8";
	return strEncoding;
}

const BOOL CFontDesc::operator !=(const CFontDesc& descSrc) const
{
	return !(*this == descSrc);
}
CString CFontDesc::GetFace() const
{
	return CFontDesc::GetFaceByReal(m_strReal);
};
void CFontDesc::SetByFace(CString strFace)
{
	m_strReal = CFontDesc::GetRealByFace(strFace);
	m_strFamily = CFontDesc::GetFamilyByReal(m_strReal);
}
void CFontDesc::SetReal(CStringA strReal)
{
	m_strReal = strReal;
	m_strFamily = CFontDesc::GetFamilyByReal(m_strReal);
}
void CFontDesc::SetStyle(Gdiplus::FontStyle style)
{
	m_style = style;
}
void CFontDesc::Serialize(CArchive& ar, const DWORD& dwVersion)
{
	BYTE nCharSet = 0X00;//because one font could support multiple charset, it fst useless here.
	CString strScript = _T("");
	if(ar.IsStoring())
	{
		ar << m_strReal;
		ar << strScript;
		ar << (BYTE)m_style;
	}
	else
	{
		BYTE style;
		ar >> m_strReal;
		ar >> strScript;
		ar >> style;
		if(FontRealNameExists(m_strReal) == FALSE)
		{
			const _bstr_t btrFont(m_strReal);
			const Gdiplus::FontFamily fontFamily(btrFont);
			if(fontFamily.IsAvailable() == TRUE)
			{
				TCHAR family[512];
				fontFamily.GetFamilyName(family);
				m_strFamily = family;
				m_strReal = CFontDesc::GetRealByFamily(m_strFamily, (Gdiplus::FontStyle)style);
			}
			else
			{
				m_strFamily = m_strReal;
			}
		}
		else
		{
			m_strFamily = CFontDesc::GetFamilyByReal(m_strReal);		
		}
		m_style = (Gdiplus::FontStyle)style;
	}
}

DWORD CFontDesc::PackPC(CFile* pFile, BYTE*& bytes)
{
	if(pFile != nullptr)
	{
		DWORD size = PackStrPC(m_strReal, pFile);
		size += sizeof(BYTE);
		size += sizeof(BYTE);
		size += sizeof(DWORD);
		return size;
	}
	else
	{
		bytes += sizeof(BYTE);

		bytes += sizeof(DWORD);
		return 0;
	}
}

void CFontDesc::ReleaseWeb(CFile& file) const
{
	SaveAsUTF8(file, m_strReal);

	const LANGID nCharSet = GetUserDefaultUILanguage();
	file.Write(&nCharSet, sizeof(BYTE));

}
void CFontDesc::ReleaseWeb(BinaryStream& stream) const
{
	SaveAsUTF8(stream, m_strReal);
}
void CFontDesc::ReleaseWeb(boost::json::object& json) const
{
	SaveAsEscapedASCII(json, "Family", m_strFamily);
	json["style"] = (int)m_style;
}
void CFontDesc::ReleaseWeb(pbf::writer& writer) const
{	
	CString strFace = this->GetFaceByReal(m_strReal);
	writer.add_string(EscapedASCII(strFace));
	writer.add_string(EscapedASCII(m_strFamily));

	writer.add_byte(m_style);
}
void CFontDesc::LoadFonts()
{
	CFontDesc::m_RealToFace.RemoveAll();
	CFontDesc::m_RealToFamily.RemoveAll();

	OSVERSIONINFO osversion;
	osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osversion) == FALSE)
		return;

	CString strKey;
	if(osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		strKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts");
	else if(osversion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		strKey = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	TCHAR szWinDir[_MAX_PATH * 2];
	::GetWindowsDirectory(szWinDir, _MAX_PATH);
	CString strFolder = szWinDir;
	strFolder += _T("\\Fonts\\");
	OutputDebugString(_T("fonts from files:"));
	_TCHAR szName[2 * MAX_PATH];
	_TCHAR szData[2 * MAX_PATH];

	while(GetNextNameValue(HKEY_LOCAL_MACHINE, strKey, szName, szData) == ERROR_SUCCESS)
	{
		CString strFile = strFolder + szData;
		CStringA strType = GetFontType(strFile);
		if(strType == "TTF")
		{
			FONT_PROPERTIES fontprops;
			if(GetTTFFontProperties(strFile, fontprops) == TRUE)
			{
				CFontDesc::m_RealToFace.SetAt(fontprops.realName, fontprops.faceName);
				CFontDesc::m_RealToFamily.SetAt(fontprops.realName, fontprops.csFamily);
				OutputDebugString(_T("Real: ") + CString(fontprops.realName) + _T("Face: ") + fontprops.faceName + _T("\t") + _T("Famiy: ") + fontprops.csFamily + _T("\n"));
			}
		}
		else if(strType == "TTC")
		{
			const int count = GetTTCFontCount(strFile);
			for(int index = 0; index < count; index++)
			{
				FONT_PROPERTIES fontprops;
				if(GetTTCFontProperties(strFile, fontprops, index) == TRUE)
				{
					CFontDesc::m_RealToFace.SetAt(fontprops.realName, fontprops.faceName);
					CFontDesc::m_RealToFamily.SetAt(fontprops.realName, fontprops.csFamily);
					OutputDebugString(_T("Real: ") + CString(fontprops.realName) + _T("Face: ") + fontprops.faceName + _T("\t") + _T("Famiy: ") + fontprops.csFamily + _T("\n"));
				}
			}
		}
		else
		{
		}

		strKey.Empty(); // this will get next value, same key
	}
	GetNextNameValue(HKEY_LOCAL_MACHINE, nullptr, nullptr, nullptr); // close the registry key	
}

bool CFontDesc::FontRealNameExists(CStringA strReal)
{
	CString strFace;
	return CFontDesc::m_RealToFace.Lookup(strReal, strFace);
}
CString CFontDesc::GetFaceByReal(CStringA strReal)
{
	CString strFace;
	if(CFontDesc::m_RealToFace.Lookup(strReal, strFace) == TRUE)
		return CFontDesc::m_RealToFace[strReal];
	else
		return CString(strReal);
}
CStringA CFontDesc::GetRealByFace(CString strFace)
{ 
	POSITION pos = m_RealToFace.GetStartPosition();
	while(pos != nullptr)
	{
		CStringA strName1;
		CString strName2;
		m_RealToFace.GetNextAssoc(pos, strName1, strName2);
		if(strName2 == strFace)
			return strName1;
	}

	return CStringA(strFace);
}
CString CFontDesc::GetFamilyByReal(CStringA strReal)
{
	CString strFamily;
	if(CFontDesc::m_RealToFamily.Lookup(strReal, strFamily) == TRUE)
		return CFontDesc::m_RealToFamily[strReal];
	else
		return CString(strReal);
}
CStringA CFontDesc::GetRealByFamily(CString strFamily, Gdiplus::FontStyle style)
{
	POSITION pos = m_RealToFamily.GetStartPosition();
	while(pos != nullptr)
	{
		CStringA strName1;
		CString strName2;
		m_RealToFamily.GetNextAssoc(pos, strName1, strName2);
		if(strName2 == strFamily)
			return strName1;
	}

	return CStringA(strFamily);
}
int MatchingChars(const char* s1, const char* s2)
{
	size_t fst = 0;
	size_t snd = 0;
	while(fst < strlen(s1) && snd < strlen(s2))
	{
		if(s1[fst] == s2[snd])
		{
			fst++;
			snd++;
		}
		else if(s1[snd] == ' ')
			fst++;
		else if(s1[snd] == '_')
			fst++;
		else if(s2[snd] == ' ')
			snd++;
		else if(s2[snd] == '_')
			snd++;
		else
			break;
	}
	return min(fst, snd);
}

char* CFontDesc::GetBestMatched(const char* family, const char* name)
{
	char name1[256] = {0};
	char name2[256] = {0};
	char* best = (char*)malloc(256);

	memset(best, 0, 256);
	int maxed = 0;
	POSITION pos = m_RealToFace.GetStartPosition();
	while(pos != nullptr)
	{
		CStringA strName1;
		CString strName2;
		m_RealToFace.GetNextAssoc(pos, strName1, strName2);
			
		strcpy_s(name1, strName1.GetBuffer());
		strcpy_s(name2, CT2A(strName2.GetBuffer()));
		strName1.ReleaseBuffer();
		strName2.ReleaseBuffer();
		if(family == nullptr && name == nullptr)
			break;

		if(family == nullptr)
		{
		}
		else if(strcmpi(name1, family) == 0 || strcmpi(name2, family) == 0)
		{
			memset(best, 0, 256);
			memcpy(best, name1, strlen(name1));
			return best;
		}
		else if(const char* ptr = strchr(name2, ' '); ptr != nullptr)
		{
			int m1 = MatchingChars(name1, family);
			int m2 = MatchingChars(name2, family);
			const int maxmatch = max(m1, m2);
			const int mimmatch = strlen(name2) - strlen(ptr);
			if(maxmatch < strlen(name))
			{
			}
			else if(maxmatch < mimmatch)
			{
			}
			else if(maxmatch > maxed)
			{
				maxed = maxmatch;
				memset(best, 0, 256);
				memcpy(best, name1, strlen(name1));
			}
		}

		if(name == nullptr)
		{
		}
		else if(strcmpi(name1, name) == 0 || strcmpi(name2, name) == 0)
		{
			memset(best, 0, 256);
			memcpy(best, name1, strlen(name1));
			return best;
		}
		else if(const char* ptr = strchr(name2, ' '); ptr != nullptr)
		{
			int m1 = MatchingChars(name1, name);
			int m2 = MatchingChars(name2, name);
			const int maxmatch = max(m1, m2);
			const int mimmatch = strlen(name2) - strlen(ptr);
			if(maxmatch < strlen(name))
			{
			}
			else if(maxmatch < mimmatch)
			{
			}
			else if(maxmatch > maxed)
			{
				maxed = maxmatch;
				memset(best, 0, 256);
				memcpy(best, name1, strlen(name1));
			}
		}
	}
	if(best == nullptr || strlen(best) == 0)
	{
		memset(best, 0, 256);
		memcpy(best, "Times New Roman", 16);
	}
	return  best;
}

CString CFontDesc::GetFileByReal(CStringA strReal)
{
	OSVERSIONINFO osversion;
	osversion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if(GetVersionEx(&osversion) == FALSE)
		return _T("");

	CString strKey;
	if(osversion.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
		strKey = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Fonts");
	else if(osversion.dwPlatformId == VER_PLATFORM_WIN32_NT)
		strKey = _T("Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts");

	TCHAR szWinDir[_MAX_PATH * 2];
	::GetWindowsDirectory(szWinDir, _MAX_PATH);
	CString strFolder = szWinDir;
	strFolder += _T("\\Fonts\\");

	_TCHAR szName[2 * MAX_PATH];
	_TCHAR szData[2 * MAX_PATH];
	FONT_PROPERTIES fontprops;
	while(GetNextNameValue(HKEY_LOCAL_MACHINE, strKey, szName, szData) == ERROR_SUCCESS)
	{
		CString strFile = strFolder + szData;
		CStringA strType = GetFontType(strFile);
		if(strType == "TTF")
		{
			FONT_PROPERTIES fontprops;
			if(GetTTFFontProperties(strFile, fontprops) == TRUE)
			{
				if(fontprops.realName == strReal)
					return strFile;
			}
		}
		else if(strType == "TTC")
		{
			const int count = GetTTCFontCount(strFile);
			for(int index = 0; index < count; index++)
			{
				FONT_PROPERTIES fontprops;
				if(GetTTCFontProperties(strFile, fontprops, index) == TRUE)
				{
					if(fontprops.realName == strReal)
						return strFile;
				}
			}
		}

		strKey.Empty(); // this will get next value, same key
	}
	GetNextNameValue(HKEY_LOCAL_MACHINE, nullptr, nullptr, nullptr); // close the registry key	
	return _T("");
}