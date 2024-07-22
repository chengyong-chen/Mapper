#include "stdafx.h"
#include "StatDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatDlg dialog

CStatDlg::CStatDlg(CWnd* pParent, CString strField, CString strCount, CString strMax, CString strMin, CString strAvg, CString strSum)
	: CDialog(CStatDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CStatDlg)

	//}}AFX_DATA_INIT
	m_strField = strField;
	m_strCount = strCount;
	m_strMax = strMax;
	m_strMin = strMin;
	m_strAvg = strAvg;
	m_strSum = strSum;
}

void CStatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CStatDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CStatDlg, CDialog)
	//{{AFX_MSG_MAP(CStatDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStatDlg message handlers

BOOL CStatDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_FIELD)->SetWindowText(m_strField);
	GetDlgItem(IDC_COUNT)->SetWindowText(m_strCount);
	GetDlgItem(IDC_MAX)->SetWindowText(m_strMax);
	GetDlgItem(IDC_MIN)->SetWindowText(m_strMin);
	GetDlgItem(IDC_AVG)->SetWindowText(m_strAvg);
	GetDlgItem(IDC_SUM)->SetWindowText(m_strSum);

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
