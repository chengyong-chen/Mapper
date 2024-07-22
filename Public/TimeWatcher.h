#pragma once

class __declspec(dllexport) CTimeWatcher
{
public:
	CTimeWatcher();
private:
	LARGE_INTEGER m_start;
	LARGE_INTEGER m_stop;
	LARGE_INTEGER frequency;

public:
	void Start();
	void Stop();
	double GetElapsed();

private:
	double LIToSecs(LARGE_INTEGER& L) const;
};
