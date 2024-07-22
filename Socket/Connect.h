#pragma once

#include "messageq.h"

typedef void (ProRecvCallback)(long param1, long param2, long param3);
typedef void (ListenCallback)(long param);

class AFX_EXT_CLASS CConnect
{
public:
	CConnect();
	~CConnect();

	//Public data
	enum
	{
		DEFAULT_FRAME = 0x76453201
	};//DWORD
	enum
	{
		DEFAULT_HEADER_SIZE = 8
	};//DWORD
	enum
	{
		DEFAULT_TIMEOUT = 3600,
		NO_TIMEOUT = 0xFFFFFFFF
	};//DWORD - timeout in seconds

public:
	int m_headLength;
	long m_nActiveThread;

public:
	virtual BOOL CreateThreads();
	virtual void AddThread() { ::InterlockedIncrement(&m_nActiveThread); };
	virtual void RemoveThread() { ::InterlockedDecrement(&m_nActiveThread); };

	//send/receive message attributes
	virtual void* cioRecvBytes(int& length, int& Error);
	virtual BOOL cioSendBytes(void* msg, int msg_size);
	virtual void* ReceiveData(int& length, int& Error);
	virtual bool SendData(void* msg, DWORD length);

	virtual BOOL IsCorrectHeader(char* header, int size = 8);
	virtual BOOL IsCorrectMessage(char* msg, int size);
	virtual int GetMessageSizeFromHeader(char* header);

	//message process routines
	virtual void ProcessInMessage();

	//connection validity
	virtual BOOL IsConnectionValid();
	virtual void ResetConnection();

public:
	CMessageQueue InQueue;
	CMessageQueue OutQueue;

protected:
	HANDLE m_hTerminate;
public:
	virtual void Close();
	virtual BOOL IsTerminated();
	virtual void WaitForTermination();

public:
	void* proRecvCallback;
	DWORD m_dwParent;

	void SetProcessFunction(void* newFunc);
	void ClearProcessCallbackFunction();
	void SetParent(DWORD dwParent) { m_dwParent = dwParent; }
};