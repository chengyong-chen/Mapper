#pragma once
#include "resource.h"

class __declspec(dllexport) CFieldsPicker : public CDialog
{
public:
	CFieldsPicker(CWnd* pParent = nullptr, CDatabase* pDatabase = nullptr, CString strTable = _T(""), CString strField = _T(""), float minimumScale = 80000000, float maximumScale = 1000);

	// Dialog Data
	//{{AFX_DATA(CFieldsPicker)
	enum
	{
		IDD = IDD_FIELDPICKER
	};

	CComboBox m_FieldCombo;
	float m_minimumScale;
	float m_maximumScale;
	//}}AFX_DATA

	CDatabase* m_pDatabase;
	CString m_strTable;
	CString m_strField;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldsPicker)
protected:
	void DoDataExchange(CDataExchange* pDX) override; // DDX/DDV support
	//}}AFX_VIRTUAL

	// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFieldsPicker)
	BOOL OnInitDialog() override;
	void OnOK() override;
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
