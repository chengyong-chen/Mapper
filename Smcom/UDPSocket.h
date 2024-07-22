#pragma once

#include <map>
#include <string>
#include <afxmt.h>
using namespace std;

struct Address_IP_Port
{
	std::string m_strIP;
	UINT m_uPort;

	Address_IP_Port()
	{
		m_uPort = 0;
		m_strIP = "";
	}
};

class CUDPSocket : public CAsyncSocket
{
public:
	CUDPSocket();

	~CUDPSocket() override;

public:
	virtual bool SendMsg(LPCSTR IVUNo, LPCSTR Msg, BOOL Chinese, BOOL NeedReport);
	virtual bool SendMsg(LPCSTR IVUNo, LPCSTR Msg);

	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CUDPSocket)
public:
	void OnReceive(int nErrorCode) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CUDPSocket)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//	DECLARE_MESSAGE_MAP()

public:
	static map<DWORD, Address_IP_Port> g_mapVhcNumToAddress;
	static CMutex g_mutexMapVhcNumToAddress;

	static BOOL GetIPPort(const DWORD& dwId, CStringA& strIP, UINT& uPort);
	static void SetIPPort(const DWORD& dwId, CStringA& strIP, UINT uPort);

protected:
	static UINT DealRecvMsgThread(LPVOID lParam);
};
