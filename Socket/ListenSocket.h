// ServerSocket.h: interface for the CServerConnection class.

//////////////////////////////////////////////////////////////////////

#pragma once

class AFX_EXT_CLASS CListenSocket
{
public:
	CListenSocket();
	virtual ~CListenSocket();

public:
	BOOL StartListen(int _port);

public:
	void* acptCallbackFuc;
	CWnd* m_pParent;

public:
	void SetAcptCallbackFuc(void* newFunc);
	void SetParent(CWnd* pParent);

private:
	UINT m_port;
	SOCKET m_socket;
public:
	UINT GetPort() const
	{
		return m_port;
	};

public:
	HANDLE m_hTerminated;
	HANDLE m_hSockClosed;

public:
	BOOL IsTerminated() const;
	void Close() const;

public:
	static void ListenThreadFunc(void* param);
};
