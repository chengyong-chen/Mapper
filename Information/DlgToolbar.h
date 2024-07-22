#pragma once



class __declspec(dllexport) CDlgToolbar : public CToolBarCtrl
{
public:
	TBBUTTON    *m_pTBButtons;

// Construction
public:
	CDlgToolbar();

// Attributes
public:

// Operations
public:


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgToolbar)
	//}}AFX_VIRTUAL


// Implementation
public:
	virtual ~CDlgToolbar();

protected:
	CString NeedText(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDlgToolbar)
	afx_msg void OnNeedTextW(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult);
	afx_msg void OnNeedTextA(UINT nId, NMHDR*pNotifyStruct, LRESULT*lResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
