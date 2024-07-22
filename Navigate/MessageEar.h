#pragma once

class __declspec(dllexport) CMessageEar : public CEdit
{
	public:
	CMessageEar();

	// Attributes
	public:
	CString m_strContent;
	int m_nLine; // 信息滚动框显示信息的行数,不超过1000行

	// Operations
	public:
	void AddMessage(CString& strMsg);

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMessageEar)
	//}}AFX_VIRTUAL

	// Implementation
	public:
	~CMessageEar() override;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CMessageEar)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
