// CClientSocket.cpp: implementation of the CClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientSocket.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CClientSocket::CClientSocket() :
	CConnect()
{
	m_strUser = _T("");
	m_logined = FALSE;

	m_iPulsCounter = 0;
	m_headLength = sizeof(STRUCT_MSG_HEADER);
}

CClientSocket::~CClientSocket()
{
	if(IsTerminated()==FALSE)
	{
		Close();
		WaitForTermination();
	}

	ClearProcessCallbackFunction();

	if(m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

void CClientSocket::ResetConnection()
{
	if(m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}

	CConnect::ResetConnection();
}

BOOL CClientSocket::InitConnection(SOCKET socket)
{
	SOCKADDR_IN in;
	int len = sizeof(in);
	getpeername(socket, (SOCKADDR*)&in, &len);
	m_strIP = inet_ntoa(in.sin_addr);

	m_socket = socket;
	const BOOL bl = TRUE;

	setsockopt(m_socket, IPPROTO_TCP, TCP_NODELAY, (const char*)&bl, sizeof(BOOL));
	setsockopt(m_socket, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&bl, sizeof(BOOL));

	m_loadTime = COleDateTime::GetCurrentTime();
	return TRUE;
}

void CClientSocket::Close()
{
	while(OutQueue.IsQueueEmpty()==false&&IsTerminated()==FALSE)
	{
		Sleep(1000);
	}

	CConnect::Close();
	WaitForTermination();

	if(m_socket!=INVALID_SOCKET)
	{
		closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
}

BOOL CClientSocket::IsTimerOut()
{
	if(m_iPulsCounter>=3)
	{
		m_iPulsCounter = 0;
		return TRUE;
	}
	else
	{
		IncremePulsCounter();
		return FALSE;
	}
};

void* CClientSocket::cioRecvBytes(int& length, int& Error)
{
	try
	{
		int TickCount = 0;
		Error = 0;
		char* buf = new char[length];
		char* temp = buf;
		int recl = 0;
		do
		{
			const int len = recv(GetSocket(), temp, length-recl, 0);
			if(len==SOCKET_ERROR||len==0)
			{
				Error = 1;
				delete[] buf;
				return nullptr;
			}

			recl += len;
			temp += len;
			if(recl<length)
			{
				TickCount += 1;
				Sleep(50);
			}
		} while(recl<length&&TickCount<2000);

		length = recl;
		return buf;
	}
	catch(CException*ex)
	{
		ex->Delete();
		return nullptr;
	};
}

BOOL CClientSocket::cioSendBytes(void* msg, int msg_size)
{
	if(msg==nullptr)
		return FALSE;

	try
	{
		int TickCount = 0;
		char* tempbuf = (char*)msg;
		int sentl = 0;
		do
		{
			const int l = send(GetSocket(), tempbuf, msg_size-sentl, 0);
			if(l==SOCKET_ERROR)
				return FALSE;

			sentl += l;
			tempbuf += l;
			if(sentl<msg_size)
			{
				Sleep(50);
				TickCount += 1;
			};
		} while(sentl<msg_size&&TickCount<2000);

		if(sentl!=msg_size)
			return FALSE;

		return TRUE;
	}
	catch(CException*ex)
	{
		ex->Delete();
		return FALSE;
	};
}

BOOL CClientSocket::IsCorrectHeader(char* header, int size)
{
	if(size!=m_headLength)
		return FALSE;

	BYTE crc = *(BYTE*)header;
	for(int i = 1; i<m_headLength-2; i++)
		crc ^= *((BYTE*)(header+i));

	if(crc!=*(BYTE*)(header+m_headLength-2))
		return FALSE;
	else
		return TRUE;
}

BOOL CClientSocket::IsCorrectMessage(char* msg, int size)
{
	const int totalen = *((DWORD*)msg);
	if(totalen!=size)
		return FALSE;

	// body crc
	char* lp = msg;
	BYTE crc = *((BYTE*)(lp+m_headLength));
	for(int i = m_headLength+1; i<totalen; i++)
		crc ^= *((BYTE*)(lp+i));

	if(*(BYTE*)(lp+m_headLength-1)!=crc)
		return FALSE;
	else
		return TRUE;
}

int CClientSocket::GetMessageSizeFromHeader(char* header)
{
	return (*(DWORD*)(header));
}

BOOL CClientSocket::ConnectToServer(char* serverName, int _port)
{
	try
	{
		ResetConnection();
		if(IsConnectionValid()==TRUE)
			return TRUE;

		//create socket for server connection
		const SOCKET ServerSock = socket(AF_INET, SOCK_STREAM, 0);
		sockaddr_in sai;
		memset(&sai, 0, sizeof(sockaddr_in));
		sai.sin_port = htons(_port);
		sai.sin_family = AF_INET;
		sai.sin_addr.s_addr = inet_addr(serverName);
		if(sai.sin_addr.s_addr==INADDR_NONE)
		{
			hostent* Hostent = gethostbyname(serverName);
			if(Hostent==nullptr)
			{
				closesocket(ServerSock);
				return FALSE;
			}
			sai.sin_addr.s_addr = ((struct in_addr FAR*)(Hostent->h_addr))->s_addr;
		};
		const int res = connect(ServerSock, (sockaddr*)&sai, sizeof(sockaddr_in));
		if(res!=0)//error during connection
		{
			closesocket(ServerSock);
			return FALSE;
		};

		InitConnection(ServerSock);
		if(!CreateThreads())
			return FALSE;

		return TRUE;
	}
	catch(CException*ex)
	{
		ex->Delete();
		ASSERT(FALSE);
		return FALSE;
	};
}

bool CClientSocket::SendData(void* msg, DWORD length)
{
	if(length==0||length<m_headLength)
		return true;
	const int totalen = *((DWORD*)msg);
	if(totalen!=length)
		return true;

	// header crc
	char* lp = (char*)msg;
	BYTE crc = lp[0];
	for(int i = 1; i<m_headLength-2; i++)
		crc ^= BYTE(*(lp+i));
	lp[m_headLength-2] = crc;

	// body crc
	crc = BYTE(*(lp+m_headLength));
	for(int i = m_headLength+1; i<totalen; i++)
		crc ^= BYTE(*(lp+i));
	lp[m_headLength-1] = crc;

	return cioSendBytes(lp, length);
}

BOOL CClientSocket::SendMsg(CMessageReceived* msg)
{
	if(msg==nullptr)
		return FALSE;

	try
	{
		DWORD length;
		char* lp = (char*)PrepareSendMsg(msg, length);
		if(lp==nullptr||length<sizeof(STRUCT_MSG_HEADER))
			return FALSE;

		return SendMessage(lp, length);
	}
	catch(CException*ex)
	{
		ex->Delete();
		return FALSE;
	};
}

BOOL CClientSocket::SendMsg(char* lp, DWORD length)
{
	if(length<sizeof(STRUCT_MSG_HEADER))
	{
		delete lp;
		lp = nullptr;
		return FALSE;
	}

	STRUCT_MSG_HEADER hdr;
	memcpy((char*)&hdr, lp, sizeof(STRUCT_MSG_HEADER));
	if(hdr.wCmdId==INVALID_MESSAGE)
	{
		delete lp;
		lp = nullptr;
		return FALSE;
	}
	else
		return SendMessage(lp, length);
}

BOOL CClientSocket::SendMessage(void* msg, DWORD length)
{
	if(IsTerminated()==TRUE)
	{
		delete msg;
		msg = nullptr;
		return FALSE;
	}
	else
	{
		OutQueue.PushMessage(msg, length);
		return TRUE;
	}
}
