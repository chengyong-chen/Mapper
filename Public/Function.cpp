#include "stdafx.h"
#include "Function.h"
#include <Mmsystem.h>
#include <atlstr.h>
#include <algorithm>
#include <iomanip>
#include <sstream>

#include "../Public/BinaryStream.h"

#include "../../Thirdparty/ZLib/1.2.11/Zlib.h"
#include "../../ThirdParty/boost/1.83.0/boost/archive/iterators/base64_from_binary.hpp"
#include "../../ThirdParty/boost/1.83.0/boost/archive/iterators/transform_width.hpp"

#include <boost/json.hpp>
using namespace std;
using namespace boost::archive::iterators;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif


void ReverseOrder(WORD& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	const BYTE tmp=*p;
	*p=*(p + 1);
	*(p + 1)=tmp;
}

void ReverseOrder(DWORD& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	for(int i=0; i < 2; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (3 - i));
		*(p + (3 - i))=tmp;
	}
}

void ReverseOrder(int& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	for(int i=0; i < 2; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (3 - i));
		*(p + (3 - i))=tmp;
	}
}

void ReverseOrder(unsigned int& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	for(int i=0; i < 2; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (3 - i));
		*(p + (3 - i))=tmp;
	}
}

void ReverseOrder(long& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	for(int i=0; i < 2; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (3 - i));
		*(p + (3 - i))=tmp;
	}
}

void ReverseOrder(short& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	const BYTE tmp=*p;
	*p=*(p + 1);
	*(p + 1)=tmp;
}

void ReverseOrder(float& x)
{
	BYTE* p=(BYTE*)&x; //   指向首字节   
	for(int i=0; i < 2; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (3 - i));
		*(p + (3 - i))=tmp;
	}
}

double ReverseOrder(double x)
{
	double y=x;
	BYTE* p=(BYTE*)&y; //   指向首字节   
	for(int i=0; i < 4; i++)
	{
		const BYTE tmp=*(p + i);
		*(p + i)=*(p + (7 - i));
		*(p + (7 - i))=tmp;
	}
	return y;
}

void ReverseOrder(CPoint* pPoints, unsigned int nPoints)
{
	for(unsigned int index=0; index < nPoints/2; index++)
	{
		const CPoint buffer=pPoints[index];
		pPoints[index]=pPoints[nPoints - 1 - index];
		pPoints[nPoints - 1 - index]=buffer;
	}
}

void AFXAPI AfxToRelatedPath(CString strDocPath, CString& strPath)
{
	//	if(strPath.GetLength() >=2 && strPath.Left(2) == _T("\\\\"))
	//		return;

	bool bSameVoume=false;
	while(true)
	{
		const long pos1=strDocPath.Find('\\');
		const long pos2=strPath.Find('\\');
		if(pos1 != -1 && pos2 != -1)
		{
			CString string1=strDocPath.Left(pos1);
			CString string2=strPath.Left(pos2);
			if(string1.CompareNoCase(string2) == 0)
			{
				strDocPath=strDocPath.Mid(pos1 + 1);
				strPath=strPath.Mid(pos2 + 1);
				bSameVoume=true;
			}
			else
				break;
		}
		else
			break;
	}

	while(bSameVoume == true)
	{
		const long pos1=strDocPath.Find('\\');
		if(pos1 != -1)
		{
			strDocPath=strDocPath.Mid(pos1 + 1);
			strPath=_T("..\\") + strPath;
		}
		else
			break;
	}
}

void AFXAPI AfxToFullPath(CString strPath, CString& strFile)
{
	if(strPath.IsEmpty() == TRUE)
		return;
	if(strFile.IsEmpty() == TRUE)
		return;

	if(strFile.Find(':') != -1)
		return;
	if(strFile.Find(_T("\\\\")) != -1)
		return;

	strPath.Replace(_T('/'), _T('\\'));
	if(strPath[strPath.GetLength() - 1] != _T('\\') && strPath[strPath.GetLength() - 1] != _T('/'))
	{
		const int pos=strPath.ReverseFind('\\');
		if(pos != -1)
			strPath=strPath.Left(pos + 1);
	}

	int pos=strFile.Find(_T("..\\"));
	while(pos != -1)
	{
		pos=strPath.ReverseFind('\\');
		if(pos != -1)
			strPath=strPath.Left(pos);
		else
			return;

		pos=strPath.ReverseFind('\\');
		if(pos != -1)
			strPath=strPath.Left(pos + 1);
		else
			return;

		strFile=strFile.Mid(3);
		pos=strFile.Find(_T("..\\"));
	}

	if(strFile.GetLength() >= 2 && strFile.Left(2) == _T(".\\"))
		strFile=strFile.Mid(2);

	if(strPath.IsEmpty() == false && strFile.IsEmpty() == false)
	{
		strFile=strPath + strFile;
	}

	return;
}

void AFXAPI AfxToServerPath(CString strSharedFold, CString& strPath)
{
	if(strPath.IsEmpty() == TRUE)
		return;
	if(strSharedFold.IsEmpty() == TRUE)
		return;
	const int pos=strPath.Find(strSharedFold, 0);
	if(pos != -1)
	{
		strPath=strPath.Mid(pos);
	}

	if(strPath.GetLength() > 2 && strPath.Left(2) != _T("\\\\"))
	{
		char szHostName[256];
		gethostname(szHostName, strlen(szHostName));

		CString string;
		string.Format(_T("\\\\%s\\%s"), szHostName, strPath);
		strPath=string;
	}
}

DWORD AFXAPI AfxGetCurrentArVersion()
{
	return 4;
}

DWORD AFXAPI AfxGetSoftVersion()
{
	const CStringA strVersion=GetVersionInfo(nullptr, "ProductVersion");
	unsigned int year;
	unsigned int month;
	unsigned int day;
	sscanf(strVersion, "%u.%u.%u.", &year, &month, &day);

	DWORD dwArVersion=day;
	dwArVersion|=(year << 16);
	dwArVersion|=(month << 8);
	return dwArVersion;
}

struct LANGANDCODEPAGE
{
	WORD wLanguage;
	WORD wCodePage;
};

CStringA AFXAPI GetVersionInfo(HMODULE hModule, CStringA strEntry)
{
	CStringA strReturn;

	if(hModule == nullptr)
		hModule=AfxGetResourceHandle();
	const HRSRC hVersion=FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
	if(hVersion != nullptr)
	{
		const HGLOBAL hGlobal=LoadResource(hModule, hVersion);
		if(hGlobal != nullptr)
		{
			const LPVOID versionInfo=LockResource(hGlobal);
			if(versionInfo != nullptr)
			{
				DWORD dwBytes;
				DWORD lang;

				LPVOID retbuf=nullptr;
				static char fileEntry[256];
				sprintf(fileEntry, "\\VarFileInfo\\Translation");
				const BOOL retVal=VerQueryValueA(versionInfo, fileEntry, &retbuf, (UINT*)&dwBytes);
				if(retVal && dwBytes == 4)
				{
					memcpy(&lang, retbuf, 4);
					sprintf(fileEntry, "\\StringFileInfo\\%02X%02X%02X%02X\\%s", (lang & 0x0000ff00) >> 8, lang & 0x000000ff, (lang & 0xff000000) >> 24, (lang & 0x00ff0000) >> 16, strEntry);
				}
				else
					sprintf(fileEntry, "\\StringFileInfo\\%04X04B0\\%s", GetUserDefaultLangID(), strEntry);
				const DWORD resSize=SizeofResource(hModule, hVersion);
				char* infocopy=new char[(int)resSize*2 + 4];
				memcpy(infocopy, (char*)versionInfo, resSize*2 + 4);
				if(VerQueryValueA(infocopy, fileEntry, &retbuf, (UINT*)&dwBytes))
					strReturn=(char*)retbuf;

				delete[] infocopy;
			}
		}

		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}

	return strReturn;
}

DWORD PlayMCI(CString strFile)
{
	DWORD dwReturn;
	MCI_OPEN_PARMS mciOpenParms;
	MCI_PLAY_PARMS mciPlayParms;
	MCI_STATUS_PARMS mciStatusParms;

	mciSendCommand(1, MCI_CLOSE, 0, 0);

	mciOpenParms.lpstrDeviceType=strFile.Find(_T(".wav")) != -1 ? _T("waveaudio") : _T("sequencer");
	mciOpenParms.lpstrElementName=strFile;;

	if(dwReturn=mciSendCommand(0, MCI_OPEN, MCI_OPEN_TYPE | MCI_OPEN_ELEMENT, (DWORD)(LPVOID)&mciOpenParms))
		return dwReturn;
	const DWORD wDeviceID=mciOpenParms.wDeviceID;

	mciStatusParms.dwItem=MCI_SEQ_STATUS_PORT;
	if(dwReturn=mciSendCommand(wDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPVOID)&mciStatusParms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0);
		return dwReturn;
	}

	if(LOWORD(mciStatusParms.dwReturn) != MIDI_MAPPER)
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0);
		return 0;
	}

	if(dwReturn=mciSendCommand(wDeviceID, MCI_PLAY, MCI_NOTIFY, (DWORD)(LPVOID)&mciPlayParms))
	{
		mciSendCommand(wDeviceID, MCI_CLOSE, 0, 0);
		return dwReturn;
	}

	return TRUE;
}

CString AfxGetProfileString(HKEY hkeySpace, LPCTSTR lpszRegisterKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault)
{
	if(lpszEntry != nullptr)
	{
		CString strSubkey;
		strSubkey.Format(_T("SOFTWARE\\%s\\%s\\"), lpszRegisterKey, lpszSection);
		strSubkey.Replace(_T("\\\\"), _T("\\"));
		HKEY hKey=nullptr;
		if(RegOpenKeyEx(hkeySpace, strSubkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize=0;
			if(RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr, nullptr, &dwSize) == ERROR_SUCCESS)
			{
				CString strValue;
				const LONG lRet=RegQueryValueEx(hKey, (LPTSTR)lpszEntry, nullptr, nullptr, (LPBYTE)strValue.GetBuffer(dwSize/sizeof(TCHAR)), &dwSize);
				strValue.ReleaseBuffer();

				RegCloseKey(hKey);
				if(lRet == ERROR_SUCCESS)
					return strValue;
				else
					return lpszDefault;
			}
			RegCloseKey(hKey);
			return lpszDefault;
		}
	}

	return lpszDefault;
}

BOOL AfxWriteProfileString(HKEY hkeySpace, LPCTSTR lpszRegisterKey, LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszValue)
{
	if(lpszSection != nullptr)
	{
		CString strBase;
		strBase.Format(_T("SOFTWARE\\%s\\%s"), lpszRegisterKey, lpszSection);

		HKEY hKey=nullptr;
		if(RegCreateKey(HKEY_LOCAL_MACHINE, strBase, &hKey) == ERROR_SUCCESS)
		{
			DWORD dwSize=0;
			if(RegSetValueEx(hKey, lpszEntry, 0, REG_SZ, (LPBYTE)lpszValue, _tcslen(lpszValue)*sizeof(TCHAR)) == ERROR_SUCCESS)
			{
				RegCloseKey(hKey);
				return TRUE;
			}
			else
			{
				RegCloseKey(hKey);
				return FALSE;
			}
		}
		else
			return FALSE;
	}
}

DWORD AFXAPI PackStrPC(CString& string, CFile* pFile)
{
	DWORD size=0;

	const BYTE bIn=0Xff;
	const WORD wIn1=0Xfffe;
	const WORD wIn2=0Xffff;

#ifdef _UNICODE
	pFile->Write(&bIn, sizeof(BYTE));
	pFile->Write(&wIn1, sizeof(WORD));

	size+=sizeof(BYTE) + sizeof(WORD);
#endif

	const DWORD dwLength=string.GetLength();
	if(dwLength < 255)
	{
		const BYTE bLength=dwLength;
		pFile->Write(&bLength, sizeof(BYTE));
		size+=sizeof(BYTE);
	}
	else if(dwLength < 0xfffe)
	{
		pFile->Write(&bIn, sizeof(BYTE));
		size+=sizeof(BYTE);

		const WORD wLength=dwLength;
		pFile->Write(&wLength, sizeof(WORD));
		size+=sizeof(WORD);
	}
	else
	{
		pFile->Write(&bIn, sizeof(BYTE));
		size+=sizeof(BYTE);

		pFile->Write(&wIn2, sizeof(WORD));
		size+=sizeof(WORD);

		pFile->Write(&dwLength, sizeof(DWORD));
		size+=sizeof(DWORD);
	}

	BYTE* bytes=new BYTE[dwLength*sizeof(TCHAR)];
	memcpy(bytes, string.GetBuffer(0), dwLength*sizeof(TCHAR));
	string.ReleaseBuffer();

	for(DWORD i=0; i < dwLength*sizeof(TCHAR); i++)
	{
		bytes[i]=(bytes[i] ^ (char)(i*i*dwLength*sizeof(TCHAR)));
	}
	pFile->Write(bytes, dwLength*sizeof(TCHAR));
	delete[] bytes;

	return size+dwLength*sizeof(TCHAR);
}
DWORD AFXAPI PackStrPC(CStringA& string, CFile* pFile)
{
	DWORD size = 0;

	const BYTE bIn = 0Xff;
	const WORD wIn1 = 0Xfffe;
	const WORD wIn2 = 0Xffff;
	const DWORD dwLength = string.GetLength();
	if(dwLength < 255)
	{
		const BYTE bLength = dwLength;
		pFile->Write(&bLength, sizeof(BYTE));
		size += sizeof(BYTE);
	}
	else if(dwLength < 0xfffe)
	{
		pFile->Write(&bIn, sizeof(BYTE));
		size += sizeof(BYTE);

		const WORD wLength = dwLength;
		pFile->Write(&wLength, sizeof(WORD));
		size += sizeof(WORD);
	}
	else
	{
		pFile->Write(&bIn, sizeof(BYTE));
		size += sizeof(BYTE);

		pFile->Write(&wIn2, sizeof(WORD));
		size += sizeof(WORD);

		pFile->Write(&dwLength, sizeof(DWORD));
		size += sizeof(DWORD);
	}

	BYTE* bytes = new BYTE[dwLength];
	memcpy(bytes, string.GetBuffer(0), dwLength);
	string.ReleaseBuffer();

	for(DWORD i = 0; i < dwLength; i++)
	{
		bytes[i] = (bytes[i] ^ (char)(i * i * dwLength));
	}
	pFile->Write(bytes, dwLength);
	delete[] bytes;

	return size + dwLength;
}
UINT AFXAPI ReadStringLength(BYTE*& bytes)
{
	DWORD nNewLen;

	// attempt BYTE length first
	const BYTE bLen=*(BYTE*)bytes;;
	bytes+=sizeof(BYTE);

	if(bLen < 0xff)
		return bLen;

	// attempt WORD length
	const WORD wLen=*(WORD*)bytes;;
	bytes+=sizeof(WORD);
	if(wLen == 0xfffe)
	{
		// UNICODE string prefix (length will follow)
		return (UINT)-1;
	}
	else if(wLen == 0xffff)
	{
		// read DWORD of length
		nNewLen=*(DWORD*)bytes;
		bytes+=sizeof(DWORD);
		return (UINT)nNewLen;
	}
	else
		return wLen;
}

CString AFXAPI UnpackStrPC(BYTE*& bytes)
{
#ifdef _UNICODE
	int nConvert=1; // if we get ANSI, convert
#else
	int nConvert=0;   // if we get UNICODE, convert
#endif

	UINT charCount=ReadStringLength(bytes);
	if(charCount == (UINT)-1)
	{
		nConvert=1 - nConvert;
		charCount=ReadStringLength(bytes);
		ASSERT(charCount != -1);
	}

	// set length of string to new length
	UINT byteCount;
#ifdef _UNICODE
	byteCount=charCount + charCount*(1 - nConvert); // bytes to read
#else
	byteCount=charCount + charCount*nConvert;    // bytes to read
#endif

	CString string;
	// read in the characters
	if(charCount != 0)
	{
		ASSERT(byteCount != 0);

		// read new data
		BYTE* chars=new BYTE[byteCount + 1];
		memcpy(chars, bytes, byteCount);
		bytes+=byteCount;

		for(int i=0; i < byteCount; i++)
		{
			chars[i]=(chars[i] ^ (i*i*byteCount));
		}
		chars[byteCount]='\0'; // must be NUL terminated

		// convert the data if as necessary

		if(nConvert != 0)
		{
		#ifdef _UNICODE
			string=(LPSTR)chars;
		#else
			string=(LPWSTR)chars;
		#endif
		}
		else
		{
		#ifdef _UNICODE
			string=(LPWSTR)chars;
		#else
			string=(LPSTR)chars;
		#endif
		}
		delete[] chars;
		chars=nullptr;
	}

	return string;
}

char* AFXAPI UnicodeToMultiByte(WCHAR* string)
{
	const int len1=WideCharToMultiByte(CP_ACP, 0, string, -1, nullptr, 0, nullptr, nullptr);
	char* pBuffer=new char[len1 + 1];
	int len2=WideCharToMultiByte(CP_ACP, 0, string, -1, pBuffer, len1, nullptr, nullptr);
	return pBuffer;
}

OLECHAR* MultiByteToUnicode(UINT CodePage, const char* szA)//CP_ACP from local machine
{
	// see how big the unicode string will be
	const ULONG ulCnt=MultiByteToWideChar(CodePage, MB_PRECOMPOSED, szA, -1, nullptr, 0);

	// allocate a buffer for the unicode string
	OLECHAR* pOleStr=new OLECHAR[ulCnt + 1];
	MultiByteToWideChar(CodePage, MB_PRECOMPOSED, szA, -1, pOleStr, ulCnt);
	return pOleStr;
}

OLECHAR* UTF8ToUnicode(const char* szA)
{
	// see how big the unicode string will be
	const ULONG ulCnt=MultiByteToWideChar(CP_UTF8, 0, szA, sizeof(szA), nullptr, 0);

	// allocate a buffer for the unicode string
	OLECHAR* pOleStr=new OLECHAR[ulCnt];
	MultiByteToWideChar(CP_UTF8, 0, szA, -1, pOleStr, ulCnt);
	return pOleStr;
}
void AFXAPI SerializeStrCE(CArchive& ar, CStringA string)
{
}
void AFXAPI SerializeStrCE(CArchive& ar, CString string)
{
	/*BYTE* pBuffer = nullptr;
	int nLength = string.GetLength();
	if(nLength>0)
	{
	nLength = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string.GetBuffer(0), -1, nullptr, 0);
	nLength-= 1;

	OLECHAR* wChars = MultiByteToUnicode((char*)string.GetBuffer(0));

	int nUTF8Len = WideCharToMultiByte(CP_UTF8,0,wChars,nLength,nullptr,0,nullptr,nullptr);
	pBuffer = new BYTE[nUTF8Len+1];
	nLength = WideCharToMultiByte(CP_UTF8,0,wChars,nLength,(LPSTR)pBuffer,nUTF8Len+1,nullptr,nullptr);
	}

	unsigned int value = (unsigned int)nLength;
	while(value >= 0x80)
	{
	ar << (BYTE)(value|0x80);
	value >>= 7;
	}
	ar << (byte)value;
	+
	if(nLength>0)
	{
	ar.Write(pBuffer,nLength*sizeof(BYTE));
	delete [] pBuffer;
	}*/
}

DWORD AFXAPI PackStrCE(const CString& string, CFile& file)
{
	/*DWORD size = 0;

	DWORD dwLength = string.GetLength();
	if(dwLength>0)
	{
	dwLength = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string.GetBuffer(0), -1, nullptr, 0)-1;

	file.Write(&dwLength,sizeof(DWORD));
	size += sizeof(DWORD);

	OLECHAR* wChars = MultiByteToUnicode(string.GetBuffer(0));

	file.Write(wChars,dwLength*sizeof(WCHAR));
	delete [] wChars;

	size += dwLength*sizeof(WCHAR);
	}
	else
	{
	file.Write(&dwLength,sizeof(DWORD));
	size += sizeof(DWORD);
	}
	return size;*/

	return 0;
}

void SaveAsUTF8(CFile& file, CString string)
{
	string=string.Trim();
	if(string.IsEmpty() == FALSE)
	{
		unsigned short count=0;
		BYTE* bytes=CStringToUTF8(string, count);
		unsigned short size=count;
		ReverseOrder(size);
		file.Write(&size, sizeof(unsigned short));

		if(bytes != nullptr)
		{
			file.Write(bytes, count);
			delete[] bytes;
		}
	}
	else
	{
		const unsigned short count=0;
		file.Write(&count, sizeof(unsigned short));
	}
}
void SaveAsUTF8(CFile& file, CStringA string)
{
	string = string.Trim();
	if(string.IsEmpty() == FALSE)
	{
		unsigned short count = string.GetLength();
		BYTE* bytes = CStringAToUTF8(string, count);
		unsigned short size = count;
		ReverseOrder(size);
		file.Write(&size, sizeof(unsigned short));

		if(bytes != nullptr)
		{
			file.Write(bytes, count);
			delete[] bytes;
		}
	}
	else
	{
		const unsigned short count = 0;
		file.Write(&count, sizeof(unsigned short));
	}
}
void SaveAsUTF8(BinaryStream& stream, CStringA string)
{
	string = string.Trim();
	if(string.IsEmpty() == FALSE)
	{
		unsigned short count = 0;
		BYTE* bytes = CStringAToUTF8(string, count);
		if(bytes != nullptr)
		{
			stream << bytes;
			delete[] bytes;
		}
	}
	else
	{
		stream << (Byte)0;
	}
}
void SaveAsUTF8(BinaryStream& stream, CString string)
{
	string=string.Trim();
	if(string.IsEmpty() == FALSE)
	{
		unsigned short count=0;
		BYTE* bytes=CStringToUTF8(string, count);
		if(bytes != nullptr)
		{
			stream << bytes;
			delete[] bytes;
		}
	}
	else
	{
		stream << (Byte)0;
	}
}
void SaveAsAscii(CFile& file, const char* string)
{
	if(string != nullptr)
	{
		const unsigned short count=strlen(string);
		unsigned short size=count;
		ReverseOrder(size);
		file.Write(&size, sizeof(unsigned short));
		file.Write(string, count);
	}
	else
	{
		const unsigned short count=0;
		file.Write(&count, sizeof(unsigned short));
	}
}
void stringJSON(std::string& input){
	size_t pos=0;
	while((pos=input.find("\"", pos)) != std::string::npos) {
		input.replace(pos, 1, "\\\"");
		pos+=2;
	}
}
std::string escapeJSON(const std::string& input)
{
	std::ostringstream o;
	for(auto c=input.cbegin(); c != input.cend(); c++) {
		switch(*c) {
			case '"':
				o << "\\\"";
				break;
			case '\\':
				o << "\\\\";
				break;
			case '\b':
				o << "\\b";
				break;
			case '\f':
				o << "\\f";
				break;
			case '\n':
				o << "\\n";
				break;
			case '\r':
				o << "\\r";
				break;
			case '\t':
				o << "\\t";
				break;
			default:
				if('\x00' <= *c && *c <= '\x1f')
					o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
				else
					o << *c;
				break;
		}
	}
	return o.str();
}

std::string EscapedASCII(CString string)
{
	return std::string(CW2A(string.GetString(), CP_UTF8));

//	if(string.IsEmpty() == FALSE)
//	{
//		std::string str;
//		for(int index=0; index < string.GetLength(); index++)
//		{
//			const TCHAR c=string.GetAt(index);
//			if(c > 127)
//			{
//				str+="\\u";
//				char buf[5];
//				sprintf_s(buf, "%04X", c);
//				str+=buf;
//			}
//			else
//			{
//				str+=c;
//			}
//		}
//		return str;
//	}
//	else
//		return {};
}
void SaveAsEscapedASCII(boost::json::object& json, const char* name, CString string)
{
	if(string.IsEmpty() == FALSE)
	{
		std::string str;
		for(int index=0; index < string.GetLength(); index++)
		{
			const TCHAR c=string.GetAt(index);
			if(c > 127)
			{
				str+="\\u";
				char buf[5];
				sprintf_s(buf, "%04X", c);
				str+=buf;
			}
			else
			{
				str+=c;
			}
		}
		json[name]=str.c_str();
	}
}

std::string StringToEscapedASCII(CString string)
{
	string=string.Trim();

	std::string str;
	for(int index=0; index < string.GetLength(); index++)
	{
		const TCHAR c=string.GetAt(index);
		if(c > 127)
		{
			str+="\\u";
			char buf[5];
			sprintf_s(buf, "%04X", c);
			str+=buf;
		}
		else
		{
			str+=c;
		}
	}
	return str;
}

BOOL DeleteDirectory(LPCTSTR DirName)
{
	CFileFind tempFind;
	TCHAR tempFileFind[200];
	_stprintf(tempFileFind, _T("%s\\*.*"), DirName);

	BOOL bFound=(BOOL)tempFind.FindFile(tempFileFind);
	while(bFound)
	{
		bFound=(BOOL)tempFind.FindNextFile();
		if(!tempFind.IsDots())
		{
			CString strFile=tempFind.GetFileName();
			TCHAR foundFileName[200];
			_tcscpy(foundFileName, strFile.GetBuffer(200));
			strFile.ReleaseBuffer();
			if(tempFind.IsDirectory())
			{
				TCHAR tempDir[200];
				_stprintf(tempDir, _T("%s\\%s"), DirName, foundFileName);
				DeleteDirectory(tempDir);
			}
			else
			{
				TCHAR tempFileName[200];
				_stprintf(tempFileName, _T("%s\\%s"), DirName, foundFileName);
				DeleteFile(tempFileName);
			}
		}
	}
	tempFind.Close();

	if(!RemoveDirectory(DirName))
	{
		CString strMessage;
		strMessage.Format(_T("Failed to remove the %s!"), DirName);
		AfxMessageBox(strMessage, MB_OK);
		return FALSE;
	}

	return TRUE;
}

CString ConvertToOctalString(CString string)
{
	CString str;
	const int count=string.GetLength();
	int i=0;
	while(i < count)
	{
		TCHAR c=string.GetAt(i++);
		if(c == L'(')
			str+=L"\\(";
		else if(c == ')')
			str+=L"\\)";
		else if(c == L'\\')
			str+=L"\\\\";
		else if(c < 0X81)
			str+=c;
		else
		{
			const ULONG ulCnt=::WideCharToMultiByte(CP_ACP, 0, &c, 1, nullptr, 0, nullptr, nullptr);
			char* pStr=new char[ulCnt];
			WideCharToMultiByte(CP_ACP, 0, &c, -1, pStr, ulCnt, nullptr, nullptr);
			for(int index=0; index < ulCnt; index++)
			{
				CStringA ch;
				ch.Format("\\%o", (BYTE)(0XFF & pStr[index]));
				str+=ch;
			}
			delete[] pStr;
		}
	}

	return str;
}

float PointToPixel(float numeral, Gdiplus::Graphics& g)
{
	return numeral*g.GetDpiX()/72.0f;
}

void PPtoDPtoLP(Gdiplus::Graphics& g, CSize& size)
{
	const float cx=size.cx*g.GetDpiX()/72.0f;
	const float cy=size.cy*g.GetDpiY()/72.0f;

	Gdiplus::PointF point1(0, 0);
	Gdiplus::PointF point2(cx, cy);
	g.TransformPoints(Gdiplus::CoordinateSpaceWorld, Gdiplus::CoordinateSpaceDevice, &point1, 1);
	g.TransformPoints(Gdiplus::CoordinateSpaceWorld, Gdiplus::CoordinateSpaceDevice, &point2, 1);

	size.cx=std::abs(point2.X - point1.X);
	size.cy=std::abs(point2.Y - point1.Y);
}

void DPtoLP(Gdiplus::Graphics& g, CSize& size)
{
	Gdiplus::PointF point1(0, 0);
	Gdiplus::PointF point2(size.cx, size.cy);
	g.TransformPoints(Gdiplus::CoordinateSpaceWorld, Gdiplus::CoordinateSpaceDevice, &point1, 1);
	g.TransformPoints(Gdiplus::CoordinateSpaceWorld, Gdiplus::CoordinateSpaceDevice, &point2, 1);

	size.cx=(int)std::abs(point2.X - point1.X);
	size.cy=(int)std::abs(point2.Y - point1.Y);
}

DWORD AfxGetAuthorizationKey()
{
	return 0XFFFFFFFF;;

	DWORD dwKey=0B00000000000000000000000000000000;
	dwKey|=LICENCE::BACKGROUND;

	HKEY keyRet;
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, &keyRet) == ERROR_SUCCESS)
	{
		DWORD type=REG_SZ;
		DWORD size=256;
		char value[256]="000-0098007-05600";
		if(RegQueryValueExA(keyRet, "MProductId", nullptr, &type, (LPBYTE)value, &size) == ERROR_SUCCESS)
		{
			int times=0;
			sscanf((char*)value, "000-0098007-056%d", &times);
			if(times <= 50)
				dwKey|=0XFFFFFFFF;
		}
		RegCloseKey(keyRet);
	}
	return dwKey;
	/*
	if(DogOpen(0XE6F3,0X263A,0XFFFF,0X0000) != DOG_NOERROR)
	{
	if(strSoftware == _T("Viewer"))
	{
	if(DogOpen(0XE6F3,0X263A,0X000F,0X0000) != DOG_NOERROR)
	{
	AfxMessageBox(_T("没有安装指定的加密狗!"));
	return false;
	}
	}
	else if(strSoftware == _T("Mapseed"))
	{
	if(DogOpen(0XE6F3,0X263A,0X00F0,0X0000) != DOG_NOERROR)
	{
	AfxMessageBox(_T("没有安装指定的加密狗!"));
	return false;
	}
	}
	else
	{
	AfxMessageBox(_T("没有安装指定的加密狗!"));
	return false;
	}
	}

	if(DogCheck(0X0717) != DOG_NOERROR)
	{
	AfxMessageBox(_T("加密狗的密码不正确!"));
	DogClose() ;
	return false;
	}
	*/
}

void AfxIncreaseCounter()
{
	HKEY keyRet;
	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows\\CurrentVersion", 0, KEY_READ | KEY_WRITE | KEY_WOW64_64KEY, &keyRet) == ERROR_SUCCESS)
	{
		DWORD type=REG_SZ;
		DWORD size=256;
		const char value[256]="000-0098007-05600";
		if(RegQueryValueExA(keyRet, "MProductId", nullptr, &type, (LPBYTE)value, &size) != ERROR_SUCCESS)
		{
			RegSetValueExA(keyRet, "MProductId", 0, REG_SZ, (LPBYTE)value, strlen(value));
		}
		else
		{
			int times=0;
			sscanf((char*)value, "000-0098007-056%d", &times);
			sprintf((char*)value, "000-0098007-056%02d", times + 1);
			size=strlen((char*)value);
			RegSetValueExA(keyRet, "MProductId", 0, REG_SZ, (unsigned char*)value, size);//MProductId    
		}
		RegCloseKey(keyRet);
	}
}

/*
CString strTmpPath;
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
char szDir[MAX_PATH];
switch(uMsg)
{
case BFFM_INITIALIZED:
if(lpData)
{
strcpy_s(szDir, strTmpPath.GetBuffer(strTmpPath.GetLength()));
SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szDir);
}
break;
case BFFM_SELCHANGED:
if(SHGetPathFromIDList((LPITEMIDLIST) lParam ,szDir))
{
SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
}
break;
default:
break;
}

return 0;
}

BOOL GetFolder(CString* strSelectedFolder, const HWND hwndOwner)
{
char pszDisplayName[MAX_PATH];

BROWSEINFOA bi;
bi.hwndOwner = hwndOwner;
bi.pidlRoot = nullptr;
bi.pszDisplayName = pszDisplayName;
bi.lpszTitle = _T("选择文件夹");
bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
bi.lpfn = BrowseCallbackProc;
bi.lParam = FALSE;
bi.iImage = nullptr;
LPITEMIDLIST lpID = SHBrowseForFolderA(&bi);
if(lpID != nullptr)
{
if(SHGetPathFromIDList(lpID, pszDisplayName) == TRUE)
{
strSelectedFolder->Format("%s",pszDisplayName);
return TRUE;
}
}
else
{
strSelectedFolder->Empty();
}

return FALSE;
}
*/

BYTE* CStringToUTF8(const CString& string, unsigned short& nByteCount)
{
	BYTE* descBuff=nullptr;
#if defined(_UNICODE)
	const int len=string.GetLength();
	wchar_t* wcBuff=new wchar_t[len + 1];
	memcpy(wcBuff, string, len*sizeof(wchar_t));
	wcBuff[len]=L'\0';
	descBuff=new BYTE[len*3];
#else
	int len=MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string), -1, nullptr, 0);
	len-=1;
	wchar_t* wcBuff=new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, string, -1, wcBuff, len);
	wcBuff[len]=L'\0';

	descBuff=new BYTE[len*3];
#endif

	nByteCount=0;
	for(int i=0; i < len; i++)
	{
		if(wcBuff[i] < 128)
		{
			descBuff[nByteCount++]=wcBuff[i];
		}
		else if(wcBuff[i] < 2047)
		{
			BYTE bt2=(wcBuff[i] >> 6);
			bt2&=0x1F;
			descBuff[nByteCount++]=(0xC0 | bt2);
			const BYTE bt3=(wcBuff[i] & 0x3F);
			descBuff[nByteCount++]=(0x80 | bt3);
		}
		else
		{
			const BYTE bt1=(wcBuff[i] >> 12);
			descBuff[nByteCount++]=(0xE0 | bt1);

			BYTE bt2=(wcBuff[i] >> 6);
			bt2&=0x3F;
			descBuff[nByteCount++]=(0x80 | bt2);
			const BYTE bt3=(wcBuff[i] & 0x3F);
			descBuff[nByteCount++]=(0x80 | bt3);
		}
	}

	delete[] wcBuff;
	wcBuff=nullptr;

	return descBuff;
}
BYTE* CStringAToUTF8(const CStringA& string, unsigned short& nByteCount)
{
	int len = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string, -1, nullptr, 0) - 1;
	wchar_t* wcBuff = new wchar_t[len + 1];
	MultiByteToWideChar(CP_ACP, 0, string, -1, wcBuff, len);
	wcBuff[len] = L'\0';

	BYTE* descBuff = new BYTE[len * 3];

	nByteCount = 0;
	for(int i = 0; i < len; i++)
	{
		if(wcBuff[i] < 128)
		{
			descBuff[nByteCount++] = wcBuff[i];
		}
		else if(wcBuff[i] < 2047)
		{
			BYTE bt2 = (wcBuff[i] >> 6);
			bt2 &= 0x1F;
			descBuff[nByteCount++] = (0xC0 | bt2);
			const BYTE bt3 = (wcBuff[i] & 0x3F);
			descBuff[nByteCount++] = (0x80 | bt3);
		}
		else
		{
			const BYTE bt1 = (wcBuff[i] >> 12);
			descBuff[nByteCount++] = (0xE0 | bt1);

			BYTE bt2 = (wcBuff[i] >> 6);
			bt2 &= 0x3F;
			descBuff[nByteCount++] = (0x80 | bt2);
			const BYTE bt3 = (wcBuff[i] & 0x3F);
			descBuff[nByteCount++] = (0x80 | bt3);
		}
	}

	delete[] wcBuff;
	wcBuff = nullptr;

	return descBuff;
}

int GetIndexFromOBList(CObList& oblist, const CObject* object)
{
	int index=-1;
	POSITION pos=oblist.GetHeadPosition();
	while(pos != nullptr)
	{
		index++;

		CObject* pObject=oblist.GetNext(pos);
		if(pObject == object)
			break;
	}

	return index;
}

BYTE FAR* Compress(BYTE FAR* pBytes, unsigned long length, bool base64, unsigned long FAR& newLength)
{
	newLength=compressBound(length);
	BYTE FAR* pResult=new BYTE[newLength];
	if(::compress(pResult, &newLength, pBytes, length) == Z_OK)
	{
		if(pResult != nullptr && newLength > 0)
		{
			if(base64 == true)
			{
				char tail[3]={0, 0, 0};
				typedef base64_from_binary<transform_width<const char*, 6, 8>> base64_enc;
				const unsigned int one_third_len=newLength/3;
				const unsigned int len_rounded_down=one_third_len*3;
				const unsigned int j=len_rounded_down + one_third_len;
				BYTE FAR* encoded=new BYTE[j];

				std::copy(base64_enc(pResult), base64_enc(pResult + len_rounded_down), encoded);

				if(len_rounded_down != newLength)
				{
					for(unsigned int i=0; i < newLength - len_rounded_down; ++i)
					{
						tail[i]=pResult[len_rounded_down + i];
					}
					std::copy(base64_enc(tail), base64_enc(tail + 3), encoded + j);
					for(unsigned int i=newLength + one_third_len + 1; i < j + 4; ++i)
					{
						encoded[i]='=';
					}
				}
				delete[] pResult;
				pResult=encoded;
				newLength=j;
			}
		}
	}
	return pResult;
}

//download file
//#include <urlmon.h>
//#pragma comment(lib, "urlmon.lib")
//
//for(int index=1;index<9680;index++)
//{
//	CString strURL;
//	strURL.Format("http://www.colorschemer.com/schemes/xml.php?id=%d",index);
//	CString strFile;
//	strFile.Format("C:\\35\\%d.xml",index);
//
//	HRESULT hr = URLDownloadToFile ( nullptr,strURL , strFile.GetBuffer(MAX_PATH), 0, nullptr );
//
//}

DWORD HextoDec(CString strHex)
{
	DWORD nDec=0;

	_stscanf(strHex, _T("%x"), &nDec);

	/*
	for(int index=strHex.GetLength()-1;index>=0;index--)
	{
	int cur = 0;

	TCHAR c = strHex[index];
	if(c>=_T('A') && c<=_T('F'))
	cur = c-_T('A') + 10;
	else
	cur = c-_T('0');

	nDec = (nDec*16) + cur;
	}
	*/
	return nDec;
}

CString DectoHex(int nDec)
{
	CString strHex;
	/*	for(int index=strDec.GetLength()-1;index>=0;index--)
	{
	int cur = 0;

	TCHAR c = strDec.Mid(index,1);
	if(c>=_T('A') && c<=_T('F'))
	cur = c-_T('A') + 10;
	else
	cur = c-_T('0');

	nDec = (nDec*16) + cur;
	}
	*/
	return strHex;
}

CRect InflateRect(CRect rect, int cx, int cy)
{
	const double left=(double)rect.left - (double)cx;
	const double right=(double)rect.right + (double)cx;
	const double top=(double)rect.top - (double)cy;
	const double bottom=(double)rect.bottom + (double)cy;

	rect.left=left <= INT_MIN ? INT_MIN : left;
	rect.right=right >= INT_MAX ? INT_MAX : right;
	rect.top=top <= INT_MIN ? INT_MIN : top;
	rect.bottom=bottom >= INT_MAX ? INT_MAX : bottom;
	return rect;
}

void DrawGdiplusBitmapOnGDI(CDC& dc, CPoint& point, Gdiplus::Bitmap* bitmap)
{
	HBITMAP hBitmap=nullptr;
	if(bitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap) == Gdiplus::Ok)
	{
		CBitmap cbitmap;
		cbitmap.Attach(hBitmap);

		CDC mdc;
		mdc.CreateCompatibleDC(&dc);
		CBitmap* oldBmp=mdc.SelectObject(&cbitmap);
		dc.TransparentBlt(point.x, point.y, bitmap->GetWidth(), bitmap->GetHeight(), &mdc, 0, 0, bitmap->GetWidth(), bitmap->GetHeight(), RGB(254, 254, 254));
		mdc.SelectObject(oldBmp);
		mdc.DeleteDC();
		cbitmap.Detach();
	}

	::DeleteObject(hBitmap);
}

void DrawGDIHBitmapOnGDI(CDC& dc, CPoint& point, HBITMAP hBitmap)
{
	if(hBitmap != nullptr)
	{
		BITMAP bm;
		GetObject(hBitmap, sizeof(BITMAP), &bm);

		CBitmap bitmap;
		bitmap.Attach(hBitmap);

		CDC mdc;
		mdc.CreateCompatibleDC(&dc);
		CBitmap* oldBmp=mdc.SelectObject(&bitmap);
		dc.BitBlt(point.x, point.y, bm.bmWidth, bm.bmHeight, &mdc, 0, 0, SRCCOPY);
		mdc.SelectObject(oldBmp);
		mdc.DeleteDC();
		bitmap.Detach();
	}
}

HBITMAP CaptureScreen(CWnd* pWnd, CRect& rect)
{
	if(pWnd == nullptr)
		return nullptr;
	if(rect.IsRectEmpty())
		return nullptr;

	CRect client;
	pWnd->GetClientRect(client);
	rect.IntersectRect(rect, client);

	CClientDC dc(pWnd);
	CDC mdc;
	mdc.CreateCompatibleDC(&dc);
	const HBITMAP hBitbmp=::CreateBitmap(rect.Width(), rect.Height(), 1, mdc.GetDeviceCaps(BITSPIXEL), nullptr);
	const HANDLE oldBmp=mdc.SelectObject(hBitbmp);
	mdc.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCCOPY);
	mdc.SelectObject(oldBmp);
	mdc.DeleteDC();

	return hBitbmp;
}

HBITMAP CaptureScreen(CDC& dc, CRect& rect)
{
	if(rect.IsRectEmpty())
		return nullptr;

	CDC mdc;
	if(mdc.CreateCompatibleDC(&dc) == TRUE)
	{
		const HBITMAP hBitbmp=::CreateBitmap(rect.Width(), rect.Height(), 1, mdc.GetDeviceCaps(BITSPIXEL), nullptr);
		const HANDLE oldBmp=mdc.SelectObject(hBitbmp);
		mdc.BitBlt(0, 0, rect.Width(), rect.Height(), &dc, rect.left, rect.top, SRCCOPY);
		mdc.SelectObject(oldBmp);
		mdc.DeleteDC();
		return hBitbmp;
	}
	else
		return nullptr;
}
