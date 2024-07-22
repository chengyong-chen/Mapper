#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizServer.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizServer property page

CConnectWizServer::CConnectWizServer(CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizServer::IDD, 0, IDS_ADOCONNECTWIZ_SERVER, 0), m_ConnectInfo(connectioninfo)
{
}

CConnectWizServer::~CConnectWizServer()
{
}

void CConnectWizServer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_DBNAME, m_edDbName);
	DDX_Control(pDX, IDC_STC_DB_TEXT, m_stcDbText);
	DDX_Control(pDX, IDC_EDT_SERVER_NAME, m_edServerName);
	DDX_Control(pDX, IDC_STC_SN_TEXT, m_stcSnText);
	DDX_Control(pDX, IDC_STC_SN_CAPTION, m_stcSnCaption);
	DDX_Control(pDX, IDC_STC_PORT, m_stcPort);
	DDX_Control(pDX, IDC_EDT_PORT, m_edtPort);
}

BEGIN_MESSAGE_MAP(CConnectWizServer, CPropertyPage)
	ON_EN_CHANGE(IDC_EDIT_DBNAME, OnChangeEditDbname)
	ON_EN_CHANGE(IDC_EDT_SERVER_NAME, OnChangeEdtServerName)
	ON_EN_CHANGE(IDC_EDT_PORT, &CConnectWizServer::OnEnChangeEdtPort)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Internal use

void CConnectWizServer::SetWizardButtons()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();

	CString cServerName;
	m_edServerName.GetWindowText(cServerName);
	cServerName.TrimLeft();
	cServerName.TrimRight();

	CString cDbName;
	m_edDbName.GetWindowText(cDbName);
	cDbName.TrimLeft();
	cDbName.TrimRight();

	CString sPort;
	m_edtPort.GetWindowText(sPort);
	sPort.Trim();

	if(sPort.IsEmpty()==TRUE||cServerName.IsEmpty()==TRUE||m_ConnectInfo.m_strUID.CompareNoCase(L"Oralce")==0&&cDbName.IsEmpty()==TRUE)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectWizServer message handlers

BOOL CConnectWizServer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_edtPort.SetWindowText(L"3306");

	// TODO: Add extra initialization here

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectWizServer::OnSetActive()
{
	if(m_sDbType.CompareNoCase(m_ConnectInfo.m_strUID)!=0)
	{
		m_edServerName.SetWindowText((LPCWSTR)m_ConnectInfo.m_strServer);

		if(m_ConnectInfo.m_strUID.CompareNoCase(L"Oracle")==0)
		{
			m_edDbName.SetWindowText(L"");
			m_edDbName.ShowWindow(SW_HIDE);
			m_stcDbText.ShowWindow(SW_HIDE);
			m_stcPort.ShowWindow(SW_HIDE);
			m_edtPort.ShowWindow(SW_HIDE);
			m_stcSnText.SetWindowText(L"Oracle Connection String");
			m_stcSnCaption.SetWindowText(L"Oracle Easy Connect string. Format: <Hostname>:<Port>/<Service Name|Sid>");
		}
		else
		{
			m_edDbName.SetWindowText((LPCWSTR)m_ConnectInfo.m_strDatabase);
			m_edDbName.ShowWindow(SW_SHOW);
			m_stcDbText.ShowWindow(SW_SHOW);
			m_stcPort.ShowWindow(SW_HIDE);
			m_edtPort.ShowWindow(SW_HIDE);
			m_stcSnText.SetWindowText(L"Database Server Computer Name");
			m_stcSnCaption.SetWindowText(L"Enter the name of the computer running the database");
		}
	}

	m_sDbType = m_ConnectInfo.m_strUID;

	SetWizardButtons();

	return CPropertyPage::OnSetActive();
}

LRESULT CConnectWizServer::OnWizardNext()
{
	CString cServerName;
	m_edServerName.GetWindowText(cServerName);
	cServerName.TrimLeft();
	cServerName.TrimRight();

	CString cDbName;
	m_edDbName.GetWindowText(cDbName);
	cDbName.TrimLeft();
	cDbName.TrimRight();

	m_ConnectInfo.m_strServer = cServerName;
	m_ConnectInfo.m_strDatabase = cDbName;
	return IDD_ADOCONNECTWIZ_USERINFO;
}

void CConnectWizServer::OnChangeEditDbname()
{
	SetWizardButtons();
}

void CConnectWizServer::OnChangeEdtServerName()
{
	SetWizardButtons();
}

void CConnectWizServer::OnEnChangeEdtPort()
{
	SetWizardButtons();
}
