#include "stdafx.h"
#include "DeckDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeckDlg dialog
CDeckDlg::CDeckDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CDeckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeckDlg)
	m_strFile = _T("");
	m_strHtml = _T("");
	m_strLegend = _T("");
	m_bNavigatable = FALSE;
	m_bQueryble = TRUE;
	//}}AFX_DATA_INIT
}

CDeckDlg::CDeckDlg(CWnd* pParent, CString strFile, CString strHtml, CString strLegend, BOOL bNavigatable, BOOL bQueryble)
	: CDialog(CDeckDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeckDlg)
	m_strFile = _T("");
	//}}AFX_DATA_INIT

	m_strFile = strFile;
	m_strHtml = strHtml;
	m_strLegend = strLegend;
	m_bNavigatable = bNavigatable;
	m_bQueryble = bQueryble;
}

void CDeckDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeckDlg)
	DDX_Text(pDX, IDC_FILE, m_strFile);
	DDX_Text(pDX, IDC_HTMLFILE, m_strHtml);
	DDX_Text(pDX, IDC_LEGEND, m_strLegend);
	DDX_Check(pDX, IDC_NAVIGATE, m_bNavigatable);
	DDX_Check(pDX, IDC_QUERY, m_bQueryble);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDeckDlg, CDialog)
	//{{AFX_MSG_MAP(CDeckDlg)
	// NOTE: the ClassWizard will add message map macros here	
	ON_BN_CLICKED(IDC_FIND1, OnFind1)
	ON_BN_CLICKED(IDC_FIND2, OnFind2)
	ON_BN_CLICKED(IDC_FIND3, OnFind3)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeckDlg message handlers
void CDeckDlg::OnFind1()
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
		m_strFile = dlg.GetPathName();
		((CStatic*)GetDlgItem(IDC_FILE))->SetWindowText(m_strFile);
	}
	SetCurrentDirectory(CurrentDir);
}

void CDeckDlg::OnFind2()
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
		m_strHtml = dlg.GetPathName();
		((CStatic*)GetDlgItem(IDC_HTMLFILE))->SetWindowText(m_strHtml);
	}
	SetCurrentDirectory(CurrentDir);
}

void CDeckDlg::OnFind3()
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
		m_strLegend = dlg.GetPathName();
		((CStatic*)GetDlgItem(IDC_LEGEND))->SetWindowText(m_strLegend);
	}
	SetCurrentDirectory(CurrentDir);
}
