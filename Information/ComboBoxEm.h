#pragma once

// ComboBoxEm.h : header file

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEm window

class __declspec(dllexport) CComboBoxEm : public CComboBox
{
public:
	CComboBoxEm();

	// Attributes
public:

	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CComboBoxEm)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CComboBoxEm() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CComboBoxEm)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
