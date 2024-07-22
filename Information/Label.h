#pragma once

// Label.h : header file

/////////////////////////////////////////////////////////////////////////////
// CLabel window
enum FlashType
{
	None,
	Text,
	Background
};

class CLabel : public CStatic
{
	public:
	CLabel();
	CLabel& SetBkColor(COLORREF crBkgnd, BOOL bIsLink = FALSE);
	CLabel& SetTextColor(COLORREF crText, BOOL bIsLink = FALSE);
	CLabel& SetText(const CString& strText);
	CLabel& SetFontBold(BOOL bBold, BOOL bIsLink = FALSE);
	CLabel& SetFontName(const CString& strFont, BOOL bIsLink = FALSE);
	CLabel& SetFontUnderline(BOOL bSet, BOOL bIsLink = FALSE);
	CLabel& SetFontItalic(BOOL bSet, BOOL bIsLink = FALSE);
	CLabel& SetFontSize(int nSize, BOOL bIsLink = FALSE);
	CLabel& SetSunken(BOOL bSet);
	CLabel& SetBorder(BOOL bSet);
	CLabel& FlashText(BOOL bActivate);
	CLabel& FlashBackground(BOOL bActivate);
	CLabel& SetLink(BOOL bLink, LPCTSTR strLinkTarget);
	CLabel& SetLinkCursor(HCURSOR hCursor);
	BOOL HasLink() const
	{ return m_bLink; }

	// Attributes
	public:
	protected:
	void ReconstructFont(BOOL bIsLink = FALSE);
	COLORREF m_crText;
	HBRUSH m_hBrush;
	HBRUSH m_hwndBrush;
	LOGFONT m_lf;
	CFont m_font;
	CString m_strText;
	BOOL m_bState;
	BOOL m_bTimer;
	FlashType m_Type;
	HCURSOR m_hCursor;

	HBRUSH m_hBrushLink;
	COLORREF m_crTextLink;
	LOGFONT m_lfLink;
	CFont m_fontLink;
	BOOL m_bLink;
	CString m_strLinkTarget;
	BOOL m_bMouseOver;

	// Operations
	public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLabel)
	//}}AFX_VIRTUAL

	// Implementation
	public:
	~CLabel() override;

	// Generated message map functions
	protected:
	//{{AFX_MSG(CLabel)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
