#pragma once

// HyperlinkStatic.h : header file

/////////////////////////////////////////////////////////////////////////////
// CHyperlinkStatic window

class CHyperlinkStatic : public CStatic
{
public:
	CHyperlinkStatic();

	// Attributes
public:
	CString m_strURL;
	// Operations
public:

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHyperlinkStatic)
protected:
	void PreSubclassWindow() override;
	//}}AFX_VIRTUAL

	// Implementation
public:
	~CHyperlinkStatic() override;

	// Generated message map functions
protected:
	//{{AFX_MSG(CHyperlinkStatic)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	bool m_bMouseIn;
};

/////////////////////////////////////////////////////////////////////////////
