#pragma once

#ifndef HPP_USLEEP
#define HPP_USLEEP

#include <winsock2.h>
#include <windows.h>
namespace
{
	void usleep(__int64 usec) 
	{ 
		HANDLE timer; 
		LARGE_INTEGER ft; 

		ft.QuadPart = -(10*usec); // Convert to 100 nanosecond interval, negative value indicates relative time

		timer = CreateWaitableTimer(NULL, TRUE, NULL); 
		SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0); 
		WaitForSingleObject(timer, INFINITE); 
		CloseHandle(timer); 
	};
	//#else
	//	extern void usleep(__int64 usec);
}
#endif