



#pragma once

// LoginDlg.h : header file


/////////////////////////////////////////////////////////////////////////////
// CLoginDlg dialog

class CLoginDlg : public CDialog
{
// Construction
public:
	CLoginDlg(CWnd* pParent = nullptr);  

public:
	BOOL m_bChangePass;   // 修改密码标志

// Dialog Data
	//{{AFX_DATA(CLoginDlg)
	enum { IDD = IDD_LOGIN };
	CString	m_strUser;
	CString	m_strPass;
	CString	m_strPass2;
	//}}AFX_DATA

public:
	int m_iCounter;   // 用户输入次数计数，3次输入机会后，退出应用程序

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginDlg)
	virtual void OnOK() override;
	virtual void OnCancel() override;
	virtual BOOL OnInitDialog() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





