#include "stdafx.h"

#include "RRectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRRectDlg dialog
CRRectDlg::CRRectDlg(CWnd* pParent, long nRadius, long nMax)
	: CDialog(CRRectDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRRectDlg)
	m_nRadius = 0;
	//}}AFX_DATA_INIT
	m_nRadius = nRadius;
	m_nMax = nMax;
}

void CRRectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRRectDlg)
	DDX_Text(pDX, IDC_RADIUS, m_nRadius);
	DDV_MinMaxInt(pDX, m_nRadius, 1, m_nMax);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRRectDlg, CDialog)
	//{{AFX_MSG_MAP(CRRectDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRRectDlg message handlers

BOOL CRRectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CenterWindow();
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
