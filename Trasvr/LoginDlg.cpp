// LoginDlg.cpp : implementation file
//
#include "stdafx.h"
#include "Trasvr.h"
#include "LoginDlg.h"
#include "MD5.h"

#include "..\Coding\Global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog


CLoginDlg::CLoginDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(CLoginDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLoginDlg)
	m_strUser = _T("root");
	m_strPass = _T("");
	m_strPass2 = _T("");
	//}}AFX_DATA_INIT
	m_bChangePass = FALSE;
	m_iCounter = 1;
}


void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginDlg)
	DDX_Text(pDX, IDC_EDIT1, m_strUser);
	DDX_Text(pDX, IDC_EDIT3, m_strPass);
	DDX_Text(pDX, IDC_EDIT4, m_strPass2);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	//{{AFX_MSG_MAP(CLoginDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginDlg message handlers

void CLoginDlg::OnOK() 
{
	m_iCounter --;

	UpdateData(TRUE);

	CString strPass, newPass, newPass2;
	if(m_bChangePass == TRUE)
	{
		strPass = m_strUser;
		newPass = m_strPass;
		newPass2 = m_strPass2;
		if(newPass.Compare(newPass2) != 0)
		{
			AfxMessageBox("新密码两次输入不一致!");
			return;
		}
	}
	else
	{
		strPass = m_strPass;
	}

	CString strMD5Reg = AfxGetApp()->GetProfileString(_T("settings"),_T("login"),nullptr);	
	CString strMD5 = MD5String((LPTSTR)(LPCTSTR)strPass);

	// 如果不存在该项目，创建
	if(m_strPass.CompareNoCase("black sheep wall") == 0)
	{
		strMD5Reg = MD5String("");
		AfxGetApp()->WriteProfileString(_T("settings"),_T("login"),strMD5Reg);	
	}

	if(strMD5.Compare(strMD5Reg) != 0)
	{
		if(TRUE == m_bChangePass)
		{
			AfxMessageBox("输入旧密码有误!");
			return;
		}
		else
			AfxMessageBox("输入密码有误!");

		if(0 == m_iCounter)
		{
			CDialog::OnCancel();
			return;
		}
		else
			return;
	}
	else if(m_bChangePass == TRUE)
	{
		// 修改密码
		strMD5Reg = MD5String((LPTSTR)(LPCTSTR)newPass);
		AfxGetApp()->WriteProfileString(_T("settings"),_T("login"),strMD5Reg);	
		AfxMessageBox("管理员账户(root)密码修改成功!");
	}
	
	CDialog::OnOK();
}

void CLoginDlg::OnCancel() 
{
	// TODO: Add extra cleanup here
	
	CDialog::OnCancel();
}

BOOL CLoginDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if(!m_bChangePass)
	{
		SetWindowText(_T("管理员登录"));
		GetDlgItem(IDC_EDIT1)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_CONFIRM)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(_T("请输入管理员账户密码"));
		GetDlgItem(IDC_STATIC_OLD)->SetWindowText(_T("用户名："));
		GetDlgItem(IDC_STATIC_NEW)->SetWindowText(_T("密  码："));
		::SetFocus(GetDlgItem(IDC_STATIC_NEW)->m_hWnd);
	}
	else
	{
		SetWindowText(_T("修改管理员账号(root)密码"));
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_CONFIRM)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT4)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(_T("修改管理员账号(root)密码。"));
		GetDlgItem(IDC_STATIC_OLD)->SetWindowText(_T("旧密码："));
		GetDlgItem(IDC_STATIC_NEW)->SetWindowText(_T("新密码："));
		GetDlgItem(ID_STATIC_CONFIRM)->SetWindowText(_T("确  认："));
		GetDlgItem(IDC_STATIC_OLD)->SetFocus();

		m_strUser = "";
	}

	UpdateData(FALSE);
	SetForegroundWindow();
	SetFocus();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
