#include "stdafx.h"

#include "Vehicle.h"
#include "Port.h"
#include "Replay.h"

#include "../Viewer/Global.h"
#include "../Coding/Instruction.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CReplay, CPort)

CReplay::CReplay(CFrameWnd* pParents)
	: CPort(pParents)
{
	m_Count = 0;
	m_Current = 0;
}

bool CReplay::Open()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CString strPath = AfxGetApp()->GetProfileString(_T(""), _T("Gps Route Path"), nullptr)+_T("*.Grf");
	CFileDialog dlg(TRUE,
		_T("*.Grf"),
		strPath,
		OFN_HIDEREADONLY,
		_T("Gps route Files (*.Grf)|*.Grf||"),
		nullptr);

	dlg.m_ofn.lpstrTitle = _T("请选择历史文件");
	if(dlg.DoModal()!=IDOK)
	{
		SetCurrentDirectory(CurrentDir);
		return false;
	}
	SetCurrentDirectory(CurrentDir);

	CString strFile = dlg.GetPathName();

	if(m_Route.Open(strFile, CFile::modeRead|CFile::shareDenyWrite|CFile::typeBinary)==TRUE)
	{
		m_Route.Read(&m_dwId, 4);

		long length = m_Route.GetLength();
		m_Count = (length-4)/(8*2+4*3);
		m_Current = 0;

		strFile = strFile.Left(strFile.ReverseFind(_T('\\'))+1);
		AfxGetApp()->WriteProfileString(_T(""), _T("Gps Route Path"), strFile);

		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;

		TCHAR szUser[256];
		DWORD size = 256;
		GetUserName(szUser, &size);

		Content content;
		content.dwId = m_dwId;
		content.strUser = szUser;
		content.strCode = _T("");
		pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_NEW, (long)&content);

		BOOL bTrack = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("Track"), 1);
		BOOL bDrawTrace = AfxGetApp()->GetProfileInt(_T("Receiver"), _T("DrawTrace"), 0);
		if(bTrack==TRUE)
			pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_TRACK, m_dwId);
		if(bDrawTrace==TRUE)
			pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_TRACE_DRAW, m_dwId);

		long nReplaySpeed = AfxGetApp()->GetProfileInt(_T("Socket"), _T("ReplaySpeed"), 200);
		pAppFrame->SetTimer(TIMER_REPLAY, nReplaySpeed, nullptr);

		return CPort::Open();
	}
	else
		return false;
}

void CReplay::ReceiveMsg()
{
	if(m_Route.m_hFile==CFile::hFileNull)
	{
		return;
	}

	if(m_Current<m_Count)
	{
		m_message.m_bAvailable = TRUE;
		m_Route.Read(&m_message.lng, 8);
		m_Route.Read(&m_message.lat, 8);
		m_Route.Read(&m_message.fSpeed, 4);
		m_Route.Read(&m_message.nBearing, 4);
		m_Route.Read(&m_message.nTime, 4);
		m_Current++;

		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		if(pAppFrame!=nullptr)
		{
			pAppFrame->SendMessage(WM_VEHICLEMOVE, m_dwId, (LONG)&m_message);
		}

		CPort::ReceiveMsg();
	}
	else
	{
		CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
		if(pAppFrame!=nullptr)
		{
			pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, m_dwId);
			pAppFrame->SendMessage(WM_VEHICLECONTROL, SERVER_DOWN, 0);
		}
	}
}

void CReplay::Close()
{
	CPort::Close();

	CFrameWnd* pAppFrame = (CFrameWnd*)AfxGetApp()->m_pMainWnd;
	if(pAppFrame!=nullptr)
	{
		pAppFrame->SendMessage(WM_VEHICLECONTROL, GPSSERVER_VEHICLE_EREASE, m_dwId);
	}

	AfxGetMainWnd()->KillTimer(TIMER_REPLAY);

	if(m_Route.m_hFile!=CFile::hFileNull)
	{
		m_Route.Close();
	}
}
