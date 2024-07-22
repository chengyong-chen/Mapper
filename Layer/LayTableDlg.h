#pragma once

#include "resource.h"

class AFX_EXT_CLASS CLayTableDlg : public CDialog
{
public:
	CLayTableDlg(CWnd* pParent = nullptr);
	CLayTableDlg(CWnd* pParent, CString tableData, CString tableAttribute, CString strWhere, CString strTag);

	// Dialog Data
	//{{AFX_DATA(CLayTableDlg)
	enum
	{
		IDD = IDD_LAYTABLE
	};

	CString m_geoTable;
	CString m_attTable;
	CString m_strWhere;
	CString m_strTag;
	//}}AFX_DATA

	CString m_strTitle;
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLayTableDlg)
protected:
	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLayTableDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
