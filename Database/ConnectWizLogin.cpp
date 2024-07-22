#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizLogin.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CConnectWizLogin::CConnectWizLogin(CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizLogin::IDD, 0, IDS_ADOCONNECTWIZ_LOGIN, 0), m_ConnectInfo(connectioninfo)
{
	//{{AFX_DATA_INIT(CConnectWizLogin)

	//}}AFX_DATA_INIT
}

CConnectWizLogin::~CConnectWizLogin()
{
}

void CConnectWizLogin::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectWizLogin)
	DDX_Control(pDX, IDC_ED_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_ED_USERNAME, m_edUserName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConnectWizLogin, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectWizLogin)
	ON_BN_CLICKED(IDC_BTN_TEST, OnBtnTest)
	ON_EN_CHANGE(IDC_ED_USERNAME, OnChangeEdUsername)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Internal use

void CConnectWizLogin::SetWizardButtons() const
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();

	CString cUserName;
	m_edUserName.GetWindowText(cUserName);
	cUserName.TrimLeft();
	cUserName.TrimRight();

	if(m_ConnectInfo.m_dbms==DBMSTYPE::Access)
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
	else if(cUserName.IsEmpty()==TRUE)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

BOOL CConnectWizLogin::OnSetActive()
{
	m_edUserName.SetWindowText(m_ConnectInfo.m_strUID);
	m_edPassword.SetWindowText(m_ConnectInfo.m_strPWD);

	SetWizardButtons();

	return CPropertyPage::OnSetActive();
}

LRESULT CConnectWizLogin::OnWizardNext()
{
	CString cUserName;
	m_edUserName.GetWindowText(cUserName);
	cUserName.TrimLeft();
	cUserName.TrimRight();
	m_ConnectInfo.m_strUID = cUserName;

	CString cPW;
	m_edPassword.GetWindowText(cPW);
	cPW.TrimLeft();
	cPW.TrimRight();
	m_ConnectInfo.m_strPWD = cPW;
	//CAgrCrypt cCrypt(C_BACKOFFICEDS);
	//m_ConnectInfo.sInitString = cCrypt.Crypt(cUserName, cPW);

	return CPropertyPage::OnWizardNext();
}

LRESULT CConnectWizLogin::OnWizardBack()
{
	if(m_ConnectInfo.m_dbms==DBMSTYPE::Access)
		return IDD_ADOCONNECTWIZ_PICKFILE;
	else
		return IDD_ADOCONNECTWIZ_DBSERVER;
}

void CConnectWizLogin::OnBtnTest()
{
	m_edUserName.GetWindowText(m_ConnectInfo.m_strUID);
	m_edPassword.GetWindowText(m_ConnectInfo.m_strPWD);

	if(m_ConnectInfo.Open()==true)
	{
		MessageBox(L"Connection succeeded", L"Connection Test", MB_ICONINFORMATION|MB_OK);
	}
	else
	{
		MessageBox(L"Test Failed!\n\n", L"Connection Test", MB_ICONERROR|MB_OK);
	}
}

void CConnectWizLogin::OnChangeEdUsername()
{
	SetWizardButtons();
}
