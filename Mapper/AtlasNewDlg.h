#pragma once
#include "resource.h"

class CAtlasNewDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CAtlasNewDlg)

public:
	CAtlasNewDlg(CWnd* pParent = nullptr);

	~CAtlasNewDlg() override;

public:
	int m_nRadio;
	CString m_strPath;

	// Dialog Data
	//{{AFX_DATA(CAtlasNewDlg)
	enum
	{
		IDD = IDD_ATLASNEW
	};

	CMFCShellTreeCtrl m_PathPicker;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtlasNewDlg)
protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	void OnOK() override;
	//}}AFX_VIRTUAL

	// Implementation
protected:
	//{{AFX_MSG(CAtlasNewDlg)
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
