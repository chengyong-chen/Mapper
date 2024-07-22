#pragma once
#include <afxinet.h>

class CHttpFile;
struct RecvHTTPPackage;

class CHTTPSession : public CInternetSession
{
public:
	CHTTPSession(LPCTSTR pszAppName, int nMethod);

public:
	CHttpConnection* m_pHTTPConnection;
	CHttpFile* m_pHTTPFile;

public:
	BOOL Open(CString strURL, CString strParameter);
	void Close() override;
	void OnStatusCallback(DWORD dwContext, DWORD dwInternetStatus, LPVOID lpvStatusInfomration, DWORD dwStatusInformationLen) override;

protected:
	static void StripTags(LPTSTR pszBuffer);
	static UINT RecvMsgThread(LPVOID lParam);

private:
	UINT DealRecvMsg(RecvHTTPPackage* pPack) const;

public:
	bool m_bNeedDate;
	HANDLE RecvThreadEnded;
};
