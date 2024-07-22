#include "stdafx.h"
#include "SetIfDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CSetIfDlg dialog

IMPLEMENT_DYNAMIC(CSetIfDlg, CDialog)

CSetIfDlg::CSetIfDlg(CWnd* pParent /*=nullptr*/, CString strIf)
	: CDialog(CSetIfDlg::IDD, pParent)
{
	m_strIf = strIf;
}

CSetIfDlg::~CSetIfDlg()
{
}

void CSetIfDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSetIfDlg)
	DDX_Text(pDX, IDC_IF, m_strIf);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSetIfDlg, CDialog)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetIfDlg message handlers
BOOL CSetIfDlg::OnInitDialog()
{
	CDialog::OnInitDialog();


	return TRUE;
}

void CSetIfDlg::OnOK()
{
	CDialog::OnOK();
}
