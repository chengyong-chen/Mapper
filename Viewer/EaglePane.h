#pragma once

#include <gdiplus.h>



// CEaglePane

class CEaglePane : public CDockablePane
{
	DECLARE_DYNAMIC(CEaglePane)

public:
	CEaglePane();

public:
	virtual ~CEaglePane();

public:
	void  OpenBmp(CView* pView,CString strBmp,CSize docSize);
	void  SetRect(CView* pView,CRect vRect);

public:
	Gdiplus::Image* m_Image;
	
	CRect   m_vRect;
	double  m_fHScale;
	double  m_fVScale;

	CView*  m_pView;
	CString m_strBmp;
	CSize   m_docSize;

public:		
	CPoint m_ptDown;
	bool   m_bDown;

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAtlasPane)
	//}}AFX_VIRTUAL

protected:
    // Generated message map functions
	//{{AFX_MSG(CAtlasPane)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


