// AtlasNewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AtlasNewDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CAtlasNewDlg dialog

IMPLEMENT_DYNAMIC(CAtlasNewDlg, CDialogEx)

CAtlasNewDlg::CAtlasNewDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CAtlasNewDlg::IDD, pParent)
{
	m_nRadio = -1;
}

CAtlasNewDlg::~CAtlasNewDlg()
{
}

void CAtlasNewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAtlasNewDlg)
	DDX_Control(pDX, IDC_PATHPICKER, m_PathPicker);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAtlasNewDlg, CDialog)
	ON_BN_CLICKED(IDC_RADIO1, &CAtlasNewDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CAtlasNewDlg::OnBnClickedRadio2)
END_MESSAGE_MAP()

BOOL CAtlasNewDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_PathPicker.Expand(m_PathPicker.GetRootItem(), TVE_EXPAND);
	m_PathPicker.EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	m_nRadio = 1;
	return TRUE;
}

void CAtlasNewDlg::OnBnClickedRadio1()
{
	m_nRadio = 1;
	m_PathPicker.EnableWindow(FALSE);
}

void CAtlasNewDlg::OnBnClickedRadio2()
{
	m_nRadio = 2;
	m_PathPicker.EnableWindow(TRUE);
}

void CAtlasNewDlg::OnOK()
{
	m_strPath = "";
	if(m_nRadio==2)
	{
		const HTREEITEM item = m_PathPicker.GetSelectedItem();
		if(item!=nullptr)
		{
			m_PathPicker.GetItemPath(m_strPath, item);
		}
	}

	CDialogEx::OnOK();
}
