#include "stdafx.h"
#include "DatabasePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDatabasePage dialog

CDatabasePage::CDatabasePage()
	: CPropertyPage(CDatabasePage::IDD)
{
	//{{AFX_DATA_INIT(CDatabasePage)
	//}}AFX_DATA_INIT
}

CDatabasePage::~CDatabasePage()
{
}

void CDatabasePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDatabasePage)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDatabasePage, CPropertyPage)
	//{{AFX_MSG_MAP(CDatabasePage)
	ON_BN_CLICKED(IDC_SETUP, OnSetup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDatabasePage message handlers

BOOL CDatabasePage::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	SetWindowPos(&wndTop, 10, 77, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	const CString strDSN = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("DSN"), nullptr);
	const CString strServerName = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("ServerName"), nullptr);
	const CString strUID = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("UID"), nullptr);
	const CString strPWD = AfxGetApp()->GetProfileString(_T("VechileDB"), _T("PWD"), nullptr);

	GetDlgItem(IDC_ODBCDSN)->SetWindowText(strDSN);
	GetDlgItem(IDC_SERVERNAME)->SetWindowText(strServerName);
	GetDlgItem(IDC_ODBCUID)->SetWindowText(strUID);
	GetDlgItem(IDC_ODBCPWD)->SetWindowText(strPWD);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDatabasePage::OnSetup()
{
	CDatabase database;
	try
	{
		database.Open(nullptr, FALSE, FALSE, _T("ODBC;"), FALSE);

		CString dbms;
		CString strDSN;
		CString strServerName;
		CString strUID;
		CString strPWD;

		UCHAR buffer[200];
		SWORD cbData;

		::SQLGetInfoA(database.m_hdbc, SQL_DRIVER_NAME, (PTR)buffer, 200, &cbData);
		const CString sBuff(buffer);
		const BOOL bIsJetDriver = !sBuff.CompareNoCase(_T("ODBCJT32.DLL"));
		if(bIsJetDriver)
		{
			strServerName = database.GetDatabaseName();
		}
		else
		{
			::SQLGetInfoA(database.m_hdbc, SQL_SERVER_NAME, (PTR)buffer, 200, &cbData);
			strServerName = buffer;
			if(strServerName.IsEmpty())
			{
				::SQLGetInfoA(database.m_hdbc, SQL_DBMS_NAME, (PTR)buffer, 200, &cbData);
				strServerName = buffer;
			}
		}

		::SQLGetInfoA(database.m_hdbc, SQL_DATA_SOURCE_NAME, (PTR)buffer, 200, &cbData);
		strDSN = buffer;

		::SQLGetInfoA(database.m_hdbc, SQL_DBMS_NAME, (PTR)buffer, 200, &cbData);
		dbms = buffer;
		const CString strConnect = database.GetConnect();
		const int idPos = strConnect.Find(_T("UID="));
		if(idPos!=-1)
		{
			const int pos = strConnect.Find(_T(';'), idPos+4);
			strUID = strConnect.Mid(idPos+4, pos-idPos-4);
		}
		const int pwdPos = strConnect.Find(_T("PWD="));
		if(pwdPos!=-1)
		{
			const int pos = strConnect.Find(_T(';'), pwdPos+4);
			strPWD = strConnect.Mid(pwdPos+4, pos-pwdPos-4);
		}

		GetDlgItem(IDC_ODBCDSN)->SetWindowText(strDSN);
		GetDlgItem(IDC_SERVERNAME)->SetWindowText(strServerName);
		GetDlgItem(IDC_ODBCUID)->SetWindowText(strUID);
		GetDlgItem(IDC_ODBCPWD)->SetWindowText(strPWD);

		AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("DBMS"), dbms);
		AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("DSN"), strDSN);
		AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("ServerName"), strServerName);
		AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("UID"), strUID);
		AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("PWD"), strPWD);

		database.Close();
	}
	catch(CDBException*ex)
	{
		ex->ReportError();
		AfxMessageBox(_T("Error happened!"));
		ex->Delete();
	}
}

void CDatabasePage::OnOK()
{
	CDialog::OnOK();

	CString strDSN;
	CString strServerName;
	CString strUID;
	CString strPWD;

	GetDlgItem(IDC_ODBCDSN)->GetWindowText(strDSN);
	GetDlgItem(IDC_SERVERNAME)->GetWindowText(strServerName);
	GetDlgItem(IDC_ODBCUID)->GetWindowText(strUID);
	GetDlgItem(IDC_ODBCPWD)->GetWindowText(strPWD);

	AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("DSN"), strDSN);
	AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("ServerName"), strServerName);
	AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("UID"), strUID);
	AfxGetApp()->WriteProfileString(_T("VechileDB"), _T("PWD"), strPWD);
}
