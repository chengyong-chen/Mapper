#include "stdafx.h"
#include "Page.h"
#include "PageDlg.h"
#include <winuser.h>

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
	m_strTarget = _T("");
	m_X = 0;
	m_Y = 0;
	//}}AFX_DATA_INIT
}

CPageDlg::CPageDlg(CWnd* pParent, CString strTarget, WORD wType, CPoint Anchor)
	: CDialog(CPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPageDlg)
	m_strTarget = _T("");
	//}}AFX_DATA_INIT

	m_strTarget = strTarget;
	m_wType = wType;

	m_X = Anchor.x;
	m_Y = Anchor.y;
}

void CPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPageDlg)
	DDX_Text(pDX, IDC_TARGET, m_strTarget);
	DDX_Text(pDX, IDC_X, m_X);
	DDX_Text(pDX, IDC_Y, m_Y);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPageDlg, CDialog)
	//{{AFX_MSG_MAP(CPageDlg)
	// NOTE: the ClassWizard will add message map macros here	
	ON_BN_CLICKED(IDC_FINDFILE, OnFindFile)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_FOLDER, OnBnClickedFolder)
	ON_BN_CLICKED(IDC_MAP, OnBnClickedMap)
	ON_BN_CLICKED(IDC_HTML, OnBnClickedHtml)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageDlg message handlers

BOOL CPageDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	switch(m_wType)
	{
	case CPage::typeFolder:
	{
		((CButton*)GetDlgItem(IDC_FOLDER))->SetCheck(TRUE);
		OnBnClickedFolder();
	}
	break;
	case CPage::typeMap:
	{
		((CButton*)GetDlgItem(IDC_MAP))->SetCheck(TRUE);
		OnBnClickedMap();
	}
	break;
	case CPage::typeHtml:
	{
		((CButton*)GetDlgItem(IDC_HTML))->SetCheck(TRUE);
		OnBnClickedHtml();
	}
	break;
	}

	return TRUE;
}

void CPageDlg::OnFindFile()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CFileDialog dlg(TRUE,
		_T("*.*"),
		nullptr,
		OFN_HIDEREADONLY,
		_T("All File (*.*)|*.*||"),
		this);

	if(dlg.DoModal()==IDOK)
	{
		m_strTarget = dlg.GetPathName();
		((CStatic*)GetDlgItem(IDC_TARGET))->SetWindowText(m_strTarget);
	}
	SetCurrentDirectory(CurrentDir);
}

void CPageDlg::OnOK()
{
	CDialog::OnOK();

	m_wType = ((CButton*)GetDlgItem(IDC_FOLDER))->GetCheck()==BST_CHECKED ? CPage::typeFolder : m_wType;
	m_wType = ((CButton*)GetDlgItem(IDC_MAP))->GetCheck()==BST_CHECKED ? CPage::typeMap : m_wType;
	m_wType = ((CButton*)GetDlgItem(IDC_HTML))->GetCheck()==BST_CHECKED ? CPage::typeHtml : m_wType;
}

void CPageDlg::OnBnClickedFolder()
{
	GetDlgItem(IDC_TARGET)->EnableWindow(FALSE);
	GetDlgItem(IDC_FINDFILE)->EnableWindow(FALSE);

	GetDlgItem(IDC_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_Y)->EnableWindow(FALSE);
}

void CPageDlg::OnBnClickedMap()
{
	GetDlgItem(IDC_TARGET)->EnableWindow(TRUE);
	GetDlgItem(IDC_FINDFILE)->EnableWindow(TRUE);

	GetDlgItem(IDC_X)->EnableWindow(TRUE);
	GetDlgItem(IDC_Y)->EnableWindow(TRUE);
}

void CPageDlg::OnBnClickedHtml()
{
	GetDlgItem(IDC_TARGET)->EnableWindow(TRUE);
	GetDlgItem(IDC_FINDFILE)->EnableWindow(TRUE);

	GetDlgItem(IDC_X)->EnableWindow(FALSE);
	GetDlgItem(IDC_Y)->EnableWindow(FALSE);
}
