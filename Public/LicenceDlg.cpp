// LicenceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LicenceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CLicenceDlg dialog

IMPLEMENT_DYNAMIC(CLicenceDlg, CDialogEx)

CLicenceDlg::CLicenceDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(CLicenceDlg::IDD, pParent)
{
}

CLicenceDlg::~CLicenceDlg()
{
}

void CLicenceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLicenceDlg, CDialogEx)
END_MESSAGE_MAP()

// CLicenceDlg message handlers

BOOL CLicenceDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString str;
	str.LoadString(IDS_NOTAUTHORIZED);
	GetDlgItem(IDC_NOTAUTHORIZED)->SetWindowText(str);
	return TRUE;
}

void CLicenceDlg::Display()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CLicenceDlg dlg;
	dlg.DoModal();
}
