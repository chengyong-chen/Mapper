#pragma once

class __declspec(dllexport) COutputDebugStringCatcher
{
private:
	struct dbwin_buffer
	{
		DWORD dwProcessId;
		char data[4096-sizeof(DWORD)];
	};

public:
	COutputDebugStringCatcher();
	virtual ~COutputDebugStringCatcher();

private:
	HANDLE m_hDBWinMutex;
	HANDLE m_hDBMonBuffer;
	HANDLE m_hEventBufferReady;
	HANDLE m_hEventDataReady;

	HANDLE m_hLogCatcherThread;
	BOOL m_bWinDebugMonStopped;
	struct dbwin_buffer* m_hFileMapView;

public:
	DWORD Initialize();
	void Unintialize();
	void ProcessMessage();

public:
	virtual void OutputString(const char* str)
	{
	};

public:
	static DWORD WINAPI CatcherThread(void* pData);
};
