#include "stdafx.h"
#include "messageq.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CMessageQueue class implementation ********************************

CMessageQueue::CMessageQueue()
{
	InitializeCriticalSection(&UsageFlag);

	//create event indicator event
	SECURITY_ATTRIBUTES sat;
	sat.nLength = sizeof(SECURITY_ATTRIBUTES);
	sat.lpSecurityDescriptor = nullptr;
	sat.bInheritHandle = TRUE;
	emptyFlag = CreateEvent(&sat, TRUE, FALSE, nullptr);
	validFlag = CreateEvent(&sat, TRUE, FALSE, nullptr);
	SetEvent(emptyFlag);
}

CMessageQueue::~CMessageQueue()
{
	EnterCriticalSection(&UsageFlag);

	for(unsigned long i = 0; i<GetMsgCount(); i++)
	{
		const SSingleMessage ssm = MsgQueue.at(i);
		const LPVOID lp = ssm.message;
		delete[] lp;
	};
	MsgQueue.clear();
	SetEvent(emptyFlag);
	ResetEvent(validFlag);

	LeaveCriticalSection(&UsageFlag);

	CloseHandle(emptyFlag);
	CloseHandle(validFlag);

	DeleteCriticalSection(&UsageFlag);
}

void CMessageQueue::PushMessage(void* Msg, DWORD length)
{
	EnterCriticalSection(&UsageFlag);

	SSingleMessage ssm;
	ssm.message = Msg;
	ssm.length = length;
	MsgQueue.push_back(ssm);

	ResetEvent(emptyFlag);
	SetEvent(validFlag);

	LeaveCriticalSection(&UsageFlag);
}

unsigned long CMessageQueue::GetMsgCount()
{
	EnterCriticalSection(&UsageFlag);
	const int l = MsgQueue.size();
	LeaveCriticalSection(&UsageFlag);
	return l;
}

bool CMessageQueue::IsQueueEmpty() const
{
	if(WaitForSingleObject(emptyFlag, 0)==WAIT_OBJECT_0)
		return true;
	else
		return false;
}

DWORD CMessageQueue::WaitForMsg() const
{
	return WaitForSingleObject(validFlag, 5000);
}

void* CMessageQueue::PopMessage(DWORD& length)
{
	if(GetMsgCount()>0)
	{
		EnterCriticalSection(&UsageFlag);
		const SSingleMessage ssm = MsgQueue.at(0);
		const LPVOID lp = ssm.message;
		length = ssm.length;

		MsgQueue.erase(MsgQueue.begin());

		if(MsgQueue.size()==0)
		{
			SetEvent(emptyFlag);
			ResetEvent(validFlag);
		}
		else
		{
			ResetEvent(emptyFlag);
			SetEvent(validFlag);
		}

		LeaveCriticalSection(&UsageFlag);
		return lp;
	}
	else
		return nullptr;
}

void CMessageQueue::Clear()
{
	EnterCriticalSection(&UsageFlag);

	for(unsigned long i = 0; i<GetMsgCount(); i++)
	{
		const SSingleMessage ssm = MsgQueue.at(i);
		const LPVOID lp = ssm.message;
		delete[] lp;
	};
	MsgQueue.clear();
	SetEvent(emptyFlag);
	ResetEvent(validFlag);

	LeaveCriticalSection(&UsageFlag);
}
