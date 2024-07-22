#pragma once
#include "WhereTextPage.h"
#include "WherePickPage.h"

/////////////////////////////////////////////////////////////////////////////
// CWhereDlg dialog

class __declspec(dllexport) CWhereDlg : public CDialog
{
	public:
	CWhereDlg(CWnd* pParent = nullptr, CDatabase* pDatabase = nullptr, CString strTable = _T(""), CString strWhere = _T(""));

	// Dialog Data
	//{{AFX_DATA(CWhereDlg)
	enum
	{
		IDD = IDD_WHERE
	};

	CComboBox m_TableCombo;
	//}}AFX_DATA

	public:
	CPropertySheet m_WhereSheet;
	CWhereTextPage m_textWherePage;
	CWherePickPage m_pickWherePage;

	CDatabase* m_pDatabase;
	CString m_strTable;
	CString m_strWhere;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhereDlg)
	protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
	protected:

	// Generated message map functions
	//{{AFX_MSG(CWhereDlg)
	void OnOK() override;
	BOOL OnInitDialog() override;
	afx_msg void OnSelchangeTable();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
