#pragma once

#pragma once

#include "Connect.h"

class CMessageReceived;

class AFX_EXT_CLASS CClientSocket : public CConnect
{
public:
	CClientSocket();
	virtual ~CClientSocket();

private:
	int m_iPulsCounter;
public:
	int IncremePulsCounter()
	{
		m_iPulsCounter++;
		return m_iPulsCounter;
	};
	void ResetPulsCounter() { m_iPulsCounter = 0; };
	BOOL IsTimerOut();

public:
	virtual BOOL InitConnection(SOCKET socket);
	void ResetConnection() override;
	void Close() override;

	void* cioRecvBytes(int& length, int& Error) override;
	BOOL cioSendBytes(void* msg, int msg_size) override;
	BOOL IsCorrectHeader(char* header, int size = 8) override;
	BOOL IsCorrectMessage(char* msg, int size) override;
	int GetMessageSizeFromHeader(char* header) override;
	virtual BOOL ConnectToServer(char* serverName, int port);

	virtual void* PrepareSendMsg(CMessageReceived* msg, DWORD& length) { return nullptr; };
	virtual BOOL SendMsg(CMessageReceived* msg);
	virtual BOOL SendMsg(char* lp, DWORD length);
	virtual BOOL SendMessage(void* msg, DWORD length);
	bool SendData(void* msg, DWORD length) override;

private:
	SOCKET m_socket;

public:
	CString m_strIP;
	CString m_strUser;
	BOOL m_logined;
	COleDateTime m_loadTime;

public:
	SOCKET GetSocket() const
	{
		return m_socket;
	};
	CString GetClientIP() const
	{
		return m_strIP;
	};
	void SetUserName(CString user) { m_strUser = user; };
	CString GetUserName() const
	{
		return m_strUser;
	};
	BOOL IsLogin() const
	{
		return m_logined;
	};
	void SetLogin(BOOL b) { m_logined = b; };
	CString GetLoadTimeString() const
	{
		return m_loadTime.Format(_T("%Y/%m/%d %H:%M"));
	};
};
