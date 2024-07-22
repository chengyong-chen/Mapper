// CClientSocket.cpp: implementation of the CClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GpsSinkSocket.h"
#include "ExStdCFunction.h"

#include "../Coding/Instruction.h"
#include "../Coding/Coder.h"

#include <Mmsystem.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGpsSinkSocket::CGpsSinkSocket() :
	CClientSocket(),
	m_uQueueSize(0),
	m_pRecordset(nullptr),
	m_dwReplayID(0)
{
	m_dwUser = 0;
}

CGpsSinkSocket::~CGpsSinkSocket()
{
	for(CVehicleMap::iterator it = m_mapIDVehicle.begin(); it!=m_mapIDVehicle.end(); it++)
	{
		const VehicleStatus* pVhcState = (VehicleStatus*)it->second;
		delete pVhcState;
	}
	m_mapIDVehicle.clear();
}

BOOL CGpsSinkSocket::SendStringToClient(DWORD vhcId, TCHAR* str)
{
	STRUCT_GPSSERVER_VEHICLE_MESSAGE* lp = new STRUCT_GPSSERVER_VEHICLE_MESSAGE(vhcId, str);
	return CClientSocket::SendMsg((char*)lp, sizeof(*lp));
}

void* CGpsSinkSocket::PrepareSendMsg(CMessageReceived* pMsg, DWORD& length)
{
	if(pMsg->m_dwVehicle==m_dwReplayID)
		return nullptr;

	switch(pMsg->m_wCommand)
	{
	case GPSSERVER_VEHICLE_MOVE:
	{
		STRUCT_GPSSERVER_VEHICLE_MOVE* lp = new STRUCT_GPSSERVER_VEHICLE_MOVE(pMsg->m_dwVehicle, pMsg->m_Lat, pMsg->m_Lon, pMsg->m_Height, pMsg->m_Veo, pMsg->m_Direct, pMsg->m_varGPSTime, pMsg->m_varRevcTime, pMsg->m_dwStatus, pMsg->m_bAvailable);
		length = sizeof(*lp);
		return lp;
	}
	break;
	case GPSSERVER_VEHICLE_MESSAGE:
	{
		SendStringToClient(pMsg->m_dwVehicle, (LPTSTR)(LPCTSTR)pMsg->m_strMessage);
		return nullptr;
	}
	break;
	case GPSSERVER_VEHICLE_DISPATCH:
	{
	}
	break;
	default:
		length = 0;
		break;
	}
	return nullptr;
}

BOOL CGpsSinkSocket::AskInspectOneVehicle(SVehicle* pVhc, ADOCG::_ConnectionPtr dbconnect)
{
	try
	{
		if(pVhc==nullptr)
			return FALSE;

		STRUCT_GPSSERVER_VEHICLE_NEW* lp = new STRUCT_GPSSERVER_VEHICLE_NEW(
			pVhc->m_dwId,
			pVhc->m_strCode.GetBuffer(),
			pVhc->m_strColor.GetBuffer(),
			pVhc->m_strIVUSIM.GetBuffer(),
			pVhc->m_strType.GetBuffer(),
			pVhc->m_strIVUPwd.GetBuffer(),
			pVhc->m_strIVUType.GetBuffer(),
			pVhc->m_strOwnerMobile.GetBuffer(),
			pVhc->m_strDriver.GetBuffer());
		if(CClientSocket::SendMsg((char*)lp, sizeof(*lp))==TRUE)
		{
			if(dbconnect!=nullptr) // 发送最后一次定位数据
			{
				CString strSQL;
				strSQL.Format(_T("SELECT * FROM VIEW_VEHICLE_LAST_POSITION WHERE ID=%d"), pVhc->m_dwId);

				try
				{
					_variant_t varRowsAff;
					ADOCG::_RecordsetPtr rs = dbconnect->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);

					if(!rs->EndofFile)
					{
						_variant_t var;
						static double lng, lat, height;
						static float veo, direct;
						static double gpsTime, localTime;
						static DWORD state;
						static BOOL bAV;

						rs->MoveFirst();
						var = rs->Fields->Item[_variant_t("经度")]->Value;
						if(var.vt!=VT_NULL)
						{
							lng = var.dblVal;
						}
						var = rs->Fields->Item[_variant_t("纬度")]->Value;
						if(var.vt!=VT_NULL)
						{
							lat = var.dblVal;
						}
						var = rs->Fields->Item[_variant_t("速度")]->Value;
						if(var.vt!=VT_NULL)
						{
							veo = var.dblVal;
						}
						var = rs->Fields->Item[_variant_t("方向")]->Value;
						if(var.vt!=VT_NULL)
						{
							direct = var.iVal;
						}
						state = 0;
						var = rs->Fields->Item[_variant_t("state")]->Value;
						if(var.vt!=VT_NULL)
						{
							state = var.lVal;
						}
						var = rs->Fields->Item[_variant_t("时间")]->Value;
						if(var.vt!=VT_NULL)
						{
							gpsTime = var.date;
							localTime = var.date;
						}
						bAV = TRUE;

						STRUCT_GPSSERVER_VEHICLE_MOVE* lp = new STRUCT_GPSSERVER_VEHICLE_MOVE(pVhc->m_dwId, lat, lng, height, veo, direct, gpsTime, localTime, state, bAV);
						CClientSocket::SendMsg((char*)lp, sizeof(*lp));
					}
					rs->Close();
				}
				catch(_com_error&)
				{
					_bstr_t bstr;
					_variant_t var(short(0));

					if(dbconnect->Errors->Count>0)
					{
						bstr = dbconnect->Errors->Item[var]->Description;
						Log_or_AfxMessageBox(bstr);
					}
				}
			}
			return TRUE;
		}
	}
	catch(CException*ex)
	{
	}
	return FALSE;
}

VehicleStatus* CGpsSinkSocket::GetVhcStatus(DWORD dwID)
{
	const CVehicleMap::iterator it = m_mapIDVehicle.find(dwID);
	if(it==m_mapIDVehicle.end())
		return nullptr;
	else
		return (VehicleStatus*)it->second;
};

void PASCAL MMTimerProc2(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2);

BOOL CGpsSinkSocket::SetMMTimer(UINT nInterval, UINT nResolution)
{
	TIMECAPS tc;
	const UINT m_nTimerRes = nResolution;
	if(timeGetDevCaps(&tc, sizeof(TIMECAPS))==TIMERR_NOERROR)
	{
		if(m_nTimerRes!=min(max(tc.wPeriodMin, m_nTimerRes), tc.wPeriodMax))
			return FALSE;
	}

	if(m_nTimerID)
		KillMMTimer();

	if(timeBeginPeriod(m_nTimerRes)==TIMERR_NOERROR)
	{
		m_nTimerID = timeSetEvent(nInterval, m_nTimerRes, MMTimerProc2, (DWORD)this, TIME_PERIODIC);
		if(!m_nTimerID)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

void CGpsSinkSocket::SetReplaySet(ADOCG::_ConnectionPtr& pDBConnect, DWORD vhcId, CString strTime1, CString strTime2, UINT uDelay)
{
	if(pDBConnect==nullptr)
	{
		m_dwReplayID = 0;
		m_pRecordset = nullptr;
		KillMMTimer();
		return;
	}

	CString strSQL;
	strSQL.Format(_T("SELECT * FROM RecvPos WHERE Vehicle=%d AND 接收时间>=\'%s\' AND 接收时间<\'%s\' ORDER BY 接收时间"), vhcId, strTime1, strTime2);

	try
	{
		_variant_t varRowsAff;
		ADOCG::_RecordsetPtr rs = pDBConnect->Execute(LPCTSTR(strSQL), &varRowsAff, ADOCG::adCmdText);

		if(!rs->EndofFile)
		{
			m_pRecordset = rs;
			m_dwReplayID = vhcId;
			if(uDelay<100)
				uDelay = 100;
			if(uDelay>2000)
				uDelay = 2000;
			SetMMTimer(uDelay, 111);
		}
	}
	catch(_com_error&)
	{
		_bstr_t bstr;
		_variant_t var(short(0));

		if(pDBConnect->Errors->Count>0)
		{
			bstr = pDBConnect->Errors->Item[var]->Description;
			Log_or_AfxMessageBox(bstr);
		}
	}
}

void CGpsSinkSocket::ClearReplaySet()
{
	if(m_pRecordset!=nullptr)
	{
		//		m_pRecordset->Close();
		delete m_pRecordset;
		m_pRecordset = nullptr;

		m_dwReplayID = 0;
		m_pRecordset = nullptr;
		KillMMTimer();
	}
}

void CGpsSinkSocket::KillMMTimer()
{
	if(m_nTimerID)
	{
		timeKillEvent(m_nTimerID);
		m_nTimerID = 0;
	}
	timeEndPeriod(m_nTimerRes);
}

void PASCAL MMTimerProc2(UINT wTimerID, UINT msg, DWORD dwUser, DWORD dw1, DWORD dw2)
{
	CGpsSinkSocket* pClientSocket = (CGpsSinkSocket*)dwUser;
	ASSERT(pClientSocket!=nullptr);

	_variant_t var;
	static double lng, lat, height;
	static float veo, direct;
	static double gpsTime, localTime;
	static DWORD state;
	static BOOL bAV;
	try
	{
		if(pClientSocket->GetReplaySet()==nullptr)
		{
			pClientSocket->ClearReplaySet();
			return;
		}

		if((pClientSocket->GetReplaySet())->EndofFile)
		{
			pClientSocket->ClearReplaySet();
			return;
		}

		(pClientSocket->GetReplaySet())->MoveNext();
		var = (pClientSocket->GetReplaySet())->Fields->Item["经度"]->Value;
		if(var.vt!=VT_NULL)
		{
			lng = var.dblVal;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item["纬度"]->Value;
		if(var.vt!=VT_NULL)
		{
			lat = var.dblVal;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("高度")]->Value;
		if(var.vt!=VT_NULL)
		{
			height = var.dblVal;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("速度")]->Value;
		if(var.vt!=VT_NULL)
		{
			veo = var.dblVal;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("方向")]->Value;
		if(var.vt!=VT_NULL)
		{
			direct = var.iVal;
		}
		state = 0;
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("状态")]->Value;
		if(var.vt!=VT_NULL)
		{
			state = var.lVal;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("GPS时间")]->Value;
		if(var.vt!=VT_NULL)
		{
			gpsTime = var.date;
		}
		var = (pClientSocket->GetReplaySet())->Fields->Item[_T("接收时间")]->Value;
		if(var.vt!=VT_NULL)
		{
			localTime = var.date;
		}
		bAV = ((state&0X00000200)==0);
		if((pClientSocket->GetReplaySet())->EndofFile)
			state |= 0X00008000;
		else
			state |= 0X80000000;

		STRUCT_GPSSERVER_VEHICLE_MOVE* lp = new STRUCT_GPSSERVER_VEHICLE_MOVE(
			pClientSocket->GetReplayingVhcID(), lat, lng,
			height, veo,
			direct, gpsTime,
			localTime, state, bAV);
		pClientSocket->SendMsg((char*)lp, sizeof(*lp));
	}
	catch(_com_error&)
	{
		try
		{
			if(lng>10.0&&lat>10.0)
			{
				(pClientSocket->GetReplaySet())->Close();
				state ^= 0X80000000;
				state |= 0X00008000;
				STRUCT_GPSSERVER_VEHICLE_MOVE* lp = new STRUCT_GPSSERVER_VEHICLE_MOVE(
					pClientSocket->GetReplayingVhcID(), lat, lng,
					height, veo,
					direct, gpsTime,
					localTime, state, bAV);
				pClientSocket->SendMsg((char*)lp, sizeof(*lp));
			}
		}
		catch(_com_error&)
		{
		}

		pClientSocket->ClearReplaySet();
	}
}
