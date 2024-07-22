#include "stdafx.h"

#include "ExtentDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtentDlg dialog

CExtentDlg::CExtentDlg(CWnd* pParent /*=nullptr*/, long nExtent)
	: CDialog(CExtentDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExtentDlg)
	m_nExtent = nExtent;
	//}}AFX_DATA_INIT
}

void CExtentDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExtentDlg)
	DDX_Text(pDX, IDC_EXTENT, m_nExtent);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExtentDlg, CDialog)
	//{{AFX_MSG_MAP(CExtentDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtentDlg message handlers
