#pragma once

#ifndef HPP_GETTIMEOFDAY
#define HPP_GETTIMEOFDAY

#include <time.h>
#include <winsock2.h>
#include <windows.h>
namespace
{
	const __int64 DELTA_EPOCH_IN_MICROSECS = 11644473600000000Ui64;

	struct Timezone 
	{
		__int32  tz_minuteswest; /* minutes W of Greenwich */
		bool  tz_dsttime;     /* type of dst correction */
	};

	unsigned int gettimeofday(struct timeval *tv/*in*/, struct Timezone *tz/*in*/)
	{
		FILETIME ft;
		__int64 tmpres = 0;
		TIME_ZONE_INFORMATION tz_winapi;
		int rez=0;

		ZeroMemory(&ft,sizeof(ft));
		ZeroMemory(&tz_winapi,sizeof(tz_winapi));

		if(tv != nullptr)
		{
			GetSystemTimeAsFileTime(&ft);

			tmpres = ft.dwHighDateTime;
			tmpres <<= 32;
			tmpres |= ft.dwLowDateTime;

			/*converting file time to unix epoch*/
			tmpres /= 10;  /*convert into microseconds*/
			tmpres -= DELTA_EPOCH_IN_MICROSECS; 
			tv->tv_sec = (__int32)(tmpres/1000000);
			tv->tv_usec =(__int32)(tmpres%1000000);
		}

		//_tzset(),don't work properly, so we use GetTimeZoneInformation
		if(tz != nullptr)
		{
			rez=GetTimeZoneInformation(&tz_winapi);
			tz->tz_dsttime=(rez==2)?true:false;
			tz->tz_minuteswest = tz_winapi.Bias + ((rez==2)?tz_winapi.DaylightBias:0);
		}
		return 0;
	};

	unsigned long gettimestamp()
	{
		timeval time;
		gettimeofday(&time, NULL);
		unsigned long milseconds = (time.tv_sec*1000) + (time.tv_usec/1000);
		return milseconds;
	}
}
//#else
//	extern int gettimeofday(struct timeval *tv/*in*/, struct Timezone *tz/*in*/);
#endif