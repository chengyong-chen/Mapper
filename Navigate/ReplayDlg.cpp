#include "stdafx.h"
#include "ReplayDlg.h"

#include "Port.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReplayDlg dialog

CReplayDlg::CReplayDlg(CWnd* pParent /*=nullptr*/, CPort* pPort)
	: CDialog(CReplayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReplayDlg)
	m_Date1 = COleDateTime::GetCurrentTime();
	m_Date2 = COleDateTime::GetCurrentTime();
	m_Time1 = COleDateTime::GetCurrentTime();
	m_Time2 = COleDateTime::GetCurrentTime();
	m_bHandle = FALSE;
	m_bHashing = FALSE;
	//}}AFX_DATA_INIT
	m_pPort = pPort;
	m_dwVehicle = -1;
}

void CReplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReplayDlg)
	DDX_DateTimeCtrl(pDX, IDC_DATEPICKER1, m_Date1);
	DDX_DateTimeCtrl(pDX, IDC_DATEPICKER2, m_Date2);
	DDX_DateTimeCtrl(pDX, IDC_TIMEPICKER1, m_Time1);
	DDX_DateTimeCtrl(pDX, IDC_TIMEPICKER2, m_Time2);
	DDX_Check(pDX, IDC_ANCHOR, m_bHandle);
	DDX_Check(pDX, IDC_HASHING, m_bHashing);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReplayDlg, CDialog)
	//{{AFX_MSG_MAP(CReplayDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReplayDlg message handlers

BOOL CReplayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	/*
		m_SelectDlg.m_pDatabase = &(m_pPort->m_database);
		if(m_SelectDlg.Create(IDD_SELECT,this) == TRUE)
		{
			m_SelectDlg.SetWindowPos(&wndTop, 3, 3, 0, 0, SWP_NOSIZE | SWP_SHOWWINDOW);
			m_SelectDlg.OnVehicle();

			CenterWindow();
			return TRUE;
		}
		else
		{
			return FALSE;
		}*/
	return TRUE;
}

BEGIN_EVENTSINK_MAP(CReplayDlg, CDialog)
	//{{AFX_EVENTSINK_MAP(CReplayDlg)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

LRESULT CReplayDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
	case WM_SELDLG_DBCLICK:
		break;
	case WM_SELDLG_CLICK:
		m_dwVehicle = wParam;
		break;
	case WM_SELDLG_RCLICK:
		break;
	}

	return CDialog::WindowProc(message, wParam, lParam);
}
