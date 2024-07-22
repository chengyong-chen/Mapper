// ServerConnection.cpp: implementation of the CServerConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ListenSocket.h"
#include <process.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

typedef void (AcptCallback)(SOCKET socket, DWORD dwParent);

CListenSocket::CListenSocket()
{
	SECURITY_ATTRIBUTES sat;
	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.lpSecurityDescriptor = nullptr;
	sat.bInheritHandle = TRUE;
	m_hTerminated = CreateEvent(&sat, TRUE, FALSE, nullptr);
	m_hSockClosed = CreateEvent(nullptr, TRUE, FALSE, nullptr);
	m_pParent = nullptr;

	SetEvent(m_hTerminated);
	SetEvent(m_hSockClosed);
}

CListenSocket::~CListenSocket()
{
	Close();

	if(m_hTerminated!=nullptr)
	{
		CloseHandle(m_hTerminated);
		m_hTerminated = nullptr;
	}

	if(m_hSockClosed!=nullptr)
	{
		CloseHandle(m_hSockClosed);
		m_hSockClosed = nullptr;
	}

	m_pParent = nullptr;
}

BOOL CListenSocket::StartListen(int nPort)
{
	Close();

	m_port = nPort;
	m_socket = socket(AF_INET, SOCK_STREAM, 0);

	ResetEvent(m_hTerminated);
	ResetEvent(m_hSockClosed);

	::_beginthread(CListenSocket::ListenThreadFunc, 0, (void*)this);

	if(WaitForSingleObject(m_hSockClosed, 1000)==WAIT_OBJECT_0)
		return FALSE;
	else
		return TRUE;
}

BOOL CListenSocket::IsTerminated() const
{
	return (WaitForSingleObject(m_hTerminated, 1)==WAIT_OBJECT_0);
}

void CListenSocket::Close() const
{
	SetEvent(m_hTerminated);

	closesocket(m_socket);

	WaitForSingleObject(m_hSockClosed, INFINITE);
}

void CListenSocket::SetAcptCallbackFuc(void* callbackfun)
{
	acptCallbackFuc = callbackfun;
}

void CListenSocket::SetParent(CWnd* pParent)
{
	m_pParent = pParent;
}

void CListenSocket::ListenThreadFunc(void* param)
{
	CListenSocket* pServer = (CListenSocket*)param;
	if(pServer==nullptr)
		return;

	ResetEvent(pServer->m_hTerminated);
	ResetEvent(pServer->m_hSockClosed);

	try
	{
		sockaddr_in sai;
		memset(&sai, 0, sizeof(sockaddr_in));
		sai.sin_port = htons(pServer->GetPort());
		sai.sin_family = AF_INET;
		if(bind(pServer->m_socket, (sockaddr*)&sai, sizeof(sockaddr_in))==0)
		{
			int len = sizeof(sai);
			getsockname(pServer->m_socket, (sockaddr*)&sai, &len);
			if(listen(pServer->m_socket, 5)==0)
			{
				while(pServer->IsTerminated()==FALSE)
				{
					int len = sizeof(sockaddr_in);
					const SOCKET incs = accept(pServer->m_socket, (sockaddr*)&sai, &len);
					if(incs==INVALID_SOCKET)
						break;

					((AcptCallback*)pServer->acptCallbackFuc)((long)incs, (DWORD)pServer->m_pParent);
				}
			}
		}

		closesocket(pServer->m_socket);
	}
	catch(CException*ex)
	{
		ex->Delete();
		ASSERT(FALSE);
	};

	SetEvent(pServer->m_hSockClosed);
}
