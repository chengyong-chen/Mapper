// CGPSClientSocket.cpp: implementation of the CClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GPSClientSocket.h"
#include "../Coding/Instruction.h"
#include "../Coding/message.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGPSClientSocket::CGPSClientSocket() :
	CClientSocket()
{
	m_dwUser = 0;
}

CGPSClientSocket::~CGPSClientSocket()
{
}

BOOL CGPSClientSocket::SendStringToClient(DWORD vhcId, TCHAR* str)
{
	STRUCT_GPSSERVER_VEHICLE_MESSAGE* lp = new STRUCT_GPSSERVER_VEHICLE_MESSAGE(vhcId, str);
	return CClientSocket::SendMsg((CHAR*)lp, sizeof(*lp));
}

void* CGPSClientSocket::PrepareSendMsg(CMessageReceived* pMsg, DWORD& length)
{
	switch(pMsg->m_wCommand)
	{
	case GPSSERVER_VEHICLE_MOVE:
	{
		STRUCT_GPSSERVER_VEHICLE_MOVE* lp = new STRUCT_GPSSERVER_VEHICLE_MOVE(
			pMsg->m_dwVehicle, pMsg->m_Lat, pMsg->m_Lon,
			pMsg->m_Height, pMsg->m_Veo,
			pMsg->m_Direct, pMsg->m_varGPSTime,
			pMsg->m_varRevcTime, pMsg->m_dwStatus, pMsg->m_bAvailable);
		length = sizeof(*lp);
		return lp;
	}
	break;
	default:
		length = 0;
		break;
	}
	return nullptr;
}
