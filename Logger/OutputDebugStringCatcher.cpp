#include "Stdafx.h"
#include "OutputDebugStringCatcher.h"
#include <stdio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

COutputDebugStringCatcher::COutputDebugStringCatcher()
{
	m_hDBWinMutex = nullptr;
	m_hEventBufferReady = nullptr;
	m_hEventDataReady = nullptr;
	m_hDBMonBuffer = nullptr;
	m_hLogCatcherThread = nullptr;
	m_hFileMapView = nullptr;
}

COutputDebugStringCatcher::~COutputDebugStringCatcher()
{
}

DWORD COutputDebugStringCatcher::Initialize()
{
	SetLastError(0);
	m_hDBWinMutex = ::OpenMutex(MUTEX_ALL_ACCESS, FALSE, L"DBWinMutex");
	if(m_hDBWinMutex == nullptr)
	{
		return GetLastError();
	}
	m_hEventBufferReady = ::OpenEvent(EVENT_ALL_ACCESS, FALSE, L"DBWIN_BUFFER_READY");
	if(m_hEventBufferReady == nullptr)
	{
		m_hEventBufferReady = ::CreateEvent(nullptr, FALSE, TRUE, L"DBWIN_BUFFER_READY");
		if(m_hEventBufferReady == nullptr)
		{
			return GetLastError();
		}
	}
	m_hEventDataReady = ::OpenEvent(SYNCHRONIZE, FALSE, L"DBWIN_DATA_READY");
	if(m_hEventDataReady == nullptr)
	{
		m_hEventDataReady = ::CreateEvent(nullptr, FALSE, FALSE, L"DBWIN_DATA_READY");
		if(m_hEventDataReady == nullptr)
		{
			return GetLastError();
		}
	}
	m_hDBMonBuffer = ::OpenFileMapping(FILE_MAP_READ, FALSE, L"DBWIN_BUFFER");
	if(m_hDBMonBuffer == nullptr)
	{
		m_hDBMonBuffer = ::CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, sizeof(struct dbwin_buffer), L"DBWIN_BUFFER");
		if(m_hDBMonBuffer == nullptr)
		{
			return GetLastError();
		}
	}
	m_hFileMapView = (struct dbwin_buffer*)::MapViewOfFile(m_hDBMonBuffer, SECTION_MAP_READ, 0, 0, 0);
	if(m_hFileMapView == nullptr)
	{
		return GetLastError();
	}
	m_bWinDebugMonStopped = FALSE;
	m_hLogCatcherThread = ::CreateThread(nullptr, 0, CatcherThread, this, 0, nullptr);
	if(m_hLogCatcherThread == nullptr)
	{
		m_bWinDebugMonStopped = TRUE;
		return GetLastError();
	}

	//BOOL bSuccessful1 = ::SetPriorityClass(::GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
	//BOOL bSuccessful2 = ::SetThreadPriority(m_hLogCatcherThread,THREAD_PRIORITY_TIME_CRITICAL);
	return 0;
}

void COutputDebugStringCatcher::Unintialize()
{
	m_bWinDebugMonStopped = TRUE;
	
	if(m_hLogCatcherThread != nullptr)
	{		
		::WaitForSingleObject(m_hLogCatcherThread, INFINITE);
		CloseHandle(m_hLogCatcherThread);
		m_hLogCatcherThread = nullptr;
	}
	if(m_hFileMapView != nullptr)
	{
		::UnmapViewOfFile(m_hFileMapView);
		m_hFileMapView = nullptr;
	}
	if(m_hDBWinMutex != nullptr)
	{
		CloseHandle(m_hDBWinMutex);
		m_hDBWinMutex = nullptr;
	}
	if(m_hDBMonBuffer != nullptr)
	{
		CloseHandle(m_hDBMonBuffer);
		m_hDBMonBuffer = nullptr;
	}
	if(m_hEventBufferReady != nullptr)
	{
		CloseHandle(m_hEventBufferReady);
		m_hEventBufferReady = nullptr;
	}
	if(m_hEventDataReady != nullptr)
	{
		CloseHandle(m_hEventDataReady);
		m_hEventDataReady = nullptr;
	}
}

void COutputDebugStringCatcher::ProcessMessage()
{
	if(::WaitForSingleObject(m_hEventDataReady, 100) == WAIT_OBJECT_0) // wait for data ready
	{
		OutputString(m_hFileMapView->data);
		SetEvent(m_hEventBufferReady);// signal buffer ready
	}
}

DWORD WINAPI COutputDebugStringCatcher::CatcherThread(void* pData)
{
	COutputDebugStringCatcher* parent = (COutputDebugStringCatcher*)pData;
	if(parent == nullptr)
		return 0;

	while(parent->m_bWinDebugMonStopped == FALSE)
	{
		parent->ProcessMessage();
	}
	return 0;
}
