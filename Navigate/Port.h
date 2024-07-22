#pragma once

class CIVU;

#include "Vehicle.h"
class CVehicleInfoDlg;

#include "Message.h"
#include "../Public/Global.h"
#include "../Coding/Coder.h"

class __declspec(dllexport) CPort : public CObject
{
	DECLARE_DYNAMIC(CPort);
	public:
	CPort(CFrameWnd* pParents); // protected constructor used by dynamic creation

	public:
	CDatabase m_database;

	public:
	SMessage m_message;

	virtual bool Open();
	virtual void Close();

	virtual void ReceiveMsg();

	virtual bool SendMessage(const DWORD& dwVehicle, CStringA& string) { return FALSE; };
	virtual bool SendControl(const DWORD& dwVehicle, WORD wControl) { return FALSE; };
	virtual bool SendCommand(const DWORD& dwVehicle, WORD wCommand, CString strIVU, CCoder::SParameter& parameter) { return FALSE; };

	public:
	CWinThread* m_hThread;
	bool m_bThreadAlive;
	bool RestartMonitor() const;
	bool StopMonitor() const;

	public:
	CWnd* m_pParents;
	DWORD m_dwId;

	static DWORD m_dwUser;
	protected:
	CVehicleInfoDlg* m_pVehicleInfoDlg;

	public:
	virtual BOOL Dispatch(CPointF geoPoint, CSizeF geoSize) { return FALSE; };

	virtual bool GetUserInfo(const DWORD& dwId, Content& content);

	public:
	static bool ParseRMC(CString sentence, SMessage& message);

	static bool ParseGGA(CString sentence, SMessage& message);
};
