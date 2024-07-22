#pragma once

// StatusCombo.h : header file

/////////////////////////////////////////////////////////////////////////////
// CStatusCombo window

class __declspec(dllexport) CStatusCombo : public CComboBox
{
public:
	CStatusCombo();
	// Attributes
public:

	// Operations
public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusCombo)
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CStatusCombo() override;
	// maxlen limits the number of items that can be displayed in the combo box
	// before a scrollbar is needed. It will adjust the dropdown to pop upwards
	// or downwards to hold the maximum possible. If this value is set to
	// 0, the combo box is limited by the screen size
	// If it is set to -1, there is no limit to the combo box.
	int maxlen;
	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusCombo)
	afx_msg void OnDropdown();
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
