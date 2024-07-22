#pragma once

class CPort;
class CWinSock;

#include "Vehicle.h"
#include "../Socket/GPSClientSocket.h"
#include "../Coding/Coder.h"

class CPort;

class __declspec(dllexport) CPortSocket : public CPort
{
	DECLARE_DYNAMIC (CPortSocket);
public:
	CPortSocket (CFrameWnd* pParents); // protected constructor used by dynamic creation

public:
	static CGPSClientSocket m_socket;

public:
	bool Open () override;
	void Close () override;
	bool SendMessage (const DWORD& dwVehicle, CStringA& string) override;
	bool SendControl (const DWORD& dwVehicle, WORD wControl) override;
	virtual bool SendCommand (const DWORD& dwVehicle, WORD wCommand, CStringA strIVU, CCoder::SParameter& parameter);

public:
	static void CallbackRecvFromSocket (long param);
	static void CallbackErrorFromSocket (long param);

public:
	BOOL Dispatch (CPointF geoPoint, CSizeF geoSize) override;
};
