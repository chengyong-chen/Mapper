#include "stdafx.h"
#include "RectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRectDlg dialog

CRectDlg::CRectDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CRectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRectDlg)

	m_nLeft = 0;
	m_nTop = 0;
	m_nRight = 0;
	m_nBottom = 0;
	//}}AFX_DATA_INIT
	m_bPrint = TRUE;
}

void CRectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRectDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_LEFT, m_nLeft);
	DDX_Text(pDX, IDC_TOP, m_nTop);
	DDX_Text(pDX, IDC_RIGHT, m_nRight);
	DDX_Text(pDX, IDC_BOTTOM, m_nBottom);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRectDlg, CDialog)
	//{{AFX_MSG_MAP(CRectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRectDlg message handlers

BOOL CRectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_PRINT))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_PREVIEW))->SetCheck(FALSE);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRectDlg::OnOK()
{
	// TODO: Add extra validation here
	m_bPrint = ((CButton*)GetDlgItem(IDC_PRINT))->GetState();
	CDialog::OnOK();
}
