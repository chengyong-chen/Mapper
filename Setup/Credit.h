



#pragma once

// Credit.h : header file


/////////////////////////////////////////////////////////////////////////////
// CCredit window

class CCredit : public CWnd
{
// Construction
public:
	CCredit();

// Attributes
public:
	CStringArray m_rCreditArray;
	COLORREF m_colCreditHeadColor;
	COLORREF m_colCreditSecondColor;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCredit)
	protected:
	virtual void PreSubclassWindow() override;
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCredit();

	// Generated message map functions
protected:
	HBITMAP m_hBitmap;
	void    CreateContents();
	long    m_nOffset;
	long    m_count;
	long    m_nAllCount;
	
	//{{AFX_MSG(CCredit)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////





