#include "stdafx.h"
#include "RotateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg dialog

CRotateDlg::CRotateDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CRotateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRotateDlg)
	m_bFill = FALSE;
	m_bLine = FALSE;
	m_bSpot = FALSE;
	m_bType = FALSE;
	p_bMouse = TRUE;
	m_bRegular = TRUE;
	//}}AFX_DATA_INIT
}

void CRotateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRotateDlg)
	DDX_Check(pDX, IDC_FILL, m_bFill);
	DDX_Check(pDX, IDC_LINE, m_bLine);
	DDX_Check(pDX, IDC_SPOT, m_bSpot);
	DDX_Check(pDX, IDC_TYPE, m_bType);
	DDX_Check(pDX, IDC_REGULAR, m_bRegular);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRotateDlg, CDialog)
	//{{AFX_MSG_MAP(CRotateDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRotateDlg message handlers

BOOL CRotateDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	((CButton*)GetDlgItem(IDC_MOUSE))->SetCheck(p_bMouse);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CRotateDlg::OnOK()
{
	p_bMouse = ((CButton*)GetDlgItem(IDC_MOUSE))->GetState();

	TCHAR str[20];
	double dAngle;

	GetDlgItem(IDC_ANGLE)->GetWindowText(str, 20);
	dAngle = _tcstod(str, nullptr);

	if(dAngle==0)
	{
		OnCancel();
		return;
	}
	p_fAngle = dAngle;
	CDialog::OnOK();
}
