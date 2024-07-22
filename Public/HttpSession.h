#pragma once

#include <afxinet.h>

class CHttpFile;
struct RecvHTTPPackage;

class __declspec(dllexport) CHTTPSession : public CInternetSession
{
public:
	CHTTPSession(LPCTSTR pszAppName, int nMethod);

public:
	CHttpConnection* m_pHTTPConnection;
	CHttpFile* m_pHTTPFile;

public:
	BOOL Open(CString strURL, CString strParameter, DWORD dwFlag);
	void Close() override;
	void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen) override;

public:
	CString ReadContent() const;
};
