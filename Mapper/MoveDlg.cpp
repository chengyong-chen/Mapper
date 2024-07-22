#include "stdafx.h"
#include "MoveDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMoveDlg dialog
CMoveDlg::CMoveDlg(CWnd* pParent, CString strUnit)
	: CDialog(CMoveDlg::IDD, pParent), m_strUnit(strUnit)
{
	//{{AFX_DATA_INIT(CMoveDlg)
	m_bContents = TRUE;
	m_bFills = TRUE;
	m_fCx = 0.0f;
	m_fCy = 0.0f;
	//}}AFX_DATA_INIT
}

void CMoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMoveDlg)
	DDX_Check(pDX, IDC_CONTENT, m_bContents);
	DDX_Check(pDX, IDC_FILL, m_bFills);
	DDX_Text(pDX, IDC_CX, m_fCx);
	DDX_Text(pDX, IDC_CY, m_fCy);
	DDX_Text(pDX, IDC_UNIT, m_strUnit);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMoveDlg, CDialog)
	//{{AFX_MSG_MAP(CMoveDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMoveDlg message handlers
BOOL CMoveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_CONTENT))->SetCheck(m_bContents);
	((CButton*)GetDlgItem(IDC_FILL))->SetCheck(m_bFills);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMoveDlg::OnOK()
{
	if((((CButton*)GetDlgItem(IDC_CONTENT))->GetState()&0X0003)) m_bContents = TRUE;
	if((((CButton*)GetDlgItem(IDC_FILL))->GetState()&0X0003)) m_bFills = TRUE;

	CDialog::OnOK();
}
