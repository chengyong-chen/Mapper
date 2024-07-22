// NameFieldDlg.cpp : implementation file
//

#include "stdafx.h"
#include "NameFieldDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNameFieldDlg dialog

CNameFieldDlg::CNameFieldDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CNameFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameFieldDlg)

	//}}AFX_DATA_INIT
}

CNameFieldDlg::CNameFieldDlg(CWnd* pParent, CString strTitle, CStringList& fieldlist, int defaultsel)
	: CDialog(CNameFieldDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNameFieldDlg)

	//}}AFX_DATA_INIT

	POSITION pos = fieldlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CString string = fieldlist.GetNext(pos);
		m_fieldlist.AddTail(string);
	}

	m_strTitle = strTitle;
	m_defaultsel = defaultsel;
}

void CNameFieldDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNameFieldDlg)
	DDX_Control(pDX, IDC_LIST, m_listBox);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNameFieldDlg, CDialog)
	//{{AFX_MSG_MAP(CNameFieldDlg)
	// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNameFieldDlg message handlers

BOOL CNameFieldDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	POSITION pos = m_fieldlist.GetHeadPosition();
	while(pos!=nullptr)
	{
		CString string = m_fieldlist.GetNext(pos);
		m_listBox.AddString(string);
	}
	if(m_defaultsel!=-1)
		m_listBox.SetCurSel(m_defaultsel);

	SetWindowText(m_strTitle);
	return TRUE; // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CNameFieldDlg::OnOK()
{
	CDialog::OnOK();

	m_defaultsel = m_listBox.GetCurSel();
}
