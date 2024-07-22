#include "stdafx.h"
#include "PageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPageDlg dialog

CPageDlg::CPageDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageDlg)
	m_bottomMargin = 0;
	m_leftMargin = 0;
	m_rightMargin = 0;
	m_topMargin = 0;
	m_bVCenter = FALSE;
	m_bHCenter = FALSE;
	//}}AFX_DATA_INIT
}

void CPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageDlg)
	DDX_Text(pDX, IDC_BOTTOM, m_bottomMargin);
	DDX_Text(pDX, IDC_LEFT, m_leftMargin);
	DDX_Text(pDX, IDC_RIGHT, m_rightMargin);
	DDX_Text(pDX, IDC_TOP, m_topMargin);
	DDX_Check(pDX, IDC_VCENTER, m_bVCenter);
	DDX_Check(pDX, IDC_HCENTER, m_bHCenter);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageDlg, CDialog)
	//{{AFX_MSG_MAP(CPageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageDlg message handlers

void CPageDlg::OnOK()
{
	CDialog::OnOK();

	m_topMargin /= 2.54;
	m_bottomMargin /= 2.54;
	m_leftMargin /= 2.54;
	m_rightMargin /= 2.54;
}
