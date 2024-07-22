// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently





#pragma once


#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#define _ATL_APARTMENT_THREADED

#include <afxwin.h>
#include <afxdisp.h>
#include <atlbase.h>
#include <comdef.h>

//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module

//class CExeModule : public CComModule		default module
class CExeModule : public CComAutoThreadModule<>	//needed by auto-thread
{
public:
	LONG Unlock();
	DWORD dwThreadID;
	HANDLE hEventShutdown;
	void MonitorShutdown();
	bool StartMonitor();
	bool bActivity;
};
extern CExeModule _Module;

#include <atlcom.h>





