#include "stdafx.h"
#include "Connect.h"
#include <process.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//****************default threads implementation*************************

typedef void (RecvCallback)(void* msg, CConnect* pSocket);

void RevcThreadFunc(void* param)
{
	CConnect* pFather = (CConnect*)param;
	if(pFather==nullptr)
		return;

	pFather->AddThread();

	while(pFather->IsTerminated()==FALSE)
	{
		try
		{
			int length, error;
			void* msg = pFather->ReceiveData(length, error);
			if(msg!=nullptr)
			{
				pFather->InQueue.PushMessage(msg, length);
			}
		}
		catch(CException*ex)
		{
			ex->Delete();
		};
	};

	pFather->RemoveThread();
}

void SyncThreadFunc(void* param)
{
	CConnect* pFather = (CConnect*)param;
	if(pFather==nullptr)
		return;

	pFather->AddThread();

	while(pFather->IsTerminated()==FALSE)
	{
		if(pFather->InQueue.WaitForMsg()!=WAIT_OBJECT_0)
			continue;

		try
		{
			pFather->ProcessInMessage();
		}
		catch(CException*ex)
		{
			ex->Delete();
		};
	};

	pFather->RemoveThread();
}

void SendThreadFunc(void* param)
{
	CConnect* pFather = (CConnect*)param;
	if(pFather==nullptr)
		return;

	pFather->AddThread();

	while(pFather->IsTerminated()==FALSE)
	{
		if(pFather->OutQueue.WaitForMsg()!=WAIT_OBJECT_0)
			continue;

		DWORD length;
		void* msg = pFather->OutQueue.PopMessage(length);
		if(msg!=nullptr)
		{
			try
			{
				pFather->SendData(msg, length);
			}
			catch(CException*ex)
			{
				ex->Delete();
			};

			delete msg;
		}
	};

	pFather->RemoveThread();
}

//*************************CConnect*************************
CConnect::CConnect()
{
	m_headLength = 8;
	proRecvCallback = nullptr;
	m_dwParent = 0;

	m_nActiveThread = 0;

	SECURITY_ATTRIBUTES sat;
	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.lpSecurityDescriptor = nullptr;
	sat.bInheritHandle = TRUE;

	m_hTerminate = CreateEvent(&sat, TRUE, FALSE, nullptr);

	ResetConnection();
}

CConnect::~CConnect()
{
	if(m_hTerminate!=nullptr)
	{
		CloseHandle(m_hTerminate);
		m_hTerminate = nullptr;
	}
}

void CConnect::SetProcessFunction(void* callbackFunc)
{
	proRecvCallback = callbackFunc;
}

void CConnect::ClearProcessCallbackFunction()
{
	proRecvCallback = 0;
}

BOOL CConnect::CreateThreads()
{
	m_nActiveThread = 0;

	::_beginthread(RevcThreadFunc, 0, (void*)this);
	::_beginthread(SendThreadFunc, 0, (void*)this);
	::_beginthread(SyncThreadFunc, 0, (void*)this);

	while(m_nActiveThread<3)
	{
		Sleep(50);
	};

	if(m_nActiveThread<3)
		return FALSE;
	else
		return TRUE;
}

void CConnect::WaitForTermination()
{
	int Count = 0;
	while(m_nActiveThread>0)
	{
		Sleep(1);
		Count += 1;
	};
}

BOOL CConnect::IsTerminated()
{
	return (WaitForSingleObject(m_hTerminate, 1)==WAIT_OBJECT_0);
}

void CConnect::Close()
{
	SetEvent(m_hTerminate);

	while(m_nActiveThread>0)
	{
		Sleep(100);
	}
}

void* CConnect::ReceiveData(int& length, int& Error)
{
	try
	{
		Error = 0;
		int len = m_headLength;
		void* hdr = cioRecvBytes(len, Error);
		if(Error!=0||len!=m_headLength)
		{
			delete[] hdr;
			return nullptr;
		}
		if(!IsCorrectHeader((char*)hdr, len))
		{
			delete[] hdr;
			return nullptr;
		}
		const int size = GetMessageSizeFromHeader((char*)hdr);
		len = size-m_headLength;
		if(len>0)
		{
			const void* bdy = cioRecvBytes(len, Error);
			if(Error!=0||len!=size-m_headLength)
			{
				delete[] bdy;
				return nullptr;
			}

			length = size;
			char* buf = new char[length];
			memcpy(buf, hdr, m_headLength);
			memcpy(buf+m_headLength, bdy, len);

			delete[] hdr;
			delete[] bdy;
			if(!IsCorrectMessage((char*)buf, length))
			{
				delete[] buf;
				return nullptr;
			}
			return buf;
		}
		else
		{
			return hdr;
		}
	}
	catch(CException*ex)
	{
		ex->Delete();
		ASSERT(FALSE);
		return nullptr;
	};
}

void* CConnect::cioRecvBytes(int& length, int& Error)
{
	Error = 1;
	return nullptr;
}

BOOL CConnect::cioSendBytes(void* msg, int msg_size)
{
	return FALSE;
}

BOOL CConnect::IsCorrectHeader(char* header, int size/*=8*/)
{
	if(((DWORD*)header)[0]==CConnect::DEFAULT_FRAME)
		return TRUE;
	else
		return FALSE;
}

BOOL CConnect::IsCorrectMessage(char* msg, int size)
{
	if(IsCorrectHeader(msg, size)==FALSE)
		return FALSE;
	else
		return TRUE;
}

int CConnect::GetMessageSizeFromHeader(char* header)
{
	return ((DWORD*)header)[1];
}

bool CConnect::SendData(void* msg, DWORD length)
{
	if(length!=0)
	{
		const DWORD len = length+CConnect::DEFAULT_HEADER_SIZE;
		const LPVOID lp = (void*) new char[len];
		memcpy((char*)lp+CConnect::DEFAULT_HEADER_SIZE, msg, length);
		DWORD* dw = (DWORD*)lp;
		dw[0] = CConnect::DEFAULT_FRAME;
		dw[1] = length;

		BOOL re = TRUE;
		try
		{
			re = cioSendBytes(lp, len);
		}
		catch(CException*ex)
		{
			ex->Delete();
		}

		delete[] lp;
		return re;
	}
	else
		return TRUE;
}

void CConnect::ProcessInMessage()
{
	DWORD length;
	char* lp = (char*)InQueue.PopMessage(length);
	if(lp!=nullptr)
	{
		if(proRecvCallback!=nullptr)
		{
			((ProRecvCallback*)proRecvCallback)((long)lp, (long)this, m_dwParent);
		}

		delete[] lp;
		lp = nullptr;
	}
}

BOOL CConnect::IsConnectionValid()
{
	return (m_nActiveThread==3);
}

void CConnect::ResetConnection()
{
	ResetEvent(m_hTerminate);

	proRecvCallback = nullptr;
}
