#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectWizName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizName property page

CConnectWizName::CConnectWizName(CString& strName, CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizName::IDD, 0, IDS_ADOCONNECTWIZ_DSNAME, 0), m_ConnectInfo(connectioninfo), m_strName(strName)
{
}

CConnectWizName::~CConnectWizName()
{
}

void CConnectWizName::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDT_DS_NAME, m_edName);
}

BEGIN_MESSAGE_MAP(CConnectWizName, CPropertyPage)
	ON_EN_CHANGE(IDC_EDT_DS_NAME, SetWizardButtons)
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Internal use

void CConnectWizName::SetWizardButtons()
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();

	m_edName.GetWindowText(m_strName);
	m_strName.TrimLeft();
	m_strName.TrimRight();

	if(m_strName.IsEmpty()==TRUE)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectWizName message handlers

BOOL CConnectWizName::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// TODO: Add extra initialization here

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectWizName::OnSetActive()
{
	SetWizardButtons();

	return CPropertyPage::OnSetActive();
}
