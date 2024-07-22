#include "stdafx.h"
#include "DatabaseDlg.h"

#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatabaseDlg dialog

CDatabaseDlg::CDatabaseDlg()
	: CDialog(CDatabaseDlg::IDD)
{
	//{{AFX_DATA_INIT(CDatabaseDlg)
	//}}AFX_DATA_INIT
}

CDatabaseDlg::~CDatabaseDlg()
{
}

void CDatabaseDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatabaseDlg)
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_COMBODBMS, m_comboDBMS);
}

BEGIN_MESSAGE_MAP(CDatabaseDlg, CDialog)
	//{{AFX_MSG_MAP(CDatabaseDlg)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatabaseDlg message handlers

void CDatabaseDlg::OnOK()
{
	CDialog::OnOK();
	const int item = m_comboDBMS.GetCurSel();
	if(item!=CB_ERR)
	{
		dbms = (DBMSTYPE)(m_comboDBMS.GetItemData(item));
	}
	GetDlgItem(IDC_SERVER)->GetWindowText(strServer);
	GetDlgItem(IDC_DATABASE)->GetWindowText(strDatabase);
	GetDlgItem(IDC_ODBCUID)->GetWindowText(strUID);
	GetDlgItem(IDC_ODBCPWD)->GetWindowText(strPWD);
}

BOOL CDatabaseDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SetWindowPos(&wndTop, 10, 77, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	const int item1 = ((CComboBox*)GetDlgItem(IDC_COMBODBMS))->AddString(_T("Microsoft SQL Server"));
	((CComboBox*)GetDlgItem(IDC_COMBODBMS))->SetItemData(item1, DBMSTYPE::MSSQL);
	const int item2 = ((CComboBox*)GetDlgItem(IDC_COMBODBMS))->AddString(_T("Oracal"));
	((CComboBox*)GetDlgItem(IDC_COMBODBMS))->SetItemData(item2, DBMSTYPE::Oracle);
	const int item3 = ((CComboBox*)GetDlgItem(IDC_COMBODBMS))->AddString(_T("Microsoft Access"));
	((CComboBox*)GetDlgItem(IDC_COMBODBMS))->SetItemData(item3, DBMSTYPE::Access);
	((CComboBox*)GetDlgItem(IDC_COMBODBMS))->SetCurSel(dbms);

	GetDlgItem(IDC_SERVER)->SetWindowText(strServer);
	GetDlgItem(IDC_DATABASE)->SetWindowText(strDatabase);
	GetDlgItem(IDC_ODBCUID)->SetWindowText(strUID);
	GetDlgItem(IDC_ODBCPWD)->SetWindowText(strPWD);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDatabaseDlg::OnSetup()
{
	CODBCDatabase database;
	try
	{
		//		if(database.Attach() == true)
		{
			dbms = database.m_dbms;
			strServer = database.m_strServer;
			strDatabase = database.m_strDatabase;
			strUID = database.m_strUID;
			strPWD = database.m_strPWD;

			m_comboDBMS.SetCurSel(dbms-1);
			GetDlgItem(IDC_SERVER)->SetWindowText(strServer);
			GetDlgItem(IDC_DATABASE)->SetWindowText(strDatabase);
			GetDlgItem(IDC_ODBCUID)->SetWindowText(strUID);
			GetDlgItem(IDC_ODBCPWD)->SetWindowText(strPWD);

			database.Close();
		}
	}
	catch(CDBException*ex)
	{
		OutputDebugString(ex->m_strError);
		ex->ReportError();
		AfxMessageBox(_T("Failed to set up the database!"));
		ex->Delete();
	}
	catch(CException*ex)
	{
		ex->Delete();
	}
}
