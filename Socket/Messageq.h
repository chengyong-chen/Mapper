#pragma once

#include <vector>
using namespace std;

class AFX_EXT_CLASS CMessageQueue
{
public:
	typedef struct SSingleMessage
	{
		void* message;
		DWORD length;
	} SSingleMessage;

public:
	CMessageQueue();
	~CMessageQueue();

	void PushMessage(void* pMsg, DWORD length);
	void* PopMessage(DWORD& length);
	void Clear();

	unsigned long GetMsgCount();

	bool IsQueueEmpty() const;
	DWORD WaitForMsg() const;

protected:
	vector<SSingleMessage> MsgQueue;

	CRITICAL_SECTION UsageFlag;
	HANDLE emptyFlag;
	HANDLE validFlag;
};
