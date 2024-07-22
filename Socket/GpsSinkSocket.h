#pragma once

#include "ClientSocket.h"
#include "../Coding/message.h"
#include <map>

class CGpsSinkSocket;

struct VehicleStatus
{
	// 基本信息
	SVehicle* m_pVhcInfo;

	// 运行时信息
	BOOL m_bTrace; // 是否跟踪
	BOOL m_bAlarm; // 是否报警
	DWORD m_dwStatus; // 当前状态
	COleDateTime m_timeLast; // 最后一次回报时间
	CGpsSinkSocket* m_pSocket; // 当时使用socket

	VehicleStatus(SVehicle* pVhc, CGpsSinkSocket* pSocket) :
		m_pSocket(pSocket),
		m_pVhcInfo(pVhc),
		m_bTrace(FALSE),
		m_bAlarm(FALSE),
		m_timeLast((double)0.0)
	{
	}
};

typedef std::map<DWORD, VehicleStatus*> CVehicleMap;

class AFX_EXT_CLASS CGpsSinkSocket : public CClientSocket
{
public:
	CGpsSinkSocket();

	~CGpsSinkSocket() override;

public:
	virtual BOOL SendStringToClient(DWORD vhcId, TCHAR* str);

	void* PrepareSendMsg(CMessageReceived* msg, DWORD& length) override;
	BOOL AskInspectOneVehicle(SVehicle* pVhc, ADOCG::_ConnectionPtr dbconnect = nullptr);
	BOOL SetMMTimer(UINT nInterval, UINT nResolution);
	void KillMMTimer();

public:
	void SetUserID(DWORD id) { m_dwUser = id; };
	DWORD GetUserID() const
	{
		return m_dwUser;
	};
	void SetQueueSize(UINT size) { m_uQueueSize = size; };
	UINT GetQueueSize() const
	{
		return m_uQueueSize;
	};
	VehicleStatus* GetVhcStatus(DWORD dwID);
	void AddVhcStatus(DWORD dwID, VehicleStatus* pState) { m_mapIDVehicle[dwID] = pState; };
	ADOCG::_RecordsetPtr GetReplaySet() const
	{
		return m_pRecordset;
	};
	void SetReplaySet(ADOCG::_ConnectionPtr& pDBConnect, DWORD vhcId, CString strTime1, CString strTime2, UINT uDelay = 200);
	void ClearReplaySet();
	DWORD GetReplayingVhcID() const
	{
		return m_dwReplayID;
	};

public:
	DWORD m_dwUser;
	UINT m_uQueueSize;
	int m_nTimerID; // MMTimer ID
	int m_nTimerRes; // MMTimer resource
	DWORD m_dwReplayID; // replaying vhc id
	ADOCG::_RecordsetPtr m_pRecordset; // vhc's replay resultset

	CVehicleMap m_mapIDVehicle;
};
