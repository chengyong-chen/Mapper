#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "../Public/Databaseconnection.h"

class __declspec(dllexport) CDatabaseDlg : public CDialog
{
public:
	CDatabaseDlg();

	~CDatabaseDlg() override;

	DBMSTYPE dbms;
	CString strServer;
	CString strDatabase;
	CString strUID;
	CString strPWD;
	BOOL bFileSource;

	// Dialog Data
	//{{AFX_DATA(CDatabaseDlg)
	enum
	{
		IDD = IDD_DATABASE
	};

	CComboBox m_comboDBMS;
	//}}AFX_DATA

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatabaseDlg)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDatabaseDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	afx_msg void OnSetup();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
