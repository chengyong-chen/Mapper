#include "stdafx.h"
//#include "agrmmccomdlg.h"
#include "ConnectOptionDlg.h"
#include "ConnectWizType.h"
#include "../Public/ODBCDatabase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectWizType property page

CConnectWizType::CConnectWizType(CODBCDatabase& connectioninfo)
	: CPropertyPage(CConnectWizType::IDD, 0, IDS_ADOCONNECTWIZ_DBTYPE, 0), m_ConnectInfo(connectioninfo), m_bFirstShow(true)
{
	//{{AFX_DATA_INIT(CConnectWizType)
	//}}AFX_DATA_INIT
}

CConnectWizType::~CConnectWizType()
{
}

void CConnectWizType::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CConnectWizType)
	DDX_Control(pDX, IDC_CMB_DBTYPE, m_cmbDBTypes);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CConnectWizType, CPropertyPage)
	//{{AFX_MSG_MAP(CConnectWizType)
	ON_CBN_SELCHANGE(IDC_CMB_DBTYPE, OnSelchangeCmbDbtype)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// Internal use

void CConnectWizType::SetWizardButtons() const
{
	CPropertySheet* psheet = (CPropertySheet*)GetParent();
	const DBMSTYPE dbms = (DBMSTYPE)m_cmbDBTypes.GetItemData(m_cmbDBTypes.GetCurSel());
	if(dbms==DBMSTYPE::UnKnown)
		psheet->SetWizardButtons(PSWIZB_BACK);
	else
		psheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_NEXT);
}

/////////////////////////////////////////////////////////////////////////////
// CConnectWizType message handlers

BOOL CConnectWizType::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	const int item1 = ((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->AddString(_T("Microsoft SQL Server"));
	((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->SetItemData(item1, DBMSTYPE::MSSQL);
	const int item2 = ((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->AddString(_T("Oracal"));
	((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->SetItemData(item2, DBMSTYPE::Oracle);
	const int item3 = ((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->AddString(_T("Microsoft Access"));
	((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->SetItemData(item3, DBMSTYPE::Access);

	((CComboBox*)GetDlgItem(IDC_CMB_DBTYPE))->SetCurSel(m_ConnectInfo.m_dbms);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CConnectWizType::OnSetActive()
{
	if(m_bFirstShow==true)
	{
		m_cmbDBTypes.SetCurSel(m_ConnectInfo.m_dbms);
	}

	m_bFirstShow = false;

	SetWizardButtons();

	return CPropertyPage::OnSetActive();
}

LRESULT CConnectWizType::OnWizardNext()
{
	m_ConnectInfo.m_dbms = (DBMSTYPE)m_cmbDBTypes.GetItemData(m_cmbDBTypes.GetCurSel());
	if(m_ConnectInfo.m_dbms==DBMSTYPE::Access)
		return IDD_ADOCONNECTWIZ_PICKFILE;
	else
		return IDD_ADOCONNECTWIZ_DBSERVER;
}

void CConnectWizType::OnSelchangeCmbDbtype()
{
	SetWizardButtons();
}
