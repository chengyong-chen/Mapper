


#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// GfxBitmapButton.h : header file


/////////////////////////////////////////////////////////////////////////////
// CGfxBitmapButton window

class CGfxBitmapButton : public CButton
{
public:

	CGfxBitmapButton();

// Attributes
public:
		
// Operations
public:
	void SetBkColor(COLORREF colBkColor);
	void SetTextColor(COLORREF colTextColor);
	void SetTextColor(COLORREF colTextColor, COLORREF m_colFocusTextColor, COLORREF m_colDisableTextColor);
	void SetFrameColor(COLORREF colFrameColor);
	BOOL SetImageList(CImageList* imagelist, int nType);
	void SetFlat(BOOL flag = TRUE) { m_bFlat = flag;}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGfxBitmapButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGfxBitmapButton();

	// Generated message map functions
protected:
	CImageList	m_cImageBk;
	CImageList	m_cImageMark;

	COLORREF    m_colBkColor;
	COLORREF    m_colTextColor;
	COLORREF    m_colDisableTextColor;
	COLORREF    m_colFocusTextColor;
	COLORREF    m_colFrameColor;

	BOOL        m_bMouseTracking;
	BOOL        m_bFlat;
	
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS) override;
	//{{AFX_MSG(CGfxBitmapButton)
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////


// Microsoft Developer Studio will insert additional declarations immediately before the previous line.


