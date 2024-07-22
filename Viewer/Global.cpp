#include "stdafx.h"

#include <Wininet.h>
#include "Global.h"

#include <stdio.h>
#include <Sensapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool IsInternetConnection()
{
	DWORD dwType;
	if(InternetGetConnectedState(&dwType, 0)==TRUE)
		return true;
	else
		return false;

	if(InternetCheckConnection(_T("http://www.SinoMaps.com"), FLAG_ICC_FORCE_CONNECTION, 0)==TRUE)
		return true;

	QOCINFO strucQOCINFO;
	strucQOCINFO.dwSize = sizeof(QOCINFO);
	if(IsDestinationReachable(_T("http://www.SinoMaps.com"), &strucQOCINFO)==TRUE)
		return true;

	//	if(InternetOpenURL(_T("http://www.SinoMaps.com")) != nullptr) 
	//		return true;

	return false;
}
