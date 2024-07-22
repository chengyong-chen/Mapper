#include "stdafx.h"
#include "Link.h"

#include "LinkFile.h"
#include "LinkFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CLinkFileDlg::CLinkFileDlg(CWnd* pParent /*=nullptr*/, CLinkFile* plink)
	: CDialog(CLinkFileDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLinkFileDlg)
	m_strFile = _T("");
	//}}AFX_DATA_INIT
	d_pLink = plink;

	m_strFile = plink->m_strTarget;
}

void CLinkFileDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLinkFileDlg)
	DDX_Text(pDX, IDC_FILE, m_strFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CLinkFileDlg, CDialog)
	//{{AFX_MSG_MAP(CLinkFileDlg)
	ON_BN_CLICKED(IDC_SEARCH, OnSearch)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLinkFileDlg message handlers

BOOL CLinkFileDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO: Add extra initialization here

	SetWindowPos(&wndTop, 10, 63, 0, 0, SWP_NOSIZE|SWP_SHOWWINDOW);
	return TRUE;
}

void CLinkFileDlg::OnOK()
{
	CDialog::OnOK();

	d_pLink->m_strTarget = m_strFile;
}

void CLinkFileDlg::OnSearch()
{
	TCHAR CurrentDir[256];
	GetCurrentDirectory(255, CurrentDir);

	CFileDialog dlg(TRUE, nullptr, nullptr, OFN_HIDEREADONLY, _T("All Files (*.*)|*.*|"), this);
	if(dlg.DoModal()==IDOK)
	{
		m_strFile = dlg.GetPathName();
		((CStatic*)GetDlgItem(IDC_FILE))->SetWindowText(m_strFile);

		CString strPath = dlg.GetPathName();
		strPath = strPath.Left(strPath.ReverseFind(_T('\\'))+1);
		AfxGetApp()->WriteProfileString(_T(""), _T("Link File Path"), strPath);
	}

	SetCurrentDirectory(CurrentDir);
}
