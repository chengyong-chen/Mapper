#include "StdAfx.h"
#include <afxinet.h>
#include "WebService.h"
#include "HttpUrlParser.hpp"
#include <iphlpapi.h>


using namespace std;
#include <string>
#include <map>					// for the STL multimap container

#pragma comment(lib, "iphlpapi")

#include "../Public/Base64.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#endif

bool CWebService::IsInternetAvailable()
{
	bool bIsInternetAvailable=false;
	DWORD dwBufferSize=0;
	if(ERROR_INSUFFICIENT_BUFFER == GetIpForwardTable(nullptr, &dwBufferSize, false))
	{
		BYTE* pByte=new BYTE[dwBufferSize];
		if(pByte)
		{
			const PMIB_IPFORWARDTABLE pRoutingTable=reinterpret_cast<PMIB_IPFORWARDTABLE>(pByte);
			if(NO_ERROR == GetIpForwardTable(pRoutingTable, &dwBufferSize, false))
			{
				const DWORD dwRowCount=pRoutingTable->dwNumEntries; // Get row count
				for(DWORD dwIndex=0; dwIndex < dwRowCount; ++dwIndex)
				{
					if(pRoutingTable->table[dwIndex].dwForwardDest == 0)
					{
						bIsInternetAvailable=true;
						break;
					}
				}
			}
			delete[] pByte;
		}
	}
	return bIsInternetAvailable;
}
char* CWebService::CreateBoundary(void)
{
	GUID			guid;
	if(FAILED(::CoCreateGuid(&guid)))
		return nullptr;
	const PSTR		boundary=(PSTR) ::malloc(sizeof(CHAR)*44);
	::sprintf(boundary, "----LYOUL-%.08x%.04x%.04x%.02x%.02x%.02x%.02x%.02x%.02x%.02x%.02x-", guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);

	return boundary;
}
void CWebService::CallService(std::string url, CString tenant, CString username, CString password, std::string payload, function<void(CString)> failed, function<void(std::string)> success)
{
	int	totalBytes=payload.size() + 2;
	auto contenttype=[](CHttpFile* pHttpFile)
	{
		pHttpFile->AddRequestHeaders(_T("Content-Type: application/json\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
	};
	auto buffersize=[&]()
	{
		return totalBytes;
	};
	auto writePayload=[&](CHttpFile* pHttpFile)
	{
		pHttpFile->Write(payload.c_str(), payload.size());
		pHttpFile->Write("\r\n", 2);
	};

	CWebService::CallService(url, tenant, username, password, failed, success, contenttype, buffersize, writePayload);
}
void CWebService::CallService(std::string url, CString tenant, CString username, CString password, std::map<CString, std::variant<std::string, int>> parameters, std::map<CString, CString> files, function<void(CString)> failed, function<void(std::string)> success)
{
	int	totalBytes=0;
	char* boundary=CWebService::CreateBoundary();
	for(std::map<CString, std::variant<std::string, int>>::iterator it=parameters.begin(); it != parameters.end(); it++)
	{
		totalBytes+=2 + 43 + 2;																					//--boundary\r\n
		totalBytes+=43;																							//Content-Disposition: form-data; name=""\r\n\r\n
		totalBytes+=CWebService::GetMultiByteLength(it->first) + CWebService::GetMultiByteLength(it->second);	//parameter name value
		totalBytes+=2;																							//\r\n
	}
	for(std::map<CString, CString>::iterator it=files.begin(); it != files.end(); ++it)
	{
		HANDLE hFile=::CreateFile(it->second, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
		if(hFile == INVALID_HANDLE_VALUE)
			continue;
		DWORD dwFileSize=::GetFileSize(hFile, nullptr);
		if(dwFileSize == INVALID_FILE_SIZE)
			continue;

		totalBytes+=2 + 43 + 2;																					//--boundary\r\n
		totalBytes+=54;																							//Content-Disposition: form-data; name=""; filename=""\r\n
		totalBytes+=CWebService::GetMultiByteLength(it->first) + CWebService::GetMultiByteLength(it->second);	//parameter name value				
		totalBytes+=18;																							//Content-Type: \r\n\r\n
		char* mime=CWebService::GetFileMimeType(hFile);
		if(mime == nullptr)
			totalBytes+=strlen("application/octet-stream");
		else
		{
			totalBytes+=strlen(mime);																			//Mime Type
			::free(mime);
		}
		totalBytes+=dwFileSize;																					//file body data	
		totalBytes+=2;																							//\r\n
		::CloseHandle(hFile);
	}	
	totalBytes+=2 + strlen(boundary) + 4;																		//--boundary--\r\n" 

	auto contenttype=[&](CHttpFile* pHttpFile)
	{
		CString conenttype;
		conenttype.Format(_T("Content-Type: multipart/form-data; boundary=%s"), CString(boundary));
		pHttpFile->AddRequestHeaders(conenttype, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
	};
	auto buffersize=[&]()
	{
		return totalBytes;
	};
	auto writePayload=[&](CHttpFile* pHttpFile)
	{
		BYTE* block=(BYTE*)::malloc(1024*56 + 1);
		for(std::map<CString, CString>::iterator it=files.begin(); it != files.end(); ++it)
		{
			HANDLE hFile=::CreateFile(it->second, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
			if(hFile == INVALID_HANDLE_VALUE)
				continue;

			pHttpFile->Write("--", 2);
			pHttpFile->Write(boundary, 43);
			pHttpFile->Write("\r\n", 2);
			char* bytes1=CWebService::StringToMultiByte(it->first);
			char* bytes2=CWebService::StringToMultiByte(it->second);
			char* bytes3=(char*)::malloc(54 + strlen(bytes1) + strlen(bytes2));
			::memcpy(bytes3, "Content-Disposition: form-data; name = \"", 40);
			::memcpy(bytes3 + 40, bytes1, strlen(bytes1));
			::memcpy(bytes3 + 40 + strlen(bytes1), "\"; filename = \"", 15);
			::memcpy(bytes3 + 40 + strlen(bytes1) + 15, bytes2, strlen(bytes2));
			::memcpy(bytes3 + 40 + strlen(bytes1) + 15 + strlen(bytes2), "\"\r\n", 3);
			pHttpFile->Write(bytes3, 54 + strlen(bytes1) + strlen(bytes2));
			::free(bytes1);
			::free(bytes2);
			::free(bytes3);

			char* mime=CWebService::GetFileMimeType(hFile);
			if(mime == nullptr)
				pHttpFile->Write("Content-Type: application/octet-stream\r\n\r\n", 42);
			else
			{
				char* bytes4=(char*)::malloc(18 + strlen(mime));
				::memcpy(bytes4, "Content-Type: ", 14);
				::memcpy(bytes4 + 14, mime, strlen(mime));
				::memcpy(bytes4 + 14 + strlen(mime), "\r\n\r\n", 4);
				pHttpFile->Write(bytes4, 18 + strlen(mime));

				::free(bytes4);
				::free(mime);
			}

			::SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
			DWORD dwFileSize=::GetFileSize(hFile, nullptr);
			DWORD dwLeft=dwFileSize;
			while(dwLeft > 0)
			{
				DWORD tryToread=dwLeft > 1024*56 ? 1024*56 : dwLeft;
				DWORD finalRead=0;
				::ReadFile(hFile, block, tryToread, &finalRead, nullptr);
				pHttpFile->Write(block, finalRead);
				dwLeft-=finalRead;
			}
			pHttpFile->Write("\r\n", 2);
			::CloseHandle(hFile);
		}		
		pHttpFile->Write("--", 2);
		pHttpFile->Write(boundary, 43);
		pHttpFile->Write("--\r\n", 4);

		::free(boundary);
		::free(block);
	};

	CWebService::CallService(url, tenant, username, password, failed, success, contenttype, buffersize, writePayload);
}
void CWebService::CallService(std::string url, CString tenant, CString username, CString password, std::map<CString, std::variant<std::string, int>> parameters, std::vector<std::tuple<CStringA, const BYTE*, int>> bytes, function<void(CString)> failed, function<void(std::string)> success)
{
	int	totalBytes=0;
	char* boundary=CWebService::CreateBoundary();
	for(std::map<CString, std::variant<std::string, int>>::iterator it=parameters.begin(); it != parameters.end(); it++)
	{
		totalBytes+=2 + 43 + 2;																					//--boundary\r\n
		totalBytes+=43;																							//Content-Disposition: form-data; name=""\r\n\r\n
		totalBytes+=CWebService::GetMultiByteLength(it->first) + CWebService::GetMultiByteLength(it->second);	//parameter name value
		totalBytes+=2;																							//\r\n
	}	
	for(std::vector<std::tuple<CStringA, const BYTE*, int>>::iterator it=bytes.begin(); it != bytes.end(); ++it)
	{
		if(std::get<1>(*it) == nullptr)
			continue;

		totalBytes+=2 + 43 + 2;																					//--boundary\r\n
		totalBytes+=54;																							//Content-Disposition: form-data; name="";\r\n
		totalBytes+=std::get<0>(*it).GetLength();																//parameter name value				
		totalBytes+=18;																							//Content-Type: \r\n\r\n
		totalBytes+=strlen("application/octet-stream");
		totalBytes+=std::get<2>(*it);																			//body data	
		totalBytes+=2;																							//\r\n
	}
	totalBytes+=2 + strlen(boundary) + 4;																		//--boundary--\r\n" 

	auto contenttype=[&](CHttpFile* pHttpFile)
	{
		CString conenttype;
		conenttype.Format(_T("Content-Type: multipart/related; boundary=%s"), CString(boundary));
		pHttpFile->AddRequestHeaders(conenttype, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
	};
	auto buffersize=[&]()
	{
		return totalBytes;
	};
	auto writePayload=[&](CHttpFile* pHttpFile)
	{
		BYTE* block=(BYTE*)::malloc(1024*56 + 1);
		for(std::vector<std::tuple<CStringA, const BYTE*, int>>::iterator it=bytes.begin(); it != bytes.end(); ++it)
		{
			if(std::get<1>(*it) == nullptr)
				continue;

			totalBytes+=54;																							//Content-Disposition: form-data; name="";\r\n
			totalBytes+=std::get<0>(*it).GetLength();																//parameter name value				
			totalBytes+=18;																							//Content-Type: \r\n\r\n
			totalBytes+=strlen("application/octet-stream");
			totalBytes+=std::get<2>(*it);																			//body data	
			totalBytes+=2;																							//\r\n

			pHttpFile->Write("--", 2);
			pHttpFile->Write(boundary, 43);
			pHttpFile->Write("\r\n", 2);

			CStringA str;
			str.Format("Content-Disposition: form-data; name = \"%s\"\r\n", std::get<0>(*it));
			pHttpFile->Write(str, str.GetLength());
			pHttpFile->Write("Content-Type: application/octet-stream\r\n\r\n", 42);
			pHttpFile->Write(std::get<1>(*it), std::get<2>(*it));
			pHttpFile->Write("\r\n", 2);
		}
		pHttpFile->Write("--", 2);
		pHttpFile->Write(boundary, 43);
		pHttpFile->Write("--\r\n", 4);

		::free(boundary);
		::free(block);
	};

	CWebService::CallService(url, tenant, username, password, failed, success, contenttype, buffersize, writePayload);
}
void CWebService::CallService(std::string url, CString tenant, CString username, CString password, function<void(CString)> failed, function<void(std::string)> success, function<void(CHttpFile*)> contenttype, function<int()> buffersize, function<void(CHttpFile*)> writeContent)
{
	if(CWebService::IsInternetAvailable() == false)
	{
		failed(_T("No Internet connection!"));
		return;
	}

	CHttpUrlParser parser(url);
	if(parser.valid == false)
	{
		failed(_T("Invalid url for calling web service!"));
		return;
	}

	CHttpConnection* pHttpConnection=nullptr;
	CHttpFile* pHttpFile=nullptr;
	CInternetSession session(_T("WebService"), 1, INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, INTERNET_FLAG_DONT_CACHE);//Instantiate CInternetSession		
	try
	{
		session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 1000*60);
		session.SetOption(INTERNET_OPTION_CONNECT_RETRIES, 1);
		session.SetOption(INTERNET_OPTION_CONNECT_BACKOFF, 1000);

		pHttpConnection=session.GetHttpConnection(parser.domain, INTERNET_FLAG_NO_AUTO_REDIRECT, parser.port, username, password);//Get CHttpConnection
		if(pHttpConnection == nullptr)
		{
			failed(_T("Failed to connect to the server!"));
			session.Close();
			return;
		}

		DWORD dwFlags=INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_DONT_CACHE | INTERNET_FLAG_NO_UI | INTERNET_FLAG_RESYNCHRONIZE | INTERNET_FLAG_NO_CACHE_WRITE | INTERNET_FLAG_PRAGMA_NOCACHE | INTERNET_FLAG_RELOAD | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTP | INTERNET_FLAG_IGNORE_REDIRECT_TO_HTTPS;
		if(parser.scheme == "https")
			dwFlags|=INTERNET_FLAG_SECURE;
		pHttpFile=pHttpConnection->OpenRequest(CHttpConnection::HTTP_VERB_POST, parser.path, nullptr, 1, nullptr, _T("HTTP/1.1"), dwFlags);
		if(pHttpFile != 0 && pHttpFile->GetContext() != 0)
		{
			pHttpFile->AddRequestHeaders(_T("*/*\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			pHttpFile->AddRequestHeaders(_T("Cache-Control: no-cache\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			pHttpFile->AddRequestHeaders(_T("Ryeol-Magic: My Magic Header\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			pHttpFile->AddRequestHeaders(_T("User-Magic: User's Magic Header\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			pHttpFile->AddRequestHeaders(_T("accept-encoding: gzip, deflate, br\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			pHttpFile->AddRequestHeaders(_T("origin: https://www.diwatu.com\r\n"), HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			contenttype(pHttpFile);

			if(username.IsEmpty() == FALSE)
			{
				CString authentication;
				authentication.Format(_T("%s\\%s:%s"), tenant, username, password);
				
				char* encoded = Base64_Encode(CW2A(authentication.GetBuffer()), authentication.GetLength());
				authentication.ReleaseBuffer();

				CString authenheader;
				authenheader.Format(_T("Authorization: Basic %s"), CString(encoded));
				::free(encoded);
				pHttpFile->AddRequestHeaders(authenheader, HTTP_ADDREQ_FLAG_ADD_IF_NEW);
			}
			INTERNET_BUFFERS bufferin;
			bufferin.dwStructSize = sizeof(INTERNET_BUFFERS);	// Must be set or error will occur
			bufferin.Next = nullptr;
			bufferin.lpcszHeader = nullptr;
			bufferin.dwHeadersLength = 0;
			bufferin.dwHeadersTotal = 0;
			bufferin.lpvBuffer = nullptr;
			bufferin.dwBufferLength = 0;
			bufferin.dwBufferTotal = buffersize();				// This is the only member used other than dwStructSize
			bufferin.dwOffsetLow = 0;
			bufferin.dwOffsetHigh = 0;

			bool sent = false;
			TCHAR message[MAX_PATH + 1];
			memset(message, 0, MAX_PATH + 1);
			do {
				bool retried = false;
				try
				{
					sent = pHttpFile->SendRequestEx(&bufferin, nullptr, 0, 0);
					break;
				}
				catch(CInternetException* ex)
				{
					if(ex->m_dwError == ERROR_INTERNET_SEC_CERT_DATE_INVALID || ex->m_dwError == ERROR_INTERNET_INVALID_CA && retried == false)
					{
						DWORD dwFlags;
						DWORD dwBufferLen = sizeof(dwFlags);
						pHttpFile->QueryOption(INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBufferLen);
						dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA | SECURITY_FLAG_IGNORE_CERT_CN_INVALID| SECURITY_FLAG_IGNORE_CERT_DATE_INVALID| SECURITY_FLAG_IGNORE_CERT_CN_INVALID;
						pHttpFile->SetOption(INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof(dwFlags));
						retried = true;
						ex->Delete();
					}
					else
					{						
						ex->GetErrorMessage(message, MAX_PATH);
						ex->Delete();
						break;
					}
				}
				catch(CException* ex)
				{
					ex->GetErrorMessage(message, MAX_PATH);
					ex->Delete();
					break;
				}
			} while(true);

			if(sent == false)
			{
				_tcsclen(message) == 0 ? failed(_T("Failed to send the request to the server!")) : failed(message);
			}
			else
			{
				writeContent(pHttpFile);

				pHttpFile->EndRequest();

				DWORD dwStatucCode = 0;
				pHttpFile->QueryInfoStatusCode(dwStatucCode);// Check the return HTTP Status Code
				if(dwStatucCode == HTTP_STATUS_OK)
				{
					std::string response;
					while(true)
					{
						char szBuf[1025] = {0};
						UINT nBytesRead = pHttpFile->Read((void*)szBuf, 1024);
						response += std::string(szBuf);
						if(nBytesRead < 1024)
							break;
					}
					success(response);
				}
				else
				{
					CString message;
					message.Format(_T("Failed to send the request to the server with error code: %d!"), dwStatucCode);
					failed(message);
				}
			}
		}		
	}
	catch(CInternetException* ex)
	{
		TCHAR message[MAX_PATH + 1];
		memset(message, 0, MAX_PATH + 1);
		ex->GetErrorMessage(message, MAX_PATH);
		ex->Delete();
		failed(message);
	}
	catch(CException* ex)
	{
		TCHAR message[MAX_PATH + 1];
		memset(message, 0, MAX_PATH + 1);
		ex->GetErrorMessage(message, MAX_PATH);
		ex->Delete();
		failed(message);
	}

	session.Close();
	if(pHttpFile != nullptr)
	{
		pHttpFile->Close();
		delete pHttpFile;
	}
	if(pHttpConnection != nullptr)
	{
		pHttpConnection->Close();
		delete pHttpConnection;
	}
}
int CWebService::GetMultiByteLength(CString str)
{
#ifdef _UNICODE 
	return _tcslen(str);
#else
	return str.length;
#endif
}
int CWebService::GetMultiByteLength(std::variant<std::string, int> var)
{
	try
	{
		const std::string value=std::get<std::string>(var);
		return value.size();
	}
	catch(std::bad_variant_access&)
	{

	}
	try
	{
		const int value=std::get<int>(var);
		const std::string str=std::to_string(value);
		return str.size();
	}
	catch(std::bad_variant_access&)
	{

	}
	return 0;
}
std::string CWebService::VariantToString(std::variant<std::string, int> var)
{
	try
	{
		std::string value=std::get<std::string>(var);
		return value;
	}
	catch(std::bad_variant_access&)
	{

	}
	try
	{
		const int value=std::get<int>(var);
		return std::to_string(value);
	}
	catch(std::bad_variant_access&)
	{

	}
	return "";
}
char* CWebService::StringToMultiByte(CString str)
{
#ifdef _UNICODE 
	const int dwLength=_tcslen(str) + 1;
	char* lpFormData=new char[dwLength];
	::WideCharToMultiByte(CP_ACP, 0, str, -1, lpFormData, dwLength, 0, 0);
	return lpFormData;
#else
	return str.GetBuffer();
#endif
}

CString CWebService::MakeJsonRequestHeadString(CString tenant, CString username, CString password, int length)
{
	CString strRewLen;
	strRewLen.Format(_T("%d"), length);

	CString authentication;
	authentication.Format(_T("%s\\%s:%s"), tenant, username, password);
	char* encoded=Base64_Encode(CW2A(authentication.GetBuffer()), authentication.GetLength());
	authentication.ReleaseBuffer();

	CString strRequestHead=_T("");
	strRequestHead+=_T("Authentication: Basic ") + CString(encoded) + _T("\r\n");
	strRequestHead+=_T("Content-Type: application/json; charset=utf-8\r\n");
	strRequestHead+=_T("Content-Length: ") + strRewLen + _T("\r\n");

	::free(encoded);
	return strRequestHead;
}
CString CWebService::MakeSOAPRequestHeadString(int length)
{
	CString strRewLen;
	strRewLen.Format(_T("%d"), length);

	CString strRequestHead=_T("");
	strRequestHead+=_T("Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n");
	strRequestHead+=_T("Content-Length: ") + strRewLen + _T("\r\n");
	return strRequestHead;
}


char* CWebService::GetFileMimeType(HANDLE hFile)
{
	if(hFile == INVALID_HANDLE_VALUE)
		return nullptr;

	BYTE	byBuff[256];
	DWORD	dwRead;
	if(::ReadFile(hFile, byBuff, 256, &dwRead, nullptr) != 0)
	{
		PWSTR		szMimeW=nullptr;
		PSTR		szMimeA=nullptr;
		const HRESULT		hResult=::FindMimeFromData(nullptr, nullptr, byBuff, dwRead, nullptr, 0, &szMimeW, 0);
		if(hResult != NOERROR)
			return nullptr;// "application/octet-stream";
		else
			szMimeA=CWebService::StringToMultiByte(szMimeW);

		::CoTaskMemFree(szMimeW);
		return szMimeA;
	}
	return nullptr;
}
char* CWebService::BitmapToString64(Gdiplus::Bitmap* bitmap)
{
	char* string64=nullptr;
	if(bitmap != nullptr)
	{
		CLSID pngClsid;
		CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &pngClsid);

		IStream* istream=nullptr;
		CreateStreamOnHGlobal(nullptr, TRUE, &istream);
		bitmap->Save(istream, &pngClsid);

		//get memory handle associated with istream
		HGLOBAL hg=nullptr;
		GetHGlobalFromStream(istream, &hg);

		const int size=GlobalSize(hg);
		void* buffer=malloc(size);

		const LPVOID pimage=GlobalLock(hg);
		memcpy(buffer, pimage, size);
		GlobalUnlock(hg);
		istream->Release();

		string64=Base64_Encode((char*)buffer, size);
		free(buffer);
	}
	return string64;
}

