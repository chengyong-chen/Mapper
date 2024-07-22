#include "stdafx.h"
#include "SkewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSkewDlg dialog

CSkewDlg::CSkewDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CSkewDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSkewDlg)
	m_nHangle = 0;
	m_nVangle = 0;
	m_bFill = FALSE;
	m_bLine = FALSE;
	m_bSpot = FALSE;
	m_bType = FALSE;

	p_bMouse = TRUE;
	m_bRegular = TRUE;
	//}}AFX_DATA_INIT
}

void CSkewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSkewDlg)
	DDX_Text(pDX, IDC_HANGLE, m_nHangle);
	DDV_MinMaxInt(pDX, m_nHangle, -360, 360);
	DDX_Text(pDX, IDC_VANGLE, m_nVangle);
	DDV_MinMaxInt(pDX, m_nVangle, -360, 360);
	DDX_Check(pDX, IDC_FILL, m_bFill);
	DDX_Check(pDX, IDC_LINE, m_bLine);
	DDX_Check(pDX, IDC_SPOT, m_bSpot);
	DDX_Check(pDX, IDC_TYPE, m_bType);
	DDX_Check(pDX, IDC_REGULAR, m_bRegular);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSkewDlg, CDialog)
	//{{AFX_MSG_MAP(CSkewDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSkewDlg message handlers

BOOL CSkewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_MOUSE))->SetCheck(p_bMouse);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
