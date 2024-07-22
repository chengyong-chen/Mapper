#pragma once

#include <afxsock.h>

class CTCPSocket : public CSocket //设计还不成熟，还不能直接使用
{
public:
	CTCPSocket();

	~CTCPSocket() override;

public:
	virtual bool SendMsg(LPCSTR IVUNo, LPCSTR Msg, BOOL Chinese, BOOL NeedReport);
	virtual bool SendMsg(LPCSTR IVUNo, LPCSTR Msg);

public:
	// ClassWizard generated virtual function overrides 
	//{{AFX_VIRTUAL(CTCPSocket)
	void OnAccept(int nErrorCode) override;
	void OnReceive(int nErrorCode) override;
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CTCPSocket)
	// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

protected:
	static UINT DealRecvMsgThread(LPVOID lParam);
};
