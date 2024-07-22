#pragma once
#include "resource.h"

class __declspec(dllexport) CColumnPicker : public CDialog
{
public:
	CColumnPicker(CWnd* pParent = nullptr, CDatabase* pDatabase = nullptr, CString strTable = _T(""));

	// Dialog Data
	//{{AFX_DATA(CColumnPicker)
	enum
	{
		IDD = IDD_COLUMNPICKER
	};
	CComboBox m_FieldCombo;
	//}}AFX_DATA

	CDatabase* m_pDatabase;
	CString m_strTable;
	CString m_strField;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColumnPicker)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColumnPicker)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
