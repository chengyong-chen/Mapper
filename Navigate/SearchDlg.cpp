// SearchDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SearchDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg dialog

CSearchDlg::CSearchDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSearchDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSearchDlg)
	m_strKeyField = _T("");
	//}}AFX_DATA_INIT
}

void CSearchDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSearchDlg)
	DDX_Text(pDX, IDC_KEY_STRING, m_strKeyField);
	DDV_MaxChars(pDX, m_strKeyField, 20);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSearchDlg, CDialog)
	//{{AFX_MSG_MAP(CSearchDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSearchDlg message handlers

void CSearchDlg::OnOK()
{
	UpdateData(TRUE);

	CDialog::OnOK();
}

BOOL CSearchDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_KEY_STRING)->SetFocus();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
