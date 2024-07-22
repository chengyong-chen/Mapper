#include "stdafx.h"

#include "ScaleDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CScaleDlg dialog

CScaleDlg::CScaleDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CScaleDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CScaleDlg)
	m_bFill = FALSE;
	m_bLine = FALSE;
	m_bSpot = FALSE;
	m_bType = FALSE;

	p_fVscale = 100;
	p_fHscale = 100;
	p_bMouse = TRUE;
	m_bRegular = TRUE;
	//}}AFX_DATA_INIT
}

void CScaleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CScaleDlg)
	DDX_Check(pDX, IDC_FILL, m_bFill);
	DDX_Check(pDX, IDC_LINE, m_bLine);
	DDX_Check(pDX, IDC_SPOT, m_bSpot);
	DDX_Check(pDX, IDC_TYPE, m_bType);
	DDX_Check(pDX, IDC_REGULAR, m_bRegular);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CScaleDlg, CDialog)
	//{{AFX_MSG_MAP(CScaleDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScaleDlg message handlers

BOOL CScaleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	((CButton*)GetDlgItem(IDC_UNIFORM))->SetCheck(TRUE);

	GetDlgItem(IDC_USCALE)->SetWindowText(_T("100"));
	GetDlgItem(IDC_HSCALE)->SetWindowText(_T("100"));
	GetDlgItem(IDC_VSCALE)->SetWindowText(_T("100"));

	((CButton*)GetDlgItem(IDC_MOUSE))->SetCheck(p_bMouse);
	//	((CButton*)GetDlgItem(IDC_CONTENT))->SetCheck(p_bContens);
	//	((CButton*)GetDlgItem(IDC_FILL   ))->SetCheck(p_bFills);

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CScaleDlg::OnOK()
{
	// TODO: Add extra validation here
	//	p_bContens =((CButton*)GetDlgItem(IDC_CONTENT))->GetState();
	//	p_bFills   =((CButton*)GetDlgItem(IDC_FILL   ))->GetState();
	p_bMouse = ((CButton*)GetDlgItem(IDC_MOUSE))->GetState();

	if(((CButton*)GetDlgItem(IDC_UNIFORM))->GetState())
	{
		TCHAR str[20];
		double dScale;
		GetDlgItem(IDC_USCALE)->GetWindowText(str, 20);
		dScale = _tcstod(str, nullptr);
		if(dScale<=0)
		{
			OnCancel();
			return;
		}
		p_fVscale = dScale;
		p_fHscale = dScale;
	}
	else
	{
		TCHAR str[20];
		GetDlgItem(IDC_HSCALE)->GetWindowText(str, 20);
		const double dHScale = _tcstod(str, nullptr);

		GetDlgItem(IDC_VSCALE)->GetWindowText(str, 20);
		const double dVScale = _tcstod(str, nullptr);

		if(dHScale<=0||dVScale<=0)
		{
			OnCancel();
			return;
		}
		p_fHscale = dHScale;
		p_fVscale = dVScale;
	}

	CDialog::OnOK();
}
