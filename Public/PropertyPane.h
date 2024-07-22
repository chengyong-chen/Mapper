#pragma once
/////////////////////////////////////////////////////////////////////////////
// CMyBar window

class __declspec(dllexport) CPropertyPane : public CDockablePane
{
public:
	CPropertyPane();

public:
	CMapStringToPtr m_mapForm;
	CComboBox m_ComboBox;

public:
	static CString GetTabLabel(char cTab);
	CWnd* GetTabWnd(char cTab) const;
	void ValidateTab(char cTab);
	void InvalidateTab(char cTab);
	void ActivateTab(char cTab);

public:
	CWnd* GetCurWindow();
	// Operations

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyPane)	
public:
	BOOL DestroyWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertyPane)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnComboBoxSelChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
