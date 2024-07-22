#include "stdafx.h"
#include "Resource.h"
#include "ExportDIBDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExportDIBDlg dialog

CExportDIBDlg::CExportDIBDlg(CWnd* pParent /*=nullptr*/, double fScale)
	: CDialog(CExportDIBDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CExportDIBDlg)

	m_fScale = fScale;
	//}}AFX_DATA_INIT
}

void CExportDIBDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExportDIBDlg)
	// NOTE: the ClassWizard will add DDX and DDV calls here
	DDX_Text(pDX, IDC_SCALE, m_fScale);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CExportDIBDlg, CDialog)
	//{{AFX_MSG_MAP(CExportDIBDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExportDIBDlg message handlers
