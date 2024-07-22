



#pragma once

// MapStatic.h : header file


/////////////////////////////////////////////////////////////////////////////
// CMapStatic window

class CMapStatic : public CStatic
{
// Construction
public:
	CMapStatic();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMapStatic)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMapStatic();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMapStatic)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////





