// RegtoolDlg.h : header file






#pragma once


/////////////////////////////////////////////////////////////////////////////
// CRegtoolDlg dialog

class CRegtoolDlg : public CDialog
{
// Construction
public:
	CRegtoolDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRegtoolDlg)
	enum { IDD = IDD_REGTOOL_DIALOG };
	UINT	m_Key1;
	UINT	m_Key2;
	CString	m_RegCode;
	CString	m_strFolder;
	//}}AFX_DATA

	void Recursion(CString strPath);

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRegtoolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRegtoolDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGet();
	afx_msg void OnWrite();
	afx_msg void OnGetFolder();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};





