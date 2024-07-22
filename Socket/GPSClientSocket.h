#pragma once

#include "../Socket/ClientSocket.h"

class AFX_EXT_CLASS CGPSClientSocket : public CClientSocket
{
public:
	CGPSClientSocket();

	~CGPSClientSocket() override;

public:
	BOOL SendStringToClient(DWORD vhcId, TCHAR* str);

	void* PrepareSendMsg(CMessageReceived* msg, DWORD& length) override;

public:
	void SetUserID(DWORD id) { m_dwUser = id; };
	DWORD GetUserID() const
	{
		return m_dwUser;
	};

private:
	DWORD m_dwUser;
};
