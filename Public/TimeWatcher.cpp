#include "StdAfx.h"

#include "TimeWatcher.h"

double CTimeWatcher::LIToSecs(LARGE_INTEGER& L) const
{
	return ((double)L.QuadPart/(double)frequency.QuadPart);
}

CTimeWatcher::CTimeWatcher()
{
	m_start.QuadPart = 0;
	m_stop.QuadPart = 0;
	QueryPerformanceFrequency(&frequency);
}

void CTimeWatcher::Start()
{
	QueryPerformanceCounter(&m_start);
}

void CTimeWatcher::Stop()
{
	QueryPerformanceCounter(&m_stop);
}

double CTimeWatcher::GetElapsed()
{
	LARGE_INTEGER time;
	time.QuadPart = m_stop.QuadPart-m_start.QuadPart;
	return LIToSecs(time);
}
