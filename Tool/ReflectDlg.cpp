#include "stdafx.h"

#include "ReflectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReflectDlg dialog

CReflectDlg::CReflectDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CReflectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReflectDlg)
	m_bFill = FALSE;
	m_bLine = FALSE;
	m_bSpot = FALSE;
	m_bType = FALSE;

	p_bMouse = TRUE;
	m_bRegular = TRUE;
	//}}AFX_DATA_INIT
}

void CReflectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReflectDlg)
	DDX_Check(pDX, IDC_FILL, m_bFill);
	DDX_Check(pDX, IDC_LINE, m_bLine);
	DDX_Check(pDX, IDC_SPOT, m_bSpot);
	DDX_Check(pDX, IDC_TYPE, m_bType);
	DDX_Check(pDX, IDC_REGULAR, m_bRegular);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CReflectDlg, CDialog)
	//{{AFX_MSG_MAP(CReflectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CReflectDlg message handlers

BOOL CReflectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	GetDlgItem(IDC_ANGLE)->SetWindowText(_T("90"));

	((CButton*)GetDlgItem(IDC_VERTICAL))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_MOUSE))->SetCheck(p_bMouse);
	//	((CButton*)GetDlgItem(IDC_CONTENT))->SetCheck(p_bContens);
	//	((CButton*)GetDlgItem(IDC_FILL   ))->SetCheck(p_bFills);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CReflectDlg::OnOK()
{
	// TODO: Add extra validation here

	//	p_bContens =((CButton*)GetDlgItem(IDC_CONTENT))->GetState();
	//	p_bFills   =((CButton*)GetDlgItem(IDC_FILL   ))->GetState();
	p_bMouse = ((CButton*)GetDlgItem(IDC_MOUSE))->GetState();

	if(((CButton*)GetDlgItem(IDC_VERTICAL))->GetState())
		p_fAngle = 90;
	if(((CButton*)GetDlgItem(IDC_VERTICAL))->GetState())
		p_fAngle = 0;

	if(((CButton*)GetDlgItem(IDC_ANGLED))->GetState())
	{
		TCHAR str[20];
		double dAngle;
		GetDlgItem(IDC_ANGLE)->GetWindowText(str, 20);
		dAngle = _tcstod(str, nullptr);
		p_fAngle = (long)dAngle%180;
		p_fAngle /= 57.2958;
	}
	CDialog::OnOK();
}
