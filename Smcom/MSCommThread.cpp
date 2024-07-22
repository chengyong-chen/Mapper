// ThreadMSComm.cpp : implementation file
//

#include "stdafx.h"
#include "MSCommThread.h"
#include "MSCommDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMSCommThread

IMPLEMENT_DYNCREATE(CMSCommThread, CWinThread)

CMSCommThread::CMSCommThread()
{
	m_pMSCommDlg = nullptr;
}

CMSCommThread::~CMSCommThread()
{
}

BOOL CMSCommThread::InitInstance()
{
	m_pMainWnd = nullptr;

	CoInitialize(nullptr);
	AfxEnableControlContainer();

	CMSCommDlg* pMSCommDlg = new CMSCommDlg();
	if(pMSCommDlg->Create(IDD_MSCOMM)==FALSE)
	{
		delete pMSCommDlg;
		pMSCommDlg = nullptr;

		AfxMessageBox(_T("����ͨѶ�߳�ʧ��!"));

		return FALSE;
	}
	else
	{
		pMSCommDlg->ShowWindow(SW_HIDE);
		m_pMSCommDlg = pMSCommDlg;

		return TRUE;
	}
}

int CMSCommThread::ExitInstance()
{
	if(m_pMSCommDlg!=nullptr)
	{
		m_pMSCommDlg->Close();

		Sleep(800);

		delete m_pMSCommDlg;
		m_pMSCommDlg = nullptr;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CMSCommThread, CWinThread)
	//{{AFX_MSG_MAP(CMSCommThread)
	// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSCommThread message handlers
