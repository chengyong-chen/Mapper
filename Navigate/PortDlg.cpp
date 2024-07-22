#include "stdafx.h"
#include "PortDlg.h"

#include "SerialPage.h"
#include "SocketPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPortDlg dialog

CPortDlg::CPortDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPortDlg)

	//}}AFX_DATA_INIT
	d_pStyledlg = nullptr;
}

void CPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPortDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPortDlg, CDialog)
	//{{AFX_MSG_MAP(CPortDlg)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPortDlg message handlers

void CPortDlg::OnDestroy()
{
	if(d_pStyledlg!=nullptr)
	{
		d_pStyledlg->PostMessage(WM_DESTROY, 0, 0);
		d_pStyledlg->DestroyWindow();
		delete d_pStyledlg;
		d_pStyledlg = nullptr;
	}

	CDialog::OnDestroy();
}

BOOL CPortDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("GPS Receiver"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Socket"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("GSM Modem"));
	const CString strPort = AfxGetApp()->GetProfileString(_T("Navigate"), _T("Port"), _T("GPS Receiver"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SelectString(-1, strPort);
	OnTypeSelchange();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CPortDlg::OnTypeSelchange()
{
	if(d_pStyledlg!=nullptr)
	{
		d_pStyledlg->PostMessage(WM_DESTROY, 0, 0);
		d_pStyledlg->DestroyWindow();
		delete d_pStyledlg;
		d_pStyledlg = nullptr;
	}
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	HINSTANCE hInst = ::LoadLibrary(_T("Navigate.dll"));
	AfxSetResourceHandle(hInst);
	const long index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	if(index>=0)
	{
		CString strPort;
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetLBText(index, strPort);

		if(strPort.CompareNoCase(_T("GPS Receiver"))==0)
		{
			d_pStyledlg = new CSerialPage();
			((CSerialPage*)d_pStyledlg)->Create(IDD_SERIAL, (CWnd*)this);
			d_pStyledlg->ShowWindow(SW_SHOWNORMAL);
		}
		else if(strPort.CompareNoCase(_T("Socket"))==0)
		{
			d_pStyledlg = new CSocketPage();
			((CSocketPage*)d_pStyledlg)->Create(IDD_SOCKET, (CWnd*)this);
			d_pStyledlg->ShowWindow(SW_SHOWNORMAL);
		}
		else if(strPort.CompareNoCase(_T("GSM Modem"))==0)
		{
			d_pStyledlg = new CSerialPage();
			((CSerialPage*)d_pStyledlg)->Create(IDD_SERIAL, (CWnd*)this);
			d_pStyledlg->ShowWindow(SW_SHOWNORMAL);
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CPortDlg::OnOK()
{
	if(d_pStyledlg!=nullptr&&d_pStyledlg->m_hWnd!=nullptr)
	{
		d_pStyledlg->SendMessage(WM_COMMAND, IDOK, 0);
	}
	const long index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	if(index>=0)
	{
		CString strPort;
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetLBText(index, strPort);
		AfxGetApp()->WriteProfileString(_T("Navigate"), _T("Port"), strPort);
	}

	CDialog::OnOK();
}
