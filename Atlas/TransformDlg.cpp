#include "stdafx.h"
#include "TransformDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg dialog

CTransformDlg::CTransformDlg(CWnd* pParent)
	: CDialog(CTransformDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTransformDlg)
	m_a11 = 1.0;
	m_a12 = 0.0;
	m_a22 = 1.0;
	m_a31 = 0.0;
	m_a32 = 0.0;
	m_a21 = 0.0;
	//}}AFX_DATA_INIT
}

void CTransformDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransformDlg)
	DDX_Text(pDX, IDC_A11, m_a11);
	DDX_Text(pDX, IDC_A12, m_a12);
	DDX_Text(pDX, IDC_A22, m_a22);
	DDX_Text(pDX, IDC_A31, m_a31);
	DDX_Text(pDX, IDC_A32, m_a32);
	DDX_Text(pDX, IDC_A21, m_a21);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTransformDlg, CDialog)
	//{{AFX_MSG_MAP(CTransformDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransformDlg message handlers
