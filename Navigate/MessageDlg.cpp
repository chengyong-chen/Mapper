// MessageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MessageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg dialog

CMessageDlg::CMessageDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CMessageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMessageDlg)
	m_dwId = 0;
	m_strMessage = _T("");
	//}}AFX_DATA_INIT
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMessageDlg)
	DDX_Text(pDX, IDC_ID, m_dwId);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMessageDlg, CDialog)
	//{{AFX_MSG_MAP(CMessageDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMessageDlg message handlers

BOOL CMessageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
