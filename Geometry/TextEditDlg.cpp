#include "stdafx.h"
#include "TextEditDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTextEditDlg dialog

CTextEditDlg::CTextEditDlg(CWnd* pParent /*=nullptr*/, CString pstring)
	: CDialogEx(CTextEditDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTextEditDlg)
	m_String = _T("");
	//}}AFX_DATA_INIT
	m_String = pstring;
}

void CTextEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextEditDlg)
	DDX_Text(pDX, IDC_TEXT, m_String);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTextEditDlg, CDialog)
	//{{AFX_MSG_MAP(CTextEditDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEditDlg message handlers
