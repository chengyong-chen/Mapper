#include "stdafx.h"
#include "Global.h"
#include "Function.h"
#include <assert.h>
#include "../Utility/string.hpp"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CArchive& AFXAPI operator <<(CArchive& ar, CPointF& point)
{
	ar.Write(&point, sizeof(CPointF));
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CPointF& point)
{
	ar.Read(&point, sizeof(CPointF));
	return ar;
}

CArchive& AFXAPI operator <<(CArchive& ar, CSizeF& size)
{
	ar.Write(&size, sizeof(CSizeF));
	return ar;
}

CArchive& AFXAPI operator >>(CArchive& ar, CSizeF& size)
{
	ar.Read(&size, sizeof(CSizeF));
	return ar;
}

HWND m_hWnd = nullptr;

BOOL CALLBACK EnumThreadWndProc(HWND hWnd, LONG lParam)
{
	DWORD dwProcessID;
	GetWindowThreadProcessId(hWnd, &dwProcessID);
	if(dwProcessID==lParam)
	{
		const int nMaxCount = 256;
		TCHAR pszClassName[nMaxCount];
		::GetClassName(hWnd, pszClassName, nMaxCount);
		if(_tcsicmp(pszClassName, _T("#32770"))==0)//one identical thread and process id can hvae multiple  windows, now don't know how to identy it
		{
			m_hWnd = hWnd;
			return FALSE;
		}
		else
			return TRUE;
	}
	else
	{
		return TRUE;
	}
}

HWND GetWndFromThread(DWORD threadID, DWORD dwProcessID)
{
	if(threadID!=0)
	{
		m_hWnd = nullptr;
		::EnumThreadWindows(threadID, EnumThreadWndProc, dwProcessID);
		return m_hWnd;
	}
	else
		return nullptr;
}

CStringA GetExeDirectory()
{
	char buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, buff, sizeof(buff));
	const CStringA strPath = buff;
	return strPath.Left(strPath.ReverseFind('\\')+1);
}

CStringA FindOCXDirectory(CStringA strOCX)
{
	char buff[MAX_PATH];
	memset(buff, 0, MAX_PATH);
	::GetModuleFileNameA(nullptr, buff, sizeof(buff));
	CStringA strPath = buff;
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	if(PathFileExistsA(strPath+'\\'+strOCX))//in the same folder with exe file
		return strPath+'\\';
	else if(PathFileExistsA(strPath+"\\Controls\\"+strOCX))
		return strPath+"\\Controls\\";
	else
	{
		const CString strInstalled = AfxGetProfileString(HKEY_LOCAL_MACHINE, _T("Diwatu"), _T(""), _T("Assistant"), nullptr);
		if(PathFileExists(strInstalled+_T('\\')+(CString)strOCX))//was installed 
			return (CStringA)strInstalled;
		else
			return "";
	}
}

BOOL RegisterOCX(CStringA strOCX, Platform platform, CStringA extra)
{
	const CStringA strPath = FindOCXDirectory(strOCX);
	char systemfolder[MAX_PATH];
	GetSystemDirectoryA(systemfolder, MAX_PATH);
	const char* realdirectory = (platform==Platform::X86&&IsWow64()==TRUE) ? str::replace(systemfolder, "system32", "SysWOW64") : systemfolder;
	if(CopyFileA(strPath+strOCX, realdirectory+CStringA("\\")+strOCX, FALSE))
	{
		if(extra.IsEmpty()==FALSE)
			CopyFileA(strPath+extra, realdirectory + CStringA("\\") +extra, FALSE);
		const HMODULE hMod = LoadLibraryA(realdirectory + CStringA("\\") +strOCX);
		if(hMod!=nullptr)// Check if we can load library
		{
			const FARPROC proc = GetProcAddress(hMod, "DllRegisterServer");// Get proc address		
			if(proc!=nullptr)// Check if we can run the function
			{
				::CoInitialize(nullptr);
				const HRESULT hres = proc();
				::FreeLibrary(hMod);

				if(FAILED(hres))
					return FALSE;
				else
					return TRUE;
			}
			else
			{
				::FreeLibrary(hMod);
				return FALSE;
			}
		}
		else
			return FALSE;
	}
	else
		return FALSE;
}

typedef void (*MYFUNC)();

BOOL IsOCXRegistered(CStringA strOCX, BSTR bstrLicKey)
{
	strOCX.MakeUpper();

	CoInitialize(nullptr);

	ICatInformation* pCatInfo = nullptr;
	HRESULT hr = CoCreateInstance(CLSID_StdComponentCategoriesMgr, nullptr, CLSCTX_INPROC_SERVER, IID_ICatInformation, (void**)&pCatInfo);

	pCatInfo->AddRef();

	CATID pcatidImpl[1];
	CATID pcatidReqd[1];

	pcatidImpl[0] = CATID_Control;//We are intersted in finding out only controls so put CATID_Control in the array

	IEnumGUID* pEnumGUID = nullptr;
	pCatInfo->EnumClassesOfCategories(1, pcatidImpl, 0, pcatidReqd, &pEnumGUID);//Now enumerate the classes i.e. com objects of this type.

	CLSID clsid;
	while((hr = pEnumGUID->Next(1, &clsid, nullptr))==S_OK)//Enumerate as long as you get S_OK
	{
		LPOLESTR progID;
		if(::ProgIDFromCLSID(clsid, &progID)==S_OK)
		{
			LPOLESTR lpOleStr = nullptr;
			StringFromCLSID(clsid, &lpOleStr);
			::CoTaskMemFree(progID);
			CStringA str(lpOleStr);
			::CoTaskMemFree(lpOleStr);

			CStringA strKey = "SOFTWARE\\Classes\\CLSID\\"+str+"\\InprocServer32";
			HKEY hKey = nullptr;
			if(RegOpenKeyExA(HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hKey)==ERROR_SUCCESS)
			{
				DWORD type = REG_SZ;
				DWORD dwSize = 255;
				char szPathFile[MAX_PATH];
				if(RegQueryValueExA(hKey, nullptr, nullptr, &type, (LPBYTE)szPathFile, &dwSize)==ERROR_SUCCESS)
				{
					char* pos = strrchr(szPathFile, '\\');

					CStringA strFileName = (pos==nullptr) ? szPathFile : pos+1;
					if(strFileName.CompareNoCase(strOCX)==0/* && _access(szPathFile,00) != -1*/)//could be registered multiple, but only one works
					{
						RegCloseKey(hKey);

						pCatInfo->Release();//we are done so now release the interface ptr
						CoUninitialize();
						return TRUE;//need more code to check if the ocx is really working, after this when trying to create the ocx instance, it still can fail.
					}
				}

				RegCloseKey(hKey);
			}
		}
	}

	pCatInfo->Release();//we are done so now release the interface ptr
	CoUninitialize();
	return FALSE;
}

//
//CStringArray TokenString(CString string,CString seperator )
//{
//	CStringArray array;
//	int position = 0;
//	do
//	{
//		CString token = string.Tokenize(seperator, position);
//		if(token.IsEmpty() == true)
//			break;
//		else
//		{
//			array.Add(token);
//		}
//	}while(true);
//
//	return array;
//}

void DeleteFiles(CString strWildcards)
{
	CFileFind filefind;
	while(filefind.FindFile(strWildcards)==TRUE)
	{
		filefind.FindNextFile();
		if(filefind.IsDirectory())
			continue;
		else if(filefind.IsDots()==TRUE)
			continue;
		else if(filefind.IsDirectory()==TRUE)
			continue;
		else
			DeleteFile(filefind.GetFilePath());
	}
}

typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS)(HANDLE, PBOOL);

BOOL IsWow64()
{
	//IsWow64Process is not available on all supported versions of Windows.
	//Use GetModuleHandle to get a handle to the DLL that contains the function
	//and GetProcAddress to get a pointer to the function if available.
	const LPFN_ISWOW64PROCESS fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "IsWow64Process");
	if(fnIsWow64Process!=nullptr)
	{
		BOOL bIsWow64 = FALSE;
		fnIsWow64Process(GetCurrentProcess(), &bIsWow64);
		return bIsWow64;
	}
	else
		return FALSE;
}
bool IsNumeric(const char* string)
{
	return IsDecimal(string)||IsInteger(string);
}

bool IsDecimal(const char* string)
{
	try
	{
		double value = atof(string);
		return true;
	}
	catch(...)
	{
	}

	return false;
}

bool IsInteger(const char* string)
{
	try
	{
		double value = atoi(string);
		return true;
	}
	catch(...)
	{
	}

	return false;
}

void Replace(CStringA& string, CStringA original, int data)
{
	char chars[256];
	sprintf_s(chars, 256, "%d", data);
	string.Replace(original, chars);
}

bool CreateDeepDirectory(LPCSTR lpszPath)
{
	if(PathFileExistsA(lpszPath))
		return true;

	// Ignore trailing backslash
	int len = strlen(lpszPath);
	if(lpszPath[len-1]=='\\')
		len--;

	// Skip past drive specifier
	int nCurrLen = 0;
	if(len>=3&&lpszPath[1]==':'&&lpszPath[2]=='\\')
		nCurrLen = 2;

	// We can't create root so skip past any root specifier
	while(lpszPath[nCurrLen]=='\\')
		nCurrLen++;

	// Test each component of this path, creating directories as needed
	while(nCurrLen<len)
	{
		// Parse next path compenent
		const LPCSTR psz = strchr(lpszPath+nCurrLen, '\\');
		if(psz!=nullptr)
			nCurrLen = (int)(psz-lpszPath);
		else
			nCurrLen = len;

		// Ensure this path exists
		CStringA sPath;
		sPath.SetString(lpszPath, nCurrLen);
		if(!PathFileExistsA(sPath))
			if(!::CreateDirectoryA(sPath, nullptr))
				return false;

		// Skip over current backslash
		if(lpszPath[nCurrLen]!='\0')
			nCurrLen++;
	}
	return true;
}