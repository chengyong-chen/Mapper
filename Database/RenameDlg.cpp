#include "stdafx.h"
#include "RenameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg

CRenameDlg::CRenameDlg(CWnd* pParent, const CString& cOldName)
	: m_cOldName(cOldName), CDialog(CRenameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRenameDlg)
	m_cNewName = _T("");
	//}}AFX_DATA_INIT
}

void CRenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameDlg)
	DDX_Text(pDX, IDC_NAME_NEW, m_cNewName);
	DDV_MaxChars(pDX, m_cNewName, 64);
	DDX_Text(pDX, IDC_NAME_OLD, m_cOldName);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRenameDlg, CDialog)
	//{{AFX_MSG_MAP(CRenameDlg)
	ON_BN_CLICKED(IDOK, OnOk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenameDlg message handlers

void CRenameDlg::OnOk()
{
	// get dialog data
	UpdateData(TRUE);

	CDialog::OnOK();
}
