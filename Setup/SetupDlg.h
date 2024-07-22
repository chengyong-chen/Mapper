// SetupDlg.h : header file






#pragma once


/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog
#include "Credit.h"

class CSetupDlg : public CDialog
{
// Construction
public:
	CSetupDlg(CWnd* pParent = nullptr);	// standard constructor
	virtual ~CSetupDlg();

// Dialog Data
	//{{AFX_DATA(CSetupDlg)
	enum { IDD = IDD_SETUP };
	CButton	m_readme;
	CButton	m_exit;
	CButton	m_uninstall;
	CButton	m_run;
	CButton	m_setup;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX) override;	// DDX/DDV support
	//}}AFX_VIRTUAL

	COLORREF m_colButtonNormalColor;
	COLORREF m_colButtonSelectColor;
	COLORREF m_colButtonDisableColor;

// Implementation
protected:
	HICON m_hIcon;
	BOOL  LoadInitialFile();
	void  StateChange();
	void  ReportError();

	BOOL  CanInstall();

	BOOL  CanUninstall();
	BOOL  Uninstall();

	// 安装信息段
	CRect   m_rSetup;

	CString m_rSetupName;
	CString m_rSetupDir;
	CString m_rClassName;
	CString m_rWindowTitle;
	// 运行信息段
	CRect   m_rRun;
	CString m_rRunName;
	CString m_rRunKey;
	CString m_rLocalPath;
	// 卸载信息段
	CRect   m_rUninstall;
	CString m_rUninstallName;
	CString m_rUninstallKey;
	// 退出信息段
	CRect   m_rQuit;
	// 说明信息段
	CRect   m_rReadme;
	CString m_rReadmeName;
	CString m_rReadmeFile;
	// 字幕信息段
	CRect   m_rCredit;
	CString m_rProductName;
	CString m_rProductBrand;
	CString m_rCompanyName;
		
	CCredit m_credit;
	HANDLE  m_hDIB;
	
// Generated message map functions
	//{{AFX_MSG(CSetupDlg)
	virtual BOOL OnInitDialog() override;
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSetupSetup();
	afx_msg void OnSetupRun();
	afx_msg void OnSetupExit();
	afx_msg void OnSetupUninstall();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSetupReadme();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	friend class CCredit;
};





