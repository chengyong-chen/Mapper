#include "stdafx.h"
#include "SetupDlg.h"

#include "SerialPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

CSetupDlg::CSetupDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)

	//}}AFX_DATA_INIT
	d_pStyledlg = nullptr;
}

void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_TYPECOMBO, OnTypeSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

void CSetupDlg::OnDestroy()
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

BOOL CSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->AddString(_T("Wavecom"));
	((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->SelectString(-1, _T("Wavecom"));

	OnTypeSelchange();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupDlg::OnTypeSelchange()
{
	if(d_pStyledlg!=nullptr)
	{
		d_pStyledlg->PostMessage(WM_DESTROY, 0, 0);
		d_pStyledlg->DestroyWindow();
		delete d_pStyledlg;
		d_pStyledlg = nullptr;
	}
	HINSTANCE hInst = ::LoadLibrary(_T("Smcom.dll"));
	const HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(hInst);
	const long index = ((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetCurSel();
	if(index>=0)
	{
		CString strPort;
		((CComboBox*)GetDlgItem(IDC_TYPECOMBO))->GetLBText(index, strPort);

		if(strPort.CompareNoCase(_T("Wavecom"))==0)
		{
			d_pStyledlg = new CSerialPage();
			((CSerialPage*)d_pStyledlg)->Create(IDD_SERIAL, (CWnd*)this);
			d_pStyledlg->ShowWindow(SW_SHOWNORMAL);
		}
	}

	AfxSetResourceHandle(hInstOld);
	::FreeLibrary(hInst);
}

void CSetupDlg::OnOK()
{
	if(d_pStyledlg!=nullptr&&d_pStyledlg->m_hWnd!=nullptr)
	{
		d_pStyledlg->SendMessage(WM_COMMAND, IDOK, 0);
	}

	CDialog::OnOK();
}
